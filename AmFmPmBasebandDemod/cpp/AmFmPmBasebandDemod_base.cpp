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

#include "AmFmPmBasebandDemod_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY
    
 	Source: AmFmPmBasebandDemod.spd.xml
 	Generated on: Fri Mar 08 12:26:11 EST 2013
 	Redhawk IDE
 	Version:N.1.8.3
 	Build id: v201302261726

*******************************************************************************************/

/******************************************************************************************

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/
 
AmFmPmBasebandDemod_base::AmFmPmBasebandDemod_base(const char *uuid, const char *label) :
                                     Resource_impl(uuid, label), serviceThread(0) {
    construct();
}

void AmFmPmBasebandDemod_base::construct()
{
    Resource_impl::_started = false;
    loadProperties();
    serviceThread = 0;
    
    PortableServer::ObjectId_var oid;
    dataFloat_In = new BULKIO_dataFloat_In_i("dataFloat_In", this);
    oid = ossie::corba::RootPOA()->activate_object(dataFloat_In);
    pm_dataFloat_out = new BULKIO_dataFloat_Out_i("pm_dataFloat_out", this);
    oid = ossie::corba::RootPOA()->activate_object(pm_dataFloat_out);
    fm_dataFloat_out = new BULKIO_dataFloat_Out_i("fm_dataFloat_out", this);
    oid = ossie::corba::RootPOA()->activate_object(fm_dataFloat_out);
    am_dataFloat_out = new BULKIO_dataFloat_Out_i("am_dataFloat_out", this);
    oid = ossie::corba::RootPOA()->activate_object(am_dataFloat_out);

    registerInPort(dataFloat_In);
    registerOutPort(pm_dataFloat_out, pm_dataFloat_out->_this());
    registerOutPort(fm_dataFloat_out, fm_dataFloat_out->_this());
    registerOutPort(am_dataFloat_out, am_dataFloat_out->_this());
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void AmFmPmBasebandDemod_base::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
}

void AmFmPmBasebandDemod_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    if (serviceThread == 0) {
        dataFloat_In->unblock();
        serviceThread = new ProcessThread<AmFmPmBasebandDemod_base>(this, 0.1);
        serviceThread->start();
    }
    
    if (!Resource_impl::started()) {
    	Resource_impl::start();
    }
}

void AmFmPmBasebandDemod_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    boost::mutex::scoped_lock lock(serviceThreadLock);
    // release the child thread (if it exists)
    if (serviceThread != 0) {
        dataFloat_In->block();
        if (!serviceThread->release(2)) {
            throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
        }
        serviceThread = 0;
    }
    
    if (Resource_impl::started()) {
    	Resource_impl::stop();
    }
}

CORBA::Object_ptr AmFmPmBasebandDemod_base::getPort(const char* _id) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{

    std::map<std::string, Port_Provides_base_impl *>::iterator p_in = inPorts.find(std::string(_id));
    if (p_in != inPorts.end()) {

        if (!strcmp(_id,"dataFloat_In")) {
            BULKIO_dataFloat_In_i *ptr = dynamic_cast<BULKIO_dataFloat_In_i *>(p_in->second);
            if (ptr) {
                return BULKIO::dataFloat::_duplicate(ptr->_this());
            }
        }
    }

    std::map<std::string, CF::Port_var>::iterator p_out = outPorts_var.find(std::string(_id));
    if (p_out != outPorts_var.end()) {
        return CF::Port::_duplicate(p_out->second);
    }

    throw (CF::PortSupplier::UnknownPort());
}

void AmFmPmBasebandDemod_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
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

    delete(dataFloat_In);
    delete(pm_dataFloat_out);
    delete(fm_dataFloat_out);
    delete(am_dataFloat_out);
 
    Resource_impl::releaseObject();
}

void AmFmPmBasebandDemod_base::loadProperties()
{
    addProperty(freqDeviation,
                0.0, 
               "freqDeviation",
               "",
               "readwrite",
               "Hz",
               "external",
               "configure");

    addProperty(squelch,
                -150, 
               "squelch",
               "",
               "readwrite",
               "dB",
               "external",
               "configure");

    addProperty(phaseDeviation,
                1.0, 
               "phaseDeviation",
               "",
               "readwrite",
               "cycles",
               "external",
               "configure");

    addProperty(debug,
                false, 
               "debug",
               "",
               "readwrite",
               "",
               "external",
               "configure");

}
