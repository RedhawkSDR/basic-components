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
	friend class BULKIO_dataOctet_Out_ii;
	sourcesocket_i(const char *uuid, const char *label);
	~sourcesocket_i();
	int serviceFunction();

private:
	void sriChanged(const std::string&);
	void updateSocket(const std::string&);

	BULKIO::StreamSRI theSri;
	server* server_;
	client* client_;
	QuickStats stats_;
	std::vector<unsigned char> data;
	boost::recursive_mutex socketLock_;
};

#endif
