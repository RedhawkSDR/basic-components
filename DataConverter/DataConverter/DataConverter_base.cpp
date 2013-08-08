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
#include "DataConverter_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY
    
 	Source: DataConverter.spd.xml
 	Generated on: Thu Aug 08 18:56:39 UTC 2013
 	REDHAWK IDE
 	Version:  1.9.0
 	Build id: N201308081242

*******************************************************************************************/

/******************************************************************************************

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/
 
DataConverter_base::DataConverter_base(const char *uuid, const char *label) :
                                     Resource_impl(uuid, label), serviceThread(0) {
    construct();
}

void DataConverter_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    dataOctet = new BULKIO_dataOctet_In_i("dataOctet", this);
    oid = ossie::corba::RootPOA()->activate_object(dataOctet);
    dataUshort = new BULKIO_dataUshort_In_i("dataUshort", this);
    oid = ossie::corba::RootPOA()->activate_object(dataUshort);
    dataShort = new BULKIO_dataShort_In_i("dataShort", this);
    oid = ossie::corba::RootPOA()->activate_object(dataShort);
    dataUlong = new BULKIO_dataUlong_In_i("dataUlong", this);
    oid = ossie::corba::RootPOA()->activate_object(dataUlong);
    dataLong = new BULKIO_dataLong_In_i("dataLong", this);
    oid = ossie::corba::RootPOA()->activate_object(dataLong);
    dataFloat = new BULKIO_dataFloat_In_i("dataFloat", this);
    oid = ossie::corba::RootPOA()->activate_object(dataFloat);
    dataDouble = new BULKIO_dataDouble_In_i("dataDouble", this);
    oid = ossie::corba::RootPOA()->activate_object(dataDouble);
    dataOctet_out = new BULKIO_dataOctet_Out_i("dataOctet_out", this);
    oid = ossie::corba::RootPOA()->activate_object(dataOctet_out);
    dataShort_out = new BULKIO_dataShort_Out_i("dataShort_out", this);
    oid = ossie::corba::RootPOA()->activate_object(dataShort_out);
    dataUshort_out = new BULKIO_dataUshort_Out_i("dataUshort_out", this);
    oid = ossie::corba::RootPOA()->activate_object(dataUshort_out);
    dataLong_out = new BULKIO_dataLong_Out_i("dataLong_out", this);
    oid = ossie::corba::RootPOA()->activate_object(dataLong_out);
    dataUlong_out = new BULKIO_dataUlong_Out_i("dataUlong_out", this);
    oid = ossie::corba::RootPOA()->activate_object(dataUlong_out);
    dataFloat_out = new BULKIO_dataFloat_Out_i("dataFloat_out", this);
    oid = ossie::corba::RootPOA()->activate_object(dataFloat_out);
    dataDouble_out = new BULKIO_dataDouble_Out_i("dataDouble_out", this);
    oid = ossie::corba::RootPOA()->activate_object(dataDouble_out);

    registerInPort(dataOctet);
    registerInPort(dataUshort);
    registerInPort(dataShort);
    registerInPort(dataUlong);
    registerInPort(dataLong);
    registerInPort(dataFloat);
    registerInPort(dataDouble);
    registerOutPort(dataOctet_out, dataOctet_out->_this());
    registerOutPort(dataShort_out, dataShort_out->_this());
    registerOutPort(dataUshort_out, dataUshort_out->_this());
    registerOutPort(dataLong_out, dataLong_out->_this());
    registerOutPort(dataUlong_out, dataUlong_out->_this());
    registerOutPort(dataFloat_out, dataFloat_out->_this());
    registerOutPort(dataDouble_out, dataDouble_out->_this());
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void DataConverter_base::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
}

void DataConverter_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    if (serviceThread == 0) {
        dataOctet->unblock();
        dataUshort->unblock();
        dataShort->unblock();
        dataUlong->unblock();
        dataLong->unblock();
        dataFloat->unblock();
        dataDouble->unblock();
        serviceThread = new ProcessThread<DataConverter_base>(this, 0.1);
        serviceThread->start();
    }
    
    if (!Resource_impl::started()) {
    	Resource_impl::start();
    }
}

void DataConverter_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    // release the child thread (if it exists)
    if (serviceThread != 0) {
        dataOctet->block();
        dataUshort->block();
        dataShort->block();
        dataUlong->block();
        dataLong->block();
        dataFloat->block();
        dataDouble->block();
        if (!serviceThread->release(2)) {
            throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
        }
        serviceThread = 0;
    }
    
    if (Resource_impl::started()) {
    	Resource_impl::stop();
    }
}

CORBA::Object_ptr DataConverter_base::getPort(const char* _id) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{

    std::map<std::string, Port_Provides_base_impl *>::iterator p_in = inPorts.find(std::string(_id));
    if (p_in != inPorts.end()) {

        if (!strcmp(_id,"dataOctet")) {
            BULKIO_dataOctet_In_i *ptr = dynamic_cast<BULKIO_dataOctet_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataOctet::_duplicate(ptr->_this());
            }
        }
        if (!strcmp(_id,"dataUshort")) {
            BULKIO_dataUshort_In_i *ptr = dynamic_cast<BULKIO_dataUshort_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataUshort::_duplicate(ptr->_this());
            }
        }
        if (!strcmp(_id,"dataShort")) {
            BULKIO_dataShort_In_i *ptr = dynamic_cast<BULKIO_dataShort_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataShort::_duplicate(ptr->_this());
            }
        }
        if (!strcmp(_id,"dataUlong")) {
            BULKIO_dataUlong_In_i *ptr = dynamic_cast<BULKIO_dataUlong_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataUlong::_duplicate(ptr->_this());
            }
        }
        if (!strcmp(_id,"dataLong")) {
            BULKIO_dataLong_In_i *ptr = dynamic_cast<BULKIO_dataLong_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataLong::_duplicate(ptr->_this());
            }
        }
        if (!strcmp(_id,"dataFloat")) {
            BULKIO_dataFloat_In_i *ptr = dynamic_cast<BULKIO_dataFloat_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataFloat::_duplicate(ptr->_this());
            }
        }
        if (!strcmp(_id,"dataDouble")) {
            BULKIO_dataDouble_In_i *ptr = dynamic_cast<BULKIO_dataDouble_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataDouble::_duplicate(ptr->_this());
            }
        }
    }

    std::map<std::string, CF::Port_var>::iterator p_out = outPorts_var.find(std::string(_id));
    if (p_out != outPorts_var.end()) {
        return CF::Port::_duplicate(p_out->second);
    }

    throw (CF::PortSupplier::UnknownPort());
}

void DataConverter_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    // deactivate ports
    releaseInPorts();
    releaseOutPorts();

    delete(dataOctet);
    delete(dataUshort);
    delete(dataShort);
    delete(dataUlong);
    delete(dataLong);
    delete(dataFloat);
    delete(dataDouble);
    delete(dataOctet_out);
    delete(dataShort_out);
    delete(dataUshort_out);
    delete(dataLong_out);
    delete(dataUlong_out);
    delete(dataFloat_out);
    delete(dataDouble_out);
 
    Resource_impl::releaseObject();
}

void DataConverter_base::loadProperties()
{
    addProperty(Octet,
               "Octet",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Ushort,
               "Ushort",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Short,
               "Short",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(ULong,
               "ULong",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Long,
               "Long",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Float,
               "Float",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Double,
               "Double",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Octet_out,
               "Octet_out",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Ushort_out,
               "Ushort_out",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Short_out,
               "Short_out",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Ulong_out,
               "Ulong_out",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Long_out,
               "Long_out",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Float_out,
               "Float_out",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(Double_out,
               "Double_out",
               "",
               "readwrite",
               "",
               "external",
               "configure");

}
