// edm_qm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


//using namespace  ::dli;

#include "EDM_interface.h"
#include "CLogger.h"
#include "VELaSSCo_Operations.h"
#include "VELaSSCoHandler.h"
#include "VELaSSCoMethods.h"
#include <omp.h>

void GetBoundaryOfLocalMesh(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID, char *analysisName, double timeStep)
{
   int endTime, startTime;
   std::string result, errMsg;

   printf("\n--->GetBoundaryOfLocalMesh - \"%s\"\n", modelName);
   startTime = GetTickCount();
   server->calculateBoundaryOfAMesh(sessionID, modelID, 0, "Tetrahedra", analysisName, timeStep, &result, &errMsg);
   endTime = GetTickCount();
   printf("Elapsed time for GetBoundaryOfLocalMesh is %d milliseconds\n", endTime - startTime);


   //if (strncmp(boundaryRV.status.data(), "Error", 5) == 0) {
   //   printf("Error message: \"%s\"\n", boundaryRV.report.data());
   //} else {
   //   printf("Query report:\n%s\n\n\n", boundaryRV.report.data());
   //   printf("Mesh boundary of %s has %d triangles.\n", modelName, boundaryRV.elements.size());
   //   int nn = 0;
   //   for (vector<VELaSSCoSM::Triangle>::iterator bufferIter = boundaryRV.elements.begin(); bufferIter != boundaryRV.elements.end() && nn++ < 20; bufferIter++) {
   //      for (vector<VELaSSCoSM::NodeID>::iterator nodeIter = bufferIter->nodes.begin(); nodeIter != bufferIter->nodes.end(); nodeIter++) {
   //         printf("%10llu", *nodeIter);
   //      }
   //      printf("\n");
   //   }
   //}
}

void GetResultFromVerticesID(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID)
{
   int endTime, startTime;
   rvGetResultFromVerticesID verticesResultRV;
   vector<int64_t> listOfVertices;

//   listOfVertices.push_back(63327);
   listOfVertices.push_back(2814146);
//   listOfVertices.push_back(123400);

   printf("\n--->GetResultFromVerticesID - \"%s\"\n", modelName);
   startTime = GetTickCount();
   server->getResultFromVerticesID(verticesResultRV, sessionID, modelID, "Kratos", 61, "VELOCITY", listOfVertices);
   endTime = GetTickCount();
   printf("Elapsed time for GetResultFromVerticesID is %d milliseconds\n", endTime - startTime);
   printf("Return status: %s\n", verticesResultRV.status.data());
   if (strncmp(verticesResultRV.status.data(), "Error", 5) == 0) {
      printf("Error message: \"%s\"\n", verticesResultRV.report.data());
   } else {
      int nRes = 0;
//#define BLOB 1
#ifdef BLOB
      printf("%s has %d results.\n", modelName, verticesResultRV.n_result_records);
      char *cp = (char*)verticesResultRV.result_array.data();
      for (int i = 0; i < 16 && i < verticesResultRV.n_result_records; i++) {
         EDMVD::ResultOnVertex *vertexResult = (EDMVD::ResultOnVertex*)cp;
         printf("%10llu %Lf\n", vertexResult->id, vertexResult->value[0]);
         cp += verticesResultRV.result_record_size;
      }
#else
      for (vector<VELaSSCoSM::ResultOnVertex>::iterator resIter = verticesResultRV.result_list.begin(); resIter != verticesResultRV.result_list.end(); resIter++) {
         vector<double>::iterator valuesIter = resIter->value.begin();
         if (nRes++ > 16) break;
         printf("%10llu %Lf\n", resIter->id, *valuesIter);
      }
#endif
      printf(verticesResultRV.report.data());
   }
   int timeStep = 21;
   for (int i=1; i < 6; i++) {
      server->getResultFromVerticesID(verticesResultRV, sessionID, modelID, "Kratos", timeStep, "PRESSURE", listOfVertices);
      printf("\n%10d %10d", i, timeStep);
      timeStep += 20;
      for (vector<VELaSSCoSM::ResultOnVertex>::iterator resIter = verticesResultRV.result_list.begin(); resIter != verticesResultRV.result_list.end(); resIter++) {
         vector<double>::iterator valuesIter = resIter->value.begin();
         printf("%12Lf", *valuesIter);
      }
   }
   printf("\n\nVELOCITY\n");
   timeStep = 21;
   for (int i=1; i < 6; i++) {
      server->getResultFromVerticesID(verticesResultRV, sessionID, modelID, "Kratos", timeStep, "VELOCITY", listOfVertices);
      printf("\n%10d %10d", i, timeStep);
      timeStep += 20;
      for (vector<VELaSSCoSM::ResultOnVertex>::iterator resIter = verticesResultRV.result_list.begin(); resIter != verticesResultRV.result_list.end(); resIter++) {
         vector<double>::iterator valuesIter = resIter->value.begin();
         printf("%12Lf", *valuesIter);
      }
   }
}



void GetListOfMeshes(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID)
{
   int endTime, startTime;
   rvGetListOfMeshes rvMeshes;

   printf("\n--->GetListOfMeshes - \"%s\"\n", modelName);
   startTime = GetTickCount();
   server->getListOfMeshes(rvMeshes, sessionID, modelID, "Kratos", 21.0);
   endTime = GetTickCount();
   printf("Elapsed time for GetListOfMeshes is %d milliseconds\n", endTime - startTime);
   printf("Return status: %s\n", rvMeshes.status.data());
   printf("%s has %d meshes.\n", modelName, rvMeshes.meshInfos.size());
   for (vector<VELaSSCoSM::MeshInfo>::iterator meshIter = rvMeshes.meshInfos.begin(); meshIter != rvMeshes.meshInfos.end(); meshIter++) {
      printf("   %20s  %10llu  %10llu\n", meshIter->name.data(), meshIter->nElements, meshIter->nVertices);
   }
}
void GetListOfResultsFromTimeStepAndAnalysis(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID, string analysisID, double timeStep)
{
   int endTime, startTime;
   rvGetListOfResults resultRV;
   printf("\n--->GetListOfResultsFromTimeStepAndAnalysis - \"%s\"\n", modelName);
   startTime = GetTickCount();
   server->getListOfResultsFromTimeStepAndAnalysis(resultRV, sessionID, modelID, analysisID, timeStep);
   endTime = GetTickCount();
   printf("Elapsed time for GetListOfResultsFromTimeStepAndAnalysis is %d milliseconds\n", endTime - startTime);
   printf("Return status: %s\n", resultRV.status.data());
   printf("%s has %d result headers.\n", modelName, resultRV.result_list.size());
   int nRes = 0;
   if (strncmp(resultRV.status.data(), "Error", 5) == 0) {
      printf("Error message: \"%s\"\n", resultRV.report.data());
   } else {
      for (vector<VELaSSCoSM::ResultInfo>::iterator resIter = resultRV.result_list.begin(); resIter != resultRV.result_list.end(); resIter++) {
         if (nRes++ > 16) break;
         printf("%20s %15s %15s %10d\n", (char*)resIter->name.data(), (char*)resIter->type.data(), (char*)resIter->location.data(), resIter->numberOfComponents);
         for (vector<string>::iterator compName = resIter->componentNames.begin(); compName != resIter->componentNames.end(); compName++) {
            printf("                                     %s\n", compName->data());
         }
      }
   }
}


void UseCase_FEM_M1_02(VELaSSCoHandler *server, string sessionID)
{
   string modelID;
   rvOpenModel rvOM;
   char *modelName = "telescope_copy";

   server->openModel(rvOM, sessionID, modelName, "read");
   modelID = rvOM.modelID;
   
   rvGetResultFromVerticesID rv;
   std::vector<int64_t> listOfVertices;
   listOfVertices.push_back(2814146);
   server->getResultFromVerticesID(rv, sessionID, modelID, "Kratos", 61.0, "PRESSURE", listOfVertices);
}

void CalculateBoundingBox(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID)
{
   int64_t listOfVertices[1];
   string dataTableName;
   double steps[3];
   double return_bbox[6];
   string return_error_str;

   listOfVertices[0]= 2814146;
   steps[0] = 21.0; steps[1] = 41.0; steps[2] = 61.0;
   server->calculateBoundingBox(sessionID, modelID, "Kratos", 3, steps, 0, listOfVertices, return_bbox, &return_error_str);
   for (int i=0; i < 6; i++) {
      printf("%12Lf\n", return_bbox[i]);
   }
}

//
//calculateBoundingBox(const std::string &sessionID, const std::string &modelID, const e,
//   const std::string &analysisID, const int numSteps, const double *lstSteps,
//   const int64_t numVertexIDs, const int64_t *lstVertexIDs,
//   double *return_bbox, std::string *return_error_str)
//{
//

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


void GetCoordinatesAndElementsFromMesh(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID)
{
   rvGetCoordinatesAndElementsFromMesh rv;
   MeshInfo meshInfo;

   server->getCoordinatesAndElementsFromMesh(rv, sessionID, modelID, "Kratos", 21, meshInfo);

}

/*=================================================================================================
9000 E:\VELaSSCo\installation\database\EDMcluster VELaSSCo v "E:\VELaSSCo\installation\scripts\VELaSSCo_cluster_0_127.txt"

9000 "O:\projects\VELaSSCo\SVN_src\EDM_plug_in\db_template" VELaSSCo v "O:\projects\VELaSSCo\SVN_src\EDM_plug_in\scripts\VELaSSCo_cluster_1_0_9.txt"
*/




void report_num_threads(int level)
{
   //#pragma omp single
   //{
      printf("Level %d: number of threads in the team - %d, %d\n", level, omp_get_num_threads(), omp_get_thread_num());
   //}
}


int main(int argc, char* argv[])
{
   int rstat;
   char errTxt[1024];

    //omp_set_nested(1);

    //omp_set_dynamic(1);
    //omp_set_num_threads(128);

    //#pragma omp parallel num_threads(2)
    //{
    //    report_num_threads(1);

    //    #pragma omp parallel num_threads(6)
    //    {
    //        report_num_threads(2);
    //        #pragma omp parallel num_threads(4)
    //        {
    //            report_num_threads(3);
    //            int nThreads = omp_get_num_threads();
    //            nThreads = 0;
    //        }
    //    }
    //}
    ////#pragma omp parallel num_threads(2)
    //#pragma omp parallel for
    //for (int i=0; i < 12; i++)
    //{
    //    report_num_threads(1);

    //    //#pragma omp parallel num_threads(6)
    //    #pragma omp parallel for
    //    for (int j=0; j < 12; j++)
    //    {
    //        report_num_threads(2);
    //        #pragma omp parallel num_threads(4)
    //        {
    //            report_num_threads(3);
    //            int nThreads = omp_get_num_threads();
    //            nThreads = 0;
    //        }
    //    }
    //}

    //int nThreads = omp_get_num_threads();


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
      //CLoggWriter    ourLogger(logFile, true, true);
      CLoggWriter    ourLogger(logFile, false, false);
      ourVELaSSCoHandler->defineLogger(&ourLogger);

      if (argc >= 7) {
         // Temporarly solution for server file injection
         ourVELaSSCoHandler->InjectData(argv[nextCommandPos]);
         printf("\n\nEnter a character to stop the program.\n");
         getchar();

      } else {

         string sessionID;
         string modelID;
         rvOpenModel rvOM;
         char *modelName = "telescope";
         char *analysisName = "Kratos";
         double timeStep = 21;
         modelName = "VELaSSCo_HbaseBasicTest";
         analysisName = "geometry";
         timeStep = 10;


         printf("\n--->UserLogin\n");
         ourVELaSSCoHandler->userLogin(sessionID, "olav", "myRole", "myPassword");
         //printf("User logged on with session ID: %s\n", sessionID.data());

         //printf("\n--->OpenModel - \"telescope\"\n");
         ourVELaSSCoHandler->openModel(rvOM, sessionID, modelName, "read");
         //printf("Returned modelID: %s\n", rvOM.modelID.data());
         //printf("Comments: %s\n", rvOM.report.data());
         modelID = rvOM.modelID;
      
         GetListOfAnalyses(ourVELaSSCoHandler, sessionID, modelName, modelID);

         GetBoundaryOfLocalMesh(ourVELaSSCoHandler, sessionID, modelName, modelID, analysisName, timeStep);

         GetCoordinatesAndElementsFromMesh(ourVELaSSCoHandler, sessionID, modelName, modelID);

         CalculateBoundingBox(ourVELaSSCoHandler, sessionID, modelName, modelID);

         GetListOfMeshes(ourVELaSSCoHandler, sessionID, modelName, modelID);

         GetResultFromVerticesID(ourVELaSSCoHandler, sessionID, modelName, modelID);

         ourCluster.stopAllEDMservers();

         UseCase_FEM_M1_02(ourVELaSSCoHandler, sessionID);
      }

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

