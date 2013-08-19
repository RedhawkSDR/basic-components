#!/usr/bin/env python
#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components whitenoise.
# 
# REDHAWK Basic Components whitenoise is free software: you can redistribute it and/or modify it under the terms of 
# the GNU Lesser General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components whitenoise is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
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
import time
from ossie.cf import CF
from omniORB import CORBA
import numpy

class GaussianHelper(object):
    #a bunch of constants used in our calculations
    sqrtTwo = math.sqrt(2)
    a1 =  0.254829592
    a2 = -0.284496736
    a3 =  1.421413741
    a4 = -1.453152027
    a5 =  1.061405429
    p  =  0.3275911
    #the approximation for the theoretical erf function
    def erf(self, x):
        # save the sign of x
        sign = 1 if x >= 0 else -1
        x = abs(x)
    
        # A&S formula 7.1.26
        t = 1.0/(1.0 + self.p*x)
        y = 1.0 - (((((self.a5*t + self.a4)*t) + self.a3)*t + self.a2)*t + self.a1)*t*math.exp(-x*x)
        return sign*y # erf(-x) = -erf(x)

    #the cdf for gaussian noise
    def cdf(self, x, mean, stddev):
        return .5*(1+self.erf((x-mean)/stddev/self.sqrtTwo))
    
    #compute the theoretical quantized bell curve using the cdf
    #for gaussian noise given bins defined in xVals for numPoints with mean and standard deviation
    def getBellCurve(self, xVals, mean, stddev, numPoints):
        myPoints = [-1e99]
        myPoints.extend(xVals)
        last = None
        out = []
        for pt in myPoints:
            cdfPt = self.cdf(pt,mean, stddev)*numPoints
            if last:
                out.append(cdfPt-last)
            last = cdfPt
        return out
            

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in fcalc"""

    #utility methods to help with SCA set up and tear down for testing

    def setUp(self):
        """Set up the unit test - this is run before every method that starts with test
        """
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        self.sink = sb.DataSink()
        
        self.mean = 10.0
        self.stddev = 100.0
        self.xfer_len = 200000
        self.xdelta = .0001
        self.streamID = 'my_whitenoise_stream'
        #setup my components
        self.setupComponent()
        
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

    def testReal(self):
        print " ... testReal"
        self.complex = False
        self.main()

    def testComplex(self):
        print " ... testComplex"
        self.complex = True
        self.main()

                
    def main(self):
        """The main engine for all the test cases - configure the equation, push data, and get output
           As applicable
        """
                #note we have to do the manual configuration because props_from_dict doesn't handle float types
        myProps = [CF.DataType(id='mean',value=CORBA.Any(CORBA.TC_float, self.mean)), 
                   CF.DataType(id='stddev',value=CORBA.Any(CORBA.TC_float, self.stddev)),
                   CF.DataType(id='xfer_len',value=CORBA.Any(CORBA.TC_long, self.xfer_len)),
                   CF.DataType(id='sri', value=CORBA.Any(CORBA.TypeCode("IDL:CF/Properties:1.0"), 
                                                         [CF.DataType(id='xdelta', value=CORBA.Any(CORBA.TC_float, self.xdelta)), 
                                                          CF.DataType(id='complex', value=CORBA.Any(CORBA.TC_boolean, self.complex)), 
                                                          CF.DataType(id='streamID', value=CORBA.Any(CORBA.TC_string, self.streamID))]))]
                
        #configure it
        self.comp.configure(myProps)
        
        self.comp.start()
        self.sink.start()
        
        #do the connections        
        self.comp.connect(self.sink,'floatIn')

        
        #data processing is asynchronos - so wait until the data is all processed
        count=0
        while True:
            out =  self.sink.getData()
            if out:
                break
            if count==100:
                print 'no data after 100 tries'
                break
            time.sleep(.01)
            count+=1
        sriOutput = self.sink.sri()
        self.assertAlmostEquals(sriOutput.xdelta, self.xdelta)
        self.assertEqual(sriOutput.streamID, self.streamID)
        if self.complex:
             self.assertEqual(sriOutput.mode, 1)
        else:
            self.assertEqual(sriOutput.mode, 0)
        self.checkResults(out)
    
    def checkResults(self, out):        
        """check the results to see how good the white noise is doing
        """
        N = len(out)
        if self.complex:
            self.assertEqual(N,2*self.xfer_len)
        else:
            self.assertEqual(N,self.xfer_len)
        mean = sum(out)/N
        stdDev = math.sqrt(sum([(x-mean)**2 for x in out])/N)

        #validate the mean and standard deviation are correct
        self.assertTrue(abs(mean-self.mean)<1)
        self.assertTrue(abs(stdDev-self.stddev)<1)
        
        #check that the fft should be constant
        fftLen = 512
        numFFT = N/fftLen
        fftAvg = [0]*fftLen
        for i in xrange(numFFT):
            fft = numpy.fft.fft(out[i*numFFT:(i+1)*numFFT],fftLen)
            fftAvg = [x+abs(y) for x, y in zip(fftAvg, fft)]
        #we are not including the dc value - point 0 - but the rest of the average fft should be flat        
        maxAvg = max(fftAvg[1:])
        minAvg = min(fftAvg[1:])
        #we don't want a deviation of more than 25%
        self.assertTrue((maxAvg-minAvg)/maxAvg < .25)
        
        #now lets make a histogram and calculate the theoretical values for the 
        #gaussian bell curve
        #3 standard deviations is chosen as a bound through trial and error
        #as it gives us enough data per our number of points to have repeatable results
        #with too many standard deviations on the bell curve there are too few points that 
        #you can have statistical outliers and the percentage difference calculation is not a meaningful
        #metric at that point
        minVal = mean-3*stdDev
        maxVal = mean+3*stdDev
        
        #histogram the output data
        yVals, xVals = numpy.histogram(out, bins=33, range=((minVal,maxVal)))
        #now calculate the theoretical bell curve
        helper = GaussianHelper()
        bellCurve = helper.getBellCurve(xVals,mean, stdDev, N)
        #wish I had a better way to be sure this was working - but use a percent error and assert all are less than 10%
        dif = [abs(x-y)/x for x, y in zip(bellCurve,yVals)]
        self.assertTrue(max(dif)< .1)
        
    
if __name__ == "__main__":
    ossie.utils.testing.main("../whitenoise.spd.xml") # By default tests all implementations
