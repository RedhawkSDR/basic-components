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


template<typename T> struct max_min
{
	const static T maxVal=1e99;
	const static T minVal=-1e99;
};
template<> struct max_min<char>
{
	const static char maxVal=127;
	const static char minVal=-128;
};
template<> struct max_min<unsigned char>
{
	const static unsigned char maxVal=255;
	const static unsigned char minVal=0;
};
template<> struct max_min<unsigned short>
{
	const static unsigned short maxVal=65535;
	const static unsigned short minVal=0;
};
template<> struct max_min<short>
{
	const static short maxVal=32767;
	const static short minVal=-32768;
};
template<> struct max_min<CORBA::ULong>
{
	const static CORBA::ULong maxVal=4294967295;
	const static CORBA::ULong minVal=0;
};
template<> struct max_min<CORBA::Long>
{
	const static CORBA::Long maxVal=2147483647;
	const static CORBA::Long minVal=-2147483648;
};

template<typename TIn, typename TOut, typename IN_TYPE_ALLOC, typename OUT_TYPE_ALLOC>
void cast(std::vector<TIn, IN_TYPE_ALLOC>* in, std::vector<TOut, OUT_TYPE_ALLOC>* out)
{
	out->resize(in->size());
	typename std::vector<TOut, OUT_TYPE_ALLOC>::iterator o = out->begin();
	//I'm not particularly happy with this solution -- but because of rounding errors and limitations of
	//floating point I've had to make these guys doubles
	//check for clipping and clip signal rather than truncating it
	double MAX_VAL = max_min<TOut>::maxVal;
	double MIN_VAL = max_min<TOut>::minVal;

	for (typename std::vector<TIn, IN_TYPE_ALLOC>::iterator i = in->begin(); i!=in->end(); i++)
	{
		if (*i>MAX_VAL)
			*o = static_cast<TOut>(MAX_VAL);
		else if(*i<MIN_VAL)
			*o = static_cast<TOut>(MIN_VAL);
		else
			*o = static_cast<TOut>(*i);
		o++;
	}
}

class DataConverter_i : public DataConverter_base
{
    ENABLE_LOGGING
    public:
        DataConverter_i(const char *uuid, const char *label);
        ~DataConverter_i();

        int serviceFunction();
        template <class IN_PORT_TYPE> bool singleService(IN_PORT_TYPE *dataPortIn);
        template <typename OUT_TYPE, typename IN_TYPE, typename IN_TYPE_ALLOC, class OUT,  typename OUT_TYPE_ALLOC>
        void pushDataService(std::vector<IN_TYPE,IN_TYPE_ALLOC> *data, OUT *output, bool EOS, BULKIO::PrecisionUTCTime tt, std::string streamID, bool sriChanged, BULKIO::StreamSRI& SRI, std::vector<OUT_TYPE, OUT_TYPE_ALLOC>* outputVec);

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
