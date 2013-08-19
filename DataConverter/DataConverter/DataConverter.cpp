/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This file is part of REDHAWK Basic Components DataConverter.
 *
 * REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
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

#include "DataConverter.h"

PREPARE_LOGGING(DataConverter_i)

//make a calc_type which is always float unless either an input or output type is double
//then it is double
template <typename T, typename U> struct calc_type
{
   typedef float type;
};
template <typename T> struct calc_type<T, double> {
   typedef double type;
};
template <typename T> struct calc_type<double, T> {
   typedef double type;
};

//because of rounding issues I was seeing change long & ULong to use double point arithmetic
template <typename T> struct calc_type<T, CORBA::Long> {
   typedef double type;
};

template <typename T> struct calc_type<T, CORBA::ULong> {
   typedef double type;
};

//when they are both double - still use double since the compiler couldn't figure that out on its own ;-)
template <> struct calc_type<double, double> {
   typedef double type;
};

template <> struct calc_type<double, CORBA::Long> {
   typedef double type;
};
template <> struct calc_type<double, CORBA::ULong> {
   typedef double type;
};

template<typename TIn, typename TOut, typename IN_TYPE_ALLOC, typename OUT_TYPE_ALLOC>
void convert(std::vector<TIn, IN_TYPE_ALLOC>* in, std::vector<TOut, OUT_TYPE_ALLOC>* out, TIn maxIn, TIn minIn, TOut maxOut, TOut minOut)
{
	out->resize(in->size());
	//get the calculation type
	typedef typename calc_type<TIn, TOut>::type calcType;
	//compute the gain and the offset necessary to scale one type to another
	calcType gain = (static_cast<calcType>(maxOut)-minOut)/(static_cast<calcType>(maxIn)-minIn);
	calcType offset = (static_cast<calcType>(minOut)*maxIn-static_cast<calcType>(maxOut)*minIn)/ (static_cast<calcType>(maxIn) -minIn);
	//go threw the vector and convert the input to the output

	typename std::vector<TOut, OUT_TYPE_ALLOC>::iterator o = out->begin();
	{
		for (typename std::vector<TIn, IN_TYPE_ALLOC>::iterator i = in->begin(); i!=in->end(); i++)
		{
			calcType val = (*i*gain)+offset;
			if (val>maxOut)
				*o = maxOut;
			else if(val<minOut)
				*o = minOut;
			else
				*o = static_cast<TOut>(val);
			//std::cout<<"val= "<<val<<" maxOut= "<<maxOut<<" o ="<<*o<<std::endl;
			o++;
		}
	}
}

DataConverter_i::DataConverter_i(const char *uuid, const char *label) :
    DataConverter_base(uuid, label)
{
}

DataConverter_i::~DataConverter_i()
{
}

//template to get the output parameters if the output gain is enabled
template <typename OUT_TYPE> bool DataConverter_i::isEnabled(OUT_TYPE* min, OUT_TYPE* max)
{
	std::cout<<"DataConverter_i::isEnabled this should never be called"<<std::endl;
	return false;
}
//specialize the templates to get the right guys pending the data type
template <> bool DataConverter_i::isEnabled<unsigned char>(unsigned char* min, unsigned char* max)
{
	*max = Octet_out.Octet_out__MaxValue;
	*min =  Octet_out.Octet_out__MinValue;
	return Octet_out.Octet_out__scale_enabled;
}
template <> bool DataConverter_i::isEnabled<unsigned short>(unsigned short* min, unsigned short*max)
{
	*max = Ushort_out.Ushort_out__MaxValue;
	*min =  Ushort_out.Ushort_out__MinValue;
	return Ushort_out.Ushort_out__scale_enabled;
}
template <> bool DataConverter_i::isEnabled<short>(short* min, short*max)
{
	*max = Short_out.Short_out__MaxValue;
	*min =  Short_out.Short_out__MinValue;
	return Short_out.Short_out__scale_enabled;
}
template <> bool DataConverter_i::isEnabled<CORBA::ULong>(CORBA::ULong* min, CORBA::ULong*max)
{
	*max = Ulong_out.Ulong_out__MaxValue;
	*min =  Ulong_out.Ulong_out__MinValue;
	return Ulong_out.Ulong_out__scale_enabled;
}
template <> bool DataConverter_i::isEnabled<CORBA::Long>(CORBA::Long* min, CORBA::Long*max)
{
	*max = Long_out.Long_out__MaxValue;
	*min =  Long_out.Long_out__MinValue;
	return Long_out.Long_out__scale_enabled;
}
template <> bool DataConverter_i::isEnabled<float>(float* min, float*max)
{
	*max = Float_out.Float_out__MaxValue;
	*min =  Float_out.Float_out__MinValue;
	return Float_out.Float_out__scale_enabled;
}
template <> bool DataConverter_i::isEnabled<double>(double* min, double*max)
{
	*max = Double_out.Double_out__MaxValue;
	*min =  Double_out.Double_out__MinValue;
	return Double_out.Double_out__scale_enabled;
}


//template to get the input parameters
template <typename IN_TYPE> void DataConverter_i::getMinMax(IN_TYPE* min, IN_TYPE* max)
{
	std::cout<<"DataConverter_i::getMinMax this shoud never be called"<<std::endl;
}
template <> void DataConverter_i::getMinMax<unsigned char>(unsigned char* min, unsigned char* max)
{
	*max = Octet.Octet__MaxValue;
	*min = Octet.Octet__MinValue;
}
template <> void DataConverter_i::getMinMax<unsigned short>(unsigned short* min, unsigned short* max)
{
	*max = Ushort.Ushort__MaxValue;
	*min = Ushort.Ushort__MinValue;
}
template <> void DataConverter_i::getMinMax<short>(short* min, short* max)
{
	*max = Short.Short__MaxValue;
	*min = Short.Short__MinValue;
}
template <> void DataConverter_i::getMinMax<CORBA::ULong>(CORBA::ULong* min, CORBA::ULong* max)
{
	*max = ULong.ULong__MaxValue;
	*min = ULong.ULong__MinValue;
}
template <> void DataConverter_i::getMinMax<CORBA::Long>(CORBA::Long* min, CORBA::Long* max)
{
	*max = Long.Long__MaxValue;
	*min = Long.Long__MinValue;
}
template <> void DataConverter_i::getMinMax<float>(float* min, float* max)
{
	*max = Float.Float__MaxValue;
	*min = Float.Float__MinValue;
}
template <> void DataConverter_i::getMinMax<double>(double* min, double* max)
{
	*max = Double.Double__MaxValue;
	*min = Double.Double__MinValue;
}


/***********************************************************************************************

    Basic functionality:

        The service function is called by the serviceThread object (of type ProcessThread).
        This call happens immediately after the previous call if the return value for
        the previous call was NORMAL.
        If the return value for the previous call was NOOP, then the serviceThread waits
        an amount of time defined in the serviceThread's constructor.
        
    SRI:
        To create a StreamSRI object, use the following code:
                std::string stream_id = "testStream";
                BULKIO::StreamSRI sri = bulkio::sri::create(stream_id);

	Time:
	    To create a PrecisionUTCTime object, use the following code:
                BULKIO::PrecisionUTCTime tstamp = bulkio::time::utils::now();

        
    Ports:

        Data is passed to the serviceFunction through the getPacket call (BULKIO only).
        The dataTransfer class is a port-specific class, so each port implementing the
        BULKIO interface will have its own type-specific dataTransfer.

        The argument to the getPacket function is a floating point number that specifies
        the time to wait in seconds. A zero value is non-blocking. A negative value
        is blocking.  Constants have been defined for these values, bulkio::Const::BLOCKING and
        bulkio::Const::NON_BLOCKING.

        Each received dataTransfer is owned by serviceFunction and *MUST* be
        explicitly deallocated.

        To send data using a BULKIO interface, a convenience interface has been added 
        that takes a std::vector as the data input

        NOTE: If you have a BULKIO dataSDDS port, you must manually call 
              "port->updateStats()" to update the port statistics when appropriate.

        Example:
            // this example assumes that the component has two ports:
            //  A provides (input) port of type bulkio::InShortPort called short_in
            //  A uses (output) port of type bulkio::OutFloatPort called float_out
            // The mapping between the port and the class is found
            // in the component base class header file

            bulkio::InShortPort::dataTransfer *tmp = short_in->getPacket(bulkio::Const::BLOCKING);
            if (not tmp) { // No data is available
                return NOOP;
            }

            std::vector<float> outputData;
            outputData.resize(tmp->dataBuffer.size());
            for (unsigned int i=0; i<tmp->dataBuffer.size(); i++) {
                outputData[i] = (float)tmp->dataBuffer[i];
            }

            // NOTE: You must make at least one valid pushSRI call
            if (tmp->sriChanged) {
                float_out->pushSRI(tmp->SRI);
            }
            float_out->pushPacket(outputData, tmp->T, tmp->EOS, tmp->streamID);

            delete tmp; // IMPORTANT: MUST RELEASE THE RECEIVED DATA BLOCK
            return NORMAL;

        If working with complex data (i.e., the "mode" on the SRI is set to
        true), the std::vector passed from/to BulkIO can be typecast to/from
        std::vector< std::complex<dataType> >.  For example, for short data:

            bulkio::InShortPort::dataTransfer *tmp = myInput->getPacket(bulkio::Const::BLOCKING);
            std::vector<std::complex<short> >* intermediate = (std::vector<std::complex<short> >*) &(tmp->dataBuffer);
            // do work here
            std::vector<short>* output = (std::vector<short>*) intermediate;
            myOutput->pushPacket(*output, tmp->T, tmp->EOS, tmp->streamID);

        Interactions with non-BULKIO ports are left up to the component developer's discretion

    Properties:
        
        Properties are accessed directly as member variables. For example, if the
        property name is "baudRate", it may be accessed within member functions as
        "baudRate". Unnamed properties are given a generated name of the form
        "prop_n", where "n" is the ordinal number of the property in the PRF file.
        Property types are mapped to the nearest C++ type, (e.g. "string" becomes
        "std::string"). All generated properties are declared in the base class
        (DataConverter_base).
    
        Simple sequence properties are mapped to "std::vector" of the simple type.
        Struct properties, if used, are mapped to C++ structs defined in the
        generated file "struct_props.h". Field names are taken from the name in
        the properties file; if no name is given, a generated name of the form
        "field_n" is used, where "n" is the ordinal number of the field.
        
        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            //  - A boolean called scaleInput
              
            if (scaleInput) {
                dataOut[i] = dataIn[i] * scaleValue;
            } else {
                dataOut[i] = dataIn[i];
            }
            
        A callback method can be associated with a property so that the method is
        called each time the property value changes.  This is done by calling 
        setPropertyChangeListener(<property name>, this, &DataConverter_i::<callback method>)
        in the constructor.
            
        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            
        //Add to DataConverter.cpp
        DataConverter_i::DataConverter_i(const char *uuid, const char *label) :
            DataConverter_base(uuid, label)
        {
            setPropertyChangeListener("scaleValue", this, &DataConverter_i::scaleChanged);
        }

        void DataConverter_i::scaleChanged(const std::string& id){
            std::cout << "scaleChanged scaleValue " << scaleValue << std::endl;
        }
            
        //Add to DataConverter.h
        void scaleChanged(const std::string&);
        
        
************************************************************************************************/
int DataConverter_i::serviceFunction()
{
    LOG_DEBUG(DataConverter_i, "serviceFunction()");
    
    bool retService = false;

    //call each of the service functions and if any are legit

	retService = singleService(dataOctet) || retService;
	retService = singleService(dataShort) || retService;
	retService = singleService(dataUshort) || retService;
	retService = singleService(dataLong) || retService;
	retService = singleService(dataUlong) || retService;
	retService = singleService(dataFloat) || retService;
	retService = singleService(dataDouble) || retService;

	if (retService)
		return NORMAL;

	return NOOP;

}

template <class IN_PORT_TYPE> bool DataConverter_i::singleService(IN_PORT_TYPE *dataPortIn)
{
	    typename IN_PORT_TYPE::dataTransfer *packet = dataPortIn->getPacket(0);
        if (packet == NULL)
                return false;

        //std::cout<<"doing data for input "<<dataPortIn->getName()<<std::endl;

        //call for each of the outputs
        pushDataService<unsigned char>(&(packet->dataBuffer), dataOctet_out, packet->EOS, packet->T, packet->streamID, packet->sriChanged, packet->SRI, &uCharVec);
        pushDataService<short>(&(packet->dataBuffer), dataShort_out, packet->EOS, packet->T, packet->streamID, packet->sriChanged, packet->SRI, &shortVec);
        pushDataService<unsigned short>(&(packet->dataBuffer), dataUshort_out, packet->EOS, packet->T, packet->streamID, packet->sriChanged, packet->SRI, &uShortVec);
        pushDataService<CORBA::Long>(&(packet->dataBuffer), dataLong_out, packet->EOS, packet->T, packet->streamID, packet->sriChanged, packet->SRI, &corbaLongVec);
        pushDataService<CORBA::ULong>(&(packet->dataBuffer), dataUlong_out, packet->EOS, packet->T, packet->streamID, packet->sriChanged, packet->SRI, &corbaULongVec);
        pushDataService<float>(&(packet->dataBuffer), dataFloat_out, packet->EOS, packet->T, packet->streamID,packet->sriChanged, packet->SRI, &floatVec);
        pushDataService<double>(&(packet->dataBuffer), dataDouble_out, packet->EOS, packet->T, packet->streamID,packet->sriChanged, packet->SRI, &doubleVec);

        /* delete the dataTransfer object */
        delete packet;
        return true;
}

template <typename OUT_TYPE, typename IN_TYPE, typename IN_TYPE_ALLOC, class OUT, typename OUT_TYPE_ALLOC>
void DataConverter_i::pushDataService(std::vector<IN_TYPE, IN_TYPE_ALLOC> *data,
                                                              OUT *output,
                                                              bool EOS,
                                                              BULKIO::PrecisionUTCTime tt, std::string streamID,
                                                              bool sriChanged,
                                                              BULKIO::StreamSRI& SRI, std::vector<OUT_TYPE, OUT_TYPE_ALLOC>* outputVec)
{
	//if the output isn't hooked up - just quit now
	if (!output->isActive())
		return;

	//std::cout<<"doing data for output "<<output->getName()<<std::endl;

	// Reconfigure if SRI Changed
	if (sriChanged || (output->getCurrentSRI().count(streamID)==0)) {
		output->pushSRI(SRI);
	}

	//Check if we need to scale the output
	OUT_TYPE outMax;
	OUT_TYPE outMin;
	if (isEnabled(&outMin, &outMax))
	{
		IN_TYPE inMax;
		IN_TYPE inMin;
		//if we are scaling get the input min/max
		getMinMax(&inMin,&inMax);
		convert(data,outputVec,inMax,inMin,outMax,outMin);
	}
	else
		//just cast the data with no scaling
		cast(data, outputVec);
	//push the output and done
	output->pushPacket(*outputVec, tt, EOS, streamID);
}
