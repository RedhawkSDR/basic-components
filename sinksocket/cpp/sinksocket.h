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
