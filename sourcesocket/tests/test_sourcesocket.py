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
import time

import struct

STRING_MAP = {'octet':'B',
              'char':'b',
              'short':'h',
              'ushort':'H',
              'long':'i',
              'ulong':'I',
              'float':'f',
              'double':'d'}
def toStr(data,dataType):
    """pack data in to a string
    """
    return struct.pack("%s%s"%(len(data), STRING_MAP[dataType]),*data)

def flip(dataStr,numBytes):
    """given data packed into a string - reverse bytes for a given word length and returned the byte-flipped string
    """
    out = ""
    for i in xrange(len(dataStr)/numBytes):
        l= list(dataStr[numBytes*i:numBytes*(i+1)])
        l.reverse()
        out+=(''.join(l))
    return out

import traceback                

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in sourcesocket"""
    PORT = 8645
    OCTET_DATA = [range(256)*100]
    CHAR_DATA = [range(-128,128)*100]
    U_SHORT_DATA = [range(i*16384,(i+1)*16384) for i in xrange(4)]
    SHORT_DATA = [range(i*16384,(i+1)*16384) for i in xrange(-2,2)]
    U_LONG_DATA = [range(i*2**30,(i+1)*2**30, 500000) for i in xrange(0,4)]
    LONG_DATA = [range(i*2**30,(i+1)*2**30, 500000) for i in xrange(-2,2)]
    FLOAT_DATA = DOUBLE_DATA = [[float(x) for x in range(i*4096,(i+1)*4096)] for i in xrange(16)]
    

    def startSourceSocket(self):
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
        
        
    def testScaBasicBehavior(self):
        self.startSourceSocket()
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
        
           
    #test a bunch of stuff - vary the start order for client.  Vary which one is the client.  Vary the data ports as well
    def testA(self):
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=self.OCTET_DATA,portType='octet')
    def testB(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=self.OCTET_DATA,portType='octet')
    def testC(self):
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=self.OCTET_DATA,portType='octet')
    def testD(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=self.OCTET_DATA,portType='octet')

    def testAChar(self):
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=self.CHAR_DATA,portType='char')
    def testBChar(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=self.CHAR_DATA,portType='char')
    def testCChar(self):
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=self.CHAR_DATA,portType='char')
    def testDChar(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=self.CHAR_DATA,portType='char')

    def testAUShort(self):
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=self.U_SHORT_DATA,portType='ushort')
    def testBUShort(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=self.U_SHORT_DATA,portType='ushort')
    def testCUShort(self):
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=self.U_SHORT_DATA,portType='ushort')
    def testDUShort(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=self.U_SHORT_DATA,portType='ushort')


    def testAShort(self):
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=self.SHORT_DATA,portType='short')
    def testBShort(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=self.SHORT_DATA,portType='short')
    def testCShort(self):
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=self.SHORT_DATA,portType='short')
    def testDShort(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=self.SHORT_DATA,portType='short')

    def testAULong(self):
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=self.U_LONG_DATA,portType='ulong')
    def testBULong(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=self.U_LONG_DATA,portType='ulong')
    def testCULong(self):
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=self.U_LONG_DATA,portType='ulong')
    def testDULong(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=self.U_LONG_DATA,portType='ulong')

    def testALong(self):
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=self.LONG_DATA,portType='long')
    def testBLong(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=self.LONG_DATA,portType='long')
    def testCLong(self):
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=self.LONG_DATA,portType='long')
    def testDLong(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=self.LONG_DATA,portType='long')

    def testAFloat(self):
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=self.FLOAT_DATA,portType='float')
    def testBFloat(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=self.FLOAT_DATA,portType='float')
    def testCFloat(self):
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=self.FLOAT_DATA,portType='float')
    def testDFloat(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=self.FLOAT_DATA,portType='float')

    def testADouble(self):
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=self.DOUBLE_DATA,portType='double')
    def testBDouble(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=self.DOUBLE_DATA,portType='double')
    def testCDouble(self):
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=self.DOUBLE_DATA,portType='double')
    def testDDouble(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=self.DOUBLE_DATA,portType='double')

    #Test sending a bunch of little packets
    def testLITTLE_PACKETS(self):
        print "testLITTLE_PACKETS"
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=[range(200) for _ in xrange(50000)], maxBytes=256*256, minBytes=0)

    def testLITTLE_PACKETS_2(self):
        print "testLITTLE_PACKETS_2"
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=[range(200) for _ in xrange(50000)], maxBytes=256*256, minBytes=0)     

    #Test sending two BIG PACKETS
    def testBIG_PACKETS(self):
        print "testBIG_PACKETS"
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=[range(200)*25000 for _ in xrange(2)])
    def testBIG_PACKETS_2(self):
        print "testBIG_PACKETS_2"
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=[range(200)*25000 for _ in xrange(2)])

    #A bunch of tests for byte swapping
    #start with octet port and using various number of bytes for swapping
    #flip the bits and show they are equal for the output
    def testByteSwap1(self):
        TYPE= 'octet'
        SWAP = 2
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap2(self):
        TYPE= 'octet'
        SWAP = 2
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap3(self):
        TYPE= 'octet'
        SWAP = 4
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap4(self):
        TYPE= 'octet'
        SWAP = 4
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap5(self):
        TYPE= 'octet'
        SWAP = 8
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap6(self):
        TYPE= 'octet'
        SWAP=8
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)


    def testByteSwap7(self):
        TYPE= 'octet'
        SWAP=10
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap8(self):
        TYPE= 'octet'
        SWAP = 10
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    #here are some tests where we swap both ends -
    #verify the output is equal
    def testByteSwap9(self):
        TYPE= 'octet'
        SWAP = 10
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        assert(s[:len(so)]==so)

    def testByteSwap10(self):
        TYPE= 'octet'
        SWAP = 8
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        assert(s[:len(so)]==so)

    def testByteSwap11(self):
        TYPE= 'octet'
        SWAP = 4
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        assert(s[:len(so)]==so)
        
    def testByteSwap12(self):
        TYPE= 'octet'
        SWAP = 2
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        assert(s[:len(so)]==so)
        
    #here are some tests where we use the swapping defined as the default for each port by settign SWAP to 1
    #2 bytes for short & ushort
    def testByteSwap13(self):
        TYPE= 'short'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,2)
        assert(s[:len(f)]==f)

    def testByteSwap14(self):
        TYPE= 'short'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,2)
        assert(s[:len(f)]==f)

    def testByteSwap15(self):
        TYPE= 'ushort'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,2)
        assert(s[:len(f)]==f)

    def testByteSwap16(self):
        TYPE= 'ushort'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,2)
        assert(s[:len(f)]==f)
    
    #4 bytes for long & ulong, & float
    def testByteSwap17(self):
        TYPE= 'long'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,4)
        assert(s[:len(f)]==f)

    def testByteSwap18(self):
        TYPE= 'long'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,4)
        assert(s[:len(f)]==f)

    def testByteSwap19(self):
        TYPE= 'ulong'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,4)
        assert(s[:len(f)]==f)

    def testByteSwap20(self):
        TYPE= 'ulong'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,4)
        assert(s[:len(f)]==f)

    def testByteSwap21(self):
        TYPE= 'float'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,4)
        assert(s[:len(f)]==f)

    def testByteSwap22(self):
        TYPE= 'float'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,4)
        assert(s[:len(f)]==f)
    
    
    #8 bytes for double    
    def testByteSwap23(self):
        TYPE= 'double'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,8)
        assert(s[:len(f)]==f)

    def testByteSwap24(self):
        TYPE= 'double'
        SWAP = 1
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,100)*10], byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,8)
        assert(s[:len(f)]==f)
        
    #Do a test with all the different type of ports using a non-standard byte swapping
    def testByteSwap25(self):
        TYPE= 'short'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap26(self):
        TYPE= 'short'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap27(self):
        TYPE= 'ushort'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap28(self):
        TYPE= 'ushort'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)
    
    def testByteSwap29(self):
        TYPE= 'long'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap30(self):
        TYPE= 'long'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap31(self):
        TYPE= 'ulong'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap32(self):
        TYPE= 'ulong'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap33(self):
        TYPE= 'float'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap34(self):
        TYPE= 'float'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)
    
     
    def testByteSwap35(self):
        TYPE= 'double'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=None, byteSwapSink=SWAP,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)

    def testByteSwap36(self):
        TYPE= 'double'
        SWAP = 5
        self.runTest(client = 'sourcesocket', dataPackets=[range(0,99)]*9, byteSwapSrc=SWAP, byteSwapSink=None,minBytes=1,portType=TYPE)
        s= toStr(self.input,TYPE)
        so = toStr(self.output,TYPE)
        f= flip(so,SWAP)
        assert(s[:len(f)]==f)    
        
    
    def runTest(self, clientFirst=True, client = 'sinksocket',dataPackets=[],maxBytes=None,minBytes=None, portType='octet',byteSwapSrc=None, byteSwapSink=None):
        self.startSourceSocket()
        self.startTest(client, portType)
        
        if maxBytes!=None:
            self.sourceSocket.max_bytes=maxBytes
        if minBytes!=None:
            self.sourceSocket.min_bytes=minBytes
        
        #print self.sourceSocket.query([])
        
        if clientFirst:
            self.configureClient()
            self.configureServer()
        else:
            self.configureServer()
            self.configureClient()
        
        if byteSwapSrc!=None:
            self.sourceSocket.byte_swap=byteSwapSrc
        
        if byteSwapSink!=None:
            self.sinkSocket.byte_swap=byteSwapSink
        
        self.src.start()
        self.sink.start()
        self.sourceSocket.start()
        self.sinkSocket.start()
        
        self.input=[]
        self.output=[]
        time.sleep(.1)
        for packet in dataPackets:
            self.input.extend(packet)
            #time.sleep(1e-6)
            self.src.push(packet, False, "test stream", 1.0)
            newdata =self.sink.getData()
            if newdata:
                if portType ==  'octet':
                    self.output.extend([ord(x) for x in newdata])
                else:
                    self.output.extend(newdata)
        time.sleep(.25)

        noData=0
        while True:
            newdata =self.sink.getData()
            if newdata:
                noData=0
                if portType == 'octet':
                    self.output.extend([ord(x) for x in newdata])
                else:
                    self.output.extend(newdata)
            else:
                noData+=1
                if noData==200:
                    break
                time.sleep(.01)

        print "self.sourceSocket.bytes_per_sec", self.sourceSocket.bytes_per_sec
        print "self.sinkSocket.bytes_per_sec", self.sinkSocket.bytes_per_sec
##        
        print "self.sinkSocket.total_bytes", self.sinkSocket.total_bytes
        print "self.sourceSocket.total_bytes",  self.sourceSocket.total_bytes
##        
        print "len(self.input)", len(self.input), "len(self.output)", len(self.output)
        
        self.assertTrue(len(self.output)> 0)
        self.assertTrue(len(self.input)-len(self.output)< self.sourceSocket.max_bytes)
        self.assertTrue(len(self.input)>=len(self.output))
        if byteSwapSrc==byteSwapSink:
            self.assertEquals(self.input[:len(self.output)],self.output)
        
        self.stopTest()
    
    def configureClient(self):
        self.client.connection_type='client'
        self.client.ip_address = "localhost"
        self.client.port=self.PORT
        self.assertTrue(self.client.connection_type=='client')
        self.assertTrue(self.client.port==self.PORT)
        
    def configureServer(self):
        self.server.connection_type='server'
        self.server.port=self.PORT
        self.assertTrue(self.server.connection_type=='server')
        self.assertTrue(self.server.port==self.PORT)        

    def startTest(self, client='sinksocket',portType='octet'):
        self.assertNotEqual(self.comp, None)
        self.src = sb.DataSource()
        self.sink = sb.DataSink()
        self.sourceSocket = self.comp
        self.sinkSocket = sb.launch('../../sinksocket/sinksocket.spd.xml')
        
        if client=='sinksocket':
            self.client = self.sinkSocket
            self.server = self.sourceSocket
        else:
            self.server = self.sinkSocket
            self.client = self.sourceSocket
        
        sinkSocktName = 'data%s_in'%portType.capitalize()
        #print self.sinkSocket.api()
        #print self.sink.api()
        self.src.connect(self.sinkSocket, sinkSocktName)
        self.sourceSocket.connect(self.sink, None, 'data%s_out'%portType.capitalize())
        
    def stopTest(self):
        self.src.stop()
        self.sink.stop()
        self.sinkSocket.stop()
        self.sourceSocket.stop()
                
        self.src.releaseObject()
        self.sink.releaseObject()
        self.sinkSocket.releaseObject()
        self.sourceSocket.releaseObject()
        
        self.src = self.sink = self.sinkSocket = self.sourceSocket = self.client = self.server = None
    # TODO Add additional tests here
    #
    # See:
    #   ossie.utils.bulkio.bulkio_helpers,
    #   ossie.utils.bluefile.bluefile_helpers
    # for modules that will assist with testing components with BULKIO ports
    
if __name__ == "__main__":
    ossie.utils.testing.main("../sourcesocket.spd.xml") # By default tests all implementations
