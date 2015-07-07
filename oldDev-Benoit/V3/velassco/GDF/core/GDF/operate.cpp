#include "operate.h"

operate::operate()
{
    psCMD = " ps -ax";
}

void operate::updateNodesStatus()
{
    for(uint i = 0; i < nodes.size(); i ++)
    {
        QString ps;
        QString exe = "ssh";

        QStringList args;
        args<<nodes.at(i).name;
        args<<"uptime";
        ps = execCommand(exe, args);

        if(ps.contains("up"))
        {
            nodes.at(i).state = true;
        }
        else
        {
            nodes.at(i).state = false;
        }
    }
}

void operate::checkServices()
{
    for(uint i = 0; i < nodes.size(); i ++)
    {
        QString ps;
        QString exe = "ssh";

        QStringList args;
        args<<nodes.at(i).name;
        args<<psCMD;

        ps = execCommand(exe, args);
        QStringList qsl = ps.split(QRegExp("\n|\r\n|\r"));


        for(uint j = 0; j < services[i].size();j++)
        {
            int k = 0;
            bool test = false;
            while (k < qsl.size() && !test)
            {
                if(qsl.at(k).contains(services[i].at(j).service))
                {
                    QStringList qslTemp = qsl.at(k).split(QRegExp(" "));
                    services[i].at(j).pid = qslTemp.at(0).toInt();
                    test = true;
                }
                k++;
            }
            if(k == qsl.size())
            {
                services[i].at(j).pid = -1;
            }

        }
    }
}
void operate::startService(QString node,QString exe, QStringList args)
{
    QString exec = "ssh";
    QStringList argsExe;
    argsExe << node;
    argsExe << exe;
    argsExe << args;

    QString ps = execCommand(exec, argsExe);
}

void operate::stopService(QString node,int pid)
{
    QString exe = "ssh";
    QStringList args;
    args << node;
    args << " kill -9 ";
    args << QString::number(pid);

    QString ps = execCommand(exe, args);
}



void operate::listServices()
{

    for(uint i = 0; i < nodes.size(); i ++)
    {
        if(nodes[i].state)
        {
            cout << "(" << serviceName.toStdString() << ") "<<nodes[i].name.toStdString()<<" : ON"<<endl;
            for(uint j = 0; j < services[i].size();j++)
            {
                if(services[i][j].cmd.execOnce == -1)
                {
                    cout<<"\t" << services[i][j].service.toStdString() <<": "<< services[i][j].pid<<endl;
                }
            }
            cout<<endl;
        }
        else
        {
            cout << nodes[i].name.toStdString()<<" : OFF"<< endl;
        }
    }
}
vector<statusNodes> operate::getListOfnodes()
{
    return nodes;
}

vector< vector<_service> > operate::getListOfServices()
{
    return services;
}

vector<_service> operate::getListOfServices(int id)
{
    return services.at(id);
}

QDomElement operate::writeXML()
{
    QDomDocument tdoc;
    QDomElement xservice =  tdoc.createElement("operation");

    QDomElement xname = tdoc.createElement("name");
    QDomText xtextName = tdoc.createTextNode(serviceName);
    xname.appendChild(xtextName);
    xservice.appendChild(xname);



    QDomElement xpath = tdoc.createElement("path");
    QDomText xtextpath = tdoc.createTextNode(path);
    xpath.appendChild(xtextpath);
    xservice.appendChild(xpath);

    QDomElement xconfDir = tdoc.createElement("confDir");
    QDomText xtextconfDir = tdoc.createTextNode(confDir);
    xconfDir.appendChild(xtextconfDir);
    xservice.appendChild(xconfDir);

    {
        QDomElement xnodes = tdoc.createElement("config");

        xservice.appendChild(xnodes);
        for (uint i = 0; i < confFiles.size(); i ++)
        {
            QDomElement xNode = tdoc.createElement("file");
            xnodes.appendChild(xNode);
            QDomElement xNameNode = tdoc.createElement("name");
            xNode.appendChild(xNameNode);
            QDomText xtextNameNode = tdoc.createTextNode(confFiles.at(i)->name);
            xNameNode.appendChild(xtextNameNode);
            xNode.appendChild(confFiles.at(i)->config);


        }
    }

    {
        QDomElement xnodes = tdoc.createElement("nodes");
        for(uint i = 0; i < nodes.size(); i ++)
        {
            QDomElement xNode = tdoc.createElement("node");

            QDomElement xNameNode = tdoc.createElement("name");
            QDomText xtextNameNode = tdoc.createTextNode(nodes.at(i).name);
            xNameNode.appendChild(xtextNameNode);
            xNode.appendChild(xNameNode);
            xnodes.appendChild(xNode);

            QDomElement xTypeNode = tdoc.createElement("type");
            QDomText xtextTypeNode = tdoc.createTextNode(QString::number(nodes.at(i).type));
            xTypeNode.appendChild(xtextTypeNode);
            xNode.appendChild(xTypeNode);

            QDomElement xNodesServices =  tdoc.createElement("services");
            for(uint j = 0; j < services.at(i).size(); j ++)
            {
                QDomElement xNodeService = tdoc.createElement("service");

                {
                    QDomElement xNodeServiceName = tdoc.createElement("name");
                    QDomText xtextNodeServiceName = tdoc.createTextNode(services.at(i).at(j).service);
                    xNodeServiceName.appendChild(xtextNodeServiceName);
                    xNodeService.appendChild(xNodeServiceName);
                }
                {
                    QDomElement xNodeServiceName = tdoc.createElement("cmd.execOnce");
                    QDomText xtextNodeServiceName = tdoc.createTextNode(QString::number(services.at(i).at(j).cmd.execOnce));
                    xNodeServiceName.appendChild(xtextNodeServiceName);
                    xNodeService.appendChild(xNodeServiceName);
                }
                {
                    QDomElement xNodeServiceName = tdoc.createElement("cmd.start");
                    QDomText xtextNodeServiceName = tdoc.createTextNode(services.at(i).at(j).cmd.start);
                    xNodeServiceName.appendChild(xtextNodeServiceName);
                    xNodeService.appendChild(xNodeServiceName);
                }
                {
                    QDomElement xNodeServiceName = tdoc.createElement("cmd.stop");
                    QDomText xtextNodeServiceName = tdoc.createTextNode(services.at(i).at(j).cmd.stop);
                    xNodeServiceName.appendChild(xtextNodeServiceName);
                    xNodeService.appendChild(xNodeServiceName);
                }
                {
                    QDomElement xNodeServiceName = tdoc.createElement("cmd.update");
                    QDomText xtextNodeServiceName = tdoc.createTextNode(services.at(i).at(j).cmd.update);
                    xNodeServiceName.appendChild(xtextNodeServiceName);
                    xNodeService.appendChild(xNodeServiceName);
                }

                xNodesServices.appendChild(xNodeService);
            }
            xNode.appendChild(xNodesServices);
        }

        xservice.appendChild(xnodes);
    }
    return xservice;
}

void operate::addService(_service stemp)
{
    services.at(services.size()-1).push_back(stemp);
}

void operate::addServiceOnANode(_service stemp, int node)
{
    services.at(node).push_back(stemp);
}

void operate::addNode(QString name, int type)
{
    statusNodes s;
    s.name = name;
s.type = (nodeType)type;
    nodes.push_back(s);
    vector<_service> Vtemp;
    services.push_back(Vtemp );
}

void operate::setPath(QString s)
{
    path = s;
}

void operate::setConfDir(QString s)
{
    confDir = s;
}
void operate::download ()
{
    printcli("#### operate::download ####\n");
    for (uint i = 0; i < nodes.size(); i++)
    {
        QString ps;
        QString exe = "ssh";

        QStringList args;
        {
            args.clear();
            args<<nodes.at(i).name;
            args<<"ls";
            args<<path;
            ps = execCommand(exe, args);
            if(ps.isEmpty())
            {
                args.clear();
                args<<nodes.at(i).name;
                args<<"mkdir";
                args<<"-p";
                args<<path;
                execCommand(exe, args);
            }
        }
        {
            args.clear();
            args<<nodes.at(i).name;
            args<<"ls";
            args<<confDir;
            ps = execCommand(exe, args);
            if(ps.isEmpty())
            {
                args.clear();
                args<<nodes.at(i).name;
                args<<"mkdir";
                args<<"-p";
                args<<confDir;
                execCommand(exe, args);
            }
        }
    }
    printcli("#### _operate::download_ ####\n");
}

void operate::install()
{
    printcli("#### operate::install ####\n");

    printcli("#### _operate::install_ ####\n");
}

void operate::conf()
{
    printcli("#### operate::conf ####\n");
    for (uint i = 0; i < confFiles.size(); i ++)
    {
        QDomDocument doc;

        //doc.importNode(confFiles.at(i)->config);
        QDomNode xmlNode = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
        doc.insertBefore(xmlNode, doc.firstChild());
        xmlNode = doc.createProcessingInstruction("xml-stylesheet","type=\"text/xsl\" href=\"configuration.xsl\"");
        doc.insertAfter(xmlNode, doc.firstChild());

        QDomNode clonedNode = confFiles.at(i)->config.cloneNode(true);
        doc.insertAfter(clonedNode, doc.lastChild());

        QFile file(  "temp/" + confFiles.at(i)->name);
        file.open(QIODevice::WriteOnly);
        QTextStream ts(&file);
        int indent = 2;
        doc.save(ts, indent);
    }

    QString ps;
    QString exe = "scp";

    QStringList args;


    for (uint i = 0; i < nodes.size(); i++)
    {
        for (uint j = 0; j < confFiles.size(); j ++)
        {
            args.clear();
            QString f;
            f.append("temp/");
            f.append(confFiles.at(j)->name);

            args << f;
            QString loc;
            loc.append(nodes.at(i).name);
            loc.append(":");
            loc.append(confDir);
            args <<  loc;
            ps = execCommand(exe, args);
        }
    }
    printcli("#### _operate::conf_ ####\n");
}

QString operate::getName()
{
    return serviceName;
}

void operate::setConfigXML(QDomElement file)
{


    confFiles.push_back(new xml());


    QDomElement fileName = file.firstChild().toElement();
    confFiles.at(confFiles.size() - 1)->name = fileName.text();



    fileName = fileName.nextSibling().toElement();

    QDomElement root = confFiles.at(confFiles.size() - 1)->config.createElement("configuration");
    confFiles.at(confFiles.size() - 1)->config.appendChild(root);
    {
        for( int i = 0; i < fileName.elementsByTagName("name").size(); i++)
        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");


            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode(fileName.elementsByTagName("name").item(i).toElement().text());
            QDomElement value = confFiles.at(confFiles.size() - 1)->config.createElement("value");
            QDomText textValue = confFiles.at(confFiles.size() - 1)->config.createTextNode(fileName.elementsByTagName("value").item(i).toElement().text());
            name.appendChild(textName);
            value.appendChild(textValue);
            property.appendChild(name);
            property.appendChild(value);
            root.appendChild(property);

        }
    }
}

void operate::execOnce(int i , int j)
{
    services.at(i).at(j).cmd.execOnce = 1;
}

QString operate::getServiceName()
{
    return serviceName;
}
