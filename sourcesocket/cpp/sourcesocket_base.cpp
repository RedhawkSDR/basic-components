
#include "sourcesocket_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY
    
 	Source: sourcesocket.spd.xml
 	Generated on: Thu Jun 20 09:36:00 EDT 2013
 	REDHAWK IDE
 	Version: 1.8.4
 	Build id: R201305151907

*******************************************************************************************/

/******************************************************************************************

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/
 
sourcesocket_base::sourcesocket_base(const char *uuid, const char *label) :
                                     Resource_impl(uuid, label), serviceThread(0) {
    construct();
}

void sourcesocket_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    dataOctet_out = new BULKIO_dataOctet_Out_i("dataOctet_out", this);
    oid = ossie::corba::RootPOA()->activate_object(dataOctet_out);

    registerOutPort(dataOctet_out, dataOctet_out->_this());
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void sourcesocket_base::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
}

void sourcesocket_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    if (serviceThread == 0) {
        serviceThread = new ProcessThread<sourcesocket_base>(this, 0.1);
        serviceThread->start();
    }
    
    if (!Resource_impl::started()) {
    	Resource_impl::start();
    }
}

void sourcesocket_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    // release the child thread (if it exists)
    if (serviceThread != 0) {
        if (!serviceThread->release(2)) {
            throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
        }
        serviceThread = 0;
    }
    
    if (Resource_impl::started()) {
    	Resource_impl::stop();
    }
}

CORBA::Object_ptr sourcesocket_base::getPort(const char* _id) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{

    std::map<std::string, Port_Provides_base_impl *>::iterator p_in = inPorts.find(std::string(_id));
    if (p_in != inPorts.end()) {

    }

    std::map<std::string, CF::Port_var>::iterator p_out = outPorts_var.find(std::string(_id));
    if (p_out != outPorts_var.end()) {
        return CF::Port::_duplicate(p_out->second);
    }

    throw (CF::PortSupplier::UnknownPort());
}

void sourcesocket_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
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

    delete(dataOctet_out);
 
    Resource_impl::releaseObject();
}

void sourcesocket_base::loadProperties()
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

    addProperty(max_bytes,
                16384, 
               "max_bytes",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(min_bytes,
                16384, 
               "min_bytes",
               "",
               "readwrite",
               "",
               "external",
               "configure");

    addProperty(sri,
               "sri",
               "",
               "readwrite",
               "",
               "external",
               "configure");

}
