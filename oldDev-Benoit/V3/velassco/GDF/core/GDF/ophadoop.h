#ifndef OPHADOOP_H
#define OPHADOOP_H
#include "header.h"
#include "operate.h"
class opHadoop : public operate
{
public:
    opHadoop();
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

#endif // OPHADOOP_H
