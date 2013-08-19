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
#
# AUTO-GENERATED
#
# Source: fcalc.spd.xml
# Generated on: Tue Feb 12 10:24:14 EST 2013
# Redhawk IDE
# Version:R.1.8.2
# Build id: v201212041901
from ossie.resource import Resource, start_component
import logging
from ossie.cf import CF

from fcalc_base import * 
import re

import sys
class CxPortBULKIODataDoubleIn_i(bulkio.InDoublePort):
    def getPacket(self):
        """Convert data to complex if the mode is set
        """
        data, T, EOS, streamID, sri, sriChanged, inputQueueFlushed = bulkio.InDoublePort.getPacket(self)
        if data !=None and sri !=None:
            if sri.mode==1:
                data = [complex(*arg) for arg in zip(data[::2],data[1::2])]
        return data, T, EOS, streamID, sri, sriChanged, inputQueueFlushed
   

class CxPortBULKIODataDoubleOut_i(bulkio.OutDoublePort):

    def pushPacket(self, data, T, EOS, streamID, convertCx):
        """Convert data to complex if the mode is set
        """
        if not convertCx:
            #even if we don't know to convertCx - if the equation made the data complex then we 
            #need to convert it anyway
            for x in data:
                if isinstance(x,complex):
                    convertCx=True
                    if self.sriDict[streamID].mode==0:
                        self.sriDict[streamID].mode=1
                        self.refreshSRI = True
                    break        
        if convertCx:
            realData = []
            for x in data:
                if isinstance(x, complex):
                    realData.append(x.real)
                    realData.append(x.imag)
                else:
                    realData.append(x)
                    realData.append(0)
            data = realData
        return bulkio.OutDoublePort.pushPacket(self, data, T, EOS, streamID)

class fcalc_i(fcalc_base):
    """Implementation for the calculator component
    
       This component allows users to perform calculations on one or two input streams simultaneously on an element by element basis
       Set the equation property for the function you wish to calculate with the variable "a" as the first input and "b" as the second input
       
       For example "3*a+4*b" is a valid equation using both a and b
       for input a = [0,1,2,3,4] and b = [4,3,2,1,0] the output from the component would be
       out = [3*0+4*4, 3*1+4*3, 3*2+4*2, 3*3+4*1, 3*4+4*0] = [16,15,14,13,12] 

       You can also use just a or b as follows:
       "3*a" or ".2*b"
       
       What operations are supported?  I'm glad you asked...
       The "import" property is a sequence of strings that are imported for you to use in your equation
       The default set-up include both math and random
       Both "import x" and "from x import * are supported by default.
       This allows you to use functions like "math.sin(3*a)+cos(4*b)+random.random()"
       If you want to use your own custom methods just write your own custom python module, add it to the imports and you are off and running
       
       
       Finally - a word to the wise...with great power comes great responsibility
       
       The calculator offers lots of flexibility by allowing users to define their own functions in their custom modules
       This allows users to easily shoot themselves in the foot by doing weird (potentially harmful) things in their methods
       Use common sense when creating your own methods and configuring the equation to use this component responsibly.
    
    """
    
    #use some regular expressions to determine if a and b are both used in the equation
    startRe = "\A" #match the start of the string
    endRe = "\Z" #match the end of the string
    nonAplhaRe = "[^A-Aa-z_]" #match non alpha numeric digit
    nonAlphaStartRe = '(%s|%s)'%(startRe,nonAplhaRe) #match the start or a non-alpha character
    nonAlphaEndRe = '(%s|%s)'%(endRe,nonAplhaRe) #match the end or a non-alpha character
    findA = re.compile(nonAlphaStartRe+'a'+nonAlphaEndRe) #use this regular expression to find "a" in the equation
    findB = re.compile(nonAlphaStartRe+'b'+nonAlphaEndRe) #use this regular expression to find "b" in the equation 

    def initialize(self):
        """
        This is called by the framework immediately after your component registers with the NameService.
        
        In general, you should add customization here and not in the __init__ constructor.  If you have 
        a custom port implementation you can override the specific implementation here with a statement
        similar to the following:
          self.some_port = MyPortImplementation()
        """
        Resource.initialize(self)
        self.port_a = CxPortBULKIODataDoubleIn_i("a", maxsize=self.DEFAULT_QUEUE_SIZE)
        self.port_b = CxPortBULKIODataDoubleIn_i("b", maxsize=self.DEFAULT_QUEUE_SIZE)
        self.port_out = CxPortBULKIODataDoubleOut_i("out")       
        
        #take care of the requested imports    
        #we are supporting both syntax for imports
        #IE - "import x" and "from x import *"
        #this allows users to use either math.cos or just cos 
        #and they will both work
        self.globals = {}
        self.oldA = []
        self.oldB = []
        self.useA = False
        self.useB = False    
        #call this in case we already have a good equation 
        self.sri = None
        self.streamID = None
        self.onconfigure_prop_import()
        self.onconfigure_prop_equation()
        self.firsttime = True

    def onconfigure_prop_import(self,oldval=None,val=None):
        if val!=None:
            fcalc_i.import_.set(self,val)
        for module in self.import_:
            if not module in self.globals:
                e = None
                try:
                    #do the standard imports - this is equivalent to the statement:
                    #"import math"
                    mod = __import__(module)
                    self.globals[module] = mod
                except Exception, e:
                    print 'FCALC -- ERROR cannot import module "%s"'%module
                    raise CF.PropertySet.InvalidConfiguration('import "%s" is invalid' %val, [val])
                if e==None:
                    #Now add all the stuff into my name space
                    #like "from math import *" 
                    for name in dir(mod):
                        if not name.startswith('_'):
                            if name in self.globals:
                                #if we already have the method in in global dictionary - don't trample on it
                                #for example the module random has a method called random
                                #we choose to make "random" in the equation refer to the MODULE not the METHOD
                                #as things are done on a first come first serve basis
                                print "FCALC -- WARNING - NOT overriding global namespace with %s from %s" %(name, module)
                            else:    
                                self.globals[name]=getattr(mod,name)         

        
    def onconfigure_prop_equation(self,oldval=None,val=None):
        """This is called when "equation" is configured
           check the equation with the regular expression stuff to see if we are using both streams or not    
        """
        if not val:
            val = self.equation
        if val:

            oldVal = str(self.equation)
            #set the value for the equation to the new value    
            fcalc_i.equation.set(self,val)
            #do a simple test to validate the equation 
            try:
                self.evaluate(a=1,b=1)
            except Exception, e:
                print "FCALC - cannot validate equation %s" %val
                self._log.exception(e)
                fcalc_i.equation.set(self,oldVal)
                raise CF.PropertySet.InvalidConfiguration('equation "%s" is invalid' %val, [val])
            
            if val: 
                aFound = self.findA.search(val)
                self.useA =  bool(aFound)
                bFound = self.findB.search(val)
                self.useB = bool(bFound)
            else:
                self.useA = False
                self.useB = False    
            if self.useA !=self.useB:
                #if we are only using A or B then throw all the old data on the floor
                #in case we configured mid stream and someone reconfigures us again later
                #it would be weird to use old data
                self.oldA = []
                self.oldB = []

           

    def process(self):
        """Main processs loop - deligate our processing method per stream use
        """
        if self.useA and self.useB:
            return self.bothStreams()
        elif self.useA:
            #get from portB just in case someone is sending data so it doesn't cause upstream backups
            self.port_b.getPacket()
            return self.singleStream(self.port_a,True)            
        elif self.useB:
            #get from portA just in case someone is sending data so it doesn't cause upstream backups
            self.port_a.getPacket()
            return self.singleStream(self.port_b,False)
        else:
            return NOOP   

    def bothStreams(self):
        """ run the component using both input streams
        """
        dataA, tA, EOS_A, streamID_A, sriA, sriChangedA, inputQueueFlushedA = self.port_a.getPacket()
        dataB, tB, EOS_B, streamID_B, sriB, sriChangedB, inputQueueFlushedB = self.port_b.getPacket()

        if dataA == None and dataB == None:
            return NOOP
        
        if self.useAsri:
            if sriA:
                self.sri = sriA
                self.streamID = streamID_A
                if sriB and sriB.mode==1:
                    self.sri.mode=1
        else:
            if sriB:
                self.sri = sriB
                self.streamID = streamID_B
                if sriA and sriA.mode==1:
                    self.sri.mode=1
                        
        if  self.firsttime or sriChangedA or sriChangedB :
            if self.sri:
                self.port_out.pushSRI(self.sri)
                self.firsttime = False
        

        #check for old data from previous iterations
        if self.oldA:
            if dataA:
                self.oldA.extend(dataA)
            dataA = self.oldA
        elif self.oldB:
            if dataB:
                self.oldB.extend(dataB)
            dataB = self.oldB
        
        if dataA and dataB:
            #lets do the math here
            outData=[]
            for a, b in zip(dataA,dataB):
                outData.append(self.evaluate(a=a,b=b))
            #take care of the case where we have unequal data amounts 
            #by buffering more data for next time
            numA = len(dataA)
            numB = len(dataB)
            if numA > numB:
                self.oldA = dataA[numA-numB:]
            elif numB > numA:
                self.oldB = dataB[numB-numA:]
            
            EOS = bool(EOS_A or EOS_B)
            #should have a smarter way to do this with the time stamp for the output ...but we don't
            if tA:
                T = tA
            elif tB:
                T = tB
            self.port_out.pushPacket(outData, T, EOS, self.streamID, self.sri.mode)
        #append data at this point
        elif dataA:
            self.oldA = dataA
            return NOOP
        else: #dataB only
            self.oldB = dataB
            return NOOP

    def singleStream(self, port,useA):
        """Run the component in single stream only mode
        """
        data, T, EOS, streamID, sri, sriChanged, inputQueueFlushed = port.getPacket()
        
        if not data:
            return NOOP
        
        if self.firsttime or sriChanged:
            self.port_out.pushSRI(sri)
            self.firsttime = False

        outData=[]
        for d in data:
            if useA:           
                res = self.evaluate(a=d)
            else:
                res = self.evaluate(b=d)
            outData.append(res)    
        self.port_out.pushPacket(outData, T, bool(EOS), streamID, sri.mode)
        return NORMAL        
                
    def evaluate(self,**keys):
        """This is where we evaluate the result of the function
           NOTE - eval is not safe -- users can do serious damage with this
           We might consider using a safer (and less powerful) way to evaluate the function
           If we need to           
        """
        try:
            return eval(self.equation,self.globals, keys)            
        except ZeroDivisionError:
            return float('nan')
            
  
if __name__ == '__main__':
    logging.getLogger().setLevel(logging.WARN)
    logging.debug("Starting Component")
    start_component(fcalc_i)
