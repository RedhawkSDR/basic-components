/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This file is part of REDHAWK Basic Components.
 *
 * REDHAWK Basic Components is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * REDHAWK Basic Components is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this
 * program.  If not, see http://www.gnu.org/licenses/.
 */
/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

**************************************************************************/

#include "whitenoise.h"
#include <iostream>
#include <uuid/uuid.h>

PREPARE_LOGGING(whitenoise_i)

whitenoise_i::whitenoise_i(const char *uuid, const char *label) :
    whitenoise_base(uuid, label),
    lastTime(0)
{
	std::string s = "sri";
	setPropertyChangeListener(s, this, &whitenoise_i::newSri);
}

whitenoise_i::~whitenoise_i()
{
}

void whitenoise_i::configure (const CF::Properties& configProperties) throw (CF::PropertySet::PartialConfiguration,
        CF::PropertySet::InvalidConfiguration, CORBA::SystemException)
{
	whitenoise_base::configure(configProperties);
}

void whitenoise_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
	whitenoise_base::initialize();
	if (packetTime<=0)
	{
		//use current time as start time
		struct timeval tmp_time;
		struct timezone tmp_tz;
		gettimeofday(&tmp_time, &tmp_tz);
		packetTime = tmp_time.tv_sec+tmp_time.tv_usec / 1e6;
	}
	new_sri.hversion = 1;
	new_sri.xstart = 0.0;
	new_sri.xunits = BULKIO::UNITS_TIME;
	new_sri.subsize = 0;
	new_sri.ystart = 0.0;
	new_sri.ydelta = 0.0;
	new_sri.yunits = BULKIO::UNITS_NONE;
	//blocking in signal generators is the appropriate behavior
	new_sri.blocking = true;

	//make sure we do an initial sri
	sriChanged = true;
}

void whitenoise_i::newSri(const std::string& sriStr)
{
	if (sriStr !="sri")
	{
		std::cout<<"wrong string value "<<sriStr<<std::endl;
	}
	else
	{
		boost::mutex::scoped_lock lock(processLock);
		new_sri.xdelta = sri.xdelta;
		if (sri.complex)
			new_sri.mode = 1;
		else
			new_sri.mode=0;
		if ((!sri.streamID.empty()) && (sri.streamID !=streamID))
			streamID = sri.streamID;
		else if (streamID.empty())
		{
			//generate a new streamID
			char uuidBuff[36];
			uuid_t uuidGenerated;
			uuid_generate_random(uuidGenerated);
			uuid_unparse(uuidGenerated, uuidBuff);
			streamID = uuidBuff;
		}
		new_sri.streamID = streamID.c_str();
		sriChanged=true;
	}
}

int whitenoise_i::serviceFunction()
{
    LOG_DEBUG(whitenoise_i, "serviceFunction() example log message");

    //This algorithm uses the Box–Muller transform to transform the uniform numbers
    //into normally distributed

    //this algorithm outputs an even number of points
    //to ease execution - just add an element if its odd and remove it at the end

    float u, v; //uniform distributed numbers between [-1,1]
    float k;    //scale factor
    float s;    //r^squared - u*u+v*v
    float* ptr; // the output pointer

    bool resizeOut;
    size_t numFloats;
    if (new_sri.mode==1)
    {
    	numFloats= 2*xfer_len;
    	resizeOut = false;
    }
    else
    {
    	resizeOut = output.size()%2==1;
    	if (resizeOut)
    		numFloats = xfer_len+1;
    	else
    		numFloats = xfer_len;
    }
    output.resize(numFloats);
    size_t numLoops = numFloats/2;
    assert(numFloats%2==0);

    ptr = &output[0];
    for (unsigned int i =0; i!=numLoops; i++)
    {
		while (true)
		{
			u = 2.0*rand()/RAND_MAX - 1;
			v = 2.0*rand()/RAND_MAX - 1;
			s = u*u + v*v;
			if (s>0.0 and s < 1.0)
				break;
		}
		k = stddev * static_cast<float>(sqrt(-2 * log(double(s)) / s));
		*ptr = u*k+mean;
		ptr++;
		*ptr = v*k+mean;
		ptr++;
    }
    assert(ptr==&output[output.size()]);
    if (resizeOut)
    	//remove the last element if output is requested
    	output.resize(xfer_len);

    //good to go - lets update the time stamp

	double fsec;
	BULKIO::PrecisionUTCTime tstamp = BULKIO::PrecisionUTCTime();
	tstamp.tcmode = BULKIO::TCM_OFF;
	tstamp.tcstatus = (short)1;
	tstamp.toff = 0.0;
	double wsec = modf(packetTime,&fsec);
	tstamp.twsec = wsec;
	tstamp.tfsec = fsec;

	//send the output
	std::cerr<<"to do - remove comment after next 1.9.0 release canddiate"<<std::endl;
	if(sriChanged) // || (dataFloatOut->currentSRIs.count(streamID)==0))
	{
		dataFloatOut->pushSRI(new_sri);
		sriChanged = false;
	}

	double elapsedTime = output.size()*new_sri.xdelta;

	struct timeval tmp_time;
	struct timezone tmp_tz;

	if (throttle > 0 && lastTime > 1)
	{
		double requestedSleepTime = elapsedTime/throttle;
		gettimeofday(&tmp_time, &tmp_tz);
		double now = tmp_time.tv_sec + tmp_time.tv_usec / 1e6;
		double elapsedTime = now - lastTime;
		long actualSleepTimeUSec=  (requestedSleepTime -elapsedTime)*1000000.0;
		if (actualSleepTimeUSec > 0)
		{
			try {
					usleep(actualSleepTimeUSec);
				} catch (...) {
					return NORMAL;
				}
		}
	}
	gettimeofday(&tmp_time, &tmp_tz);
	double now = tmp_time.tv_sec + tmp_time.tv_usec / 1e6;
	lastTime = now;
	dataFloatOut->pushPacket(output,tstamp,false, streamID);

	//update the packetTime for the next time stamp
	packetTime+=elapsedTime;
    return NORMAL;
}

