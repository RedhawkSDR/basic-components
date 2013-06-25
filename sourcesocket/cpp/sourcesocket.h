#ifndef SOURCESOCKET_IMPL_H
#define SOURCESOCKET_IMPL_H

#include "sourcesocket_base.h"
#include "port_impl.h"
#include "BoostServer.h"
#include "BoostClient.h"
#include "quickstats.h"

class sourcesocket_i;


class sourcesocket_i : public sourcesocket_base
{
	ENABLE_LOGGING
public:
	sourcesocket_i(const char *uuid, const char *label);
	~sourcesocket_i();
	int serviceFunction();
	template<typename T, typename U>
	void pushData(T* port, char* start, size_t numBytes);


private:
	void sriChanged(const std::string&);
	void updateSocket(const std::string&);

	BULKIO::StreamSRI theSri;
	server* server_;
	client* client_;
	QuickStats stats_;
	std::vector<char> data_;
	boost::recursive_mutex socketLock_;
	BULKIO::PrecisionUTCTime tstamp_;
};

#endif
