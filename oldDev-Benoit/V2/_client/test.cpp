#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <sys/time.h>
#include <string>
#include <poll.h>

#include <vector>
using namespace std;
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
    {"help", 0, "[-help]"}
};


//Parameters
string host = "127.0.0.1";
int nb_test = 1;
int nb = 50;
int nbIt = 1;
std::vector<string> vectLocal;
std::vector<string> vectNFS;
std::vector<string> vectODBC;
std::vector<string> vectHive;
std::vector<string> vectHbase;
int measure = 7;
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
                            
                        default:
                            help();
                            break;
                    }
                    i = i + 1 + long_options[j].nb_args;
                    if(i > argc)
                        help();
                }
                if(vectLocal.size() > 0)
                    cout<<"local ; ";
                if(vectNFS.size() > 0)
                    cout<<"NFS ; ";
                if(vectODBC.size() > 0)
                    cout<<"ODBC ; ";
                if(vectHive.size() > 0)
                    cout<<"Hive ; ";
                if(vectHbase.size() > 0)
                    cout<<"Hbase ; ";
                cout<<endl;
                
                if ((measure&1) > 0)
                {
                    for (int k = 0; k < nb_test; k++)
                    {
                        for( j = 0; j < vectLocal.size(); j++)
                        {
                            cout<<vectLocal.at(j)<<endl;
                        }
                        for( j = 0; j < vectNFS.size(); j++)
                        {
                            
                        }
                        for( j = 0; j < vectODBC.size(); j++)
                        {
                            
                        }
                        for( j = 0; j < vectHive.size(); j++)
                        {
                            
                        }
                        for( j = 0; j < vectHbase.size(); j++)
                        {
                            
                        }
                    }
                    if ((measure&2) > 0)
                    {
                        for (int k = 0; k < nb_test; k++)
                        {
                            for( j = 0; j < vectLocal.size(); j++)
                            {
                                cout<<vectLocal.at(j)<<endl;
                            }
                            for( j = 0; j < vectNFS.size(); j++)
                            {
                                
                            }
                            for( j = 0; j < vectODBC.size(); j++)
                            {
                                
                            }
                            for( j = 0; j < vectHive.size(); j++)
                            {
                                
                            }
                            for( j = 0; j < vectHbase.size(); j++)
                            {
                                
                            }
                        }
                    }
                    if ((measure&4) > 0)
                    {
                        for (int k = 0; k < nb_test; k++)
                        {
                            for( j = 0; j < vectLocal.size(); j++)
                            {
                                cout<<vectLocal.at(j)<<endl;
                            }
                            for( j = 0; j < vectNFS.size(); j++)
                            {
                                
                            }
                            for( j = 0; j < vectODBC.size(); j++)
                            {
                                
                            }
                            for( j = 0; j < vectHive.size(); j++)
                            {
                                
                            }
                            for( j = 0; j < vectHbase.size(); j++)
                            {
                                
                            }
                        }
                    }
                }
                
            }
            else
            {
                cout<<"invalid parameter: "<<endl<<"\t\t"<<argv[i]<<endl;
                help();
            }
        }
    }
    return 0;
}
