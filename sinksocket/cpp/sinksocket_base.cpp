
#include "sinksocket_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY
    
 	Source: sinksocket.spd.xml
 	Generated on: Thu Jun 20 09:43:15 EDT 2013
 	REDHAWK IDE
 	Version: 1.8.4
 	Build id: R201305151907

*******************************************************************************************/

/******************************************************************************************

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/
 
sinksocket_base::sinksocket_base(const char *uuid, const char *label) :
                                     Resource_impl(uuid, label), serviceThread(0) {
    construct();
}

void sinksocket_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    dataOctet_in = new BULKIO_dataOctet_In_i("dataOctet_in", this);
    oid = ossie::corba::RootPOA()->activate_object(dataOctet_in);

    registerInPort(dataOctet_in);
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
        dataOctet_in->unblock();
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
            BULKIO_dataOctet_In_i *ptr = dynamic_cast<BULKIO_dataOctet_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataOctet::_duplicate(ptr->_this());
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

}
