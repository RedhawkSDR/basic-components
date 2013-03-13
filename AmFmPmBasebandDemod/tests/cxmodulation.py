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
#module to make complex baseband test signals for am, pm, and fm

import math
import numpy

def getHilbertWin(nTap,window=False):
    #quick method to generate a hilbert transform
    positive = []
    c = 2/math.pi
    for i in xrange(1,(nTap+1)/2,2):
        val = c/i
        positive.append(val)
        positive.append(0)
    
    #get rid of the last zero - it is pointless
    positive.pop()

    out = [-x for x in positive]
    out.reverse()
    out.append(0)
    out.extend(positive)
    if window:
        w = numpy.hanning(len(out))
        for i in xrange(len(out)):
            out[i]=out[i]*w[i]

    return out

def firfilter(input,h):
    return numpy.convolve(h,input,mode='valid')

def hilbertTransform(input,numTaps=127,window=True):
    h = getHilbertWin(numTaps,window)
    return firfilter(input,h)

def upperSideBand2(input,numTaps=127, window=True):
    #this implementation is the mux of the hilbert transform with the input
    hil = hilbertTransform(input,numTaps,window)
    numMissing = len(input)-len(hil)
    numStart = numMissing/2
    out = [complex(x,y) for (x,y) in zip(input[numStart:],hil)]
    return out

def upperSideBand(input,numTaps=127,window=True):
    # this implememntation changes hilber filter
    # to return an upper sideband filter

    #multiply all the taps by j and and 1 to the center tap
    h = getHilbertWin(numTaps,window)
    sideBandH = [complex(0,x) for x in h]
    filtLen = len(h)
    sideBandH[filtLen/2]= sideBandH[filtLen/2]+1
    return firfilter(input,sideBandH)    

def cxBB(input,numTaps=127,window=True):
    usb = upperSideBand(input,numTaps,window)
    #shift it by -fs/4
    #aka e^(-jnpi/2) = [1,-j,-1,j,...]
    cx = [1,complex(0,-1),-1,complex(0,1)]
    out =[]
    for i, x in enumerate(usb):
        out.append(x*cx[i%4])
    return out

def getFilterDelay(inSig,outSig):
    numMissing = len(inSig)-len(outSig)
    delay = numMissing/2
    return delay

#all of these little modulation methods use 
#a signal pocessing trick we are multiplying by
#cos(2*pi*fs/4*n*T) = cos(pi/2*n)
#so just construct the phasor ahead of time as it repeats for ever

def phasemodulate(inSig):
   """phase modulate the signal to fs/4
   """
   out = []
   #start the phasor at pi/2 to avoid an offset
   phasor = [math.pi/2,math.pi,3*math.pi/2,0]
   for i, d in enumerate(inSig):
     ph = phasor[i%4]
     out.append(math.cos(ph+d*math.pi*2))
   return out

def freqmodulate(inSig):
   """ freq modulate the signal to fs/4
   """
   phasor = [math.pi/2,math.pi,3*math.pi/2,0]
   #start the phasor at pi/2 to avoid an osset
   out =[]
   f=0
   for i, d in enumerate(inSig):
     ph = phasor[i%4]
     f+=d
     out.append(math.cos(ph+math.pi*2*f))
   return out     

def amplitudemodulate(inSig):
   """ am modulate the signal to fs/4
   """
   phasor = [0,-1,0,1]
   out=[]
   for i, d in enumerate(inSig):
     ph = phasor[i%4]
     out.append(ph*d)
   return out

def amBB(inSig, numTaps=127,window=True):
    """ Complex baseband amplitude modulation
    """
    am = amplitudemodulate(inSig)
    ambb = cxBB(am,numTaps,window)
    return ambb

def pmBB(inSig, numTaps=127,window=True):
    pm = phasemodulate(inSig)
    pmbb = cxBB(pm,numTaps,window)
    return pmbb

def fmBB(inSig, numTaps = 127, window = True):
    fm = freqmodulate(inSig)
    fmbb = cxBB(fm, numTaps, window)
    return fmbb


def makeChirpSig(numPts, mean, amplitude, lowFreq, HiFreq, freqDelta):
    #create a simple chirpSignal
    freq = lowFreq
    phase = 0.0
    up=True
    out=[]
    for _ in xrange(numPts):
        val = amplitude * math.cos(phase) + mean
        out.append(val)
        if up:
            freq+=freqDelta
            if freq >=HiFreq:
                up=False
        else:
            freq-=freqDelta
            if freq <=lowFreq:
                up = True
        phase +=freq
    return out

def getAmTest(numPts):
    return makeChirpSig(numPts,5,2,.05,.3,.0001)

def getFmTest(numPts):
    return makeChirpSig(numPts,0,.2,.05,.3,.0001)
