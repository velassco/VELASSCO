#ifndef OPHIVE_H
#define OPHIVE_H

#include "header.h"
#include "operate.h"
#include "../../hive/thrift-hive/hiveclient.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#define RETURN_ON_ASSERT_ONE_ARG_CLOSE(condition, err_format, arg, db_conn, ret_val) {          \
if (condition) {                                                                              \
char error_buffer_[MAX_HIVE_ERR_MSG_LEN];                                                   \
fprintf(stderr, "----LINE %i: ", __LINE__);                                                 \
fprintf(stderr, err_format, arg);                                                           \
DBCloseConnection(db_conn, error_buffer_, sizeof(error_buffer_));                           \
}                                                                                             \
}
#define RETURN_ON_ASSERT_ONE_ARG(condition, err_format, arg, ret_val) {                         \
if (condition) {                                                                              \
fprintf(stderr, "----LINE %i: ", __LINE__);                                                 \
fprintf(stderr, err_format, arg);                                                           \
\
}                                                                                             \
}
#define MAX_FIELD_LEN 255

class ophive : public operate
{
public:
    ophive();
    void demo();
    void download ();
    void install();
    void conf();


    int writeFile(QString path,QString filename, QString content);
    QString readFile(QString path,QString filename);
    QString grepFile(QString path,QString filename,  QString content);
    QString lsFile(QString path);

     void query(message& _return, const message& querry);
};

#endif // OPHIVE_H
