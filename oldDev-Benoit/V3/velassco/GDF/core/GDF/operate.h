#ifndef OPERATE_H
#define OPERATE_H

#include "header.h"

#include <QStringList>
#include <QProcess>
#include <QDomElement>
#include <QFile>
#include <QTextStream>

#include <vector>
#include <iostream> // pour std::cout
#include <string>   // pour std::string
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../../own/gen-cpp/ServerService.h"
using namespace std;


enum nodeType{MASTER, SLAVE, SECONDARY};

struct command
{
    int execOnce;
    QString start;
    QString stop;
    QString update;
};
struct _service
{
    int pid;
    QString service;
    command cmd;

};
struct statusNodes
{
    QString name;
    nodeType type;
    bool state;
};

struct xml
{
    QString name;
    QDomDocument config;
};

class operate
{
public:
    operate();

    vector<statusNodes> getListOfnodes();
    vector< vector<_service> > getListOfServices();
    vector<_service> getListOfServices(int id);


    void addNode(QString name, int type);
    void addService(_service);
    void addServiceOnANode(_service, int node);

    QString gestMaster();
    QString getSecondary();
    void updateNodesStatus();


    void checkServices();
    void listServices();

    void startService(QString node,QString exe, QStringList args);
    void stopService(QString node,int pid);

    QDomElement writeXML();

    void download ();
    void install();
    void conf();

    void setPath(QString);
    void setConfDir(QString);

    QString getName();

    void setConfigXML(QDomElement file);
    void execOnce(int, int);

    int writeFile(QString path,QString filename, QString content);
    QString readFile(QString path,QString filename);
    QString grepFile(QString path,QString filename,  QString content);
    QString lsFile(QString path);

    QString getServiceName();

     void query(message& _return, const message& querry);

protected:

    QString serviceName;
    QString path;
    QString confDir;
    QString varConfDir;
    QString dll;
    QString psCMD;

    vector<statusNodes> nodes;

    vector< vector<_service> > services;

    vector<operate> needs;
    vector<xml *> confFiles;
};

#endif // OPERATE_H
