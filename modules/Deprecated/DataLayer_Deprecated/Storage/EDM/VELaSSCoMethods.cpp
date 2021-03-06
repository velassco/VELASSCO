
#include "stdafx.h"
#include "VELaSSCoMethods.h"
#include "modules/DataLayer/Storage/EDM_server_dll/EDM_server_dll.h"
#include "VELaSSCo_utils.h"
#include <omp.h>





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
      EDMULONG nOfSubdomains = subQueries->size();
      int nError = 0;
      bool errorFound = false;

#pragma omp parallel for
      for (int i = 0; i < nOfSubdomains; i++) {
         try {
            EDMexecution *e = subQueries->getElementp(i);
//            printf("GetListOfAnalyses, nSubQuery=%llu, i=%d, e->modelName=%s\n", nOfSubdomains, i, e ? e->modelName : "EDMexecution e is NULL");
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
      for (int i = 0; i < nOfSubdomains; i++) {
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
         string report;
         printExecutionReport(report);
         rv.__set_status("OK");
         rv.__set_report(report);
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
      EDMULONG nOfSubdomains = subQueries->size();
      nodeInGetListOfTimeSteps *inParams = new(&ma)nodeInGetListOfTimeSteps(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      bool errorFound = false;

#pragma omp parallel for
      for (int i = 0; i < nOfSubdomains; i++) {
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

      for (int i = 0; i < nOfSubdomains; i++) {
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
         string report;
         printExecutionReport(report);
         rv.__set_report(report);
         rv.__set_status("OK");
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
         }
         string report;
         printExecutionReport(report);
         rv.__set_report(report);
         rv.__set_status("OK");
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
      EDMULONG nOfSubdomains = subQueries->size();
      nodeInGetListOfResultsFromTimeStepAndAnalysis *inParams = new(&ma)nodeInGetListOfResultsFromTimeStepAndAnalysis(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->stepValue->putReal(stepValue);
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nOfSubdomains; i++) {
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

      for (int i = 0; i < nOfSubdomains; i++) {
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
         for (int i = 0; i < nOfSubdomains; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetListOfResultsFromTimeStepAndAnalysis *retVal = (nodeRvGetListOfResultsFromTimeStepAndAnalysis *)e->returnValues;
            CMemoryAllocator resMa(retVal->ResultInfoList, false);

            relocateResultInfo *relocator = (relocateResultInfo *)resMa.getRelocateInfo();
            resMa.prepareForRelocationAfterTransfer();

            Container<EDMVD::ResultInfo> *dResults = (Container<EDMVD::ResultInfo>*)relocator->bufferInfo->relocatePointer((char*)relocator->sResults);
            dResults->relocateStructContainer(relocator->bufferInfo);
            for (EDMVD::ResultInfo *ri = dResults->firstp(); ri; ri = dResults->nextp()) {
               ri->relocateThis(relocator->bufferInfo);
               vector<VELaSSCoSM::ResultInfo>::iterator resIter;
               for (resIter = result_list.begin(); resIter != result_list.end(); resIter++) {
                  if (strEQL(ri->name, resIter->name.data())) break;
               }
               if (resIter == result_list.end()) {
                  VELaSSCoSM::ResultInfo res;
                  if (ri->name) res.__set_name(ri->name);
                  if (ri->type) res.__set_type(ri->type);
                  if (ri->location) res.__set_location(ri->location);
                  res.__set_numberOfComponents(ri->componentNames->size());
                  vector<string> componentNames;
                  for (char *cn = ri->componentNames->first(); cn; cn = ri->componentNames->next()) {
                     componentNames.push_back(cn);
                  }
                  res.__set_componentNames(componentNames);
                  result_list.push_back(res);
               }
            }
         }
         string report;
         printExecutionReport(report);
         rv.__set_report(report);
         rv.__set_result_list(result_list);
         rv.__set_status("OK");
      }
   }
}
/*=============================================================================================================================*/
void VELaSSCoMethods::GetCoordinatesAndElementsFromMesh(rvGetCoordinatesAndElementsFromMesh& rv, const std::string& analysisID, const double stepValue)
/*=============================================================================================================================*/
{
   int startTime, endTime;
   if (subQueries) {
      EDMULONG nOfSubdomains = subQueries->size();
      nodeInGetCoordinatesAndElementsFromMesh *inParams = new(&ma)nodeInGetCoordinatesAndElementsFromMesh(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(stepValue);
      inParams->meshName->putString("");
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nOfSubdomains; i++) {
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
      //printf("Elapsed time for parallel execution is %d milliseconds\n", endTime - startTime);
      nodeRvGetCoordinatesAndElementsFromMesh *retValueWithError = NULL;
      EDMULONG maxNodeID = 0, minNodeID = 0xfffffffffffffff, maxElementID = 0, minElementID = 0xfffffffffffffff;
      EDMULONG n_vertices = 0, n_elements = 0;
      ReturnedMeshInfo *firstMetaData;

      for (int i = 0; i < nOfSubdomains; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetCoordinatesAndElementsFromMesh *retVal = (nodeRvGetCoordinatesAndElementsFromMesh *)e->returnValues;
         ReturnedMeshInfo *metaData = (ReturnedMeshInfo*)retVal->returned_mesh_info->value.blobVal;
         if (i == 0) {
            firstMetaData = metaData;
         //} else if (firstMetaData->element_record_size != metaData->element_record_size || firstMetaData->element_type != metaData->element_type || firstMetaData->model_type != metaData->model_type) {
         } else if (firstMetaData->element_record_size != metaData->element_record_size || firstMetaData->model_type != metaData->model_type) {
            retValueWithError = retVal; break;
         }
         n_vertices += metaData->n_vertices; n_elements += metaData->n_elements;
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
         EDMULONG nNodeDuplicates = 0, nElementDuplicates = 0;
         int beforeThriftTime = GetTickCount();

         vector<VELaSSCoSM::Vertex> vertex_list;
         vector<VELaSSCoSM::Element> element_list;
         int sizeV = sizeof(VELaSSCoSM::Vertex);
         vertex_list.reserve(n_vertices);
         element_list.reserve(n_elements);

         EDMULONG n_vertices_copied = 0, n_elements_copied = 0;
         EDMULONG n_vertices_pr_element = firstMetaData->n_vertices_pr_element;

         for (int i = 0; i < nOfSubdomains; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetCoordinatesAndElementsFromMesh *retVal = (nodeRvGetCoordinatesAndElementsFromMesh *)e->returnValues;

            Container<EDMVD::Vertex> vertices(&ma, retVal->node_array);
            for (EDMVD::Vertex *v = vertices.firstp(); v; v = vertices.nextp()) {
               if (verticesExist[v->id] == 0) {
                  VELaSSCoSM::Vertex vv;
                  vv.__set_id(v->id); vv.__set_x(v->x); vv.__set_y(v->y); vv.__set_z(v->z);
                  vertex_list.push_back(vv);  verticesExist[v->id] = 1;
               } else {
                  nNodeDuplicates++;
               }
            }
#ifdef DATA_returned
            Container<EDMVD::FEMelement> elements(&ma, retVal->elemnt_array);
            for (EDMVD::FEMelement *e = elements.firstp(); e; e = elements.nextp()) {
               VELaSSCoSM::Element el;
               el.__set_id(e->id);
               vector<NodeID> nodes;
               nodes.reserve(n_vertices_pr_element);
               for (int i = 0; i < n_vertices_pr_element; i++) nodes.push_back(e->nodes_ids[i]);
               el.__set_nodes_ids(nodes);
               element_list.push_back(el);
            }
#endif
         }
         char tbuf[2000];
         sprintf(tbuf, "\nTime before thrift: %d\n", beforeThriftTime - startTime);
         string report;
         printExecutionReport(report); report += tbuf;
         rv.__set_report(report);
         rv.__set_status("OK");
         rv.__set_vertex_list(vertex_list);
         rv.__set_element_list(element_list);
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
      EDMULONG nOfSubdomains = subQueries->size();
      nodeInGetResultFromVerticesID *inParams = new(&ma)nodeInGetResultFromVerticesID(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(timeStep);
      inParams->resultID->putString((char*)resultID.data());
      inParams->listOfVertices->putIntegerAggr((EDMLONG*)listOfVertices.data(), listOfVertices.size());
      bool errorFound = false;

      startTime = GetTickCount();

#pragma omp parallel for 
      for (int i = 0; i < nOfSubdomains; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         if (!e) {
            int asdfsdg = 9999;
         }
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
      for (int i = 0; i < nOfSubdomains; i++) {
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
         Container<EDMVD::ResultOnVertex> **vertexResultsArr = (Container<EDMVD::ResultOnVertex> **)ma.alloc(sizeof(Container<EDMVD::ResultOnVertex> *) * nOfSubdomains);
         startTime2 = GetTickCount();
         for (int i = 0; i < nOfSubdomains; i++) {
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
//#define BLOB 1
#ifdef BLOB
         EDMLONG ResultOnVertexSize = sizeof(EDMVD::ResultOnVertex) + sizeof(double)* (nOfValuesPrVertex - 1);
         std::string result_array;
         EDMLONG blobSize = ResultOnVertexSize * n_result_records;
         result_array.resize(blobSize);
         char *cp = (char*)result_array.data();
#else
         vector<VELaSSCoSM::ResultOnVertex>  result_array;
         EDMLONG resSize = result_array.size();
         result_array.reserve(n_result_records);
         resSize = result_array.size();
#endif
         memset(verticesExist, 0, sizeof(unsigned char *)* (maxID + 1));

         t5 = GetTickCount();
         for (int i = 0; i < nOfSubdomains; i++) {
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
         sprintf(msgBuffer, "Execution time subqueries: %d\nOfSubdomainsution time building return values: %d\nt2=%d\nt3=%d\nt4=%d\nt5=%d\nt6=%d\nNumber of distinct results: %lld\nNumber of duplicates: %lld\n",
            timeSubQueryExecytion, timeCollectingReturnValues, t2 - startTime, t3 - startTime, t4 - startTime, t5 - startTime, t6 - startTime,
            n_result_records, nDuplicates);
         string report;
         printExecutionReport(report); report += msgBuffer;
         rv.__set_report(report);
         rv.__set_status("OK");
#ifdef BLOB
         rv.__set_result_array(result_array);
         rv.__set_n_values_pr_record(nOfValuesPrVertex);
         rv.__set_result_record_size(ResultOnVertexSize);
         rv.__set_n_result_records(n_result_records);
#else
         rv.__set_result_list(result_array);
#endif
      }
   }
}

void writeTrianglesToFile(char *fileName, Container<EDMVD::Triangle> *cont)
{
   int nError = 0;
   FILE *triangleFile = fopen(fileName, "w");
   if (triangleFile) {
      EDMVD::Triangle *p = NULL;
      for (EDMVD::Triangle *t = cont->firstp(); t; t = cont->nextp()) {
         if (p) {
            if (p->conpare(t) >= 0) {
               //THROW("Merge error in VELaSSCoMethods::GetBoundaryOfLocalMesh.")
               if (++nError < 100)
                  fprintf(triangleFile, "Merge error in VELaSSCoMethods::GetBoundaryOfLocalMesh.");
            }
         }
         p = t;
         fprintf(triangleFile, "%10llu%10llu%10llu\n", t->node_ids[0], t->node_ids[1], t->node_ids[2]);
      }
      fclose(triangleFile);
   }
}
/*=============================================================================================================================*/
void VELaSSCoMethods::GetBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& rv, const std::string& analysisID, const double stepValue,
   const std::string& meshID)
/*=============================================================================================================================*/
{
   int startTime, endTime, timeSubQueryExecytion, timeCollectingReturnValues;
   if (subQueries) {
      EDMULONG nOfSubdomains = subQueries->size();
      nodeInGetBoundaryOfLocalMesh *inParams = new(&ma)nodeInGetBoundaryOfLocalMesh(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(stepValue);
      inParams->meshID->putString((char*)meshID.data());
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nOfSubdomains; i++) {
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
      char tfilena[1024];
      for (int i = 0; i < nOfSubdomains; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetBoundaryOfLocalMesh *retVal = (nodeRvGetBoundaryOfLocalMesh *)e->returnValues;
         if(strNEQ(retVal->status->value.stringVal, "OK")) {
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
         //std::vector<std::string> triangle_strings;
         //EDMULONG n_triangles_in_this_string = n_triangles_pr_string;
         //int stringNo = -1;
         //std::string *cTriangleBuffer;
         //char *bp;
         int nOfCores = omp_get_max_threads();
         int nOfMergeJobs = nOfCores;
         int nOfContainersPrMergeJob = nOfSubdomains / nOfMergeJobs;
         if (nOfContainersPrMergeJob < 2) {
            nOfContainersPrMergeJob = 2; nOfMergeJobs = nOfSubdomains / 2;
         }
         int rest = nOfSubdomains % nOfMergeJobs;

         MergeJob **mergeJobs = (MergeJob **)ma.alloc(nOfMergeJobs * sizeof(MergeJob *));
         int cSubQuery = 0;

         for (int i = 0; i < nOfMergeJobs; i++) {
            MergeJob *mj = mergeJobs[i] = new(&ma)MergeJob(&ma, nOfContainersPrMergeJob + 1, 0x20000);
            int nOfContainersInThisMergeJob = nOfContainersPrMergeJob;
            if (rest > 0) {
               rest--; nOfContainersInThisMergeJob++;
            }
            for (int j = 0; j < nOfContainersInThisMergeJob && cSubQuery < nOfSubdomains; j++) {
               char tfilena[1024];
               sprintf(tfilena, "TriangleFile_%d", cSubQuery);

               EDMexecution *e = subQueries->getElementp(cSubQuery++);
               nodeRvGetBoundaryOfLocalMesh *retVal = (nodeRvGetBoundaryOfLocalMesh *)e->returnValues;
               Container<EDMVD::Triangle> *triangleContainer = new(&ma)Container<EDMVD::Triangle>(&ma, retVal->triangle_array);
               mj->addContainer(triangleContainer);
               #ifdef _DEBUG
               writeTrianglesToFile(tfilena, triangleContainer);
               #endif
            }
         }

         #pragma omp parallel for
         for (int i = 0; i < nOfMergeJobs; i++) {
            mergeJobs[i]->merge();
         }
         for (int i = 0; i < nOfSubdomains; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            delete e->ema;
            e->ema = NULL;
         }
         MergeJob *lastMerge = new(&ma)MergeJob(&ma, nOfMergeJobs, 0x20000);
         for (int i = 0; i < nOfMergeJobs; i++) {
            lastMerge->addContainer(mergeJobs[i]->targetContainer);
         }
         lastMerge->merge();

         #ifdef _DEBUG
         writeTrianglesToFile("triangleFile.txt", lastMerge->targetContainer);
         #endif

         vector<VELaSSCoSM::Triangle> triangles;
         triangles.reserve(lastMerge->targetContainer->size());
         int nNodesInTriangle = 3;
         Container<EDMVD::Triangle> *resultContainer = lastMerge->targetContainer;

         VELaSSCoSM::Triangle vt;
         for (EDMVD::Triangle *t = resultContainer->firstp(); t; t = resultContainer->nextp()) {
            vector<VELaSSCoSM::NodeID> nodes;
            nodes.reserve(nNodesInTriangle);
            for (int i = 0; i < nNodesInTriangle; i++) {
               nodes.push_back(t->node_ids[i]);
            }
            vt.__set_nodes(nodes);
            triangles.push_back(vt);
         }

         endTime = GetTickCount();
         timeCollectingReturnValues = endTime - startTime;
         char msgBuffer[2048];
         string report;
         sprintf(msgBuffer, "\n\nExecution time subqueries: %d\nOfSubdomainsution time building return values: %d\n",
            timeSubQueryExecytion, timeCollectingReturnValues, tot_n_triangles, n_triangles_pr_string);
         printExecutionReport(report); report += msgBuffer;

         rv.__set_status("OK");
         rv.__set_report(report);
         rv.__set_elements(triangles);
         //OLI
         //rv.__set_triangle_record_size(sizeof(EDMVD::Triangle));
         //rv.__set_nTriangles(tot_n_triangles);
         //rv.__set_triangles(triangle_strings);
      }
   }
}
/*=============================================================================================================================*/
void VELaSSCoMethods::GetListOfMeshes(rvGetListOfMeshes& rv, const std::string& analysisID, const double timeStep)
/*=============================================================================================================================*/
{
   int startTime, endTime;
   if (subQueries) {
      EDMULONG nOfSubdomains = subQueries->size();
      nodeInGetListOfMeshes *inParams = new(&ma)nodeInGetListOfMeshes(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(timeStep);
      bool errorFound = false;

      startTime = GetTickCount();
#pragma omp parallel for
      for (int i = 0; i < nOfSubdomains; i++) {
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
      for (int i = 0; i < nOfSubdomains; i++) {
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

         for (int i = 0; i < nOfSubdomains; i++) {
            int meshNumber = 1;
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetListOfMeshes *retVal = (nodeRvGetListOfMeshes *)e->returnValues;
            CMemoryAllocator resMa(retVal->mesh_info_list, false);

            relocateMeshInfo *mesh_info_relocator = (relocateMeshInfo *)resMa.getRelocateInfo();
            resMa.prepareForRelocationAfterTransfer();
            Container<EDMVD::MeshInfo> *mesh_info_container = (Container<EDMVD::MeshInfo>*)mesh_info_relocator->bufferInfo->relocatePointer((char*)mesh_info_relocator->meshes);
            mesh_info_container->relocateStructContainer(mesh_info_relocator->bufferInfo);

            for (EDMVD::MeshInfo *mi = mesh_info_container->firstp(); mi; mi = mesh_info_container->nextp()) {
               mi->relocateThis(mesh_info_relocator->bufferInfo);
               vector<VELaSSCoSM::MeshInfo>::iterator meshInfoIter;
               for (meshInfoIter = meshInfos.begin(); meshInfoIter != meshInfos.end(); meshInfoIter++) {
                  if (strEQL(mi->name, meshInfoIter->name.data()))
                     break;
               }
               if (meshInfoIter == meshInfos.end()) {
                  VELaSSCoSM::MeshInfo meshInfo;
                  meshInfo.__set_name(mi->name); meshInfo.__set_nElements(mi->nElements);  meshInfo.__set_nVertices(mi->nVertices);
                  VELaSSCoSM::ElementType elementType;
                  //elementType.__set_shape(mi->elementType.shape);
                  elementType.__set_num_nodes(mi->nVertices);
                  meshInfo.__set_elementType(elementType);
                  meshInfo.__set_meshNumber(meshNumber++);

                  meshInfos.push_back(meshInfo);
               }
            }
         }
         string report;
         printExecutionReport(report);
         rv.__set_report(report);
         rv.__set_status("OK");
         rv.__set_meshInfos(meshInfos);
      }
   }
}
/*=============================================================================================================================*/
void VELaSSCoMethods::InjectFileSequence(Container<char*> *FileNameFormats, int FirstModelNo, int LastModelNo,
   char *EDMmodelNameFormat, Container<char*> *msgs)
/*=============================================================================================================================*/
{
   int startTime, endTime;
   if (subQueries) {
      EDMULONG nOfSubdomains = subQueries->size();
      bool errorFound = false;
      for (EDMexecution *e = subQueries->firstp(); e; e = subQueries->nextp()) {
         nodeInInjectFiles *inParams = new(&ma)nodeInInjectFiles(&ma, NULL);
         int i = 0, cModelno = FirstModelNo;
         for (char *fnf = FileNameFormats->first(); fnf && i < MAX_INJECT_FILES && cModelno <= LastModelNo; fnf = FileNameFormats->next()) {
            char fnbuf[2048];
            sprintf(fnbuf, fnf, e->modelNumber);
            inParams->attrPointerArr[i++]->putString(ma.allocString(fnbuf));
         }
         nodeRvInjectFiles *retVal = new(e->ema)nodeRvInjectFiles(e->ema, NULL);
         e->returnValues = retVal; e->inParams = inParams;
      }
      EDMexecution *e;

      startTime = GetTickCount();
#pragma omp parallel for private (e)
      for (int i = 0; i < nOfSubdomains; i++) {
         e = subQueries->getElementp(i);
         ExecuteRemoteCppMethod(e, "InjectFEMfiles", e->inParams, &errorFound);
#pragma omp critical
         msgs->add(ma.allocString(((nodeRvInjectFiles *)e->returnValues)->report->value.stringVal));
      }
      endTime = GetTickCount();
      char t[1024];
      sprintf(t, "   Time used %d millisec", endTime - startTime);
      msgs->add(ma.allocString(t));
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