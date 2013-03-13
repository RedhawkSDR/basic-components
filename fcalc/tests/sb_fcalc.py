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
from ossie.utils import sb
from ossie.properties import props_from_dict

import ossie.properties
print ossie.properties.__file__
import math
from math import cos

import time
c = sb.Component('../fcalc.spd.xml') 

src1 = sb.DataSource()
src2 = sb.DataSource()
sink = sb.DataSink()

src1.connect(c,'a')
src2.connect(c,'b')
c.connect(sink,'doubleIn')

c.equation="math.sin(3*a)+cos(5*b)"
print c.query([])

sb.start()
data1 = [float(x) for x in xrange(1024)]
data2 = [float(x) for x in xrange(-512,0)]
data3 = [float(x) for x in xrange(0,512)]
src1.push(data1)
src2.push(data2)
src2.push(data3)
#sleep while it processes the data
time.sleep(1.0)

out =  sink.getData()
myOut = []
eq = str(c.equation)
print len(out)
data4 = data2
data4.extend(data3)
print len(data1), len(data4)
for a,b,o in zip(data1,data4,out):
  if eval(eq) !=o:
    print a,b,o

print "test is done"

