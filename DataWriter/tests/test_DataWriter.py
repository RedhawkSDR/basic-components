#!/usr/bin/env python
#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components.
# 
# REDHAWK Basic Components is free software: you can redistribute it and/or modify it under the terms of 
# the GNU Lesser General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
# PURPOSE.  See the GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License along with this 
# program.  If not, see http://www.gnu.org/licenses/.
#
import unittest
import ossie.utils.testing
import os
from omniORB import any
import test_utils
from bulkio.bulkioInterfaces import BULKIO, BULKIO__POA
from ossie.properties import props_from_dict, props_to_dict
import time
import struct
import ConfigParser

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in DataWriter"""

    def testScaBasicBehavior(self):
        #######################################################################
        # Launch the component with the default execparams
        execparams = self.getPropertySet(kinds=("execparam",), modes=("readwrite", "writeonly"), includeNil=False)
        execparams = dict([(x.id, any.from_any(x.value)) for x in execparams])
        self.launch(execparams)
        
        #######################################################################
        # Verify the basic state of the component
        self.assertNotEqual(self.comp, None)
        self.assertEqual(self.comp.ref._non_existent(), False)
        self.assertEqual(self.comp.ref._is_a("IDL:CF/Resource:1.0"), True)
        #self.assertEqual(self.spd.get_id(), self.comp.ref._get_identifier())
        
        #######################################################################
        # Simulate regular component startup
        # Verify that initialize nor configure throw errors
        self.comp.initialize()
        configureProps = self.getPropertySet(kinds=("configure",), modes=("readwrite", "writeonly"), includeNil=False)
        self.comp.configure(configureProps)
        
        #######################################################################
        # Validate that query returns all expected parameters
        # Query of '[]' should return the following set of properties
        expectedProps = []
        expectedProps.extend(self.getPropertySet(kinds=("configure", "execparam"), modes=("readwrite", "readonly"), includeNil=True))
        expectedProps.extend(self.getPropertySet(kinds=("allocate",), action="external", includeNil=True))
        props = self.comp.query([])
        props = dict((x.id, any.from_any(x.value)) for x in props)
        # Query may return more than expected, but not less
        for expectedProp in expectedProps:
            self.assertEquals(props.has_key(expectedProp.id), True)
        
        #######################################################################
        # Verify that all expected ports are available
        for port in self.scd.get_componentfeatures().get_ports().get_uses():
            port_obj = self.comp.getPort(str(port.get_usesname()))
            self.assertNotEqual(port_obj, None)
            self.assertEqual(port_obj._non_existent(), False)
            self.assertEqual(port_obj._is_a("IDL:CF/Port:1.0"),  True)
            
        for port in self.scd.get_componentfeatures().get_ports().get_provides():
            port_obj = self.comp.getPort(str(port.get_providesname()))
            self.assertNotEqual(port_obj, None)
            self.assertEqual(port_obj._non_existent(), False)
            self.assertEqual(port_obj._is_a(port.get_repid()),  True)
            
        #######################################################################
        # Make sure start and stop can be called without throwing exceptions
        self.comp.start()
        self.comp.stop()
        
        #######################################################################
        # Simulate regular component shutdown
        # self.comp.releaseObject() # taken care of by tearDown()
        
    def setUp(self):
        """Set up the unit test - this is run before every method that starts with test        """
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        self.src = test_utils.MyDataSource()
        #start all components
        self.launch()
        self.comp.start()
        self.src.start()
        #do the connections
        self.src.connect(self.comp,'dataFloat')

        self.test_data_len = 5
        self.offset = 0
        self.stream_id1 = "stream1"
        self.t_delta = 0.005
        self.sr = 1./self.t_delta
        
        self.t_data_wait = 5 # Hack to wait for DataWriter to finish writing to disk before we verify contents
        
    def tearDown(self):
        """Finish the unit test - this is run after every method that starts with test        """
        self.comp.stop()
        #######################################################################
        # Simulate regular component shutdown
        self.comp.releaseObject()
        self.src.stop()
        ossie.utils.testing.ScaComponentTestCase.tearDown(self)
        
    def test_single_file(self):
        print "\n... Staring single file write test"
        config_dict = self._generate_config()
        data_filename = config_dict['filename']
        sri_filename = data_filename + ".sri"

        if os.path.exists(data_filename):
            os.remove(data_filename)
            while os.path.exists(data_filename):
                time.sleep(self.t_delta)

        self.comp.configure(props_from_dict(config_dict))
        self._generate_keywords()
        
        self._send_data()
        self._sleepTillDone(data_filename)
        self._validate_data(data_filename, self.expected_data, config_dict["endian"])
        self._validate_metadata(sri_filename, self.t_delta, self.stream_id1, self.keywords_dict, self.first_pkt_time, self.last_pkt_time)
    
    def test_multiple_existing(self):
        print "\n... Staring multiple existing data files test"
        config_dict = self._generate_config()
        data_filename = config_dict['filename']

        if os.path.exists(data_filename):
            os.system("rm %s" % (data_filename+"*"))
            while os.path.exists(data_filename):
                time.sleep(self.t_delta)
                
        self._generate_keywords()
        self.comp.configure(props_from_dict(config_dict))
        
        data_filename = config_dict['filename']
        base_filename = data_filename
        
        for i in range(10):
            filename = data_filename
            if i == 0:
                # On first pass, file is already created/ opened by DataWriter
                data_filename = filename
            else:
                x = 0
                while True:
                    if not os.path.exists(filename):
                        # File does not exist, good to go
                        data_filename = filename
                        break
                    x += 1
                    filename = base_filename + ".%d" % x
            
            sri_filename = data_filename + ".sri"
            self.keywords_dict["pass"] = i
            
            self._send_data()
            self._sleepTillDone(data_filename) # Hack to wait for DataWriter to finish writing to disk before we verify contents
            self._validate_data(data_filename, self.expected_data, config_dict["endian"])
            self._validate_metadata(sri_filename, self.t_delta, self.stream_id1, self.keywords_dict, self.first_pkt_time, self.last_pkt_time)

    def test_overwrite(self):
        print "\n... Staring overwrite test"
        config_dict = self._generate_config()
        data_filename = config_dict['filename']

        if os.path.exists(data_filename):
            os.system("rm %s" % (data_filename+"*"))
            while os.path.exists(data_filename):
                time.sleep(self.t_delta)
        
        config_dict["overwrite"] = True
        self._generate_keywords()
        self.comp.configure(props_from_dict(config_dict))
        
        data_filename = config_dict['filename']
        sri_filename = data_filename + ".sri"
        
        for i in range(5):
            self.keywords_dict["pass"] = i
            self._send_data()
            self._sleepTillDone(data_filename)
            self._validate_data(data_filename, self.expected_data, config_dict["endian"])
            self._validate_metadata(sri_filename, self.t_delta, self.stream_id1, self.keywords_dict, self.first_pkt_time, self.last_pkt_time)

    def test_write_toggle(self):
        print "\n... Staring write toggle test"
        config_dict = self._generate_config()
        data_filename = config_dict['filename']

        if os.path.exists(data_filename):
            os.system("rm %s" % (data_filename+"*"))
            while os.path.exists(data_filename):
                time.sleep(self.t_delta)
                
        self._generate_keywords()
        config_dict["write"] = False
        self.comp.configure(props_from_dict({"write": False})) # Ensure write property is configured before filename (otherwise, file may exist before write is disabled)
        self.comp.configure(props_from_dict(config_dict))
        
        data_filename = config_dict['filename']
        base_filename = data_filename
        
        for i in range(10):
            filename = data_filename
            if i == 0:
                # On first pass, file is already created/ opened by DataWriter
                data_filename = filename
            else:
                x = 0
                while True:
                    if not os.path.exists(filename):
                        # File does not exist, good to go
                        data_filename = filename
                        break
                    x += 1
                    filename = base_filename + ".%d" % x
            
            sri_filename = data_filename + ".sri"
            self.keywords_dict["pass"] = i
            
            if i == 5:
                self.comp.configure(props_from_dict({"write": True}))
                while self.comp.write!=True:
                    time.sleep(self.t_delta)
            self._send_data()
            self._sleepTillDone(data_filename) 
                        
            if i >= 5:
                # expecting output
                self._validate_data(data_filename, self.expected_data, config_dict["endian"])
                self._validate_metadata(sri_filename, self.t_delta, self.stream_id1, self.keywords_dict, self.first_pkt_time, self.last_pkt_time)
            else:
                # Verify that data was not written
                self.assertFalse(os.path.exists(data_filename))
                self.assertFalse(os.path.exists(sri_filename))
    
    def test_same_stream_snaps(self):
        print "\n... Staring multiple snapshots from one stream test"
        config_dict = self._generate_config()
        data_filename = config_dict['filename']

        if os.path.exists(data_filename):
            os.system("rm %s" % (data_filename+"*"))
            while os.path.exists(data_filename):
                time.sleep(self.t_delta)
                
        self._generate_keywords()
        self.comp.configure(props_from_dict(config_dict))
        
        data_filename = config_dict['filename']
        base_filename = data_filename
        
        count = 0
        expected_data = []
        results = []
        for i in xrange(6):
            # Disable file writing, this should cause the current file to be written out
            # and a new file to be created the next time write in enabled
            enabled_write = not ((i+1) % 2 == 0)

            
            self.comp.configure(props_from_dict({"write": enabled_write}))
            while props_to_dict(self.comp.query([]))['write']!=enabled_write:
                time.sleep(self.t_delta)
            
            first_pkt_time = None
            for j in xrange(3):
                self._send_data(last_pkt_eos=False) # send data but don't send eos (not stopping streaming just yet)
                first_pkt_time = first_pkt_time or self.first_pkt_time
                last_pkt_time = self.last_pkt_time
                if enabled_write:
                    expected_data.extend(self.expected_data)
                    self._sleepTillDone(data_filename, expected_data=expected_data)
                else:
                    time.sleep(self.t_data_wait)   
            
            if not enabled_write:
                # File should have been closed at the start of this pass
                sri_filename = data_filename + ".sri"
                self._validate_data(data_filename, expected_data, config_dict["endian"])
                self._validate_metadata(sri_filename, self.t_delta, self.stream_id1, self.keywords_dict, first_pkt_time, last_pkt_time)
                # reset for the next file
                expected_data = []
                count += 1
                data_filename = base_filename + ".%d" % count
    
    def test_same_stream_snaps_overwrite(self):
        print "\n... Staring multiple snapshots from one stream test, overwrite enabled"
        config_dict = self._generate_config()
        data_filename = config_dict['filename']
        config_dict['overwrite'] = True

        if os.path.exists(data_filename):
            os.system("rm %s" % (data_filename+"*"))
            while os.path.exists(data_filename):
                time.sleep(self.t_delta)
                
        self._generate_keywords()
        self.comp.configure(props_from_dict(config_dict))
        
        data_filename = config_dict['filename']
        base_filename = data_filename
        
        expected_data = []
        results = []
        for i in xrange(6):
            # Disable file writing, this should cause the current file to be written out
            # and a new file to be created the next time write in enabled
            enabled_write = not ((i+1) % 2 == 0)
           
            self.comp.configure(props_from_dict({"write": enabled_write}))
            while props_to_dict(self.comp.query([]))['write'] !=enabled_write:
                time.sleep(self.t_delta)
            
            first_pkt_time = None
            for j in xrange(3):
                self._send_data(last_pkt_eos=False) # send data but don't send eos (not stopping streaming just yet)
                first_pkt_time = first_pkt_time or self.first_pkt_time
                last_pkt_time = self.last_pkt_time
                if enabled_write:
                    expected_data.extend(self.expected_data)
                    self._sleepTillDone(data_filename, expected_data=expected_data)
                else:
                    time.sleep(self.t_data_wait)   
            
            if not enabled_write:
                # File should have been closed at the start of this pass
                sri_filename = data_filename + ".sri"
                self._validate_data(data_filename, expected_data, config_dict["endian"])
                self._validate_metadata(sri_filename, self.t_delta, self.stream_id1, self.keywords_dict, first_pkt_time, last_pkt_time)
                # reset for the next file
                expected_data = []
    
    def _sleepTillDone(self, filename, timeout=None, expected_data=None):
        if timeout==None:
            timeout=self.t_data_wait
        if expected_data==None:
           expected_data = self.expected_data
        done=False
        numSleeps  = self.t_data_wait / self.t_delta
        count=0
        fileSize=None
        while not done and count !=numSleeps:
            try:
                fileSize=os.path.getsize(filename)
                done = fileSize == len(expected_data)*4 #float data
            except OSError:
                done = False                
            
            time.sleep(self.t_delta)
            count +=1
        return done
             
    def _generate_config(self):
        config_params = {}
        config_params["filename"] = "/tmp/data_writer_test.tmp"
        config_params["write"] = True
        config_params["overwrite"] = False
        config_params["endian"] = "little"
        return config_params
    
    def _generate_keywords(self):
        self.keywords_dict = {}
        self.keywords_dict["test_key1"] = "value1"
        self.keywords_dict["test_key2"] = "value2"
        self.keywords_dict["test_key3"] = 3.14159
    
    def _generate_test_data(self, data_len=None):
        self.offset += 10
        if data_len is None:
            data_len = self.test_data_len
        data1 = [float(x+self.offset) for x in xrange(data_len)]
        return data1
    
    def _send_data(self, last_pkt_eos=True):
        self.expected_data = []
        self.pkt_time=0.
        EOS = False
        self.first_pkt_time = self.pkt_time + self.t_delta
        for x in xrange(100):
            time.sleep(self.t_delta)
            self.pkt_time+=self.t_delta
            data1 = self._generate_test_data()
            self.expected_data.extend(data1)
            self.src.push(data1, EOS=EOS, streamID=self.stream_id1, SRIKeywords=self.keywords_dict, time=self.pkt_time, sampleRate = self.sr)
        
        EOS = last_pkt_eos
        self.pkt_time+=self.t_delta
        data1 = self._generate_test_data()
        self.expected_data.extend(data1)
        self.src.push(data1, EOS=EOS, streamID=self.stream_id1, SRIKeywords=self.keywords_dict, time=self.pkt_time, sampleRate = self.sr)
        
        self.last_pkt_time = self.pkt_time
        
    def _validate_metadata(self, filename, xdelta, stream_id, keyword_dict, first_pkt_time, last_pkt_time):
        numSleeps  = self.t_data_wait / self.t_delta
        count=0
        ret = False
        while not ret and count!=numSleeps:
            try:
                ret = self._really_validate_metadata(filename, xdelta, stream_id, keyword_dict, first_pkt_time, last_pkt_time)
            except AssertionError:
                pass
            time.sleep(self.t_delta)
            count+=1              
    
    def _really_validate_metadata(self, filename, xdelta, stream_id, keyword_dict, first_pkt_time, last_pkt_time):
        metadata = ConfigParser.ConfigParser()
        metadata.read(filename)
        
        sri_section = "SRI"
        keywords_section = "SRI Keywords"
        pkt_1_section = "First Packet Time"
        pkt_n_section = "Last Packet Time"
        
        # Verify sri data (for scalar 1 dimensional data)
        self.assertTrue(metadata.has_section(sri_section))
        self.assertEqual(metadata.getint(sri_section, "hversion"), 1)
        self.assertEqual(metadata.getfloat(sri_section, "xstart"), 0.0)
        self.assertEqual(metadata.getfloat(sri_section, "xdelta"), xdelta)
        self.assertEqual(metadata.getint(sri_section, "xunits"), BULKIO.UNITS_NONE)
        self.assertEqual(metadata.getint(sri_section, "subsize"), 0)
        self.assertEqual(metadata.getfloat(sri_section, "ystart"), 0.0)
        self.assertEqual(metadata.getfloat(sri_section, "ydelta"), 0.0)
        self.assertEqual(metadata.getint(sri_section, "yunits"), BULKIO.UNITS_NONE)
        self.assertEqual(metadata.getint(sri_section, "mode"), 0)
        self.assertEqual(metadata.get(sri_section, "streamID"), stream_id)
        self.assertTrue(metadata.getboolean(sri_section, "blocking")) # DataSource sets blocking=True
        
        # Verify keywords
        self.assertTrue(metadata.has_section(keywords_section))
        for k,v in keyword_dict.items():
            self.assertEqual(metadata.get(keywords_section, k), str(v))
        
        # Verify First Packet Time
        self.assertTrue(metadata.has_section(pkt_1_section))
        self.assertEqual(metadata.getint(pkt_1_section, "tcmode"), 1)
        self.assertEqual(metadata.getint(pkt_1_section, "tcstatus"), 1)
        self.assertEqual(metadata.getfloat(pkt_1_section, "toff"), 0.0)
        self.assertEqual(metadata.getfloat(pkt_1_section, "twsec"), int(first_pkt_time))
        self.assertAlmostEqual(metadata.getfloat(pkt_1_section, "tfsec"), first_pkt_time - int(first_pkt_time))
        
        if last_pkt_time is None:
            # There should have only been one packet sent
            self.assertFalse(metadata.has_section(pkt_n_section))
        else:
            # Verify Last Packet Time
            self.assertTrue(metadata.has_section(pkt_n_section))
            self.assertEqual(metadata.getint(pkt_n_section, "tcmode"), 1)
            self.assertEqual(metadata.getint(pkt_n_section, "tcstatus"), 1)
            self.assertEqual(metadata.getfloat(pkt_n_section, "toff"), 0.0)
            self.assertEqual(metadata.getfloat(pkt_n_section, "twsec"), int(last_pkt_time))
            self.assertAlmostEqual(metadata.getfloat(pkt_n_section, "tfsec"), last_pkt_time - int(last_pkt_time))
        return True
    
    def _validate_data(self, filename, expected_data, endian):
        if endian == "big":
            fmt_dir = ">"
        elif endian == "little":
            fmt_dir = "<"
        else:
            fmt_dir = "@"
        
        data_type = "f"
        data_size = 4.
                
        f = open(filename, "rb")
        bytes = f.read()
        f.close()
        
        fmt = fmt_dir + "%d" % (len(bytes)/data_size) + data_type
        data = struct.unpack(fmt, bytes)
        
        self.assertEqual(len(data), len(expected_data))
        
        for d, e in zip(data, expected_data):
            self.assertEqual(d, e)

if __name__ == "__main__":
    ossie.utils.testing.main("../DataWriter.spd.xml") # By default tests all implementations
