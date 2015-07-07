
#include <QApplication>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "data.h"
#include "server.h"
int main(int argc, char *argv[])
{
    /*QApplication a(argc, argv);

    a.exec();*/

    //data generation
    srand (time(NULL));
    data* d = new data(10,2);
    server* serv = new server();
    int nbTest = 1;


    serv->startConnection();

    int idSearch = rand() % d->N;
    if(false)
    {


        QString type("FS");
        QString path("/tmp/");

        // Write data
        cout<<endl<<"Write : "<<type.toStdString()<<endl;
        for (int i = 0; i < nbTest; i++)
        {
            QString file("data-");
            file.append(QString::number(i));
            cout
                    << (serv->write(type,path,file,d->csv(';', 0,8))).toStdString()
                    << "------------------"
                    << endl;
        }
        //Read
        cout<<endl<<"Read : "<<type.toStdString()<<endl;
        for (int i = 0; i < nbTest; i++)
        {
            QString file("data-");
            file.append(QString::number(i));
            cout
                    << (serv->read(type,path,file)).toStdString()
                    << "------------------"
                    << endl;
        }
        //Grep
        cout<<endl<<"Grep : "<<type.toStdString()<<endl;

        QString search;
        search.append("\";");
        search.append(QString::number(idSearch));
        search.append(";");
        search.append(QString::number(0));
        search.append(";\"");
        for (int i = 0; i < nbTest; i++)
        {

            QString file("data-");
            file.append(QString::number(i));

            cout
                    << (serv->grep(type,path,file,search)).toStdString()
                    << "------------------"
                    << endl;
        }
    }
if(false)
    {


        QString type("Hadoop");
        QString path("/tmp/");

        // Write data
        cout<<endl<<"Write : "<<type.toStdString()<<endl;
        for (int i = 0; i < nbTest; i++)
        {
            QString file("data-");
            file.append(QString::number(i));
            cout
                    << (serv->write(type,path,file,d->csv(';',0,8))).toStdString()
                    << "------------------"
                    << endl;
        }
        //Read
        cout<<endl<<"Read : "<<type.toStdString()<<endl;
        for (int i = 0; i < nbTest; i++)
        {
            QString file("data-");
            file.append(QString::number(i));
            cout
                    << (serv->read(type,path,file)).toStdString()
                    << "------------------"
                    << endl;
        }
        //Grep
        cout<<endl<<"Grep : "<<type.toStdString()<<endl;

        QString search;
        search.append("\";");
        search.append(QString::number(idSearch));
        search.append(";\"");
        for (int i = 0; i < nbTest; i++)
        {

            QString file("data-");
            file.append(QString::number(i));

            cout
                    << (serv->grep(type,path,file,search)).toStdString()
                    << "------------------"
                    << endl;
        }
    }
if(false)
    {


        QString type("Hbase");
        QString path("tmp");
        // Write data
        cout<<endl<<"Write : "<<type.toStdString()<<endl;
        for (int i = 0; i < nbTest; i++)
        {
            QString file("data-");
            file.append(QString::number(i));
            cout
                    << (serv->write(type,path,file,d->csv(0,8))).toStdString()
                    << "------------------"
                    << endl;
        }


        //Read
        cout<<endl<<"Read : "<<type.toStdString()<<endl;
        for (int i = 0; i < nbTest; i++)
        {
            QString file("data-");
            file.append(QString::number(i));
            cout
                    << (serv->read(type,path,file)).toStdString()
                    << "------------------"
                    << endl;
        }
        //Grep
        cout<<endl<<"Grep : "<<type.toStdString()<<endl;

        std::stringstream search;
        search<<std::setfill('0');
        search<<std::setw(10);
        search << 0;
        search<<std::setfill('0');
        search<<std::setw(10);
        search << idSearch;


        for (int i = 0; i < nbTest; i++)
        {

            QString file("data-");
            file.append(QString::number(i));
            QString  req;
            req.append(search.str().c_str());
            cout
                    << (serv->grep(type,path,req,"")).toStdString()
                    << "------------------"
                    << endl;
        }
    }

    if(true)
    {


        QString type("Hive");
        QString path("tmp");
        // Write data
        cout<<endl<<"Write : "<<type.toStdString()<<endl;
        {
            serv->write("","","","DROP TABLE temp;");
serv->write("","","","DROP TABLE velassco;");
            serv->write("","","","CREATE TABLE velassco(key INT, ts INT, x FLOAT, y FLOAT, z FLOAT, ax FLOAT, ay FLOAT, az FLOAT);");
            serv->write("","","","CREATE TABLE temp(col_value STRING);");
            cout
                    << "------------------"
                    << endl;
        }


        /*//Read
        cout<<endl<<"Read : "<<type.toStdString()<<endl;
        for (int i = 0; i < nbTest; i++)
        {
            QString file("data-");
            file.append(QString::number(i));
            cout
                    << (serv->read(type,path,file)).toStdString()
                    << "------------------"
                    << endl;
        }
        //Grep
        cout<<endl<<"Grep : "<<type.toStdString()<<endl;

        std::stringstream search;
        search<<std::setfill('0');
        search<<std::setw(10);
        search << 0;
        search<<std::setfill('0');
        search<<std::setw(10);
        search << idSearch;


        for (int i = 0; i < nbTest; i++)
        {

            QString file("data-");
            file.append(QString::number(i));
            QString  req;
            req.append(search.str().c_str());
            cout
                    << (serv->grep(type,path,req,"")).toStdString()
                    << "------------------"
                    << endl;
        }*/
    }
    return 0;
}
