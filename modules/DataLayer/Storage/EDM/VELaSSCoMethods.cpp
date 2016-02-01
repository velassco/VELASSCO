
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
   //printf("Elapsed time for the validation is %d millisecponds\n", endTime - startTime);
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
   //printf("Elapsed time is %d milliseconds\n", endTime - startTime);
}



/*=============================================================================================================================*/
void VELaSSCoMethods::GetListOfAnalyses(rvGetListOfAnalyses& rv)
/*=============================================================================================================================*/
{
   int startTime = GetTickCount();
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      int nError = 0;
      bool errorFound = false;
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         try {
            EDMexecution *e = subQueries->getElementp(i);
            nodervGetListOfAnalyses *retVal = new(e->ema)nodervGetListOfAnalyses(e->ema, NULL);
            e->returnValues = retVal;
            ExecuteRemoteCppMethod(e, "GetListOfAnalyses", NULL, &errorFound);
         } catch (CedmError *e) {
            delete e; nError++;
         }
      }
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg);
         rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
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
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         rv.__set_status("OK");
         rv.__set_report("");
         rv.__set_analyses(names);
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
   //printf("Elapsed time for the validation is %d millisecponds\n", endTime - startTime);
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
      bool errorFound = false;

#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfTimeSteps *retVal = new(e->ema)nodeRvGetListOfTimeSteps(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetListOfTimeSteps", inParams, &errorFound);
      }
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg);
         rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
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
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfVerticesFromMesh *retVal = new(e->ema)nodeRvGetListOfVerticesFromMesh(e->ema, NULL);
         e->returnValues = retVal;
         int localTime = GetTickCount();
         printf("Before ExecuteRemoteCppMethod: Time since start is %d milliseconds, Thread no %d\n", localTime - startTime, omp_get_thread_num());
         ExecuteRemoteCppMethod(e, "GetListOfVerticesFromMesh", inParams, &errorFound);
      }
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg);
         rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
      }
      endTime = GetTickCount();
      //printf("Elapsed time for parallel execution is %d milliseconds\n", endTime - startTime);
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
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfResultsFromTimeStepAndAnalysis *retVal = new(e->ema)nodeRvGetListOfResultsFromTimeStepAndAnalysis(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetListOfResultsFromTimeStepAndAnalysis", inParams, &errorFound);
      }
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg); rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
      }
      endTime = GetTickCount();
      //printf("Elapsed time for parallel execution is %d milliseconds\n", endTime - startTime);
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
            CMemoryAllocator resMa(retVal->ResultInfoList, false);

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
      inParams->meshName->putString("");
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetCoordinatesAndElementsFromMesh *retVal = new(e->ema)nodeRvGetCoordinatesAndElementsFromMesh(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetCoordinatesAndElementsFromMesh", inParams, &errorFound);
      }
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg); rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
      }
      endTime = GetTickCount();
      //printf("Elapsed time for parallel execution is %d milliseconds\n", endTime - startTime);
      nodeRvGetCoordinatesAndElementsFromMesh *retValueWithError = NULL;
      EDMULONG maxNodeID = 0, minNodeID = 0xfffffffffffffff, maxElementID = 0, minElementID = 0xfffffffffffffff;

      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetCoordinatesAndElementsFromMesh *retVal = (nodeRvGetCoordinatesAndElementsFromMesh *)e->returnValues;
         ReturnedMeshInfo *metaData = (ReturnedMeshInfo*)retVal->returned_mesh_info;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         if (metaData->maxNodeID > maxNodeID) maxNodeID = metaData->maxNodeID;
         if (metaData->minNodeID < minNodeID) minNodeID = metaData->minNodeID;
         if (metaData->maxElementID > maxElementID) maxElementID = metaData->maxElementID;
         if (metaData->minElementID < minElementID) minElementID = metaData->minElementID;
      }
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         unsigned char *verticesExist = (unsigned char *)ma.allocZeroFilled(sizeof(unsigned char *)* (maxNodeID + 1));
         EDMULONG nDuplicates = 0, n_vertices = 0, n_elements = 0;
         Container<EDMVD::ResultOnVertex> **vertexResultsArr = (Container<EDMVD::ResultOnVertex> **)ma.alloc(sizeof(Container<EDMVD::ResultOnVertex> *) * nexec);
         startTime = GetTickCount();
         for (int i = 0; i < nexec; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetCoordinatesAndElementsFromMesh *retVal = (nodeRvGetCoordinatesAndElementsFromMesh *)e->returnValues;

            Container<EDMVD::Vertex> *vertices = (Container<EDMVD::Vertex>*)retVal->node_array->value.blobVal;
            for (EDMVD::Vertex *v = vertices->firstp(); v; v = vertices->nextp()) {
               if (verticesExist[v->id] == 0) {
                  verticesExist[v->id] = 1; n_vertices++;
               } else {
                  nDuplicates++;
               }
            }
            Container<EDMVD::FEMelement> *elements = (Container<EDMVD::FEMelement>*)retVal->elemnt_array->value.blobVal;


         }
         
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
   int startTime, endTime, timeSubQueryExecytion, timeCollectingReturnValues;
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      nodeInGetResultFromVerticesID *inParams = new(&ma)nodeInGetResultFromVerticesID(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(timeStep);
      inParams->resultID->putString((char*)resultID.data());
      inParams->listOfVertices->putInteger(1);
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetResultFromVerticesID *retVal = new(e->ema)nodeRvGetResultFromVerticesID(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetResultFromVerticesID", inParams, &errorFound);
      }
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg); rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
      }
      endTime = GetTickCount(); timeSubQueryExecytion = endTime - startTime;
      //printf("Elapsed time for parallel execution is %d milliseconds\n", endTime - startTime);
      nodeRvGetResultFromVerticesID *retValueWithError = NULL;

      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      int nOfValuesPrVertex = 0;
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetResultFromVerticesID *retVal = (nodeRvGetResultFromVerticesID *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         if (i == 0) {
            nOfValuesPrVertex = retVal->nOfValuesPrVertex->value.intVal;
         } else if (nOfValuesPrVertex != retVal->nOfValuesPrVertex->value.intVal) {
            THROW("Different number of values pr vertex in Subqueries in GetResultFromVerticesID.")
         }
         if (retVal->maxID->value.intVal > maxID) maxID = retVal->maxID->value.intVal;
         if (retVal->minID->value.intVal < minID) minID = retVal->minID->value.intVal;
      }
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         unsigned char *verticesExist = (unsigned char *)ma.allocZeroFilled(sizeof(unsigned char *)* (maxID + 1));
         EDMULONG nDuplicates = 0, n_result_records = 0;
         Container<EDMVD::ResultOnVertex> **vertexResultsArr = (Container<EDMVD::ResultOnVertex> **)ma.alloc(sizeof(Container<EDMVD::ResultOnVertex> *) * nexec);
         startTime = GetTickCount();
         for (int i = 0; i < nexec; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetResultFromVerticesID *retVal = (nodeRvGetResultFromVerticesID *)e->returnValues;
            CMemoryAllocator result_ma(retVal->result_list, false);


            relocateResultOnVertex *relocateInfo = (relocateResultOnVertex *)result_ma.getRelocateInfo();
            result_ma.prepareForRelocationAfterTransfer();
            relocateInfo->vertexResults = (Container<EDMVD::ResultOnVertex>*)relocateInfo->bufferInfo->relocatePointer((char*)relocateInfo->vertexResults);
            Container<EDMVD::ResultOnVertex> *vertexResults = relocateInfo->vertexResults;
            vertexResults->relocateStructContainer(relocateInfo->bufferInfo);
            vertexResultsArr[i] = vertexResults;
            for (EDMVD::ResultOnVertex *rov = vertexResults->firstp(); rov; rov = vertexResults->nextp()) {
               if (verticesExist[rov->id] == 0) {
                  verticesExist[rov->id] = 1;
                  n_result_records++;
               } else {
                  nDuplicates++;
               }
            }
         }
#define BLOB 1
#ifdef BLOB
         EDMLONG ResultOnVertexSize = sizeof(EDMVD::ResultOnVertex) + sizeof(double)* (nOfValuesPrVertex - 1);
         std::string result_array;
         EDMLONG blobSize = ResultOnVertexSize * n_result_records;
         result_array.resize(blobSize);
         char *cp = (char*)result_array.data();
#else
         vector<VELaSSCoSM::ResultOnVertex>  result_array;
         EDMLONG resSize = result_array.size();
         //result_array.reserve(n_result_records);
         resSize = result_array.size();
#endif
         memset(verticesExist, 0, sizeof(unsigned char *)* (maxID + 1));

         for (int i = 0; i < nexec; i++) {
            Container<EDMVD::ResultOnVertex> *vertexResults = vertexResultsArr[i];
            for (EDMVD::ResultOnVertex *rov = vertexResults->firstp(); rov; rov = vertexResults->nextp()) {
               if (verticesExist[rov->id] == 0) {
#ifdef BLOB
                  memmove(cp, rov, ResultOnVertexSize);
                  cp += ResultOnVertexSize;
#else
                  vector<double> values;
                  //values.reserve(nOfValuesPrVertex);
                  VELaSSCoSM::ResultOnVertex VELaSSCoSM_rov;
                  VELaSSCoSM_rov.__set_id(rov->id);
                  for (int vi = 0; vi < nOfValuesPrVertex; vi++) {
                     values.push_back(rov->value[vi]);
                  }
                  VELaSSCoSM_rov.__set_value(values); 
                  result_array.push_back(VELaSSCoSM_rov);
#endif
                  verticesExist[rov->id] = 1;
               }
            }
         }
         endTime = GetTickCount();
         timeCollectingReturnValues = endTime - startTime;
         char msgBuffer[2048];
         sprintf(msgBuffer, "Execution time subqueries: %d\nExecution time building return values: %d\nNumber of distinct results: %lld\nNumber of duplicates: %lld\n",
            timeSubQueryExecytion, timeCollectingReturnValues, n_result_records, nDuplicates);
         rv.__set_status("OK");
         rv.__set_report(msgBuffer);
         rv.__set_result_array(result_array);
         rv.__set_n_values_pr_record(nOfValuesPrVertex);
         rv.__set_result_record_size(ResultOnVertexSize);
         rv.__set_n_result_records(n_result_records);
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