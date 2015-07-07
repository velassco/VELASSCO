#include "ophive.h"

ophive::ophive()
{
    serviceName = "Hive";
    path ="/scratch/tools/hive/";
    confDir ="/scratch/tools/hive/hive/conf/";
    dll ="www.lange.xyz/velassco/hive.zip";
    psCMD ="jps";
    columnName = new vector<string>();


}

void ophive::demo()
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
            stemp.cmd.execOnce = -1;
            stemp.service = "RunJar";
            stemp.cmd.start = "hive --service hiveserver &";
            services.at(services.size()-1).push_back(stemp);
        }


    }

}
void ophive::download ()
{
    printcli("#### ophive::download ####\n");
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
            hadoopFile.append("/hive.zip");

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
                args<<tmp.append("hive.zip");
                ps = execCommand(exe, args);

            }
        }
    }
    printcli("#### _ophive::download_ ####\n");
}
void ophive::install()
{
    printcli("#### ophive::install ####\n");
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
                stmp.append("/hive/bin");
                if(!ps.contains(stmp))
                {
                    args.clear();
                    args << nodes.at(i).name;
                    args <<"echo";
                    stmp.clear();
                    stmp.append("PATH=$PATH:");
                    stmp.append(path);
                    stmp.append("/hive/bin:");
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
                stmp.append("/hive/bin:");
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
    printcli("#### _ophive::install_ ####\n");
}
void ophive::conf()
{
    printcli("#### ophive::conf ####\n");
    operate::conf();
    printcli("#### _ophive::conf_ ####\n");
}


int ophive::writeFile(QString path,QString filename, QString content)
{
    QString ps;
    char err_buf[MAX_HIVE_ERR_MSG_LEN];
    HiveResultSet* resultset;
    HiveReturn retval;
    HiveConnection* connection =  DBOpenConnection("default", nodes.at(0).name.toStdString().c_str(), atoi(DEFAULT_PORT),atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
    assert(connection != NULL);
    char req[1024];
    sprintf(req, "%s", content.toStdString().c_str());


    retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
    RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                   "DBExecute failed: %s\n",
                                   err_buf, connection, 0);
    char field[MAX_FIELD_LEN];
    size_t data_byte_size;
    int is_null_value;
    retval = DBFetch(resultset, err_buf, sizeof(err_buf));
    size_t col_count;


    if(retval == HIVE_SUCCESS)
    {
        return 1;
    }
    else if(retval == HIVE_NO_MORE_DATA)
    {
       return 0;
    }


}
QString ophive::readFile(QString path,QString filename)
{
    QString ps;
    char err_buf[MAX_HIVE_ERR_MSG_LEN];
    HiveResultSet* resultset;
    HiveReturn retval;
    HiveConnection* connection =  DBOpenConnection("default", nodes.at(0).name.toStdString().c_str(), atoi(DEFAULT_PORT),atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
    assert(connection != NULL);
    char req[1024];
    sprintf(req, "select * from %s;", path.toStdString().c_str());


    retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
    RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                   "DBExecute failed: %s\n",
                                   err_buf, connection, 0);
    char field[MAX_FIELD_LEN];
    size_t data_byte_size;
    int is_null_value;
    retval = DBFetch(resultset, err_buf, sizeof(err_buf));
    size_t col_count;


    while(retval == HIVE_SUCCESS)
    {
        retval = DBGetColumnCount(resultset, &col_count, err_buf, sizeof(err_buf));
        for( int i = 0; i <  col_count; i++)
        {
            retval = DBGetFieldAsCString(resultset, i, field, sizeof(field), &data_byte_size, &is_null_value,
                                         err_buf, sizeof(err_buf));
            ps.append(field);
            ps.append("\n");
        }
        retval = DBFetch(resultset, err_buf, sizeof(err_buf));
    }
     if(retval == HIVE_NO_MORE_DATA)
    {
        ps.append("");
    }

    return ps;
}
QString ophive::grepFile(QString path,QString filename,  QString content)
{
    QString ps;
    char err_buf[MAX_HIVE_ERR_MSG_LEN];
    HiveResultSet* resultset;
    HiveReturn retval;
    HiveConnection* connection =  DBOpenConnection("default", nodes.at(0).name.toStdString().c_str(), atoi(DEFAULT_PORT),atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
    assert(connection != NULL);
    char req[1024];
    sprintf(req, "select * from %s where key =%d;", path.toStdString().c_str(),filename.toInt());


    retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
    RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                   "DBExecute failed: %s\n",
                                   err_buf, connection, 0);
    char field[MAX_FIELD_LEN];
    size_t data_byte_size;
    int is_null_value;
    retval = DBFetch(resultset, err_buf, sizeof(err_buf));
    size_t col_count;


    while(retval == HIVE_SUCCESS)
    {
        retval = DBGetColumnCount(resultset, &col_count, err_buf, sizeof(err_buf));
        for( int i = 0; i <  col_count; i++)
        {
            retval = DBGetFieldAsCString(resultset, i, field, sizeof(field), &data_byte_size, &is_null_value,
                                         err_buf, sizeof(err_buf));
            ps.append(field);
            ps.append("\n");
        }
        retval = DBFetch(resultset, err_buf, sizeof(err_buf));
    }
    if(retval == HIVE_NO_MORE_DATA)
    {
        ps.append("");
    }

    return ps;
}
QString ophive::lsFile(QString path)
{

    QString ps;
    char err_buf[MAX_HIVE_ERR_MSG_LEN];
    HiveResultSet* resultset;
    HiveReturn retval;
    HiveConnection* connection =  DBOpenConnection("default", nodes.at(0).name.toStdString().c_str(), atoi(DEFAULT_PORT),atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
    assert(connection != NULL);
    char req[1024];
    sprintf(req, "show tables;");


    retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
    RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                   "DBExecute failed: %s\n",
                                   err_buf, connection, 0);
    char field[MAX_FIELD_LEN];
    size_t data_byte_size;
    int is_null_value;
    retval = DBFetch(resultset, err_buf, sizeof(err_buf));
    size_t col_count;


    while(retval == HIVE_SUCCESS)
    {
        retval = DBGetColumnCount(resultset, &col_count, err_buf, sizeof(err_buf));
        for( int i = 0; i <  col_count; i++)
        {
            retval = DBGetFieldAsCString(resultset, i, field, sizeof(field), &data_byte_size, &is_null_value,
                                         err_buf, sizeof(err_buf));
            ps.append(field);
            ps.append("\n");
        }
        retval = DBFetch(resultset, err_buf, sizeof(err_buf));
    }
    if(retval == HIVE_NO_MORE_DATA)
    {
        ps.append("");
    }

    return ps;

}

void ophive::query(message& _return, const message& querry)
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
