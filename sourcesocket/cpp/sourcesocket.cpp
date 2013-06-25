
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

template<typename T, typename U>
void sourcesocket_i::pushData(T* port, char* start, size_t numBytes)
{
	assert(numBytes%sizeof(U)==0);
	std::string streamID(theSri.streamID._ptr);
	std::vector<U> output(numBytes/sizeof(U));
	memcpy(&output[0], start, numBytes);
	now(tstamp_);
	port->pushPacket(output, tstamp_, false, streamID);
}


int sourcesocket_i::serviceFunction()
{
	LOG_DEBUG(sourcesocket_i, "serviceFunction() example log message");
	//cash off max_bytes & min_bytes in case their properties are updated mid service function
	size_t multSize= sizeof(double);
	unsigned int maxBytes = max_bytes- max_bytes%multSize;
	unsigned int minBytes = (min_bytes+multSize-1)/multSize;
	std::string streamID(theSri.streamID._ptr);

	//send out data if we have more than we should
	//loop until we have less than max_bytes left
	//this should only be called if max_bytes was DECREASED since last loop
	if (data_.size() >= maxBytes)
	{
	    size_t numLoops = data_.size()/maxBytes;
		for(size_t i=0; i!=numLoops; i++)
		{
			pushData<BULKIO_dataOctet_Out_i, unsigned char>(dataOctet_out,&data_[i*maxBytes], maxBytes);
			pushData<BULKIO_dataChar_Out_i, char>(dataChar_out, &data_[i*maxBytes], maxBytes);
			pushData<BULKIO_dataUshort_Out_i, unsigned short>(dataUshort_out, &data_[i*maxBytes], maxBytes);
			pushData<BULKIO_dataShort_Out_i, short>(dataShort_out, &data_[i*maxBytes], maxBytes);
			pushData<BULKIO_dataUlong_Out_i, unsigned int>(dataUlong_out, &data_[i*maxBytes], maxBytes);
			pushData<BULKIO_dataLong_Out_i, int> (dataLong_out, &data_[i*maxBytes], maxBytes);
			pushData<BULKIO_dataDouble_Out_i, double>(dataDouble_out,&data_[i*maxBytes], maxBytes);
			pushData<BULKIO_dataFloat_Out_i, float>(dataFloat_out, &data_[i*maxBytes], maxBytes);
		}
		data_.erase(data_.begin(), data_.begin()+numLoops*maxBytes);
	}

	int startIndex=data_.size();
	// resize the data vector to grab data from the socket
	data_.resize(max_bytes);

	boost::recursive_mutex::scoped_lock lock(socketLock_);

	if (server_==NULL && client_==NULL)
		updateSocket("");

	if (server_)
	{
		if (server_->is_connected())
		{
			status = "connected";
			server_->read(data_,startIndex);
		}
		else
		{
			status = "disconnected";
			data_.resize(startIndex);
		}
	}
	else if (client_)
	{
		if (client_->connect_if_necessary())
		{
			status = "connected";
			client_->read(data_,startIndex);
		}
		else
		{
			data_.resize(startIndex);
			status = "disconnected";
		}
	}
	else
	{
		status="error";
		LOG_ERROR(sourcesocket_i, "no server or client initialized ");
	}
	int numRead = data_.size()-startIndex;

	std::stringstream ss;
	ss<<"Receveived " << numRead<< " bytes - max size = "<<max_bytes;
	LOG_DEBUG(sourcesocket_i, ss.str())

	bytes_per_sec = stats_.newPacket(numRead);
	total_bytes+=numRead;

	if (! data_.empty() && data_.size() >= minBytes)
	{
		size_t numLeft = data_.size()%multSize;
		size_t pushBytes = data_.size() - numLeft;

		pushData<BULKIO_dataOctet_Out_i, unsigned char>(dataOctet_out,&data_[0], pushBytes);
		pushData<BULKIO_dataChar_Out_i, char>(dataChar_out, &data_[0], pushBytes);
		pushData<BULKIO_dataUshort_Out_i, unsigned short>(dataUshort_out, &data_[0], pushBytes);
		pushData<BULKIO_dataShort_Out_i, short>(dataShort_out, &data_[0], pushBytes);
		pushData<BULKIO_dataUlong_Out_i, unsigned int>(dataUlong_out, &data_[0], pushBytes);
		pushData<BULKIO_dataLong_Out_i, int> (dataLong_out, &data_[0], pushBytes);
		pushData<BULKIO_dataDouble_Out_i, double>(dataDouble_out,&data_[0], pushBytes);
		pushData<BULKIO_dataFloat_Out_i, float>(dataFloat_out, &data_[0], pushBytes);

		data_.erase(data_.begin(), data_.end()-numLeft);
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
