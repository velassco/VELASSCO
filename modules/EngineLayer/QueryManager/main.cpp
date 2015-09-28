#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


#include <iostream>
#include <string>
#include <sstream>

#include <boost/thread.hpp>


#include "queryManagerModule.h"
#include "Server.h"

using namespace std;

void parse(string cmd)
{
    if( cmd.find("query") == 0)
    {
        cout<<"#### Query ####"<<endl;
        stringstream listOfVertices;
        
        listOfVertices <<"{\"id\":[";
        bool firstEleme = false;
        for (int i = 0; i < 100; i++)
        {
            if(rand()%2)
            {
                if(firstEleme)
                {
                    listOfVertices << ",";
                }
                else
                {
                    firstEleme = true;
                }
                listOfVertices <<i ;
            }
        }
        
        listOfVertices <<"]}";
        
        string _return = "";
        string sessionID = "sessionID";
        string modelID  = "0001";
        string analysisID  = "DEM";
        double timeStep = 1;
        string resultID = "00001";
        queryManagerModule::Instance()->getResultFormVerticesID(_return ,sessionID ,modelID ,analysisID ,timeStep ,resultID ,listOfVertices.str());
        cout<<"#### /Query ####"<<endl;
        
    }
    else if( cmd.find("ping")  == 0)
    {
        cout<<"#### Ping ####"<<endl;
        queryManagerModule::Instance()->getStatus();
        cout<<"#### /Ping ####"<<endl;
    }
    else if( cmd.find("stop")  == 0)
    {
        cout<<"#### stop ####"<<endl;
        queryManagerModule::Instance()->stopAll();
        cout<<"#### /stop ####"<<endl;
    }
}

void printListOfCmd()
{
    cout << "List of avaiblable cmd :" << endl;
    cout << "exit (or quit): stop the current application (enginelayer)" << endl;
    cout << "dl_exit (or dl_quit or dlq or dle): stop the dataLayer application" << endl;
    cout <<" ################################################" << endl << endl;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    
        int counter = 0;
    pid_t pid = fork();
    
    if (pid == 0)
    {
        // child process
    }
    else if (pid > 0)
    {
        queryManagerModule::Instance()->startConnection();

	// boost::thread serverThread(StartServer, 9090);
	boost::thread serverThread(StartServer, 26267);

        string cmd ="";
        do
        {
            printListOfCmd();
            cin >> cmd;
            parse(cmd);
        }
        while (cmd.find("exit") != 0 && cmd.find("quit")  != 0);
        queryManagerModule::Instance()->stopConnection();
    }
    else
    {
        // fork failed
        printf("fork() failed!\n");
        return 1;
    }
    
    
    return 0;
}

