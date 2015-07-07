
#include "ophbase.h"


ophbase::ophbase()
{
    serviceName = "Hbase";
    path ="/scratch/tools/hbase/";
    confDir ="/scratch/tools/hbase/hbase/conf/";
    dll ="www.lange.xyz/velassco/hbase.zip";
    psCMD ="jps";
    columnName = new vector<string>();
    columnName->push_back("cf:ID");
    columnName->push_back("cf:TS");
    columnName->push_back("cf:X");
    columnName->push_back("cf:Y");
    columnName->push_back("cf:Z");
    columnName->push_back("cf:AX");
    columnName->push_back("cf:AY");
    columnName->push_back("cf:AZ");

}


void ophbase::demo()
{
    {
        statusNodes s;
        s.name = "shok";
        s.type = MASTER;
        nodes.push_back(s);
        vector<_service> Vtemp;
        services.push_back(Vtemp );
        /* {
            _service stemp;
            stemp.pid =-1;
            stemp.cmd.execOnce = 0;
            stemp.service = "startHBASE";
            stemp.cmd.start = "start-hbase.sh";
            stemp.cmd.stop = "stop-hbase.sh";

            services.at(services.size()-1).push_back(stemp);
        }*/

        {
            _service stemp;
            stemp.pid =-1;
            stemp.cmd.execOnce = -1;
            stemp.service = "HQuorumPeer";
            stemp.cmd.start = "hbase-daemon.sh start zookeeper";
            stemp.cmd.stop = "hbase-daemon.sh stop zookeeper";
            services.at(services.size()-1).push_back(stemp);
        }

        {
            _service stemp;
            stemp.pid =-1;
            stemp.cmd.execOnce = -1;
            stemp.service = "HRegionServer";
            stemp.cmd.start = "hbase-daemon.sh start regionserver";
            stemp.cmd.stop = "hbase-daemon.sh stop regionserver";
            services.at(services.size()-1).push_back(stemp);
        }

        {
            _service stemp;
            stemp.pid =-1;
            stemp.cmd.execOnce = -1;
            stemp.service = "HMaster";
            stemp.cmd.start = "hbase-daemon.sh start master";
            stemp.cmd.stop = "hbase-daemon.sh stop master";
            services.at(services.size()-1).push_back(stemp);
        }
        {
            _service stemp;
            stemp.service = "HRegionServer";
            stemp.pid =-1;
            stemp.cmd.execOnce = -1;
            stemp.service = "ThriftServer";
            stemp.cmd.start = "hbase-daemon.sh start thrift";
            stemp.cmd.stop = "hbase-daemon.sh stop thrift";

            services.at(services.size()-1).push_back(stemp);
        }

    }
    {
        confFiles.push_back(new xml());
        confFiles.at(confFiles.size() - 1)->name = "hbase-site.xml";

        QDomElement root = confFiles.at(confFiles.size() - 1)->config.createElement("configuration");
        confFiles.at(confFiles.size() - 1)->config.appendChild(root);
        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");
            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode("hbase.rootdir");
            QDomElement value = confFiles.at(confFiles.size() - 1)->config.createElement("value");
            QDomText textValue = confFiles.at(confFiles.size() - 1)->config.createTextNode("hdfs://shok:55555/hbase");
            name.appendChild(textName);
            value.appendChild(textValue);
            property.appendChild(name);
            property.appendChild(value);
            root.appendChild(property);
        }

        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");
            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode("hbase.cluster.distributed");
            QDomElement value = confFiles.at(confFiles.size() - 1)->config.createElement("value");
            QDomText textValue = confFiles.at(confFiles.size() - 1)->config.createTextNode("true");
            name.appendChild(textName);
            value.appendChild(textValue);
            property.appendChild(name);
            property.appendChild(value);
            root.appendChild(property);
        }

        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");
            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode("hbase.zookeeper.quorum");
            QDomElement value = confFiles.at(confFiles.size() - 1)->config.createElement("value");
            QDomText textValue = confFiles.at(confFiles.size() - 1)->config.createTextNode("shok");
            name.appendChild(textName);
            value.appendChild(textValue);
            property.appendChild(name);
            property.appendChild(value);
            root.appendChild(property);
        }

        {
            QDomElement property = confFiles.at(confFiles.size() - 1)->config.createElement("property");
            QDomElement name = confFiles.at(confFiles.size() - 1)->config.createElement("name");
            QDomText textName = confFiles.at(confFiles.size() - 1)->config.createTextNode("hbase.master.info.bindAddress");
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

void ophbase::download ()
{
    printcli("#### ophbase::download ####\n");
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
            hadoopFile.append("/hbase.zip");

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
                args<<tmp.append("hbase.zip");
                ps = execCommand(exe, args);

            }
        }
    }
    printcli("#### _ophbase::download_ ####\n");
}

void ophbase::install()
{
    printcli("#### ophbase::install ####\n");
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
                stmp.append("/hbase/bin");
                if(!ps.contains(stmp))
                {
                    args.clear();
                    args << nodes.at(i).name;
                    args <<"echo";
                    stmp.clear();
                    stmp.append("PATH=$PATH:");
                    stmp.append(path);
                    stmp.append("/hbase/bin:");
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
                stmp.append("/hbase/bin:");
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
    printcli("#### _ophbase::install_ ####\n");
}

void ophbase::conf()
{
    printcli("#### ophbase::conf ####\n");
    operate::conf();
    QFile file(  "temp/regionservers");
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
            f.append("temp/regionservers");

            args << f;
            QString loc;
            loc.append(nodes.at(i).name);
            loc.append(":");
            loc.append(confDir);
            args <<  loc;
            ps = execCommand(exe, args);
        }
    }
    printcli("#### _ophbase::conf_ ####\n");
}


int ophbase::writeFile(QString path,QString filename, QString content)
{

    boost::shared_ptr<TTransport> socket(new TSocket(nodes.at(0).name.toStdString().c_str(), 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);

    try
    {
        transport->open();
        vector<ColumnDescriptor> columDesc;

        for (int i = 0; i < columnName->size(); i++)
        {

            ColumnDescriptor tmp;
            tmp.name = columnName->at(i);
            columDesc.push_back(tmp);
        }
        client.createTable(path.toStdString(),columDesc);

        transport->close();
    }
    catch (AlreadyExists &tx)
    {
    }
    catch(IllegalArgument &tx)
    {
        cout << "Error ILlegal Args: " << tx.what()<<endl;
    }

    /* V1 = 1 mutation
     *
     *
     try
    {

        transport->open();
        vector<string> thrift_list_table;
        thrift_list_table.push_back(path.toStdString());
        vector<Mutation> mutations;
        char buf[32];
        std::string row(buf);
        std::map<std::string,std::string> m;
        char tmp1[32];
        QStringList list;
        list = content.split(";");
        for (int i = 0; i < columnName->size() &&  i < list.size(); i++)
        {


            mutations.push_back(Mutation());
            sprintf(tmp1, "%s",columnName->at(i).c_str());
            mutations.back().column = string(tmp1);
            sprintf(tmp1, "%s", list.at(i).toStdString().c_str());
            mutations.back().value = string(tmp1);
        }
        client.mutateRow(path.toStdString(),filename.toStdString(),mutations, m);
        client.mutateRows(path.toStdString());
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;

    }*/
    try
    {

        transport->open();
        vector<BatchMutation> mutations;

        char buf[32];
        std::string row(buf);
        std::map<std::string,std::string> m;
        char tmp1[32];
        QStringList line = content.split("\n");
        vector<Mutation> mutation;
        QStringList list;
        for (int j = 0; j < line.size()-1 ; j++)
        {
            mutations.push_back(BatchMutation());
            mutation.clear();
            list.clear();
            list = line.at(j).split(";");
            std::stringstream stream;
            stream<<std::setfill('0');
            stream<<std::setw(10);
            stream << list.at(1).toStdString();
            stream<<std::setfill('0');
            stream<<std::setw(10);
            stream << list.at(0).toStdString();
            mutations.back().__set_row(stream.str());
            for (int i = 0; i < columnName->size() &&  i < list.size(); i++)
            {

                mutation.push_back(Mutation());
                sprintf(tmp1, "%s",
                        columnName->at(i).c_str()/*,
                        list.at(1).toStdString().c_str()*/
                        );
                mutation.back().column = string(tmp1);
                sprintf(tmp1, "%s", list.at(i).toStdString().c_str());
                mutation.back().value = string(tmp1);
            }
            Mutation m;

            mutations.back().__set_mutations(mutation);
        }
        client.mutateRows(path.toStdString(),mutations, m);


        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;

    }
    return 1;
}

QString ophbase::readFile(QString path,QString filename)
{
    QString ps;
    boost::shared_ptr<TTransport> socket(new TSocket(nodes.at(0).name.toStdString().c_str(), 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {

        transport->open();
        std::vector<std::string> columnNames ;
        char tmp1[32];
        for (int i = 0; i < columnName->size(); i++)
        {
            sprintf(tmp1, "%s",
                    columnName->at(i).c_str()
                    );
            columnNames.push_back(tmp1);
        }
        std::map<std::string,std::string> m;
        int scannerID = client.scannerOpen(path.toStdString(), "", columnNames,m);
        vector<TRowResult> rowsResult;
        bool header = false;
        do
        {

            client.scannerGet(rowsResult, scannerID);
            if(rowsResult.size() > 0 && !header)
            {
                for (CellMap::const_iterator it = rowsResult.at(0).columns.begin();
                     it != rowsResult.at(0).columns.end(); ++it)
                {
                    ps.append( it->first.c_str());
                    ps.append(";");

                }
                ps.append("\n");
                header = true;
            }
            for (int i = 0; i < rowsResult.size(); i++)
            {

                for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                     it != rowsResult.at(i).columns.end(); ++it)
                {

                    ps.append( it->second.value.c_str());
                    ps.append(";");
                }
                ps.append("\n");
            }
        }

        while(rowsResult.size() > 0);


        client.scannerClose(scannerID);
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;

    }
    return ps;
}

QString ophbase::grepFile(QString path,QString filename,  QString )
{
    QString ps;
    boost::shared_ptr<TTransport> socket(new TSocket(nodes.at(0).name.toStdString().c_str(), 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {

        transport->open();
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        bool header = false;

        {
            client.getRow(rowsResult,path.toStdString() ,filename.toStdString(), m);

            if(rowsResult.size() > 0 && !header)
            {
                for (CellMap::const_iterator it = rowsResult.at(0).columns.begin();
                     it != rowsResult.at(0).columns.end(); ++it)
                {
                    ps.append( it->first.c_str());
                    ps.append(";");

                }
                ps.append("\n");
                header = true;
            }
            for (int i = 0; i < rowsResult.size(); i++)
            {

                for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                     it != rowsResult.at(i).columns.end(); ++it)
                {

                    ps.append( it->second.value.c_str());
                    ps.append(";");
                }
                ps.append("\n");

            }
        }

        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;

    }

    return ps;
}

QString ophbase::lsFile(QString path)
{
    QString ps;
    boost::shared_ptr<TTransport> socket(new TSocket(nodes.at(0).name.toStdString().c_str(), 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {

        transport->open();
        std::vector<Text> _return;
        client.getTableNames(_return);
        transport->close();
        for(int i = 0; i< _return.size(); i++)
        {
            ps.append(_return.at(i).c_str());
            ps.append("\n");
        }
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;

    }
    return ps;
}

void ophbase::query(message& _return, const message& querry)
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
