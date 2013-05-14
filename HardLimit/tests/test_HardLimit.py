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

from ossie.utils import sb
from ossie.properties import props_from_dict
import time

from bulkio.bulkioInterfaces import BULKIO

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in HardLimit"""

    def setUp(self):
        """Set up the unit test - this is run before every method that starts with test
        """
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        self.src1 = sb.DataSource(dataFormat="double")
        self.sink = sb.DataSink()
        
        
        
        #start all my components
        self.startComponent()
        self.src1.start()
        self.sink.start()
        
        #do the connections        
        self.src1.connect(self.comp,'dataDouble_in')
        self.comp.connect(self.sink,'doubleIn')
        
    def tearDown(self):
        """Finish the unit test - this is run after every method that starts with test
        """
        self.comp.stop()
        #######################################################################
        # Simulate regular component shutdown
        self.comp.releaseObject()
        self.src1.stop()
        self.sink.stop()      
        ossie.utils.testing.ScaComponentTestCase.tearDown(self)

    def startComponent(self):
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
        
            
        self.comp.start()

    def myTestCase(self, data, upper_limit, lower_limit):
        """The main engine for all the test cases - configure, push data, and get output
           As applicable
        """    

        # Configure upper and lower limit
        self.comp.configure(props_from_dict({'upper_limit':upper_limit,'lower_limit':lower_limit}))
        
        kw = sb.SRIKeyword("testkw", 10.0,'double')
        # Push in data
        self.src1.push(data,EOS=False, streamID='myStreamID', sampleRate=200, complexData=False, SRIKeywords=[kw], loop=None)

        #data processing is asynchronos - so wait until the data is all processed
        count=0
        while True:
            out =  self.sink.getData()
            sri = self.sink.sri()
            if out:
                break
            if count==100:
                break
            time.sleep(.01)
            count+=1        
      
        return out,sri

    def checkSRI(self,sri):
        self.assertEqual(sri.streamID,"myStreamID")
        self.assertEqual(sri.mode,0)
        self.assertAlmostEqual(sri.xdelta,1.0/200)
        self.assertEqual(sri.keywords[0].id, "testkw")
        self.assertEqual(len(sri.keywords), 1)
        self.assertAlmostEqual(any.from_any(sri.keywords[0].value), 10.0)

    def testA(self):
        """Test No Change in Data 
        """
        testdata = [float(x) for x in xrange(5)]
        outdata,sri = self.myTestCase(testdata,100.0,-100.0)
        self.assertEqual(outdata,testdata)
        self.checkSRI(sri)

    def testB(self):
        """Test Upper Limit 
        """
        testdata = [float(x) for x in xrange(20)]
        outdata,sri = self.myTestCase(testdata,10.0,-10.0)
        expectedoutput = [float(x) for x in xrange(10)]+[10.0 for x in xrange(10)]
        self.assertEqual(outdata,expectedoutput)
        self.checkSRI(sri)

    def testC(self):
        """Test Lower Limit 
        """
        testdata = [float(x) for x in xrange(-20,0)]
        outdata,sri = self.myTestCase(testdata,10.0,-10.0)
        expectedoutput = [-10.0 for x in xrange(10)]+[float(x) for x in xrange(-10,0)]
        self.assertEqual(outdata,expectedoutput)
        self.checkSRI(sri)
    
    def testD(self):
        """Test Upper and Lower Limit 
        """
        testdata = [float(x) for x in xrange(-20,20)]
        outdata,sri = self.myTestCase(testdata,10.0,-10.0)
        expectedoutput = [-10.0 for x in xrange(10)]+[float(x) for x in xrange(-10,10)] +[10.0 for x in xrange(10)]
        self.assertEqual(outdata,expectedoutput)
        self.checkSRI(sri)

    
if __name__ == "__main__":
    ossie.utils.testing.main("../HardLimit.spd.xml") # By default tests all implementations
