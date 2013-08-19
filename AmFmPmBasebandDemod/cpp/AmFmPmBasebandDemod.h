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

#ifndef AMFMPMBASEBANDDEMOD_IMPL_H
#define AMFMPMBASEBANDDEMOD_IMPL_H

#include "AmFmPmBasebandDemod_base.h"
#include "am_fm_pm_baseband_demod.h"

#define BUFFER_LENGTH 8192

class AmFmPmBasebandDemod_i;

class AmFmPmBasebandDemod_i : public AmFmPmBasebandDemod_base
{
	//AmFmPmBasebandDemod_i is the implementation for a complex baseband demodulator component
	//This component wraps the AmFmPmBasebandDemod from the dsp library
	//to perform am, pm, and fm demdulation of complex baseband input signals
    ENABLE_LOGGING
    public:
        AmFmPmBasebandDemod_i(const char *uuid, const char *label);
        ~AmFmPmBasebandDemod_i();
        int serviceFunction();

        //redefine the base class' configure function
        void configure(const CF::Properties &props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration);

    private:
        //handle changes to sri
        void configureSRI(BULKIO::StreamSRI &sri);
        //Hanlde the remaking of the FM object
        void remakeDemod();
        //quick method to help us write debug
        void debugOut(std::string s);

        //lock for the demod
        boost::mutex demodLock;

        //demod class which does the work
        AmFmPmBasebandDemod* demod;
        //input array
        ComplexArray demodInput;
        //output array
        RealArray fmOutput;
        RealArray pmOutput;
        RealArray amOutput;
        //keep track which modes are used
        bool doingAM;
        bool doingFM;
        bool doingPM;

        //output buffer
        std::vector<float> outputBuffer;

        //some internal variables
        float sampleRate;
        double squelchThreshold;
        //flag to help us remake our demod when parameters change
        bool DemodParamsChanged;
        size_t inputIndex; //for loading demodInput asychronousy to input packets
        std::string streamID;

};

#endif
