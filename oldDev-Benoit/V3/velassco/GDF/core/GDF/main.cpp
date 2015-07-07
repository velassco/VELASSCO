#include "header.h"

#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <QCoreApplication>
#include <QTimer>


#include "updater.h"
#include"serverlistener.h"
#include "serialize.h"
#include "operate.h"
#include "deploy.h"


#include "ophadoop.h"
#include "opfs.h"
#include "ophbase.h"
#include "ophive.h"

serialize* s;
vector< operate* > service;
deploy* d;
updater *u;
serverListener *SL;
#include <QMutex>
QMutex lock;


QString execCommand(QString exe, QStringList args)
{
    cout<<"\t------- CMD :"<<exe.toStdString()<<" ";
    for(int i = 0; i < args.size(); i++)
    {
        cout<<args.at(i).toStdString()<<" ";
    }
    cout<<"-------"<<endl;
    QProcess process;
    process.start(exe,args);

    process.waitForFinished(-1);
    return process.readAll();
}

void ouch(int sig)
{
    u->saveConf();
    s->write();
    exit(0);
}

int main(int argc, char *argv[])
{

    QDir tempDir;
    tempDir.mkdir("temp");
    struct sigaction act;
    act.sa_handler = ouch;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);

    d = new deploy();
    s = new serialize();
    if(s->read())
    {
        d->alreadyDeploy();
        s->reload();
    }
    else
    {
        {
            opFS *tmp = new opFS();
            service.push_back(tmp);
            tmp->demo();
        }
        {
            opHadoop *tmp = new opHadoop();
            service.push_back(tmp);
            tmp->demo();

        }
        {
            ophbase *tmp = new ophbase();
            service.push_back(tmp);
            tmp->demo();

        }
        {
            ophive *tmp = new ophive();
            service.push_back(tmp);
            tmp->demo();

        }
    }
    u = new updater();
    d->changeState();
    u->start();
    SL = new serverListener();
    SL->start();
   while(1)
   {
       sleep(1000);
   }
    return 0;

}
