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


#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

using namespace std;


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

struct resLine
{
    string first_elem;
    string result_name;
    string analysis_name;
    string result_type;
    int timeStep;
    string location;
    string GPlocation;
    
};

struct Scalar
{
    float Val;
};
struct Vector
{
    float Val[4];
};


struct TS
{
    vector<Scalar> PartitionId;
    vector<Vector> Height;
    vector<Vector> function;
};
struct tetra
{
    int id;
    int parti;
    int idP1, idP2, idP3, idP4;
    double min[3];
    double max[3];
};


struct vertex
{
    int id;
    int parti;
    float x, y, z;
    vector<tetra> listOfTetras;
    
};

vector<vertex> listOfVertices;
vector<TS> results;

vector<tetra> listOfTetras;

void deleteDropHbaseTable(string table, string server, int port)
{

    {
        cout<< "######### Disable and delete : "<<table <<endl;
        boost::shared_ptr<TTransport> socket(new TSocket(server, port));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        HbaseClient client(protocol);
        try
        {
            
            transport->open();
            /*
             Check if the table exists
             */
            if ( client.isTableEnabled(table) == 1)
            {
                cout <<"\t(enabled)" <<endl;
                client.disableTable(table);
                
                
            }
            client.deleteTable(table);
            transport->close();
        }
        catch (TException &tx)
        {
            cout << "\t\t@Error : " << tx.what()<<endl;
            
        }
        cout<< "********* Disable and delete : "<<table <<endl;
    }
}


void createHbaseTable(string table, string server, int port, int nbCol, int NumberOfVersion, bool memory)
{
    cout<< "######### Create : "<<table <<endl ;
    boost::shared_ptr<TTransport> socket(new TSocket(server, port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        
        transport->open();
        /*
         Check if the table does not exist
         */
        if ( client.isTableEnabled(table) == 0)
        {
            cout <<"\t(Not exists)" <<endl;
            std::vector<ColumnDescriptor> vcd;
            
            for(int i = 0; i < nbCol; i++)
            {
                /*
                 It is possible to add some specific features on each column :
                 (see Hbase.h in gen-cpp)
                 void __set_compression(const std::string& val);
                 */
                
                {
                    //Create the Column Name
                    stringstream colName("");
                    colName << "cf" <<(i+1);
                    vcd.push_back(ColumnDescriptor());
                    
                    //Set the column Name
                    vcd.back().__set_name(colName.str());
                }
                
                //Set the number of Version
                vcd.back().__set_maxVersions(NumberOfVersion);
                vcd.back().__set_inMemory(memory);
            }
            
            
            client.createTable(table,vcd);
        }
        
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "\t\t@Error : " << tx.what()<<endl;
        
    }
    cout<< "********* Create : "<<table <<endl;
}

/*
 Overloading of the table creation
 */
void createHbaseTable(string table, string server, int port, int nbCol, int NumberOfVersion)
{
    createHbaseTable( table,  server,  port,  nbCol,  NumberOfVersion, false);
}

void createHbaseTable(string table, string server, int port, int nbCol)
{
    createHbaseTable( table,  server,  port,  nbCol,  1, false);
}

void createHbaseTable(string table, string server, int port, int nbCol, bool memory)
{
    createHbaseTable( table,  server,  port,  nbCol,  1, memory);
}

void writeVRHbaseData(string table, string server, int port)
{
    int cpt = 0;
    /*
     We assume thaht the table exsits
     */
    cout<< "######### Add Elements : "<<table <<endl;
    boost::shared_ptr<TTransport> socket(new TSocket(server, port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        
        transport->open();
        vector<BatchMutation> mutations;
        vector<Mutation> mutation;
        std::map<Text, Text> attributes;
        for(int i = 0 ; i < listOfVertices.size() ; i++)
        {
            for(int j = 0 ; j < results.at(i).function.size() ; j++)
            {
                
                
                mutations.push_back(BatchMutation());
                mutation.clear();
                
                std::stringstream key;
                
                key <<"P" << listOfVertices.at(i).parti;
                
                //timeStep
                key << "TS" << std::setfill('0');
                key << std::setw(6);
                key << j;
                
                key << "ID" << std::setfill('0');
                key << std::setw(6);
                key << listOfVertices.at(i).id;
                
                
                
                mutations.back().__set_row(key.str());
                
                stringstream ssTmp;
                stringstream sssTmp;
                
                /* Store the coordinates */
                {
                    ssTmp.str("");
                    ssTmp<<listOfVertices.at(i).x;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf1:X";
                    mutation.back().value=ssTmp.str();
                }
                
                {
                    ssTmp.str("");
                    ssTmp<<listOfVertices.at(i).y;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf1:Y";
                    mutation.back().value=ssTmp.str();
                }
                
                {
                    ssTmp.str("");
                    ssTmp<<listOfVertices.at(i).z;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf1:Z";
                    mutation.back().value=ssTmp.str();
                }
                
                /*
                 Store the Partition
                 /!\ Not necessary because alredy stored
                 */
                {
                    ssTmp.str("");
                    ssTmp<< results.at(i).PartitionId.at(j).Val;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf2:PartitionId";
                    mutation.back().value=ssTmp.str();
                }
                
                
                /* Store the results for "function"  */
                for(int k = 0; k < 4; k++)
                {
                    sssTmp.str("");
                    sssTmp << "cf2:function" << k;
                    ssTmp.str("");
                    ssTmp<< results.at(i).function.at(j).Val[k];
                    mutation.push_back(Mutation());
                    mutation.back().column= sssTmp.str();
                    mutation.back().value=ssTmp.str();
                }
                
                /* Store the results for "Height"  */
                for(int k = 0; k < 4; k++)
                {
                    sssTmp.str("");
                    sssTmp << "cf2:Height" << k;
                    ssTmp.str("");
                    ssTmp<< results.at(i).Height.at(j).Val[k];
                    mutation.push_back(Mutation());
                    mutation.back().column= sssTmp.str();
                    mutation.back().value=ssTmp.str();
                }
                /* Store the list of tetrahedrons  */
                for(int k = 0; k < listOfVertices.at(i).listOfTetras.size(); k++)
                {
                    sssTmp.str("");
                    sssTmp << "cf2:Tetra" << k;
                    
                    ssTmp.str("");
                    ssTmp << listOfVertices.at(i).listOfTetras.at(k).id << " " << listOfVertices.at(i).listOfTetras.at(k).idP1 << " "<< listOfVertices.at(i).listOfTetras.at(k).idP2 << " "<< listOfVertices.at(i).listOfTetras.at(k).idP3 << " " << listOfVertices.at(i).listOfTetras.at(k).idP4 ;
                    mutation.push_back(Mutation());
                    mutation.back().column= sssTmp.str();
                    mutation.back().value=ssTmp.str();
                }
                
                
                //Apply mutations
                client.mutateRow(table.c_str(), key.str() ,mutation, attributes);
                cpt ++;
                if(cpt == 500)
                {
                    cout<<"a"<<endl;
                    usleep(1000);
                    cpt = 0;
                }
            }
        }
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "\t\t@Error : " << tx.what()<<endl;
        
    }
    cout<< "********* Add Elements : "<<table <<endl;
}

void writeVRHbaseDataV2(string table, string server, int port)
{
    /*
     We assume thaht the table exsits
     */
    cout<< "######### Add Elements : "<<table <<endl;
    boost::shared_ptr<TTransport> socket(new TSocket(server, port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        
        transport->open();
        vector<BatchMutation> mutations;
        vector<Mutation> mutation;
        std::map<Text, Text> attributes;
        for(int j = 0 ; j < results.at(0).function.size() ; j++)
        {
            
            for(int i = 0 ; i < listOfVertices.size() ; i++)
            {
                mutations.push_back(BatchMutation());
                mutation.clear();
                
                std::stringstream key;
                
                key << "ID" << std::setfill('0') << std::setw(16);
                key << listOfVertices.at(i).id;
                key <<"P" << listOfVertices.at(i).parti;
                
                
                
                mutations.back().__set_row(key.str());
                
                stringstream ssTmp;
                stringstream sssTmp;
                
                /* Store the coordinates */
                {
                    ssTmp.str("");
                    ssTmp<<listOfVertices.at(i).x;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf1:X";
                    mutation.back().value=ssTmp.str();
                }
                
                {
                    ssTmp.str("");
                    ssTmp<<listOfVertices.at(i).y;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf1:Y";
                    mutation.back().value=ssTmp.str();
                }
                
                {
                    ssTmp.str("");
                    ssTmp<<listOfVertices.at(i).z;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf1:Z";
                    mutation.back().value=ssTmp.str();
                }
                
                
                
                /* Store the results for "function"  */
                for(int k = 0; k < 4; k++)
                {
                    sssTmp.str("");
                    sssTmp << "cf2:function" << k;
                    ssTmp.str("");
                    ssTmp<< results.at(i).function.at(j).Val[k];
                    mutation.push_back(Mutation());
                    mutation.back().column= sssTmp.str();
                    mutation.back().value=ssTmp.str();
                }
                
                /* Store the results for "Height"  */
                for(int k = 0; k < 4; k++)
                {
                    sssTmp.str("");
                    sssTmp << "cf2:Height" << k;
                    ssTmp.str("");
                    ssTmp<< results.at(i).Height.at(j).Val[k];
                    mutation.push_back(Mutation());
                    mutation.back().column= sssTmp.str();
                    mutation.back().value=ssTmp.str();
                }
                
                /* Store the list of tetrahedrons  */
                for(int k = 0; k < listOfVertices.at(i).listOfTetras.size(); k++)
                {
                    sssTmp.str("");
                    sssTmp << "cf2:Tetra" << k;
                    
                    ssTmp.str("");
                    ssTmp << listOfVertices.at(i).listOfTetras.at(k).id << " " <<listOfVertices.at(i).listOfTetras.at(k).idP1 << " "<< listOfVertices.at(i).listOfTetras.at(k).idP2 << " "<< listOfVertices.at(i).listOfTetras.at(k).idP3 << " " << listOfVertices.at(i).listOfTetras.at(k).idP4 ;
                    mutation.push_back(Mutation());
                    mutation.back().column= sssTmp.str();
                    mutation.back().value=ssTmp.str();
                }
                
                
                //Apply mutations
                client.mutateRowTs(table.c_str(),key.str() ,mutation, j,attributes);
                usleep(20);
            }
            
        }
        
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "\t\t@Error : " << tx.what() << " " << endl;
        
    }
    cout<< "********* Add Elements : "<<table <<endl;
}

void writeVRHbaseDataV3(string table, string server, int port)
{
    /*
     We assume thaht the table exsits
     */
    cout<< "######### Add Elements : "<<table <<endl ;
    boost::shared_ptr<TTransport> socket(new TSocket(server, port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        
        transport->open();
        vector<BatchMutation> mutations;
        vector<Mutation> mutation;
        std::map<Text, Text> attributes;
        for(int j = 0 ; j < results.at(0).function.size() ; j++)
        {
            
            for(int i = 0 ; i < listOfVertices.size() ; i++)
            {
                mutations.push_back(BatchMutation());
                mutation.clear();
                
                std::stringstream key;
                
                
                key << "ID" << std::setfill('0') << std::setw(16);
                key << listOfVertices.at(i).id;
                
                
                
                mutations.back().__set_row(key.str());
                
                stringstream ssTmp;
                stringstream sssTmp;
                
                /* Store the coordinates */
                {
                    ssTmp.str("");
                    ssTmp<<listOfVertices.at(i).x;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf1:X";
                    mutation.back().value=ssTmp.str();
                }
                
                {
                    ssTmp.str("");
                    ssTmp<<listOfVertices.at(i).y;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf1:Y";
                    mutation.back().value=ssTmp.str();
                }
                
                {
                    ssTmp.str("");
                    ssTmp<<listOfVertices.at(i).z;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf1:Z";
                    mutation.back().value=ssTmp.str();
                }
                
                
                
                /* Store the results for "function"  */
                for(int k = 0; k < 4; k++)
                {
                    sssTmp.str("");
                    sssTmp << "cf2:function" << k;
                    ssTmp.str("");
                    ssTmp<< results.at(i).function.at(j).Val[k];
                    mutation.push_back(Mutation());
                    mutation.back().column= sssTmp.str();
                    mutation.back().value=ssTmp.str();
                }
                
                {
                    ssTmp.str("");
                    ssTmp<< results.at(i).PartitionId.at(j).Val;
                    mutation.push_back(Mutation());
                    mutation.back().column="cf2:PartitionId";
                    mutation.back().value=ssTmp.str();
                }
                
                /* Store the results for "Height"  */
                for(int k = 0; k < 4; k++)
                {
                    sssTmp.str("");
                    sssTmp << "cf2:Height" << k;
                    ssTmp.str("");
                    ssTmp<< results.at(i).Height.at(j).Val[k];
                    mutation.push_back(Mutation());
                    mutation.back().column= sssTmp.str();
                    mutation.back().value=ssTmp.str();
                }
                
                /* Store the list of tetrahedrons  */
                for(int k = 0; k < listOfVertices.at(i).listOfTetras.size(); k++)
                {
                    sssTmp.str("");
                    sssTmp << "cf2:Tetra" << k;
                    
                    ssTmp.str("");
                    ssTmp << listOfVertices.at(i).listOfTetras.at(k).id << " " << listOfVertices.at(i).listOfTetras.at(k).idP1 << " "<< listOfVertices.at(i).listOfTetras.at(k).idP2 << " "<< listOfVertices.at(i).listOfTetras.at(k).idP3 << " " << listOfVertices.at(i).listOfTetras.at(k).idP4 ;
                    mutation.push_back(Mutation());
                    mutation.back().column= sssTmp.str();
                    mutation.back().value=ssTmp.str();
                }
                
                //Apply mutations
                client.mutateRowTs(table.c_str(), key.str() , mutation, j,attributes);
                usleep(20);
                
            }
        }
        
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "\t\t@Error : " << tx.what()<<endl;
        
    }
    cout<< "********* Add Elements : "<<table <<endl;
}


void writeConnectivityHbaseData(string table, string server, int port)
{
    /*
     We assume thaht the table exsits
     */
    cout<< "######### Add Elements : " << table << endl;
    boost::shared_ptr<TTransport> socket(new TSocket(server, port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        
        transport->open();
        vector<BatchMutation> mutations;
        vector<Mutation> mutation;
        std::map<Text, Text> attributes;
        for(int i = 0 ; i < listOfTetras.size() ; i++)
        {
            mutations.push_back(BatchMutation());
            mutation.clear();
            
            std::stringstream key;
            key << "P";
            key << std::setfill('0');
            key << std::setw(6);
            key << listOfTetras.at(i).parti;
            
            key << "ID";
            key << std::setfill('0');
            key << std::setw(6);
            key << listOfTetras.at(i).id;
            
            mutations.back().__set_row(key.str());
            
            stringstream ssTmp;
            stringstream sssTmp;
            
            //Add element to CF1
            {
                //creating a mutation
                mutation.push_back(Mutation());
                //empty the string
                ssTmp.str("");
                //Add cotnetn of idP1 to the empty string
                ssTmp << listOfTetras.at(i).idP1;
                //select the column name
                mutation.back().column="cf1:idP1";
                //store the value
                mutation.back().value=ssTmp.str();
            }
            
            //Add element to CF1
            {
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).idP2;
                mutation.back().column="cf1:idP2";
                mutation.back().value=ssTmp.str();
            }
            
            //Add element to CF1
            {
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).idP3;
                mutation.back().column="cf1:idP3";
                mutation.back().value=ssTmp.str();
            }
            
            //Add element to CF1
            {
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).idP4;
                mutation.back().column="cf1:idP4";
                mutation.back().value=ssTmp.str();
            }
            
            //ADD of the AABB
            {
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).min[0];
                mutation.back().column="cf1:Xmin";
                mutation.back().value=ssTmp.str();
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).min[1];
                mutation.back().column="cf1:Ymin";
                mutation.back().value=ssTmp.str();
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).min[2];
                mutation.back().column="cf1:Zmin";
                mutation.back().value=ssTmp.str();
                
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).max[0];
                mutation.back().column="cf1:Xmax";
                mutation.back().value=ssTmp.str();
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).max[1];
                mutation.back().column="cf1:Ymax";
                mutation.back().value=ssTmp.str();
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).max[2];
                mutation.back().column="cf1:Zmax";
                mutation.back().value=ssTmp.str();
                
            }
            
            //Apply mutations
            client.mutateRow(table.c_str(),key.str() ,mutation, attributes);
            usleep(20);
            
        }
        
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "\t\t@Error : " << tx.what()<<endl;
        
    }
    cout<< "********* Add Elements : "<<table <<endl;
}

void writeConnectivityHbaseDataV3(string table, string server, int port)
{
    /*
     We assume thaht the table exsits
     */
    cout<< "######### Add Elements : " << table <<endl;
    boost::shared_ptr<TTransport> socket(new TSocket(server, port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        
        transport->open();
        vector<BatchMutation> mutations;
        vector<Mutation> mutation;
        std::map<Text, Text> attributes;
        
        for(int i = 0 ; i < listOfTetras.size() ; i++)
        {
            mutations.push_back(BatchMutation());
            mutation.clear();
            
            std::stringstream key;
            key << "ID";
            key << std::setfill('0');
            key << std::setw(6);
            key << listOfTetras.at(i).id;
            
            mutations.back().__set_row(key.str());
            
            stringstream ssTmp;
            stringstream sssTmp;
            
            //Add element to CF1
            {
                //creating a mutation
                mutation.push_back(Mutation());
                //empty the string
                ssTmp.str("");
                //Add cotnetn of idP1 to the empty string
                ssTmp << listOfTetras.at(i).idP1;
                //select the column name
                mutation.back().column="cf1:idP1";
                //store the value
                mutation.back().value=ssTmp.str();
            }
            
            //Add element to CF1
            {
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).idP2;
                mutation.back().column="cf1:idP2";
                mutation.back().value=ssTmp.str();
            }
            
            //Add element to CF1
            {
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).idP3;
                mutation.back().column="cf1:idP3";
                mutation.back().value=ssTmp.str();
            }
            
            //Add element to CF1
            {
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).idP4;
                mutation.back().column="cf1:idP4";
                mutation.back().value=ssTmp.str();
            }
            //ADD of the AABB
            {
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).min[0];
                mutation.back().column="cf1:Xmin";
                mutation.back().value=ssTmp.str();
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).min[1];
                mutation.back().column="cf1:Ymin";
                mutation.back().value=ssTmp.str();
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).min[2];
                mutation.back().column="cf1:Zmin";
                mutation.back().value=ssTmp.str();
                
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).max[0];
                mutation.back().column="cf1:Xmax";
                mutation.back().value=ssTmp.str();
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).max[1];
                mutation.back().column="cf1:Ymax";
                mutation.back().value=ssTmp.str();
                
                mutation.push_back(Mutation());
                ssTmp.str("");
                ssTmp << listOfTetras.at(i).max[2];
                mutation.back().column="cf1:Zmax";
                mutation.back().value=ssTmp.str();
                
                
            }
            //Set the created mutation to the mutations vector
            //Apply mutations
            client.mutateRow(table.c_str(),key.str() , mutation, attributes);
            usleep(20);
            
        }
        transport->close();
    }
    catch (TException &tx)
    {
        cout << "\t\t@Error : " << tx.what()<<endl;
        
    }
    cout<< "********* Add Elements : "<<table <<endl;
}

void parseMSHLine(int state, string line, int parti)
{
    stringstream ss(line);
    switch (state)
    {
        case 1:
        {
            vertex p ;
            p.parti = parti;
            ss >> p.id >> p.x >> p.y >> p.z ;
            int i = 0;
            while ( i < listOfVertices.size() && listOfVertices.at(i).id !=  p.id )
            {
                i++;
            }
            if( i >= listOfVertices.size())
            {
                listOfVertices.push_back(p);
                //timeStep ts;
                results.push_back( TS());
            }
            break;
        }
            
            
        case 2:
        {
            tetra t ;
            t.parti = parti;
            
            ss >> t.id >> t.idP1 >> t.idP2 >> t.idP3 >> t.idP4;
            listOfTetras.push_back(t);
            break;
        }
            
            
    }
}
void parseRESVector(string line, resLine res)
{
    stringstream ss(line);
    int idParticule;
    float tab[4];
    ss >> idParticule >> tab[0] >> tab[1] >> tab[2] >> tab[3];
    int i = 0;
    while ( i < listOfVertices.size() && listOfVertices.at(i).id != idParticule)
    {
        i++;
    }
    if (i != listOfVertices.size())
    {
        if(res.result_name.find("Height") > 0 && res.result_name.find("Height") < res.result_name.size())
        {
            while(results.at(i).Height.size() < res.timeStep)
            {
                results.at(i).Height.push_back(Vector());
            }
            for( int j = 0; j < 4 ; j ++)
            {
                results.at(i).Height.at(res.timeStep-1).Val[j] = tab[j];
            }
        }
        else if(res.result_name.find("Vectorfunction") > 0 && res.result_name.find("Vectorfunction") < res.result_name.size())
        {
            while(results.at(i).function.size() < res.timeStep)
            {
                results.at(i).function.push_back(Vector());
            }
            for( int j = 0; j < 4 ; j ++)
            {
                results.at(i).function.at(res.timeStep-1).Val[j] = tab[j];
            }
        }
    }
    else
    {
        cout<< "###Error : parseRESVector"<<endl;
        exit(0);
    }
}
void parseRESScalar(string line, resLine res)
{
    stringstream ss(line);
    int idParticule;
    int scalarValue;
    ss >> idParticule >> scalarValue;
    int i = 0;
    while ( i < listOfVertices.size() && listOfVertices.at(i).id != idParticule)
    {
        i++;
    }
    if (i != listOfVertices.size())
    {
        while(results.at(i).PartitionId.size() < res.timeStep)
        {
            results.at(i).PartitionId.push_back(Scalar());
        }
        results.at(i).PartitionId.at(res.timeStep-1).Val = scalarValue;
    }
}

/*function... might want it in some class?*/
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }
    
    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}


void computeAABB()
{
    for (int i = 0 ; i < listOfTetras.size() ; i++)
    {
        double min[3] = {std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max()};
        double max[3] = {std::numeric_limits<double>::min(),std::numeric_limits<double>::min(),std::numeric_limits<double>::min()};
        int elem = 0;
        
        int j = 0;
        while (j < listOfVertices.size() && elem < 4)
        {
            if(listOfVertices.at(j).id == listOfTetras.at(i).idP1 || listOfVertices.at(j).id == listOfTetras.at(i).idP2 || listOfVertices.at(j).id == listOfTetras.at(i).idP3 || listOfVertices.at(j).id == listOfTetras.at(i).idP4)
            {
                if(listOfVertices.at(j).x < min[0])
                {
                    min[0] = listOfVertices.at(j).x;
                }
                if(listOfVertices.at(j).x > max[0])
                {
                    max[0] = listOfVertices.at(j).x;
                }
                
                if(listOfVertices.at(j).y < min[1])
                {
                    min[1] = listOfVertices.at(j).y;
                }
                if(listOfVertices.at(j).y > max[1])
                {
                    max[1] = listOfVertices.at(j).y;
                }
                
                if(listOfVertices.at(j).z < min[2])
                {
                    min[2] = listOfVertices.at(j).z;
                }
                if(listOfVertices.at(j).z > max[2])
                {
                    max[2] = listOfVertices.at(j).z;
                }
                
                elem++;
            }
            j++;
        }
        if(elem ==4)
        {
            for (int j = 0; j < 3; j++)
            {
                listOfTetras.at(i).min[j] = min[j];
                listOfTetras.at(i).max[j] = max[j];
            }
            
        }
        else
            
        {
            cout <<"ERROR AABB !!"<< endl;
            exit(0);
        }
    }
}

void addThetra()
{
    for (int i = 0 ; i < listOfVertices.size() ; i++)
    {
        for (int j = 0 ; j < listOfTetras.size() ; j++)
        {
            if(listOfTetras.at(j).idP1 == listOfVertices.at(i).id
               || listOfTetras.at(j).idP2 == listOfVertices.at(i).id
               || listOfTetras.at(j).idP3 == listOfVertices.at(i).id
               || listOfTetras.at(j).idP4 == listOfVertices.at(i).id)
            {
                listOfVertices.at(i).listOfTetras.push_back(listOfTetras.at(j));
            }
        }
    }
}

bool compareByID(const vertex &a, const vertex &b)
{
    return a.id < b.id;
}


int main(int argc, const char * argv[])
{
    srand(time(NULL));
    vector<string> files = vector<string>();
    
    getdir("data/",files);
    int parti = 0;
    
    /*
     In these part, we parse the msh files, in roder to feed : listOfVertices and listOfTetras;
     /!\ this parsing is based on the example provided by CIMNE, for example, with 2D data, some errors may occurs.
     */
    
    for(int i = 0; i < files.size(); i++)
    {
        int state = 0;
        uint pos =  files.at(i).find("msh");
        if(pos == files.at(i).size() - 3)
        {
            std::ifstream infile("data/" + files.at(i));
            std::string line;
            while (std::getline(infile, line))
            {
                //        cout << line <<endl;
                std::istringstream iss(line);
                if(line.find("Coordinates") == 0)
                {
                    state = 1;
                }
                else if(line.find("end coordinates") == 0)
                {
                    state = 0;
                }
                else if(line.find("Elements") == 0)
                {
                    state = 2;
                }
                else if(line.find("end elements") == 0)
                {
                    state = 0;
                }
                else
                {
                    parseMSHLine(state, line, parti);
                }
                
            }
            parti++;
        }
    }
    
    /*
     In these part, we parse the msh files, in roder to feed : listOfResults;
     /!\ this parsing is based on the example provided by CIMNE, for example, with 2D data, some errors may occurs.
     */
    
    parti = 0;
    for(int i = 0; i < files.size(); i++)
    {
        int state = 0;
        uint pos =  files.at(i).find("res");
        if(pos == files.at(i).size() - 3)
        {
            std::ifstream infile("data/" + files.at(i));
            std::string line;
            resLine res;
            while (std::getline(infile, line))
            {
                //        cout << line <<endl;
                if (line.size() > 1)
                {
                    
                    std::istringstream iss(line);
                    if(line.find("Result") == 0)
                    {
                        stringstream ss(line);
                        if(line.find("Vector function") >=0 && line.find("Vector function") < line.size())
                        {
                            string tmp1, tmp2;
                            ss >> res.first_elem >> tmp1 >> tmp2 >> res.analysis_name >> res.timeStep >> res.result_type >> res.location >> res.GPlocation ;
                            res.result_name = tmp1 + tmp2;
                        }
                        else
                        {
                            ss >> res.first_elem >> res.result_name >> res.analysis_name >> res.timeStep >> res.result_type >> res.location >> res.GPlocation ;
                        }
                        
                    }
                    else if(line.find("Values") == 0)
                    {
                        state = state + 1;
                    }
                    
                    else if(line.find("End Values") == 0 || line.find("End values") == 0)
                    {
                        state = 0;
                    }
                    else if(line.find("ComponentNames") == 0)
                    {
                        
                    }
                    else
                    {
                        if(res.result_type == "Scalar")
                        {
                            parseRESScalar(line, res);
                        }
                        else if(res.result_type == "Vector")
                        {
                            parseRESVector(line, res);
                            
                        }
                    }
                }
            }
            parti++;
        }
    }
    /*
     Here I am doing a random check in order to have a visual feedback on the parsing
     In the example provided by CIMNE, 10 time steps have been produced.
     The output has to look like :
     
     ####################
     2060 10051 2060
     --------------------
     1494 : 253 p:2 - 10 10 10
     23 : 81 p:0 - 10 10 10
     115 : 400 p:0 - 10 10 10
     1457 : 176 p:2 - 10 10 10
     1067 : 1625 p:1 - 10 10 10
     1379 : 47 p:2 - 10 10 10
     18 : 75 p:0 - 10 10 10
     1467 : 209 p:2 - 10 10 10
     2052 : 1964 p:3 - 10 10 10
     1824 : 325 p:3 - 10 10 10
     ####################
     
     */
    cout<<"####################"<<endl;
    cout<< listOfVertices.size() << " " <<listOfTetras.size() << " " <<results.size()  << endl;
    cout<<"--------------------"<<endl;
    for(int i = 0; i < 10; i++)
    {
        int k = rand()%listOfVertices.size() - 1 ;
        
        cout <<"\t " << k << " : " << listOfVertices.at(k).id <<" p:" <<  listOfVertices.at(k).parti<< " - "  <<results.at(k).PartitionId.size() << " " <<results.at(k).Height.size()<< " " <<results.at(k).function.size()<<endl;
        
    }
    cout<<"####################"<<endl;
    
    
    
    //Sort operation in order to simplify the computation of the
    sort(listOfVertices.begin(), listOfVertices.end(), compareByID);
    
    
    /*
     In order to decrease the query time, one solution can be based on computaion of AABB.
     */
    computeAABB();
    
    
    /*
     In order to decrease the query time, Anothe solution will store thetraedron inforamtion in vertices.
     This solution remove one layer of query for the access of the thetrahedron
     */
    addThetra();
    
    
    /*
     HERE:
     This function has to be update in order to store data in the correct format.
     */
    
    string server="pez001";
    int port = 9095;
    {
        /*
         The simplest use case :
         - The first table stores for each pation, time, and particule a row (each row contains result values on cf2)
         - The second table will contains the connectivty (with a Row key based on parition and id of the connectivity (This table will be stored in memory - small sized table).
         */
        
        /* Example - scan "testBenoitVR"  - :
         P3TS000009ID002043                                       column=cf1:X, timestamp=1433335595457, value=0.3
         P3TS000009ID002043                                       column=cf1:Y, timestamp=1433335595457, value=0
         P3TS000009ID002043                                       column=cf1:Z, timestamp=1433335595457, value=0
         P3TS000009ID002043                                       column=cf2:Height0, timestamp=1433335595457, value=0.3
         P3TS000009ID002043                                       column=cf2:Height1, timestamp=1433335595457, value=0
         P3TS000009ID002043                                       column=cf2:Height2, timestamp=1433335595457, value=0
         P3TS000009ID002043                                       column=cf2:Height3, timestamp=1433335595457, value=0.3
         P3TS000009ID002043                                       column=cf2:PartitionId, timestamp=1433335595457, value=2
         P3TS000009ID002043                                       column=cf2:function0, timestamp=1433335595457, value=0.129
         P3TS000009ID002043                                       column=cf2:function1, timestamp=1433335595457, value=-2.08834
         P3TS000009ID002043                                       column=cf2:function2, timestamp=1433335595457, value=0
         P3TS000009ID002043                                       column=cf2:function3, timestamp=1433335595457, value=2.09232
         */
        string table ="BenoitVR";
        usleep(500);
        deleteDropHbaseTable(table, server, port);
        usleep(500);
        createHbaseTable(table, server, port, 2);
        usleep(500);
        writeVRHbaseData(table, server, port);
        
        /* Example :
         P000003ID010049                                          column=cf1:idP1, timestamp=1433335193357, value=1734
         P000003ID010049                                          column=cf1:idP2, timestamp=1433335193357, value=1603
         P000003ID010049                                          column=cf1:idP3, timestamp=1433335193357, value=1688
         P000003ID010049                                          column=cf1:idP4, timestamp=1433335193357, value=1558
         */
        table ="BenoitConnectivity";
        usleep(500);
        deleteDropHbaseTable(table, server, port);
        usleep(500);
        createHbaseTable(table, server, port, 1);
        usleep(500);
        writeConnectivityHbaseData(table, server, port);
    }
    
    usleep(5000);
    {
        int version = 100;
        /*
         A anothe example wich use some of the HBase features :
         - The first table is composed of the same structure tahn before, but key is based on Parition ID + particule ID. We HAve multiple version of particle and we have set the timestep to the timestep simulation.
         - The second table will contains the connectivty (with a Row key based on parition and id of the connectivity (This table will be stored in memory - small sized table).
         */
        
        /* Example ( scan "testBenoitVR2", {TIMESTAMP => 7}) :
         P3ID002043                                               column=cf1:X, timestamp=7, value=0.3
         P3ID002043                                               column=cf1:Y, timestamp=7, value=0
         P3ID002043                                               column=cf1:Z, timestamp=7, value=0
         P3ID002043                                               column=cf2:Height0, timestamp=7, value=0.3
         P3ID002043                                               column=cf2:Height1, timestamp=7, value=0
         P3ID002043                                               column=cf2:Height2, timestamp=7, value=0
         P3ID002043                                               column=cf2:Height3, timestamp=7, value=0.3
         P3ID002043                                               column=cf2:function0, timestamp=7, value=0.16125
         P3ID002043                                               column=cf2:function1, timestamp=7, value=-1.77923
         P3ID002043                                               column=cf2:function2, timestamp=7, value=0
         P3ID002043                                               column=cf2:function3, timestamp=7, value=1.78652
         
         Example ( scan "testBenoitVR2", {TIMESTAMP => 1}) :
         P3ID002043                                               column=cf1:X, timestamp=1, value=0.3
         P3ID002043                                               column=cf1:Y, timestamp=1, value=0
         P3ID002043                                               column=cf1:Z, timestamp=1, value=0
         P3ID002043                                               column=cf2:Height0, timestamp=1, value=0.3
         P3ID002043                                               column=cf2:Height1, timestamp=1, value=0
         P3ID002043                                               column=cf2:Height2, timestamp=1, value=0
         P3ID002043                                               column=cf2:Height3, timestamp=1, value=0.3
         P3ID002043                                               column=cf2:function0, timestamp=1, value=0.645
         P3ID002043                                               column=cf2:function1, timestamp=1, value=-0.234715
         P3ID002043                                               column=cf2:function2, timestamp=1, value=0
         P3ID002043                                               column=cf2:function3, timestamp=1, value=0.686379
         */
        string table = "BenoitVR2";
        usleep(500);
        deleteDropHbaseTable(table, server, port);
        usleep(500);
        createHbaseTable(table, server, port, 2, version);
        usleep(500);
        writeVRHbaseDataV2(table, server, port);
        
        /* Example :
         ID010049                                          column=cf1:idP1, timestamp=1433335193357, value=1734
         ID010049                                          column=cf1:idP2, timestamp=1433335193357, value=1603
         ID010049                                          column=cf1:idP3, timestamp=1433335193357, value=1688
         ID010049                                          column=cf1:idP4, timestamp=1433335193357, value=1558
         */
        table = "BenoitConnectivity2";
        usleep(500);
        deleteDropHbaseTable(table, server, port);
        usleep(500);
        createHbaseTable(table, server, port, 1, version);
        usleep(500);
        writeConnectivityHbaseDataV3(table, server, port);
    }
    usleep(5000);
    {
        int version = 100;
        /*
         A anothe example wich use some of the HBase features :
         - The first table is composed of the same structure tahn before, but key is based on Parition ID + particule ID. We HAve multiple version of particle and we have set the timestep to the timestep simulation.
         - The second table will contains the connectivty (with a Row key based on parition and id of the connectivity (This table will be stored in memory - small sized table).
         */
        
        /* Example ( scan "testBenoitVR2", {TIMESTAMP => 7}) :
         ID002043                                               column=cf1:X, timestamp=7, value=0.3
         ID002043                                               column=cf1:Y, timestamp=7, value=0
         ID002043                                               column=cf1:Z, timestamp=7, value=0
         ID002043                                               column=cf2:Height0, timestamp=7, value=0.3
         ID002043                                               column=cf2:Height1, timestamp=7, value=0
         ID002043                                               column=cf2:Height2, timestamp=7, value=0
         ID002043                                               column=cf2:Height3, timestamp=7, value=0.3
         ID002043                                               column=cf2:PartitionId, timestamp=7, value=2
         ID002043                                               column=cf2:function0, timestamp=7, value=0.16125
         ID002043                                               column=cf2:function1, timestamp=7, value=-1.77923
         ID002043                                               column=cf2:function2, timestamp=7, value=0
         ID002043                                               column=cf2:function3, timestamp=7, value=1.78652
         
         Example ( scan "testBenoitVR2", {TIMESTAMP => 1}) :
         ID002043                                               column=cf1:X, timestamp=1, value=0.3
         ID002043                                               column=cf1:Y, timestamp=1, value=0
         ID002043                                               column=cf1:Z, timestamp=1, value=0
         ID002043                                               column=cf2:Height0, timestamp=1, value=0.3
         ID002043                                               column=cf2:Height1, timestamp=1, value=0
         ID002043                                               column=cf2:Height2, timestamp=1, value=0
         ID002043                                               column=cf2:Height3, timestamp=1, value=0.3
         ID002043                                                 column=cf2:PartitionId, timestamp=1, value=2
         ID002043                                               column=cf2:function0, timestamp=1, value=0.645
         ID002043                                               column=cf2:function1, timestamp=1, value=-0.234715
         ID002043                                               column=cf2:function2, timestamp=1, value=0
         ID002043                                               column=cf2:function3, timestamp=1, value=0.686379
         */
        string table = "BenoitVR3";
        usleep(500);
        deleteDropHbaseTable(table, server, port);
        usleep(500);
        createHbaseTable(table, server, port, 2, version);
        usleep(500);
        writeVRHbaseDataV3(table, server, port);
        
        /* Example :
         ID010049                                          column=cf1:idP1, timestamp=1433335193357, value=1734
         ID010049                                          column=cf1:idP2, timestamp=1433335193357, value=1603
         ID010049                                          column=cf1:idP3, timestamp=1433335193357, value=1688
         ID010049                                          column=cf1:idP4, timestamp=1433335193357, value=1558
         */
        table = "BenoitConnectivity3";
        usleep(500);
        deleteDropHbaseTable(table, server, port);
        usleep(500);
        createHbaseTable(table, server, port, 1, version);
        usleep(500);
        writeConnectivityHbaseDataV3(table, server, port);
    }
    
    return 0;
}
