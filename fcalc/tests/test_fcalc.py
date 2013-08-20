#!/usr/bin/env python
#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components fcalc.
# 
# REDHAWK Basic Components fcalc is free software: you can redistribute it and/or modify it under the terms of 
# the GNU Lesser General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components fcalc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
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
import math
from ossie.properties import props_from_dict
from math import cos
import time
from ossie.cf import CF

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in fcalc"""

    #utility methods to help with SCA set up and tear down for testing

    def setUp(self):
        """Set up the unit test - this is run before every method that starts with test
        """
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        self.src1 = sb.DataSource()
        self.src2 = sb.DataSource()
        self.sink = sb.DataSink()
        
        #start all my components
        self.startComponent()
        self.src1.start()
        self.src2.start()
        self.sink.start()
        
        #do the connections        
        self.src1.connect(self.comp,'a')
        self.src2.connect(self.comp,'b')
        self.comp.connect(self.sink,'doubleIn')
        
    def tearDown(self):
        """Finish the unit test - this is run after every method that starts with test
        """
        self.comp.stop()
        #######################################################################
        # Simulate regular component shutdown
        self.comp.releaseObject()
        self.src1.stop()
        self.src2.stop()
        self.sink.stop()      
        ossie.utils.testing.ScaComponentTestCase.tearDown(self)

    def startComponent(self):
        """Standard start-up for testing the component
        """
        #######################################################################
        # Launch the component with the default execparams
        execparams = self.getPropertySet(kinds=("execparam",), modes=("readwrite", "writeonly"), includeNil=False)
        execparams = dict([(x.id, any.from_any(x.value)) for x in execparams])
        self.launch(execparams, initialize=True)
        
        #######################################################################
        # Verify the basic state of the component
        self.assertNotEqual(self.comp, None)
        self.assertEqual(self.comp.ref._non_existent(), False)
        self.assertEqual(self.comp.ref._is_a("IDL:CF/Resource:1.0"), True)
       # self.assertEqual(self.spd.get_id(), self.comp.ref._get_identifier())
        
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
            
        self.comp.start()

    #here are the main test cases and the other helper code given a running environment
    def testA(self):
        """Test sending to port a only
        """
        self.myTestCase("3*a+4", [float(x) for x in xrange(1024)],[])
    
    def testB(self):
        """Test sending to port b only
        """
        self.myTestCase("cos(3*b+4)", [], [float(x) for x in xrange(1024)])
    
    def testBoth1(self):
        """test sending both to a and b
        """
        self.myTestCase("math.sin(.01*a)+math.cos(.1*b)",[float(x) for x in xrange(1024)],[float(x-512) for x in xrange(1024)])
    
    def testComplexA(self):
        """multiply complex input times j
        """
        inData= [float(x) for x in xrange(1024)]
        outData = []
        for (re,cx) in zip(inData[::2],inData[1::2]):
            outData.append(-cx)
            outData.append(re)
        self.myTestCase("a*1j", inData,None, outData, data1Cx=True)

    def testComplexB(self):
        """multiply real input times j
        """
        inData= [float(x) for x in xrange(1024)]
        outData = []
        for x in inData:
            outData.append(0)
            outData.append(x)
        self.myTestCase("1j*a", inData,None, outData)

    def testComplexC(self):
        """multiply real input times j
        """
        inData= [float(x) for x in xrange(1024)]
        outData = []
        for x in inData:
            outData.append(0)
            outData.append(x)
        self.myTestCase("1j*b", None, inData, outData)
    
    def testComplexD(self):
        inDataA= [float(x) for x in xrange(1024)]
        inDataB = [2*float(x) for x in xrange(1024)]
        outData = []
        for re,cx in zip(inDataA, inDataB):
            outData.append(re)
            outData.append(cx)
        self.myTestCase("a+1j*b", inDataA, inDataB, outData)

    def testComplexE(self):
        inDataA= [float(x) for x in xrange(1024)]
        inDataB = [2*float(x) for x in xrange(2048)]
        outData = []
        for i in xrange(1024):
            outData.append(inDataA[i]+inDataB[2*i])
            outData.append(inDataB[2*i+1])
        self.myTestCase("a+b", inDataA, inDataB, outData,data2Cx=True)

    def testComplexF(self):
        inDataA= [float(x) for x in xrange(2048)]
        inDataB = [2*float(x) for x in xrange(1024)]
        outData = []
        for i in xrange(1024):
            outData.append(inDataA[2*i]+2*inDataB[i])
            outData.append(inDataA[2*i+1])
        self.myTestCase("a+2*b", inDataA, inDataB, outData,data1Cx=True)

    def testComplexG(self):
        inDataA= [float(x) for x in xrange(1024)]
        inDataB = [2*float(x) for x in xrange(1024)]
        outData = []
        for i in xrange(512):
            outData.append(inDataA[2*i])
            outData.append(inDataB[2*i+1])
        self.myTestCase("complex(a.real,b.imag)", inDataA, inDataB, outData,data1Cx=True,data2Cx=True)

    def testComplexH(self):
        inDataA= [float(x) for x in xrange(1024)]
        outData = []
        for i in xrange(512):
            outData.append(inDataA[2*i])
            outData.append(0.0)
        self.myTestCase("a.real", inDataA, None, outData,data1Cx=True)

    def testComplexI(self):
        inDataA= [float(x) for x in xrange(1024)]
        outData = []
        for i in xrange(512):
            outData.append(0.0)
            outData.append(inDataA[2*i+1])
        self.myTestCase("1j*a.imag", inDataA, None, outData,data1Cx=True)
    
    def testBothLag(self):
        """Test sending data to a and b but send data to b in two different chunks to demonstrate asychronous buffering behavior
        """
        inA = [float(x) for x in xrange(1024)]
        inB = [float(x-512) for x in xrange(512)]
        equation = "a**2+math.sin(.01*b)"
        outA = self.myTestCase(equation,inA,inB,False)
        outB = self.myTestCase(None,[],inB,False)
        numOut = self.checkResults(equation,inA,inB+inB,outA+outB)
        self.assertEqual(numOut,1024)
    
    def testBadEquation(self):
        """Do a test with a bad equation to verify we have an invalid configuration
        """
        print "\n... running testBadEquation"
        print "FYI: A successful test will also cause a stack trace to be displayed"
        try:
            self.comp.configure(props_from_dict({'equation':"a+asdf+b"}))
        except CF.PropertySet.InvalidConfiguration, e:
            return 
        except Exception, e:
            raise e

    def testZeroDiv(self):
        """Test the zero division and validate the component handles it correctly
        """
        out = self.myTestCase("1/a", [float(x) for x in xrange(1024)],[],False)
        self.assertTrue(len(out)==1024)
        self.assertTrue(math.isnan(out[0]))
                
    def myTestCase(self, testEquation, data1,data2,checkResults=True,data1Cx=False, data2Cx = False):
        """The main engine for all the test cases - configure the equation, push data, and get output
           As applicable
        """    
        if testEquation:
            print "\n... running myTestCase %s" %testEquation
            self.comp.configure(props_from_dict({'equation':testEquation}))
        if data1:
            self.src1.push(data1,complexData=data1Cx)
        if data2:
            self.src2.push(data2,complexData=data2Cx)
        #data processing is asynchronos - so wait until the data is all processed
        count=0
        while True:
            out =  self.sink.getData()
            if out:
                break
            if count==100:
                break
            time.sleep(.01)
            count+=1        
        if checkResults:
            if isinstance(checkResults, list):
                numOut=0
                for got, expected in zip(out,checkResults):
                   self.assertEqual(got,expected)  
                   numOut+=1 
            else:
                numOut = self.checkResults(testEquation,data1, data2, out)
            self.assertNotEqual(numOut,0)
        return out
    
    def checkResults(self, testEquation, data1,data2,out):        
        """check the results of the calculator to make sure it is as expected
        """
        numTested=0
        if data1 and data2:
            for a,b,o in zip(data1,data2,out):
                self.assertEqual(eval(testEquation),o)
                numTested+=1
        elif data1:
             for a,o in zip(data1,out):
                 self.assertEqual(eval(testEquation),o)  
                 numTested+=1               
        elif data2:
            for b,o in zip(data2,out):
                self.assertEqual(eval(testEquation),o)
                numTested+=1
        return numTested                 
    
if __name__ == "__main__":
    ossie.utils.testing.main("../fcalc.spd.xml") # By default tests all implementations
