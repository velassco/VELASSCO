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

#include "VELaSSCo_Operations.h"

#ifdef EDM

extern "C" {
#include "sdai.h"
#include "cpp10_EDM_interface.h"
}

#include "EDMcluster_entityTypes.h"
#include "container.h"
#include "EDMcluster.hpp"

using namespace std;
using namespace ecl;

#include "CppParameterClass.h"
#include "EDMclusterServices.h"

#include "EDM_interface.h"
#include "CLogger.h"
#include "VELaSSCoHandler.h"
#include "VELaSSCoMethods.h"

#endif /* EDM */


using namespace std;

int QMiD = 0;
VELaSSCoSMClient *clp = NULL;

int G_multiUser = 0; // used in queryManager/Server.cpp to start Simple or MultiUser server
int getMultiUserSetting() { return G_multiUser;}
// to get the proper location of the Analytics jar's
char *G_VELaSSCo_base_dir = NULL;
const char *getVELaSSCoBaseDir() { return G_VELaSSCo_base_dir;}
	
void parse(string cmd)
{
#ifndef EDM
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
#endif /* ifndef EDM */
}

void setVELaSSCoBaseDir( const char *argv0) {
  if ( argv0 && argv0[ 0]) {
    const size_t full_path_max_size = 10240;
    char *full_path = ( char *)malloc( full_path_max_size * sizeof( char));
    *full_path = '\0';
    if ( argv0[ 0] != '/') {
      // not an absolute pathname
      if ( getcwd( full_path, full_path_max_size)) {
      } else {
	// coudln't get current working directory ...
	*full_path = '\0';
      }
      strcat( full_path, "/");
    }
    strcat( full_path, argv0);
    // remove exe_name
    size_t end = strlen( full_path) - 1;
    while ( end && full_path[ end] && ( full_path[ end] != '/')) {
      end--;
    }
    full_path[ end] = '\0';
    // now gettng the base_dir
    // executable should be in ..../bin/QueryManager.exe
    // so getting rid of the /bin part
    if ( ( end > 4) && !strcasecmp( &full_path[ end - 4], "/bin")) {
      end -= 4;
      full_path[ end] = '\0';
    }
    if ( strlen( full_path)) {
      G_VELaSSCo_base_dir = full_path;
      printf( "Base path = %s\n", full_path);
    } else {
      free( full_path);
    }
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


VELaSSCo_Operations *queryServer = NULL;


int main(int argc, char **argv)
{
    srand(time(NULL));
    
    int listen_port = 26267; // standard thrift port : 9090
    const char *hbase_hostname = "localhost"; // or "pez001";
    int         hbase_port     = 9090; // hbase thrift server
    char *EDM_qm_database_folder = "";
    char *EDM_qm_init_file = "";
    char *EDM_inject_file = "";
    bool EDM_execute = false;
    bool EDM_inject = false;
    int         connect_EDM	    = 0;
    if ( thereIsHelpSwitch( argc, argv)) {
      printf( "Usage: %s [ options] \n", argv[ 0]);
      printf( "  -port port_number         listening port for this Engine Layer server (default %d)\n", listen_port);
      printf( "  -hbase_host hostname         host name of the HBase Thrift Server (default %s)\n", hbase_hostname);
      printf( "  -hbase_port port_number      port of the HBase Thrift Server (default %d)\n", hbase_port);
      printf( "  -dl_EDM                   EDM_qm_database_folder EDM_qm_init_file  \n");
      printf( "  -dl_EDM_inject            EDM_qm_database_folder EDM_inject_file  \n");
      printf( "  -multiuser 0              if 0, uses SimpleServer, if 1, uses MultiUser (default %d)\n", G_multiUser);
      return EXIT_FAILURE;
    }

    // to get the proper location of the Analytics jar's
    // jar files are relative to Engine.exe location, i.e.
    // Platform/bin/QueryManager.exe
    // Platform/AnalyticsYarnJobs/GetBoundaryOfAMesh_pez001.jar
    setVELaSSCoBaseDir( argv[ 0]);

    int processed_args = 1; // first is the program name itself
    for ( int ia = 1; ia + 1 < argc; ia++) {
      if ( !strcasecmp( argv[ ia], "-port")) {
		ia++;
		int new_port = listen_port;
		if ( sscanf( argv[ ia], "%d", &new_port) == 1) {
		  listen_port = new_port;
		  processed_args += 2;
		}
      } else if ( !strcasecmp( argv[ ia], "-hbase_host")) {
		   ia++;
		   hbase_hostname = argv[ ia];
		   processed_args += 2;
      } else if ( !strcasecmp( argv[ ia], "-hbase_port")) {
		   ia++;
		   int new_port = hbase_port;
		   if ( sscanf( argv[ ia], "%d", &new_port) == 1) {
		     hbase_port = new_port;
		     processed_args += 2;
		   }
      } else if ( !strcasecmp( argv[ ia], "-dl_EDM")) {
		   EDM_qm_database_folder = argv[ ++ia];
		   EDM_qm_init_file = argv[ ++ia];
		   processed_args += 3; EDM_execute = true;
      } else if ( !strcasecmp( argv[ ia], "-dl_EDM_inject")) {
		   EDM_qm_database_folder = argv[ ++ia];
		   EDM_qm_init_file = argv[ ++ia];
		   EDM_inject_file = argv[ ++ia];
		   processed_args += 4; EDM_inject = true;
      } else if ( !strcasecmp( argv[ ia], "-multiuser")) {
		ia++;
		int is_multi = G_multiUser;
		if ( sscanf( argv[ ia], "%d", &is_multi) == 1) {
		  G_multiUser = is_multi;
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


#ifdef EDM
    int rstat;
    char errTxt[1024];
    VELaSSCoHandler *ourVELaSSCoHandler;

    FILE *logFile = fopen("EDMstorageModule.log", "w");
    CLoggWriter    ourLogger(logFile, true, true);

    try {
       ourVELaSSCoHandler = new VELaSSCoHandler();
       queryServer = ourVELaSSCoHandler;
       ourVELaSSCoHandler->defineLogger(&ourLogger);

       Database VELaSSCo_db(EDM_qm_database_folder, "VELaSSCo", "v");
       VELaSSCo_db.open();

       SdaiRepository repositoryId;
       CMemoryAllocator ma(0x100000);
       int rstat = edmiCreateRepository("EDMcluster", &repositoryId);
       if (rstat == OK) {
          rstat = edmiCreateModelBN(repositoryId, "EDMcluster", "EDMcluster", 0);
       }
       Repository clusterRepository(&VELaSSCo_db, "EDMcluster");
       Model clusterModel(&clusterRepository, &ma, &EDMcluster_SchemaObject);
       clusterModel.open("EDMcluster", sdaiRW);
       VELaSSCoCluster ourCluster(&clusterModel);
       if (!EDM_execute && !EDM_inject) {
          ourLogger.logg(0, "\nThe command line must have eiter a -dl_EDM command or a -dl_EDM_inject cpommand.\nExecution stops.\n\n");
       } else if (EDM_execute && EDM_qm_init_file && *EDM_qm_init_file) {
          ourLogger.logg(2, "EDMqueryManager starts with\nEDM_qm_database_folder=%s\nEDM_qm_init_file=%s\n\n, ", EDM_qm_database_folder, EDM_qm_init_file);
          ourCluster.initClusterModel(EDM_qm_init_file);
          ourCluster.startServices();
          ourVELaSSCoHandler->defineCluster(&ourCluster);
          QMiD = listen_port;
          DEBUG("listening on port " << listen_port);
          boost::thread serverThread(StartServer, listen_port);

          cout << endl;
          string cmd = "";
          do {
             printListOfCmd();
             cin >> cmd;
             if (cmd.find("stop") == 0) {
                ourCluster.stopAllEDMservers();
             } else if (cmd.find("list") == 0) {
                ourCluster.listAllEDMservers();
             }
          } while (cmd.find("exit") != 0 && cmd.find("quit") != 0);

       } else if (EDM_inject_file && *EDM_inject_file) {
         
         ourLogger.logg(3, "EDMqueryManager starts injection with\nEDM_qm_database_folder=%s\nEDM_qm_init_file=%s\nEDM_inject_file=%s\n\n, ",
            EDM_qm_database_folder, EDM_qm_init_file, EDM_inject_file);
         ourCluster.initClusterModel(EDM_qm_init_file);
         ourCluster.startServices();
         ourVELaSSCoHandler->defineCluster(&ourCluster);
         ourVELaSSCoHandler->InjectData(EDM_inject_file);

       } else {
          ourLogger.error(0, "EDMqueryManager init file must be specified.\n");
       }

    } catch (CedmError *e) {
       rstat = e->rstat;
       if (e->message) {
          strncpy(errTxt, e->message, sizeof(errTxt));
       } else {
          strncpy(errTxt, edmiGetErrorText(rstat), sizeof(errTxt));
       }
       delete e;
       ourLogger.error(1, "Exception\n\n%s\n\n", errTxt);
    } catch (int thrownRstat) {
       strncpy(errTxt, edmiGetErrorText(thrownRstat), sizeof(errTxt));
       ourLogger.error(1, "Exception\n\n%s\n\n", errTxt);
    }
   printf("\n\nEnter a character to stop the program.\n");
   getchar();

#else /* ndef EDM */

	if (connect_EDM == 0)
	{
		//Open Source behaviour.
		
		// const char *hbase_host = "localhost"; // or pez001
    		// int hbase_port = 9090; // hbase thrift server
    
		DataLayerAccess::Instance()->startConnection( hbase_hostname, hbase_port);
		queryServer =  DataLayerAccess::Instance();
		
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

#endif /* ndef EDM */

   return 0;
}

