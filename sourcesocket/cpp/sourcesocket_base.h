#ifndef SOURCESOCKET_IMPL_BASE_H
#define SOURCESOCKET_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Resource_impl.h>

#include "port_impl.h"
#include "struct_props.h"

#define NOOP 0
#define FINISH -1
#define NORMAL 1

class sourcesocket_base;


template < typename TargetClass >
class ProcessThread
{
    public:
        ProcessThread(TargetClass *_target, float _delay) :
            target(_target)
        {
            _mythread = 0;
            _thread_running = false;
            _udelay = (__useconds_t)(_delay * 1000000);
        };

        // kick off the thread
        void start() {
            if (_mythread == 0) {
                _thread_running = true;
                _mythread = new boost::thread(&ProcessThread::run, this);
            }
        };

        // manage calls to target's service function
        void run() {
            int state = NORMAL;
            while (_thread_running and (state != FINISH)) {
                state = target->serviceFunction();
                if (state == NOOP) usleep(_udelay);
            }
        };

        // stop thread and wait for termination
        bool release(unsigned long secs = 0, unsigned long usecs = 0) {
            _thread_running = false;
            if (_mythread)  {
                if ((secs == 0) and (usecs == 0)){
                    _mythread->join();
                } else {
                    boost::system_time waitime= boost::get_system_time() + boost::posix_time::seconds(secs) +  boost::posix_time::microseconds(usecs) ;
                    if (!_mythread->timed_join(waitime)) {
                        return 0;
                    }
                }
                delete _mythread;
                _mythread = 0;
            }
    
            return 1;
        };

        virtual ~ProcessThread(){
            if (_mythread != 0) {
                release(0);
                _mythread = 0;
            }
        };

        void updateDelay(float _delay) { _udelay = (__useconds_t)(_delay * 1000000); };

    private:
        boost::thread *_mythread;
        bool _thread_running;
        TargetClass *target;
        __useconds_t _udelay;
        boost::condition_variable _end_of_run;
        boost::mutex _eor_mutex;
};

class sourcesocket_base : public Resource_impl
{
    friend class BULKIO_dataUshort_Out_i;
    friend class BULKIO_dataShort_Out_i;
    friend class BULKIO_dataUlong_Out_i;
    friend class BULKIO_dataChar_Out_i;
    friend class BULKIO_dataDouble_Out_i;
    friend class BULKIO_dataFloat_Out_i;
    friend class BULKIO_dataLong_Out_i;
    friend class BULKIO_dataOctet_Out_i;

    public: 
        sourcesocket_base(const char *uuid, const char *label);

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        CORBA::Object_ptr getPort(const char* _id) throw (CF::PortSupplier::UnknownPort, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);

        void loadProperties();

        virtual int serviceFunction() = 0;

    protected:
        ProcessThread<sourcesocket_base> *serviceThread; 
        boost::mutex serviceThreadLock;  

        // Member variables exposed as properties
        std::string connection_type;
        std::string ip_address;
        unsigned short port;
        std::string status;
        double total_bytes;
        float bytes_per_sec;
        CORBA::ULong max_bytes;
        CORBA::ULong min_bytes;
        unsigned short byte_swap;
        sri_struct sri;

        // Ports
        BULKIO_dataOctet_Out_i *dataOctet_out;
        BULKIO_dataChar_Out_i *dataChar_out;
        BULKIO_dataShort_Out_i *dataShort_out;
        BULKIO_dataUshort_Out_i *dataUshort_out;
        BULKIO_dataUlong_Out_i *dataUlong_out;
        BULKIO_dataLong_Out_i *dataLong_out;
        BULKIO_dataFloat_Out_i *dataFloat_out;
        BULKIO_dataDouble_Out_i *dataDouble_out;
    
    private:
        void construct();

};
#endif
