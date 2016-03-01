#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


#include <iostream>
#include <string>
#include <sstream>

#include <boost/thread.hpp>


#include "DataLayerAccess.h"
#include "Server.h"
#include "Helpers.h"

using namespace std;

void parse(string cmd)
{
    if( cmd.find("query") == 0)
    {
        cout<<"#### Query ####"<<endl;
        stringstream listOfVertices;
        
        listOfVertices <<"{\"id\":[";
        bool firstEleme = false;
        for (int i = 0; i < 10; i++)
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
        string modelID  = "0001"; // does not matter it's hard coded in the DataLayer
        string analysisID  = "DEM";
        double timeStep = 1;
	timeStep = 2799000;

	// string model_name = "VELaSSCo_Models_V4CIMNE:/home/jsperez/Sources/CIMNE/VELASSCO-Data/VELaSSCo_HbaseBasicTest_FEM:VELaSSCo_HbaseBasicTest-part-";
        // string analysisID  = "geometry";

        string resultID = "00001";
	// rvOpenModel om_return;
	// DataLayerAccess::Instance()->openModel( om_return, sessionID, model_name, "rw");
	// const string &modelID = om_return.modelID;
    //    DataLayerAccess::Instance()->getResultFromVerticesID( _return ,sessionID ,modelID ,analysisID ,timeStep ,resultID ,listOfVertices.str());
	// DataLayerAccess::Instance()->closeModel();
        cout<<"#### /Query ####"<<endl;
        
    }
    else if( cmd.find("ping")  == 0)
    {
        cout<<"#### Ping ####"<<endl;
	std::string status;
        DataLayerAccess::Instance()->getStatusDB( status);
	cout<<"### " << status << endl;
        cout<<"#### /Ping ####"<<endl;
    }
    else if( cmd.find("list")  == 0)
    {
        cout<<"#### List ####"<<endl;
	std::string status;
        rvGetListOfModels _return_;
	DataLayerAccess::Instance()->getListOfModels( _return_,
						      "", "", ""); // dl_sessionID, group_qualifier, name_pattern);
	cout<<"### " << _return_ << endl;
        cout<<"#### /List ####"<<endl;
    }
    else if( cmd.find("stop")  == 0)
    {
        cout<<"#### stop ####"<<endl;
        DataLayerAccess::Instance()->stopAll();
        cout<<"#### /stop ####"<<endl;
	exit( 0);
    }
}

void printListOfCmd()
{
    cout << "List of available commands :" << endl;
    cout << "stop: stop the dataLayer application" << endl;
    cout << "ping: get Status of DB" << endl;
    cout << "list: list models in DataLayer" << endl;
    cout << "query: does a query" << endl;
    cout << "exit (or quit): stop the current application (enginelayer)" << endl;
    cout <<" ################################################" << endl << endl;
}

bool askForHelp( const char *txt) {
  return !strcasecmp( txt, "-h") || !strcasecmp( txt, "--h") || !strcasecmp( txt, "-help") || !strcasecmp( txt, "--help");
}

bool thereIsHelpSwitch( int argc, char **argv) {
  // check help switch
  bool ret = false;
  for ( int ia = 1; ia < argc; ia++) {
    if ( askForHelp( argv[ ia])) {
      ret = true;
    }
  }
  return ret;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    
    int listen_port = 26267; // standard thrift port : 9090
    const char *data_layer_hostname = "localhost"; // or "pez001";
    int         data_layer_port     = 26266;
    if ( thereIsHelpSwitch( argc, argv)) {
      printf( "Usage: %s [ options] \n", argv[ 0]);
      printf( "  -port port_number         listening port for this Engine Layer server (default %d)\n", listen_port);
      printf( "  -dl_host hostname         host name of the Data Layer Server (default %s)\n", data_layer_hostname);
      printf( "  -dl_port port_number      port of the Data Layer Server (default %d)\n", data_layer_port);
      return EXIT_FAILURE;
    }

    int processed_args = 1; // first is the program name itself
    for ( int ia = 1; ia + 1 < argc; ia++) {
      if ( !strcasecmp( argv[ ia], "-port")) {
	ia++;
	int new_port = listen_port;
	if ( sscanf( argv[ ia], "%d", &new_port) == 1) {
	  listen_port = new_port;
	  processed_args += 2;
	}
      } else if ( !strcasecmp( argv[ ia], "-dl_host")) {
	ia++;
	data_layer_hostname = argv[ ia];
	processed_args += 2;
      } else if ( !strcasecmp( argv[ ia], "-dl_port")) {
	ia++;
	int new_port = data_layer_port;
	if ( sscanf( argv[ ia], "%d", &new_port) == 1) {
	  data_layer_port = new_port;
	  processed_args += 2;
	}
      } else {
	break;
      }
    }

    if ( processed_args != argc) {
      printf( "Error processing options.\n");
      return EXIT_FAILURE;
    }

    // int counter = 0;
    pid_t pid = fork();
    
    if (pid == 0)
    {
        // child process
    }
    else if (pid > 0)
    {
      DataLayerAccess::Instance()->startConnection( data_layer_hostname, data_layer_port);

	DEBUG( "listening on port" << listen_port);
	boost::thread serverThread(StartServer, listen_port);

        string cmd ="";
        do
        {
            printListOfCmd();
            cin >> cmd;
            parse(cmd);
        }
        while (cmd.find("exit") != 0 && cmd.find("quit")  != 0);
        DataLayerAccess::Instance()->stopConnection();
    }
    else
    {
        // fork failed
        printf("fork() failed!\n");
        return 1;
    }
    
    
    return 0;
}

