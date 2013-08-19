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
#
# AUTO-GENERATED
#
# Source: DataReader.spd.xml
from ossie.resource import Resource, start_component
import logging
import os, time
import math
import struct
from decimal import *

from bulkio.bulkioInterfaces import BULKIO
from omniORB import CORBA
from DataReader_base import *

class DataReader_i(DataReader_base):
    """<DESCRIPTION GOES HERE>"""
    def initialize(self):
        """
        This is called by the framework immediately after your component registers with the NameService.
        
        In general, you should add customization here and not in the __init__ constructor.  If you have 
        a custom port implementation you can override the specific implementation here with a statement
        similar to the following:
          self.some_port = MyPortImplementation()
        """
        DataReader_base.initialize(self)
        # Initialize file pointer and end of stream (EOS) and end of file (EOF) flags.
        # Note that EOS is never set to true in this program
        self.inFd = None
        self.EOS = False
        self.EOF = True

        # Initialize the pushPacket packet size. This is the number of floats in the 
        # packet constructed to send via pushPacket.
        self.packetSize = 8192

        self.structFormat  = 'f'
        self.direction     = '<'  # little endian
        
        # Set parameters based on float output type. Used in construction of the output packet. 
        self.floatSize = struct.calcsize(self.structFormat)
        if self.complex:
            self.testDataSize  = 2*self.floatSize
        else:
            self.testDataSize  = self.floatSize

        # Initialize SRI. 
        sri = BULKIO.StreamSRI(hversion=1,
                                    xstart=0.0,
                                    xdelta=1.0/self.SampleRate,
                                    xunits=BULKIO.UNITS_TIME,
                                    subsize=self.subsize,
                                    ystart=0.0,
                                    ydelta=self.ydelta,
                                    yunits=BULKIO.UNITS_NONE,
                                    mode=int(self.complex),    # 0=real,1=complex
                                    streamID=self.StreamID,
                                    blocking=False,
                                    keywords=[])

        # Append Keyword/Value pairs to SRI
        # This is a commonly used REDHAWK keyword for the real RF center freq of the data 
        sri.keywords.append(CF.DataType(id="COL_RF",value=CORBA.Any(CORBA.TC_long,self.FrontendRF)))

        # Assign the output port
        self.outputPort = self.port_dataFloatOut
        self.outputPort.defaultStreamSRI = sri
        self.outputPort.refreshSRI = True
        
                

    def onconfigure_prop_StreamID(self, oldvalue, newvalue):
        self.StreamID = newvalue
        if not self.StreamID:
            self.StreamID = str(uuid.uuid4())
        self.outputPort.defaultStreamSRI.streamID=self.StreamID
        self.outputPort.sriDict[self.StreamID] = self.outputPort.defaultStreamSRI
        self.outputPort.refreshSRI = True

    def onconfigure_prop_InputFile(self, oldvalue, newvalue):
        self.InputFile = newvalue
        if not os.path.exists(self.InputFile):
            self._log.error("InputFile path provided can not be accessed")

    def onconfigure_prop_FrontendRF(self, oldvalue, newvalue):  
        self.FrontendRF = newvalue
        self.outputPort.defaultStreamSRI.keywords=[]
        self.outputPort.defaultStreamSRI.keywords.append(CF.DataType(id="COL_RF",value=CORBA.Any(CORBA.TC_long,self.FrontendRF)))
        self.outputPort.refreshSRI = True        

    def onconfigure_prop_SampleRate(self, oldvalue, newvalue):
        self.SampleRate= newvalue
        self.outputPort.defaultStreamSRI.xdelta = 1.0/self.SampleRate
        self.outputPort.refreshSRI = True

    def onconfigure_prop_ydelta(self, oldvalue, newvalue):
        self.ydelta= newvalue
        self.outputPort.defaultStreamSRI.ydelta = self.ydelta
        self.outputPort.refreshSRI = True

    def onconfigure_prop_subsize(self, oldvalue, newvalue):
        self.subsize= newvalue
        self.outputPort.defaultStreamSRI.subsize = self.subsize
        self.outputPort.refreshSRI = True

    def onconfigure_prop_complex(self, oldvalue, newvalue):
        self.complex = newvalue
        self.outputPort.defaultStreamSRI.mode = int(self.complex)
        self.outputPort.refreshSRI = True

    def makeTimeStamp(self, curr_time):
        # Read current time for inclusion in packet
        intTime = int(curr_time)
        self.utcTime = BULKIO.PrecisionUTCTime(1, 1, 0.0, intTime, curr_time-intTime)        

    def process(self):
        
        if (self.Play == False):
            return NOOP
        if not (os.path.exists(self.InputFile)):
            return NOOP
        
        if (self.inFd==None):
            self.inFd = open( self.InputFile, 'rb' )    
            self.outputPort.refreshSRI = True
        if not self.inFd:
            return NOOP
            
        # Attempt to read one packetSize worth of floats from the file
        numRead = self.packetSize*self.testDataSize
        byteData = self.inFd.read(numRead) # read in bytes - x testDataSize
        byteDataLen = len(byteData)
        if byteDataLen!=numRead:
            self.EOF = True
            self.EOS = not self.Loop

            # Test whether data read from file is insufficient to create an 
            #integer value of elements
            #if not - 
            numDrop = byteDataLen%self.testDataSize
            if (numDrop) != 0:
                byteData = byteData[:-numDrop]
        
        if byteData:
            # Construct the signalData packet from the byteData read from file                            
            dataSize = len(byteData)/self.floatSize
            fmt = self.direction + str(dataSize) + self.structFormat
            signalData = struct.unpack(fmt, byteData)                    
        else:
            signalData = []

        if signalData or self.EOS:
            now = time.time()
            sleepFor = getattr(self, "nextPacket", now) - now
            # With python, it's unreasonable to expect sleep to be more accurate than a ms
            if sleepFor > 0.001:
                time.sleep(sleepFor)
                now = time.time()
            self.makeTimeStamp(now)
            # Perform remote pushPacket call
            self.outputPort.pushPacket(signalData, self.utcTime, self.EOS, self.StreamID)
            numSamples = len(signalData)
            if self.complex: numSamples /= 2
            self.nextPacket = now + ((1.0/self.SampleRate)*numSamples)
            
        if self.EOF:
            if self.inFd:
                self.inFd.close()
                if self.Loop:
                    self.inFd = None
                else:
                    self.inFd = 0
            self.EOF = False
        self.t_out = time.time()
        return NORMAL
  
if __name__ == '__main__':
    logging.getLogger().setLevel(logging.WARN)
    logging.debug("Starting Component")
    start_component(DataReader_i)

