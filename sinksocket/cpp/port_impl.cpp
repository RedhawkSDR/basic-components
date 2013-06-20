/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

 	Source: sinksocket.spd.xml
 	Generated on: Thu Jun 20 09:43:15 EDT 2013
 	REDHAWK IDE
 	Version: 1.8.4
 	Build id: R201305151907

*******************************************************************************************/

#include "sinksocket.h"
// ----------------------------------------------------------------------------------------
// BULKIO_dataOctet_In_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataOctet_In_i::BULKIO_dataOctet_In_i(std::string port_name, sinksocket_base *_parent) : 
Port_Provides_base_impl(port_name)
{
    dataAvailable = new omni_condition(&dataAvailableMutex);
    queueSem = new queueSemaphore(100);
    blocking = false;
    breakBlock = false;
    parent = static_cast<sinksocket_i *> (_parent);
}

BULKIO_dataOctet_In_i::~BULKIO_dataOctet_In_i()
{
    block();
    while (workQueue.size() != 0) {
        dataTransfer *tmp = workQueue.front();
        workQueue.pop_front();
        delete tmp;
    }
}

BULKIO::PortStatistics * BULKIO_dataOctet_In_i::statistics()
{
    boost::mutex::scoped_lock lock(dataBufferLock);
    BULKIO::PortStatistics_var recStat = new BULKIO::PortStatistics(stats.retrieve());
    // NOTE: You must delete the object that this function returns!
    return recStat._retn();
}

BULKIO::PortUsageType BULKIO_dataOctet_In_i::state()
{
    boost::mutex::scoped_lock lock(dataBufferLock);
    if (workQueue.size() == queueSem->getMaxValue()) {
        return BULKIO::BUSY;
    } else if (workQueue.size() == 0) {
        return BULKIO::IDLE;
    } else {
        return BULKIO::ACTIVE;
    }

    return BULKIO::BUSY;
}

BULKIO::StreamSRISequence * BULKIO_dataOctet_In_i::activeSRIs()
{
    boost::mutex::scoped_lock lock(sriUpdateLock);
    BULKIO::StreamSRISequence seq_rtn;
    std::map<std::string, std::pair<BULKIO::StreamSRI, bool> >::iterator currH;
    int i = 0;
    for (currH = currentHs.begin(); currH != currentHs.end(); currH++) {
    	i++;
    	seq_rtn.length(i);
    	seq_rtn[i-1] = currH->second.first;
    }
    BULKIO::StreamSRISequence_var retSRI = new BULKIO::StreamSRISequence(seq_rtn);

    // NOTE: You must delete the object that this function returns!
    return retSRI._retn();
}

int BULKIO_dataOctet_In_i::getMaxQueueDepth()
{
    boost::mutex::scoped_lock lock(dataBufferLock);
    return queueSem->getMaxValue();
}

int BULKIO_dataOctet_In_i::getCurrentQueueDepth()
{
    boost::mutex::scoped_lock lock(dataBufferLock);
    return workQueue.size();
}

void BULKIO_dataOctet_In_i::setMaxQueueDepth(int newDepth)
{
    boost::mutex::scoped_lock lock(dataBufferLock);
    queueSem->setMaxValue(newDepth);
}

void BULKIO_dataOctet_In_i::pushSRI(const BULKIO::StreamSRI& H)
{
    bool updateSem = false;
    {
        boost::mutex::scoped_lock lock(sriUpdateLock);
        BULKIO::StreamSRI tmpH = H;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> >::iterator currH = currentHs.find(std::string(H.streamID));
        if (currH == currentHs.end()) {
    	    currentHs[std::string(H.streamID)] = std::make_pair(tmpH, true);
            if (H.blocking) {
                updateSem = true;
            }
        } else {
            if (!parent->compareSRI(tmpH, currH->second.first)) {
                currentHs[std::string(H.streamID)] = std::make_pair(tmpH, true);
                if (H.blocking) {
                    updateSem = true;
                }
    	    }
        }
    }
    
    if (updateSem) {
        boost::mutex::scoped_lock lock(dataBufferLock);
        blocking = true;
        queueSem->setCurrValue(workQueue.size());
    }
}

void BULKIO_dataOctet_In_i::pushPacket(const CF::OctetSequence& data, const BULKIO::PrecisionUTCTime& T, CORBA::Boolean EOS, const char* streamID)
{
    if (queueSem->getMaxValue() == 0) {
        return;
    }    
    BULKIO::StreamSRI tmpH = {1, 0.0, 1.0, 1, 0, 0.0, 0.0, 0, 0, streamID, false, 0};
    bool sriChanged = false;
    bool portBlocking = false;

    std::map<std::string, std::pair<BULKIO::StreamSRI, bool> >::iterator currH;
    {
        boost::mutex::scoped_lock lock(sriUpdateLock);

        currH = currentHs.find(std::string(streamID));
        if (currH != currentHs.end()) {
            tmpH = currH->second.first;
            sriChanged = currH->second.second;
            currentHs[streamID] = std::make_pair(currH->second.first, false);
        }
    }

    if (EOS) {
        bool searchBlocking = false;
        std::map<std::string, std::pair<BULKIO::StreamSRI, bool> >::iterator checkH = currentHs.begin();
        while (checkH != currentHs.end()) {
            if (std::string(checkH->second.first.streamID) == std::string(streamID)) {
                checkH++;
                continue;
            }
            if (checkH->second.first.blocking) {
                searchBlocking = true;
                break;
            }
            checkH++;
        }
        blocking = false;
    }
    portBlocking = blocking;
    
    if(portBlocking) {
        queueSem->incr();
        boost::mutex::scoped_lock lock(dataBufferLock);
        stats.update(data.length(), workQueue.size()/queueSem->getMaxValue(), EOS, streamID, false);
        BULKIO_dataOctet_In_i::dataTransfer *tmpIn = new BULKIO_dataOctet_In_i::dataTransfer(data, T, EOS, streamID, tmpH, sriChanged, false);
        workQueue.push_back(tmpIn);
        dataAvailable->signal();
    } else {
        boost::mutex::scoped_lock lock(dataBufferLock);
        bool flushToReport = false;
        if (workQueue.size() == queueSem->getMaxValue()) { // reached maximum queue depth - flush the queue
            flushToReport = true;
            BULKIO_dataOctet_In_i::dataTransfer *tmp;
            while (workQueue.size() != 0) {
                tmp = workQueue.front();
                workQueue.pop_front();
                delete tmp;
            }
        }
        stats.update(data.length(), workQueue.size()/queueSem->getMaxValue(), EOS, streamID, flushToReport);
        BULKIO_dataOctet_In_i::dataTransfer *tmpIn = new BULKIO_dataOctet_In_i::dataTransfer(data, T, EOS, streamID, tmpH, sriChanged, flushToReport);
        workQueue.push_back(tmpIn);
        dataAvailable->signal();
    }
}


void BULKIO_dataOctet_In_i::block()
{
    breakBlock = true;
    dataAvailable->signal();
}

void BULKIO_dataOctet_In_i::unblock()
{
    breakBlock = false;
}

/*
 * getPacket
 *     description: retrieve data from the provides (input) port
 *
 *  timeout: the amount of time to wait for data before a NULL is returned.
 *           Use 0.0 for non-blocking and -1 for blocking.
 */
BULKIO_dataOctet_In_i::dataTransfer *BULKIO_dataOctet_In_i::getPacket(float timeout)
{
    if (breakBlock) {
        return NULL;
    }
    if (workQueue.size() == 0) {
        if (timeout == 0.0) {
            return NULL;
        } else if (timeout > 0){
            secs = (unsigned long)(trunc(timeout));
            nsecs = (unsigned long)((timeout - secs) * 1e9);
            omni_thread::get_time(&timeout_secs, &timeout_nsecs, secs, nsecs);
            if (not dataAvailable->timedwait(timeout_secs, timeout_nsecs)) {
                return NULL;
            }
            if (breakBlock) {
                return NULL;
            }
        } else {
            dataAvailable->wait();
            if (breakBlock) {
                return NULL;
            }
        }
    }
    boost::mutex::scoped_lock lock1(dataBufferLock);
    BULKIO_dataOctet_In_i::dataTransfer *tmp = workQueue.front();
    workQueue.pop_front();
    
    boost::mutex::scoped_lock lock2(sriUpdateLock);
    if (tmp->EOS) {
	    std::map<std::string, std::pair<BULKIO::StreamSRI, bool> >::iterator target = currentHs.find(std::string(tmp->streamID));
        if (target != currentHs.end()) {
            if (target->second.first.blocking) {
                std::map<std::string, std::pair<BULKIO::StreamSRI, bool> >::iterator currH;
                bool keepBlocking = false;
                for (currH = currentHs.begin(); currH != currentHs.end(); currH++) {
                    if (currH->second.first.blocking) {
                        keepBlocking = true;
                        break;
                    }
                }

                if (!keepBlocking) {
                    queueSem->setCurrValue(0);
                    blocking = false;
                }
            }
            currentHs.erase(target);
        }
    }

    if (blocking) {
        queueSem->decr();
    }
    
    return tmp;
}

