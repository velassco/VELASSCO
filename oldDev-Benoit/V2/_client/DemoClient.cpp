//
//  main.cpp
//  fileGen
//
//  Created by benoit lange on 28/07/2014.
//  Copyright (c) 2014 INRIA. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <time.h>
#include <iomanip>
#include <sys/time.h>
#include <string>
#include <poll.h>
using namespace std;

///Include Thrift Hbase
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "Hbase.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;

using namespace  ::apache::hadoop::hbase::thrift;
///Include Thrift Hbase



///Include THRIFT HIVE
#include "hiveclient.h"
///Include THRIFT HIVE


struct particule
{
    long int it;
    long int id;
    double x, y, z;
};
struct option
{
    const char* name;
    int nb_args;
    const char* help;
};
static struct option long_options[] =
{
    {"host", 1, "[-host <Hadoop Master node>]"},
    {"nb_test", 1, "[-nb_test <nb tests>]"},
    {"nb_p", 1, "[-nb_p  <nb particules>]"},
    {"nb_i", 1, "[-nb_i <nb iterations>]"},
    {"local", 1, "[-local <path of local storage>]"},
    {"NFS", 1, "[-NFS <path of NFS storage>]"},
    {"ODBC", 1, "[-ODBC <table>]"},
    {"T_Hive", 1, "[-T_Hive <table>]"},
    {"T_HBASE", 1, "[-T_HBASE <table>]"},
    {"measure", 1, "[-measure <RA/R/W(4+2+1)>]"},
    {"iterR", 1, "[-iterR <num iter>]"},
    {"minR", 1, "[-minR <num iter>]"},
    {"maxR", 1, "[-maxR <num iter>]"},
    {"iterRA", 1, "[-iterRA <num iter>]"},
    {"OTHER_HB", 1, "[-OTHER_HB <table>]"},
    {"OTHER_HIVE", 1, "[-OTHER_HIVE <table>]"},
    {"help", 0, "[-help]"},
    {0, 0, 0}
};


//Particles
particule* particulesArray;

//Parameters
//Parameters
string host = "127.0.0.1";
int nb_test = 1;
int nb = 50;
int nbIt = 1;
int measure = 7;
std::vector<string> vectLocal;
std::vector<string> vectNFS;
std::vector<string> vectODBC;
std::vector<string> vectHive;
std::vector<string> vectHive2;
std::vector<string> vectHbase;
std::vector<string> vectHbase2;


//// Parametres
int min_1 = 0;
int max_1 = 0;
int iter_1 = 0;

int iter_2 = 0;
//// Parametres



///Struct Thrift Hbase
typedef std::vector<std::string> StrVec;
typedef std::map<std::string,std::string> StrMap;
typedef std::vector<ColumnDescriptor> ColVec;
typedef std::map<std::string,ColumnDescriptor> ColMap;
typedef std::vector<TCell> CellVec;
typedef std::map<std::string,TCell> CellMap;
///Struct Thrift Hbase


///THRIFT HIVE
#define RETURN_ON_ASSERT_ONE_ARG_CLOSE(condition, err_format, arg, db_conn, ret_val) {          \
if (condition) {                                                                              \
char error_buffer_[MAX_HIVE_ERR_MSG_LEN];                                                   \
fprintf(stderr, "----LINE %i: ", __LINE__);                                                 \
fprintf(stderr, err_format, arg);                                                           \
DBCloseConnection(db_conn, error_buffer_, sizeof(error_buffer_));                           \
}                                                                                             \
}
#define RETURN_ON_ASSERT_ONE_ARG(condition, err_format, arg, ret_val) {                         \
if (condition) {                                                                              \
fprintf(stderr, "----LINE %i: ", __LINE__);                                                 \
fprintf(stderr, err_format, arg);                                                           \
\
}                                                                                             \
}
#define MAX_FIELD_LEN 255
///THRIFT HIVE




double my_gettimeofday()
{
    struct timeval tmp_time;
    gettimeofday(&tmp_time, NULL);
    return tmp_time.tv_sec + (tmp_time.tv_usec * 1.0e-6L);
}

void checkError(int rc, char* fonction)
{
    if(rc != 0)
    {
        printf("################## problem\n");
        printf("(%s) : %d\n", fonction,rc);
        exit(0);
    }
}

void odbc(const char path[],int WR)
{
    double init = my_gettimeofday();
    if(WR == 2)
    {
        /*  SQLRETURN rc;
         SQLHANDLE env = 0;
         SQLHANDLE conn = 0;
         
         char user [256] = "vagrant";
         
         char pwd [256] = "vagrant";
         
         rc = SQLAllocHandle (SQL_HANDLE_ENV,0,&env);
         checkError(rc,"SQLAllocHandle");
         
         rc = SQLSetEnvAttr (env,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0);
         checkError(rc,"SQLSetEnvAttr");
         
         rc = SQLAllocHandle (SQL_HANDLE_DBC,env,&conn);
         checkError(rc,"SQLAllocHandle");
         
         rc = SQLConnect(conn,(SQLCHAR *)"test",SQL_NTS,(SQLCHAR *)user,SQL_NTS,(SQLCHAR *)pwd,SQL_NTS);
         checkError(rc,"SQLConnect");
         printf("Connection was successful\n");*/
        /*
         rc = SQLExecDirect(conn, (SQLCHAR *)"select * from test", SQL_NTS);
         checkError(rc,"SQLExecDirect");
         */
        /*    SQLPrepare(conn, (SQLCHAR *)"select * from test", SQL_NTS);
         SQLRETURN res2 = SQLExecute(conn);
         */
        printf("Deconection was successful\n");
        /*    rc = SQLDisconnect (conn);
         checkError(rc,"SQLDisconnect");*/
        
        
    }
    else if(WR == 4)
    {
        
        
    }
    if(WR == 1)
    cout<< 0 << ";";
    else
    cout<<  my_gettimeofday() - init << ";";
    
}

void local(const char path[],int WR)
{
    double init = my_gettimeofday();
    if(WR == 1)
    {
        ofstream myfile;
        myfile.open (path);
        
        myfile.precision(5);
        std::fixed;
        myfile<< std::fixed;
        int k=0;
        for (int i = 0; i < nbIt; i++)
        {
            for(int j = 0; j < nb; j++)
            {
                myfile  << particulesArray[k].it <<
                ";" << particulesArray[k].id <<
                ";" <<particulesArray[k].x <<
                ";" << particulesArray[k].y <<
                ";" << particulesArray[k].z  <<"\n";
                k++;
            }
        }
        myfile.close();
    }
    else if(WR == 2)
    {
        ifstream in(path);
        if(!in){
            cout << "Cannot open file.";
            exit (1);
        }
        char str[1024];
        vector<particule> parti;
        int id,ts;
        particule p;
        while(in){
            in.getline(str, 1024);      // Delimiter defaults to newline
            
            sscanf(str,"%d;%d;%lf;%lf;%lf", &p.id, &p.it, &p.x, &p.y, &p.z);
            if( p.id > min_1 && p.id < max_1 &&p.it == iter_1)
            {
                parti.push_back(particule());
                parti.back().id = p.id;
                parti.back().it = p.it;
                parti.back().x= p.x;
                parti.back().y = p.y;
                parti.back().z = p.z;
            }
            
        }
        in.close();
    }
    else if(WR == 4)
    {
        ifstream in(path);
        if(!in){
            cout << "Cannot open file.";
            exit (1);
        }
        char str[1024];
        vector<particule> parti;
        int id,ts;
        particule p;
        while(in){
            in.getline(str, 1024);      // Delimiter defaults to newline
            
            sscanf(str,"%d;%d;%lf;%lf;%lf", &p.id, &p.it, &p.x, &p.y, &p.z);
            if( p.it == iter_2)
            {
                parti.push_back(particule());
                parti.back().id = p.id;
                parti.back().it = p.it;
                parti.back().x= p.x;
                parti.back().y = p.y;
                parti.back().z = p.z;
            }
            
        }
        in.close();
        //        cout<<" taille locale : "<< parti.size()<<endl;
    }
    cout<<  my_gettimeofday() - init << ";";
}


static void
printRow(const vector<TRowResult> &rowResult)
{
    for (int i = 0; i < rowResult.size(); i++)
    {
        std::cout << "row: " << rowResult.at(i).row << ", cols: ";
        for (CellMap::const_iterator it = rowResult.at(i).columns.begin();
             it != rowResult.at(i).columns.end(); ++it)
        {
            std::cout << it->first << " => " << it->second.value << "; ";
        }
        std::cout << std::endl;
    }
}

string convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

void thrfit_hbase(const char path[], int WR)
{
    {
        double init = my_gettimeofday();
        boost::shared_ptr<TTransport> socket(new TSocket(host.c_str(), 9090));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        HbaseClient client(protocol);
        vector<string> thrift_list_table;
        thrift_list_table.push_back(string(path));
        try
        {
            transport->open();
            if(WR == 1)
            {
                
                vector<Mutation> mutations;
                {
                    int k=0;
                    
                    char buf[32];
                    char tmp1[32];
                    char tmp2[32];
                    char tmp3[32];
                    
                    
                    for (int i = 0; i < nb; i++)
                    {
                        
                        
                        for(int j = 0; j < nbIt; j++)
                        {
                            
                            mutations.push_back(Mutation());
                            sprintf(tmp1, "cf1:ts");
                            mutations.back().column = string(tmp1);
                            sprintf(tmp1, "%d", particulesArray[k].it);
                            mutations.back().value = string(tmp1);
                            
                            mutations.push_back(Mutation());
                            if(j == nbIt - 1)
                            {
                                sprintf(tmp1, "cf2:x");
                            }
                            else
                            {
                                sprintf(tmp1, "cf2:x%d",j);
                            }
                            mutations.back().column = string(tmp1);
                            sprintf(tmp1, "%lf", particulesArray[k].x);
                            mutations.back().value = string(tmp1);
                            
                            mutations.push_back(Mutation());
                            if(j == nbIt - 1)
                            {
                                sprintf(tmp1, "cf3:y");
                            }
                            else
                            {
                                sprintf(tmp1, "cf3:y%d",j);
                            }
                            mutations.back().column = string(tmp1);
                            sprintf(tmp1, "%lf", particulesArray[k].y);
                            mutations.back().value = string(tmp1);
                            
                            
                            mutations.push_back(Mutation());
                            if(j == nbIt - 1)
                            {
                                sprintf(tmp1, "cf4:z");
                            }
                            else
                            {
                                sprintf(tmp1, "cf4:z%d",j);
                            }
                            mutations.back().column = string(tmp1);
                            sprintf(tmp1, "%lf", particulesArray[k].z);
                            mutations.back().value = string(tmp1);
                            
                            sprintf(buf, "%0.15d", i);
                            std::string row(buf);
                            
                            client.mutateRow(*(thrift_list_table.begin()), row, mutations);
                            
                            k++;
                            
                        }
                    }
                }
                
            }
            else if(WR == 2)
            {
                char tmp1[32];
                char buf[32];
                
                
                
                sprintf(buf, "%0.15d", min_1);
                std::string pati_min(buf);
                
                sprintf(buf, "%0.15d", max_1);
                std::string pati_max(buf);
                
                StrVec columnNames;
                if(iter_1 == nbIt - 1)
                {
                    sprintf(tmp1, "cf2:x");
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf3:y");
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf4:z");
                    columnNames.push_back( string(tmp1));
                }
                else
                {
                    sprintf(tmp1, "cf2:x%d",iter_1);
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf3:y%d",iter_1);
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf4:z%d",iter_1);
                    columnNames.push_back( string(tmp1));
                }
                
                int scannerID = client.scannerOpenWithStop(*(thrift_list_table.begin()), pati_min, pati_max, columnNames);
                vector<TRowResult> rowsResult;
                client.scannerGetList(rowsResult, scannerID, max_1 - min_1);
                
                
                /* printRow(rowsResult);
                 cout << "taille hbase :"<<rowsResult.size()<<endl;*/
                client.scannerClose(scannerID);
                
            }
            else if(WR == 4)
            {
                
                char tmp1[32];
                char buf[32];
                
                sprintf(buf, "%0.15d", 0);
                std::string pati_min(buf);
                
                StrVec columnNames;
                if(iter_2 == nbIt - 1)
                {
                    sprintf(tmp1, "cf2:x");
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf3:y");
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf4:z");
                    columnNames.push_back( string(tmp1));
                }
                else
                {
                    sprintf(tmp1, "cf2:x%d",iter_2);
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf3:y%d",iter_2);
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf4:z%d",iter_2);
                    columnNames.push_back( string(tmp1));
                }
                
                
                int scannerID = client.scannerOpen(*(thrift_list_table.begin()), pati_min, columnNames);
                vector<TRowResult> rowsResult;
                client.scannerGetList(rowsResult, scannerID,nb);
                
                
                /*printRow(rowsResult);
                 cout << "taille hbase :"<<rowsResult.size()<<endl;*/
                client.scannerClose(scannerID);
                
            }
            transport->close();
        }
        catch (TException &tx)
        {
            cout << "Error : " << tx.what()<<endl;
            
        }
        
        cout<<  my_gettimeofday() - init << ";";
    }
}
void thrfit_hbase2(const char path[], int WR)
{
    double init = my_gettimeofday();
    {
        
        if(WR == 1)
        {
            
            vector<BatchMutation> bm;
            {
                int k=0;
                
                char buf[32];
                char tmp1[32];
                char tmp2[32];
                char tmp3[32];
                
                
                for (int i = 0; i < nb; i++)
                {
                    
                    
                    for(int j = 0; j < nbIt; j++)
                    {
                        vector<Mutation> mutations;
                        mutations.push_back(Mutation());
                        sprintf(tmp1, "cf1:ts");
                        mutations.back().column = string(tmp1);
                        sprintf(tmp1, "%d", particulesArray[k].it);
                        mutations.back().value = string(tmp1);
                        
                        mutations.push_back(Mutation());
                        if(j == nbIt - 1)
                        {
                            sprintf(tmp1, "cf2:x");
                        }
                        else
                        {
                            sprintf(tmp1, "cf2:x%d",j);
                        }
                        mutations.back().column = string(tmp1);
                        sprintf(tmp1, "%lf", particulesArray[k].x);
                        mutations.back().value = string(tmp1);
                        
                        mutations.push_back(Mutation());
                        if(j == nbIt - 1)
                        {
                            sprintf(tmp1, "cf3:y");
                        }
                        else
                        {
                            sprintf(tmp1, "cf3:y%d",j);
                        }
                        mutations.back().column = string(tmp1);
                        sprintf(tmp1, "%lf", particulesArray[k].y);
                        mutations.back().value = string(tmp1);
                        
                        
                        mutations.push_back(Mutation());
                        if(j == nbIt - 1)
                        {
                            sprintf(tmp1, "cf4:z");
                        }
                        else
                        {
                            sprintf(tmp1, "cf4:z%d",j);
                        }
                        mutations.back().column = string(tmp1);
                        sprintf(tmp1, "%lf", particulesArray[k].z);
                        mutations.back().value = string(tmp1);
                        
                        sprintf(buf, "%0.15d", i);
                        std::string row(buf);
                        
                        bm.push_back(BatchMutation());
                        bm.back().__set_mutations(mutations);
                        bm.back().__set_row(row);
                        k++;
                        if (k % 1000 == 0)
                        {
                            boost::shared_ptr<TTransport> socket(new TSocket(host.c_str(), 9090));
                            boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
                            boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
                            HbaseClient client(protocol);
                            vector<string> thrift_list_table;
                            thrift_list_table.push_back(string(path));
                            try
                            {
                                transport->open();
                                client.mutateRows(*(thrift_list_table.begin()), bm);
                                bm.clear();
                                transport->close();
                            }
                            catch (TException &tx)
                            {
                                cout << "Error : " << tx.what()<<endl;
                                
                                
                            }
                            
                        }
                    }
                    
                }
                boost::shared_ptr<TTransport> socket(new TSocket(host.c_str(), 9090));
                boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
                boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
                HbaseClient client(protocol);
                vector<string> thrift_list_table;
                thrift_list_table.push_back(string(path));
                try
                {
                    transport->open();
                    client.mutateRows(*(thrift_list_table.begin()), bm);
                    bm.clear();
                    transport->close();
                }
                catch (TException &tx)
                {
                    cout << "Error : " << tx.what()<<endl;
                    
                    
                }
                
            }
            
            
        }
        else if(WR == 2)
        {
            char tmp1[32];
            char buf[32];
            
            
            
            sprintf(buf, "%0.15d", min_1);
            std::string pati_min(buf);
            
            sprintf(buf, "%0.15d", max_1);
            std::string pati_max(buf);
            
            StrVec columnNames;
            if(iter_1 == nbIt - 1)
            {
                sprintf(tmp1, "cf2:x");
                columnNames.push_back( string(tmp1));
                sprintf(tmp1, "cf3:y");
                columnNames.push_back( string(tmp1));
                sprintf(tmp1, "cf4:z");
                columnNames.push_back( string(tmp1));
            }
            else
            {
                sprintf(tmp1, "cf2:x%d",iter_1);
                columnNames.push_back( string(tmp1));
                sprintf(tmp1, "cf3:y%d",iter_1);
                columnNames.push_back( string(tmp1));
                sprintf(tmp1, "cf4:z%d",iter_1);
                columnNames.push_back( string(tmp1));
            }
            
            
            boost::shared_ptr<TTransport> socket(new TSocket(host.c_str(), 9090));
            boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
            boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
            HbaseClient client(protocol);
            vector<string> thrift_list_table;
            thrift_list_table.push_back(string(path));
            try
            {
                transport->open();
                
                int scannerID = client.scannerOpenWithStop(*(thrift_list_table.begin()), pati_min, pati_max, columnNames);
                vector<TRowResult> rowsResult;
                
                
                client.scannerGetList(rowsResult, scannerID, max_1 - min_1);
                
                
                /* printRow(rowsResult);
                 cout << "taille hbase :"<<rowsResult.size()<<endl;*/
                client.scannerClose(scannerID);
                
                transport->close();
            }
            catch (TException &tx)
            {
                cout << "Error : " << tx.what()<<endl;
                
                
            }
            
            
        }
        else if(WR == 4)
        {
            
            char tmp1[32];
            char buf[32];
            int i= 0;
            for(; i < nb; i = i + 1000)
            {
                
                sprintf(buf, "%0.15d", i);
                std::string pati_min(buf);
                
                sprintf(buf, "%0.15d", i+1000);
                std::string pati_max(buf);
                
                StrVec columnNames;
                if(iter_1 == nbIt - 1)
                {
                    sprintf(tmp1, "cf2:x");
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf3:y");
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf4:z");
                    columnNames.push_back( string(tmp1));
                }
                else
                {
                    sprintf(tmp1, "cf2:x%d",iter_1);
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf3:y%d",iter_1);
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf4:z%d",iter_1);
                    columnNames.push_back( string(tmp1));
                }
                
                
                boost::shared_ptr<TTransport> socket(new TSocket(host.c_str(), 9090));
                boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
                boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
                HbaseClient client(protocol);
                vector<string> thrift_list_table;
                thrift_list_table.push_back(string(path));
                try
                {
                    transport->open();
                    
                    int scannerID = client.scannerOpenWithStop(*(thrift_list_table.begin()), pati_min, pati_max, columnNames);
                    vector<TRowResult> rowsResult;
                    
                    
                    client.scannerGetList(rowsResult, scannerID, max_1 - min_1);
                    
                    
                    /* printRow(rowsResult);
                     cout << "taille hbase :"<<rowsResult.size()<<endl;*/
                    client.scannerClose(scannerID);
                    
                    transport->close();
                }
                catch (TException &tx)
                {
                    cout << "Error : " << tx.what()<<endl;
                    
                    
                }
            }
            {
                
                sprintf(buf, "%0.15d", i);
                std::string pati_min(buf);
                
                StrVec columnNames;
                if(iter_1 == nbIt - 1)
                {
                    sprintf(tmp1, "cf2:x");
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf3:y");
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf4:z");
                    columnNames.push_back( string(tmp1));
                }
                else
                {
                    sprintf(tmp1, "cf2:x%d",iter_1);
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf3:y%d",iter_1);
                    columnNames.push_back( string(tmp1));
                    sprintf(tmp1, "cf4:z%d",iter_1);
                    columnNames.push_back( string(tmp1));
                }
                
                
                boost::shared_ptr<TTransport> socket(new TSocket(host.c_str(), 9090));
                boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
                boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
                HbaseClient client(protocol);
                vector<string> thrift_list_table;
                thrift_list_table.push_back(string(path));
                try
                {
                    transport->open();
                    
                    int scannerID = client.scannerOpen(*(thrift_list_table.begin()), pati_min, columnNames);
                    vector<TRowResult> rowsResult;
                    
                    
                    client.scannerGetList(rowsResult, scannerID, max_1 - min_1);
                    
                    
                    /* printRow(rowsResult);
                     cout << "taille hbase :"<<rowsResult.size()<<endl;*/
                    client.scannerClose(scannerID);
                    
                    transport->close();
                }
                catch (TException &tx)
                {
                    cout << "Error : " << tx.what()<<endl;
                    
                    
                }
            }
            
        }
        
    }
    cout<<  my_gettimeofday() - init << ";";
    
}

void thrfit_hive(const char path[],int WR)
{
    
    double init = my_gettimeofday();
    if(WR == 1)
    {
        char err_buf[MAX_HIVE_ERR_MSG_LEN];
        HiveResultSet* resultset;
        HiveReturn retval;
        HiveConnection* connection = DBOpenConnection(", host.c_str(), atoi(DEFAULT_PORT),
                                                      atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
        assert(connection != NULL);
        char req[1024];
        sprintf(req, "LOAD DATA INPATH '/res/res-%d-%d.csv' OVERWRITE INTO TABLE temp;", nb, nb_test);
        //cout<<req <<endl;
        retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBExecute failed: %s\n",
                                       err_buf, connection, 0);
        sprintf(req, "insert overwrite table %s "
                "SELECT "
                "regexp_extract(col_value, '^(?:([^;]*)\;?){1}', 1) key, "
                "regexp_extract(col_value, '^(?:([^;]*)\;?){2}', 1) ts, "
                "regexp_extract(col_value, '^(?:([^;]*)\;?){3}', 1) x, "
                "regexp_extract(col_value, '^(?:([^;]*)\;?){4}', 1) y, "
                "regexp_extract(col_value, '^(?:([^;]*)\;?){5}', 1) z "
                "from temp; "
                ,path);
        //cout<<req <<endl;
        retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBExecute failed: %s\n",
                                       err_buf, connection, 0);
        
        
        retval = DBCloseResultSet(resultset, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBCloseResultSet failed: %s\n",
                                       err_buf, connection, 0);
        
        retval = DBCloseConnection(connection, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBDisconnect failed: %s\n",
                                       err_buf, connection, 0);
        
        
    }
    else if(WR == 2)
    {
        //   fprintf(stderr, "Running %s...\n", __FUNCTION__);
        char err_buf[MAX_HIVE_ERR_MSG_LEN];
        HiveResultSet* resultset;
        HiveReturn retval;
        HiveConnection* connection = DBOpenConnection(DEFAULT_DATABASE, host.c_str(), atoi(DEFAULT_PORT),
                                                      atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
        /* If this fails, make sure that Hive server is running with the connect parameter arguments */
        assert(connection != NULL);
        char req[1024];
        if(iter_1 == nbIt - 1)
        {
            sprintf(req, "select * from %s where key> %d AND key < %d;", path, min_1, max_1);
        }
        else
        
        {
            sprintf(req, "CREATE EXTERNAL TABLE %s%d(key int, ts INT, x DOUBLE, y DOUBLE, z DOUBLE) STORED BY 'org.apache.hadoop.hive.hbase.HBaseStorageHandler' WITH SERDEPROPERTIES (\"hbase.columns.mapping\" = \":key,cf1:ts,cf2:x1,cf3:y1,cf4:z1\") TBLPROPERTIES (\"hbase.table.name\" = \"%s\");",path, iter_1,path);
            //cout<<req <<endl;
            retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
            RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                           "DBExecute failed: %s\n",
                                           err_buf, connection, 0);
            
            sprintf(req, "select * from %s%d where key> %d AND key < %d;", path, iter_1, min_1, max_1);
        }
        //cout<<req <<endl;
        retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBExecute failed: %s\n",
                                       err_buf, connection, 0);
        
        /*char field[MAX_FIELD_LEN];
         size_t data_byte_size;
         int is_null_value;
         retval = DBFetch(resultset, err_buf, sizeof(err_buf));
         size_t col_count;
         cout<<endl;
         
         while(retval == HIVE_SUCCESS)
         {
         retval = DBGetColumnCount(resultset, &col_count, err_buf, sizeof(err_buf));
         for( int i = 0; i <  col_count; i++)
         {
         retval = DBGetFieldAsCString(resultset, i, field, sizeof(field), &data_byte_size, &is_null_value,
         err_buf, sizeof(err_buf));
         cout << field<<" ";
         }
         cout<<endl;
         retval = DBFetch(resultset, err_buf, sizeof(err_buf));
         }
         if(iter_1 == nbIt - 1)
         {
         sprintf(req, "drop table %s%d ;",path, min_1, max_1);
         retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
         RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
         "DBExecute failed: %s\n",
         err_buf, connection, 0);
         
         }*/
        
        retval = DBCloseResultSet(resultset, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBCloseResultSet failed: %s\n",
                                       err_buf, connection, 0);
        
        retval = DBCloseConnection(connection, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBDisconnect failed: %s\n",
                                       err_buf, connection, 0);
        
        
    }
    else if(WR == 4)
    {
        //   fprintf(stderr, "Running %s...\n", __FUNCTION__);
        char err_buf[MAX_HIVE_ERR_MSG_LEN];
        HiveResultSet* resultset;
        HiveReturn retval;
        HiveConnection* connection = DBOpenConnection(DEFAULT_DATABASE, host.c_str(), atoi(DEFAULT_PORT),
                                                      atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
        /* If this fails, make sure that Hive server is running with the connect parameter arguments */
        assert(connection != NULL);
        char req[1024];
        if(iter_2 == nbIt - 1)
        {
            sprintf(req, "select * from %s;", path);
        }
        else
        
        {
            sprintf(req, "CREATE EXTERNAL TABLE %s%d(key int, ts INT, x DOUBLE, y DOUBLE, z DOUBLE) STORED BY 'org.apache.hadoop.hive.hbase.HBaseStorageHandler' WITH SERDEPROPERTIES (\"hbase.columns.mapping\" = \":key,cf1:ts,cf2:x1,cf3:y1,cf4:z1\") TBLPROPERTIES (\"hbase.table.name\" = \"%s\");",path, iter_2, path);
            cout<<req<<endl;
            retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
            RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                           "DBExecute failed: %s\n",
                                           err_buf, connection, 0);
            
            sprintf(req, "select * from %s%d;", path, iter_2);
        }
        //cout<<req <<endl;
        retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBExecute failed: %s\n",
                                       err_buf, connection, 0);
        
        /* char field[MAX_FIELD_LEN];
         size_t data_byte_size;
         int is_null_value;
         retval = DBFetch(resultset, err_buf, sizeof(err_buf));
         size_t col_count;
         cout<<endl;
         
         while(retval == HIVE_SUCCESS)
         {
         retval = DBGetColumnCount(resultset, &col_count, err_buf, sizeof(err_buf));
         for( int i = 0; i <  col_count; i++)
         {
         retval = DBGetFieldAsCString(resultset, i, field, sizeof(field), &data_byte_size, &is_null_value,
         err_buf, sizeof(err_buf));
         cout << field<<" ";
         }
         cout<<endl;
         retval = DBFetch(resultset, err_buf, sizeof(err_buf));
         }*/
        if(iter_2 == nbIt - 1)
        {
            sprintf(req, "drop table %s%d ;", path, iter_2);
            retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
            RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                           "DBExecute failed: %s\n",
                                           err_buf, connection, 0);
            
        }
        
        retval = DBCloseResultSet(resultset, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBCloseResultSet failed: %s\n",
                                       err_buf, connection, 0);
        
        retval = DBCloseConnection(connection, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBDisconnect failed: %s\n",
                                       err_buf, connection, 0);
        
        
        
    }
    cout<<  my_gettimeofday() - init << ";";
}
void thrfit_hive2(const char path[],int WR)
{
    
    double init = my_gettimeofday();
    if(WR == 1)
    {
        char err_buf[MAX_HIVE_ERR_MSG_LEN];
        HiveResultSet* resultset;
        HiveReturn retval;
        HiveConnection* connection = DBOpenConnection(DEFAULT_DATABASE, host.c_str(), atoi(DEFAULT_PORT),
                                                      atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
        assert(connection != NULL);
        char req[1024];
        sprintf(req, "LOAD DATA INPATH '/res/res-%d-%d.csv' OVERWRITE INTO TABLE %s;", nb, nb_test,path);
        
        retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBExecute failed: %s\n",
                                       err_buf, connection, 0);
        
        
        retval = DBCloseResultSet(resultset, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBCloseResultSet failed: %s\n",
                                       err_buf, connection, 0);
        
        retval = DBCloseConnection(connection, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBDisconnect failed: %s\n",
                                       err_buf, connection, 0);
        
        
    }
    else if(WR == 2)
    {
        //   fprintf(stderr, "Running %s...\n", __FUNCTION__);
        char err_buf[MAX_HIVE_ERR_MSG_LEN];
        HiveResultSet* resultset;
        HiveReturn retval;
        HiveConnection* connection = DBOpenConnection(DEFAULT_DATABASE, host.c_str(), atoi(DEFAULT_PORT),
                                                      atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
        /* If this fails, make sure that Hive server is running with the connect parameter arguments */
        assert(connection != NULL);
        char req[1024];
        if(iter_1 == nbIt - 1)
        {
            sprintf(req, "select * from %s where key> %d AND key < %d;", path, min_1, max_1);
        }
        else
        
        {
            sprintf(req, "CREATE EXTERNAL TABLE %s%d(key int, ts INT, x DOUBLE, y DOUBLE, z DOUBLE) STORED BY 'org.apache.hadoop.hive.hbase.HBaseStorageHandler' WITH SERDEPROPERTIES (\"hbase.columns.mapping\" = \":key,cf1:ts,cf2:x1,cf3:y1,cf4:z1\") TBLPROPERTIES (\"hbase.table.name\" = \"%s\");",path, iter_1,path);
            retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
            RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                           "DBExecute failed: %s\n",
                                           err_buf, connection, 0);
            
            sprintf(req, "select * from %s%d where key> %d AND key < %d;", path, iter_1, min_1, max_1);
        }
        retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBExecute failed: %s\n",
                                       err_buf, connection, 0);
        
        /*char field[MAX_FIELD_LEN];
         size_t data_byte_size;
         int is_null_value;
         retval = DBFetch(resultset, err_buf, sizeof(err_buf));
         size_t col_count;
         cout<<endl;
         
         while(retval == HIVE_SUCCESS)
         {
         retval = DBGetColumnCount(resultset, &col_count, err_buf, sizeof(err_buf));
         for( int i = 0; i <  col_count; i++)
         {
         retval = DBGetFieldAsCString(resultset, i, field, sizeof(field), &data_byte_size, &is_null_value,
         err_buf, sizeof(err_buf));
         cout << field<<" ";
         }
         cout<<endl;
         retval = DBFetch(resultset, err_buf, sizeof(err_buf));
         }
         if(iter_1 == nbIt - 1)
         {
         sprintf(req, "drop table %s%d ;",path, min_1, max_1);
         retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
         RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
         "DBExecute failed: %s\n",
         err_buf, connection, 0);
         
         }*/
        
        retval = DBCloseResultSet(resultset, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBCloseResultSet failed: %s\n",
                                       err_buf, connection, 0);
        
        retval = DBCloseConnection(connection, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBDisconnect failed: %s\n",
                                       err_buf, connection, 0);
        
        
    }
    else if(WR == 4)
    {
        //   fprintf(stderr, "Running %s...\n", __FUNCTION__);
        char err_buf[MAX_HIVE_ERR_MSG_LEN];
        HiveResultSet* resultset;
        HiveReturn retval;
        HiveConnection* connection = DBOpenConnection(DEFAULT_DATABASE, host.c_str(), atoi(DEFAULT_PORT),
                                                      atoi(DEFAULT_FRAMED), err_buf, sizeof(err_buf));
        /* If this fails, make sure that Hive server is running with the connect parameter arguments */
        assert(connection != NULL);
        char req[1024];
        if(iter_2 == nbIt - 1)
        {
            sprintf(req, "select * from %s;", path);
        }
        else
        
        {
            sprintf(req, "CREATE EXTERNAL TABLE %s%d(key int, ts INT, x DOUBLE, y DOUBLE, z DOUBLE) STORED BY 'org.apache.hadoop.hive.hbase.HBaseStorageHandler' WITH SERDEPROPERTIES (\"hbase.columns.mapping\" = \":key,cf1:ts,cf2:x1,cf3:y1,cf4:z1\") TBLPROPERTIES (\"hbase.table.name\" = \"%s\");",path, iter_2, path);
            cout<<req<<endl;
            retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
            RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                           "DBExecute failed: %s\n",
                                           err_buf, connection, 0);
            
            sprintf(req, "select * from %s%d;", path, iter_2);
        }
        
        retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBExecute failed: %s\n",
                                       err_buf, connection, 0);
        
        /* char field[MAX_FIELD_LEN];
         size_t data_byte_size;
         int is_null_value;
         retval = DBFetch(resultset, err_buf, sizeof(err_buf));
         size_t col_count;
         cout<<endl;
         
         while(retval == HIVE_SUCCESS)
         {
         retval = DBGetColumnCount(resultset, &col_count, err_buf, sizeof(err_buf));
         for( int i = 0; i <  col_count; i++)
         {
         retval = DBGetFieldAsCString(resultset, i, field, sizeof(field), &data_byte_size, &is_null_value,
         err_buf, sizeof(err_buf));
         cout << field<<" ";
         }
         cout<<endl;
         retval = DBFetch(resultset, err_buf, sizeof(err_buf));
         }*/
        if(iter_2 == nbIt - 1)
        {
            sprintf(req, "drop table %s%d ;", path, iter_2);
            retval = DBExecute(connection,req, &resultset, 10, err_buf, sizeof(err_buf));
            RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                           "DBExecute failed: %s\n",
                                           err_buf, connection, 0);
            
        }
        
        retval = DBCloseResultSet(resultset, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBCloseResultSet failed: %s\n",
                                       err_buf, connection, 0);
        
        retval = DBCloseConnection(connection, err_buf, sizeof(err_buf));
        RETURN_ON_ASSERT_ONE_ARG_CLOSE(retval == HIVE_ERROR,
                                       "DBDisconnect failed: %s\n",
                                       err_buf, connection, 0);
        
        
        
    }
    cout<<  my_gettimeofday() - init << ";";
}

void gen()
{
    double init = my_gettimeofday();
    particulesArray = new particule[nbIt*nb];
    int k=0;
    for (int i = 0; i < nbIt; i++)
    {
        for(int j = 0; j < nb; j++)
        {
            particulesArray[k].it = i;
            particulesArray[k].id = j;
            particulesArray[k].x = ((double) rand() / (RAND_MAX));
            particulesArray[k].y = ((double) rand() / (RAND_MAX));
            particulesArray[k].z = ((double) rand() / (RAND_MAX));
            k++;
        }
    }
    cout<< "Generation: "<<my_gettimeofday()  - init << ";"<<endl;
}



void help()
{
    cout<<endl;
    for(int i = 0; i < sizeof(long_options)/sizeof(*long_options); i++)
    {
        cout<< long_options[i].help<<endl;
    }
    cout<<endl;
    exit(1);
}

int main(int argc, const char * argv[])
{
    
    srand(time(NULL));
    if(argc == 1)
    {
        help();
    }
    else
    {
        int i = 1;
        min_1 = 0;
        max_1 = 1;
        iter_1 = 0;
        
        iter_2 = 0;
        while(i <argc)
        {
            if(argv[i][0] == '-')
            {
                string temp(argv[i]);
                temp.erase (0,1);
                bool find = false;
                int j = 0;
                while(!find && j < sizeof(long_options)/sizeof(*long_options))
                {
                    if ( temp.compare(long_options[j].name)== 0)
                    find = true;
                    else
                    j++;
                }
                if(!find)
                {
                    cout<<"invalid parameter: "<<endl<<"\t\t"<<argv[i]<<endl;
                    help();
                    
                }
                else
                {
                    switch (j) {
                        case 0:
                            if(i + 1 + long_options[j].nb_args > argc)
                            help();
                            host = string(argv[i+1]);
                            break;
                        case 1:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            nb_test = atoi(argv[i+1]);
                            break;
                        case 2:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            nb = atoi(argv[i+1]);
                            break;
                        case 3:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            nbIt = atoi(argv[i+1]);
                            break;
                            
                        case 4:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            vectLocal.push_back(string(argv[i+1]));
                            break;
                        case 5:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            vectNFS.push_back(string(argv[i+1]));
                            break;
                        case 6:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            vectODBC.push_back(string(argv[i+1]));
                            break;
                        case 7:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            vectHive.push_back(string(argv[i+1]));
                            break;
                        case 8:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            vectHbase.push_back(string(argv[i+1]));
                            break;
                        case 9:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            measure = atoi(argv[i+1]);
                            break;
                        case 10:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            iter_1 = atoi(argv[i+1]);
                            break;
                        case 11:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            min_1 = atoi(argv[i+1]);
                            break;
                        case 12:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            max_1 = atoi(argv[i+1]);
                            break;
                        case 13:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            iter_2 = atoi(argv[i+1]);
                            break;
                            
                        case 14:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            vectHbase2.push_back(string(argv[i+1]));
                            break;
                        case 15:
                            if(i +  1 + long_options[j].nb_args > argc)
                            help();
                            vectHive2.push_back(string(argv[i+1]));
                            break;
                        default:
                            help();
                            break;
                    }
                    i = i + 1 + long_options[j].nb_args;
                    if(i > argc)
                    help();
                }
                
            }
            else
            {
                cout<<"invalid parameter: "<<endl<<"\t\t"<<argv[i]<<endl;
                help();
            }
        }
        
        
        gen();
        
        int type = 1;
        while (type != 8)
        {
            if ((measure&type) > 0)
            {
                cout<<type<< " ; ";
                if(vectLocal.size() > 0)
                {
                    for(int  j = 0; j < vectLocal.size(); j++)
                    cout<<"local ; ";
                }
                if(vectNFS.size() > 0)
                {
                    for(int  j = 0; j < vectNFS.size(); j++)
                    cout<<"NFS ; ";
                }
                
                if(vectODBC.size() > 0)
                {
                    for(int  j = 0; j < vectODBC.size(); j++)
                    cout<<"ODBC ; ";
                }
                
                if(vectHive.size() > 0)
                {
                    for(int  j = 0; j < vectHive.size(); j++)
                    cout<<"Hive ; ";
                }
                if(vectHive2.size() > 0)
                {
                    for(int  j = 0; j < vectHive2.size(); j++)
                    cout<<"Hive2 ; ";
                }
                
                if(vectHbase.size() > 0)
                {
                    for(int  j = 0; j < vectHbase.size(); j++)
                    cout<<"Hbase ; ";
                }
                
                if(vectHbase2.size() > 0)
                {
                    for(int  j = 0; j < vectHbase2.size(); j++)
                    cout<<"Hbase2 ; ";
                }
                
                
                cout<<endl;
                
                // for (int k = 0; k < nb_test; k++)
                {
                    cout<<" "<< nb_test <<" ; ";
                    for(int  j = 0; j < vectLocal.size(); j++)
                    {
                        local(vectLocal.at(j).c_str() , type);
                    }
                    for(int j = 0; j < vectNFS.size(); j++)
                    {
                        local(vectNFS.at(j).c_str() , type);
                    }
                    for(int j = 0; j < vectODBC.size(); j++)
                    {
                        odbc(vectODBC.at(j).c_str() , type);
                    }
                    for(int j = 0; j < vectHive.size(); j++)
                    {
                        thrfit_hive(vectHive.at(j).c_str() , type);
                    }
                    for(int j = 0; j < vectHive2.size(); j++)
                    {
                        thrfit_hive2(vectHive2.at(j).c_str() , type);
                    }
                    for(int j = 0; j < vectHbase.size(); j++)
                    {
                        thrfit_hbase(vectHbase.at(j).c_str() , type);
                    }
                    for(int j = 0; j < vectHbase2.size(); j++)
                    {
                        thrfit_hbase2(vectHbase2.at(j).c_str() , type);
                    }
                    cout<<endl;
                }
            }
            type = type *2;
            cout<<endl;
            
        }
        
    }
    return 0;
    
}

