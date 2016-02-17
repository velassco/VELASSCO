
#include "stdafx.h"
#include "VELaSSCoMethods.h"
#include "modules/DataLayer/Storage/EDM_server_dll/EDM_server_dll.h"
#include <omp.h>




///*=============================================================================================================================*/
//void VELaSSCoMethods::ListModels()
///*=============================================================================================================================*/
//{
//   if (serverContexts) {
////      SdaiServerContext *scs = serverContexts->getElementArray();
////      EDMULONG nsc = serverContexts->size();
////#pragma omp parallel for
////      for (int i = 0; i < nsc; i++) {
////         SdaiString        *foundNames;
////         CHECK(edmiRemoteListModels(scs[i], NULL, NULL, NULL, NULL, NULL, 0, &foundNames, NULL));
////         printf("Thread number %llu\n", omp_get_thread_num());
////         for (int i = 0; foundNames[i]; i++) {
////            printf("%s\n", foundNames[i]);
////         }
////         printf("\n\n");
////      }
//   }
//}

///*=============================================================================================================================*/
//void VELaSSCoMethods::ValidateModels()
///*=============================================================================================================================*/
//{
//   int startTime = GetTickCount();
////   if (serverContexts) {
////      SdaiServerContext *scs = serverContexts->getElementArray();
////      EDMULONG nsc = serverContexts->size();
////#pragma omp parallel for
////      for (int i = 0; i < nsc; i++) {
////         SdaiInteger        warnings, errors;
////         char diaFileName[2048];
////         sprintf(diaFileName, "O:\\projects\\VELaSSCo\\SVN_src\\EDM_plug_in\\db_cluster\\validate_%d.txt", i);
////         CHECK(edmiRemoteValidateModel(scs[i], "FEM_models", "VELaSSCo_HbaseBasicTest_part_1", diaFileName, 0, NULL, FULL_VALIDATION,
////            NULL, NULL, 0, NULL, 0, &warnings, &errors, NULL));
////      }
////   }
//   int endTime = GetTickCount();
//   //printf("Elapsed time for the validation is %d millisecponds\n", endTime - startTime);
//}


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


///*=============================================================================================================================*/
//void VELaSSCoMethods::GetListOfResults(char *modelId, char *analysisID, double timeStep)
///*=============================================================================================================================*/
//{
//   int startTime = GetTickCount();
////   if (serverContexts) {
////      SdaiServerContext *scs = serverContexts->getElementArray();
////      EDMULONG nsc = serverContexts->size();
////      SdaiInteger          edmSystemType;
////
////#pragma omp parallel for
////      for (int i = 0; i < nsc; i++) {
////
////      }
////   }
//   int endTime = GetTickCount();
//   //printf("Elapsed time for the validation is %d millisecponds\n", endTime - startTime);
//}


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
         for (double *ts = timeStepList.firstp(); timeStepList.moreElements(); ts = timeStepList.nextp()) {
            if (std::find(time_steps.begin(), time_steps.end(), *ts) == time_steps.end()) {
               time_steps.push_back(*ts);
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
      EDMULONG nOfSubmodels = subQueries->size();
      nodeInGetListOfVerticesFromMesh *inParams = new(&ma)nodeInGetListOfVerticesFromMesh(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->stepValue->putReal(stepValue);
      inParams->meshID->putInteger(meshID);
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nOfSubmodels; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         // e contains information about the execution of one subquery
         // create return value object for this subquery, retVal
         nodeRvGetListOfVerticesFromMesh *retVal = new(e->ema)nodeRvGetListOfVerticesFromMesh(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetListOfVerticesFromMesh", inParams, &errorFound);
      }
      // Execution in single thread mode from here
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg);
         rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
      }
      endTime = GetTickCount();
      nodeRvGetListOfVerticesFromMesh *retValueWithError = NULL;
      vector<VELaSSCoSM::Vertex>  vertices;
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      for (int i = 0; i < nOfSubmodels; i++) {
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
         for (int i = 0; i < nOfSubmodels; i++) {
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
      ReturnedMeshInfo *firstMetaData;

      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetCoordinatesAndElementsFromMesh *retVal = (nodeRvGetCoordinatesAndElementsFromMesh *)e->returnValues;
         ReturnedMeshInfo *metaData = (ReturnedMeshInfo*)retVal->returned_mesh_info->value.blobVal;
         if (i == 0) {
            firstMetaData = metaData;
         } else if (firstMetaData->element_record_size != metaData->element_record_size || firstMetaData->element_type != metaData->element_type || firstMetaData->model_type != metaData->model_type) {
            retValueWithError = retVal; break;
         }
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
         unsigned char *verticesExist = (unsigned char *)ma.allocZeroFilled(sizeof(unsigned char) * (maxNodeID + 1));
         unsigned char *elementExist = (unsigned char *)ma.allocZeroFilled(sizeof(unsigned char) * (maxElementID + 1));
         EDMULONG nDuplicates = 0, n_vertices = 0, n_elements = 0;

         rv.__set_element_record_size(firstMetaData->element_record_size);
         rv.__set_n_vertices_pr_element(firstMetaData->n_vertices_pr_element);
         rv.__set_vertex_record_size(firstMetaData->vertex_record_size);
         rv.__set_element_type(firstMetaData->element_type);
         rv.__set_model_type(firstMetaData->model_type);
         rv.__set_model_type(1);

         startTime = GetTickCount();
         for (int i = 0; i < nexec; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetCoordinatesAndElementsFromMesh *retVal = (nodeRvGetCoordinatesAndElementsFromMesh *)e->returnValues;

            Container<EDMVD::Vertex> vertices(&ma, retVal->node_array);
            for (EDMVD::Vertex *v = vertices.firstp(); v; v = vertices.nextp()) {
               if (verticesExist[v->id] == 0) { verticesExist[v->id] = 1; n_vertices++; } else { nDuplicates++; }
            }
            Container<EDMVD::FEMelement> elements(&ma, retVal->elemnt_array);
            for (EDMVD::FEMelement *e = elements.firstp(); e; e = elements.nextp()) {
               if (elementExist[e->id] == 0) { elementExist[e->id] = 1; n_elements++; } else { nDuplicates++; }
            }
         }
         string vertex_string, element_string;
         EDMULONG vertexBufferSize = sizeof(EDMVD::Vertex) * n_vertices;
         vertex_string.resize(vertexBufferSize);
         element_string.resize(firstMetaData->element_record_size * n_elements);
         char *vp = (char*)vertex_string.data();
         char *vpEnd = vp + vertexBufferSize;
         char *ep = (char*)element_string.data();
         memset(verticesExist, 0, sizeof(unsigned char) * (maxNodeID + 1));
         memset(elementExist, 0, sizeof(unsigned char) * (maxElementID + 1));
         EDMULONG n_vertices_copied = 0, n_elements_copied = 0;

         for (int i = 0; i < nexec; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetCoordinatesAndElementsFromMesh *retVal = (nodeRvGetCoordinatesAndElementsFromMesh *)e->returnValues;

            Container<EDMVD::Vertex> vertices(&ma, retVal->node_array);
            for (EDMVD::Vertex *v = vertices.firstp(); v; v = vertices.nextp()) {
               if (verticesExist[v->id] == 0) {
                  memmove(vp, v, firstMetaData->vertex_record_size);
                  vp += firstMetaData->vertex_record_size; n_vertices_copied++; verticesExist[v->id] = 1;
               }
            }
            Container<EDMVD::FEMelement> elements(&ma, retVal->elemnt_array);
            for (EDMVD::FEMelement *e = elements.firstp(); e; e = elements.nextp()) {
               if (elementExist[e->id] == 0) {
                  memmove(ep, e, firstMetaData->element_record_size);
                  ep += firstMetaData->element_record_size; n_elements_copied++; elementExist[e->id] = 1;
               }
            }
         }
         rv.__set_n_vertices(n_vertices_copied);
         rv.__set_n_elements(n_elements_copied);
         rv.__set_element_array(element_string);
         rv.__set_vertex_array(vertex_string);
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
   int startTime, startTime2, endTime, timeSubQueryExecytion, timeCollectingReturnValues, t2, t3, t4, t5, t6;
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      nodeInGetResultFromVerticesID *inParams = new(&ma)nodeInGetResultFromVerticesID(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(timeStep);
      inParams->resultID->putString((char*)resultID.data());
      inParams->listOfVertices->putIntegerAggr((EDMLONG*)listOfVertices.data(), listOfVertices.size());
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetResultFromVerticesID *retVal = new(e->ema)nodeRvGetResultFromVerticesID(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetResultFromVerticesID", inParams, &errorFound);
      }
      t2 = GetTickCount();
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg); rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
      }
      endTime = GetTickCount(); timeSubQueryExecytion = endTime - startTime;
      //printf("Elapsed time for parallel execution is %d milliseconds\n", endTime - startTime);
      nodeRvGetResultFromVerticesID *retValueWithError = NULL;

      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      EDMULONG nOfValuesPrVertex = 0;
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
      t3 = GetTickCount();
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         unsigned char *verticesExist = (unsigned char *)ma.allocZeroFilled(sizeof(unsigned char *)* (maxID + 1));
         EDMULONG nDuplicates = 0, n_result_records = 0;
         Container<EDMVD::ResultOnVertex> **vertexResultsArr = (Container<EDMVD::ResultOnVertex> **)ma.alloc(sizeof(Container<EDMVD::ResultOnVertex> *) * nexec);
         startTime2 = GetTickCount();
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
         t4 = GetTickCount();
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

         t5 = GetTickCount();
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
         t6 = GetTickCount();
         endTime = GetTickCount();
         timeCollectingReturnValues = endTime - startTime2;
         char msgBuffer[2048];
         sprintf(msgBuffer, "Execution time subqueries: %d\nExecution time building return values: %d\nt2=%d\nt3=%d\nt4=%d\nt5=%d\nt6=%d\nNumber of distinct results: %lld\nNumber of duplicates: %lld\n",
            timeSubQueryExecytion, timeCollectingReturnValues, t2 - startTime, t3 - startTime, t4 - startTime, t5 - startTime, t6 - startTime,
            n_result_records, nDuplicates);
         rv.__set_status("OK");
         rv.__set_report(msgBuffer);
         rv.__set_result_array(result_array);
         rv.__set_n_values_pr_record(nOfValuesPrVertex);
         rv.__set_result_record_size(ResultOnVertexSize);
         rv.__set_n_result_records(n_result_records);
      }
   }
}
/*=============================================================================================================================*/
void VELaSSCoMethods::GetBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& rv, const std::string& analysisID, const double stepValue,
   const std::string& meshID)
/*=============================================================================================================================*/
{
   int startTime, endTime, timeSubQueryExecytion, timeCollectingReturnValues;
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      nodeInGetBoundaryOfLocalMesh *inParams = new(&ma)nodeInGetBoundaryOfLocalMesh(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(stepValue);
      inParams->meshID->putString((char*)meshID.data());
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetBoundaryOfLocalMesh *retVal = new(e->ema)nodeRvGetBoundaryOfLocalMesh(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetBoundaryOfLocalMesh", inParams, &errorFound);
      }
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg); rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
      }
      endTime = GetTickCount(); timeSubQueryExecytion = endTime - startTime;
      nodeRvGetBoundaryOfLocalMesh *retValueWithError = NULL;

      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      int tot_n_triangles = 0;
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetBoundaryOfLocalMesh *retVal = (nodeRvGetBoundaryOfLocalMesh *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         tot_n_triangles += retVal->n_triangles->value.intVal;
         //if (i == 0) {
         //   nOfValuesPrVertex = retVal->nOfValuesPrVertex->value.intVal;
         //} else if (nOfValuesPrVertex != retVal->nOfValuesPrVertex->value.intVal) {
         //   THROW("Different number of values pr vertex in Subqueries in GetResultFromVerticesID.")
         //}
         //if (retVal->maxID->value.intVal > maxID) maxID = retVal->maxID->value.intVal;
         //if (retVal->minID->value.intVal < minID) minID = retVal->minID->value.intVal;
      }
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         startTime = GetTickCount();
         EDMULONG n_triangles_pr_string = tot_n_triangles / 4;
         std::vector<std::string> triangle_strings;
         EDMULONG n_triangles_in_this_string = n_triangles_pr_string;
         int stringNo = -1;
         //std::string *cTriangleBuffer;
         char *bp;

         for (int i = 0; i < nexec; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetBoundaryOfLocalMesh *retVal = (nodeRvGetBoundaryOfLocalMesh *)e->returnValues;
            Container<EDMVD::Triangle>  triangles(&ma, retVal->triangle_array);
            for (EDMVD::Triangle *t = triangles.firstp(); t; t = triangles.nextp()) {
               if (t->node_ids[0] == 0 || t->node_ids[1] == 0 || t->node_ids[2] == 0) {
                  int nasdfasdf = 9999;
               }
               if (n_triangles_in_this_string >= n_triangles_pr_string) {
                  n_triangles_in_this_string = 0;
                  stringNo++;
                  triangle_strings.push_back(std::string());
                  triangle_strings[stringNo].resize(n_triangles_pr_string * sizeof(EDMVD::Triangle));
                  bp = (char*)triangle_strings[stringNo].data();
               }
               memmove(bp, t, sizeof(EDMVD::Triangle)); bp += sizeof(EDMVD::Triangle);
               n_triangles_in_this_string++;
            }
         }
         triangle_strings[stringNo].resize(n_triangles_in_this_string * sizeof(EDMVD::Triangle));

         endTime = GetTickCount();
         timeCollectingReturnValues = endTime - startTime;
         char msgBuffer[2048];
         sprintf(msgBuffer, "Execution time subqueries: %d\nExecution time building return values: %d\nNumber of triangles: %lld\nNumber of triangles pr string: %lld\n",
            timeSubQueryExecytion, timeCollectingReturnValues, tot_n_triangles, n_triangles_pr_string);
         rv.__set_status("OK");
         rv.__set_report(msgBuffer);
         rv.__set_triangle_record_size(sizeof(EDMVD::Triangle));
         rv.__set_nTriangles(tot_n_triangles);
         rv.__set_triangles(triangle_strings);
      }
   }
}
/*=============================================================================================================================*/
void VELaSSCoMethods::GetListOfMeshes(rvGetListOfMeshes& rv, const std::string& analysisID, const double timeStep)
/*=============================================================================================================================*/
{
   int startTime, endTime;
   if (subQueries) {
      EDMULONG nexec = subQueries->size();
      nodeInGetListOfMeshes *inParams = new(&ma)nodeInGetListOfMeshes(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(timeStep);
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfMeshes *retVal = new(e->ema)nodeRvGetListOfMeshes(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "GetListOfMeshes", inParams, &errorFound);
      }
      if (errorFound) {
         string errorMsg;
         writeErrorMessageForSubQueries(errorMsg); rv.__set_status("Error"); rv.__set_report(errorMsg);
         return;
      }
      nodeRvGetListOfMeshes *retValueWithError = NULL;
      int tot_n_triangles = 0;
      for (int i = 0; i < nexec; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfMeshes *retVal = (nodeRvGetListOfMeshes *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
      }
      if (retValueWithError) {
         rv.__set_status("Error");
         rv.__set_report(retValueWithError->report->value.stringVal);
      } else {
         vector<VELaSSCoSM::MeshInfo> meshInfos;
         int meshNumber = 1;

         for (int i = 0; i < nexec; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetListOfMeshes *retVal = (nodeRvGetListOfMeshes *)e->returnValues;
            CMemoryAllocator resMa(retVal->mesh_info_list, false);

            relocateMeshInfo *mesh_info_relocator = (relocateMeshInfo *)resMa.getRelocateInfo();
            resMa.prepareForRelocationAfterTransfer();
            Container<EDMVD::MeshInfo> *mesh_info_container = (Container<EDMVD::MeshInfo>*)mesh_info_relocator->bufferInfo->relocatePointer((char*)mesh_info_relocator->meshes);
            mesh_info_container->relocateStructContainer(mesh_info_relocator->bufferInfo);

            for (EDMVD::MeshInfo *mi = mesh_info_container->firstp(); mi; mi = mesh_info_container->nextp()) {
               mi->relocateThis(mesh_info_relocator->bufferInfo);
               //vector<VELaSSCoSM::MeshInfo>::iterator meshInfoIter;
               //for (meshInfoIter = meshInfos.begin(); meshInfoIter != meshInfos.end(); meshInfoIter++) {
               //   if (strEQL(mi->name, meshInfoIter->name.data()))
               //      break;
               //}
               //if (meshInfoIter == meshInfos.end()) {
                  VELaSSCoSM::MeshInfo meshInfo;
                  meshInfo.__set_name(mi->name); meshInfo.__set_nElements(mi->nElements);  meshInfo.__set_nVertices(mi->nVertices);
                  VELaSSCoSM::ElementType elementType;
                  elementType.__set_shape(mi->elementType.shape); elementType.__set_num_nodes(mi->nVertices);
                  meshInfo.__set_elementType(elementType);
                  meshInfo.__set_meshNumber(meshNumber++);

                  meshInfos.push_back(meshInfo);
               //}
            }
         }
         rv.__set_status("OK");
         rv.__set_meshInfos(meshInfos);
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
/*==============================================================================================================================*/
VELaSSCoMethods::~VELaSSCoMethods()
/*==============================================================================================================================*/
{
}