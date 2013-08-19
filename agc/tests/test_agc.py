#!/usr/bin/env python
#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components agc.
# 
# REDHAWK Basic Components agc is free software: you can redistribute it and/or modify it under the terms of 
# the GNU Lesser General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components agc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
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
import time
from ossie.cf import CF
from omniORB import CORBA
import random

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in fcalc"""

    #utility methods to help with SCA set up and tear down for testing

    def setProps(self, alpha=None,eps=None, avgPower=None, minPower =None,maxPower=None, enabled=None):
        myProps=[]
        if alpha!=None:
            self.alpha = alpha
            myProps.append(CF.DataType(id='alpha',value=CORBA.Any(CORBA.TC_float, self.alpha)))
        if avgPower!=None:
            self.avgPower = avgPower
            myProps.append(CF.DataType(id='avgPower',value=CORBA.Any(CORBA.TC_float, self.avgPower)))
        if minPower!=None:
            self.minPower = minPower
            myProps.append(CF.DataType(id='minPower',value=CORBA.Any(CORBA.TC_float, self.minPower)))        
        if maxPower!=None:
            self.maxPower = maxPower
            myProps.append(CF.DataType(id='maxPower',value=CORBA.Any(CORBA.TC_float, self.maxPower)))
        if enabled!=None:
            self.enabled = enabled
            myProps.append(CF.DataType(id='enabled',value=CORBA.Any(CORBA.TC_boolean, self.enabled)))

        if myProps:

            #configure it
            self.comp.configure(myProps)

    def setUp(self):
        """Set up the unit test - this is run before every method that starts with test
        """
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        self.src = sb.DataSource()
        self.sink = sb.DataSink()
        
        #setup my components
        self.setupComponent()
        
        self.setProps(alpha = .9, eps = 1e-4, avgPower = 100, minPower = 1e10, maxPower = -1e10, enabled = True)
        
        self.comp.start()
        self.src.start()
        self.sink.start()
        
        #do the connections
        self.src.connect(self.comp)        
        self.comp.connect(self.sink,'floatIn')
        
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

                
    def testOne(self):
        input = [random.random() for _ in xrange(1024)] 
        output = self.main(input)
        self.checkResults(input, output,threshold = .03)
    
    def testTwo(self):
        #send data threw to initialize it - then send another batch threw with lower power to make sure the averaging is working
        input = [random.random() for _ in xrange(1024)] 
        output = self.main(input)
        input = [random.random()/10 for _ in xrange(1024*32)] 
        output = self.main(input)
        self.checkResults(input, output,threshold=.2)
    
    def testDisabled(self):
        input = [random.random() for _ in xrange(1024)] 
        output = self.main(input)
        self.checkResults(input, output,threshold = .03)
        
        #make sure it is a pass threw if it is disabled        
        self.setProps(enabled=False)
        input = [random.random() for _ in xrange(1024)]
        output = self.main(input)
        self.checkResults(input, output,threshold=.001)
        
        #now re-enable it to ensure AGC kicks back in
        
        self.setProps(enabled=True)
        input = [random.random() for _ in xrange(1024)]
        output = self.main(input)
        self.checkResults(input, output,threshold=.03)

    def testAlphaUpdate(self):
        input = [random.random() for _ in xrange(1024)] 
        output = self.main(input)
        self.checkResults(input, output,threshold = .03)
        self.setProps(alpha=.99)
        
        input = [random.random() for _ in xrange(1024)] 
        output = self.main(input)
        self.checkResults(input, output,threshold = .05)
    
    def testBadAlpha(self):
        """Send in some bad alpha values and ensure it stays between 0 and 1
        """
        self.setProps(alpha=1.5)
        input = [random.random() for _ in xrange(1024)] 
        output = self.main(input)
        res = self.comp.query([CF.DataType(id='alpha',value=CORBA.Any(CORBA.TC_float, 0.0))])
        self.assertTrue(0<res[0].value.value()<1)
        self.setProps(alpha=-5.0)
        output = self.main(input)
        res = self.comp.query([CF.DataType(id='alpha',value=CORBA.Any(CORBA.TC_float, 0.0))])
        self.assertTrue(0<res[0].value.value()<1)
    
    def testHisteresous(self):
        """If AGC is set for hysteresis then make sure AGC is disabled when appropriate
        """
        #using a low alpha so it doesn't have too much memory and can handle the big swing
        self.setProps(minPower = 100, maxPower=10000, alpha=.75)
        #Send in some big  data which needs AGC
        input = [random.uniform(20000.0,80000.0) for _ in xrange(1024)]
        output = self.main(input)
        self.checkResults(input, output,threshold = .05)
        #Send in little data which must be AGC'd
        input = [random.uniform(-8.0,8.0) for _ in xrange(1024)]
        output = self.main(input)
        self.checkResults(input, output,threshold = .2)
        
        #send in medium data which is just right - no agc on it
        #lets put in a bunch of data to make sure that all the memory is out of the system
        #because we will be applying AGC for the first few points
        input = [random.uniform(20,80) for _ in xrange(1024)]
        output = self.main(input)
        
        #send in the final bit of data which should have no AGC applied
        input = [random.uniform(20,80) for _ in xrange(1024)]
        output = self.main(input)
        self.checkResults(input, output,threshold = .001)
    
    def testComplex(self):
        
        input = [random.random() for _ in xrange(1024)] 
        output = self.main(input, True)
        self.checkResults(input, output,threshold=.05, cmplx = True)
        input = [random.random()/10 for _ in xrange(1024*32)] 
        output = self.main(input, True)
        self.checkResults(input, output,threshold=.05, cmplx = True)

    def testRealComplex(self):
        """First send real data than send complex data
        """
        
        input = [random.random() for _ in xrange(1024)] 
        output = self.main(input, False)
        self.checkResults(input, output,threshold=.05, cmplx = False)
        input = [random.random()/10 for _ in xrange(1024*32)] 
        output = self.main(input, False)
        self.checkResults(input, output,threshold=.05, cmplx = False)
        
        input = [random.random() for _ in xrange(32*1024)] 
        output = self.main(input, True)
        self.checkResults(input, output,threshold=.1, cmplx = True)
        
        input = [random.random()/10 for _ in xrange(1024*32)] 
        output = self.main(input, True)
        self.checkResults(input, output,threshold=.1, cmplx = True)
        
        
    def main(self,inData,cmplx = False):
        """The main engine for all the test cases - configure the equation, push data, and get output
           As applicable
        """
        #data processing is asynchronos - so wait until the data is all processed
        count=0
        self.src.push(inData,complexData = cmplx)
        while True:
            out =  self.sink.getData()
            if out:
                break
            if count==100:
                break
            time.sleep(.01)
            count+=1       
        return out
    
    def checkResults(self, input, output,threshold,cmplx = False):        
        self.assertEqual(len(input), len(output))
        if self.enabled:
            avgPowerIn = self.avgVecPower(input,cmplx)
            #if the input power is not in the right range no agc
            if not (self.minPower < avgPowerIn < self.maxPower):
                avgPowerOut= self.avgVecPower(output,cmplx)
                self.assertTrue(abs((avgPowerOut - self.avgPower))/self.avgPower <threshold)
                return 
        self.checkAllElements(input, output, threshold)

    def checkAllElements(self, input, output, threshold):
        dif = [abs(i- o) for i, o in zip(input, output)]
        maxDif =max(dif)
        self.assertTrue(max(dif)<threshold)
               
    def avgVecPower(self, data,cmplx):
        #remember - if the data is complex there are 1/2 as many elements to divide by
        dataLen = len(data)
        if cmplx:
            divisor = dataLen/2
        else:
            divisor= dataLen
        return sum([abs(x)**2 for x in data])/divisor
    
if __name__ == "__main__":
    ossie.utils.testing.main("../agc.spd.xml") # By default tests all implementations
