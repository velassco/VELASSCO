#ifndef STORAGEMODULE_H
#define STORAGEMODULE_H

#include <iostream> // pour std::cout
#include <string>   // pour std::string
#include <vector>
#include <random>
#include <math.h> 

/////////////////////////////////
// Thrift
/////////////////////////////////
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

/////////////////////////////////
// HBase Thrift 1
/////////////////////////////////
#include "Hbase.h"
using namespace  ::apache::hadoop::hbase::thrift;
typedef std::map<std::string,TCell> CellMap;

/////////////////////////////////
// Base 64
/////////////////////////////////
#include "base64.h"

using namespace std;
class storageModule
{
public:
    static storageModule* Instance();
    string getResultOnVertices( std::string sessionID,
                               std::string modelID,
                               std::string analysisID,
                               double timeStep,
                               std::string resultID,
                               std::string listOfVertices);
    
    string checkIfAllVerticesArePresent(std::string listOfVertices, string contentToCheck);

private:
    storageModule(){};
    storageModule(storageModule const&){};
    storageModule& operator=(storageModule const&){};
    static storageModule* m_pInstance;
    double fRand(double fMin, double fMax);
    
    string parse1DEM(string, string);

};

#endif
