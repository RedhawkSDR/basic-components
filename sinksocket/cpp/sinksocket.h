#ifndef SINKSOCKET_IMPL_H
#define SINKSOCKET_IMPL_H

#include "sinksocket_base.h"
#include "BoostClient.h"
#include "BoostServer.h"
#include "quickstats.h"
#include <vector>

class sinksocket_i;

class sinksocket_i : public sinksocket_base
{
	ENABLE_LOGGING
public:
	sinksocket_i(const char *uuid, const char *label);
	~sinksocket_i();
	int serviceFunction();
	template<typename T>
	int serviceFunctionT(T* inputPort);
private:

	void updateSocket(const std::string&);

	template<typename T, typename U>
	void sendData(std::vector<T, U>& outData);

	template<typename T, typename U>
	void newData(std::vector<T, U>& newData);

	std::vector<char> leftover_;

	server* server_;
	client* client_;
	QuickStats stats_;
	boost::recursive_mutex socketLock_;
	std::stringstream warn_;
};

#endif
