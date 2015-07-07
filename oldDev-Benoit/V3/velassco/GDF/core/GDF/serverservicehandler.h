#ifndef SERVERSERVICEHANDLER_H
#define SERVERSERVICEHANDLER_H

#include "header.h"

#include "deploy.h"


#include <iostream>
#include <fstream>
#include <string>

#include "../../own/gen-cpp/ServerService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>


#include <QThread>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

#include <QMutex>
#include "operate.h"
#include "ophadoop.h"
#include "opFS.h"
#include "ophbase.h"
#include "ophive.h"


#include "serialize.h"
extern vector< operate* > service;
extern QMutex lock;
extern serialize* s;

class ServerServiceHandler  : virtual public ServerServiceIf
{
public:
    ServerServiceHandler();

    void getStatusOfAllNodes(message& _return) ;

    void getStatusOfNode(message& _return, const std::string& nodeName) ;

    void getStatusOfAllServicesOnNode(message& _return, const std::string& nodeName) ;

    void getStatusOfServiceOnNode(message& _return, const std::string& nodeName, const std::string& serviceName) ;

    void startAllServicesOnAllNodes() ;

    void startAllServicesOnANode(const std::string& nodeName) ;

    void startAServiceOnANode(const std::string& nodeName, const std::string& serviceName) ;

    void stopAllServicesOnAllNodes() ;

    void stopAllServicesOnANode (const std::string& nodeName) ;

    void stopAServiceOnANode (const std::string& nodeName, const std::string& serviceName) ;

    void getGDFConfigurationFile(message& _return, const std::string& fileName) ;

    void setGDFConfigurationFile(const std::string& fileName, const std::string& message) ;

    void query(message& _return, const message& querry);

    void ping(message& _return) ;


    void update();

    //vector< operate* > getServices();
private:
    //vector< operate* > service;

};
#endif // SERVERSERVICEHANDLER_H
