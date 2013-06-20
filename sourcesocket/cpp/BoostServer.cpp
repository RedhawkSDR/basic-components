/*
 * BoostServer.cpp
 *
 *  Created on: Jun 14, 2013
 *      Author: bsg
 */
#include "BoostServer.h"
#include "port_impl.h"

void session::start()
{
	socket_.async_read_some(boost::asio::buffer(data_, max_length_),
			boost::bind(&session::handle_read, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}

template<typename T>
void session::write(std::vector<unsigned char, T> data)
{
	if (socket_.is_open())
		boost::asio::async_write(socket_,
				boost::asio::buffer(data, data.size()),
				boost::bind(&session::handle_write, this,
						boost::asio::placeholders::error));
}

void session::handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
{
	if (!error)
	{
		data_.resize(bytes_transferred);
		server_->newSessionData(data_);
		data_.resize(max_length_);
		socket_.async_read_some(boost::asio::buffer(data_, max_length_),
				boost::bind(&session::handle_read, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		server_->closeSession(this);
	}
}

void session::handle_write(const boost::system::error_code& error)
{
	if (error)
	{
		server_->closeSession(this);
	}
}


template<typename T>
void server::write(std::vector<unsigned char, T>& data)
{
	boost::mutex::scoped_lock lock(sessionsLock_);
	for (std::list<session*>::iterator i = sessions_.begin(); i!=sessions_.end(); i++)
	{
		session* thisSession= *i;
		thisSession->write(data);
	}
}
template<typename T>
void server::read(std::vector<unsigned char, T> & data, size_t index)
{
	boost::mutex::scoped_lock lock(pendingDataLock_);
	int numRead=std::min(data.size()-index, pendingData_.size());
	data.resize(index+numRead);
	int j=0;
	for (unsigned int i=index; i!=data.size(); i++)
	{
		data[i]=pendingData_[j];
		j++;
	}
	pendingData_.erase(pendingData_.begin(), pendingData_.begin()+numRead);
}

bool server::is_connected()
{
	return !sessions_.empty();
}

template<typename T>
void server::newSessionData(std::vector<char, T>& data)
{
	boost::mutex::scoped_lock lock(pendingDataLock_);
	int oldSize=pendingData_.size();
	pendingData_.resize(oldSize+data.size());
	unsigned char* newData= reinterpret_cast<unsigned char *>(&data[0]);
	for (unsigned int i=oldSize; i!=pendingData_.size(); i++)
	{
		pendingData_[i]=*newData;
		newData++;
	}
}
void server::closeSession(session* ptr)
{
	boost::mutex::scoped_lock lock(sessionsLock_);
	for (std::list<session*>::iterator i=sessions_.begin(); i!=sessions_.end(); i++)
	{
		if (ptr==*i)
		{
			sessions_.remove(ptr);
			delete ptr;
			break;
		}
	}
}


void server::start_accept()
{
	{
		boost::mutex::scoped_lock lock(pendingLock_);
		assert(pending_==NULL);
		pending_ = new session(io_service_, this, maxLength_);

		acceptor_.async_accept(pending_->socket(),
				boost::bind(&server::handle_accept, this, pending_,
						boost::asio::placeholders::error));
	}
}

void server::handle_accept(session* new_session,
		const boost::system::error_code& error)
{
	if (pending_)
	{
		{
			assert(new_session==pending_);

			boost::mutex::scoped_lock lock(pendingLock_);
			if (!error)
			{
				{
					boost::mutex::scoped_lock lock(sessionsLock_);
					sessions_.push_back(new_session);
				}
				new_session->start();
			}
			else
			{
				delete new_session;
			}
			pending_=NULL;
		}
		start_accept();
	}
}

void server::run()
{
	try
	{
		io_service_.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
		std::exit(1);
	}
}

//need to put these bad boys in here for templates or you get undefined references when linking ...grr...
template void server::write(std::vector<unsigned char, std::allocator<unsigned char> >&);
template void server::read(std::vector<unsigned char, std::allocator<unsigned char> >&, size_t);
template void server::write(std::vector<unsigned char, _seqVector::seqVectorAllocator<unsigned char> >&);
template void server::read(std::vector<unsigned char, _seqVector::seqVectorAllocator<unsigned char> >&, size_t);

