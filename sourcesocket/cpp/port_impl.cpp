/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

 	Source: sourcesocket.spd.xml
 	Generated on: Mon Jun 24 17:48:25 EDT 2013
 	REDHAWK IDE
 	Version: 1.8.4
 	Build id: R201305151907

*******************************************************************************************/

#include "sourcesocket.h"
// ----------------------------------------------------------------------------------------
// BULKIO_dataUshort_Out_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataUshort_Out_i::BULKIO_dataUshort_Out_i(std::string port_name, sourcesocket_base *_parent) :
Port_Uses_base_impl(port_name)
{
    parent = static_cast<sourcesocket_i *> (_parent);
    recConnectionsRefresh = false;
    recConnections.length(0);
}

BULKIO_dataUshort_Out_i::~BULKIO_dataUshort_Out_i()
{
}

/*
 * pushSRI
 *     description: send out SRI describing the data payload
 *
 *  H: structure of type BULKIO::StreamSRI with the SRI for this stream
 *    hversion
 *    xstart: start time of the stream
 *    xdelta: delta between two samples
 *    xunits: unit types from Platinum specification
 *    subsize: 0 if the data is one-dimensional
 *    ystart
 *    ydelta
 *    yunits: unit types from Platinum specification
 *    mode: 0-scalar, 1-complex
 *    streamID: stream identifier
 *    sequence<CF::DataType> keywords: unconstrained sequence of key-value pairs for additional description
 */
void BULKIO_dataUshort_Out_i::pushSRI(const BULKIO::StreamSRI& H)
{
    std::vector < std::pair < BULKIO::dataUshort_var, std::string > >::iterator i;

    boost::mutex::scoped_lock lock(updatingPortsLock);   // don't want to process while command information is coming in

    if (active) {
        for (i = outConnections.begin(); i != outConnections.end(); ++i) {
            try {
                ((*i).first)->pushSRI(H);
            } catch(...) {
                std::cout << "Call to pushSRI by BULKIO_dataUshort_Out_i failed" << std::endl;
            }
        }
    }

    currentSRIs[std::string(H.streamID)] = H;
    refreshSRI = false;

    return;
}



// ----------------------------------------------------------------------------------------
// BULKIO_dataShort_Out_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataShort_Out_i::BULKIO_dataShort_Out_i(std::string port_name, sourcesocket_base *_parent) :
Port_Uses_base_impl(port_name)
{
    parent = static_cast<sourcesocket_i *> (_parent);
    recConnectionsRefresh = false;
    recConnections.length(0);
}

BULKIO_dataShort_Out_i::~BULKIO_dataShort_Out_i()
{
}

/*
 * pushSRI
 *     description: send out SRI describing the data payload
 *
 *  H: structure of type BULKIO::StreamSRI with the SRI for this stream
 *    hversion
 *    xstart: start time of the stream
 *    xdelta: delta between two samples
 *    xunits: unit types from Platinum specification
 *    subsize: 0 if the data is one-dimensional
 *    ystart
 *    ydelta
 *    yunits: unit types from Platinum specification
 *    mode: 0-scalar, 1-complex
 *    streamID: stream identifier
 *    sequence<CF::DataType> keywords: unconstrained sequence of key-value pairs for additional description
 */
void BULKIO_dataShort_Out_i::pushSRI(const BULKIO::StreamSRI& H)
{
    std::vector < std::pair < BULKIO::dataShort_var, std::string > >::iterator i;

    boost::mutex::scoped_lock lock(updatingPortsLock);   // don't want to process while command information is coming in

    if (active) {
        for (i = outConnections.begin(); i != outConnections.end(); ++i) {
            try {
                ((*i).first)->pushSRI(H);
            } catch(...) {
                std::cout << "Call to pushSRI by BULKIO_dataShort_Out_i failed" << std::endl;
            }
        }
    }

    currentSRIs[std::string(H.streamID)] = H;
    refreshSRI = false;

    return;
}



// ----------------------------------------------------------------------------------------
// BULKIO_dataUlong_Out_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataUlong_Out_i::BULKIO_dataUlong_Out_i(std::string port_name, sourcesocket_base *_parent) :
Port_Uses_base_impl(port_name)
{
    parent = static_cast<sourcesocket_i *> (_parent);
    recConnectionsRefresh = false;
    recConnections.length(0);
}

BULKIO_dataUlong_Out_i::~BULKIO_dataUlong_Out_i()
{
}

/*
 * pushSRI
 *     description: send out SRI describing the data payload
 *
 *  H: structure of type BULKIO::StreamSRI with the SRI for this stream
 *    hversion
 *    xstart: start time of the stream
 *    xdelta: delta between two samples
 *    xunits: unit types from Platinum specification
 *    subsize: 0 if the data is one-dimensional
 *    ystart
 *    ydelta
 *    yunits: unit types from Platinum specification
 *    mode: 0-scalar, 1-complex
 *    streamID: stream identifier
 *    sequence<CF::DataType> keywords: unconstrained sequence of key-value pairs for additional description
 */
void BULKIO_dataUlong_Out_i::pushSRI(const BULKIO::StreamSRI& H)
{
    std::vector < std::pair < BULKIO::dataUlong_var, std::string > >::iterator i;

    boost::mutex::scoped_lock lock(updatingPortsLock);   // don't want to process while command information is coming in

    if (active) {
        for (i = outConnections.begin(); i != outConnections.end(); ++i) {
            try {
                ((*i).first)->pushSRI(H);
            } catch(...) {
                std::cout << "Call to pushSRI by BULKIO_dataUlong_Out_i failed" << std::endl;
            }
        }
    }

    currentSRIs[std::string(H.streamID)] = H;
    refreshSRI = false;

    return;
}



// ----------------------------------------------------------------------------------------
// BULKIO_dataChar_Out_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataChar_Out_i::BULKIO_dataChar_Out_i(std::string port_name, sourcesocket_base *_parent) :
Port_Uses_base_impl(port_name)
{
    parent = static_cast<sourcesocket_i *> (_parent);
    recConnectionsRefresh = false;
    recConnections.length(0);
}

BULKIO_dataChar_Out_i::~BULKIO_dataChar_Out_i()
{
}

/*
 * pushSRI
 *     description: send out SRI describing the data payload
 *
 *  H: structure of type BULKIO::StreamSRI with the SRI for this stream
 *    hversion
 *    xstart: start time of the stream
 *    xdelta: delta between two samples
 *    xunits: unit types from Platinum specification
 *    subsize: 0 if the data is one-dimensional
 *    ystart
 *    ydelta
 *    yunits: unit types from Platinum specification
 *    mode: 0-scalar, 1-complex
 *    streamID: stream identifier
 *    sequence<CF::DataType> keywords: unconstrained sequence of key-value pairs for additional description
 */
void BULKIO_dataChar_Out_i::pushSRI(const BULKIO::StreamSRI& H)
{
    std::vector < std::pair < BULKIO::dataChar_var, std::string > >::iterator i;

    boost::mutex::scoped_lock lock(updatingPortsLock);   // don't want to process while command information is coming in

    if (active) {
        for (i = outConnections.begin(); i != outConnections.end(); ++i) {
            try {
                ((*i).first)->pushSRI(H);
            } catch(...) {
                std::cout << "Call to pushSRI by BULKIO_dataChar_Out_i failed" << std::endl;
            }
        }
    }

    currentSRIs[std::string(H.streamID)] = H;
    refreshSRI = false;

    return;
}



// ----------------------------------------------------------------------------------------
// BULKIO_dataDouble_Out_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataDouble_Out_i::BULKIO_dataDouble_Out_i(std::string port_name, sourcesocket_base *_parent) :
Port_Uses_base_impl(port_name)
{
    parent = static_cast<sourcesocket_i *> (_parent);
    recConnectionsRefresh = false;
    recConnections.length(0);
}

BULKIO_dataDouble_Out_i::~BULKIO_dataDouble_Out_i()
{
}

/*
 * pushSRI
 *     description: send out SRI describing the data payload
 *
 *  H: structure of type BULKIO::StreamSRI with the SRI for this stream
 *    hversion
 *    xstart: start time of the stream
 *    xdelta: delta between two samples
 *    xunits: unit types from Platinum specification
 *    subsize: 0 if the data is one-dimensional
 *    ystart
 *    ydelta
 *    yunits: unit types from Platinum specification
 *    mode: 0-scalar, 1-complex
 *    streamID: stream identifier
 *    sequence<CF::DataType> keywords: unconstrained sequence of key-value pairs for additional description
 */
void BULKIO_dataDouble_Out_i::pushSRI(const BULKIO::StreamSRI& H)
{
    std::vector < std::pair < BULKIO::dataDouble_var, std::string > >::iterator i;

    boost::mutex::scoped_lock lock(updatingPortsLock);   // don't want to process while command information is coming in

    if (active) {
        for (i = outConnections.begin(); i != outConnections.end(); ++i) {
            try {
                ((*i).first)->pushSRI(H);
            } catch(...) {
                std::cout << "Call to pushSRI by BULKIO_dataDouble_Out_i failed" << std::endl;
            }
        }
    }

    currentSRIs[std::string(H.streamID)] = H;
    refreshSRI = false;

    return;
}



// ----------------------------------------------------------------------------------------
// BULKIO_dataFloat_Out_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataFloat_Out_i::BULKIO_dataFloat_Out_i(std::string port_name, sourcesocket_base *_parent) :
Port_Uses_base_impl(port_name)
{
    parent = static_cast<sourcesocket_i *> (_parent);
    recConnectionsRefresh = false;
    recConnections.length(0);
}

BULKIO_dataFloat_Out_i::~BULKIO_dataFloat_Out_i()
{
}

/*
 * pushSRI
 *     description: send out SRI describing the data payload
 *
 *  H: structure of type BULKIO::StreamSRI with the SRI for this stream
 *    hversion
 *    xstart: start time of the stream
 *    xdelta: delta between two samples
 *    xunits: unit types from Platinum specification
 *    subsize: 0 if the data is one-dimensional
 *    ystart
 *    ydelta
 *    yunits: unit types from Platinum specification
 *    mode: 0-scalar, 1-complex
 *    streamID: stream identifier
 *    sequence<CF::DataType> keywords: unconstrained sequence of key-value pairs for additional description
 */
void BULKIO_dataFloat_Out_i::pushSRI(const BULKIO::StreamSRI& H)
{
    std::vector < std::pair < BULKIO::dataFloat_var, std::string > >::iterator i;

    boost::mutex::scoped_lock lock(updatingPortsLock);   // don't want to process while command information is coming in

    if (active) {
        for (i = outConnections.begin(); i != outConnections.end(); ++i) {
            try {
                ((*i).first)->pushSRI(H);
            } catch(...) {
                std::cout << "Call to pushSRI by BULKIO_dataFloat_Out_i failed" << std::endl;
            }
        }
    }

    currentSRIs[std::string(H.streamID)] = H;
    refreshSRI = false;

    return;
}



// ----------------------------------------------------------------------------------------
// BULKIO_dataLong_Out_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataLong_Out_i::BULKIO_dataLong_Out_i(std::string port_name, sourcesocket_base *_parent) :
Port_Uses_base_impl(port_name)
{
    parent = static_cast<sourcesocket_i *> (_parent);
    recConnectionsRefresh = false;
    recConnections.length(0);
}

BULKIO_dataLong_Out_i::~BULKIO_dataLong_Out_i()
{
}

/*
 * pushSRI
 *     description: send out SRI describing the data payload
 *
 *  H: structure of type BULKIO::StreamSRI with the SRI for this stream
 *    hversion
 *    xstart: start time of the stream
 *    xdelta: delta between two samples
 *    xunits: unit types from Platinum specification
 *    subsize: 0 if the data is one-dimensional
 *    ystart
 *    ydelta
 *    yunits: unit types from Platinum specification
 *    mode: 0-scalar, 1-complex
 *    streamID: stream identifier
 *    sequence<CF::DataType> keywords: unconstrained sequence of key-value pairs for additional description
 */
void BULKIO_dataLong_Out_i::pushSRI(const BULKIO::StreamSRI& H)
{
    std::vector < std::pair < BULKIO::dataLong_var, std::string > >::iterator i;

    boost::mutex::scoped_lock lock(updatingPortsLock);   // don't want to process while command information is coming in

    if (active) {
        for (i = outConnections.begin(); i != outConnections.end(); ++i) {
            try {
                ((*i).first)->pushSRI(H);
            } catch(...) {
                std::cout << "Call to pushSRI by BULKIO_dataLong_Out_i failed" << std::endl;
            }
        }
    }

    currentSRIs[std::string(H.streamID)] = H;
    refreshSRI = false;

    return;
}



// ----------------------------------------------------------------------------------------
// BULKIO_dataOctet_Out_i definition
// ----------------------------------------------------------------------------------------
BULKIO_dataOctet_Out_i::BULKIO_dataOctet_Out_i(std::string port_name, sourcesocket_base *_parent) :
Port_Uses_base_impl(port_name)
{
    parent = static_cast<sourcesocket_i *> (_parent);
    recConnectionsRefresh = false;
    recConnections.length(0);
}

BULKIO_dataOctet_Out_i::~BULKIO_dataOctet_Out_i()
{
}

/*
 * pushSRI
 *     description: send out SRI describing the data payload
 *
 *  H: structure of type BULKIO::StreamSRI with the SRI for this stream
 *    hversion
 *    xstart: start time of the stream
 *    xdelta: delta between two samples
 *    xunits: unit types from Platinum specification
 *    subsize: 0 if the data is one-dimensional
 *    ystart
 *    ydelta
 *    yunits: unit types from Platinum specification
 *    mode: 0-scalar, 1-complex
 *    streamID: stream identifier
 *    sequence<CF::DataType> keywords: unconstrained sequence of key-value pairs for additional description
 */
void BULKIO_dataOctet_Out_i::pushSRI(const BULKIO::StreamSRI& H)
{
    std::vector < std::pair < BULKIO::dataOctet_var, std::string > >::iterator i;

    boost::mutex::scoped_lock lock(updatingPortsLock);   // don't want to process while command information is coming in

    if (active) {
        for (i = outConnections.begin(); i != outConnections.end(); ++i) {
            try {
                ((*i).first)->pushSRI(H);
            } catch(...) {
                std::cout << "Call to pushSRI by BULKIO_dataOctet_Out_i failed" << std::endl;
            }
        }
    }

    currentSRIs[std::string(H.streamID)] = H;
    refreshSRI = false;

    return;
}



