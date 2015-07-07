
#ifndef DEPLOY_H
#define DEPLOY_H

#include "header.h"
#include <QStringList>
#include <QProcess>
#include <QtXml>

#include <iostream> // pour std::cout
#include <string>   // pour std::string

#include "operate.h"
#include "ophadoop.h"
#include "ophbase.h"
#include "ophive.h"
#include "opfs.h"
extern vector< operate* > service;
using namespace std;
enum value{INIT,DOWNLOAD, INSTALL, CONFIGURE, DONE};


class deploy
{
public:
    deploy();
    void download ();
    void install();
    void conf();

    void demo();
    void changeState();

    QDomElement writeXML();
    void readXML();

    void alreadyDeploy();
private:
    int state;
    QStringList path;


    vector<QStringList > configParameters;
};

#endif // DEPLOY_H
