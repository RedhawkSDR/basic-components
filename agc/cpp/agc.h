/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This file is part of REDHAWK Basic Components agc.
 *
 * REDHAWK Basic Components agc is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * REDHAWK Basic Components agc is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this
 * program.  If not, see http://www.gnu.org/licenses/.
 */
#ifndef AGC_IMPL_H
#define AGC_IMPL_H

#include "agc_base.h"
#include "exp_agc.h"
#include <complex>
#include <valarray>

class agc_i;

class agc_i : public agc_base
{
    ENABLE_LOGGING
    public:
        agc_i(const char *uuid, const char *label);
        ~agc_i();
        int serviceFunction();
        void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
    private:

        void updateAGC(int mode);
        void propChange(const std::string& propStr);

        template<typename T>
        void updateTheAGC(ExpAgc<float,T>*& agc, std::valarray<T>& in, std::valarray<T>&out);

        ExpAgc<float,float>* realAgc;
        ExpAgc<float,std::complex<float> >* cmplxAgc;

        std::valarray<float> realIn;
        std::valarray<float> realOut;
        std::valarray<std::complex<float> > cmplxIn;
        std::valarray<std::complex<float> > cmplxOut;

        std::vector<float> outputData;

        bool agcNeedsUpdate;
        std::string streamID;
};

#endif
