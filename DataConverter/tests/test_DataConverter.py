#!/usr/bin/env python
#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components DataConverter.
# 
# REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it under the terms of 
# the GNU Lesser General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
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
import math
import pprint

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in fcalc"""

    TYPES = ("short", "long", "float", "ulong", "double", "octet", "ushort")
    MAP = {'Octet':CORBA.TC_octet, 
           'Octet_out':CORBA.TC_octet,
           'Short':CORBA.TC_short, 
           'Short_out':CORBA.TC_short, 
           'Ushort':CORBA.TC_ushort, 
           'Ushort_out':CORBA.TC_ushort, 
           'Long':CORBA.TC_long, 
           'Long_out':CORBA.TC_long,
           'ULong':CORBA.TC_ulong,
           'Ulong_out':CORBA.TC_ulong, 
           'Float':CORBA.TC_float, 
           'Float_out':CORBA.TC_float, 
           'Double':CORBA.TC_double, 
           'Double_out':CORBA.TC_double}
    MAP_OUT = {'octet':'Octet_out', 
           'short':'Short_out', 
           'ushort':'Ushort_out', 
           'long':'Long_out', 
           'ulong':'Ulong_out',
           'float':'Float_out', 
           'double':'Double_out'} 

    def setProps(self, Octet=None, Octet_out=None, Short=None, Short_out=None, Ushort=None, Ushort_out=None, Long=None, Long_out=None, ULong=None, Ulong_out=None, Float=None, Float_out=None, Double=None, Double_out=None):
        """Each guy that you pass in is a dictionary with keys: "min", "max", and _out also has "enabled"
        """
        lcls = locals().copy()
        keys = lcls.keys()

        myProps=[]
        
        for name in keys:
            if name =='self':
                continue
            val = lcls[name]
            if val!=None:

                l= [ossie.cf.CF.DataType(id='%s::MaxValue'%name, value=CORBA.Any(self.MAP[name], val['max'])), ossie.cf.CF.DataType(id='%s::MinValue'%name, value=CORBA.Any(self.MAP[name], val['min']))]
                if val.has_key('enabled'):
                    l.append(ossie.cf.CF.DataType(id='%s::scale_enabled'%name, value=CORBA.Any(CORBA.TC_boolean, val['enabled'])))
                prop = ossie.cf.CF.DataType(id=name, value=CORBA.Any(CORBA.TypeCode("IDL:CF/Properties:1.0"), l))
                myProps.append(prop)
                setattr(self,name,val)

        if myProps:
            #configure it
            self.comp.configure(myProps)

    def mySetUp(self,short=True,long=True,float=True,ulong=True,double=True,octet=True,ushort=True):
        """Set up the unit test - this is run before every method that starts with test
        """
        l = locals()
        self.inputs={}
        self.outputs={}
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        for s in ("short", "long", "float", "ulong", "double", "octet", "ushort"):
            inPort = sb.DataSource(dataFormat=s)
            setattr(self,"%sIn", inPort)
            self.inputs[s]=inPort
            #only do the outputs if requested
            if l[s]:
                outPort = sb.DataSink()
                setattr(self,"%sOut", outPort)
                self.outputs[s]=outPort
        
        #setup my components
        self.setupComponent()
        
        self.comp.start()
        for port in self.inputs.values():
            port.start()
            port.connect(self.comp)
        
        #do the connections
        for name, port in self.outputs.items():
            port.start()
            portName = "data%s_out"%name.capitalize()
            self.comp.connect(port, None,portName)
        
    def tearDown(self):
        """Finish the unit test - this is run after every method that starts with test
        """
        self.comp.stop()
        #######################################################################
        # Simulate regular component shutdown
        self.comp.releaseObject()
        for port in self.inputs.values():
            port.stop()
        for port in self.outputs.values():
            port.stop()
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
    
    def testShort(self):
        self.main('short')
    
    def testLong(self):
        self.main('long')
    
    def testULong(self):
        self.main('ulong')
    
    def testDouble(self):
        self.main("double")
    
    def testOctet(self):
        self.main("octet")
    
    def testUShort(self):
        self.main('ushort')
    
    def testUShort(self):
        self.main('ushort')
    
    def testNoShort(self):
        self.mySetUp(short=False)
        out = self.main('short')
        for key, output in out.items():
            if key=='short':
                self.assertTrue(output==[])
            else:
                self.assertTrue(output!=[])
    
    def testNoLong(self):
        self.mySetUp(long=False)
        out = self.main('short')
        for key, output in out.items():
            if key=='long':
                self.assertTrue(output==[])
            else:
                self.assertTrue(output!=[])
    
    def testNoULong(self):
        self.mySetUp(ulong=False)
        out = self.main('short')
        for key, output in out.items():
            if key=='ulong':
                self.assertTrue(output==[])
            else:
                self.assertTrue(output!=[])
    
    def testNoDouble(self):
        self.mySetUp(double=False)
        out = self.main('short')
        for key, output in out.items():
            if key=='double':
                self.assertTrue(output==[])
            else:
                self.assertTrue(output!=[])
    
    def testNoOctet(self):
        self.mySetUp(octet=False)
        out = self.main('short')
        for key, output in out.items():
            if key=='octet':
                self.assertTrue(output==[])
            else:
                self.assertTrue(output!=[])
        
    def testNoUShort(self):
        self.mySetUp(ushort=False)
        out = self.main('short')
        for key, output in out.items():
            if key=='ushort':
                self.assertTrue(output==[])
            else:
                self.assertTrue(output!=[])

    def testScaleOutFull(self):
        #self.mySetUp(char=False,short=False,long=False,float=False,double=False,octet=False,ushort=False)
        self.mySetUp()
        self.setProps(Octet_out={"max":255,"min":0,'enabled':True}, 
                      Short_out={"max":32767,'min':-32768, 'enabled':True}, 
                      Ushort_out={"max":65535,'min':0, 'enabled':True}, 
                      Ulong_out={"max": 4294967295,'min':0, 'enabled':True},
                      Long_out={"max":2147483647,'min':-2147483648, 'enabled':True},
                      Float_out={"max":1e10,'min':-1e10, 'enabled':True},
                      Double_out={"max":1e10,'min':-1e10, 'enabled':True})
        inData = range(-32768,32768)
        self.main('short',inData)
    
    def testScaleOutCustom(self):
        #self.mySetUp(char=False,short=False,long=False,float=False,double=False,octet=False,ushort=False)
        self.mySetUp()
        self.setProps(Octet_out={"max":200,"min":100,'enabled':True}, 
                      Short_out={"max":30000,'min':-30000, 'enabled':True}, 
                      Ushort_out={"max":60000,'min':100, 'enabled':True}, 
                      Long_out={"max": 2000000000,'min':-2000000000, 'enabled':True},
                      Ulong_out={"max":4000000000,'min':100, 'enabled':True},
                      Float_out={"max":1.0,'min':-1.0, 'enabled':True},
                      Double_out={"max":1.0,'min':1.0, 'enabled':True})
        inData = range(-32768,-32760)
        self.main('short',inData)

    def testClipGain(self):
        """do a test where there is clipping with the gains set
        """
        self.mySetUp(short=True,long=False,float=False,double=False,octet=True,ushort=True,ulong=False)
        self.setProps(Octet_out={"max":255,"min":0,'enabled':True}, 
                      Short_out={"max":32767,'min':-32768, 'enabled':True}, 
                      Ushort_out={"max":65535,'min':0, 'enabled':True}, 
                      Ulong_out={"max": 4294967295,'min':0, 'enabled':True},
                      Long_out={"max":2147483647,'min':-2147483648, 'enabled':True},
                      Float_out={"max":1e10,'min':-1e10, 'enabled':True},
                      Double_out={"max":1e10,'min':-1e10, 'enabled':True},
                      Float = {"max":8e4,'min':-8e4})

        inData = [1e3*x for x in xrange(-100,100)]
        output = self.main('float',inData)

    def testClipNoGain(self):
        """Do a test where there is clipping in the pass threw mode and ensure data is OK
        """
        self.mySetUp(short=True,long=False,float=False,double=False,octet=True,ushort=True,ulong=False)

        inData = [100*x for x in xrange(-400,700)]
        output = self.main('float',inData)


    def main(self,inType=None,inData = None):
        """The main engine for all the test cases - configure the equation, push data, and get output
           As applicable
        """
        if not hasattr(self,'inputs'):
            self.mySetUp()
        
        
        if inData==None:
            maxIn = 127
            minIn = 0
            inData=[]
            for i in xrange(minIn,maxIn+1):
                inData.append(i)
        
        
        #data processing is asynchronos - so wait until the data is all processed
        count=0
        outputs={}
        for key in self.TYPES:
            outputs[key]=[]
        if inData:
            port = self.inputs[inType]
            port.push(inData)
        while True:
            for key, port in self.outputs.items():
                try:
                    out = port.getData()
                except:
                    out = None
                if out:
                    count=0
                    outputs[key].extend(out)
            if count==40:
                break
            time.sleep(.01)
            count+=1
                
        #do the validation
        #make sure it is monatonic
        #and in between the bounds when applicable
        for outType, output in outputs.items():
            minVal=None
            maxVal=None
            try:
                outDict = getattr(self,self.MAP_OUT[outType])
                if outDict['enabled']:
                    minVal = self.castType(outType,outDict['min'])
                    maxVal = self.castType(outType,outDict['max'])
            except AttributeError:
                pass

            if output:
                #print "type = ", outType
                last = self.castType(outType,output[0])
                if minVal:
                    #print last, minVal, outType
                    self.assertTrue(last>=minVal)
                for val in output:
                    #print val, last
                    val = self.castType(outType,val)
                    self.assertTrue(val>=last)
                    last=val
                if maxVal:
                    #print "***0", last, maxVal
                    try:
                       self.assertTrue(last<=maxVal)
                    except:
                        print last, maxVal
        return outputs      

    def castType(self,type,inVal):
        if type=='octet':
            if isinstance(inVal,str):
                return ord(inVal)
            else:
                return inVal
        else:
            return inVal
    
if __name__ == "__main__":
    ossie.utils.testing.main("../DataConverter.spd.xml") # By default tests all implementations
