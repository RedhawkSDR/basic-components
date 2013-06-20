
/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

 	Source: sourcesocket.spd.xml
 	Generated on: Mon Jun 10 16:49:00 EDT 2013
 	REDHAWK IDE
 	Version: 1.8.4
 	Build id: R201305151907

 **************************************************************************/

#include "sourcesocket.h"
#include <sstream>
void now(BULKIO::PrecisionUTCTime& tstamp)
{
	struct timeval tmp_time;
	struct timezone tmp_tz;
	gettimeofday(&tmp_time, &tmp_tz);
	double wsec = tmp_time.tv_sec;
	double fsec = tmp_time.tv_usec / 1e6;;
	tstamp.tcmode = BULKIO::TCM_CPU;
	tstamp.tcstatus = (short)1;
	tstamp.toff = 0.0;
	tstamp.twsec = wsec;
	tstamp.tfsec = fsec;
}

PREPARE_LOGGING(sourcesocket_i)

sourcesocket_i::sourcesocket_i(const char *uuid, const char *label) : 
sourcesocket_base(uuid, label),
server_(NULL),
client_(NULL)
{
	theSri.hversion = 1;
	theSri.xunits = BULKIO::UNITS_TIME;
	theSri.subsize = 0;
	theSri.ystart = 0.0;
	theSri.ydelta = 0.0;
	theSri.yunits = BULKIO::UNITS_NONE;
	setPropertyChangeListener("sri", this, &sourcesocket_i::sriChanged);
	setPropertyChangeListener("connection_type", this, &sourcesocket_i::updateSocket);
	setPropertyChangeListener("ip_address", this, &sourcesocket_i::updateSocket);
	setPropertyChangeListener("port", this, &sourcesocket_i::updateSocket);
	setPropertyChangeListener("max_bytes", this, &sourcesocket_i::updateSocket);
	sriChanged("");
	status = "initialize";
	total_bytes=0;
	bytes_per_sec=0;
}

sourcesocket_i::~sourcesocket_i()
{
	boost::recursive_mutex::scoped_lock lock(socketLock_);
	status = "deleted";
	if (server_)
		delete server_;
	if (client_)
		delete client_;

}
int sourcesocket_i::serviceFunction()
{
	LOG_DEBUG(sourcesocket_i, "serviceFunction() example log message");
	//cash off max_bytes & min_bytes in case their properties are updated mid service function
	unsigned int maxBytes = max_bytes;
	unsigned int minBytes = min_bytes;
	std::string streamID(theSri.streamID._ptr);
	BULKIO::PrecisionUTCTime tstamp = BULKIO::PrecisionUTCTime();

	//send out data if we have more than we should
	//loop until we have less than max_bytes left
	//this should only be called if max_bytes was DECREASED since last loop
	if (data.size() >= maxBytes)
	{
		std::vector<unsigned char>::iterator i = data.begin();
		std::vector<unsigned char>::iterator end;
		std::vector<unsigned char> out;

		while (data.end() -i >= maxBytes)
		{
			end=i+max_bytes;
			out.assign(i, end);
			now(tstamp);
			dataOctet_out->pushPacket(out, tstamp, false, streamID);
			i=end;
		}
		data.erase(data.begin(), i);
	}

	int startIndex=data.size();
	// resize the data vector to grab data from the socket
	data.resize(max_bytes);

	boost::recursive_mutex::scoped_lock lock(socketLock_);

	if (server_==NULL && client_==NULL)
		updateSocket("");

	if (server_)
	{
		if (server_->is_connected())
		{
			status = "connected";
			server_->read(data,startIndex);
		}
		else
		{
			status = "disconnected";
			data.resize(startIndex);
		}
	}
	else if (client_)
	{
		if (client_->connect_if_necessary())
		{
			status = "connected";
			client_->read(data,startIndex);
		}
		else
		{
			data.resize(startIndex);
			status = "disconnected";
		}
	}
	else
	{
		status="error";
		LOG_ERROR(sourcesocket_i, "no server or client initialized ");
	}
	int numRead = data.size()-startIndex;

	std::stringstream ss;
	ss<<"Receveived " << numRead<< " bytes - max size = "<<max_bytes;
	LOG_DEBUG(sourcesocket_i, ss.str())

	bytes_per_sec = stats_.newPacket(numRead);
	total_bytes+=numRead;

	if (! data.empty() && data.size() >= minBytes)
	{
		now(tstamp);
		dataOctet_out->pushPacket(data, tstamp, false, streamID);
		data.clear();
		return NORMAL;
	}
	return NOOP;

}

void sourcesocket_i::sriChanged(const std::string& id)
{
	if (sri.streamID.empty())
		sri.streamID =ossie::generateUUID();
	theSri.hversion = 1;
	theSri.xstart = sri.xstart;
	theSri.xdelta = sri.xdelta;
	theSri.mode = sri.mode;
	theSri.streamID = sri.streamID.c_str();
	theSri.blocking = sri.blocking;
	dataOctet_out->pushSRI(theSri);
}

void sourcesocket_i::updateSocket(const std::string& id)
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
		server_ = NULL;
	}

	if (connection_type=="server" && port > 0)
	{
		try
		{
			server_ = new server(port, max_bytes);
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
			LOG_ERROR(sourcesocket_i, "error starting server " +std::string(e.what()));
			//if (server_)
			//	delete server_;
		}
		std::stringstream ss;
		ss<<"set as SERVER :";
		ss<<port;
		LOG_INFO(sourcesocket_i, ss.str())
	}
	else if (connection_type=="client" && port > 0 && !ip_address.empty())
	{
		try
		{
			client_ = new client(port, ip_address);
			if(client_->connect())
				status = "connected";
			else
				status = "disconnected";
			std::stringstream ss;
			ss<<"set as CLIENT " + ip_address + ":";
			ss<<port;
			LOG_INFO(sourcesocket_i, ss.str())
		}
		catch (std::exception& e)
		{
			LOG_ERROR(sourcesocket_i, "error starting client " +std::string(e.what()));
		}
	}
	else
	{
		std::stringstream ss;
		ss<<"Bad connection parameters - " + connection_type + " " + ip_address + ":";
		ss<<port;
		LOG_ERROR(sourcesocket_i, ss.str());
	}
}
