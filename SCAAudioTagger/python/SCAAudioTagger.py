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
# Source: SCAAudioTagger.spd.xml
# Generated on: Thu Feb 28 15:03:24 EST 2013
# Redhawk IDE
# Version:M.1.8.3
# Build id: v201302191304
from ossie.resource import Resource, start_component
import logging

from SCAAudioTagger_base import * 
from ossie.cf import CF
from omniORB import CORBA


class SCAAudioTagger_i(SCAAudioTagger_base):
    """Adds the necessary SRI keywords for SCA Audio Player (i.e. 'Play Port')"""
    def initialize(self):
        SCAAudioTagger_base.initialize(self)
        self.encoding_updated = False
        self.channels_updated = False

    def process(self):
        data, T, EOS, streamID, sri, sriChanged, inputQueueFlushed = self.port_dataShort_in.getPacket()
        if data is None:
            return NOOP
        
        if sriChanged or self.encoding_updated or self.channels_updated or not self.port_dataShort_out.sriDict.has_key(streamID):
            self.encoding_updated = False
            self.channels_updated = False
            self.port_dataShort_out.pushSRI(self.modify_sri(sri))
        
        self.port_dataShort_out.pushPacket(data, T, EOS, streamID)
        return NORMAL
    
    def modify_sri(self, sri):
        audio_frame_size = 2 * self.channels; # dataShort - 2 bytes
        
        audio_keywords = []
        audio_keywords.append(CF.DataType("AUDIO_ENCODING", CORBA.Any(CORBA.TC_string, self.encoding)) )
        audio_keywords.append(CF.DataType("AUDIO_CHANNELS", CORBA.Any(CORBA.TC_long, self.channels)) )
        audio_keywords.append(CF.DataType("AUDIO_FRAME_SIZE", CORBA.Any(CORBA.TC_long, audio_frame_size)) ) # bytes
        audio_keywords.append(CF.DataType("AUDIO_FRAME_RATE", CORBA.Any(CORBA.TC_float, 1./sri.xdelta/(audio_frame_size*8.))) ) # (bits/sec) / (bits/frame)
        
        audio_ids = []
        for k in audio_keywords:
            audio_ids.append(k.id)
        
        update_keywords = audio_keywords
        for k in sri.keywords:
            if k.id in audio_ids:
                self._log.warning("Keyword %s is already set on incoming stream." % (k.id))
            else:
                update_keywords.append(k)
        sri.keywords = update_keywords
        self._log.debug("Sending SRI: %s" % (sri) )
        return sri
    
    def onconfigure_prop_encoding(self, oldval, newval):
        self._log.debug("onconfigure_prop_encoding() old: %s, new: %s" % (oldval, newval))
        self.encoding = newval
        if self.encoding != oldval:
            self.encoding_updated = True
    
    def onconfigure_prop_channels(self, oldval, newval):
        self._log.debug("onconfigure_prop_channels() old: %s, new: %s" % (oldval, newval))
        self.channels = newval
        if self.channels != oldval:
            self.channels_updated = True

if __name__ == '__main__':
    logging.getLogger().setLevel(logging.WARN)
    logging.debug("Starting Component")
    start_component(SCAAudioTagger_i)
