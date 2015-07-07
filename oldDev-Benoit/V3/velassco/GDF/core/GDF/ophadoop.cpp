#include "ophadoop.h"

opHadoop::opHadoop()
{
    serviceName = "Hadoop";
    path ="/scratch/tools/hadoop/";
    confDir ="/scratch/tools/hadoop/hadoop/etc/hadoop/";
    dll ="www.lange.xyz/velassco/hadoop.zip";
    varConfDir = "HADOOP_CONF_DIR";
    psCMD ="jps";

}

void opHadoop::demo()
{
    {
        statusNodes s;
        s.name = "shok";
        s.type = MASTER;
        nodes.push_back(s);
        vector<_service> Vtemp;
        services.push_back(Vtemp );
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "foramtNodemanager";
            stemp.cmd.execOnce = 0;
            stemp.cmd.start = "hdfs namenode -format";
            services.at(services.size()-1).push_back(stemp);
        }

        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "NodeManager";
            stemp.cmd.execOnce = -1;
            stemp.cmd.start = "yarn-daemon.sh start nodemanager";
            stemp.cmd.stop = "yarn-daemon.sh stop nodemanager";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "ResourceManager";
            stemp.cmd.execOnce = -1;
            stemp.cmd.start = "yarn-daemon.sh start resourcemanager";
            stemp.cmd.stop = "yarn-daemon.sh stop resourcemanager";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "NameNode";
            stemp.cmd.execOnce = -1;
            stemp.cmd.start = "hadoop-daemon.sh start namenode";
            stemp.cmd.stop = "hadoop-daemon.sh stop namenode";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "DataNode";
            stemp.cmd.execOnce = -1;
            stemp.cmd.start = "hadoop-daemon.sh start datanode";
            stemp.cmd.stop = "hadoop-daemon.sh stop datanode";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "mkdir-home";
            stemp.cmd.execOnce = 0;
            stemp.cmd.start = "hadoop fs -mkdir -p /home/lange";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "mkdir-home-hive";
            stemp.cmd.execOnce = 0;
            stemp.cmd.start = "hadoop fs -mkdir -p /home/hive";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "mkdir-home-hive2";
            stemp.cmd.execOnce = 0;
            stemp.cmd.start = "hadoop fs -mkdir -p /hive";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "mkdir-home-hbase";
            stemp.cmd.execOnce = 0;
            stemp.cmd.start = "hadoop fs -mkdir -p /home/hbase";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "mkdir-hbase2";
            stemp.cmd.execOnce = 0;
            stemp.cmd.start = "hadoop fs -mkdir -p /hbase";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "mkdir-home2";
            stemp.cmd.execOnce = 0;
            stemp.cmd.start = "hadoop fs -mkdir -p /user/lange";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "mkdir-home3";
            stemp.cmd.execOnce = 0;
            stemp.cmd.start = "hadoop fs -mkdir -p /user/hive";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.pid =-1;
            stemp.service = "mkdir-home4";
            stemp.cmd.execOnce = 0;
            stemp.cmd.start = "hadoop fs -mkdir -p /user/hbase";
            services.at(services.size()-1).push_back(stemp);
        }
    }
    {
        confFiles.push_back(new xml());
        confFiles.at(confFiles.size() - 1)->name = "hdfs-site.xml";

        QDomElement root = confFiles.at(confFiles.size() - 1)->config.createElement("configuration");
        confFiles.at(confFiles.size() - 1)->config.appendChild(root);
        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");
            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode("dfs.namenode.name.dir");
            QDomElement value = confFiles.at(confFiles.size() - 1)->config.createElement("value");
            QDomText textValue = confFiles.at(confFiles.size() - 1)->config.createTextNode("/tmp/hadoop/namenode_data/");
            name.appendChild(textName);
            value.appendChild(textValue);
            property.appendChild(name);
            property.appendChild(value);
            root.appendChild(property);
        }
        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");
            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode("dfs.datanode.data.dir");
            QDomElement value = confFiles.at(confFiles.size() - 1)->config.createElement("value");
            QDomText textValue = confFiles.at(confFiles.size() - 1)->config.createTextNode("/tmp/hadoop/datanode_data/");
            name.appendChild(textName);
            value.appendChild(textValue);
            property.appendChild(name);
            property.appendChild(value);
            root.appendChild(property);
        }
    }
    {
        confFiles.push_back(new xml());
        confFiles.at(confFiles.size() - 1)->name = "core-site.xml";

        QDomElement root = confFiles.at(confFiles.size() - 1)->config.createElement("configuration");
        confFiles.at(confFiles.size() - 1)->config.appendChild(root);

        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");
            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode("hadoop.tmp.dir");
            QDomElement value = confFiles.at(confFiles.size() - 1)->config.createElement("value");
            QDomText textValue = confFiles.at(confFiles.size() - 1)->config.createTextNode("/tmp/hadoop/localfs/");
            name.appendChild(textName);
            value.appendChild(textValue);
            property.appendChild(name);
            property.appendChild(value);
            root.appendChild(property);
        }
        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");
            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode("fs.defaultFS");
            QDomElement value = confFiles.at(confFiles.size() - 1)->config.createElement("value");
            QDomText textValue = confFiles.at(confFiles.size() - 1)->config.createTextNode("hdfs://shok:55555");
            name.appendChild(textName);
            value.appendChild(textValue);
            property.appendChild(name);
            property.appendChild(value);
            root.appendChild(property);
        }
    }


    {
        confFiles.push_back(new xml());
        confFiles.at(confFiles.size() - 1)->name = "yarn-site.xml";

        QDomElement root = confFiles.at(confFiles.size() - 1)->config.createElement("configuration");
        confFiles.at(confFiles.size() - 1)->config.appendChild(root);

        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");
            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode("yarn.resourcemanager.hostname");
            QDomElement value = confFiles.at(confFiles.size() - 1)->config.createElement("value");
            QDomText textValue = confFiles.at(confFiles.size() - 1)->config.createTextNode("shok");
            name.appendChild(textName);
            value.appendChild(textValue);
            property.appendChild(name);
            property.appendChild(value);
            root.appendChild(property);
        }
    }
}
void opHadoop::download ()
{
    printcli("#### opHadoop::download ####\n");
    operate::download();

    QString ps;
    QString exe = "ssh";
    QStringList args;
    for (uint i = 0; i < nodes.size(); i++)
    {

        {
            args<<nodes.at(i).name;
            args<<"ls";
            QString hadoopFile;
            hadoopFile.append(path);
            hadoopFile.append("/hadoop.zip");

            args<<hadoopFile;
            args<<"|";
            args<<"wc";
            args<<"-l";
            ps = execCommand(exe, args);

            if(ps.at(0) == '0')
            {
                args.clear();
                args<<nodes.at(i).name;
                args<<"wget";
                args<<"-P";
                args<<path;
                args<<dll;
                ps = execCommand(exe, args);

                args.clear();
                args<<nodes.at(i).name;
                args<<"unzip";
                args<<"-d";
                args<<path;
                QString tmp = path;
                tmp.append("/");
                args<<tmp.append("hadoop.zip");
                ps = execCommand(exe, args);

            }
        }
    }

    printcli("#### _opHadoop::download_ ####\n");
}

void opHadoop::install()
{
    printcli("#### opHadoop::install ####\n");
    operate::install();
    for (uint i = 0; i < nodes.size(); i++)
    {
        QString ps;
        QString exe = "ssh";

        QStringList args;
        {
            args << nodes.at(i).name;
            args << "env";
            ps = execCommand(exe, args);
            if(ps.contains(varConfDir))
            {
                ps.remove(0,ps.indexOf(varConfDir)+varConfDir.size()+1);
                ps.remove(ps.indexOf("\n"), ps.size());
                QString stmp = path;
                stmp.append("/hadoop/bin");
                if(!ps.contains(stmp))
                {
                    args.clear();
                    args << nodes.at(i).name;
                    args <<"echo";
                    stmp.clear();
                    stmp.append("PATH=$PATH:");
                    stmp.append(path);
                    stmp.append("/hadoop/bin:");
                    stmp.append(path);
                    stmp.append("/hadoop/sbin:");
                    stmp.append(path);
                    stmp.append("/hadoop/etc/hadoop");
                    args <<stmp;
                    args <<">>";
                    args <<"~/.bashrc";
                    ps = execCommand(exe, args);

                    args.clear();
                    stmp.clear();
                    stmp.append(varConfDir);
                    stmp.append("=");
                    stmp.append(confDir);
                    args <<stmp;
                    args <<">>";
                    args <<"~/.bashrc";
                    ps = execCommand(exe, args);
                }
            }
            else
            {
                QString stmp;
                args.clear();
                args << nodes.at(i).name;
                args <<"echo";
                stmp.clear();
                stmp.append("PATH=$PATH:");
                stmp.append(path);
                stmp.append("/hadoop/bin:");
                stmp.append(path);
                stmp.append("/hadoop/sbin:");
                stmp.append(path);
                stmp.append("/hadoop/etc/hadoop");
                args <<stmp;
                args <<">>";
                args <<"~/.bashrc";
                ps = execCommand(exe, args);

                args.clear();
                stmp.clear();
                args << nodes.at(i).name;
                args <<"echo";

                stmp.append(varConfDir);
                stmp.append("=");
                stmp.append(confDir);
                args <<stmp;
                args <<">>";
                args <<"~/.bashrc";
                ps = execCommand(exe, args);
            }
        }
    }
    printcli("#### _opHadoop::install_ ####\n");
}

void opHadoop::conf()
{
    printcli("#### opHadoop::conf ####\n");
    operate::conf();
    QFile file(  "temp/slaves");
    file.open(QIODevice::WriteOnly);

    for (uint i = 0; i < nodes.size(); i ++)
    {
        file.write(nodes.at(i).name.toStdString().c_str());
    }
    file.close();
    QString ps;
    QString exe = "scp";

    QStringList args;


    for (uint i = 0; i < nodes.size(); i++)
    {
        for (uint j = 0; j < confFiles.size(); j ++)
        {
            args.clear();
            QString f;
            f.append("temp/slaves");

            args << f;
            QString loc;
            loc.append(nodes.at(i).name);
            loc.append(":");
            loc.append(confDir);
            args <<  loc;
            ps = execCommand(exe, args);
        }
    }
    printcli("#### _opHadoop::conf_ ####\n");
}

int opHadoop::writeFile(QString path,QString filename, QString content)
{
    QString f;
    f.append("temp/");
    f.append(filename);
    QFile file(f);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << content;
    file.close();

    QString ps;
    QString exe = "scp";

    QStringList args;
    {
        args.clear();
        args << f;

        QString loc;
        loc.append(nodes.at(0).name);
        loc.append(":");
        loc.append("/tmp");
        args <<  loc;
        ps = execCommand(exe, args);
    }


    {
        exe = "ssh";
        args.clear();
        args << nodes.at(0).name;
        args << "hadoop";
        args << "fs";
        args << "-mkdir";
        args << "-p";
        QString loc;

        loc.append(path);
        args <<  loc;


        ps = execCommand(exe, args);
    }

    {
        exe = "ssh";
        args.clear();
        args << nodes.at(0).name;
        args << "hadoop";
        args << "fs";
        args << "-rm";
        args << "-r";
        QString loc;

        loc.append("/tmp/");
        loc.append(filename);
        args <<  loc;

        loc.clear();
        loc.append(path);
        loc.append("/");
        loc.append(filename);
        args <<  loc;
        ps = execCommand(exe, args);
    }
    {
        exe = "ssh";
        args.clear();
        args << nodes.at(0).name;
        args << "hadoop";
        args << "fs";
        args << "-put";
        QString loc;

        loc.append("/tmp/");
        loc.append(filename);
        args <<  loc;

        loc.clear();
        loc.append(path);
        loc.append("/");
        loc.append(filename);
        args <<  loc;
        ps = execCommand(exe, args);
    }
    return 1;
}

QString opHadoop::readFile(QString path,QString filename)
{
    QString ps;
    QString exe = "ssh";
    QStringList args;

    {

        args.clear();
        args << nodes.at(0).name;
        args << "hadoop";
        args << "fs";
        args << "-cat";
        QString loc;
        loc.append(path);
        loc.append("/");
        loc.append(filename);
        args <<  loc;
        ps = execCommand(exe, args);

    }
    return ps;

}

QString  opHadoop::grepFile(QString path,QString filename, QString content)
{
    QString ps;
    QString exe = "ssh";
    QStringList args;

    {

        args.clear();
        args << nodes.at(0).name;
        args << "hadoop";
        args << "fs";
        args << "-cat";
        QString loc;
        loc.append(path);
        loc.append("/");
        loc.append(filename);
        args <<  loc;

        args <<  "|";
        args <<  "grep";
        args <<  content;
        ps = execCommand(exe, args);

    }
    return ps;
}
QString opHadoop::lsFile(QString path)
{
    QString ps;
    QString exe = "ssh";
    QStringList args;

    {

        args.clear();
        args << nodes.at(0).name;
        args << "hadoop";
        args << "fs";
        args << "-ls";
        QString loc;
        loc.append(path);
        loc.append("/");
        args <<  loc;
        ps = execCommand(exe, args);

    }
    return ps;
}

void opHadoop::query(message& _return, const message& querry)
{
    QString messageTemp;
    messageTemp.append(querry.message1.c_str());
    if(messageTemp.contains("write"))
    {

        {
            int t = this->writeFile(
                        querry.message2.c_str() ,
                        querry.message3.c_str() ,
                        querry.message4.c_str()
                        )
                    ;
            _return.message1 = QString::number(t).toStdString();

        }
    }
    else if(messageTemp.contains("read"))
    {

        {
            _return.message1 = (
                        this->readFile(
                            querry.message2.c_str(),
                            querry.message3.c_str()
                            )
                        ).toStdString();

        }
    }
    else if(messageTemp.contains("Grep"))
    {

        {
            _return.message1 = (
                       this->grepFile(
                            querry.message2.c_str(),
                            querry.message3.c_str(),
                            querry.message4.c_str()
                            )
                        ).toStdString();

        }
    }
    else if(messageTemp.contains("ls"))
    {

        {
            _return.message1 = (
                        this->lsFile(
                            querry.message2.c_str()
                            )
                        ).toStdString();

        }
    }
}
