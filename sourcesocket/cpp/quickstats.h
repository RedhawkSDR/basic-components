/*
 * quickstats.h
 *
 *  Created on: Jun 19, 2013
 *      Author: bsg
 */

#ifndef QUICKSTATS_H_
#define QUICKSTATS_H_

#include <list>
#include <sys/time.h>

class QuickStats
{
public:
	QuickStats(size_t max_size=64*1024, float max_time=60.0, unsigned long max_bytes=64*1024*1024):
		maxSize(max_size),
		maxTime(max_time),
		maxBytes(max_bytes),
		totalBytes(0)
	{}

	float newPacket(size_t pktSize)
	{
		timeval now;
		gettimeofday ( &now,NULL);
		totalBytes+=pktSize;
		times.push_back(now);
		packetSizes.push_back(pktSize);
		if (times.size()>1)
		{
			timeval& then = times.front();
			float delT = now.tv_sec - then.tv_sec+(now.tv_usec - then.tv_usec)/1000000.0;
			//std::cout<<"QuickStats delT = "<<delT<<" totalBytes " <<totalBytes <<" times.size() = "<<times.size()<<std::endl;
			if (delT > maxTime || times.size()>maxSize || totalBytes>maxBytes)
			{
				totalBytes-=packetSizes.front();
				times.pop_front();
				packetSizes.pop_front();
			}
			return totalBytes/delT;
		}
		else
			return 0.0;
	}
private:
	const size_t maxSize;
	const float maxTime;
	const unsigned long maxBytes;
	std::list<size_t> packetSizes;
	std::list<timeval> times;
	unsigned long totalBytes;

};

#endif /* QUICKSTATS_H_ */
