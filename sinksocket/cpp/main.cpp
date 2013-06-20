#include <iostream>
#include "ossie/ossieSupport.h"

#include "sinksocket.h"

 int main(int argc, char* argv[])
{
    sinksocket_i* sinksocket_servant;
    Resource_impl::start_component(sinksocket_servant, argc, argv);
}
