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
from ossie.properties import props_from_dict, props_to_dict
import test_utils
import time
import copy

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in SCAAudioTagger"""

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
        # Simulate regular component shutdown - done in tearDown()
    
    def setUp(self):
        """Set up the unit test - this is run before every method that starts with test        """
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        self.src = test_utils.MyDataSource()
        self.sink = test_utils.MyDataSink()
        
        #start all my components
        self.launch()
        self.comp.start()
        
        self.src.start()
        self.sink.start()
        
        #do the connections
        self.src.connect(self.comp, 'dataShort_in')
        self.comp.connect(self.sink, usesPortName='dataShort_out')
        
        self.offset = 0.
        self.test_data_len = 5
        self.stream_id1 = "stream1"
        self.t_delta = 0.5
        self.sr = 1./self.t_delta
        
    def tearDown(self):
        """Finish the unit test - this is run after every method that starts with test        """
        self.comp.stop()
        #######################################################################
        # Simulate regular component shutdown
        self.comp.releaseObject()
        self.src.stop()
        self.sink.stop()
        ossie.utils.testing.ScaComponentTestCase.tearDown(self)
    
    def _generate_config(self):
        self.config_dict = {}
        self.config_dict["encoding"] = "ALAW"
        self.config_dict["channels"] = 2
        
    def _generate_keywords(self):
        self.keywords_dict = {}
        self.keywords_dict["test_key1"] = "value1"
        self.keywords_dict["test_key2"] = "value2"
        self.keywords_dict["test_key3"] = 3.14159
        
    def _generate_test_data(self, data_len=None):
        self.offset += 10
        if data_len is None:
            data_len = self.test_data_len
        data1 = [int(x+self.offset) for x in xrange(data_len)]
        return data1
        
    def _send_data(self, last_pkt_eos=True, complex=False):
        self.expected_data = []
        self.pkt_time=0.
        EOS = False
        self.first_pkt_time = self.pkt_time + self.t_delta
        for x in xrange(10):
            self.pkt_time+=self.t_delta
            data1 = self._generate_test_data()
            self.expected_data.extend(data1)
            self.src.push(data1, EOS=EOS, streamID=self.stream_id1, SRIKeywords=self.keywords_dict, time=self.pkt_time, sampleRate = self.sr, complexData=complex)
        
        EOS = last_pkt_eos
        self.pkt_time+=self.t_delta
        data1 = self._generate_test_data()
        self.expected_data.extend(data1)
        self.src.push(data1, EOS=EOS, streamID=self.stream_id1, SRIKeywords=self.keywords_dict, time=self.pkt_time, sampleRate = self.sr, complexData=complex)
        
        self.last_pkt_time = self.pkt_time

    def _get_received_data(self):
        received_data = []
        eos_all = False
        count = 0
        while not eos_all:
            out = self.sink.getData()
            test_utils.display_return_packets(out, count, "Sink1")
           
            for p in out:
                received_data.append(p)
            try:
                eos_all = out[-1].EOS
            except IndexError:
                pass
            time.sleep(.01)
            count += 1
            if count == 200:
                break
        return received_data

    def _validate_data(self, received_data, expected_data, config_dict=None):
        config_dict = config_dict or self.config_dict
        bytes_per_sample = 2
        exp_index = 0
        for rx_pkt in received_data:
            for d in rx_pkt.data:
                self.assertEqual(d, expected_data[exp_index])
                exp_index += 1
            
            # Verify keywords
            rx_keywords = props_to_dict(rx_pkt.sri.keywords)
            # Added audio keywords
            expected_frame_size = bytes_per_sample * config_dict["channels"]
            self.assertEqual(rx_keywords["AUDIO_ENCODING"], config_dict["encoding"])
            self.assertEqual(rx_keywords["AUDIO_CHANNELS"], config_dict["channels"])
            self.assertEqual(rx_keywords["AUDIO_FRAME_SIZE"], expected_frame_size) # 2 * channels
            self.assertEqual(rx_keywords["AUDIO_FRAME_RATE"], 1./self.t_delta/(expected_frame_size*8.))
            
            for k, v in self.keywords_dict.items():
                self.assertEqual(rx_keywords[k], v)
        
    def test_scalar(self):
        print "\n... Staring 1 channel data test"
        self._generate_config()
        self.comp_obj.configure(props_from_dict(self.config_dict))
        
        self._generate_keywords()
        self._send_data()
        rx_data = self._get_received_data()
        print "Received SRI keywords: %s" % props_to_dict(rx_data[-1].sri.keywords)
        self._validate_data(rx_data, self.expected_data)
    
    def test_complex(self):
        print "\n... Staring 2 channel data test"
        self._generate_config()
        self.config_dict["channels"] = 2
        self.comp_obj.configure(props_from_dict(self.config_dict))
        
        self._generate_keywords()
        self._send_data(complex=True)
        rx_data = self._get_received_data()
        print "Received SRI keywords: %s" % props_to_dict(rx_data[-1].sri.keywords)
        self._validate_data(rx_data, self.expected_data)
    
    def test_keyword_change(self):
        print "\n... Staring mid stream keyword change test"
        self._generate_config()
        self.comp_obj.configure(props_from_dict(self.config_dict))
        
        self._generate_keywords()
        self._send_data(last_pkt_eos=False)
        expected_data1 = self.expected_data
        
        time.sleep(1) # Allow SCAAudioTagger to process the already sent data before changing config parameters
        
        self.updated_config = copy.deepcopy(self.config_dict)
        self.updated_config["encoding"] = "PCM_SIGNED"
        self.updated_config["channels"] = 1
        self.comp_obj.configure(props_from_dict(self.updated_config))
        self._send_data(last_pkt_eos=True)
        expected_data2 = self.expected_data
        
        rx_data = self._get_received_data()
        rx_data1 = rx_data[:len(rx_data)/2]
        rx_data2 = rx_data[len(rx_data)/2:]
        print "Received SRI keywords: %s" % props_to_dict(rx_data1[-1].sri.keywords)
        print "Received SRI keywords: %s" % props_to_dict(rx_data2[-1].sri.keywords)
        self._validate_data(rx_data1, expected_data1, config_dict=self.config_dict)
        self._validate_data(rx_data2, expected_data2, config_dict=self.updated_config)
        

if __name__ == "__main__":
    ossie.utils.testing.main("../SCAAudioTagger.spd.xml") # By default tests all implementations
