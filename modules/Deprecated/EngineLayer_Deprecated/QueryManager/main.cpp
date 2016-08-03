#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


#include <iostream>
#include <string>
#include <sstream>

#include <boost/thread.hpp>


#include "DataLayerAccess.h"
#include "EdmAccess.h"
#include "Server.h"
#include "Helpers.h"


using namespace std;

int QMiD = 0;
VELaSSCoSMClient *clp = NULL;
	
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


        string _return;
        string sessionID;
        string modelID; // does not matter it's hard coded in the DataLayer
        double timeStep;

	 string model_name;
        string analysisID;
        string resultID;

		if (QMiD == 26267)
		{
			cout<<"# Launching getResultFromVerticesID in timeSetp 381" <<endl;
		    _return = "";
		    sessionID = "-1564890139288268170";
		    modelID  = "e863004ceac4190cea39528f81a58fcf"; // does not matter it's hard coded in the DataLayer
		    timeStep = 381;
		 	model_name = "VELaSSCo_Models:/localfs/home/velassco/common/simulation_files/Fem_small_examples/Telescope_128subdomains_ascii/:fine_mesh";
		    analysisID  = "Kratos";
		    resultID = "PRESSURE";
		 }
		 else
		 {
		 	cout<<"# Launching getResultFromVerticesID in timeSetp 361" <<endl;
		    _return = "";
		    sessionID = "-1564890139288268171";
		    modelID  = "e863004ceac4190cea39528f81a58fcf"; // does not matter it's hard coded in the DataLayer
		    timeStep = 361;
		 	model_name = "VELaSSCo_Models:/localfs/home/velassco/common/simulation_files/Fem_small_examples/Telescope_128subdomains_ascii/:fine_mesh";
		    analysisID  = "Kratos";
		    resultID = "PRESSURE";     
		 }   
        
        
        
		rvOpenModel om_return;
		DataLayerAccess::Instance()->openModel( om_return, sessionID, model_name, "rw");
		 //const string &modelID = om_return.modelID;
		    //DataLayerAccess::Instance()->getResultFromVerticesID( _return ,sessionID ,modelID ,analysisID ,timeStep ,resultID ,listOfVertices.str());
		    rvGetResultFromVerticesID _return_;
		    
		int size = 100000;      
		    std::vector<int64_t> listOfVertices2(size);
		for(int i=0; i<size; ++i){
			listOfVertices2[i] = i;
		}        
        
		DataLayerAccess::Instance()->getResultFromVerticesID( _return_, sessionID, modelID, analysisID, timeStep, resultID, listOfVertices2);     
 
		cout<<" Print results:\n"<<endl;
		int nRes = 0;
		for (vector<VELaSSCoSM::ResultOnVertex>::iterator resIter = _return_.result_list.begin(); resIter != _return_.result_list.end(); resIter++) {
			vector<double>::iterator valuesIter = resIter->value.begin();
			if (nRes++ > 16) break;
			printf("%10ld %0.3f\n", resIter->id, *valuesIter);
		}
	 //DataLayerAccess::Instance()->closeModel();
        cout<<"#### /Query Finished ####\n"<<endl;
        
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
    cout << " stop: stop the dataLayer application" << endl;
    cout << " ping: get Status of DB" << endl;
    cout << " list: list models in DataLayer" << endl;
    cout << " query: does a query" << endl;
    cout << " exit (or quit): stop the current application (enginelayer)" << endl;
    cout << "################################################" << endl << endl;
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
    int         connect_EDM			= 0;
    if ( thereIsHelpSwitch( argc, argv)) {
      printf( "Usage: %s [ options] \n", argv[ 0]);
      printf( "  -port port_number         listening port for this Engine Layer server (default %d)\n", listen_port);
      printf( "  -dl_host hostname         host name of the Data Layer Server (default %s)\n", data_layer_hostname);
      printf( "  -dl_port port_number      port of the Data Layer Server (default %d)\n", data_layer_port);
      printf( "  -dl_EDM 1                 connects to the EDM SM (default %d - OpenVersion)\n", connect_EDM);
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
      } else if ( !strcasecmp( argv[ ia], "-dl_EDM")) {
		ia++;
		int new_port = data_layer_port;
		if ( sscanf( argv[ ia], "%d", &new_port) == 1) {
		  connect_EDM = new_port;
		  processed_args += 2;
		}
      }else {
		break;
      }
    }

    if ( processed_args != argc) {
      printf( "Error processing options.\n");
      return EXIT_FAILURE;
    }

	//FIXME: AUTHOR IVAN CORES: I think this fork can be deleted ...
    // int counter = 0;
//    pid_t pid = fork();
    
//    if (pid == 0)
//    {
        // child process
//    }
//    else if (pid > 0)
//    {

	if (connect_EDM == 0)
	{
		//Open Source behaviour.
		DataLayerAccess::Instance()->startConnection( data_layer_hostname, data_layer_port);

		QMiD = listen_port;
		DEBUG( "listening on port " << listen_port);
		boost::thread serverThread(StartServer, listen_port);

		cout << endl;
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
	else //connect_EDM==1
	{
		//EDM connection...
		
		//const char *edm_server = "10.0.1.134";
		//int edm_port = 26266;
		DEBUG("EDM connection information in ACUARIO: pez010 VM IP: 10.0.1.134, port 26266");	
		
		EdmAccess::Instance()->startConnection( data_layer_hostname, data_layer_port);
		
		
//		//This is the code that SHOULD be here in the definitive version.
//			DEBUG( "listening on port " << listen_port);
//			boost::thread serverThread(StartServer, listen_port);
//
//			cout << endl;
//			string cmd ="";
//			do
//			{
//				printListOfCmd();
//				cin >> cmd;
//				parse(cmd);
//			}
//			while (cmd.find("exit") != 0 && cmd.find("quit")  != 0);
		
		
		
		
//		//This is only for testing purposes !!
//		//This is only for testing purposes !!
//		//This is only for testing purposes !!
//		//This is only for testing purposes !!		
			std::string sessionID;
		EdmAccess::Instance()->userLogin(sessionID, "name", "rol", "pass"); 
		
			rvOpenModel rvOM;
		EdmAccess::Instance()->openModel(rvOM, sessionID, "telescope", "read");
		
			rvGetResultFromVerticesID verticesResultRV;
			vector<int64_t> listOfVertices;
			listOfVertices.push_back(63327);
			listOfVertices.push_back(63699);
			listOfVertices.push_back(63707);
			listOfVertices.push_back(64285);
			listOfVertices.push_back(123400);

		EdmAccess::Instance()->getResultFromVerticesID(verticesResultRV, sessionID, rvOM.modelID, "Kratos", 21.0, "PRESSURE", listOfVertices);

			rvGetListOfMeshes rvMeshes;
		EdmAccess::Instance()->getListOfMeshes( rvMeshes, sessionID, rvOM.modelID, "Kratos", 21.0);		       
				       
			rvGetListOfAnalyses rvAnalysisList;		       
		EdmAccess::Instance()->getListOfAnalyses( rvAnalysisList,sessionID, rvOM.modelID);
			
			rvGetListOfResults resultRV;		 
		EdmAccess::Instance()->getListOfResultsFromTimeStepAndAnalysis( resultRV, sessionID, rvOM.modelID, "Kratos", 21.0);		    
		
/*			MeshInfo meshInfo;
			rvGetCoordinatesAndElementsFromMesh meshInfoRV;
		EdmAccess::Instance()->getCoordinatesAndElementsFromMesh(meshInfoRV, sessionID, rvOM.modelID, "Kratos", 21.0, meshInfo );
*/
	   
//		// END: This is only for testing purposes !!
//		// END: This is only for testing purposes !!
//		// END: This is only for testing purposes !!
//		// END: This is only for testing purposes !!	
		
		
		
		EdmAccess::Instance()->stopConnection();
	
	} //end else


//    }
//    else
//    {
//        // fork failed
//        printf("fork() failed!\n");
//        return 1;
//    }
    
    
    return 0;
}

