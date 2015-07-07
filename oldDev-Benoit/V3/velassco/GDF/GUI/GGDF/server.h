#ifndef SERVER_H
#define SERVER_H
#include <iostream> // pour std::cout
#include <string>   // pour std::string

#include <QProcess>


#include "../../own/gen-cpp/ServerService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;
using namespace std;
class server
{
public:
    server();
    int getCorePid();
    ~server();

    QString getStatusOfServices();
    void startAllServices();
    void stopAllServices();
    QString ping();

    QString write(QString type, QString path, QString filename, QString content);
    QString read(QString type, QString path, QString filename);
    QString grep(QString type, QString path, QString filename, QString content);
    QString ls(QString type, QString path);

    double getTime()
    {
        struct timeval tmp_time;
        gettimeofday(&tmp_time, NULL);
        return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
    };
    void change();
    void startConnection();


private:
    //Network configuration
    string ip;
    string login;
    string pass;

    //CoreApplication
    int corepid;


    boost::shared_ptr<TTransport> socket;
    boost::shared_ptr<TTransport> transport;
    boost::shared_ptr<TProtocol> protocol;

    bool hadoop;

};

#endif // SERVER_H
