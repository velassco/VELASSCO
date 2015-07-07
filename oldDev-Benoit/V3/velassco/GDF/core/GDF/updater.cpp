#include "updater.h"

updater::updater(QObject *parent) :
    QThread(parent)
{
    moveToThread(this);
}
void updater::run()
{

    while(1)
    {
       // lock.lock();
        updateServices();
        saveConf();
        //lock.unlock();

        this->sleep(10);
    }
}
void updater::updateServices()
{
    printcli("#### updater::updateServices ####\n");
    for(uint i = 0; i < service.size(); i++)
    {
        service.at(i)->updateNodesStatus();
        service.at(i)->checkServices();
        service.at(i)->listServices();
    }
    printcli("#### _updater::updateServices_ ####\n");
}

void updater::saveConf()
{
    QDomElement root;
    s->doc.clear();
    root = s->doc.createElement("configuration");
    s->doc.appendChild(root);
    //Services
    for(uint i = 0; i < service.size(); i++)
    {
        root.appendChild(service.at(i)->writeXML());
    }
}
