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

}


void GetListOfResultsFromTimeStepAndAnalysis(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID, char *analysisName, double timeStep)
{
   int endTime, startTime;
   std::string result, errMsg;
   rvGetListOfResults rv;

   printf("\n--->GetListOfResultsFromTimeStepAndAnalysis - \"%s\"\n", modelName);
   startTime = GetTickCount();
   server->getListOfResultsFromTimeStepAndAnalysis(rv, sessionID, modelID, analysisName, timeStep);
   endTime = GetTickCount();
   printf("Elapsed time for GetBoundaryOfLocalMesh is %d milliseconds\n", endTime - startTime);

}
int64_t vertArr[] = {
278669, 287934, 289862, 290555, 293416, 293917, 296381, 297980, 298436, 299575, 300054, 304354, 307537, 308340, 308996, 309043, 311063, 311155, 311974, 313065, 313720, 314029, 314299, 314676, 315213, 315342, 316580, 317700, 318053, 319623, 320101, 320173, 320570, 321241, 321673, 322082, 323207, 323258, 323325, 324739, 324904, 324931, 325033, 325547, 326009, 326419, 327501, 327753, 327837, 328284, 328750, 329455, 330193, 330701, 330779, 331309, 331978, 331982, 332692, 332836, 333079, 334482, 334494, 334628, 334725, 335455, 335848, 336223, 337303, 337359, 338017, 338902, 339058, 339085, 339312, 339433, 339760, 340450, 340697, 341081, 341666, 341825, 342188, 342307, 342523, 343115, 343140, 343281, 343888, 343971, 344235, 344969, 344985, 345005, 345137, 345524, 345704, 345803, 345851, 346461, 346560, 346591, 346901, 347291, 347406, 347708, 348075, 348379, 348805, 349103, 349164, 349667, 350214, 350278, 350335, 350380, 350453, 350559, 350720, 350860, 350895, 351174, 351326, 351443, 352159, 352275, 352584, 352637, 353081, 
353257, 353317, 353453, 353521, 353541, 353579, 353673, 353887, 353899, 354071, 354317, 354542, 354677, 354845, 354940, 355010, 355080, 355335, 355850, 355928, 356178, 356223, 356318, 356580, 356885, 86824, 87050, 87121, 90701, 91248, 91822, 92352, 92750, 92757, 93655, 93717, 93767, 94146, 94509, 95436, 95458, 95502, 95620, 96643, 97115, 97515, 97945, 98461, 98521, 98741, 99481, 99629, 99725, 99783, 99789, 100097, 100300, 100379, 100498, 100517, 100694, 101466, 101508, 101831, 101871, 101931, 101934, 101967, 101991, 102093, 102424, 103086, 103857, 104056, 104318, 104586, 104592, 104597, 104623, 104726, 104790, 104995, 105124, 105157, 105382, 105436, 105541, 105867, 105947, 105965, 106215, 106221, 106428, 106907, 107044, 107370, 107486, 107912, 108017, 108204, 108214, 108279, 108584, 108633, 108912, 108922, 108941, 108944, 108955, 108961, 108979, 109037, 109108, 109155, 109251, 109341, 109422, 109643, 109695, 109700, 109928, 110282, 110292, 110456, 110548, 110569, 
110792, 110886, 110897, 110998, 111162, 111177, 111342, 111448, 111567, 111773, 111836, 111981, 112261, 112547, 112590, 113010, 113122, 113142, 113199, 113262, 113271, 113328, 113389, 113465, 113513, 113683, 113718, 113880, 114001, 114015, 114031, 114109, 114279, 114406, 114416, 114494, 114881, 114895, 115041, 115182, 115191, 115250, 115296, 115553, 115617, 115625, };
void GetResultFromVerticesID(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID)
{
   int endTime, startTime;
   rvGetResultFromVerticesID verticesResultRV;
   vector<int64_t> listOfVertices;
   int timeStep = 21;

   //for (int i=0; i < (sizeof(vertArr) / sizeof(int64_t)); i++) {
   //   listOfVertices.push_back(vertArr[i]);
   //}
   server->getResultFromVerticesID(verticesResultRV, sessionID, modelID, "Kratos", timeStep, "PARTITION INDEX", listOfVertices);
   for (vector<VELaSSCoSM::ResultOnVertex>::iterator resIter = verticesResultRV.result_list.begin(); resIter != verticesResultRV.result_list.end(); resIter++) {
      vector<double>::iterator valuesIter = resIter->value.begin();
      printf("%12Lf\n", *valuesIter);
   }

   listOfVertices.clear();

   listOfVertices.push_back(278669);
   listOfVertices.push_back(2814146);
   listOfVertices.push_back(287934);

//   printf("\n--->GetResultFromVerticesID - \"%s\"\n", modelName);
//   startTime = GetTickCount();
//   server->getResultFromVerticesID(verticesResultRV, sessionID, modelID, "Kratos", 61, "VELOCITY", listOfVertices);
//   endTime = GetTickCount();
//   printf("Elapsed time for GetResultFromVerticesID is %d milliseconds\n", endTime - startTime);
//   printf("Return status: %s\n", verticesResultRV.status.data());
//   if (strncmp(verticesResultRV.status.data(), "Error", 5) == 0) {
//      printf("Error message: \"%s\"\n", verticesResultRV.report.data());
//   } else {
//      int nRes = 0;
////#define BLOB 1
//#ifdef BLOB
//      printf("%s has %d results.\n", modelName, verticesResultRV.n_result_records);
//      char *cp = (char*)verticesResultRV.result_array.data();
//      for (int i = 0; i < 16 && i < verticesResultRV.n_result_records; i++) {
//         EDMVD::ResultOnVertex *vertexResult = (EDMVD::ResultOnVertex*)cp;
//         printf("%10llu %Lf\n", vertexResult->id, vertexResult->value[0]);
//         cp += verticesResultRV.result_record_size;
//      }
//#else
//      for (vector<VELaSSCoSM::ResultOnVertex>::iterator resIter = verticesResultRV.result_list.begin(); resIter != verticesResultRV.result_list.end(); resIter++) {
//         vector<double>::iterator valuesIter = resIter->value.begin();
//         if (nRes++ > 16) break;
//         printf("%10llu %Lf\n", resIter->id, *valuesIter);
//      }
//#endif
//      printf(verticesResultRV.report.data());
//   }
   for (int i=1; i < 20; i++) {
      server->getResultFromVerticesID(verticesResultRV, sessionID, modelID, "Kratos", timeStep, "PRESSURE", listOfVertices);
      printf("\n%10d %10d", i, timeStep);
      timeStep += 20;
      for (vector<VELaSSCoSM::ResultOnVertex>::iterator resIter = verticesResultRV.result_list.begin(); resIter != verticesResultRV.result_list.end(); resIter++) {
         vector<double>::iterator valuesIter = resIter->value.begin();
         printf("%12Lf", *valuesIter);
      }
   }
   //printf("\n\nVELOCITY\n");
   //timeStep = 21;
   //for (int i=1; i < 6; i++) {
   //   server->getResultFromVerticesID(verticesResultRV, sessionID, modelID, "Kratos", timeStep, "VELOCITY", listOfVertices);
   //   printf("\n%10d %10d", i, timeStep);
   //   timeStep += 20;
   //   for (vector<VELaSSCoSM::ResultOnVertex>::iterator resIter = verticesResultRV.result_list.begin(); resIter != verticesResultRV.result_list.end(); resIter++) {
   //      vector<double>::iterator valuesIter = resIter->value.begin();
   //      printf("%12Lf", *valuesIter);
   //   }
   //}
}



void GetListOfMeshes(VELaSSCoHandler *server, string sessionID, char *modelName, string modelID, string analysisID, double timeStep)
{
   int endTime, startTime;
   rvGetListOfMeshes rvMeshes;

   printf("\n--->GetListOfMeshes - \"%s\"\n", modelName);
   startTime = GetTickCount();
   server->getListOfMeshes(rvMeshes, sessionID, modelID, analysisID, timeStep);
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

      FILE *logFile = fopen("c:/temp/EDMqueryManager.log", "w");
      CLoggWriter    ourLogger(logFile, true, true);
      ourVELaSSCoHandler->defineLogger(&ourLogger);
      ourVELaSSCoHandler->defineCluster(&ourCluster);
      ourVELaSSCoHandler->printLogHeader();

      VELaSSCoMethods findAllModels(&ourCluster, &ourLogger);


      if (argc >= 7) {
         // Temporarly solution for server file injection
         ourVELaSSCoHandler->InjectData(argv[nextCommandPos]);
         printf("\n\nEnter a character to stop the program.\n");
         getchar();

      } else {

         string sessionID;
         string modelID;
         rvOpenModel rvOM;
         //char *modelName = "telescope";
         char *modelName = "Barcelona_4m";
         char *analysisName = "Kratos";
         double timeStep = 21;
         //modelName = "VELaSSCo_HbaseBasicTest";
         //analysisName = "geometry";
         //timeStep = 10;


         printf("\n--->UserLogin\n");
         ourVELaSSCoHandler->userLogin(sessionID, "olav", "myRole", "myPassword");
         //printf("User logged on with session ID: %s\n", sessionID.data());

         //printf("\n--->OpenModel - \"telescope\"\n");
         ourVELaSSCoHandler->openModel(rvOM, sessionID, modelName, "read");
         //printf("Returned modelID: %s\n", rvOM.modelID.data());
         //printf("Comments: %s\n", rvOM.report.data());
         modelID = rvOM.modelID;

         GetResultFromVerticesID(ourVELaSSCoHandler, sessionID, modelName, modelID);

         GetListOfMeshes(ourVELaSSCoHandler, sessionID, modelName, modelID, analysisName, timeStep);

         GetListOfResultsFromTimeStepAndAnalysis(ourVELaSSCoHandler, sessionID, modelName, modelID, analysisName, timeStep);
      
         GetListOfAnalyses(ourVELaSSCoHandler, sessionID, modelName, modelID);

         GetBoundaryOfLocalMesh(ourVELaSSCoHandler, sessionID, modelName, modelID, analysisName, timeStep);

         GetCoordinatesAndElementsFromMesh(ourVELaSSCoHandler, sessionID, modelName, modelID);

         CalculateBoundingBox(ourVELaSSCoHandler, sessionID, modelName, modelID);

         //ourCluster.stopAllEDMservers();

         //UseCase_FEM_M1_02(ourVELaSSCoHandler, sessionID);
      }
      fclose(logFile);

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

	return 0;
}

