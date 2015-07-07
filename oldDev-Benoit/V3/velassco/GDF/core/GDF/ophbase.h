#ifndef OPHBASE_H
#define OPHBASE_H
#include "header.h"
#include "operate.h"


#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../../hbase/gen-cpp2/THBaseService.h"
#include "../../hbase/gen-cpp/Hbase.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::apache::hadoop::hbase::thrift2;
using namespace  ::apache::hadoop::hbase::thrift;

typedef std::map<std::string,TCell> CellMap;

class ophbase : public operate
{
public:
    ophbase();
    void demo();
    void download ();
    void install();
    void conf();


    int writeFile(QString path,QString filename, QString content);
    QString readFile(QString path,QString filename);
    QString grepFile(QString path,QString filename,  QString content);
    QString lsFile(QString path);

     void query(message& _return, const message& querry);
     std::vector<string> *columnName;



};

#endif // OPHBASE_H
