//
//  main.cpp
//  parsehbase
//
//  Created by Benoit Lange on 29/05/2015.
//  Copyright (c) 2015 Benoit Lange. All rights reserved.
//



#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

using namespace std;


int output = 1;
string thetra = "";

/////////////////////////////////
// Thrift
/////////////////////////////////
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

/////////////////////////////////
// HBase Thrift 1
/////////////////////////////////
#include "thrift/hbase/gen-cpp/Hbase.h"
using namespace  ::apache::hadoop::hbase::thrift;
typedef std::map<std::string,TCell> CellMap;

struct result
{
    string name;
    double value[4];
};

struct particule
{
    int p;
    int id;
    double x,y,z;
    vector<result*> r;
    
};



//Explode function
vector<string> explode(string line, char delimiteur)
{
    std::vector<std::string> result;
    std::istringstream iss(line);
    
    for (std::string token; std::getline(iss, token, delimiteur); )
    {
        result.push_back(std::move(token));
    }
    
    return result;
}

double my_gettimeofday()
{
    struct timeval tmp_time;
    gettimeofday(&tmp_time, NULL);
    return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}


// point inside first tetrahedron of the VELaSSCo_HbaseBasicTest example
// is 0.034967938,0.689002752,0.025866667
// it is the centre of the first tetrahedron
// assuming numbering starts with 1, then:
// first tetrahedron ( id = 1) has nodes = 1794 1660 1802 1768
// in c/c++, numberations starts with 0, so nodes = 1793 1659 1801 1767
// coordinates of these nodes are:
//    1660 0.07139438390731812 0.7269042730331421 0.0
//    1768 0.0 0.683978796005249 0.10346666723489761
//    1794 0.06847736984491348 0.6451279520988464 0.0
//    1802 0.0 0.699999988079071 0.0
bool IsPointInsideTetrahedron( double p_in_x, double p_in_y, double p_in_z,
                              double a_x, double a_y, double a_z,
                              double b_x, double b_y, double b_z,
                              double c_x, double c_y, double c_z,
                              double d_x, double d_y, double d_z,
                              double epsilon) {
    double r, s, t;
    r = s = t = 0;
    
    double da_x = a_x - d_x;
    double da_y = a_y - d_y;
    double da_z = a_z - d_z;
    double db_x = b_x - d_x;
    double db_y = b_y - d_y;
    double db_z = b_z - d_z;
    double dc_x = c_x - d_x;
    double dc_y = c_y - d_y;
    double dc_z = c_z - d_z;
    double dp_x = p_in_x - d_x;
    double dp_y = p_in_y - d_y;
    double dp_z = p_in_z - d_z;
    double r_denominator = da_x * ( db_y * dc_z - dc_y * db_z) +
    db_x * ( dc_y * da_z - da_y * dc_z) +
    dc_x * ( da_y * db_z - db_y * da_z);
    double r_numerator = dp_x * ( db_y * dc_z - dc_y * db_z) +
    db_x * ( dc_y * dp_z - dp_y * dc_z) +
    dc_x * ( dp_y * db_z - db_y * dp_z);
    
    if ( fabs( r_numerator) < epsilon)
        r_numerator = 0.0f;
    if ( fabs( r_denominator) > epsilon)
        r = r_numerator / r_denominator;
    else
        r = 0.0f;
    if ( ( r < 0.0f) || ( r > 1.0f))
        return false;
    
    double s_denominator = db_y * dc_z - dc_y * db_z;
    double s_numerator = dp_y * dc_z - dc_y * dp_z + r * ( dc_y * da_z - da_y * dc_z);
    if ( fabs( s_numerator) < epsilon)
        s_numerator = 0.0f;
    if ( fabs( s_denominator) > epsilon)
        s = s_numerator / s_denominator;
    else {
        s_denominator = db_y * dc_x - dc_y * db_x;
        s_numerator = dp_y * dc_x - dc_y * dp_x + r * ( dc_y * da_x - da_y * dc_x);
        if ( fabs( s_denominator) > epsilon)
            s = s_numerator / s_denominator;
        else {
            s_denominator = db_x * dc_z - dc_x * db_z;
            s_numerator = dp_x * dc_z - dc_x * dp_z + r * ( dc_x * da_z - da_x * dc_z);
            if ( fabs( s_denominator) > epsilon)
                s = s_numerator / s_denominator;
            else {
                s = 0.0f;
            }
        }
    }
    if ( ( s < 0.0f) || ( s > 1.0f))
        return false;
    
    double t_denominator = dc_z;
    double t_numerator = dp_z - r * da_z - s * db_z;
    if ( fabs( t_numerator) < epsilon)
        t_numerator = 0.0f;
    if ( fabs( t_denominator) > epsilon)
        t = t_numerator / t_denominator;
    else {
        t_denominator = dc_x;
        t_numerator = dp_x - r * da_x - s * db_x;
        if ( fabs( t_denominator) > epsilon)
            t = t_numerator / t_denominator;
        else {
            t_denominator = dc_y;
            t_numerator = dp_y - r * da_y - s * db_y;
            if ( fabs( t_denominator) > epsilon)
                t = t_numerator / t_denominator;
            else {
                t = 0.0f;
            }
        }
    }
    if ( ( t < 0.0f) || ( t > 1.0f))
        return false;
    
    if ( ( ( r + s + t) >= -0.0) && ( ( r + s + t) <= 1.0f)) {
        return true;
    } else {
        return false;
    }
}

double getTestBenoitVR(string table, int TS, int rndSearch)
{
    particule *p = new particule();
    bool find = false;
    double d = my_gettimeofday();
    boost::shared_ptr<TTransport> socket(new TSocket("pez001", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        transport->open();
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        std::stringstream key;
        
        int i = 0;
        while (i < 10 && !find)
        {
            key.str("");
            key <<"P" << i;
            //timeStep
            key << "TS" << std::setfill('0');
            key << std::setw(6);
            key << TS;
            
            key << "ID" << std::setfill('0');
            key << std::setw(6);
            key << rndSearch;
            client.getRow(rowsResult, table ,key.str(), m);
            if(rowsResult.size() > 0)
            {
                find = true;
                //Parsing of the row:
                for (CellMap::const_iterator it = rowsResult.at(0).columns.begin();
                     it != rowsResult.at(0).columns.end(); ++it)
                {
                    if(it->first.find("cf1:X") < it->first.size())
                    {
                        p->x = stod(it->second.value);
                    }
                    else if(it->first.find("cf1:Y") < it->first.size())
                    {
                        p->y = stod(it->second.value);
                    }
                    else if(it->first.find("cf1:Z")  < it->first.size())
                    {
                        p->z = stod(it->second.value);
                    }
                    else if(it->first.find("cf2:PartitionId")  < it->first.size())
                    {
                        p->p = stod(it->second.value);
                    }
                    else if(it->first.find("cf2:Height")  < it->first.size())
                    {
                        if(it->first.find("0")  < it->first.size())
                        {
                            p->r.push_back(new result());
                            p->r.back()->name="Height";
                        }
                        
                        int pos = it->first.find("cf2:Height") + 10;
                        
                        pos = it->first[pos] - '0';
                        p->r.back()->value[pos] = stod(it->second.value);
                    }
                    
                    else if(it->first.find("cf2:function")  < it->first.size())
                    {
                        if(it->first.find("0")  < it->first.size())
                        {
                            p->r.push_back(new result());
                            p->r.back()->name="function";
                        }
                        
                        int pos = it->first.find("cf2:function") + 12;
                        pos = it->first[pos];
                        pos = pos- '0';
                        p->r.back()->value[pos] = stod(it->second.value);
                    }
                    
                }
                
            }
            else
            {
                i++;
            }
            
        }
        
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    d = my_gettimeofday() - d;
    
    if(output >= 2)
    {
        if (find)
        {
            cout << " = t " << d << " s - ";
            cout << "p" << p->p << ", id" << p->id << " " << p->x << " " << p->y << " " << p->z;
            for (int i = 0 ; i < p->r.size(); i++)
            {
                cout << " - " << p->r.at(i)->name << " : ";
                for (int j = 0; j < 4; j++)
                {
                    cout<< " - "<< p->r.at(i)->value[j] << ", ";
                    
                }
            }
            cout << endl;
        }
        else
        {
            cout << "- NF -" <<endl;
        }
    }
    delete p;
    return d;
}

double getTestBenoitVR2(string table, int TS, int rndSearch)
{
    particule *p = new particule();
    bool find = false;
    double d = my_gettimeofday();
    boost::shared_ptr<TTransport> socket(new TSocket("pez001", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        transport->open();
        
        std::stringstream key;
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        
        int i = 0;
        while (i < 10 && !find)
        {
            key.str("");
            key << "ID" << std::setfill('0') << std::setw(16);
            key << rndSearch;
            key <<"P" << i;
            client.getRowTs(rowsResult, table ,key.str(), (TS+1), m);
            if(rowsResult.size() > 0)
            {
                
                find = true;
                p->p = i;
                for (CellMap::const_iterator it = rowsResult.at(0).columns.begin();
                     it != rowsResult.at(0).columns.end(); ++it)
                {
                    if(it->first.find("cf1:X") < it->first.size())
                    {
                        p->x = stod(it->second.value);
                    }
                    else if(it->first.find("cf1:Y") < it->first.size())
                    {
                        p->y = stod(it->second.value);
                    }
                    else if(it->first.find("cf1:Z")  < it->first.size())
                    {
                        p->z = stod(it->second.value);
                    }
                    else if(it->first.find("cf2:Height")  < it->first.size())
                    {
                        if(it->first.find("0")  < it->first.size())
                        {
                            p->r.push_back(new result());
                            p->r.back()->name="Height";
                        }
                        
                        int pos = it->first.find("cf2:Height") + 10;
                        
                        pos = it->first[pos] - '0';
                        p->r.back()->value[pos] = stod(it->second.value);
                    }
                    
                    else if(it->first.find("cf2:function")  < it->first.size())
                    {
                        if(it->first.find("0")  < it->first.size())
                        {
                            p->r.push_back(new result());
                            p->r.back()->name="function";
                        }
                        
                        int pos = it->first.find("cf2:function") + 12;
                        pos = it->first[pos];
                        pos = pos- '0';
                        p->r.back()->value[pos] = stod(it->second.value);
                    }
                }
            }
            else
            {
                i++;
            }
        }
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    d = my_gettimeofday() - d;
    
    if(output >= 2)
    {
        if (find)
        {
            cout << " = t " << d << " s - ";
            cout << "p" << p->p << ", id" << p->id << " " << p->x << " " << p->y << " " << p->z;
            for (int i = 0 ; i < p->r.size(); i++)
            {
                cout << " - " << p->r.at(i)->name << " : ";
                for (int j = 0; j < 4; j++)
                {
                    cout<< " - "<< p->r.at(i)->value[j] << ", ";
                }
            }
            cout << endl;
        }
        else
        {
            cout << "- NF -" <<endl;
        }
    }
    delete p;
    return d;
}

double getTestBenoitVR3(string table, int TS, int rndSearch)
{
    particule *p = new particule();
    bool find = false;
    double d = my_gettimeofday();
    boost::shared_ptr<TTransport> socket(new TSocket("pez001", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        transport->open();
        
        std::stringstream key;
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        
        
        key.str("");
        key << "ID" << std::setfill('0') << std::setw(16);
        key << rndSearch;
        client.getRowTs(rowsResult, table ,key.str(), (TS+1), m);
        if(rowsResult.size() > 0)
        {
            
            find = true;
            for (CellMap::const_iterator it = rowsResult.at(0).columns.begin();
                 it != rowsResult.at(0).columns.end(); ++it)
            {
                if(it->first.find("cf1:X") < it->first.size())
                {
                    p->x = stod(it->second.value);
                }
                else if(it->first.find("cf1:Y") < it->first.size())
                {
                    p->y = stod(it->second.value);
                }
                else if(it->first.find("cf1:Z")  < it->first.size())
                {
                    p->z = stod(it->second.value);
                }
                else if(it->first.find("cf2:PartitionId")  < it->first.size())
                {
                    p->p = stod(it->second.value);
                }
                else if(it->first.find("cf2:Height")  < it->first.size())
                {
                    if(it->first.find("0")  < it->first.size())
                    {
                        p->r.push_back(new result());
                        p->r.back()->name="Height";
                    }
                    
                    int pos = it->first.find("cf2:Height") + 10;
                    
                    pos = it->first[pos] - '0';
                    p->r.back()->value[pos] = stod(it->second.value);
                }
                
                else if(it->first.find("cf2:function")  < it->first.size())
                {
                    if(it->first.find("0")  < it->first.size())
                    {
                        p->r.push_back(new result());
                        p->r.back()->name="function";
                    }
                    
                    int pos = it->first.find("cf2:function") + 12;
                    pos = it->first[pos];
                    pos = pos- '0';
                    p->r.back()->value[pos] = stod(it->second.value);
                }
            }
        }
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    d = my_gettimeofday() - d;
    if(output >= 2)
    {
        
        if (find)
        {
            cout << " = t " << d << " s - ";
            cout << "p" << p->p << ", id" << p->id << " " << p->x << " " << p->y << " " << p->z;
            for (int i = 0 ; i < p->r.size(); i++)
            {
                cout << " - " << p->r.at(i)->name << " : ";
                for (int j = 0; j < 4; j++)
                {
                    cout<< " - "<< p->r.at(i)->value[j] << ", ";
                }
            }
            cout << endl;
        }
        else
        {
            cout << "- NF -" <<endl;
        }
    }
    delete p;
    return d;
}

/*
 This part is the minimal template to get particle (based on: table name, Time Step and ID)
 */
double getMinimalTest(string table, int TS, int rndSearch)
{
    particule *p = new particule();
    bool find = false;
    double d = my_gettimeofday();
    /*
     The code to gather inoframtion needs to be write here
     */
    d = my_gettimeofday() - d;
    if(output >= 1)
    {
        if (find)
        {
            cout << " = t " << d << " s - ";
            cout << "p" << p->p << ", id" << p->id << " " << p->x << " " << p->y << " " << p->z;
            for (int i = 0 ; i < p->r.size(); i++)
            {
                cout << " - " << p->r.at(i)->name << " : ";
                for (int j = 0; j < 4; j++)
                {
                    cout<< " - "<< p->r.at(i)->value[j] << ", ";
                }
            }
            cout << endl;
        }
        else
        {
            cout << "- NF -" <<endl;
        }
    }
    delete p;
    return d;
}

double getThetraBenoitVR1Sol1(string tableVertices, string tableConnect, int TS, double coord[3] , double epsylon)
{
    
    thetra = "";
    bool find = false;
    double radius = 0.01f;
    double d = my_gettimeofday();
    boost::shared_ptr<TTransport> socket(new TSocket("pez001", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        
        TScan ts;
        std::stringstream filter;
        transport->open();
        do
        {
            filter.str("");
            filter << "SingleColumnValueFilter('cf1', 'X', >= , 'binary:" << (coord[0] - radius) << "') AND  SingleColumnValueFilter('cf1', 'X', <= , 'binary:" << (coord[0] + radius) << "')";
            filter << " AND SingleColumnValueFilter('cf1', 'Y', >= , 'binary:" << (coord[1] - radius) << "') AND  SingleColumnValueFilter('cf1', 'Y', <= , 'binary:" << (coord[1] + radius) << "')";
            filter << " AND SingleColumnValueFilter('cf1', 'Z', >= , 'binary:" << (coord[2] - radius) << "') AND  SingleColumnValueFilter('cf1', 'Z', <= , 'binary:" << (coord[2] + radius) << "')";
            ts.__set_filterString(filter.str());
            ScannerID id = client.scannerOpenWithScan( tableVertices ,ts, m);
            
            do
            {
                client.scannerGetList(rowsResult, id, 10);
                for (int i = 0; i < rowsResult.size()  && !find; i++)
                {
                    for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                         it != rowsResult.at(i).columns.end() && !find; ++it)
                    {
                        int numParticules[4];
                        if(it->first.find("cf2:Tetra") < it->first.size())
                        {
                            vector<string> elem =  explode(it->second.value, ' ');
                            
                            stringstream key;
                            vector<TRowResult> rowsResult2;
                            std::vector<Text> keys;
                            for(int k = 1; k < 5; k++)
                            {
                                int l = 0;
                                while (l < 10)
                                {
                                    
                                    key.str("");
                                    key <<"P" << l;
                                    //timeStep
                                    key << "TS" << std::setfill('0');
                                    key << std::setw(6);
                                    key << TS;
                                    
                                    key << "ID" << std::setfill('0');
                                    key << std::setw(6);
                                    key << stoi(elem.at(k));
                                    keys.push_back(key.str());
                                    l++;
                                }
                            }
                            client.getRows(rowsResult2, tableVertices ,keys, m);
                            if(rowsResult2.size() == 4)
                            {
                                double coords[4][3];
                                for(int l = 0; l < 4; l++)
                                {
                                    for (CellMap::const_iterator it = rowsResult2.at(l).columns.begin();
                                         it != rowsResult2.at(l).columns.end(); ++it)
                                    {
                                        
                                        if(it->first.find("cf1:X") < it->first.size())
                                        {
                                            coords[l][0] = stod(it->second.value);
                                        }
                                        else if(it->first.find("cf1:Y") < it->first.size())
                                        {
                                            coords[l][1] = stod(it->second.value);
                                        }
                                        else if(it->first.find("cf1:Z")  < it->first.size())
                                        {
                                            coords[l][2] = stod(it->second.value);
                                        }
                                        
                                    }
                                }
                                find = IsPointInsideTetrahedron(coord[0], coord[1], coord[2],
                                                                coords[0][0], coords[0][1], coords[0][2],
                                                                coords[1][0], coords[1][1], coords[1][2],
                                                                coords[2][0], coords[2][1], coords[2][2],
                                                                coords[3][0], coords[3][1], coords[3][2],
                                                                epsylon);
                                if (find )
                                {
                                    thetra = elem.at(0);
                                    
                                    if(output >= 2)
                                    {
                                        cout  << coord[0] << " "<< coord[1] << " "<< coord[2] <<endl;
                                        cout <<"\t" << coords[0][0] << " "<< coords[0][1] << " "<< coords[0][2]<< endl;
                                        cout <<"\t" << coords[1][0] << " "<< coords[1][1] << " "<< coords[1][2]<< endl;
                                        cout <<"\t" << coords[2][0] << " "<< coords[2][1] << " "<< coords[2][2]<< endl;
                                        cout <<"\t" << coords[3][0] << " "<< coords[3][1] << " "<< coords[3][2]<< endl;
                                        cout << "\t\t" << thetra <<endl<< endl;
                                    }
                                }
                            }
                        }
                    }
                    
                }
                
            }
            while (rowsResult.size() > 0 && !find);
            radius += 0.01f;
        }
        while (radius < 1.0 && !find);
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    
    d = my_gettimeofday() - d;
    return d;
}

double getThetraBenoitVR2Sol1(string tableVertices, string tableConnect, int TS, double coord[3] , double epsylon)
{
    
    thetra = "";
    bool find = false;
    double radius = 0.01f;
    double d = my_gettimeofday();
    boost::shared_ptr<TTransport> socket(new TSocket("pez001", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        
        TScan ts;
        std::stringstream filter;
        transport->open();
        do
        {
            filter.str("");
            filter << "SingleColumnValueFilter('cf1', 'X', >= , 'binary:" << (coord[0] - radius) << "') AND  SingleColumnValueFilter('cf1', 'X', <= , 'binary:" << (coord[0] + radius) << "')";
            filter << " AND SingleColumnValueFilter('cf1', 'Y', >= , 'binary:" << (coord[1] - radius) << "') AND  SingleColumnValueFilter('cf1', 'Y', <= , 'binary:" << (coord[1] + radius) << "')";
            filter << " AND SingleColumnValueFilter('cf1', 'Z', >= , 'binary:" << (coord[2] - radius) << "') AND  SingleColumnValueFilter('cf1', 'Z', <= , 'binary:" << (coord[2] + radius) << "')";
            ts.__set_filterString(filter.str());
            ScannerID id = client.scannerOpenWithScan( tableVertices ,ts, m);
            
            do
            {
                client.scannerGetList(rowsResult, id, 10);
                for (int i = 0; i < rowsResult.size()  && !find; i++)
                {
                    for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                         it != rowsResult.at(i).columns.end() && !find; ++it)
                    {
                        int numParticules[4];
                        if(it->first.find("cf2:Tetra") < it->first.size())
                        {
                            vector<string> elem =  explode(it->second.value, ' ');
                            
                            stringstream key;
                            vector<TRowResult> rowsResult2;
                            std::vector<Text> keys;
                            for(int k = 1; k < 5; k++)
                            {
                                int l = 0;
                                while (l < 10)
                                {
                                    key.str("");
                                    
                                    key << "ID" << std::setfill('0') << std::setw(16);
                                    key << elem.at(k);
                                    key <<"P" << l;
                                    
                                    keys.push_back(key.str());
                                    l++;
                                }
                            }
                            client.getRows(rowsResult2, tableVertices ,keys, m);
                            if(rowsResult2.size() == 4)
                            {
                                double coords[4][3];
                                for(int l = 0; l < 4; l++)
                                {
                                    for (CellMap::const_iterator it = rowsResult2.at(l).columns.begin();
                                         it != rowsResult2.at(l).columns.end(); ++it)
                                    {
                                        
                                        if(it->first.find("cf1:X") < it->first.size())
                                        {
                                            coords[l][0] = stod(it->second.value);
                                        }
                                        else if(it->first.find("cf1:Y") < it->first.size())
                                        {
                                            coords[l][1] = stod(it->second.value);
                                        }
                                        else if(it->first.find("cf1:Z")  < it->first.size())
                                        {
                                            coords[l][2] = stod(it->second.value);
                                        }
                                        
                                    }
                                }
                                find = IsPointInsideTetrahedron(coord[0], coord[1], coord[2],
                                                                coords[0][0], coords[0][1], coords[0][2],
                                                                coords[1][0], coords[1][1], coords[1][2],
                                                                coords[2][0], coords[2][1], coords[2][2],
                                                                coords[3][0], coords[3][1], coords[3][2],
                                                                epsylon);
                                if (find )
                                {
                                    thetra = elem.at(0);
                                    
                                    if(output >= 2)
                                    {
                                        cout  << coord[0] << " "<< coord[1] << " "<< coord[2] <<endl;
                                        cout <<"\t" << coords[0][0] << " "<< coords[0][1] << " "<< coords[0][2]<< endl;
                                        cout <<"\t" << coords[1][0] << " "<< coords[1][1] << " "<< coords[1][2]<< endl;
                                        cout <<"\t" << coords[2][0] << " "<< coords[2][1] << " "<< coords[2][2]<< endl;
                                        cout <<"\t" << coords[3][0] << " "<< coords[3][1] << " "<< coords[3][2]<< endl;
                                        cout << "\t\t" << thetra <<endl<< endl;
                                    }
                                }
                            }
                        }
                    }
                    
                }
                
            }
            while (rowsResult.size() > 0 && !find);
            radius += 0.01f;
            
        }
        while (radius < 1.0 && !find);
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    
    d = my_gettimeofday() - d;
    return d;
}

double getThetraBenoitVR3Sol1(string tableVertices, string tableConnect, int TS, double coord[3] , double epsylon)
{
    
    thetra = "";
    bool find = false;
    double radius = 0.01f;
    double d = my_gettimeofday();
    boost::shared_ptr<TTransport> socket(new TSocket("pez001", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        
        TScan ts;
        std::stringstream filter;
        transport->open();
        do
        {
            filter.str("");
            filter << "SingleColumnValueFilter('cf1', 'X', >= , 'binary:" << (coord[0] - radius) << "') AND  SingleColumnValueFilter('cf1', 'X', <= , 'binary:" << (coord[0] + radius) << "')";
            filter << " AND SingleColumnValueFilter('cf1', 'Y', >= , 'binary:" << (coord[1] - radius) << "') AND  SingleColumnValueFilter('cf1', 'Y', <= , 'binary:" << (coord[1] + radius) << "')";
            filter << " AND SingleColumnValueFilter('cf1', 'Z', >= , 'binary:" << (coord[2] - radius) << "') AND  SingleColumnValueFilter('cf1', 'Z', <= , 'binary:" << (coord[2] + radius) << "')";
            ts.__set_filterString(filter.str());
            ScannerID id = client.scannerOpenWithScan( tableVertices ,ts, m);
            
            do
            {
                client.scannerGetList(rowsResult, id, 10);
                for (int i = 0; i < rowsResult.size()  && !find; i++)
                {
                    for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                         it != rowsResult.at(i).columns.end() && !find; ++it)
                    {
                        int numParticules[4];
                        if(it->first.find("cf2:Tetra") < it->first.size())
                        {
                            vector<string> elem =  explode(it->second.value, ' ');
                            
                            stringstream key;
                            vector<TRowResult> rowsResult2;
                            std::vector<Text> keys;
                            for(int k = 1; k < 5; k++)
                            {
                                key.str("");
                                key << "ID" << std::setfill('0') << std::setw(16);
                                key << stoi(elem.at(k));
                                
                                keys.push_back(key.str());
                            }
                            client.getRowsTs(rowsResult2, tableVertices ,keys ,TS ,m);
                            if(rowsResult2.size() == 4)
                            {
                                double coords[4][3];
                                for(int l = 0; l < 4; l++)
                                {
                                    for (CellMap::const_iterator it = rowsResult2.at(l).columns.begin();
                                         it != rowsResult2.at(l).columns.end(); ++it)
                                    {
                                        
                                        if(it->first.find("cf1:X") < it->first.size())
                                        {
                                            coords[l][0] = stod(it->second.value);
                                        }
                                        else if(it->first.find("cf1:Y") < it->first.size())
                                        {
                                            coords[l][1] = stod(it->second.value);
                                        }
                                        else if(it->first.find("cf1:Z")  < it->first.size())
                                        {
                                            coords[l][2] = stod(it->second.value);
                                        }
                                        
                                    }
                                }
                                find = IsPointInsideTetrahedron(coord[0], coord[1], coord[2],
                                                                coords[0][0], coords[0][1], coords[0][2],
                                                                coords[1][0], coords[1][1], coords[1][2],
                                                                coords[2][0], coords[2][1], coords[2][2],
                                                                coords[3][0], coords[3][1], coords[3][2],
                                                                epsylon);
                                if (find )
                                {
                                    thetra = elem.at(0);
                                    
                                    if(output >= 2)
                                    {
                                        cout  << coord[0] << " "<< coord[1] << " "<< coord[2] <<endl;
                                        cout <<"\t" << coords[0][0] << " "<< coords[0][1] << " "<< coords[0][2]<< endl;
                                        cout <<"\t" << coords[1][0] << " "<< coords[1][1] << " "<< coords[1][2]<< endl;
                                        cout <<"\t" << coords[2][0] << " "<< coords[2][1] << " "<< coords[2][2]<< endl;
                                        cout <<"\t" << coords[3][0] << " "<< coords[3][1] << " "<< coords[3][2]<< endl;
                                        cout << "\t\t" << thetra <<endl<< endl;
                                    }
                                }
                            }
                        }
                    }
                    
                }
                
            }
            while (rowsResult.size() > 0 && !find);
            radius += 0.01f;
            
        }
        while (radius < 1.0 && !find);
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    
    d = my_gettimeofday() - d;
    return d;
}

double getThetraBenoitVR1Sol2(string tableVertices, string tableConnect, int TS, double coord[3] , double epsylon)
{
    thetra = "";
    bool find = false;
    double d = my_gettimeofday();
    boost::shared_ptr<TTransport> socket(new TSocket("pez001", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        
        TScan ts;
        std::stringstream filter;
        
        filter.str("");
        filter << "SingleColumnValueFilter('cf1', 'Xmin', <= , 'binary:" << coord[0] << "') AND  SingleColumnValueFilter('cf1', 'Xmax', >= , 'binary:" << coord[0] << "')";
        filter << " AND SingleColumnValueFilter('cf1', 'Ymin', <= , 'binary:" << coord[1] << "') AND  SingleColumnValueFilter('cf1', 'Ymax', >= , 'binary:" << coord[1] << "')";
        filter << " AND SingleColumnValueFilter('cf1', 'Zmin', <= , 'binary:" << coord[2] << "') AND  SingleColumnValueFilter('cf1', 'Zmax', >= , 'binary:" << coord[2] << "')";
        ts.__set_filterString(filter.str());
        transport->open();
        ScannerID id = client.scannerOpenWithScan( tableConnect ,ts, m);
        do
        {
            client.scannerGetList(rowsResult, id, 10);
            for (int i = 0; i < rowsResult.size() && !find; i ++)
            {
                int numParticules[4];
                for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                     it != rowsResult.at(i).columns.end(); ++it)
                {
                    if(it->first.find("cf1:idP1") < it->first.size())
                    {
                        numParticules[0] = stoi(it->second.value);
                    }
                    else if(it->first.find("cf1:idP2") < it->first.size())
                    {
                        numParticules[1] = stoi(it->second.value);
                    }
                    else if(it->first.find("cf1:idP3") < it->first.size())
                    {
                        numParticules[2] = stoi(it->second.value);
                    }
                    else if(it->first.find("cf1:idP4") < it->first.size())
                    {
                        numParticules[3] = stoi(it->second.value);
                    }
                }
                stringstream key;
                vector<TRowResult> rowsResult2;
                std::vector<Text> keys;
                for(int k = 0; k < 4; k++)
                {
                    int l = 0;
                    while (l < 10)
                    {
                        
                        key.str("");
                        key <<"P" << l;
                        //timeStep
                        key << "TS" << std::setfill('0');
                        key << std::setw(6);
                        key << TS;
                        
                        key << "ID" << std::setfill('0');
                        key << std::setw(6);
                        key << numParticules[k];
                        keys.push_back(key.str());
                        l++;
                    }
                }
                
                client.getRows(rowsResult2, tableVertices ,keys, m);
                if(rowsResult2.size() == 4)
                {
                    double coords[4][3];
                    for(int l = 0; l < 4; l++)
                    {
                        for (CellMap::const_iterator it = rowsResult2.at(l).columns.begin();
                             it != rowsResult2.at(l).columns.end(); ++it)
                        {
                            
                            if(it->first.find("cf1:X") < it->first.size())
                            {
                                coords[l][0] = stod(it->second.value);
                            }
                            else if(it->first.find("cf1:Y") < it->first.size())
                            {
                                coords[l][1] = stod(it->second.value);
                            }
                            else if(it->first.find("cf1:Z")  < it->first.size())
                            {
                                coords[l][2] = stod(it->second.value);
                            }
                            
                        }
                    }
                    find = IsPointInsideTetrahedron(coord[0], coord[1], coord[2],
                                                    coords[0][0], coords[0][1], coords[0][2],
                                                    coords[1][0], coords[1][1], coords[1][2],
                                                    coords[2][0], coords[2][1], coords[2][2],
                                                    coords[3][0], coords[3][1], coords[3][2],
                                                    epsylon);
                    if (find )
                    {
                        thetra = rowsResult.at(i).row;
                        if(output >= 2)
                        {
                            cout  << coord[0] << " "<< coord[1] << " "<< coord[2] <<endl;
                            cout <<"\t" << coords[0][0] << " "<< coords[0][1] << " "<< coords[0][2]<< endl;
                            cout <<"\t" << coords[1][0] << " "<< coords[1][1] << " "<< coords[1][2]<< endl;
                            cout <<"\t" << coords[2][0] << " "<< coords[2][1] << " "<< coords[2][2]<< endl;
                            cout <<"\t" << coords[3][0] << " "<< coords[3][1] << " "<< coords[3][2]<< endl;
                            cout << "\t\t" << thetra <<endl<< endl;
                        }
                        
                    }
                }
            }
        }
        while (rowsResult.size() > 0 && !find);
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    
    d = my_gettimeofday() - d;
    return d;
}

double getThetraBenoitVR2Sol2(string tableVertices, string tableConnect, int TS, double coord[3] , double epsylon)
{
    thetra = "";
    bool find = false;
    double d = my_gettimeofday();
    boost::shared_ptr<TTransport> socket(new TSocket("pez001", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        
        TScan ts;
        std::stringstream filter;
        
        filter.str("");
        filter << "SingleColumnValueFilter('cf1', 'Xmin', <= , 'binary:" << coord[0] << "') AND  SingleColumnValueFilter('cf1', 'Xmax', >= , 'binary:" << coord[0] << "')";
        filter << " AND SingleColumnValueFilter('cf1', 'Ymin', <= , 'binary:" << coord[1] << "') AND  SingleColumnValueFilter('cf1', 'Ymax', >= , 'binary:" << coord[1] << "')";
        filter << " AND SingleColumnValueFilter('cf1', 'Zmin', <= , 'binary:" << coord[2] << "') AND  SingleColumnValueFilter('cf1', 'Zmax', >= , 'binary:" << coord[2] << "')";
        ts.__set_filterString(filter.str());
        transport->open();
        ScannerID id = client.scannerOpenWithScan( tableConnect ,ts, m);
        do
        {
            client.scannerGetList(rowsResult, id, 10);
            for (int i = 0; i < rowsResult.size() && !find; i ++)
            {
                int numParticules[4];
                for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                     it != rowsResult.at(i).columns.end(); ++it)
                {
                    if(it->first.find("cf1:idP1") < it->first.size())
                    {
                        numParticules[0] = stoi(it->second.value);
                    }
                    else if(it->first.find("cf1:idP2") < it->first.size())
                    {
                        numParticules[1] = stoi(it->second.value);
                    }
                    else if(it->first.find("cf1:idP3") < it->first.size())
                    {
                        numParticules[2] = stoi(it->second.value);
                    }
                    else if(it->first.find("cf1:idP4") < it->first.size())
                    {
                        numParticules[3] = stoi(it->second.value);
                    }
                }
                
                stringstream key;
                vector<TRowResult> rowsResult2;
                std::vector<Text> keys;
                for(int k = 0; k < 4; k++)
                {
                    int l = 0;
                    while (l < 10)
                    {
                        
                        key.str("");
                        
                        key << "ID" << std::setfill('0') << std::setw(16);
                        key << numParticules[k];
                        key <<"P" << l;
                        
                        keys.push_back(key.str());
                        l++;
                    }
                }
                
                client.getRowsTs(rowsResult2, tableVertices ,keys ,TS ,m);
                if(rowsResult2.size() == 4)
                {
                    double coords[4][3];
                    for(int l = 0; l < 4; l++)
                    {
                        for (CellMap::const_iterator it = rowsResult2.at(l).columns.begin();
                             it != rowsResult2.at(l).columns.end(); ++it)
                        {
                            
                            if(it->first.find("cf1:X") < it->first.size())
                            {
                                coords[l][0] = stod(it->second.value);
                            }
                            else if(it->first.find("cf1:Y") < it->first.size())
                            {
                                coords[l][1] = stod(it->second.value);
                            }
                            else if(it->first.find("cf1:Z")  < it->first.size())
                            {
                                coords[l][2] = stod(it->second.value);
                            }
                            
                        }
                    }
                    find = IsPointInsideTetrahedron(coord[0], coord[1], coord[2],
                                                    coords[0][0], coords[0][1], coords[0][2],
                                                    coords[1][0], coords[1][1], coords[1][2],
                                                    coords[2][0], coords[2][1], coords[2][2],
                                                    coords[3][0], coords[3][1], coords[3][2],
                                                    epsylon);
                    if (find )
                    {
                        thetra = rowsResult.at(i).row;
                        if(output >= 2)
                        {
                            cout  << coord[0] << " "<< coord[1] << " "<< coord[2] <<endl;
                            cout <<"\t" << coords[0][0] << " "<< coords[0][1] << " "<< coords[0][2]<< endl;
                            cout <<"\t" << coords[1][0] << " "<< coords[1][1] << " "<< coords[1][2]<< endl;
                            cout <<"\t" << coords[2][0] << " "<< coords[2][1] << " "<< coords[2][2]<< endl;
                            cout <<"\t" << coords[3][0] << " "<< coords[3][1] << " "<< coords[3][2]<< endl;
                            cout << "\t\t" << thetra <<endl<< endl;
                        }
                        
                    }
                }
            }
        }
        while (rowsResult.size() > 0 && !find);
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    
    d = my_gettimeofday() - d;
    return d;
}

double getThetraBenoitVR3Sol2(string tableVertices, string tableConnect, int TS, double coord[3] , double epsylon)
{
    thetra = "";
    bool find = false;
    double d = my_gettimeofday();
    boost::shared_ptr<TTransport> socket(new TSocket("pez001", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        vector<TRowResult> rowsResult;
        std::map<std::string,std::string> m;
        
        TScan ts;
        std::stringstream filter;
        
        filter.str("");
        filter << "SingleColumnValueFilter('cf1', 'Xmin', <= , 'binary:" << coord[0] << "') AND  SingleColumnValueFilter('cf1', 'Xmax', >= , 'binary:" << coord[0] << "')";
        filter << " AND SingleColumnValueFilter('cf1', 'Ymin', <= , 'binary:" << coord[1] << "') AND  SingleColumnValueFilter('cf1', 'Ymax', >= , 'binary:" << coord[1] << "')";
        filter << " AND SingleColumnValueFilter('cf1', 'Zmin', <= , 'binary:" << coord[2] << "') AND  SingleColumnValueFilter('cf1', 'Zmax', >= , 'binary:" << coord[2] << "')";
        ts.__set_filterString(filter.str());
        transport->open();
        ScannerID id = client.scannerOpenWithScan( tableConnect ,ts, m);
        do
        {
            client.scannerGetList(rowsResult, id, 10);
            for (int i = 0; i < rowsResult.size() && !find; i ++)
            {
                int numParticules[4];
                for (CellMap::const_iterator it = rowsResult.at(i).columns.begin();
                     it != rowsResult.at(i).columns.end(); ++it)
                {
                    if(it->first.find("cf1:idP1") < it->first.size())
                    {
                        numParticules[0] = stoi(it->second.value);
                    }
                    else if(it->first.find("cf1:idP2") < it->first.size())
                    {
                        numParticules[1] = stoi(it->second.value);
                    }
                    else if(it->first.find("cf1:idP3") < it->first.size())
                    {
                        numParticules[2] = stoi(it->second.value);
                    }
                    else if(it->first.find("cf1:idP4") < it->first.size())
                    {
                        numParticules[3] = stoi(it->second.value);
                    }
                }
                
                stringstream key;
                vector<TRowResult> rowsResult2;
                std::vector<Text> keys;
                for(int k = 0; k < 4; k++)
                {
                    key.str("");
                    key << "ID" << std::setfill('0') << std::setw(16);
                    key << numParticules[k];
                    
                    keys.push_back(key.str());
                }
                
                client.getRowsTs(rowsResult2, tableVertices ,keys ,TS ,m);
                if(rowsResult2.size() == 4)
                {
                    double coords[4][3];
                    for(int l = 0; l < 4; l++)
                    {
                        for (CellMap::const_iterator it = rowsResult2.at(l).columns.begin();
                             it != rowsResult2.at(l).columns.end(); ++it)
                        {
                            
                            if(it->first.find("cf1:X") < it->first.size())
                            {
                                coords[l][0] = stod(it->second.value);
                            }
                            else if(it->first.find("cf1:Y") < it->first.size())
                            {
                                coords[l][1] = stod(it->second.value);
                            }
                            else if(it->first.find("cf1:Z")  < it->first.size())
                            {
                                coords[l][2] = stod(it->second.value);
                            }
                            
                        }
                    }
                    find = IsPointInsideTetrahedron(coord[0], coord[1], coord[2],
                                                    coords[0][0], coords[0][1], coords[0][2],
                                                    coords[1][0], coords[1][1], coords[1][2],
                                                    coords[2][0], coords[2][1], coords[2][2],
                                                    coords[3][0], coords[3][1], coords[3][2],
                                                    epsylon);
                    if (find )
                    {
                        thetra = rowsResult.at(i).row;
                        if(output >= 2)
                        {
                            cout  << coord[0] << " "<< coord[1] << " "<< coord[2] <<endl;
                            cout <<"\t" << coords[0][0] << " "<< coords[0][1] << " "<< coords[0][2]<< endl;
                            cout <<"\t" << coords[1][0] << " "<< coords[1][1] << " "<< coords[1][2]<< endl;
                            cout <<"\t" << coords[2][0] << " "<< coords[2][1] << " "<< coords[2][2]<< endl;
                            cout <<"\t" << coords[3][0] << " "<< coords[3][1] << " "<< coords[3][2]<< endl;
                            cout << "\t\t" << thetra <<endl<< endl;
                        }
                        
                    }
                }
            }
        }
        while (rowsResult.size() > 0 && !find);
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "Error : " << tx.what()<<endl;
        
    }
    
    d = my_gettimeofday() - d;
    return d;
}


int main(int argc, const char * argv[])
{
    srand(time(NULL));
    double d;
    // The random elem to search
    int rndSearch;
    
    //Number of element to search
    int nbelem = 100;
    //Number of repeted procedure
    int iter = 20;
    
    double avg = 0.0;
    /*
     Get Elements by ID
     */
    if(false)
    {
        
        for(int i = 0 ; i < nbelem ; i ++)
        {
            rndSearch =(rand() % 2060);
            
            avg = 0.0;
            cout<< "BenoitVR (" << rndSearch <<") " << endl;
            for(int j = 0; j < iter ; j ++)
            {
                if(output >= 1)
                {
                    
                    cout << "\t(" << i << " " << j << ") ";
                }
                avg += getTestBenoitVR("BenoitVR", 1, rndSearch);
            }
            cout << "\t\t AVG: " << (avg/iter) << " ("<<iter<<")"<< endl;
            
            avg = 0.0;
            cout<< "BenoitVR2 (" << rndSearch <<") " << endl;
            for(int j = 0; j < iter ; j ++)
            {
                if(output >= 1)
                {
                    
                    cout << "\t(" << i << " " << j << ") ";
                }
                avg += getTestBenoitVR2("BenoitVR2", 1, rndSearch);
            }
            cout << "\t\t AVG: " << (avg/iter) << " ("<<iter<<")"<< endl;
            
            avg = 0.0;
            cout<< "BenoitVR3 (" << rndSearch <<") " << endl;
            for(int j = 0; j < iter ; j ++)
            {
                if(output >= 1)
                {
                    
                    cout << "\t(" << i << " " << j << ") ";
                }
                avg += getTestBenoitVR3("BenoitVR3", 1, rndSearch);
            }
            cout << "\t\t AVG: " << (avg/iter) << " ("<<iter<<")"<< endl;
            cout<< endl<< endl;
            
        }
        
    }
    /*
     Get tetra
     */
    if(true)
    {
        double coord[3];
         for(int i = 0 ; i < nbelem; i ++)
        {
/*              coord[0] = (double)(rand() % 1100) / 1000;
             coord[1] = (double)(rand() % 1100) / 1000;
             coord[2] = (double)(rand() % 1100) / 1000;*/
            coord[0] = (double)(rand() % 1000) / 1000;
            coord[1] = (double)(rand() % 1000) / 1000;
            coord[2] = (double)(rand() % 1000) / 1000;
            cout << "Coord :"  << coord[0] <<", " << coord[1] <<", " << coord[2]  << endl;
            {
                avg = 0.0;
                cout<< "getThetraBenoitVR1Sol1 - BenoitVR, BenoitConnectivity - "<< endl;
                for(int j = 0; j < iter ; j ++)
                {
                    avg += getThetraBenoitVR1Sol1("BenoitVR", "BenoitConnectivity", 1, coord, 0.0000f);
                }
                
                cout << "\t\t ";
                if(thetra.length() > 0)
                {
                    cout << " -=T=- ";
                    if(output >= 1)
                    {
                        cout<< " -=(" << thetra << ")=-- ";
                    }
                }
                cout<< "AVG: " << (avg/iter) << " ("<<iter<<")"<< endl;
                cout<< endl<< endl;
            }
            
            {
                avg = 0.0;
                cout<< "getThetraBenoitVR2Sol1 - BenoitVR2, BenoitConnectivity2 - "<< endl;
                for(int j = 0; j < iter ; j ++)
                {
                    avg += getThetraBenoitVR2Sol1("BenoitVR2", "BenoitConnectivity2", 1, coord, 0.0000f);
                }
                
                cout << "\t\t ";
                if(thetra.length() > 0)
                {
                    cout << " -=T=- ";
                    if(output >= 1)
                    {
                        cout<< " -=(" << thetra << ")=-- ";
                    }
                }
                cout<< "AVG: " << (avg/iter) << " ("<<iter<<")"<< endl;
                cout<< endl<< endl;
            }
            
            
            {
                avg = 0.0;
                cout<< "getThetraBenoitVR3Sol1 - BenoitVR3, BenoitConnectivity3 - "<< endl;
                for(int j = 0; j < iter ; j ++)
                {
                    avg += getThetraBenoitVR3Sol1("BenoitVR3", "BenoitConnectivity3", 1, coord, 0.0000f);
                }
                
                cout << "\t\t ";
                if(thetra.length() > 0)
                {
                    cout << " -=T=- ";
                    if(output >= 1)
                    {
                        cout<< " -=(" << thetra << ")=-- ";
                    }
                }
                cout<< "AVG: " << (avg/iter) << " ("<<iter<<")"<< endl;
                cout<< endl<< endl;
            }
            
            {
                avg = 0.0;
                cout<< "getThetraBenoitVR1Sol2 - BenoitVR, BenoitConnectivity - "<< endl;
                for(int j = 0; j < iter ; j ++)
                {
                    avg += getThetraBenoitVR1Sol2("BenoitVR", "BenoitConnectivity", 1, coord, 0.0000f);
                }
                
                cout << "\t\t ";
                if(thetra.length() > 0)
                {
                    cout << " -=T=- ";
                    if(output >= 1)
                    {
                        cout<< " -=(" << thetra << ")=-- ";
                    }
                }
                cout<< "AVG: " << (avg/iter) << " ("<<iter<<")"<< endl;
                cout<< endl<< endl;
            }
            {
                avg = 0.0;
                cout<< "getThetraBenoitVR2Sol2 - BenoitVR2, BenoitConnectivity2 - "<< endl;
                for(int j = 0; j < iter ; j ++)
                {
                    avg += getThetraBenoitVR2Sol2("BenoitVR2", "BenoitConnectivity2", 1, coord, 0.0000f);
                }
                
                cout << "\t\t ";
                if(thetra.length() > 0)
                {
                    cout << " -=T=- ";
                    if(output >= 1)
                    {
                        cout<< " -=(" << thetra << ")=-- ";
                    }
                }
                cout<< "AVG: " << (avg/iter) << " ("<<iter<<")"<< endl;
                cout<< endl<< endl;
            }
            
            {
                avg = 0.0;
                cout<< "getThetraBenoitVR3Sol2 - BenoitVR3, BenoitConnectivity3 - "<< endl;
                for(int j = 0; j < iter ; j ++)
                {
                    avg += getThetraBenoitVR3Sol2("BenoitVR3", "BenoitConnectivity3", 1, coord, 0.0000f);
                }
                
                cout << "\t\t ";
                if(thetra.length() > 0)
                {
                    cout << " -=T=- ";
                    if(output >= 1)
                    {
                        cout<< " -=(" << thetra << ")=-- ";
                    }
                }
                cout<< "AVG: " << (avg/iter) << " ("<<iter<<")"<< endl;
                cout<< endl<< endl;
            }
            cout<< endl<< endl;
            
            
        }
    }
    return 0;
}


//Scan example--
//    scan  'BenoitVR', { FILTER => "SingleColumnValueFilter('cf1', 'X', >, 'binary:0.80') AND  SingleColumnValueFilter('cf1', 'Y', >, 'binary:0.80') AND SingleColumnValueFilter('cf1', 'Z', >, 'binary:0.80')  AND SingleColumnValueFilter('cf1', 'X', <=, 'binary:0.95') AND  SingleColumnValueFilter('cf1', 'Y', <=, 'binary:0.95') AND SingleColumnValueFilter('cf1', 'Z', <=, 'binary:0.95')"  }

//scan "BenoitConnectivity3" , { FILTER => "SingleColumnValueFilter('cf1', 'Xmin', <= , 'binary:0.066') AND  SingleColumnValueFilter('cf1', 'Xmax', >= , 'binary:0.066') AND SingleColumnValueFilter('cf1', 'Ymin', <= , 'binary:0.091') AND  SingleColumnValueFilter('cf1', 'Ymax', >= , 'binary:0.091') AND SingleColumnValueFilter('cf1', 'Zmin', <= , 'binary:0.558') AND  SingleColumnValueFilter('cf1', 'Zmax', >= , 'binary:0.558')"  }



