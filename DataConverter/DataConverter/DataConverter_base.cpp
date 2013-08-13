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

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

DataConverter_base::DataConverter_base(const char *uuid, const char *label) :
    Resource_impl(uuid, label),
    serviceThread(0)
{
    construct();
}

void DataConverter_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    dataOctet = new bulkio::InOctetPort("dataOctet");
    oid = ossie::corba::RootPOA()->activate_object(dataOctet);
    dataUshort = new bulkio::InUShortPort("dataUshort");
    oid = ossie::corba::RootPOA()->activate_object(dataUshort);
    dataShort = new bulkio::InShortPort("dataShort");
    oid = ossie::corba::RootPOA()->activate_object(dataShort);
    dataUlong = new bulkio::InULongPort("dataUlong");
    oid = ossie::corba::RootPOA()->activate_object(dataUlong);
    dataLong = new bulkio::InLongPort("dataLong");
    oid = ossie::corba::RootPOA()->activate_object(dataLong);
    dataFloat = new bulkio::InFloatPort("dataFloat");
    oid = ossie::corba::RootPOA()->activate_object(dataFloat);
    dataDouble = new bulkio::InDoublePort("dataDouble");
    oid = ossie::corba::RootPOA()->activate_object(dataDouble);
    dataOctet_out = new bulkio::OutOctetPort("dataOctet_out");
    oid = ossie::corba::RootPOA()->activate_object(dataOctet_out);
    dataShort_out = new bulkio::OutShortPort("dataShort_out");
    oid = ossie::corba::RootPOA()->activate_object(dataShort_out);
    dataUshort_out = new bulkio::OutUShortPort("dataUshort_out");
    oid = ossie::corba::RootPOA()->activate_object(dataUshort_out);
    dataLong_out = new bulkio::OutLongPort("dataLong_out");
    oid = ossie::corba::RootPOA()->activate_object(dataLong_out);
    dataUlong_out = new bulkio::OutULongPort("dataUlong_out");
    oid = ossie::corba::RootPOA()->activate_object(dataUlong_out);
    dataFloat_out = new bulkio::OutFloatPort("dataFloat_out");
    oid = ossie::corba::RootPOA()->activate_object(dataFloat_out);
    dataDouble_out = new bulkio::OutDoublePort("dataDouble_out");
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
            bulkio::InOctetPort *ptr = dynamic_cast<bulkio::InOctetPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataUshort")) {
            bulkio::InUShortPort *ptr = dynamic_cast<bulkio::InUShortPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataShort")) {
            bulkio::InShortPort *ptr = dynamic_cast<bulkio::InShortPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataUlong")) {
            bulkio::InULongPort *ptr = dynamic_cast<bulkio::InULongPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataLong")) {
            bulkio::InLongPort *ptr = dynamic_cast<bulkio::InLongPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataFloat")) {
            bulkio::InFloatPort *ptr = dynamic_cast<bulkio::InFloatPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataDouble")) {
            bulkio::InDoublePort *ptr = dynamic_cast<bulkio::InDoublePort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
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
                Octet_struct(),
                "Octet",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Ushort,
                Ushort_struct(),
                "Ushort",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Short,
                Short_struct(),
                "Short",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(ULong,
                ULong_struct(),
                "ULong",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Long,
                Long_struct(),
                "Long",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Float,
                Float_struct(),
                "Float",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Double,
                Double_struct(),
                "Double",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Octet_out,
                Octet_out_struct(),
                "Octet_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Ushort_out,
                Ushort_out_struct(),
                "Ushort_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Short_out,
                Short_out_struct(),
                "Short_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Ulong_out,
                Ulong_out_struct(),
                "Ulong_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Long_out,
                Long_out_struct(),
                "Long_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Float_out,
                Float_out_struct(),
                "Float_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(Double_out,
                Double_out_struct(),
                "Double_out",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}
