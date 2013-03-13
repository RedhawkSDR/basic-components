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
#
# AUTO-GENERATED
#
# Source: medianfilter.spd.xml
# Generated on: Mon Feb 25 12:14:09 EST 2013
# Redhawk IDE
# Version:R.1.8.2
# Build id: v201212041901
from ossie.resource import Resource, start_component
import logging

from scipy.signal import medfilt

from medianfilter_base import * 

class medianfilter_i(medianfilter_base):
    """This is a median filter component.  It has one property - filtLen
       filtLen must be odd
       
       This component leverages scipy to do the actual median filtering operation
       because  you don't have the state of the filter in scipy we solve this problem by caching off the last
       filtLen - 1 samples to re-run through the median filter
       
       filtering introduces a delay length.  The median filter is no exception
       We account for that by adjusting the sample offset in the time code
        
    """
    def initialize(self):
        """
        This is called by the framework immediately after your component registers with the NameService.
        """
        medianfilter_base.initialize(self)
        self.oldData=[]
        self.hasRun=False
        self.streamID = None
        

    def process(self):
        """Process loop
        """

        data, T, EOS, streamID, sri, sriChanged, inputQueueFlushed = self.port_dataFloat_in.getPacket()
            
        if data == None:
            return NOOP
        
        if self.streamID!=streamID:
            if self.streamID==None:
                self.streamID=streamID
            else:
                print "WARNING - medianfilter streamID %s differs from pkt stream ID %s. Throw this packets data on the floor" %(self.streamID, streamID)
                return NORMAL
        
        if sriChanged or not self.port_dataFloat_out.sriDict.has_key(streamID):
            self.port_dataFloat_out.pushSRI(sri)
        
        #make sure that the filterLen is odd
        if (self.filtLen%2==0):
            self.filtLen=self.filtLen+1 #must be odd
        
        #cache off filtLen in case the user configures it - it will be applied next loop           
        thisFiltLen = self.filtLen
        filtDelay = thisFiltLen-1
              
        if self.oldData:
            oldDataLen = len(self.oldData)
            if oldDataLen > filtDelay:
                #the user has shrunk the filterSize - throw away the first elements as we no longer need them
                self.oldData = self.oldData[:-filtDelay]
            elif filtDelay > oldDataLen:
                #if we haven't run yet - nothing extra to do - just keep building up samples for the future
                if self.hasRun:
                    #We don't have the old samples any more we would need to filter properly
                    #just duplicate the first sample and hope it is a good representative of the missing one
                    numMissing = filtDelay-oldDataLen
                    oldData = self.oldData                 
                    self.oldData = [oldData[0]]*numMissing
                    self.oldData.extend(oldData)               
                    
            processData = self.oldData
            processData.extend(data)
        else:         
            processData = data
                
        halfDelay = filtDelay/2
        #make sure we have enough data to process
        if len(processData) >= (thisFiltLen):
            out = medfilt(processData,thisFiltLen)
            #don't include the first and last elements as there is insufficient data to get their median properly
            outputData = out[halfDelay:-halfDelay]
            #store off the last inputs to use next loop 
            self.oldData = processData[-filtDelay:]
            #adjust the time code for the delay caused by the filtering
            T.toff = halfDelay
            #push out the data
            self.port_dataFloat_out.pushPacket(outputData.tolist(), T, EOS, sri.streamID)
            self.hasRun = True
        else:
            self.oldData = processData
        return NORMAL
        
  
if __name__ == '__main__':
    logging.getLogger().setLevel(logging.WARN)
    logging.debug("Starting Component")
    start_component(medianfilter_i)
