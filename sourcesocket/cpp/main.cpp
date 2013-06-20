#include <iostream>
#include "ossie/ossieSupport.h"

#include "sourcesocket.h"

 int main(int argc, char* argv[])
{
    sourcesocket_i* sourcesocket_servant;
    Resource_impl::start_component(sourcesocket_servant, argc, argv);
}
