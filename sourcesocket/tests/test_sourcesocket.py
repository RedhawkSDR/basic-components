#!/usr/bin/env python
import unittest
import ossie.utils.testing
import os
from omniORB import any
from ossie.utils import sb
import time

import traceback

class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in sourcesocket"""
    PORT = 8645

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
        
        #######################################################################
        # Simulate regular component shutdown
        self.comp.releaseObject()
    
    
    def testA(self):
        self.runTest(clientFirst=True, client = 'sinksocket', dataPackets=[range(256)*100])
    def testB(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=[range(256)*100])
    def testC(self):
        self.runTest(clientFirst=True, client = 'sourcesocket', dataPackets=[range(256)*100])
    def testD(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=[range(256)*100])

    def testE(self):
        self.runTest(clientFirst=False, client = 'sinksocket', dataPackets=[range(256)*2048 for _ in xrange(32)], maxBytes=256*256)

    def testF(self):
        self.runTest(clientFirst=False, client = 'sourcesocket', dataPackets=[range(200) for _ in xrange(1000)], maxBytes=256*256, minBytes=0)     

    def runTest(self, clientFirst=True, client = 'sinksocket',dataPackets=[],maxBytes=None,minBytes=None):
        self.startSourceSocket()
        self.startTest(client)
        
        if maxBytes!=None:
            self.sourceSocket.max_bytes=maxBytes
        if minBytes!=None:
            self.sourceSocket.min_bytes=minBytes
        
        print self.sourceSocket.query([])
        
        if clientFirst:
            self.configureClient()
            self.configureServer()
        else:
            self.configureServer()
            self.configureClient()
        
        self.src.start()
        self.sink.start()
        self.sourceSocket.start()
        self.sinkSocket.start()
        
        self.input=[]
        self.output=[]
        time.sleep(.1)
        for packet in dataPackets:
            self.input.extend(packet)
            time.sleep(1e-6)
            self.src.push(packet, False, "test stream", 1.0)
            newdata =self.sink.getData()
            if newdata:
                self.output.extend([ord(x) for x in newdata])

        time.sleep(.25)

        noData=0
        while True:
            newdata =self.sink.getData()
            if newdata:
                noData=0
                self.output.extend([ord(x) for x in newdata])
            else:
                noData+=1
                if noData==10:
                    break
                time.sleep(.1)

#        print "self.sinkSocket.bytes_per_sec", self.sinkSocket.bytes_per_sec
#        print "self.sourceSocket.bytes_per_sec", self.sourceSocket.bytes_per_sec
##        
#        print "self.sinkSocket.total_bytes", self.sinkSocket.total_bytes
#        print "self.sourceSocket.total_bytes",  self.sourceSocket.total_bytes
##        
#        print "len(self.input)", len(self.input), "len(self.output)", len(self.output)
##        print "len(self.input)", len(self.input)
        
        self.assertTrue(len(self.output)> 0)
        self.assertTrue(len(self.input)-len(self.output)< self.sourceSocket.max_bytes)
        self.assertTrue(len(self.input)>=len(self.output))
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

    def startTest(self, client='sinksocket'):
        self.assertNotEqual(self.comp, None)
        self.src = sb.DataSource()
        self.sink = sb.DataSink()
        self.sourceSocket = self.comp
        self.sinkSocket = sb.Component('../../sinksocket/sinksocket.spd.xml')
        
        if client=='sinksocket':
            self.client = self.sinkSocket
            self.server = self.sourceSocket
        else:
            self.server = self.sinkSocket
            self.client = self.sourceSocket
        
        self.src.connect(self.sinkSocket)
        self.sourceSocket.connect(self.sink)
        
    def stopTest(self):
        self.src.stop()
        self.sink.stop()
        self.sinkSocket.stop()
        self.sourceSocket.stop()
                
        self.src.releaseObject()
        self.sink.releaseObject()
        self.sinkSocket.releaseObject()
        self.sourceSocket.releaseObject()
        
        self.src = self.sink = self.sinkSocket = self.sourceSocket = self.client = self.server = self.comp = None
     
    # TODO Add additional tests here
    #
    # See:
    #   ossie.utils.bulkio.bulkio_helpers,
    #   ossie.utils.bluefile.bluefile_helpers
    # for modules that will assist with testing components with BULKIO ports
    
if __name__ == "__main__":
    ossie.utils.testing.main("../sourcesocket.spd.xml") # By default tests all implementations
