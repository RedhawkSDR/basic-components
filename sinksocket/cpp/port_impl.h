
#ifndef PORT_H
#define PORT_H

#include "ossie/Port_impl.h"
#include <queue>
#include <list>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>

class sinksocket_base;
class sinksocket_i;

#define CORBA_MAX_TRANSFER_BYTES omniORB::giopMaxMsgSize()


#include "BULKIO/bio_dataOctet.h"
#include "BULKIO/bio_dataChar.h"
#include "BULKIO/bio_dataShort.h"
#include "BULKIO/bio_dataUshort.h"
#include "BULKIO/bio_dataLong.h"
#include "BULKIO/bio_dataUlong.h"
#include "BULKIO/bio_dataFloat.h"
#include "BULKIO/bio_dataDouble.h"

class queueSemaphore
{
    private:
        unsigned int maxValue;
        unsigned int currValue;
        boost::mutex mutex;
        boost::condition_variable condition;

    public:
        queueSemaphore(unsigned int initialMaxValue):mutex(),condition() {
        	maxValue = initialMaxValue;
        }

        void setMaxValue(unsigned int newMaxValue) {
            boost::unique_lock<boost::mutex> lock(mutex);
            maxValue = newMaxValue;
        }

        unsigned int getMaxValue(void) {
			boost::unique_lock<boost::mutex> lock(mutex);
			return maxValue;
		}

        void setCurrValue(unsigned int newValue) {
        	boost::unique_lock<boost::mutex> lock(mutex);
        	if (newValue < maxValue) {
        		unsigned int oldValue = currValue;
        		currValue = newValue;

        		if (oldValue > newValue) {
        			condition.notify_one();
        		}
        	}
        }

        void incr() {
            boost::unique_lock<boost::mutex> lock(mutex);
            while (currValue >= maxValue) {
                condition.wait(lock);
            }
            ++currValue;
        }

        void decr() {
            boost::unique_lock<boost::mutex> lock(mutex);
            if (currValue > 0) {
            	--currValue;
            }
            condition.notify_one();
        }
};        



// ----------------------------------------------------------------------------------------
// BULKIO_dataUshort_In_i declaration
// ----------------------------------------------------------------------------------------
class BULKIO_dataUshort_In_i : public POA_BULKIO::dataUshort, public Port_Provides_base_impl
{
    public:
        BULKIO_dataUshort_In_i(std::string port_name, sinksocket_base *_parent);
        ~BULKIO_dataUshort_In_i();

        void pushSRI(const BULKIO::StreamSRI& H);
        void pushPacket(const PortTypes::UshortSequence& data, const BULKIO::PrecisionUTCTime& T, CORBA::Boolean EOS, const char* streamID);

        BULKIO::PortUsageType state();
        BULKIO::PortStatistics* statistics();
        BULKIO::StreamSRISequence* activeSRIs();
        int getCurrentQueueDepth();
        int getMaxQueueDepth();
        void setMaxQueueDepth(int newDepth);

        class linkStatistics
        {
            public:
                struct statPoint {
                    unsigned int elements;
                    float queueSize;
                    double secs;
                    double usecs;
                };

                linkStatistics() {
                    bitSize = sizeof(CORBA::UShort) * 8.0;
                    historyWindow = 10;
                    receivedStatistics_idx = 0;
                    receivedStatistics.resize(historyWindow);
                    activeStreamIDs.resize(0);
                    runningStats.elementsPerSecond = -1.0;
                    runningStats.bitsPerSecond = -1.0;
                    runningStats.callsPerSecond = -1.0;
                    runningStats.averageQueueDepth = -1.0;
                    runningStats.streamIDs.length(0);
                    runningStats.timeSinceLastCall = -1;
                    enabled = true;
                    flush_sec = 0;
                    flush_usec = 0;
                };

                ~linkStatistics() {
                }

                void setEnabled(bool enableStats) {
                    enabled = enableStats;
                }

                void update(unsigned int elementsReceived, float queueSize, bool EOS, std::string streamID, bool flush) {
                    if (!enabled) {
                        return;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);
                    receivedStatistics[receivedStatistics_idx].elements = elementsReceived;
                    receivedStatistics[receivedStatistics_idx].queueSize = queueSize;
                    receivedStatistics[receivedStatistics_idx].secs = tv.tv_sec;
                    receivedStatistics[receivedStatistics_idx++].usecs = tv.tv_usec;
                    receivedStatistics_idx = receivedStatistics_idx % historyWindow;
                    if (flush) {
                        flush_sec = tv.tv_sec;
                        flush_usec = tv.tv_usec;
                    }
                    if (!EOS) {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        bool foundStreamID = false;
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                foundStreamID = true;
                                break;
                            }
                            p++;
                        }
                        if (!foundStreamID) {
                            activeStreamIDs.push_back(streamID);
                        }
                    } else {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                activeStreamIDs.erase(p);
                                break;
                            }
                            p++;
                        }
                    }
                }

                BULKIO::PortStatistics retrieve() {
                    if (!enabled) {
                        return runningStats;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);

                    int idx = (receivedStatistics_idx == 0) ? (historyWindow - 1) : (receivedStatistics_idx - 1);
                    double front_sec = receivedStatistics[idx].secs;
                    double front_usec = receivedStatistics[idx].usecs;
                    double secDiff = tv.tv_sec - receivedStatistics[receivedStatistics_idx].secs;
                    double usecDiff = (tv.tv_usec - receivedStatistics[receivedStatistics_idx].usecs) / ((double)1e6);
                    double totalTime = secDiff + usecDiff;
                    double totalData = 0;
                    float queueSize = 0;
                    int startIdx = (receivedStatistics_idx + 1) % historyWindow;
                    for (int i = startIdx; i != receivedStatistics_idx; ) {
                        totalData += receivedStatistics[i].elements;
                        queueSize += receivedStatistics[i].queueSize;
                        i = (i + 1) % historyWindow;
                    }
                    runningStats.bitsPerSecond = ((totalData * bitSize) / totalTime);
                    runningStats.elementsPerSecond = (totalData / totalTime);
                    runningStats.averageQueueDepth = (queueSize / historyWindow);
                    runningStats.callsPerSecond = (double(historyWindow - 1) / totalTime);
                    runningStats.timeSinceLastCall = (((double)tv.tv_sec) - front_sec) + (((double)tv.tv_usec - front_usec) / ((double)1e6));
                    unsigned int streamIDsize = activeStreamIDs.size();
                    std::list< std::string >::iterator p = activeStreamIDs.begin();
                    runningStats.streamIDs.length(streamIDsize);
                    for (unsigned int i = 0; i < streamIDsize; i++) {
                        if (p == activeStreamIDs.end()) {
                            break;
                        }
                        runningStats.streamIDs[i] = CORBA::string_dup((*p).c_str());
                        p++;
                    }
                    if ((flush_sec != 0) && (flush_usec != 0)) {
                        double flushTotalTime = (((double)tv.tv_sec) - flush_sec) + (((double)tv.tv_usec - flush_usec) / ((double)1e6));
                        runningStats.keywords.length(1);
                        runningStats.keywords[0].id = CORBA::string_dup("timeSinceLastFlush");
                        runningStats.keywords[0].value <<= CORBA::Double(flushTotalTime);
                    }
                    return runningStats;
                }

            protected:
                bool enabled;
                double bitSize;
                BULKIO::PortStatistics runningStats;
                std::vector<statPoint> receivedStatistics;
                std::list< std::string > activeStreamIDs;
                unsigned long historyWindow;
                long receivedStatistics_idx;
                double flush_sec;
                double flush_usec;
        };
        
        void enableStats(bool enable) {
            stats.setEnabled(enable);
        };


        class dataTransfer
        {
            public:
                dataTransfer(const PortTypes::UshortSequence& data, const BULKIO::PrecisionUTCTime &_T, bool _EOS, const char* _streamID, BULKIO::StreamSRI &_H, bool _sriChanged, bool _inputQueueFlushed)
                {
                    int dataLength = data.length();

#ifdef EXPECTED_VECTOR_IMPL
                    std::_Vector_base<CORBA::UShort, _seqVector::seqVectorAllocator<CORBA::UShort> >::_Vector_impl *vectorPointer = (std::_Vector_base<CORBA::UShort, _seqVector::seqVectorAllocator<CORBA::UShort> >::_Vector_impl *) ((void*) & dataBuffer);
                    vectorPointer->_M_start = const_cast<PortTypes::UshortSequence*>(&data)->get_buffer(1);
                    vectorPointer->_M_finish = vectorPointer->_M_start + dataLength;
                    vectorPointer->_M_end_of_storage = vectorPointer->_M_finish;

#else
                    dataBuffer.resize(dataLength);
                    if (dataLength > 0) {
                        memcpy(&dataBuffer[0], &data[0], dataLength * sizeof(data[0]));
                    }

#endif
                    T = _T;
                    EOS = _EOS;
                    streamID = _streamID;
                    SRI = _H;
                    sriChanged = _sriChanged;
                    inputQueueFlushed = _inputQueueFlushed;
                };

#ifdef EXPECTED_VECTOR_IMPL
                std::vector< CORBA::UShort, _seqVector::seqVectorAllocator<CORBA::UShort> > dataBuffer;
#else
                std::vector<CORBA::UShort> dataBuffer;
#endif
                BULKIO::PrecisionUTCTime T;
                bool EOS;
                std::string streamID;
                BULKIO::StreamSRI SRI;
                bool sriChanged;
                bool inputQueueFlushed;
        };

        dataTransfer *getPacket(float timeout);
        void block();
        void unblock();

    protected:
        sinksocket_i *parent;
        std::deque<dataTransfer *> workQueue;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> > currentHs;
        boost::mutex dataBufferLock;
        boost::mutex sriUpdateLock;
        omni_mutex dataAvailableMutex;
        omni_condition* dataAvailable;
        unsigned long secs, nsecs, timeout_secs, timeout_nsecs;
        bool breakBlock;
        bool blocking;
        queueSemaphore* queueSem;

        // statistics
        linkStatistics stats;

};



// ----------------------------------------------------------------------------------------
// BULKIO_dataShort_In_i declaration
// ----------------------------------------------------------------------------------------
class BULKIO_dataShort_In_i : public POA_BULKIO::dataShort, public Port_Provides_base_impl
{
    public:
        BULKIO_dataShort_In_i(std::string port_name, sinksocket_base *_parent);
        ~BULKIO_dataShort_In_i();

        void pushSRI(const BULKIO::StreamSRI& H);
        void pushPacket(const PortTypes::ShortSequence& data, const BULKIO::PrecisionUTCTime& T, CORBA::Boolean EOS, const char* streamID);

        BULKIO::PortUsageType state();
        BULKIO::PortStatistics* statistics();
        BULKIO::StreamSRISequence* activeSRIs();
        int getCurrentQueueDepth();
        int getMaxQueueDepth();
        void setMaxQueueDepth(int newDepth);

        class linkStatistics
        {
            public:
                struct statPoint {
                    unsigned int elements;
                    float queueSize;
                    double secs;
                    double usecs;
                };

                linkStatistics() {
                    bitSize = sizeof(CORBA::Short) * 8.0;
                    historyWindow = 10;
                    receivedStatistics_idx = 0;
                    receivedStatistics.resize(historyWindow);
                    activeStreamIDs.resize(0);
                    runningStats.elementsPerSecond = -1.0;
                    runningStats.bitsPerSecond = -1.0;
                    runningStats.callsPerSecond = -1.0;
                    runningStats.averageQueueDepth = -1.0;
                    runningStats.streamIDs.length(0);
                    runningStats.timeSinceLastCall = -1;
                    enabled = true;
                    flush_sec = 0;
                    flush_usec = 0;
                };

                ~linkStatistics() {
                }

                void setEnabled(bool enableStats) {
                    enabled = enableStats;
                }

                void update(unsigned int elementsReceived, float queueSize, bool EOS, std::string streamID, bool flush) {
                    if (!enabled) {
                        return;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);
                    receivedStatistics[receivedStatistics_idx].elements = elementsReceived;
                    receivedStatistics[receivedStatistics_idx].queueSize = queueSize;
                    receivedStatistics[receivedStatistics_idx].secs = tv.tv_sec;
                    receivedStatistics[receivedStatistics_idx++].usecs = tv.tv_usec;
                    receivedStatistics_idx = receivedStatistics_idx % historyWindow;
                    if (flush) {
                        flush_sec = tv.tv_sec;
                        flush_usec = tv.tv_usec;
                    }
                    if (!EOS) {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        bool foundStreamID = false;
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                foundStreamID = true;
                                break;
                            }
                            p++;
                        }
                        if (!foundStreamID) {
                            activeStreamIDs.push_back(streamID);
                        }
                    } else {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                activeStreamIDs.erase(p);
                                break;
                            }
                            p++;
                        }
                    }
                }

                BULKIO::PortStatistics retrieve() {
                    if (!enabled) {
                        return runningStats;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);

                    int idx = (receivedStatistics_idx == 0) ? (historyWindow - 1) : (receivedStatistics_idx - 1);
                    double front_sec = receivedStatistics[idx].secs;
                    double front_usec = receivedStatistics[idx].usecs;
                    double secDiff = tv.tv_sec - receivedStatistics[receivedStatistics_idx].secs;
                    double usecDiff = (tv.tv_usec - receivedStatistics[receivedStatistics_idx].usecs) / ((double)1e6);
                    double totalTime = secDiff + usecDiff;
                    double totalData = 0;
                    float queueSize = 0;
                    int startIdx = (receivedStatistics_idx + 1) % historyWindow;
                    for (int i = startIdx; i != receivedStatistics_idx; ) {
                        totalData += receivedStatistics[i].elements;
                        queueSize += receivedStatistics[i].queueSize;
                        i = (i + 1) % historyWindow;
                    }
                    runningStats.bitsPerSecond = ((totalData * bitSize) / totalTime);
                    runningStats.elementsPerSecond = (totalData / totalTime);
                    runningStats.averageQueueDepth = (queueSize / historyWindow);
                    runningStats.callsPerSecond = (double(historyWindow - 1) / totalTime);
                    runningStats.timeSinceLastCall = (((double)tv.tv_sec) - front_sec) + (((double)tv.tv_usec - front_usec) / ((double)1e6));
                    unsigned int streamIDsize = activeStreamIDs.size();
                    std::list< std::string >::iterator p = activeStreamIDs.begin();
                    runningStats.streamIDs.length(streamIDsize);
                    for (unsigned int i = 0; i < streamIDsize; i++) {
                        if (p == activeStreamIDs.end()) {
                            break;
                        }
                        runningStats.streamIDs[i] = CORBA::string_dup((*p).c_str());
                        p++;
                    }
                    if ((flush_sec != 0) && (flush_usec != 0)) {
                        double flushTotalTime = (((double)tv.tv_sec) - flush_sec) + (((double)tv.tv_usec - flush_usec) / ((double)1e6));
                        runningStats.keywords.length(1);
                        runningStats.keywords[0].id = CORBA::string_dup("timeSinceLastFlush");
                        runningStats.keywords[0].value <<= CORBA::Double(flushTotalTime);
                    }
                    return runningStats;
                }

            protected:
                bool enabled;
                double bitSize;
                BULKIO::PortStatistics runningStats;
                std::vector<statPoint> receivedStatistics;
                std::list< std::string > activeStreamIDs;
                unsigned long historyWindow;
                long receivedStatistics_idx;
                double flush_sec;
                double flush_usec;
        };
        
        void enableStats(bool enable) {
            stats.setEnabled(enable);
        };


        class dataTransfer
        {
            public:
                dataTransfer(const PortTypes::ShortSequence& data, const BULKIO::PrecisionUTCTime &_T, bool _EOS, const char* _streamID, BULKIO::StreamSRI &_H, bool _sriChanged, bool _inputQueueFlushed)
                {
                    int dataLength = data.length();

#ifdef EXPECTED_VECTOR_IMPL
                    std::_Vector_base<CORBA::Short, _seqVector::seqVectorAllocator<CORBA::Short> >::_Vector_impl *vectorPointer = (std::_Vector_base<CORBA::Short, _seqVector::seqVectorAllocator<CORBA::Short> >::_Vector_impl *) ((void*) & dataBuffer);
                    vectorPointer->_M_start = const_cast<PortTypes::ShortSequence*>(&data)->get_buffer(1);
                    vectorPointer->_M_finish = vectorPointer->_M_start + dataLength;
                    vectorPointer->_M_end_of_storage = vectorPointer->_M_finish;

#else
                    dataBuffer.resize(dataLength);
                    if (dataLength > 0) {
                        memcpy(&dataBuffer[0], &data[0], dataLength * sizeof(data[0]));
                    }

#endif
                    T = _T;
                    EOS = _EOS;
                    streamID = _streamID;
                    SRI = _H;
                    sriChanged = _sriChanged;
                    inputQueueFlushed = _inputQueueFlushed;
                };

#ifdef EXPECTED_VECTOR_IMPL
                std::vector< CORBA::Short, _seqVector::seqVectorAllocator<CORBA::Short> > dataBuffer;
#else
                std::vector<CORBA::Short> dataBuffer;
#endif
                BULKIO::PrecisionUTCTime T;
                bool EOS;
                std::string streamID;
                BULKIO::StreamSRI SRI;
                bool sriChanged;
                bool inputQueueFlushed;
        };

        dataTransfer *getPacket(float timeout);
        void block();
        void unblock();

    protected:
        sinksocket_i *parent;
        std::deque<dataTransfer *> workQueue;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> > currentHs;
        boost::mutex dataBufferLock;
        boost::mutex sriUpdateLock;
        omni_mutex dataAvailableMutex;
        omni_condition* dataAvailable;
        unsigned long secs, nsecs, timeout_secs, timeout_nsecs;
        bool breakBlock;
        bool blocking;
        queueSemaphore* queueSem;

        // statistics
        linkStatistics stats;

};



// ----------------------------------------------------------------------------------------
// BULKIO_dataUlong_In_i declaration
// ----------------------------------------------------------------------------------------
class BULKIO_dataUlong_In_i : public POA_BULKIO::dataUlong, public Port_Provides_base_impl
{
    public:
        BULKIO_dataUlong_In_i(std::string port_name, sinksocket_base *_parent);
        ~BULKIO_dataUlong_In_i();

        void pushSRI(const BULKIO::StreamSRI& H);
        void pushPacket(const PortTypes::UlongSequence& data, const BULKIO::PrecisionUTCTime& T, CORBA::Boolean EOS, const char* streamID);

        BULKIO::PortUsageType state();
        BULKIO::PortStatistics* statistics();
        BULKIO::StreamSRISequence* activeSRIs();
        int getCurrentQueueDepth();
        int getMaxQueueDepth();
        void setMaxQueueDepth(int newDepth);

        class linkStatistics
        {
            public:
                struct statPoint {
                    unsigned int elements;
                    float queueSize;
                    double secs;
                    double usecs;
                };

                linkStatistics() {
                    bitSize = sizeof(CORBA::ULong) * 8.0;
                    historyWindow = 10;
                    receivedStatistics_idx = 0;
                    receivedStatistics.resize(historyWindow);
                    activeStreamIDs.resize(0);
                    runningStats.elementsPerSecond = -1.0;
                    runningStats.bitsPerSecond = -1.0;
                    runningStats.callsPerSecond = -1.0;
                    runningStats.averageQueueDepth = -1.0;
                    runningStats.streamIDs.length(0);
                    runningStats.timeSinceLastCall = -1;
                    enabled = true;
                    flush_sec = 0;
                    flush_usec = 0;
                };

                ~linkStatistics() {
                }

                void setEnabled(bool enableStats) {
                    enabled = enableStats;
                }

                void update(unsigned int elementsReceived, float queueSize, bool EOS, std::string streamID, bool flush) {
                    if (!enabled) {
                        return;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);
                    receivedStatistics[receivedStatistics_idx].elements = elementsReceived;
                    receivedStatistics[receivedStatistics_idx].queueSize = queueSize;
                    receivedStatistics[receivedStatistics_idx].secs = tv.tv_sec;
                    receivedStatistics[receivedStatistics_idx++].usecs = tv.tv_usec;
                    receivedStatistics_idx = receivedStatistics_idx % historyWindow;
                    if (flush) {
                        flush_sec = tv.tv_sec;
                        flush_usec = tv.tv_usec;
                    }
                    if (!EOS) {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        bool foundStreamID = false;
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                foundStreamID = true;
                                break;
                            }
                            p++;
                        }
                        if (!foundStreamID) {
                            activeStreamIDs.push_back(streamID);
                        }
                    } else {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                activeStreamIDs.erase(p);
                                break;
                            }
                            p++;
                        }
                    }
                }

                BULKIO::PortStatistics retrieve() {
                    if (!enabled) {
                        return runningStats;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);

                    int idx = (receivedStatistics_idx == 0) ? (historyWindow - 1) : (receivedStatistics_idx - 1);
                    double front_sec = receivedStatistics[idx].secs;
                    double front_usec = receivedStatistics[idx].usecs;
                    double secDiff = tv.tv_sec - receivedStatistics[receivedStatistics_idx].secs;
                    double usecDiff = (tv.tv_usec - receivedStatistics[receivedStatistics_idx].usecs) / ((double)1e6);
                    double totalTime = secDiff + usecDiff;
                    double totalData = 0;
                    float queueSize = 0;
                    int startIdx = (receivedStatistics_idx + 1) % historyWindow;
                    for (int i = startIdx; i != receivedStatistics_idx; ) {
                        totalData += receivedStatistics[i].elements;
                        queueSize += receivedStatistics[i].queueSize;
                        i = (i + 1) % historyWindow;
                    }
                    runningStats.bitsPerSecond = ((totalData * bitSize) / totalTime);
                    runningStats.elementsPerSecond = (totalData / totalTime);
                    runningStats.averageQueueDepth = (queueSize / historyWindow);
                    runningStats.callsPerSecond = (double(historyWindow - 1) / totalTime);
                    runningStats.timeSinceLastCall = (((double)tv.tv_sec) - front_sec) + (((double)tv.tv_usec - front_usec) / ((double)1e6));
                    unsigned int streamIDsize = activeStreamIDs.size();
                    std::list< std::string >::iterator p = activeStreamIDs.begin();
                    runningStats.streamIDs.length(streamIDsize);
                    for (unsigned int i = 0; i < streamIDsize; i++) {
                        if (p == activeStreamIDs.end()) {
                            break;
                        }
                        runningStats.streamIDs[i] = CORBA::string_dup((*p).c_str());
                        p++;
                    }
                    if ((flush_sec != 0) && (flush_usec != 0)) {
                        double flushTotalTime = (((double)tv.tv_sec) - flush_sec) + (((double)tv.tv_usec - flush_usec) / ((double)1e6));
                        runningStats.keywords.length(1);
                        runningStats.keywords[0].id = CORBA::string_dup("timeSinceLastFlush");
                        runningStats.keywords[0].value <<= CORBA::Double(flushTotalTime);
                    }
                    return runningStats;
                }

            protected:
                bool enabled;
                double bitSize;
                BULKIO::PortStatistics runningStats;
                std::vector<statPoint> receivedStatistics;
                std::list< std::string > activeStreamIDs;
                unsigned long historyWindow;
                long receivedStatistics_idx;
                double flush_sec;
                double flush_usec;
        };
        
        void enableStats(bool enable) {
            stats.setEnabled(enable);
        };


        class dataTransfer
        {
            public:
                dataTransfer(const PortTypes::UlongSequence& data, const BULKIO::PrecisionUTCTime &_T, bool _EOS, const char* _streamID, BULKIO::StreamSRI &_H, bool _sriChanged, bool _inputQueueFlushed)
                {
                    int dataLength = data.length();

#ifdef EXPECTED_VECTOR_IMPL
                    std::_Vector_base<CORBA::ULong, _seqVector::seqVectorAllocator<CORBA::ULong> >::_Vector_impl *vectorPointer = (std::_Vector_base<CORBA::ULong, _seqVector::seqVectorAllocator<CORBA::ULong> >::_Vector_impl *) ((void*) & dataBuffer);
                    vectorPointer->_M_start = const_cast<PortTypes::UlongSequence*>(&data)->get_buffer(1);
                    vectorPointer->_M_finish = vectorPointer->_M_start + dataLength;
                    vectorPointer->_M_end_of_storage = vectorPointer->_M_finish;

#else
                    dataBuffer.resize(dataLength);
                    if (dataLength > 0) {
                        memcpy(&dataBuffer[0], &data[0], dataLength * sizeof(data[0]));
                    }

#endif
                    T = _T;
                    EOS = _EOS;
                    streamID = _streamID;
                    SRI = _H;
                    sriChanged = _sriChanged;
                    inputQueueFlushed = _inputQueueFlushed;
                };

#ifdef EXPECTED_VECTOR_IMPL
                std::vector< CORBA::ULong, _seqVector::seqVectorAllocator<CORBA::ULong> > dataBuffer;
#else
                std::vector<CORBA::ULong> dataBuffer;
#endif
                BULKIO::PrecisionUTCTime T;
                bool EOS;
                std::string streamID;
                BULKIO::StreamSRI SRI;
                bool sriChanged;
                bool inputQueueFlushed;
        };

        dataTransfer *getPacket(float timeout);
        void block();
        void unblock();

    protected:
        sinksocket_i *parent;
        std::deque<dataTransfer *> workQueue;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> > currentHs;
        boost::mutex dataBufferLock;
        boost::mutex sriUpdateLock;
        omni_mutex dataAvailableMutex;
        omni_condition* dataAvailable;
        unsigned long secs, nsecs, timeout_secs, timeout_nsecs;
        bool breakBlock;
        bool blocking;
        queueSemaphore* queueSem;

        // statistics
        linkStatistics stats;

};



// ----------------------------------------------------------------------------------------
// BULKIO_dataChar_In_i declaration
// ----------------------------------------------------------------------------------------
class BULKIO_dataChar_In_i : public POA_BULKIO::dataChar, public Port_Provides_base_impl
{
    public:
        BULKIO_dataChar_In_i(std::string port_name, sinksocket_base *_parent);
        ~BULKIO_dataChar_In_i();

        void pushSRI(const BULKIO::StreamSRI& H);
        void pushPacket(const PortTypes::CharSequence& data, const BULKIO::PrecisionUTCTime& T, CORBA::Boolean EOS, const char* streamID);

        BULKIO::PortUsageType state();
        BULKIO::PortStatistics* statistics();
        BULKIO::StreamSRISequence* activeSRIs();
        int getCurrentQueueDepth();
        int getMaxQueueDepth();
        void setMaxQueueDepth(int newDepth);

        class linkStatistics
        {
            public:
                struct statPoint {
                    unsigned int elements;
                    float queueSize;
                    double secs;
                    double usecs;
                };

                linkStatistics() {
                    bitSize = sizeof(char) * 8.0;
                    historyWindow = 10;
                    receivedStatistics_idx = 0;
                    receivedStatistics.resize(historyWindow);
                    activeStreamIDs.resize(0);
                    runningStats.elementsPerSecond = -1.0;
                    runningStats.bitsPerSecond = -1.0;
                    runningStats.callsPerSecond = -1.0;
                    runningStats.averageQueueDepth = -1.0;
                    runningStats.streamIDs.length(0);
                    runningStats.timeSinceLastCall = -1;
                    enabled = true;
                    flush_sec = 0;
                    flush_usec = 0;
                };

                ~linkStatistics() {
                }

                void setEnabled(bool enableStats) {
                    enabled = enableStats;
                }

                void update(unsigned int elementsReceived, float queueSize, bool EOS, std::string streamID, bool flush) {
                    if (!enabled) {
                        return;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);
                    receivedStatistics[receivedStatistics_idx].elements = elementsReceived;
                    receivedStatistics[receivedStatistics_idx].queueSize = queueSize;
                    receivedStatistics[receivedStatistics_idx].secs = tv.tv_sec;
                    receivedStatistics[receivedStatistics_idx++].usecs = tv.tv_usec;
                    receivedStatistics_idx = receivedStatistics_idx % historyWindow;
                    if (flush) {
                        flush_sec = tv.tv_sec;
                        flush_usec = tv.tv_usec;
                    }
                    if (!EOS) {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        bool foundStreamID = false;
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                foundStreamID = true;
                                break;
                            }
                            p++;
                        }
                        if (!foundStreamID) {
                            activeStreamIDs.push_back(streamID);
                        }
                    } else {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                activeStreamIDs.erase(p);
                                break;
                            }
                            p++;
                        }
                    }
                }

                BULKIO::PortStatistics retrieve() {
                    if (!enabled) {
                        return runningStats;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);

                    int idx = (receivedStatistics_idx == 0) ? (historyWindow - 1) : (receivedStatistics_idx - 1);
                    double front_sec = receivedStatistics[idx].secs;
                    double front_usec = receivedStatistics[idx].usecs;
                    double secDiff = tv.tv_sec - receivedStatistics[receivedStatistics_idx].secs;
                    double usecDiff = (tv.tv_usec - receivedStatistics[receivedStatistics_idx].usecs) / ((double)1e6);
                    double totalTime = secDiff + usecDiff;
                    double totalData = 0;
                    float queueSize = 0;
                    int startIdx = (receivedStatistics_idx + 1) % historyWindow;
                    for (int i = startIdx; i != receivedStatistics_idx; ) {
                        totalData += receivedStatistics[i].elements;
                        queueSize += receivedStatistics[i].queueSize;
                        i = (i + 1) % historyWindow;
                    }
                    runningStats.bitsPerSecond = ((totalData * bitSize) / totalTime);
                    runningStats.elementsPerSecond = (totalData / totalTime);
                    runningStats.averageQueueDepth = (queueSize / historyWindow);
                    runningStats.callsPerSecond = (double(historyWindow - 1) / totalTime);
                    runningStats.timeSinceLastCall = (((double)tv.tv_sec) - front_sec) + (((double)tv.tv_usec - front_usec) / ((double)1e6));
                    unsigned int streamIDsize = activeStreamIDs.size();
                    std::list< std::string >::iterator p = activeStreamIDs.begin();
                    runningStats.streamIDs.length(streamIDsize);
                    for (unsigned int i = 0; i < streamIDsize; i++) {
                        if (p == activeStreamIDs.end()) {
                            break;
                        }
                        runningStats.streamIDs[i] = CORBA::string_dup((*p).c_str());
                        p++;
                    }
                    if ((flush_sec != 0) && (flush_usec != 0)) {
                        double flushTotalTime = (((double)tv.tv_sec) - flush_sec) + (((double)tv.tv_usec - flush_usec) / ((double)1e6));
                        runningStats.keywords.length(1);
                        runningStats.keywords[0].id = CORBA::string_dup("timeSinceLastFlush");
                        runningStats.keywords[0].value <<= CORBA::Double(flushTotalTime);
                    }
                    return runningStats;
                }

            protected:
                bool enabled;
                double bitSize;
                BULKIO::PortStatistics runningStats;
                std::vector<statPoint> receivedStatistics;
                std::list< std::string > activeStreamIDs;
                unsigned long historyWindow;
                long receivedStatistics_idx;
                double flush_sec;
                double flush_usec;
        };
        
        void enableStats(bool enable) {
            stats.setEnabled(enable);
        };


        class dataTransfer
        {
            public:
                dataTransfer(const PortTypes::CharSequence& data, const BULKIO::PrecisionUTCTime &_T, bool _EOS, const char* _streamID, BULKIO::StreamSRI &_H, bool _sriChanged, bool _inputQueueFlushed)
                {
                    int dataLength = data.length();

#ifdef EXPECTED_VECTOR_IMPL
                    std::_Vector_base<char, _seqVector::seqVectorAllocator<char> >::_Vector_impl *vectorPointer = (std::_Vector_base<char, _seqVector::seqVectorAllocator<char> >::_Vector_impl *) ((void*) & dataBuffer);
                    unsigned char *tmp_2 = const_cast<PortTypes::CharSequence*>(&data)->get_buffer(1);
                    char *tmp = (char *) tmp_2;
                    vectorPointer->_M_start = tmp;
                    vectorPointer->_M_finish = vectorPointer->_M_start + dataLength;
                    vectorPointer->_M_end_of_storage = vectorPointer->_M_finish;

#else
                    dataBuffer.resize(dataLength);
                    if (dataLength > 0) {
                        memcpy(&dataBuffer[0], &data[0], dataLength * sizeof(data[0]));
                    }

#endif
                    T = _T;
                    EOS = _EOS;
                    streamID = _streamID;
                    SRI = _H;
                    sriChanged = _sriChanged;
                    inputQueueFlushed = _inputQueueFlushed;
                };

#ifdef EXPECTED_VECTOR_IMPL
                std::vector< char, _seqVector::seqVectorAllocator<char> > dataBuffer;
#else
                std::vector<char> dataBuffer;
#endif
                BULKIO::PrecisionUTCTime T;
                bool EOS;
                std::string streamID;
                BULKIO::StreamSRI SRI;
                bool sriChanged;
                bool inputQueueFlushed;
        };

        dataTransfer *getPacket(float timeout);
        void block();
        void unblock();

    protected:
        sinksocket_i *parent;
        std::deque<dataTransfer *> workQueue;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> > currentHs;
        boost::mutex dataBufferLock;
        boost::mutex sriUpdateLock;
        omni_mutex dataAvailableMutex;
        omni_condition* dataAvailable;
        unsigned long secs, nsecs, timeout_secs, timeout_nsecs;
        bool breakBlock;
        bool blocking;
        queueSemaphore* queueSem;

        // statistics
        linkStatistics stats;

};



// ----------------------------------------------------------------------------------------
// BULKIO_dataDouble_In_i declaration
// ----------------------------------------------------------------------------------------
class BULKIO_dataDouble_In_i : public POA_BULKIO::dataDouble, public Port_Provides_base_impl
{
    public:
        BULKIO_dataDouble_In_i(std::string port_name, sinksocket_base *_parent);
        ~BULKIO_dataDouble_In_i();

        void pushSRI(const BULKIO::StreamSRI& H);
        void pushPacket(const PortTypes::DoubleSequence& data, const BULKIO::PrecisionUTCTime& T, CORBA::Boolean EOS, const char* streamID);

        BULKIO::PortUsageType state();
        BULKIO::PortStatistics* statistics();
        BULKIO::StreamSRISequence* activeSRIs();
        int getCurrentQueueDepth();
        int getMaxQueueDepth();
        void setMaxQueueDepth(int newDepth);

        class linkStatistics
        {
            public:
                struct statPoint {
                    unsigned int elements;
                    float queueSize;
                    double secs;
                    double usecs;
                };

                linkStatistics() {
                    bitSize = sizeof(CORBA::Double) * 8.0;
                    historyWindow = 10;
                    receivedStatistics_idx = 0;
                    receivedStatistics.resize(historyWindow);
                    activeStreamIDs.resize(0);
                    runningStats.elementsPerSecond = -1.0;
                    runningStats.bitsPerSecond = -1.0;
                    runningStats.callsPerSecond = -1.0;
                    runningStats.averageQueueDepth = -1.0;
                    runningStats.streamIDs.length(0);
                    runningStats.timeSinceLastCall = -1;
                    enabled = true;
                    flush_sec = 0;
                    flush_usec = 0;
                };

                ~linkStatistics() {
                }

                void setEnabled(bool enableStats) {
                    enabled = enableStats;
                }

                void update(unsigned int elementsReceived, float queueSize, bool EOS, std::string streamID, bool flush) {
                    if (!enabled) {
                        return;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);
                    receivedStatistics[receivedStatistics_idx].elements = elementsReceived;
                    receivedStatistics[receivedStatistics_idx].queueSize = queueSize;
                    receivedStatistics[receivedStatistics_idx].secs = tv.tv_sec;
                    receivedStatistics[receivedStatistics_idx++].usecs = tv.tv_usec;
                    receivedStatistics_idx = receivedStatistics_idx % historyWindow;
                    if (flush) {
                        flush_sec = tv.tv_sec;
                        flush_usec = tv.tv_usec;
                    }
                    if (!EOS) {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        bool foundStreamID = false;
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                foundStreamID = true;
                                break;
                            }
                            p++;
                        }
                        if (!foundStreamID) {
                            activeStreamIDs.push_back(streamID);
                        }
                    } else {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                activeStreamIDs.erase(p);
                                break;
                            }
                            p++;
                        }
                    }
                }

                BULKIO::PortStatistics retrieve() {
                    if (!enabled) {
                        return runningStats;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);

                    int idx = (receivedStatistics_idx == 0) ? (historyWindow - 1) : (receivedStatistics_idx - 1);
                    double front_sec = receivedStatistics[idx].secs;
                    double front_usec = receivedStatistics[idx].usecs;
                    double secDiff = tv.tv_sec - receivedStatistics[receivedStatistics_idx].secs;
                    double usecDiff = (tv.tv_usec - receivedStatistics[receivedStatistics_idx].usecs) / ((double)1e6);
                    double totalTime = secDiff + usecDiff;
                    double totalData = 0;
                    float queueSize = 0;
                    int startIdx = (receivedStatistics_idx + 1) % historyWindow;
                    for (int i = startIdx; i != receivedStatistics_idx; ) {
                        totalData += receivedStatistics[i].elements;
                        queueSize += receivedStatistics[i].queueSize;
                        i = (i + 1) % historyWindow;
                    }
                    runningStats.bitsPerSecond = ((totalData * bitSize) / totalTime);
                    runningStats.elementsPerSecond = (totalData / totalTime);
                    runningStats.averageQueueDepth = (queueSize / historyWindow);
                    runningStats.callsPerSecond = (double(historyWindow - 1) / totalTime);
                    runningStats.timeSinceLastCall = (((double)tv.tv_sec) - front_sec) + (((double)tv.tv_usec - front_usec) / ((double)1e6));
                    unsigned int streamIDsize = activeStreamIDs.size();
                    std::list< std::string >::iterator p = activeStreamIDs.begin();
                    runningStats.streamIDs.length(streamIDsize);
                    for (unsigned int i = 0; i < streamIDsize; i++) {
                        if (p == activeStreamIDs.end()) {
                            break;
                        }
                        runningStats.streamIDs[i] = CORBA::string_dup((*p).c_str());
                        p++;
                    }
                    if ((flush_sec != 0) && (flush_usec != 0)) {
                        double flushTotalTime = (((double)tv.tv_sec) - flush_sec) + (((double)tv.tv_usec - flush_usec) / ((double)1e6));
                        runningStats.keywords.length(1);
                        runningStats.keywords[0].id = CORBA::string_dup("timeSinceLastFlush");
                        runningStats.keywords[0].value <<= CORBA::Double(flushTotalTime);
                    }
                    return runningStats;
                }

            protected:
                bool enabled;
                double bitSize;
                BULKIO::PortStatistics runningStats;
                std::vector<statPoint> receivedStatistics;
                std::list< std::string > activeStreamIDs;
                unsigned long historyWindow;
                long receivedStatistics_idx;
                double flush_sec;
                double flush_usec;
        };
        
        void enableStats(bool enable) {
            stats.setEnabled(enable);
        };


        class dataTransfer
        {
            public:
                dataTransfer(const PortTypes::DoubleSequence& data, const BULKIO::PrecisionUTCTime &_T, bool _EOS, const char* _streamID, BULKIO::StreamSRI &_H, bool _sriChanged, bool _inputQueueFlushed)
                {
                    int dataLength = data.length();

#ifdef EXPECTED_VECTOR_IMPL
                    std::_Vector_base<CORBA::Double, _seqVector::seqVectorAllocator<CORBA::Double> >::_Vector_impl *vectorPointer = (std::_Vector_base<CORBA::Double, _seqVector::seqVectorAllocator<CORBA::Double> >::_Vector_impl *) ((void*) & dataBuffer);
                    vectorPointer->_M_start = const_cast<PortTypes::DoubleSequence*>(&data)->get_buffer(1);
                    vectorPointer->_M_finish = vectorPointer->_M_start + dataLength;
                    vectorPointer->_M_end_of_storage = vectorPointer->_M_finish;

#else
                    dataBuffer.resize(dataLength);
                    if (dataLength > 0) {
                        memcpy(&dataBuffer[0], &data[0], dataLength * sizeof(data[0]));
                    }

#endif
                    T = _T;
                    EOS = _EOS;
                    streamID = _streamID;
                    SRI = _H;
                    sriChanged = _sriChanged;
                    inputQueueFlushed = _inputQueueFlushed;
                };

#ifdef EXPECTED_VECTOR_IMPL
                std::vector< CORBA::Double, _seqVector::seqVectorAllocator<CORBA::Double> > dataBuffer;
#else
                std::vector<CORBA::Double> dataBuffer;
#endif
                BULKIO::PrecisionUTCTime T;
                bool EOS;
                std::string streamID;
                BULKIO::StreamSRI SRI;
                bool sriChanged;
                bool inputQueueFlushed;
        };

        dataTransfer *getPacket(float timeout);
        void block();
        void unblock();

    protected:
        sinksocket_i *parent;
        std::deque<dataTransfer *> workQueue;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> > currentHs;
        boost::mutex dataBufferLock;
        boost::mutex sriUpdateLock;
        omni_mutex dataAvailableMutex;
        omni_condition* dataAvailable;
        unsigned long secs, nsecs, timeout_secs, timeout_nsecs;
        bool breakBlock;
        bool blocking;
        queueSemaphore* queueSem;

        // statistics
        linkStatistics stats;

};



// ----------------------------------------------------------------------------------------
// BULKIO_dataFloat_In_i declaration
// ----------------------------------------------------------------------------------------
class BULKIO_dataFloat_In_i : public POA_BULKIO::dataFloat, public Port_Provides_base_impl
{
    public:
        BULKIO_dataFloat_In_i(std::string port_name, sinksocket_base *_parent);
        ~BULKIO_dataFloat_In_i();

        void pushSRI(const BULKIO::StreamSRI& H);
        void pushPacket(const PortTypes::FloatSequence& data, const BULKIO::PrecisionUTCTime& T, CORBA::Boolean EOS, const char* streamID);

        BULKIO::PortUsageType state();
        BULKIO::PortStatistics* statistics();
        BULKIO::StreamSRISequence* activeSRIs();
        int getCurrentQueueDepth();
        int getMaxQueueDepth();
        void setMaxQueueDepth(int newDepth);

        class linkStatistics
        {
            public:
                struct statPoint {
                    unsigned int elements;
                    float queueSize;
                    double secs;
                    double usecs;
                };

                linkStatistics() {
                    bitSize = sizeof(CORBA::Float) * 8.0;
                    historyWindow = 10;
                    receivedStatistics_idx = 0;
                    receivedStatistics.resize(historyWindow);
                    activeStreamIDs.resize(0);
                    runningStats.elementsPerSecond = -1.0;
                    runningStats.bitsPerSecond = -1.0;
                    runningStats.callsPerSecond = -1.0;
                    runningStats.averageQueueDepth = -1.0;
                    runningStats.streamIDs.length(0);
                    runningStats.timeSinceLastCall = -1;
                    enabled = true;
                    flush_sec = 0;
                    flush_usec = 0;
                };

                ~linkStatistics() {
                }

                void setEnabled(bool enableStats) {
                    enabled = enableStats;
                }

                void update(unsigned int elementsReceived, float queueSize, bool EOS, std::string streamID, bool flush) {
                    if (!enabled) {
                        return;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);
                    receivedStatistics[receivedStatistics_idx].elements = elementsReceived;
                    receivedStatistics[receivedStatistics_idx].queueSize = queueSize;
                    receivedStatistics[receivedStatistics_idx].secs = tv.tv_sec;
                    receivedStatistics[receivedStatistics_idx++].usecs = tv.tv_usec;
                    receivedStatistics_idx = receivedStatistics_idx % historyWindow;
                    if (flush) {
                        flush_sec = tv.tv_sec;
                        flush_usec = tv.tv_usec;
                    }
                    if (!EOS) {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        bool foundStreamID = false;
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                foundStreamID = true;
                                break;
                            }
                            p++;
                        }
                        if (!foundStreamID) {
                            activeStreamIDs.push_back(streamID);
                        }
                    } else {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                activeStreamIDs.erase(p);
                                break;
                            }
                            p++;
                        }
                    }
                }

                BULKIO::PortStatistics retrieve() {
                    if (!enabled) {
                        return runningStats;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);

                    int idx = (receivedStatistics_idx == 0) ? (historyWindow - 1) : (receivedStatistics_idx - 1);
                    double front_sec = receivedStatistics[idx].secs;
                    double front_usec = receivedStatistics[idx].usecs;
                    double secDiff = tv.tv_sec - receivedStatistics[receivedStatistics_idx].secs;
                    double usecDiff = (tv.tv_usec - receivedStatistics[receivedStatistics_idx].usecs) / ((double)1e6);
                    double totalTime = secDiff + usecDiff;
                    double totalData = 0;
                    float queueSize = 0;
                    int startIdx = (receivedStatistics_idx + 1) % historyWindow;
                    for (int i = startIdx; i != receivedStatistics_idx; ) {
                        totalData += receivedStatistics[i].elements;
                        queueSize += receivedStatistics[i].queueSize;
                        i = (i + 1) % historyWindow;
                    }
                    runningStats.bitsPerSecond = ((totalData * bitSize) / totalTime);
                    runningStats.elementsPerSecond = (totalData / totalTime);
                    runningStats.averageQueueDepth = (queueSize / historyWindow);
                    runningStats.callsPerSecond = (double(historyWindow - 1) / totalTime);
                    runningStats.timeSinceLastCall = (((double)tv.tv_sec) - front_sec) + (((double)tv.tv_usec - front_usec) / ((double)1e6));
                    unsigned int streamIDsize = activeStreamIDs.size();
                    std::list< std::string >::iterator p = activeStreamIDs.begin();
                    runningStats.streamIDs.length(streamIDsize);
                    for (unsigned int i = 0; i < streamIDsize; i++) {
                        if (p == activeStreamIDs.end()) {
                            break;
                        }
                        runningStats.streamIDs[i] = CORBA::string_dup((*p).c_str());
                        p++;
                    }
                    if ((flush_sec != 0) && (flush_usec != 0)) {
                        double flushTotalTime = (((double)tv.tv_sec) - flush_sec) + (((double)tv.tv_usec - flush_usec) / ((double)1e6));
                        runningStats.keywords.length(1);
                        runningStats.keywords[0].id = CORBA::string_dup("timeSinceLastFlush");
                        runningStats.keywords[0].value <<= CORBA::Double(flushTotalTime);
                    }
                    return runningStats;
                }

            protected:
                bool enabled;
                double bitSize;
                BULKIO::PortStatistics runningStats;
                std::vector<statPoint> receivedStatistics;
                std::list< std::string > activeStreamIDs;
                unsigned long historyWindow;
                long receivedStatistics_idx;
                double flush_sec;
                double flush_usec;
        };
        
        void enableStats(bool enable) {
            stats.setEnabled(enable);
        };


        class dataTransfer
        {
            public:
                dataTransfer(const PortTypes::FloatSequence& data, const BULKIO::PrecisionUTCTime &_T, bool _EOS, const char* _streamID, BULKIO::StreamSRI &_H, bool _sriChanged, bool _inputQueueFlushed)
                {
                    int dataLength = data.length();

#ifdef EXPECTED_VECTOR_IMPL
                    std::_Vector_base<CORBA::Float, _seqVector::seqVectorAllocator<CORBA::Float> >::_Vector_impl *vectorPointer = (std::_Vector_base<CORBA::Float, _seqVector::seqVectorAllocator<CORBA::Float> >::_Vector_impl *) ((void*) & dataBuffer);
                    vectorPointer->_M_start = const_cast<PortTypes::FloatSequence*>(&data)->get_buffer(1);
                    vectorPointer->_M_finish = vectorPointer->_M_start + dataLength;
                    vectorPointer->_M_end_of_storage = vectorPointer->_M_finish;

#else
                    dataBuffer.resize(dataLength);
                    if (dataLength > 0) {
                        memcpy(&dataBuffer[0], &data[0], dataLength * sizeof(data[0]));
                    }

#endif
                    T = _T;
                    EOS = _EOS;
                    streamID = _streamID;
                    SRI = _H;
                    sriChanged = _sriChanged;
                    inputQueueFlushed = _inputQueueFlushed;
                };

#ifdef EXPECTED_VECTOR_IMPL
                std::vector< CORBA::Float, _seqVector::seqVectorAllocator<CORBA::Float> > dataBuffer;
#else
                std::vector<CORBA::Float> dataBuffer;
#endif
                BULKIO::PrecisionUTCTime T;
                bool EOS;
                std::string streamID;
                BULKIO::StreamSRI SRI;
                bool sriChanged;
                bool inputQueueFlushed;
        };

        dataTransfer *getPacket(float timeout);
        void block();
        void unblock();

    protected:
        sinksocket_i *parent;
        std::deque<dataTransfer *> workQueue;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> > currentHs;
        boost::mutex dataBufferLock;
        boost::mutex sriUpdateLock;
        omni_mutex dataAvailableMutex;
        omni_condition* dataAvailable;
        unsigned long secs, nsecs, timeout_secs, timeout_nsecs;
        bool breakBlock;
        bool blocking;
        queueSemaphore* queueSem;

        // statistics
        linkStatistics stats;

};



// ----------------------------------------------------------------------------------------
// BULKIO_dataLong_In_i declaration
// ----------------------------------------------------------------------------------------
class BULKIO_dataLong_In_i : public POA_BULKIO::dataLong, public Port_Provides_base_impl
{
    public:
        BULKIO_dataLong_In_i(std::string port_name, sinksocket_base *_parent);
        ~BULKIO_dataLong_In_i();

        void pushSRI(const BULKIO::StreamSRI& H);
        void pushPacket(const PortTypes::LongSequence& data, const BULKIO::PrecisionUTCTime& T, CORBA::Boolean EOS, const char* streamID);

        BULKIO::PortUsageType state();
        BULKIO::PortStatistics* statistics();
        BULKIO::StreamSRISequence* activeSRIs();
        int getCurrentQueueDepth();
        int getMaxQueueDepth();
        void setMaxQueueDepth(int newDepth);

        class linkStatistics
        {
            public:
                struct statPoint {
                    unsigned int elements;
                    float queueSize;
                    double secs;
                    double usecs;
                };

                linkStatistics() {
                    bitSize = sizeof(CORBA::Long) * 8.0;
                    historyWindow = 10;
                    receivedStatistics_idx = 0;
                    receivedStatistics.resize(historyWindow);
                    activeStreamIDs.resize(0);
                    runningStats.elementsPerSecond = -1.0;
                    runningStats.bitsPerSecond = -1.0;
                    runningStats.callsPerSecond = -1.0;
                    runningStats.averageQueueDepth = -1.0;
                    runningStats.streamIDs.length(0);
                    runningStats.timeSinceLastCall = -1;
                    enabled = true;
                    flush_sec = 0;
                    flush_usec = 0;
                };

                ~linkStatistics() {
                }

                void setEnabled(bool enableStats) {
                    enabled = enableStats;
                }

                void update(unsigned int elementsReceived, float queueSize, bool EOS, std::string streamID, bool flush) {
                    if (!enabled) {
                        return;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);
                    receivedStatistics[receivedStatistics_idx].elements = elementsReceived;
                    receivedStatistics[receivedStatistics_idx].queueSize = queueSize;
                    receivedStatistics[receivedStatistics_idx].secs = tv.tv_sec;
                    receivedStatistics[receivedStatistics_idx++].usecs = tv.tv_usec;
                    receivedStatistics_idx = receivedStatistics_idx % historyWindow;
                    if (flush) {
                        flush_sec = tv.tv_sec;
                        flush_usec = tv.tv_usec;
                    }
                    if (!EOS) {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        bool foundStreamID = false;
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                foundStreamID = true;
                                break;
                            }
                            p++;
                        }
                        if (!foundStreamID) {
                            activeStreamIDs.push_back(streamID);
                        }
                    } else {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                activeStreamIDs.erase(p);
                                break;
                            }
                            p++;
                        }
                    }
                }

                BULKIO::PortStatistics retrieve() {
                    if (!enabled) {
                        return runningStats;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);

                    int idx = (receivedStatistics_idx == 0) ? (historyWindow - 1) : (receivedStatistics_idx - 1);
                    double front_sec = receivedStatistics[idx].secs;
                    double front_usec = receivedStatistics[idx].usecs;
                    double secDiff = tv.tv_sec - receivedStatistics[receivedStatistics_idx].secs;
                    double usecDiff = (tv.tv_usec - receivedStatistics[receivedStatistics_idx].usecs) / ((double)1e6);
                    double totalTime = secDiff + usecDiff;
                    double totalData = 0;
                    float queueSize = 0;
                    int startIdx = (receivedStatistics_idx + 1) % historyWindow;
                    for (int i = startIdx; i != receivedStatistics_idx; ) {
                        totalData += receivedStatistics[i].elements;
                        queueSize += receivedStatistics[i].queueSize;
                        i = (i + 1) % historyWindow;
                    }
                    runningStats.bitsPerSecond = ((totalData * bitSize) / totalTime);
                    runningStats.elementsPerSecond = (totalData / totalTime);
                    runningStats.averageQueueDepth = (queueSize / historyWindow);
                    runningStats.callsPerSecond = (double(historyWindow - 1) / totalTime);
                    runningStats.timeSinceLastCall = (((double)tv.tv_sec) - front_sec) + (((double)tv.tv_usec - front_usec) / ((double)1e6));
                    unsigned int streamIDsize = activeStreamIDs.size();
                    std::list< std::string >::iterator p = activeStreamIDs.begin();
                    runningStats.streamIDs.length(streamIDsize);
                    for (unsigned int i = 0; i < streamIDsize; i++) {
                        if (p == activeStreamIDs.end()) {
                            break;
                        }
                        runningStats.streamIDs[i] = CORBA::string_dup((*p).c_str());
                        p++;
                    }
                    if ((flush_sec != 0) && (flush_usec != 0)) {
                        double flushTotalTime = (((double)tv.tv_sec) - flush_sec) + (((double)tv.tv_usec - flush_usec) / ((double)1e6));
                        runningStats.keywords.length(1);
                        runningStats.keywords[0].id = CORBA::string_dup("timeSinceLastFlush");
                        runningStats.keywords[0].value <<= CORBA::Double(flushTotalTime);
                    }
                    return runningStats;
                }

            protected:
                bool enabled;
                double bitSize;
                BULKIO::PortStatistics runningStats;
                std::vector<statPoint> receivedStatistics;
                std::list< std::string > activeStreamIDs;
                unsigned long historyWindow;
                long receivedStatistics_idx;
                double flush_sec;
                double flush_usec;
        };
        
        void enableStats(bool enable) {
            stats.setEnabled(enable);
        };


        class dataTransfer
        {
            public:
                dataTransfer(const PortTypes::LongSequence& data, const BULKIO::PrecisionUTCTime &_T, bool _EOS, const char* _streamID, BULKIO::StreamSRI &_H, bool _sriChanged, bool _inputQueueFlushed)
                {
                    int dataLength = data.length();

#ifdef EXPECTED_VECTOR_IMPL
                    std::_Vector_base<CORBA::Long, _seqVector::seqVectorAllocator<CORBA::Long> >::_Vector_impl *vectorPointer = (std::_Vector_base<CORBA::Long, _seqVector::seqVectorAllocator<CORBA::Long> >::_Vector_impl *) ((void*) & dataBuffer);
                    vectorPointer->_M_start = const_cast<PortTypes::LongSequence*>(&data)->get_buffer(1);
                    vectorPointer->_M_finish = vectorPointer->_M_start + dataLength;
                    vectorPointer->_M_end_of_storage = vectorPointer->_M_finish;

#else
                    dataBuffer.resize(dataLength);
                    if (dataLength > 0) {
                        memcpy(&dataBuffer[0], &data[0], dataLength * sizeof(data[0]));
                    }

#endif
                    T = _T;
                    EOS = _EOS;
                    streamID = _streamID;
                    SRI = _H;
                    sriChanged = _sriChanged;
                    inputQueueFlushed = _inputQueueFlushed;
                };

#ifdef EXPECTED_VECTOR_IMPL
                std::vector< CORBA::Long, _seqVector::seqVectorAllocator<CORBA::Long> > dataBuffer;
#else
                std::vector<CORBA::Long> dataBuffer;
#endif
                BULKIO::PrecisionUTCTime T;
                bool EOS;
                std::string streamID;
                BULKIO::StreamSRI SRI;
                bool sriChanged;
                bool inputQueueFlushed;
        };

        dataTransfer *getPacket(float timeout);
        void block();
        void unblock();

    protected:
        sinksocket_i *parent;
        std::deque<dataTransfer *> workQueue;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> > currentHs;
        boost::mutex dataBufferLock;
        boost::mutex sriUpdateLock;
        omni_mutex dataAvailableMutex;
        omni_condition* dataAvailable;
        unsigned long secs, nsecs, timeout_secs, timeout_nsecs;
        bool breakBlock;
        bool blocking;
        queueSemaphore* queueSem;

        // statistics
        linkStatistics stats;

};



// ----------------------------------------------------------------------------------------
// BULKIO_dataOctet_In_i declaration
// ----------------------------------------------------------------------------------------
class BULKIO_dataOctet_In_i : public POA_BULKIO::dataOctet, public Port_Provides_base_impl
{
    public:
        BULKIO_dataOctet_In_i(std::string port_name, sinksocket_base *_parent);
        ~BULKIO_dataOctet_In_i();

        void pushSRI(const BULKIO::StreamSRI& H);
        void pushPacket(const CF::OctetSequence& data, const BULKIO::PrecisionUTCTime& T, CORBA::Boolean EOS, const char* streamID);

        BULKIO::PortUsageType state();
        BULKIO::PortStatistics* statistics();
        BULKIO::StreamSRISequence* activeSRIs();
        int getCurrentQueueDepth();
        int getMaxQueueDepth();
        void setMaxQueueDepth(int newDepth);

        class linkStatistics
        {
            public:
                struct statPoint {
                    unsigned int elements;
                    float queueSize;
                    double secs;
                    double usecs;
                };

                linkStatistics() {
                    bitSize = sizeof(unsigned char) * 8.0;
                    historyWindow = 10;
                    receivedStatistics_idx = 0;
                    receivedStatistics.resize(historyWindow);
                    activeStreamIDs.resize(0);
                    runningStats.elementsPerSecond = -1.0;
                    runningStats.bitsPerSecond = -1.0;
                    runningStats.callsPerSecond = -1.0;
                    runningStats.averageQueueDepth = -1.0;
                    runningStats.streamIDs.length(0);
                    runningStats.timeSinceLastCall = -1;
                    enabled = true;
                    flush_sec = 0;
                    flush_usec = 0;
                };

                ~linkStatistics() {
                }

                void setEnabled(bool enableStats) {
                    enabled = enableStats;
                }

                void update(unsigned int elementsReceived, float queueSize, bool EOS, std::string streamID, bool flush) {
                    if (!enabled) {
                        return;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);
                    receivedStatistics[receivedStatistics_idx].elements = elementsReceived;
                    receivedStatistics[receivedStatistics_idx].queueSize = queueSize;
                    receivedStatistics[receivedStatistics_idx].secs = tv.tv_sec;
                    receivedStatistics[receivedStatistics_idx++].usecs = tv.tv_usec;
                    receivedStatistics_idx = receivedStatistics_idx % historyWindow;
                    if (flush) {
                        flush_sec = tv.tv_sec;
                        flush_usec = tv.tv_usec;
                    }
                    if (!EOS) {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        bool foundStreamID = false;
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                foundStreamID = true;
                                break;
                            }
                            p++;
                        }
                        if (!foundStreamID) {
                            activeStreamIDs.push_back(streamID);
                        }
                    } else {
                        std::list<std::string>::iterator p = activeStreamIDs.begin();
                        while (p != activeStreamIDs.end()) {
                            if (*p == streamID) {
                                activeStreamIDs.erase(p);
                                break;
                            }
                            p++;
                        }
                    }
                }

                BULKIO::PortStatistics retrieve() {
                    if (!enabled) {
                        return runningStats;
                    }
                    struct timeval tv;
                    struct timezone tz;
                    gettimeofday(&tv, &tz);

                    int idx = (receivedStatistics_idx == 0) ? (historyWindow - 1) : (receivedStatistics_idx - 1);
                    double front_sec = receivedStatistics[idx].secs;
                    double front_usec = receivedStatistics[idx].usecs;
                    double secDiff = tv.tv_sec - receivedStatistics[receivedStatistics_idx].secs;
                    double usecDiff = (tv.tv_usec - receivedStatistics[receivedStatistics_idx].usecs) / ((double)1e6);
                    double totalTime = secDiff + usecDiff;
                    double totalData = 0;
                    float queueSize = 0;
                    int startIdx = (receivedStatistics_idx + 1) % historyWindow;
                    for (int i = startIdx; i != receivedStatistics_idx; ) {
                        totalData += receivedStatistics[i].elements;
                        queueSize += receivedStatistics[i].queueSize;
                        i = (i + 1) % historyWindow;
                    }
                    runningStats.bitsPerSecond = ((totalData * bitSize) / totalTime);
                    runningStats.elementsPerSecond = (totalData / totalTime);
                    runningStats.averageQueueDepth = (queueSize / historyWindow);
                    runningStats.callsPerSecond = (double(historyWindow - 1) / totalTime);
                    runningStats.timeSinceLastCall = (((double)tv.tv_sec) - front_sec) + (((double)tv.tv_usec - front_usec) / ((double)1e6));
                    unsigned int streamIDsize = activeStreamIDs.size();
                    std::list< std::string >::iterator p = activeStreamIDs.begin();
                    runningStats.streamIDs.length(streamIDsize);
                    for (unsigned int i = 0; i < streamIDsize; i++) {
                        if (p == activeStreamIDs.end()) {
                            break;
                        }
                        runningStats.streamIDs[i] = CORBA::string_dup((*p).c_str());
                        p++;
                    }
                    if ((flush_sec != 0) && (flush_usec != 0)) {
                        double flushTotalTime = (((double)tv.tv_sec) - flush_sec) + (((double)tv.tv_usec - flush_usec) / ((double)1e6));
                        runningStats.keywords.length(1);
                        runningStats.keywords[0].id = CORBA::string_dup("timeSinceLastFlush");
                        runningStats.keywords[0].value <<= CORBA::Double(flushTotalTime);
                    }
                    return runningStats;
                }

            protected:
                bool enabled;
                double bitSize;
                BULKIO::PortStatistics runningStats;
                std::vector<statPoint> receivedStatistics;
                std::list< std::string > activeStreamIDs;
                unsigned long historyWindow;
                long receivedStatistics_idx;
                double flush_sec;
                double flush_usec;
        };
        
        void enableStats(bool enable) {
            stats.setEnabled(enable);
        };


        class dataTransfer
        {
            public:
                dataTransfer(const CF::OctetSequence& data, const BULKIO::PrecisionUTCTime &_T, bool _EOS, const char* _streamID, BULKIO::StreamSRI &_H, bool _sriChanged, bool _inputQueueFlushed)
                {
                    int dataLength = data.length();

#ifdef EXPECTED_VECTOR_IMPL
                    std::_Vector_base<unsigned char, _seqVector::seqVectorAllocator<unsigned char> >::_Vector_impl *vectorPointer = (std::_Vector_base<unsigned char, _seqVector::seqVectorAllocator<unsigned char> >::_Vector_impl *) ((void*) & dataBuffer);
                    vectorPointer->_M_start = const_cast<CF::OctetSequence*>(&data)->get_buffer(1);
                    vectorPointer->_M_finish = vectorPointer->_M_start + dataLength;
                    vectorPointer->_M_end_of_storage = vectorPointer->_M_finish;

#else
                    dataBuffer.resize(dataLength);
                    if (dataLength > 0) {
                        memcpy(&dataBuffer[0], &data[0], dataLength * sizeof(data[0]));
                    }

#endif
                    T = _T;
                    EOS = _EOS;
                    streamID = _streamID;
                    SRI = _H;
                    sriChanged = _sriChanged;
                    inputQueueFlushed = _inputQueueFlushed;
                };

#ifdef EXPECTED_VECTOR_IMPL
                std::vector< unsigned char, _seqVector::seqVectorAllocator<unsigned char> > dataBuffer;
#else
                std::vector<unsigned char> dataBuffer;
#endif
                BULKIO::PrecisionUTCTime T;
                bool EOS;
                std::string streamID;
                BULKIO::StreamSRI SRI;
                bool sriChanged;
                bool inputQueueFlushed;
        };

        dataTransfer *getPacket(float timeout);
        void block();
        void unblock();

    protected:
        sinksocket_i *parent;
        std::deque<dataTransfer *> workQueue;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> > currentHs;
        boost::mutex dataBufferLock;
        boost::mutex sriUpdateLock;
        omni_mutex dataAvailableMutex;
        omni_condition* dataAvailable;
        unsigned long secs, nsecs, timeout_secs, timeout_nsecs;
        bool breakBlock;
        bool blocking;
        queueSemaphore* queueSem;

        // statistics
        linkStatistics stats;

};

#endif
