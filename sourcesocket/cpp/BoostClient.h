/*
 * BoostClient.h
 *
 *  Created on: Jun 11, 2013
 *      Author: bsg
 */

#ifndef BOOSTCLIENT_H_
#define BOOSTCLIENT_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <sstream>

using boost::asio::ip::tcp;

class client
{
public:
	client(unsigned short port, std::string ip_addr) :
		s_(io_service_),
		port_(port),
		ip_addr_(ip_addr)
	{}

	bool connect()
	{
		try
		{
			tcp::resolver resolver(io_service_);
			std::stringstream ss;
			ss<<port_;
			tcp::resolver::query query(ip_addr_, ss.str());
			tcp::resolver::iterator iter = resolver.resolve(query);
			s_.connect(*iter);
			return is_connected();
		}
		catch (...)
		{
			s_.close();
			return false;
		}
	}

	bool connect_if_necessary()
	{
		if (is_connected())
			return true;
		return connect();
	}

	bool is_connected()
	{
		return s_.is_open();
	}

	template<typename T>
	void write(std::vector<unsigned char, T>& data)
	{
		size_t numWritten=0;
		boost::system::error_code ec;
		if (connect_if_necessary())
		{
			while (numWritten!= data.size())
			{
				numWritten+= boost::asio::write(s_, boost::asio::buffer(&data[numWritten], data.size()-numWritten),boost::asio::transfer_all(), ec);
				if (ec)
				{
					s_.close();
					break;
				}
			}
		}
	}
	template<typename T>
	void read(std::vector<unsigned char, T> & data, size_t index=0)
	{
		int bytesReceived=0;
		if (connect_if_necessary() && s_.available()!=0)
		{
			bytesReceived = s_.read_some(boost::asio::buffer(&data[index], data.size()-index));
		}
		data.resize(index+bytesReceived);
	}
private:
	boost::asio::io_service io_service_;
	tcp::socket s_;
	unsigned short port_;
	std::string ip_addr_;

};


#endif /* BOOSTCLIENT_H_ */
