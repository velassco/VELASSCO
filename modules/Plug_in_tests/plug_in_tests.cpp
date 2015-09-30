// EDM_plug_in_tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "random_points.h"
#include "elementID_of_random_points.h"


StorageModuleClient *clp = NULL;

std::string getResultOnVertices(std::string sessionID,
   std::string modelID,
   std::string analysisID,
   double      timeStep,
   std::string resultID,
   std::string listOfVertices)
{

   rvOpenModel rvOM;
   string modelID_of_VELaSSCo_HbaseBasicTest_part_1;
   dli::rvGetResultFromVerticesID_B results;
   vector<int64_t> vertexIDs;

   clp->OpenModel(rvOM, sessionID, "VELaSSCo_HbaseBasicTest_part_1", "read");
   modelID_of_VELaSSCo_HbaseBasicTest_part_1 = rvOM.modelID;
   clp->GetResultFromVerticesID(results, sessionID, modelID_of_VELaSSCo_HbaseBasicTest_part_1, "", vertexIDs, "Height", 1.0, "geometry");

   string resultString;
   for (vector<VertexResult>::iterator resIter = results.vertexResults.begin(); resIter != results.vertexResults.end(); resIter++) {
      char cb[2048], *np;
      int length = sprintf(cb, "%d ", resIter->vertexID); np = cb + length;
      for (vector<double>::iterator value = resIter->resuls.begin(); value != resIter->resuls.end(); value++) {
         length = sprintf(np, " %f", *value); np = np + length;
      }
      sprintf(np, "\n");
      resultString += cb;
   }
   return resultString;
}


void testListAnalyses(StorageModuleClient &client, string sessionID, char *modelName)
{

   rvOpenModel rvOM;
   string FluidizedbedModelID;
   rvGetListOfAnalyses rvAnalysisList;
   rvGetListOfTimeSteps rvTimesteps;

   printf("\n--->OpenModel - \"%s\"\n", modelName);
   client.OpenModel(rvOM, sessionID, modelName, "read");
   printf("Returned modelID: %s\n", rvOM.modelID.data());
   printf("Comments: %s\n", rvOM.report.data());
   FluidizedbedModelID = rvOM.modelID;

   printf("\n--->GetListOfAnalyses - \"%s\"\n", modelName);
   client.GetListOfAnalyses(rvAnalysisList, sessionID, FluidizedbedModelID);
   printf("Return status: %s\n", rvAnalysisList.status.data());
   printf("%s has %d analyses.\n", modelName, rvAnalysisList.analyses.size());
   for (vector<string>::iterator nameIter = rvAnalysisList.analyses.begin(); nameIter != rvAnalysisList.analyses.end(); nameIter++) {
      printf("Analysis name : %s\n", (char*)nameIter->data());
      client.GetListOfTimeSteps(rvTimesteps, sessionID, FluidizedbedModelID, *nameIter);
      printf("   %s has the following time steps:\n", (char*)nameIter->data());
      int i = 0;
      for (vector<double>::iterator tsIter = rvTimesteps.time_steps.begin(); tsIter != rvTimesteps.time_steps.end(); tsIter++) {
         printf((i++) % 4 == 0 ? "\n   %lf" : " %lf", *tsIter);
      }
   }
}


/*===============================================================================================*/
int main(int argc, char* argv[])
/*
   Program to test VELaSSCo Data Layer thrift based server. The program has 3 command line parameters:
   1. server address
   2. server port number
   3. Command

   Command can eiter be "all" or any of the querynames.
=================================================================================================*/
{
   char *command = "all";
   char *server = "localhost";
   int port = 9090;

   if (argc > 1) server = argv[1];
   if (argc > 2) port = atol(argv[2]);
   if (argc > 3) command = argv[3];

   boost::shared_ptr<TTransport> socket(new TSocket(server, port));
   boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
   boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
   StorageModuleClient client(protocol);
   clp = &client;

#ifdef WIN32
   WSADATA wsaData = {};
   WORD wVersionRequested = MAKEWORD(2, 2);
   int err = WSAStartup(wVersionRequested, &wsaData);
#endif

   try {
      transport->open();
      
      rvGetElementOfPointsInSpace rv;
      string sessionID;
      string modelID_of_VELaSSCo_HbaseBasicTest_part_1;
      vector<Point> points;
      Point p;
      int nPoints = sizeof(random_points) / sizeof(random_points[0]);

      printf("\n--->UserLogin\n");
      client.UserLogin(sessionID, "olav", "myRole", "myPassword");
      printf("User logged on with session ID: %s\n", sessionID.data());
      rvOpenModel rvOM;

      printf("\n--->OpenModel - \"VELaSSCo_HbaseBasicTest_part_1\"\n");
      client.OpenModel(rvOM, sessionID, "VELaSSCo_HbaseBasicTest_part_1", "read");
      printf("Returned modelID: %s\n", rvOM.modelID.data());
      printf("Comments: %s\n", rvOM.report.data());
      modelID_of_VELaSSCo_HbaseBasicTest_part_1 = rvOM.modelID;

      if (strEQL(command, "all") || strEQL(command, "string_getResultOnVertices")) {
         string vertices = getResultOnVertices(sessionID, "0", "analysis", 00.1, "resultID", "LOV");
         printf("getResultOnVertices returns the following string\n");
         printf(vertices.data());
      }

      if (strEQL(command, "all") || strEQL(command, "GetElementOfPointsInSpace")) {

         for (int i = 0; i < nPoints; i++) {
            p.__set_x(random_points[i][0]); p.__set_y(random_points[i][1]); p.__set_z(random_points[i][2]);
            points.push_back(p);
         }
         printf("\n--->GetElementOfPointsInSpace  - 1000 random points:\n");
         client.GetElementOfPointsInSpace(rv, sessionID, rvOM.modelID, points);

         printf("Return status: %s\n", rv.status.data());
         printf("Comments: %s\n", rv.report.data());
         if (strEQL(rv.status.data(), "OK")) {
            int ix = 0;
            int nErrorFound = false;
            for (std::vector<dli::Element>::iterator elemId = rv.elements.begin(); elemId != rv.elements.end(); elemId++) {
               if (elemId->id != elementID_of_random_points[ix]) {
                  printf("illegal elementid for point nr %d", ix); nErrorFound++;
               }
               ix++;
            }
            printf(nErrorFound ? "GetElementOfPointsInSpace executed with %d errors\n" : "No errors in returned data detected.\n", nErrorFound);
         }
      }

      rvGetBoundaryOfLocalMesh rvm;
      if (strEQL(command, "all") || strEQL(command, "GetBoundaryOfLocalMesh_WithError")) {
         printf("\n--->GetBoundaryOfLocalMesh with wrong model id:\n");
         client.GetBoundaryOfLocalMesh(rvm, sessionID, "", "meshID", "analysisID", 1.0);
         printf("Return status: %s\n", rvm.status.data());
         printf("Comments: %s\n", rvm.report.data());
         if (strcmp(rvm.status.data(), "Error") || strcmp(rvm.report.data(), "Model does not exist.")) {
            printf("Unexpected result from GetBoundaryOfLocalMesh on non existing model\n");
         } else {
            printf("Expected error message returned\n");
         }
      }

      if (strEQL(command, "all") || strEQL(command, "GetBoundaryOfLocalMesh")) {
         printf("\n--->GetBoundaryOfLocalMesh:\n");
         client.GetBoundaryOfLocalMesh(rvm, sessionID, modelID_of_VELaSSCo_HbaseBasicTest_part_1, "meshID", "analysisID", 1.0);
         printf("Return status: %s\n", rvm.status.data());
         printf("Comments: %s\n", rvm.report.data());
     }

      if (strEQL(command, "all") || strEQL(command, "GetResultFromVerticesID_FEM")) {
         rvGetResultFromVerticesID_B rvB;
         vector<int64_t> vertexIDs;

         vertexIDs.push_back(37);
         printf("\n--->GetResultFromVerticesID - FEM model VELaSSCo_HbaseBasicTest_part_1:\n");
         client.GetResultFromVerticesID(rvB, sessionID, modelID_of_VELaSSCo_HbaseBasicTest_part_1, "", vertexIDs, "Height", 1.0, "geometry");
         printf("Return status: %s\n", rvB.status.data());
         printf("Comments: %s\n", rvB.report.data());
     }

      if (strEQL(command, "all") || strEQL(command, "GetListOfModels")) {
         rvGetListOfModels modelsInfo;
         rvOpenModel rvOM;
         string DEM_boxModelID;

         printf("\n--->GetListOfModels:\n");
         client.GetListOfModels(modelsInfo, sessionID, "", "", "");
         printf("Return status: %s\n", modelsInfo.status.data());
         printf("Comments: %s\nModels:\n", modelsInfo.report.data());
         for (std::vector<dli::ModelInfo>::iterator modelIter = modelsInfo.models.begin(); modelIter != modelsInfo.models.end(); modelIter++) {
            printf("%s\n", modelIter->name.data());
         }
         printf("\n--->OpenModel - \"DEM_b*\"\n");
         client.OpenModel(rvOM, sessionID, "DEM_b*", "read");
         printf("Returned modelID: %s\n", rvOM.modelID.data());
         printf("Comments: %s\n", rvOM.report.data());
         DEM_boxModelID = rvOM.modelID;

         rvGetResultFromVerticesID_B rvB;
         vector<int64_t> vertexIDs;

         printf("\n--->GetResultFromVerticesID - DEM model DEM_box:\n");
         client.GetResultFromVerticesID(rvB, sessionID, DEM_boxModelID, "", vertexIDs, "Height", 1.0, "geometry");
         printf("Return status: %s\n", rvB.status.data());
         printf("Comments: %s\n", rvB.report.data());
     }

      if (strEQL(command, "all") || strEQL(command, "testListAnalyses")) {
         testListAnalyses(client, sessionID, "VELaSSCo_HbaseBasicTest_part_1");
         testListAnalyses(client, sessionID, "Fluidizedbed");
     }

      transport->close();
   } catch (TException &tx) {
      printf("ERROR: %s\n", tx.what());
   }
   printf("\n\nEnter a character to stop the program.\n");
   getchar();
   return 0;
}

