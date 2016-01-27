
#include "stdafx.h"
#include "VELaSSCoMethods.h"
#include "modules/DataLayer/Storage/EDM_server_dll/EDM_server_dll.h"
#include <omp.h>




/*=============================================================================================================================*/
void VELaSSCoMethods::ListModels()
/*=============================================================================================================================*/
{
   if (serverContexts) {
//      SdaiServerContext *scs = serverContexts->getElementArray();
//      EDMULONG nsc = serverContexts->size();
//#pragma omp parallel for
//      for (int i = 0; i < nsc; i++) {
//         SdaiString        *foundNames;
//         CHECK(edmiRemoteListModels(scs[i], NULL, NULL, NULL, NULL, NULL, 0, &foundNames, NULL));
//         printf("Thread number %llu\n", omp_get_thread_num());
//         for (int i = 0; foundNames[i]; i++) {
//            printf("%s\n", foundNames[i]);
//         }
//         printf("\n\n");
//      }
   }
}

/*=============================================================================================================================*/
void VELaSSCoMethods::ValidateModels()
/*=============================================================================================================================*/
{
   int startTime = GetTickCount();
//   if (serverContexts) {
//      SdaiServerContext *scs = serverContexts->getElementArray();
//      EDMULONG nsc = serverContexts->size();
//#pragma omp parallel for
//      for (int i = 0; i < nsc; i++) {
//         SdaiInteger        warnings, errors;
//         char diaFileName[2048];
//         sprintf(diaFileName, "O:\\projects\\VELaSSCo\\SVN_src\\EDM_plug_in\\db_cluster\\validate_%d.txt", i);
//         CHECK(edmiRemoteValidateModel(scs[i], "FEM_models", "VELaSSCo_HbaseBasicTest_part_1", diaFileName, 0, NULL, FULL_VALIDATION,
//            NULL, NULL, 0, NULL, 0, &warnings, &errors, NULL));
//      }
//   }
   int endTime = GetTickCount();
   printf("Elapsed time for the validation is %d millisecponds\n", endTime - startTime);
}


/*=============================================================================================================================*/
void VELaSSCoMethods::getBoundingBox()
/*=============================================================================================================================*/
{
   int startTime = GetTickCount();
//   if (serverContexts) {
//      SdaiServerContext *scs = serverContexts->getElementArray();
//      EDMULONG nsc = serverContexts->size();
//      SdaiInteger          edmSystemType;
//
//#pragma omp parallel for
//      for (int i = 0; i < nsc; i++) {
//         SdaiInteger        warnings, errors;
//         //CHECK(edmiRemoteExecuteCppMethod(scs[i], "FEM_models", "VELaSSCo_HbaseBasicTest_part_1", "cpp_plugins", "VELaSSCo", "getBoundingBox",
//         //   0, 0, NULL, NULL, &returnValue, NULL));
//      }
//   }
   int endTime = GetTickCount();
   printf("Elapsed time is %d milliseconds\n", endTime - startTime);
}



/*=============================================================================================================================*/
void VELaSSCoMethods::GetListOfAnalyses(rvGetListOfAnalyses& _return)
/*=============================================================================================================================*/
{
   int startTime = GetTickCount();
   if (subQueries) {
      EDMULONG nexec = subQueries->size();

#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodervGetListOfAnalyses *retVal = new(e->ema)nodervGetListOfAnalyses(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetListOfAnalyses", NULL);
      }
      nodervGetListOfAnalyses *retValueWithError = NULL;
      Container<char*> analysisNames(&ma);
      vector<string> names;
      string aName;
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodervGetListOfAnalyses *retVal = (nodervGetListOfAnalyses *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         Container<char *> nameList(&ma, retVal->analysis_name_list);
         for (char *cn = nameList.first(); cn; cn = nameList.next()) {
            aName = cn;
            if (std::find(names.begin(), names.end(), aName) == names.end()) {
               names.push_back(aName);
            }
         }
      }
      if (retValueWithError) {
         _return.__set_status("Error");
         _return.__set_report(retValueWithError->report->value.stringVal);
      } else {
         _return.__set_status("OK");
         _return.__set_report("");
         _return.__set_analyses(names);
      }
   }
   int endTime = GetTickCount();
}


/*=============================================================================================================================*/
void VELaSSCoMethods::GetListOfResults(char *modelId, char *analysisID, double timeStep)
/*=============================================================================================================================*/
{
   int startTime = GetTickCount();
//   if (serverContexts) {
//      SdaiServerContext *scs = serverContexts->getElementArray();
//      EDMULONG nsc = serverContexts->size();
//      SdaiInteger          edmSystemType;
//
//#pragma omp parallel for
//      for (int i = 0; i < nsc; i++) {
//
//      }
//   }
   int endTime = GetTickCount();
   printf("Elapsed time for the validation is %d millisecponds\n", endTime - startTime);
}


/*=============================================================================================================================*/
void VELaSSCoMethods::GetListOfTimeSteps(rvGetListOfTimeSteps& rv, const std::string& analysisID)
/*=============================================================================================================================*/
{
   int startTime = GetTickCount();
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      nodeInGetListOfTimeSteps *inParams = new(&ma)nodeInGetListOfTimeSteps(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());

#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfTimeSteps *retVal = new(e->ema)nodeRvGetListOfTimeSteps(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetListOfTimeSteps", inParams);
      }
      nodeRvGetListOfTimeSteps *retValueWithError = NULL;
      vector<double>  time_steps;

      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfTimeSteps *retVal = (nodeRvGetListOfTimeSteps *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         Container<double> timeStepList(&ma, retVal->ListOfTimeSteps);
         for (double ts = timeStepList.first(); timeStepList.moreElements(); ts = timeStepList.next()) {
            if (std::find(time_steps.begin(), time_steps.end(), ts) == time_steps.end()) {
               time_steps.push_back(ts);
            }
         }
      }
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         rv.__set_status("OK");
         rv.__set_report("");
         rv.__set_time_steps(time_steps);
      }
   }
   int endTime = GetTickCount();
}


/*=============================================================================================================================*/
void VELaSSCoMethods::GetListOfVerticesFromMesh(rvGetListOfVerticesFromMesh& rv,
   const std::string& analysisID, const double stepValue, const int32_t meshID)
/*=============================================================================================================================*/
{
   int startTime, endTime;
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      nodeInGetListOfVerticesFromMesh *inParams = new(&ma)nodeInGetListOfVerticesFromMesh(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->stepValue->putReal(stepValue);
      inParams->meshID->putInteger(meshID);

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfVerticesFromMesh *retVal = new(e->ema)nodeRvGetListOfVerticesFromMesh(e->ema, NULL);
         e->returnValues = retVal;
         int localTime = GetTickCount();
         printf("Before ExecuteRemoteCppMethod: Time since start is %d milliseconds, Thread no %d\n", localTime - startTime, omp_get_thread_num());
         ExecuteRemoteCppMethod(e, "GetListOfVerticesFromMesh", inParams);
      }
      endTime = GetTickCount();
      printf("Elapsed time for parallel execiution is %d milliseconds\n", endTime - startTime);
      nodeRvGetListOfVerticesFromMesh *retValueWithError = NULL;
      vector<VELaSSCoSM::Vertex>  vertices;
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfVerticesFromMesh *retVal = (nodeRvGetListOfVerticesFromMesh *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         if (retVal->maxID->value.intVal > maxID) maxID = retVal->maxID->value.intVal;
         if (retVal->minID->value.intVal < minID) minID = retVal->minID->value.intVal;
      }
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         unsigned char *verticesExist = (unsigned char *)ma.allocZeroFilled(sizeof(unsigned char *) * (maxID + 1));
         EDMULONG nDuplicates = 0;
         for (int i = 0; i < nexec; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetListOfVerticesFromMesh *retVal = (nodeRvGetListOfVerticesFromMesh *)e->returnValues;
            Container<EDMVD::Vertex> vertexList(&ma, retVal->vertices);
            for (EDMVD::Vertex *v = vertexList.firstp(); v; v = vertexList.nextp()) {
               if (verticesExist[v->id] == 0) {
                  VELaSSCoSM::Vertex rv;
                  rv.__set_id(v->id); rv.__set_x(v->x); rv.__set_y(v->y); rv.__set_z(v->z);
                  vertices.push_back(rv); verticesExist[v->id] = 1;
               } else {
                  nDuplicates++;
               }
            }
            printf("minID=%15llu, maxID=%15llu\n", minID, maxID);
         }
         rv.__set_status("OK");
         rv.__set_report("");
         rv.__set_vertex_list(vertices);
      }
   }
}



/*=============================================================================================================================*/
void VELaSSCoMethods::GetListOfResultsFromTimeStepAndAnalysis(rvGetListOfResults& rv, const std::string& analysisID, const double stepValue)
/*=============================================================================================================================*/
{
   int startTime, endTime;
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      nodeInGetListOfResultsFromTimeStepAndAnalysis *inParams = new(&ma)nodeInGetListOfResultsFromTimeStepAndAnalysis(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->stepValue->putReal(stepValue);

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfResultsFromTimeStepAndAnalysis *retVal = new(e->ema)nodeRvGetListOfResultsFromTimeStepAndAnalysis(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetListOfResultsFromTimeStepAndAnalysis", inParams);
      }
      endTime = GetTickCount();
      printf("Elapsed time for parallel execiution is %d milliseconds\n", endTime - startTime);
      nodeRvGetListOfResultsFromTimeStepAndAnalysis *retValueWithError = NULL;

      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfResultsFromTimeStepAndAnalysis *retVal = (nodeRvGetListOfResultsFromTimeStepAndAnalysis *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
      }
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         vector<VELaSSCoSM::ResultInfo>  result_list;
         for (int i = 0; i < nexec; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetListOfResultsFromTimeStepAndAnalysis *retVal = (nodeRvGetListOfResultsFromTimeStepAndAnalysis *)e->returnValues;
            CMemoryAllocator resMa(retVal->ResultInfoList);

            relocateResultInfo *relocator = (relocateResultInfo *)resMa.getRelocateInfo();
            resMa.prepareForRelocationAfterTransfer();

            Container<EDMVD::ResultInfo> *dResults = (Container<EDMVD::ResultInfo>*)relocator->bufferInfo->relocatePointer((char*)relocator->sResults);
            dResults->relocateStructContainer(relocator->bufferInfo);
            for (EDMVD::ResultInfo *ri = dResults->firstp(); ri; ri = dResults->nextp()) {
               ri->relocateThis(relocator->bufferInfo);
               VELaSSCoSM::ResultInfo res;
               if (ri->name) res.__set_name(ri->name);
               if (ri->type) res.__set_type(ri->type);
               result_list.push_back(res);
            }
         }
         rv.__set_result_list(result_list);
         rv.__set_status("OK");
         rv.__set_report("");
      }
   }
}
/*=============================================================================================================================*/
void VELaSSCoMethods::GetCoordinatesAndElementsFromMesh(rvGetCoordinatesAndElementsFromMesh& rv, const std::string& analysisID, const double stepValue)
/*=============================================================================================================================*/
{
   int startTime, endTime;
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      nodeInGetCoordinatesAndElementsFromMesh *inParams = new(&ma)nodeInGetCoordinatesAndElementsFromMesh(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(stepValue);

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetCoordinatesAndElementsFromMesh *retVal = new(e->ema)nodeRvGetCoordinatesAndElementsFromMesh(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetListOfResultsFromTimeStepAndAnalysis", inParams);
      }
      endTime = GetTickCount();
      printf("Elapsed time for parallel execiution is %d milliseconds\n", endTime - startTime);
      nodeRvGetCoordinatesAndElementsFromMesh *retValueWithError = NULL;

      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetCoordinatesAndElementsFromMesh *retVal = (nodeRvGetCoordinatesAndElementsFromMesh *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
      }
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         rv.__set_status("OK");
         rv.__set_report("");
      }
   }
}
/*=============================================================================================================================*/
void VELaSSCoMethods::GetResultFromVerticesID(rvGetResultFromVerticesID& rv, const std::string& analysisID, const double timeStep,
   const std::string& resultID, const std::vector<int64_t> & listOfVertices)
/*=============================================================================================================================*/
{
   int startTime, endTime;
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      nodeInGetResultFromVerticesID *inParams = new(&ma)nodeInGetResultFromVerticesID(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(timeStep);
      inParams->resultID->putString((char*)analysisID.data());
      inParams->listOfVertices->putInteger(1);

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetResultFromVerticesID *retVal = new(e->ema)nodeRvGetResultFromVerticesID(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetListOfResultsFromTimeStepAndAnalysis", inParams);
      }
      endTime = GetTickCount();
      printf("Elapsed time for parallel execiution is %d milliseconds\n", endTime - startTime);
      nodeRvGetResultFromVerticesID *retValueWithError = NULL;

      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetResultFromVerticesID *retVal = (nodeRvGetResultFromVerticesID *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         if (retVal->maxID->value.intVal > maxID) maxID = retVal->maxID->value.intVal;
         if (retVal->minID->value.intVal < minID) minID = retVal->minID->value.intVal;
      }
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         unsigned char *verticesExist = (unsigned char *)ma.allocZeroFilled(sizeof(unsigned char *)* (maxID + 1));
         EDMULONG nDuplicates = 0;
         vector<VELaSSCoSM::ResultOnVertex>  result_list;
         for (int i = 0; i < nexec; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetResultFromVerticesID *retVal = (nodeRvGetResultFromVerticesID *)e->returnValues;
            Container<EDMVD::ResultOnVertex> resultList(&ma, retVal->result_list);
            for (EDMVD::ResultOnVertex *rov = resultList.firstp(); rov; rov = resultList.nextp()) {
               if (verticesExist[rov->id] == 0) {
                  VELaSSCoSM::ResultOnVertex VELaSSCoSM_rov;
                  result_list.push_back(VELaSSCoSM_rov); verticesExist[rov->id] = 1;
               } else {
                  nDuplicates++;
               }
            }
         }
         rv.__set_status("OK");
         rv.__set_report("");
         rv.__set_result_list(result_list);
      }
   }
}
/*==============================================================================================================================*/
char* VELaSSCoMethods::getPluginName()
/*==============================================================================================================================*/
{
   return "EDM_server_dll";
}
/*==============================================================================================================================*/
char* VELaSSCoMethods::getPluginPath()
/*==============================================================================================================================*/
{
   return "VELaSSCo_plugins";
}