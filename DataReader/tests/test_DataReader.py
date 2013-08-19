#!/usr/bin/env python
#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components DataReader.
# 
# REDHAWK Basic Components DataReader is free software: you can redistribute it and/or modify it under the terms of 
# the GNU Lesser General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components DataReader is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
# PURPOSE.  See the GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License along with this 
# program.  If not, see http://www.gnu.org/licenses/.
#
import unittest
import ossie.utils.testing
from ossie.utils import sb
import os
from omniORB import any
from ossie.cf import CF
from omniORB import CORBA
import struct
import time

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in DataReader"""

    #utility methods to help with SCA set up and tear down for testing

    def setProps(self, SampleRate=None,StreamID=None,FrontendRF = None, InputFile=None, SpeedFactor=None, Play=None,ydelta=None,subsize=None):
        myProps=[]
        
        if SampleRate!=None:
            self.SampleRate = SampleRate
            myProps.append(CF.DataType(id='SampleRate', value=CORBA.Any(CORBA.TC_double, SampleRate)))
        
        if StreamID!=None:
            self.StreamID = StreamID
            myProps.append(CF.DataType(id='StreamID',value=CORBA.Any(CORBA.TC_string, StreamID)))

        if FrontendRF!=None:
            self.frontendRF=FrontendRF
            myProps.append(CF.DataType(id='FrontendRF', value=CORBA.Any(CORBA.TC_long, FrontendRF)))
            
        if InputFile!=None:
            self.InputFile = InputFile
            myProps.append(CF.DataType(id='InputFile', value=CORBA.Any(CORBA.TC_string, InputFile)))
        
        if SpeedFactor!=None:
            self.SpeedFactor = SpeedFactor
            myProps.append(CF.DataType(id='SpeedFactor', value=CORBA.Any(CORBA.TC_long, SpeedFactor)))

        if Play!=None:
            self.Play = Play
            myProps.append(CF.DataType(id='Play', value=CORBA.Any(CORBA.TC_boolean, Play)))
        
        if ydelta!=None:
            self.ydelta = ydelta
            myProps.append(CF.DataType(id='ydelta', value=CORBA.Any(CORBA.TC_double, ydelta)))
        
        if subsize!=None:
            self.subsize = subsize
            myProps.append(CF.DataType(id='subsize', value=CORBA.Any(CORBA.TC_long, subsize)))
      
        if myProps:

            #configure it
            self.comp.configure(myProps)

    def setUp(self):
        """Set up the unit test - this is run before every method that starts with test
        """
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        self.sink = sb.DataSink()
        
        #setup my components
        self.setupComponent()
        
        self.comp.start()
        self.sink.start()
        
        #do the connections        
        self.comp.connect(self.sink)
        self.output=[]
        
    def tearDown(self):
        """Finish the unit test - this is run after every method that starts with test
        """
        self.comp.stop()
        #######################################################################
        # Simulate regular component shutdown
        self.comp.releaseObject()
        self.sink.stop()      
        ossie.utils.testing.ScaComponentTestCase.tearDown(self)

    def setupComponent(self):
        """Standard start-up for testing the component
        """
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
        #self.comp.initialize()
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


    def testIt(self):
        fileName= "/tmp/DataReaderTest.file" 
        f = file(fileName,'wb')
        data = [float(x) for x in range(100)]
        s = struct.pack("%sf"%len(data), *data)
        f.write(s)
        f.close()
        self.setProps(SampleRate=1e6,StreamID="tmpstream",FrontendRF = 1234567, InputFile=fileName, SpeedFactor=1, Play=True,ydelta = .001, subsize=200)
        out = self.main()
        self.assertEqual(data, out[:len(data)])
        sri = self.sink.sri()
        self.assertEqual(sri.streamID,self.StreamID)
        self.assertEqual(sri.xdelta,1.0/self.SampleRate)
        self.assertEqual(sri.ydelta, self.ydelta)
        self.assertEqual(sri.subsize, self.subsize)
        found = False
        for x in sri.keywords:
            if x.id=="COL_RF":
                self.assertEqual(x.value.value(), self.frontendRF)
                found=True
        self.assertTrue(found)
        print "IT PASSED"
    
    def main(self):
        """The main engine for all the test cases - configure the equation, push data, and get output
           As applicable
        """
        #data processing is asynchronos - so wait until the data is all processed
        count=0
        output=[]
        while True:
            output.extend(self.sink.getData())
            if count==40:
                break
            time.sleep(.01)
            count+=1   
        return output

    
if __name__ == "__main__":
    ossie.utils.testing.main("../DataReader.spd.xml") # By default tests all implementations
