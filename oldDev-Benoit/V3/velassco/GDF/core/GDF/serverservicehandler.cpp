

#include "serverservicehandler.h"

ServerServiceHandler::ServerServiceHandler()
{
}
void ServerServiceHandler::update()
{
    printcli("#### update status ####\n");

    printcli("#### _update status_ ####\n");
}

void ServerServiceHandler::getStatusOfAllNodes(message& _return)
{

    printcli("#### getStatusOfAllNodes####\n");
    QString temp;
    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {

            temp.append(
                        service.at(j)->getListOfnodes().at(i).name
                        );
            temp.append(": ");
            temp.append(
                        QString::number(service.at(j)->getListOfnodes().at(i).state)
                        );
            temp.append("\n");

        }
    }
    _return.message1=temp.toStdString();
    printcli("#### _getStatusOfAllNodes_ ####\n");
}

void ServerServiceHandler::getStatusOfNode(message& _return, const std::string& nodeName)
{
    printcli("#### getStatusOfNode ####\n");
    QString temp;
    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {
            if(
                    service.at(j)->getListOfnodes().at(i).name.compare
                    (QString(nodeName.c_str()), Qt::CaseInsensitive) == 0

                    )
            {
                temp.append(
                            service.at(j)->getListOfnodes().at(i).name
                            );
                temp.append(": ");
                temp.append(
                            QString::number(service.at(j)->getListOfnodes().at(i).state)
                            );
                temp.append("\n");
            }
        }
    }

    _return.message1=temp.toStdString();

    printcli("#### _getStatusOfNode_ ####\n");

}

void ServerServiceHandler::getStatusOfAllServicesOnNode(message& _return, const std::string& nodeName)
{
    printcli("#### getStatusOfAllServicesOnNode ####\n");
    QString temp;
    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {
            if(
                    service.at(j)->getListOfnodes().at(i).name.compare
                    (QString(nodeName.c_str()), Qt::CaseInsensitive) == 0)

            {
                for(uint k = 0; k < service.at(j)->getListOfServices()[i].size(); k++)
                {
                    if(service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == -1)
                    {
                        temp.append(
                                    service.at(j)->getListOfServices()[i].at(k).service
                                    );
                        temp.append(": ");
                        temp.append(
                                    QString::number(service.at(j)->getListOfServices()[i].at(k).pid)
                                    );
                        temp.append("\n");
                    }
                }
            }
        }
    }
    _return.message1=temp.toStdString();
    printcli("#### _getStatusOfAllServicesOnNode_ ####\n");
}

void ServerServiceHandler::getStatusOfServiceOnNode(message& _return, const std::string& nodeName, const std::string& serviceName) {

    printcli("#### getStatusOfServiceOnNode####\n");
    QString temp;
    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {
            if(
                    service.at(j)->getListOfnodes().at(i).name.compare
                    (QString(nodeName.c_str()), Qt::CaseInsensitive) == 0
                    )
            {
                for(uint k = 0; k < service.at(j)->getListOfServices()[i].size(); k++)
                {
                    if(
                            service.at(j)->getListOfServices()[i].at(k).service.compare
                            (QString(serviceName.c_str()), Qt::CaseInsensitive) == 0
                            &&
                            service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == -1
                            )
                    {
                        temp.append(
                                    QString::number(service.at(j)->getListOfServices()[i].at(k).pid)
                                    );
                        temp.append("\n");
                    }
                }
            }
        }
    }
    if(temp.size() == 0)
    {
        temp.append("-1\n");
    }
    _return.message1=temp.toStdString();
    printcli("#### _getStatusOfServiceOnNode_ ####\n");
}

void ServerServiceHandler::startAllServicesOnAllNodes()
{
    printcli("#### startAllServicesOnAllNodes ####\n");
    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {

            for(uint k = 0; k < service.at(j)->getListOfServices()[i].size(); k++)
            {
                if(service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == -1 || service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == 0)
                {

                    QStringList args;
                    args << service.at(j)->getListOfnodes().at(i).name;

                    if(service.at(j)->getListOfServices()[i].at(k).cmd.start !="")
                    {
                        args <<service.at(j)->getListOfServices()[i].at(k).cmd.start ;
                    }
                    else
                    {
                        args << service.at(j)->getListOfServices()[i].at(k).service;
                    }

                    if (service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == 0)
                    {
                        service.at(j)->execOnce(i,k);

                    }

                    execCommand("ssh",args);
                }
            }

        }
    }
    printcli("#### _startAllServicesOnAllNodes_ ####\n");
}

void ServerServiceHandler::startAllServicesOnANode(const std::string& nodeName) {

    printcli("#### startAllServicesOnANode ####\n");
    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {
            if(
                    service.at(j)->getListOfnodes().at(i).name.compare
                    (QString(nodeName.c_str()), Qt::CaseInsensitive) == 0)
            {
                for(uint k = 0; k < service.at(j)->getListOfServices()[i].size(); k++)
                {
                    if(service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == -1 || service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == 0)
                    {
                        QStringList args;
                        args << nodeName.c_str();
                        if(service.at(j)->getListOfServices()[i].at(k).cmd.start !="")
                        {
                            args <<service.at(j)->getListOfServices()[i].at(k).cmd.start ;
                        }
                        else
                        {
                            args << service.at(j)->getListOfServices()[i].at(k).service;
                        }
                        if (service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == 0)
                        {
                            service.at(j)->execOnce(i,k);
                        }
                        execCommand("ssh",args);
                    }

                }
            }
        }
    }
    printcli("#### _startAllServicesOnANode_ ####\n");
}

void ServerServiceHandler::startAServiceOnANode(const std::string& nodeName, const std::string& serviceName) {

    printcli("#### startAServiceOnANode ####\n");

    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {
            if(
                    service.at(j)->getListOfnodes().at(i).name.compare
                    (QString(nodeName.c_str()), Qt::CaseInsensitive) == 0)
            {
                for(uint k = 0; k < service.at(j)->getListOfServices()[i].size(); k++)
                {
                    if(
                            service.at(j)->getListOfServices()[i].at(k).service.compare
                            (QString(serviceName.c_str()), Qt::CaseInsensitive) == 0)
                    {
                        if(service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == -1 || service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == 0)
                        {
                            QStringList args;
                            args << nodeName.c_str();

                            if(service.at(j)->getListOfServices()[i].at(k).cmd.start !="")
                            {
                                args <<service.at(j)->getListOfServices()[i].at(k).cmd.start ;
                            }
                            else
                            {
                                args << service.at(j)->getListOfServices()[i].at(k).service;
                            }

                            if (service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == 0)
                            {
                                service.at(j)->execOnce(i,k);
                            }
                            execCommand("ssh",args);
                        }
                    }
                }
            }
        }
    }
    printcli("#### _startAServiceOnANode_ ####\n");
}

void ServerServiceHandler::stopAllServicesOnAllNodes() {

    printcli("#### stopAllServicesOnAllNodes ####\n");
    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {
            for(uint k = 0; k < service.at(j)->getListOfServices()[i].size(); k++)
            {
                if(service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == -1)
                {
                    QStringList args;
                    args << service.at(j)->getListOfnodes().at(i).name;

                    if(service.at(j)->getListOfServices()[i].at(k).cmd.stop !="")
                    {
                        args <<service.at(j)->getListOfServices()[i].at(k).cmd.stop ;
                    }
                    else
                    {
                        args << "kill";
                        args << "-9";
                        args << QString::number(service.at(j)->getListOfServices()[i].at(k).pid);
                    }
                    execCommand("ssh",args);
                }
            }
        }
    }
    printcli("#### _stopAllServicesOnAllNodes_ ####\n");
}

void ServerServiceHandler::stopAllServicesOnANode(const std::string& nodeName) {

    printcli("#### stopAllServicesOnANode ####\n");
    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {
            if(
                    service.at(j)->getListOfnodes().at(i).name.compare
                    (QString(nodeName.c_str()), Qt::CaseInsensitive) == 0)
            {
                for(uint k = 0; k < service.at(j)->getListOfServices()[i].size(); k++)
                {
                    if(service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == -1)
                    {
                        QStringList args;
                        args << nodeName.c_str();

                        if(service.at(j)->getListOfServices()[i].at(k).cmd.stop !="")
                        {
                            args <<service.at(j)->getListOfServices()[i].at(k).cmd.stop ;
                        }
                        else
                        {
                            args << "kill";
                            args << "-9";
                            args << QString::number(service.at(j)->getListOfServices()[i].at(k).pid);
                        }
                        execCommand("ssh",args);
                    }
                }
            }
        }
    }
    printcli("#### _stopAllServicesOnANode_ ####\n");
}

void ServerServiceHandler::stopAServiceOnANode(const std::string& nodeName, const std::string& serviceName) {

    printcli("#### stopAServiceOnANode ####\n");
    for(uint j = 0; j < service.size(); j++)
    {
        for(uint i = 0; i < service.at(j)->getListOfnodes().size(); i++)
        {
            if(
                    service.at(j)->getListOfnodes().at(i).name.compare
                    (QString(nodeName.c_str()), Qt::CaseInsensitive) == 0)
            {
                for(uint k = 0; k < service.at(j)->getListOfServices()[i].size(); k++)
                {
                    if(
                            service.at(j)->getListOfServices()[i].at(k).service.compare
                            (QString(serviceName.c_str()), Qt::CaseInsensitive) == 0)
                    {
                        if(service.at(j)->getListOfServices()[i].at(k).cmd.execOnce == -1)
                        {
                            QStringList args;
                            args << nodeName.c_str();

                            if(service.at(j)->getListOfServices()[i].at(k).cmd.stop !="")
                            {
                                args <<service.at(j)->getListOfServices()[i].at(k).cmd.stop ;
                            }
                            else
                            {
                                args << "kill";
                                args << "-9";
                                args << QString::number(service.at(j)->getListOfServices()[i].at(k).pid);
                            }
                            execCommand("ssh",args);
                        }
                    }
                }
            }
        }
    }
    printcli("#### _stopAServiceOnANode_ ####\n");
}

void ServerServiceHandler::getGDFConfigurationFile(message& _return, const std::string& fileName) {
    printcli("#### getGDFConfigurationFile ####\n");
    QString path = getenv ("HADOOP_CONF_DIR");
    path.append("/");
    path.append(fileName.c_str());

    QString temp;

    string STRING;
    ifstream infile;
    infile.open (path.toStdString().c_str());
    if(infile.is_open())
    {
        while(!infile.eof()) // To get you all the lines.
        {
            getline(infile,STRING); // Saves the line in STRING.
            temp.append(STRING.c_str());
            temp.append("####\n");
        }
        infile.close();
        _return.message1 =temp.toStdString();
    }
    else
    {
        infile.close();
        _return.message1 ="-1";
    }

    printcli("#### _getGDFConfigurationFile_ ####\n");
}

void ServerServiceHandler::setGDFConfigurationFile(const std::string& fileName, const std::string& message)
{
    printcli("#### setGDFConfigurationFile ####\n");
    QString path = getenv ("HADOOP_CONF_DIR");
    path.append("/");


    //Create a backup Dir
    QStringList args;
    args << "-p";
    QString temp = path;
    temp.append("/backup/");
    args << temp;

    execCommand("mkdir",args);

    //cp file
    path.append(fileName.c_str());
    temp.append(fileName.c_str());
    args = QStringList();
    args << path;
    args << temp;
    execCommand("cp",args);
    args = QStringList();
    args << path;
    execCommand("rm",args);

    //write new config file
    ofstream myfile;
    myfile.open (path.toStdString().c_str());
    myfile << message;
    myfile.close();

    printcli("#### _setGDFConfigurationFile_ ####\n");
}

void ServerServiceHandler::query(message& _return, const message& querry)
{
    printcli("#### query ####\n");

    QString messageTemp;
    messageTemp.append(querry.message1.c_str());
    if(messageTemp.contains("exit") || messageTemp.contains("quit"))
    {
        _return.message1 ="1";
        lock.lock();
        s->write();
        lock.unlock();
        exit(0);
    }
    else if(messageTemp.contains("dumpConfig"))
    {
        lock.lock();
        _return.message1 = s->doc.toString().toStdString();
        lock.unlock();
    }
    else if(messageTemp.contains("setConfig"))
    {
        lock.lock();
        messageTemp.clear();
        messageTemp.append(querry.message2.c_str());

        stopAllServicesOnAllNodes();



        s->doc.clear();
        s->doc.setContent(messageTemp);

        //delete services
        service.erase (service.begin(),service.end());
        //parse services
        s->reload();

        startAllServicesOnAllNodes();
        _return.message1 ="1";
        lock.unlock();
    }
    for( uint i = 0; i < service.size(); i++)
    {
        QString messageTemp;
        messageTemp.append(querry.message1.c_str());
        if(messageTemp.contains("Hadoop") && service.at(i)->getServiceName().contains("Hadoop") )
        {
            ((opHadoop *)service.at(i))->query(_return, querry);
        }
        else if(messageTemp.contains("FS")  && service.at(i)->getServiceName().contains("FS") )
        {
            ((opFS *)service.at(i))->query(_return, querry);
        }
        else if(messageTemp.contains("Hbase")  && service.at(i)->getServiceName().contains("Hbase") )
        {
            ((ophbase *)service.at(i))->query(_return, querry);
        }
        else if(messageTemp.contains("Hive")  && service.at(i)->getServiceName().contains("Hive") )
        {
            ((ophive*)service.at(i))->query(_return, querry);
        }

    }
    printcli("#### _query_ ####\n");
}

void ServerServiceHandler::ping(message& _return)
{
    printcli("#### Ping() ########\n");
    _return.message1 ="ping";




    printcli("#### _Ping_ ####\n");
}
/*vector< operate* >  ServerServiceHandler::getServices()
{
    return service;
}*/
