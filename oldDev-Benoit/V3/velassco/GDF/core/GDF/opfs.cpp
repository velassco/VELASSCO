#include "opfs.h"

opFS::opFS()
{
    serviceName = "FS";

}

void opFS::demo()
{
    {
        statusNodes s;
        s.name = "shok";
        s.type = MASTER;
        nodes.push_back(s);
        vector<_service> Vtemp;
        services.push_back(Vtemp );
    }
}
int opFS::writeFile(QString path,QString filename, QString content)
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
    QString exe;
    QStringList args;

    {
        exe = "ssh";
        args.clear();
        args << nodes.at(0).name;
        args << "rm";

        QString loc;
        loc.append(path);
        loc.append("/");
        loc.append(filename);
        args <<  loc;

        ps = execCommand(exe, args);
    }


    {
        exe = "scp";
        args.clear();
        args << f;

        QString loc;
        loc.append(nodes.at(0).name);
        loc.append(":");
        loc.append(path);
        args <<  loc;
        ps = execCommand(exe, args);
    }
    return 1;
}

QString opFS::readFile(QString path,QString filename)
{
    QString ps;
    QString exe = "ssh";
    QStringList args;

    {

        args.clear();
        args << nodes.at(0).name;
        args << "cat";
        QString loc;
        loc.append(path);
        loc.append("/");
        loc.append(filename);
        args <<  loc;
        ps = execCommand(exe, args);

    }
    return ps;

}

QString  opFS::grepFile(QString path,QString filename, QString content)
{
    QString ps;
    QString exe = "ssh";
    QStringList args;

    {

        args.clear();
        args << nodes.at(0).name;
        args << "cat";
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
QString opFS::lsFile(QString path)
{
    QString ps;
    QString exe = "ssh";
    QStringList args;

    {

        args.clear();
        args << nodes.at(0).name;
        args << "ls";
        args << "-la";
        QString loc;
        loc.append(path);
        loc.append("/");
        args <<  loc;
        ps = execCommand(exe, args);

    }
    return ps;
}

void opFS::query(message& _return, const message& querry)
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
