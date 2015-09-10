/* -*- C++ -*- */
#ifndef hbaseaccess_H
#define hbaseaccess_H

#include "abstractAcces.h"
#include "Hbase.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>


using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::apache::hadoop::hbase::thrift;

typedef std::map<std::string,TCell> CellMap;

class hbaseaccess : public abstractAcces
{
public:
    hbaseaccess();
    ~hbaseaccess();
    
    int create(char *path, char *filename, char *data);
    const char* cat(char *path, char *filename);
    const char* grep(char *path, char *filename, char *dataToFind);
    
    char* GetResultOnVerticesId(
                                  const char* session_Id,
                                  const char* model_Id,
                                  const char* analysis_Id,
                                  const double timeStep,
                                  const char* result_Id,
                                  // vector<int> vertices
                                  const int *vertices, // should be i64 ...
				  const int num_vertices // should be i64 ...
                                  ) ;
    
    
    
};

#endif
