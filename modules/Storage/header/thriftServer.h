#ifndef thriftServer_H
#define thriftServer_H

#include "VELaSSCo.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;


class thriftServer
{
public:
    thriftServer();
    ~thriftServer();
    
private:
    int port;
    
    
};

#endif
