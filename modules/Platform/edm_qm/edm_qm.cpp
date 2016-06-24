// edm_qm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


//using namespace  ::dli;

#include "EDM_interface.h"
#include "CLogger.h"
#include "VELaSSCo_VQueries.h"
#include "VELaSSCoHandler.h"
#include "VELaSSCoMethods.h"


void UseCase_FEM_M1_02(VELaSSCoHandler *server, string sessionID)
{
   string modelID;
   rvOpenModel rvOM;
   char *modelName = "telescope";

   server->openModel(rvOM, sessionID, modelName, "read");
   modelID = rvOM.modelID;
   
   rvGetResultFromVerticesID rv;
   std::vector<int64_t> listOfVertices;
   listOfVertices.push_back(2814146);
   server->getResultFromVerticesID(rv, sessionID, modelID, "Kratos", 61.0, "PRESSURE", listOfVertices);
}
void GetListOfAnalyses(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID)
{
   int endTime, startTime;
   rvGetListOfAnalyses rvAnalysisList;
   rvGetListOfTimeSteps rvTimesteps;

   printf("\n--->GetListOfAnalyses - \"%s\"\n", modelName);
   startTime = GetTickCount();
   server->getListOfAnalyses(rvAnalysisList, sessionID, modelID);
   endTime = GetTickCount();
   printf("Elapsed time for GetListOfAnalyses is %d milliseconds\n", endTime - startTime);
   printf("Return status: %s\n", rvAnalysisList.status.data());
   printf("%s has %d analyses.\n", modelName, rvAnalysisList.analyses.size());
   if (strncmp(rvAnalysisList.status.data(), "Error", 5) == 0) {
      printf("Error message: \"%s\"\n", rvAnalysisList.report.data());
   } else {
      printf("Execution report:\n%s\n\n\n", rvAnalysisList.report.data());
      for (vector<string>::iterator nameIter = rvAnalysisList.analyses.begin(); nameIter != rvAnalysisList.analyses.end(); nameIter++) {
         printf("Analysis name : %s\n", (char*)nameIter->data());
         double lsteps = 0.0;
         string stepOptions = "";
         server->getListOfTimeSteps(rvTimesteps, sessionID, modelID, *nameIter, stepOptions, 1, &lsteps);
         printf("   %s has %d time steps:\n", (char*)nameIter->data(), rvTimesteps.time_steps.size());
         printf("   %s has the following time steps:\n", (char*)nameIter->data());
         int i = 0;
         for (vector<double>::iterator tsIter = rvTimesteps.time_steps.begin(); tsIter != rvTimesteps.time_steps.end(); tsIter++) {
            printf("%Lf\n", *tsIter);
         }
      }
   }
}


/*=================================================================================================
9000 E:\VELaSSCo\installation\database\EDMcluster VELaSSCo v "E:\VELaSSCo\installation\scripts\VELaSSCo_cluster_0_127.txt"

9000 "O:\projects\VELaSSCo\SVN_src\EDM_plug_in\db_template" VELaSSCo v "O:\projects\VELaSSCo\SVN_src\EDM_plug_in\scripts\VELaSSCo_cluster_1_0_9.txt"
*/


int main(int argc, char* argv[])
{
   int rstat;
   char errTxt[1024];

   printf("The VELaSSCo EDM Query Manager shall have four command line parameters:\n   1. Communcation port\n   2. Database folder\n   3. Database name\n   4. Database password");
   printf("\nOptional commands can be read from the file specified as the 5. parameter\n\n"); // File name for Cluster database init file");
   if (argc < 5) {
      printf("\nAt least 4 command line parameters are needed.");
      exit(0);
   }
   int port = atol(argv[1]);
   VELaSSCoHandler *ourVELaSSCoHandler = new VELaSSCoHandler();

   try {
      Database VELaSSCo_db(argv[2], argv[3], argv[4]);
      Repository demRepository(&VELaSSCo_db, "DEM_models");
      Repository femRepository(&VELaSSCo_db, "FEM_models");
      VELaSSCo_db.open();
      demRepository.open(sdaiRO);
      femRepository.open(sdaiRO);
      ourVELaSSCoHandler->cDEMrep = &demRepository;
      ourVELaSSCoHandler->cFEMrep = &femRepository;

      int nextCommandPos = 5;

      SdaiRepository repositoryId;
      CMemoryAllocator ma(0x100000);
      EDMLONG rstat = edmiCreateRepository("EDMcluster", &repositoryId);
      if (rstat == OK) {
         rstat = edmiCreateModelBN(repositoryId, "EDMcluster", "EDMcluster", 0);
      }
      Repository clusterRepository(&VELaSSCo_db, "EDMcluster");
      Model clusterModel(&clusterRepository, &ma, &EDMcluster_SchemaObject);
      clusterModel.open("EDMcluster", sdaiRW);
      VELaSSCoCluster ourCluster(&clusterModel);
      if (argc >= 6) {
         ourCluster.initClusterModel(argv[nextCommandPos++]);
      }
      ourCluster.startServices();

      VELaSSCoMethods findAllModels(&ourCluster);

      ourVELaSSCoHandler->defineCluster(&ourCluster);

      FILE *logFile = fopen("EDMstorageModule.log", "w");
      CLoggWriter    ourLogger(logFile, true, true);
      ourVELaSSCoHandler->defineLogger(&ourLogger);


      string sessionID;
      string modelID;
      rvOpenModel rvOM;
      char *modelName = "telescope";

      printf("\n--->UserLogin\n");
      ourVELaSSCoHandler->userLogin(sessionID, "olav", "myRole", "myPassword");
      printf("User logged on with session ID: %s\n", sessionID.data());

      printf("\n--->OpenModel - \"telescope\"\n");
      ourVELaSSCoHandler->openModel(rvOM, sessionID, modelName, "read");
      printf("Returned modelID: %s\n", rvOM.modelID.data());
      printf("Comments: %s\n", rvOM.report.data());
      modelID = rvOM.modelID;

      UseCase_FEM_M1_02(ourVELaSSCoHandler, sessionID);

      GetListOfAnalyses(ourVELaSSCoHandler, sessionID, modelName, modelID);


   } catch (CedmError *e) {
      rstat = e->rstat;
      if (e->message) {
         strncpy(errTxt, e->message, sizeof(errTxt));
      } else {
         strncpy(errTxt, edmiGetErrorText(rstat), sizeof(errTxt));
      }
      delete e;
      printf(errTxt);
   } catch (int thrownRstat) {
      strncpy(errTxt, edmiGetErrorText(thrownRstat), sizeof(errTxt));
      printf(errTxt);
   }

   //rvGetListOfModels namesRv;
   //string sessionID, model_group_qualifier = "", model_name_pattern = "";

   //VELaSSCo_server.GetListOfModelNames(namesRv, sessionID, model_group_qualifier, model_name_pattern);
	return 0;
}

