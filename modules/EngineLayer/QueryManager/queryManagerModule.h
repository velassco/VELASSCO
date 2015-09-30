#ifndef queryManagerModule_H
#define queryManagerModule_H

#include <iostream> // pour std::cout
#include <string>   // pour std::string


#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;
using namespace std;


#include "VELaSSCo.h"

class queryManagerModule
{
public:
    static queryManagerModule* Instance();
    bool startConnection( const char *data_layer_hostname, const int data_layer_port);
    bool stopConnection();
    void getStatusDB( std::string& _return);
    void getResultFormVerticesID(std::string& _return, std::string sessionID, std::string modelID, std::string analysisID, double timeStep, std::string resultID,  std::string listOfVertices) ;
    void stopAll();
    
private:
    queryManagerModule(){};
    queryManagerModule(queryManagerModule const&){};
    queryManagerModule& operator=(queryManagerModule const&){};
    
    static queryManagerModule* m_pInstance;
    VELaSSCoClient* cli;
    
    
    boost::shared_ptr<TTransport> socket;
    boost::shared_ptr<TTransport> transport;
    boost::shared_ptr<TProtocol> protocol;
    


};

#endif
