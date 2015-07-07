#include "deploy.h"

deploy::deploy()
{
    state = INIT;
}

void deploy::demo()
{

}

void deploy::changeState()
{
    printcli("#### deploy::changeState ####\n");
    state++;
    switch (state)
    {
    case DOWNLOAD:
        download();
        changeState();
        break;
    case INSTALL:
        install();
        changeState();
        break;
    case CONFIGURE:
        conf();
        changeState();
        break;
    default:
        break;
    }
    printcli("#### _deploy::changeState_ ####\n");
}

void deploy::download ()
{
    printcli("#### deploy::download ####\n");
    for(uint i = 0; i < service.size(); i++)
    {
        if(service.at(i)->getName().contains("Hadoop"))
        {
            ((opHadoop*)service.at(i))->download();
        }
        else if(service.at(i)->getName().contains("Hbase"))
        {
            ((ophbase*)service.at(i))->download();
        }
        else if(service.at(i)->getName().contains("FS"))
        {
            ((opFS*)service.at(i))->download();
        }
        else if(service.at(i)->getName().contains("Hive"))
        {
            ((ophive*)service.at(i))->download();
        }
        else{
            (service.at(i))->download();
        }
    }
    printcli("#### _deploy::download_ ####\n");
}

void deploy::install()
{
    printcli("#### deploy::install ####\n");
    for(uint i = 0; i < service.size(); i++)
    {
        if(service.at(i)->getName().contains("Hadoop"))
        {
            ((opHadoop*)service.at(i))->install();
        }
        else if(service.at(i)->getName().contains("Hbase"))
        {
            ((ophbase*)service.at(i))->install();
        }
        else if(service.at(i)->getName().contains("Hive"))
        {
   ((ophive*)service.at(i))->install();
        }
        else if(service.at(i)->getName().contains("FS"))
        {
            ((opFS*)service.at(i))->install();
        }

        else{
            (service.at(i))->install();
        }
        //
    }
    printcli("#### _deploy::install_ ####\n");
}

void deploy::conf()
{
    printcli("#### deploy::conf ####\n");
    for(uint i = 0; i < service.size(); i++)
    {
        if(service.at(i)->getName().contains("Hadoop"))
        {
            ((opHadoop*)service.at(i))->conf();
        }
        else if(service.at(i)->getName().contains("Hbase"))
        {
            ((ophbase*)service.at(i))->conf();
        }
        else if(service.at(i)->getName().contains("Hive"))
        {
            ((ophive*)service.at(i))->conf();
        }
        else if(service.at(i)->getName().contains("FS"))
        {
            ((opFS*)service.at(i))->conf();
        }

        else
        {
            (service.at(i))->conf();
        }
    }
    printcli("#### _deploy::conf_ ####\n");
}

QDomElement deploy::writeXML()
{

}

void deploy::readXML()
{

}

void deploy::alreadyDeploy()
{
    state = DONE;
}
