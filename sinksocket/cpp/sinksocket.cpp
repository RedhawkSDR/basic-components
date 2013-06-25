
/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

 	Source: sinksocket.spd.xml
 	Generated on: Mon Jun 10 13:10:38 EDT 2013
 	REDHAWK IDE
 	Version: 1.8.4
 	Build id: R201305151907

 **************************************************************************/

#include "sinksocket.h"
#include <sstream>

PREPARE_LOGGING(sinksocket_i)

sinksocket_i::sinksocket_i(const char *uuid, const char *label) : 
sinksocket_base(uuid, label),
server_(NULL),
client_(NULL)
{
	setPropertyChangeListener("connection_type", this, &sinksocket_i::updateSocket);
	setPropertyChangeListener("ip_address", this, &sinksocket_i::updateSocket);
	setPropertyChangeListener("port", this, &sinksocket_i::updateSocket);
	status = "initialize";
	total_bytes=0;
	bytes_per_sec=0;
}

sinksocket_i::~sinksocket_i()
{
	boost::recursive_mutex::scoped_lock lock(socketLock_);
	status = "deleted";
	if (server_)
		delete server_;
	if (client_)
		delete client_;
}


int sinksocket_i::serviceFunction()
{
  int ret = 0;
  ret = ret || serviceFunctionT(dataOctet_in);
  ret = ret || serviceFunctionT(dataChar_in);
  ret = ret || serviceFunctionT(dataShort_in);
  ret = ret || serviceFunctionT(dataUshort_in);
  ret = ret || serviceFunctionT(dataLong_in);
  ret = ret || serviceFunctionT(dataUlong_in);
  ret = ret || serviceFunctionT(dataFloat_in);
  ret = ret || serviceFunctionT(dataDouble_in);
  return ret;
}
template<typename T>
int sinksocket_i::serviceFunctionT(T* inputPort)
{
	LOG_DEBUG(sinksocket_i, "serviceFunction() example log message");
	typename T::dataTransfer *tmp=NULL;

	boost::recursive_mutex::scoped_lock lock(socketLock_);
	if (server_==NULL && client_==NULL)
		updateSocket("");
	if (server_)
	{
		if (server_->is_connected())
		{
			status = "connected";
			tmp = inputPort->getPacket(0.0);
			if (tmp)
			{
				//std::cerr<< "server write data -- num elements = "<<tmp->dataBuffer.size()<<std::endl;
				server_->write(tmp->dataBuffer);
				//std::cerr<< "server write data DONE -- num elements = "<<tmp->dataBuffer.size()<<std::endl;
			}
		}
		else
			status = "disconnected";
	}
	else if (client_)
	{
		if (client_->connect_if_necessary())
		{
			status = "connected";
			tmp = inputPort->getPacket(0.0);
			//LOG_INFO(sinksocket_i, "sink socket try get data");
			if (tmp)
			{
				//LOG_INFO(sinksocket_i, "sink socket got data");
				//std::cerr<< "num elements = "<<tmp->dataBuffer.size()<<std::endl;
				client_->write(tmp->dataBuffer);
				//LOG_INFO(sinksocket_i, "sink socket client wrote data");
			}
		}
		else
			status = "disconnected";
	}
	else
	{
		status="error";
		LOG_ERROR(sinksocket_i, "no server or client initialized");
	}
	size_t pktSize=0;
	if (tmp && status=="connected")
		pktSize=tmp->dataBuffer.size()*sizeof(tmp->dataBuffer[0]);

	std::stringstream ss;
	ss<<"Sent " << pktSize<< " bytes";
	LOG_DEBUG(sinksocket_i, ss.str());

	bytes_per_sec = stats_.newPacket(pktSize);
	total_bytes+=pktSize;

	if (tmp)
	{
		delete tmp;
		return NORMAL;
	}
	else
		return NOOP;
}
void sinksocket_i::updateSocket(const std::string& id)
{
	boost::recursive_mutex::scoped_lock lock(socketLock_);
	if (client_)
	{
		delete client_;
		client_=NULL;
	}
	if (server_)
	{
		delete server_;
		server_=NULL;
	}

	if (connection_type=="server" && port > 0)
	{
		try
		{
			server_ = new server(port);
			if (server_->is_connected())
				status = "connected";
			else
				status = "disconnected";
		}
		catch (std::exception& e)
		{
			if (server_)
			{
				delete server_;
				server_=NULL;
			}
			LOG_ERROR(sinksocket_i, "error starting server " +std::string(e.what()));
		}
		std::stringstream ss;
		ss<<"set as SERVER :";
		ss<<port;
		LOG_INFO(sinksocket_i, ss.str())
	}
	else if (connection_type=="client" && port > 0 && !ip_address.empty())
	{
		try
		{
			client_ = new client(port, ip_address);
			bool connectionStatus = client_->connect();
			if (connectionStatus)
				status = "connected";
			else
				status = "disconnected";
			std::stringstream ss;
			ss<<"set as CLIENT " + ip_address + ":";
			ss<<port;
			LOG_INFO(sinksocket_i, ss.str())
		}
		catch (std::exception& e)
		{
			LOG_ERROR(sinksocket_i, "error starting client " +std::string(e.what()));
		}

	}
	else
	{
		std::stringstream ss;
		ss<<"Bad connection parameters - " + connection_type + " " + ip_address + ":";
		ss<<port;
		LOG_ERROR(sinksocket_i, ss.str());
		status = "disconnected";
	}
}
