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
	void pushData(T* port, char* start, size_t numBytes, unsigned int numSwap);


private:
	void sriChanged(const std::string&);
	void updateSocket(const std::string&);
	void updateMaxBytes(const std::string&);
	void updateXferLen(const std::string&);


	BULKIO::StreamSRI theSri;
	server* server_;
	client* client_;
	QuickStats stats_;
	std::vector<char> data_;
	boost::recursive_mutex socketLock_;
	boost::recursive_mutex xferLock_;
	BULKIO::PrecisionUTCTime tstamp_;
	size_t multSize_;
	std::vector<std::string> activePorts_;
};

#endif
