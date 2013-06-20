/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

 	Source: sourcesocket.spd.xml
 	Generated on: Thu Jun 20 09:35:33 EDT 2013
 	REDHAWK IDE
 	Version: 1.8.4
 	Build id: R201305151907

*******************************************************************************************/

#include "sourcesocket.h"
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



