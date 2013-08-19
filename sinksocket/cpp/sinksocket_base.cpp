/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This file is part of REDHAWK Basic Components sinksocket.
 *
 * REDHAWK Basic Components sinksocket is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * REDHAWK Basic Components sinksocket is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this
 * program.  If not, see http://www.gnu.org/licenses/.
 */
#include "sinksocket_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

sinksocket_base::sinksocket_base(const char *uuid, const char *label) :
    Resource_impl(uuid, label),
    serviceThread(0)
{
    construct();
}

void sinksocket_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    dataOctet_in = new bulkio::InOctetPort("dataOctet_in");
    oid = ossie::corba::RootPOA()->activate_object(dataOctet_in);
    dataChar_in = new bulkio::InCharPort("dataChar_in");
    oid = ossie::corba::RootPOA()->activate_object(dataChar_in);
    dataShort_in = new bulkio::InShortPort("dataShort_in");
    oid = ossie::corba::RootPOA()->activate_object(dataShort_in);
    dataUshort_in = new bulkio::InUShortPort("dataUshort_in");
    oid = ossie::corba::RootPOA()->activate_object(dataUshort_in);
    dataLong_in = new bulkio::InLongPort("dataLong_in");
    oid = ossie::corba::RootPOA()->activate_object(dataLong_in);
    dataUlong_in = new bulkio::InULongPort("dataUlong_in");
    oid = ossie::corba::RootPOA()->activate_object(dataUlong_in);
    dataFloat_in = new bulkio::InFloatPort("dataFloat_in");
    oid = ossie::corba::RootPOA()->activate_object(dataFloat_in);
    dataDouble_in = new bulkio::InDoublePort("dataDouble_in");
    oid = ossie::corba::RootPOA()->activate_object(dataDouble_in);

    registerInPort(dataOctet_in);
    registerInPort(dataChar_in);
    registerInPort(dataShort_in);
    registerInPort(dataUshort_in);
    registerInPort(dataLong_in);
    registerInPort(dataUlong_in);
    registerInPort(dataFloat_in);
    registerInPort(dataDouble_in);
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void sinksocket_base::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
}

void sinksocket_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    if (serviceThread == 0) {
        serviceThread = new ProcessThread<sinksocket_base>(this, 0.1);
        serviceThread->start();
    }
    
    if (!Resource_impl::started()) {
    	Resource_impl::start();
    }
}

void sinksocket_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    // release the child thread (if it exists)
    if (serviceThread != 0) {
        dataOctet_in->block();
        dataChar_in->block();
        dataShort_in->block();
        dataUshort_in->block();
        dataLong_in->block();
        dataUlong_in->block();
        dataFloat_in->block();
        dataDouble_in->block();
        if (!serviceThread->release(2)) {
            throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
        }
        serviceThread = 0;
    }
    
    if (Resource_impl::started()) {
    	Resource_impl::stop();
    }
}

CORBA::Object_ptr sinksocket_base::getPort(const char* _id) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{

    std::map<std::string, Port_Provides_base_impl *>::iterator p_in = inPorts.find(std::string(_id));
    if (p_in != inPorts.end()) {
        if (!strcmp(_id,"dataOctet_in")) {
            bulkio::InOctetPort *ptr = dynamic_cast<bulkio::InOctetPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataChar_in")) {
            bulkio::InCharPort *ptr = dynamic_cast<bulkio::InCharPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataShort_in")) {
            bulkio::InShortPort *ptr = dynamic_cast<bulkio::InShortPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataUshort_in")) {
            bulkio::InUShortPort *ptr = dynamic_cast<bulkio::InUShortPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataLong_in")) {
            bulkio::InLongPort *ptr = dynamic_cast<bulkio::InLongPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataUlong_in")) {
            bulkio::InULongPort *ptr = dynamic_cast<bulkio::InULongPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataFloat_in")) {
            bulkio::InFloatPort *ptr = dynamic_cast<bulkio::InFloatPort *>(p_in->second);
            if (ptr) {
                return ptr->_this();
            }
        }
        if (!strcmp(_id,"dataDouble_in")) {
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

void sinksocket_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
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

    delete(dataOctet_in);
    delete(dataChar_in);
    delete(dataShort_in);
    delete(dataUshort_in);
    delete(dataLong_in);
    delete(dataUlong_in);
    delete(dataFloat_in);
    delete(dataDouble_in);

    Resource_impl::releaseObject();
}

void sinksocket_base::loadProperties()
{
    addProperty(connection_type,
                "server",
                "connection_type",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(ip_address,
                "ip_address",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(port,
                "port",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(status,
                "status",
                "",
                "readonly",
                "",
                "external",
                "configure");

    addProperty(total_bytes,
                "total_bytes",
                "",
                "readonly",
                "",
                "external",
                "configure");

    addProperty(bytes_per_sec,
                "bytes_per_sec",
                "",
                "readonly",
                "",
                "external",
                "configure");

    addProperty(byte_swap,
                0,
                "byte_swap",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}
