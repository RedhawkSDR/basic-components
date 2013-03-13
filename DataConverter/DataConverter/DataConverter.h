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
 
#ifndef DATACONVERTER_IMPL_H
#define DATACONVERTER_IMPL_H

#include "DataConverter_base.h"

class DataConverter_i;

template<typename TIn, typename TOut, typename IN_TYPE_ALLOC, typename OUT_TYPE_ALLOC>
void convert(std::vector<TIn,IN_TYPE_ALLOC>* in, std::vector<TOut, OUT_TYPE_ALLOC>* out, TIn maxIn, TIn minIn, TOut maxOut, TOut minOut);

template<typename TIn, typename TOut>
void cast(std::vector<TIn>* in, std::vector<TOut>* out);

class DataConverter_i : public DataConverter_base
{
    ENABLE_LOGGING
    public: 
        DataConverter_i(const char *uuid, const char *label);
        ~DataConverter_i();
        int serviceFunction();

        template <class IN_PORT_TYPE> bool singleService(IN_PORT_TYPE *dataPortIn);
        template <typename OUT_TYPE, typename IN_TYPE, typename IN_TYPE_ALLOC, class OUT,  typename OUT_TYPE_ALLOC> void pushDataService(std::vector<IN_TYPE,IN_TYPE_ALLOC> *data, OUT *output, bool EOS, BULKIO::PrecisionUTCTime tt, std::string streamID, bool sriChanged, BULKIO::StreamSRI& SRI, std::vector<OUT_TYPE, OUT_TYPE_ALLOC>* outputVec);


        template <typename OUT_TYPE> bool isEnabled(OUT_TYPE* min, OUT_TYPE* max);
        template <typename IN_TYPE> void getMinMax(IN_TYPE* min, IN_TYPE* max);

        std::vector<char> charVec;
        std::vector<unsigned char> uCharVec;
        std::vector<unsigned short> uShortVec;
        std::vector<short> shortVec;
        std::vector<CORBA::ULong> corbaULongVec;
        std::vector<CORBA::Long> corbaLongVec;
        std::vector<float> floatVec;
        std::vector<double> doubleVec;

};

#endif
