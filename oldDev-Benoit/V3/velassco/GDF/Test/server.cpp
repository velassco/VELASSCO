#include "server.h"

int getLocalpid(string service)
{
    QProcess pingProcess;
    QString exe = "ps ax";
    pingProcess.start(exe);
    pingProcess.waitForFinished();
    QString output(pingProcess.readAll());

    QString str( service.c_str());
    QStringList qsl = output.split(QRegExp("\n|\r\n|\r"));
    int i = 0;
    bool test = false;
    int pid = -1;
    while (i < qsl.size() && !test)
    {
        if(qsl.at(i).contains(str))
        {
            QStringList qslTemp = qsl.at(i).split(QRegExp(" "));
            pid = qslTemp.at(0).toInt();
            test = true;
        }
        i++;
    }

    if (pid == 0)
    {
        return -1;
    }
    else
    {
        return pid;
    }
}

server::server()
{
    hadoop = false;
    ip ="127.0.0.1";
    login = "root";
    pass = "root";


    corepid = getLocalpid("coreGDF");
}

server::~server()
{
    try
    {

        transport->close();
    } catch (TException& tx) {
        cout << "ERROR: " << tx.what() << endl;
    }

}

void server::startConnection()
{
    socket= boost::shared_ptr<TTransport>(new TSocket("localhost", 9090));
    transport = boost::shared_ptr<TTransport>(new TBufferedTransport(socket));
    protocol = boost::shared_ptr<TProtocol>(new TBinaryProtocol(transport));

    transport->open();
}
int server::getCorePid()
{
    return corepid;
}

QString server::getStatusOfServices()
{

    ServerServiceClient client = protocol;

    try
    {
        message m;

        client.getStatusOfAllServicesOnNode(m, "shok");

        QString tmp;

        tmp.append(m.message1.c_str());
        tmp.append("\n\n");
        tmp.append(m.message2.c_str());
        tmp.append("\n\n");
        tmp.append(m.message3.c_str());
        tmp.append("\n\n");
        tmp.append(m.message4.c_str());
        tmp.append("\n\n");
        tmp.append(m.message5.c_str());
        tmp.append("\n\n");
        tmp.append(m.message6.c_str());
        tmp.append("\n\n");

        return tmp;
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
}


void server::startAllServices()
{
    try
    {

        ServerServiceClient client = protocol;
        client.startAllServicesOnAllNodes();
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
}

void server::stopAllServices()
{

    ServerServiceClient client = protocol;

    try
    {

        client.stopAllServicesOnAllNodes();

    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
}

QString server::ping()
{
    QString tmp;
    tmp.append("Ping\n");
    try
    {

        ServerServiceClient client = protocol;
        message m;
        double finalTime = getTime();
        client.ping(m);
        finalTime = getTime() - finalTime ;

        tmp.append(m.message1.c_str());
        tmp.append("\n\n");
        tmp.append("Time : ");
        tmp.append(QString::number(finalTime));
        tmp.append("\n Size :");
        tmp.append(QString::number(m.message1.size()));
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
    return tmp;
}

QString server::write(QString type, QString path, QString filename, QString content)
{
    QString tmp;

    try
    {

        ServerServiceClient client = protocol;
        message m;


        if(type.contains("Hive"))
        {
            m.message1="writeHive";
        }
        else if(type.contains("Hbase"))
        {
            m.message1="writeHbase";
        }
        else if(type.contains("Hadoop"))
        {
            m.message1="writeHadoop";
        }
        else
        {
            m.message1="writeFS";
        }

        m.__set_message2(path.toStdString());
        m.__set_message3(filename.toStdString());
        m.__set_message4(content.toStdString());
        message res;
        double finalTime = getTime();
        client.query(res, m);
        finalTime = getTime() - finalTime ;

     //   tmp.append(res.message1.c_str());
      //  tmp.append("\n\n");
        tmp.append("Time : ");
        tmp.append(QString::number(finalTime));
        tmp.append("\n Size :");
        tmp.append(QString::number(res.message1.size()));
        tmp.append(" o\n");
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
    return tmp;
}

QString server::read(QString type, QString path, QString filename)
{
    QString tmp;

    try
    {

        ServerServiceClient client = protocol;
        message m;


        if(type.contains("Hive"))
        {
            m.message1="readHive";
        }
        else if(type.contains("Hbase"))
        {
            m.message1="readHbase";
        }
        else if(type.contains("Hadoop"))
        {
            m.message1="readHadoop";
        }
        else
        {
            m.message1="readFS";
        }

        m.__set_message2(path.toStdString());
        m.__set_message3(filename.toStdString());
        message res;
        double finalTime = getTime();
        client.query(res, m);
        finalTime = getTime() - finalTime ;
        //tmp.append(res.message1.c_str());
        //tmp.append("\n\n");
        tmp.append("Time : ");
        tmp.append(QString::number(finalTime));
        tmp.append("\n Size :");
        tmp.append(QString::number(res.message1.size()));
        tmp.append(" o\n");
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
    return tmp;
}

QString server::grep(QString type, QString path, QString filename, QString content)
{

    QString tmp;

    try
    {

        ServerServiceClient client = protocol;
        message m;
        if(type.contains("Hive"))
        {
            m.message1="GrepHive";
        }
        else if(type.contains("Hbase"))
        {
            m.message1="GrepHbase";
        }
        else if(type.contains("Hadoop"))
        {
            m.message1="GrepHadoop";
        }
        else
        {
            m.message1="GrepFS";
        }


        m.__set_message2(path.toStdString());
        m.__set_message3(filename.toStdString());

        m.__set_message4(content.toStdString());
        message res;
        double finalTime = getTime();
        client.query(res, m);
        finalTime = getTime() - finalTime ;
        //tmp.append(res.message1.c_str());
        //tmp.append("\n\n");
        tmp.append("Time : ");
        tmp.append(QString::number(finalTime));
        tmp.append("\n Size :");
        tmp.append(QString::number(res.message1.size()));
        tmp.append(" o\n");
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
    return tmp;
}
QString server::ls(QString type, QString path)
{
    QString tmp;

    try
    {

        ServerServiceClient client = protocol;
        message m;

        if(type.contains("Hive"))
        {
            m.message1="lsHive";
        }
        else if(type.contains("Hbase"))
        {
            m.message1="lsHbase";
        }
        else if(type.contains("Hadoop"))
        {
            m.message1="lsHadoop";
        }
        else
        {
            m.message1="lsFS";
        }
        m.__set_message2(path.toStdString());
        message res;
        double finalTime = getTime();
        client.query(res, m);
        finalTime = getTime() - finalTime ;

        //tmp.append(res.message1.c_str());
       // tmp.append("\n\n");
        tmp.append("Time : ");
        tmp.append(QString::number(finalTime));
        tmp.append("\n Size :");
        tmp.append(QString::number(res.message1.size()));
        tmp.append(" o\n");
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
    return tmp;
}

void server::change()
{
    hadoop = !hadoop;
}
