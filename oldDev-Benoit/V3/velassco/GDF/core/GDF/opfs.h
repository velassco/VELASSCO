#ifndef OPFS_H
#define OPFS_H
#include "header.h"
#include "operate.h"
class opFS: public operate
{
public:
    opFS();
    void demo();


    int writeFile(QString path,QString filename, QString content);
    QString readFile(QString path,QString filename);
    QString grepFile(QString path,QString filename,  QString content);
    QString lsFile(QString path);

     void query(message& _return, const message& querry);
};

#endif // OPFS_H

