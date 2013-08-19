/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This file is part of REDHAWK Basic Components AmFmPmBasebandDemod.
 *
 * REDHAWK Basic Components AmFmPmBasebandDemod is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * REDHAWK Basic Components AmFmPmBasebandDemod is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
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

#include "AmFmPmBasebandDemod.h"

PREPARE_LOGGING(AmFmPmBasebandDemod_i)

AmFmPmBasebandDemod_i::AmFmPmBasebandDemod_i(const char *uuid, const char *label) :
    AmFmPmBasebandDemod_base(uuid, label)
{
	debugOut("AmFmPmBasebandDemod_i::BasebandDemod_i() constructor entry");
    demodInput.resize(BUFFER_LENGTH, Complex(0.0,0.0));
    fmOutput.resize(BUFFER_LENGTH, Real(0.0));
    pmOutput.resize(BUFFER_LENGTH, Real(0.0));
    amOutput.resize(BUFFER_LENGTH, Real(0.0));
    outputBuffer.reserve(BUFFER_LENGTH);
    //initialize processing classes and private variables
    demod = NULL;
    inputIndex=0;
    squelchThreshold = 0;
    sampleRate=0;
    DemodParamsChanged = false;
    dataFloat_In->setMaxQueueDepth(1000);
}

AmFmPmBasebandDemod_i::~AmFmPmBasebandDemod_i()
{
	boost::mutex::scoped_lock lock(demodLock);
	if(demod!=NULL)
		delete demod;
}

void AmFmPmBasebandDemod_i::debugOut(std::string s)
{
	if (debug)
	{
		std::cout<<"AmFmPmBasebandDemod_i::"<<s<<std::endl;
		LOG_DEBUG(AmFmPmBasebandDemod_i, s);
	}
}
void AmFmPmBasebandDemod_i::configure(const CF::Properties & props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)
{
	debugOut("configure() entry");
    AmFmPmBasebandDemod_base::configure(props);
    for (CORBA::ULong i=0; i< props.length(); ++i) {
    	const std::string id = (const char*) props[i].id;
    	PropertyInterface* property = getPropertyFromId(id);
    	if (property->id=="squelch") {
    		squelchThreshold = std::pow(10.0,squelch / 10);
    	}
    	else if (property->id=="freqDeviation" || property->id=="phaseDeviation") {
    		DemodParamsChanged = true;
    	}
    }
    if(debug)
    {
    	std::cout <<"AmFmPmBasebandDemod_i::configure() - freqDeviation    = "<<freqDeviation<<std::endl;
    	std::cout <<"AmFmPmBasebandDemod_i::configure() - phaseDeviation   = "<<phaseDeviation<<std::endl;
    	std::cout <<"AmFmPmBasebandDemod_i::configure() - squelch          = "<<squelch<<std::endl;
    	std::cout <<"AmFmPmBasebandDemod_i::configure() - squelchThreshold = "<<squelchThreshold<<std::endl;
    	std::cout <<"AmFmPmBasebandDemod_i::configure() - debug            = "<<debug<<std::endl;
    }
}


int AmFmPmBasebandDemod_i::serviceFunction()
{
    debugOut("serviceFunction()");
    bulkio::InFloatPort::dataTransfer *pkt = dataFloat_In->getPacket(0.0);
    if (pkt==NULL)
        return NOOP;

    bool forceSriUpdate = false;
    if (streamID!=pkt->streamID)
    {
    	if (streamID=="")
    	{
    		forceSriUpdate=true;
    		streamID=pkt->streamID;
    	}
    	else
    	{
    		std::cout<<"AmFmPmBasebandDemod_i::WARNING -- pkt streamID "<<pkt->streamID<<" differs from streamID "<< streamID<<". Throw the data on the floor"<<std::endl;
    		delete pkt; //must delete the dataTransfer object when no longer needed
    		return NORMAL;
    	}
    }

    //Check if SRI has been changed
    if (pkt->sriChanged || forceSriUpdate) {
        debugOut("@ serviceFunction() - sriChanges ");
    	configureSRI(pkt->SRI); //Process and/or update the SRI
    	am_dataFloat_out->pushSRI(pkt->SRI);
    	fm_dataFloat_out->pushSRI(pkt->SRI);
    	pm_dataFloat_out->pushSRI(pkt->SRI);
    }
    if (!DemodParamsChanged)
    {
        //check to see if we need to remake our demod because of changes to the connections
		DemodParamsChanged = ((doingAM !=(am_dataFloat_out->state()!=BULKIO::IDLE)) ||
							  (doingPM !=(pm_dataFloat_out->state()!=BULKIO::IDLE)) ||
							  (doingFM !=(fm_dataFloat_out->state()!=BULKIO::IDLE)));
    }
	if (DemodParamsChanged){
        if (debug)
        	std::cout<< "@ AmFmPmBasebandDemod_i:serviceFunction() - DemodParamsChanged = "<<DemodParamsChanged<<".\n";
    	remakeDemod();
    }

    if(pkt->inputQueueFlushed && debug)
    	debugOut("ERROR @ serviceFunction() - Input queue is flushing.");
    if (demod==NULL){
    	debugOut("WARNING @ serviceFunction - demod is not configured");
    	std::cout<<"::serviceFunction - demod is not configured\n";
    	delete pkt;
    	return NOOP;
    }


    if (debug)
    	std::cout<<"AmFmPmBasebandDemod_i::BasebandDemod_i() service function "<<pkt->dataBuffer.size()<<"\n";
    BULKIO::PrecisionUTCTime T = pkt->T;
    bool EOS = pkt->EOS;
    std::string streamID = pkt->streamID;

    //process some data
    for(size_t i=0; i< pkt->dataBuffer.size(); i+=2) {
    	//convert to the tunerInput complex data type
    	demodInput[inputIndex++] = Complex(pkt->dataBuffer[i],pkt->dataBuffer[i+1]);
    	if (inputIndex==BUFFER_LENGTH) {
    		inputIndex=0;
    		{
    			boost::mutex::scoped_lock lock(demodLock);
    			if (!demod->process())
    				std::cout<<"ERROR @DemodParamsChanged Demod Process\n"<<std::endl;
    		}
    		if(doingAM)
    		{
    			//Check Squelch and output
    			for (size_t j=0; j<amOutput.size(); j++) {
    					if(amOutput[j]*amOutput[j] <= squelchThreshold)
    						amOutput[j]=0.0;
    				outputBuffer.push_back(amOutput[j]);
    			}
    			if(debug)
    				std::cout<<"AmFmPmBasebandDemod_i::BasebandDemod_i() pushing AM" <<outputBuffer.size()<<std::endl;
    			am_dataFloat_out->pushPacket(outputBuffer, T, EOS, streamID);
    			outputBuffer.clear();
    		}
    		if (doingFM)
    		{
				for (size_t j=0; j<fmOutput.size(); j++) {
					if(fmOutput[j]*fmOutput[j] <= squelchThreshold)
						fmOutput[j]=0.0;
					outputBuffer.push_back(fmOutput[j]);
				}
				if(debug)
					std::cout<<"AmFmPmBasebandDemod_i::BasebandDemod_i() pushing FM" <<outputBuffer.size()<<std::endl;
				fm_dataFloat_out->pushPacket(outputBuffer, T, EOS, streamID);
				outputBuffer.clear();
    		}
    		if (doingPM)
    		{
				for (size_t j=0; j<pmOutput.size(); j++) {
					if(pmOutput[j]*pmOutput[j] <= squelchThreshold)
						pmOutput[j]=0.0;
					outputBuffer.push_back(pmOutput[j]);
				}
				if(debug)
					std::cout<<"AmFmPmBasebandDemod_i::BasebandDemod_i() pushing PM" <<outputBuffer.size()<<std::endl;
				pm_dataFloat_out->pushPacket(outputBuffer, T, EOS, streamID);
				outputBuffer.clear();
    		}
    	}
    }
	delete pkt; //must delete the dataTransfer object when no longer needed
	if (debug)
		std::cout<<"AmFmPmBasebandDemod_i::BasebandDemod_i() service function done "<<inputIndex<<"\n";
	return NORMAL;
}

void AmFmPmBasebandDemod_i::configureSRI(BULKIO::StreamSRI &sri)
{
	if (debug)
		std::cout<<"BaseBandDemmod::configureSRI() entry\n\tsri.xdelta = "<<sri.xdelta<<"\n"<<"\tstri.streamID = "<<sri.streamID<<"\n";
	if (sri.mode!=1)
		debugOut("WARNING --  mode is not 1 -- treating real data as if complex");
	float tmpSampleRate = 1.0/sri.xdelta;
	if (sampleRate != tmpSampleRate) {
		sampleRate =tmpSampleRate;
		if (freqDeviation <=0)
		{
			if (debug)
				std::cout<<"DemodParamsChanged::configureSRI() - Sample rate changed sampleRate = "<<sampleRate<<std::endl;
			DemodParamsChanged=true;
		}
	}
	//data real even though input was complex
	sri.mode=0;
}

void AmFmPmBasebandDemod_i::remakeDemod()
{
	boost::mutex::scoped_lock lock(demodLock);
	double freqGain;
	if (freqDeviation <=0)
		freqGain = sampleRate;
	else
		freqGain=freqDeviation;
	float inialPhase = 0;
	if(demod!=NULL)
	{
		inialPhase = demod->getPhase();
		delete demod;
	}
	RealArray* amBuf =& amOutput;
	RealArray* pmBuf =& pmOutput;
	RealArray* fmBuf =& fmOutput;
	doingAM = am_dataFloat_out->state()!=BULKIO::IDLE;
	if (!doingAM)
		amBuf=NULL;
	doingPM = pm_dataFloat_out->state()!=BULKIO::IDLE;
	if (!doingPM)
		pmBuf=NULL;
	doingFM = fm_dataFloat_out->state()!=BULKIO::IDLE;
	if (!doingFM)
		fmBuf=NULL;
	if (debug)
		std::cout<<"AmFmPmBasebandDemod::remakeDemod() entry\nsampleRate = "<<sampleRate <<"\n"<<"freqGain = "<<freqGain<<"\n"<<"phaseDeviation = "<<phaseDeviation<<" doingAM = "<<doingAM<<" doingPM = "<<doingPM<<" doingFM = "<<doingFM<<"\n";
	demod = new AmFmPmBasebandDemod(demodInput, amBuf, pmBuf, fmBuf, freqGain, phaseDeviation,inialPhase);
	DemodParamsChanged = false;
}
