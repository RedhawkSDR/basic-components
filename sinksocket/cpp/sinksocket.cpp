
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
#include "vectorswap.h"
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
  warn_.clear();
  ret += serviceFunctionT(dataOctet_in);
  ret += serviceFunctionT(dataChar_in);
  ret += serviceFunctionT(dataShort_in);
  ret += serviceFunctionT(dataUshort_in);
  ret += serviceFunctionT(dataLong_in);
  ret += serviceFunctionT(dataUlong_in);
  ret += serviceFunctionT(dataFloat_in);
  ret += serviceFunctionT(dataDouble_in);
  if (ret > 1)
  {
	  LOG_WARN(sinksocket_i, "More than one data port received data.  " +  warn_.str());
  	  return NORMAL;
  }
  return ret;
}


template<typename T, typename U>
void sinksocket_i::newData(std::vector<T, U>& newData)
{
	unsigned int numSwap = byte_swap;
	size_t dataSize = sizeof(T);
	//if 1 is requested - do the word size associated with the data
	if (numSwap==1)
		numSwap = dataSize;

	size_t numBytes =newData.size()*dataSize;
	size_t oldSize = leftover_.size();
	size_t totalSize = numBytes+oldSize;

	size_t newLeftoverSize;
	//make sure you send an exact mutlple of numSwap if its greater than 0
	if (numSwap > 1)
	{
		newLeftoverSize = totalSize %(numSwap);
		if (numSwap !=dataSize)
		{
			std::stringstream ss;
			ss<<"data size "<<dataSize<<" is not equal to byte swap size "<< numSwap<<". ";
			LOG_WARN(sinksocket_i, ss.str());
		}
	}
	else
		newLeftoverSize = 0;

	if (newLeftoverSize ==0 && oldSize==0)
	{
		//don't have to deal with leftover data -- this should be the typical case
		if (numSwap>1)
			vectorSwap(newData, numSwap);
		sendData(newData);
	}
	else
	{
		LOG_WARN(sinksocket_i, "Byte swapping and packet sizes are not compatible.  Swapping bytes over adjacent packets");
		//copy the right ammount of data into leftover_
		size_t outSize =totalSize - newLeftoverSize;
		size_t numCopy =outSize-oldSize;
		leftover_.resize(outSize);
		memcpy(&leftover_[oldSize], &newData[0], numCopy);
		if (numSwap>1)
			vectorSwap(leftover_, numSwap);
		//send the leftover
		sendData(leftover_);
		//if we have new leftover - populate it now
		if (newLeftoverSize!=0)
		{
			leftover_.resize(newLeftoverSize);
			memcpy(&leftover_[0], reinterpret_cast<char*>(&newData[0])+numCopy, newLeftoverSize);
		}
		else
			leftover_.clear();
	}
}

template<typename T, typename U>
void sinksocket_i::sendData(std::vector<T, U>& outData)
{
	//we should only get into this loop if we are already connected
	bool sentData=true;
	if (server_ && server_->is_connected())
		server_->write(outData);
	else if (client_ && client_->connect_if_necessary())
		client_->write(outData);
	else
		sentData=false;
	if (sentData)
	{
		size_t pktSize=outData.size()*sizeof(T);

		std::stringstream ss;
		ss<<"Sent " << pktSize<< " bytes";
		LOG_DEBUG(sinksocket_i, ss.str());

		bytes_per_sec = stats_.newPacket(pktSize);
		total_bytes+=pktSize;
	}
	else
		LOG_ERROR(sinksocket_i, "server and client are both not ready.  Let the data on the floor -- let the data hit the floor");


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
				newData(tmp->dataBuffer);
				warn_<<"Got data from "<<inputPort->getName()<<".  ";
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
				newData(tmp->dataBuffer);
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
