#include "serialize.h"

serialize::serialize()
{

    QDomElement root;
    root = doc.createElement("configuration");
    doc.appendChild(root);

}

void serialize::write()
{
    QDomNode xmlNode = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, doc.firstChild());

    QFile file( "config.xml");
    file.open(QIODevice::WriteOnly);
    QTextStream ts(&file);
    int indent = 2;
    doc.save(ts, indent);
    file.close();
}
void serialize::reload()
{
    QDomElement root=doc.documentElement();
    QDomElement Component=root.firstChild().toElement();
    while(!Component.isNull())
    {
        QDomElement Child=Component.firstChild().toElement();
        while (!Child.isNull())
        {
            if (Child.tagName()=="name")
            {
                if(Child.firstChild().toText().data().contains("Hadoop"))
                {
                    service.push_back(new opHadoop());
                }
                else if(Child.firstChild().toText().data().contains("FS"))
                {
                    service.push_back(new opFS());
                }
                else if(Child.firstChild().toText().data().contains("Hbase"))
                {
                    service.push_back(new ophbase());
                }
                else if(Child.firstChild().toText().data().contains("Hive"))
                {
                    service.push_back(new ophive());
                }

                else
                {
                    service.push_back(new operate());
                }
                //cout<<Child.firstChild().toText().data().toStdString()<<endl;
            }
            else if (Child.tagName()=="confDir")
            {
                service.at(service.size()-1)->setConfDir(Child.firstChild().toText().data());
            }
            else if (Child.tagName()=="path")
            {
                service.at(service.size()-1)->setPath(Child.firstChild().toText().data());
            }
            else if (Child.tagName()=="config")
            {
                QDomElement files=Child.firstChild().toElement();
                while (!files.isNull())
                {
                    service.at(service.size()-1)->setConfigXML(files);
                    files = files.nextSibling().toElement();
                }

            }
            else if (Child.tagName()=="nodes")
            {

                QDomElement nodes=Child.firstChild().toElement();
                while (!nodes.isNull())

                {
                    QDomElement node=nodes.firstChild().toElement();
                    QString temp = node.text();
                    node = node.nextSibling().toElement();
                    service.at(service.size()-1)->addNode(temp, node.text().toInt());

                    QDomElement nodeServices=node.nextSibling().toElement();
                    QDomElement servis=nodeServices.firstChild().toElement();
                    while (!servis.isNull())
                    {

                        QDomElement servi=servis.firstChild().toElement();
                        _service stemp;
                        while (!servi.isNull())
                        {

                            if(servi.tagName().contains("name"))
                            {
                                stemp.service=servi.text();
                            }
                            else if(servi.tagName().contains("cmd.start"))
                            {
                                stemp.cmd.start=servi.text();
                            }
                            else if(servi.tagName().contains("cmd.stop"))
                            {

                                stemp.cmd.stop=servi.text();
                            }
                            else if(servi.tagName().contains("cmd.update"))
                            {
                                stemp.cmd.update=servi.text();
                            }
                            else if(servi.tagName().contains("cmd.execOnce"))
                            {

                                stemp.cmd.execOnce=servi.text().toInt();
                            }

                            servi = servi.nextSibling().toElement();
                        }
                        service.at(service.size()-1)->addService(stemp);

                        servis = servis.nextSibling().toElement();
                    }

                    nodes = nodes.nextSibling().toElement();
                }
            }
            Child = Child.nextSibling().toElement();

        }

        Component = Component.nextSibling().toElement();
    }
}

int serialize::read()
{
    QFile* file = new QFile("config.xml");
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout<< "file does not exist"<<endl;
        return 0;
    }
    else
    {
        doc =QDomDocument ("configuration");
        if (!doc.setContent(file))
        {
            cout<< "This file is not valid"<<endl;
            return 0;
        }
        else
        {
            return 1;
        }


    }

}
