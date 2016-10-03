
#include "stdafx.h"
#include "VELaSSCoMethods.h"
#include "../edm_plugin/EDM_plugin_1.h"
#include "VELaSSCo_utils.h"
#include "../../AccessLib/AccessLib/BoundaryBinaryMesh.h"
#include <omp.h>



//#define PRAGMA_OMP_PARALLEL_FOR #pragma omp parallel for
#define PARALLEL 1
//#define #pragma omp parallel for


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
            int QstartTime = GetTickCount();
            EDMexecution *e = subQueries->getElementp(i);
            nodervGetListOfAnalyses *retVal = new(e->ema)nodervGetListOfAnalyses(e->ema, NULL);
            e->returnValues = retVal;
            ExecuteRemoteCppMethod(e, "GetListOfAnalyses", NULL, &errorFound);
            int QendTime = GetTickCount();
            printf("GetListOfAnalyses, nSubQuery=%llu, i=%d, e->modelName=%s, time=%d\n", nOfSubdomains, i, e ? e->modelName : "EDMexecution e is NULL", QendTime - QstartTime);
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

void VELaSSCoMethods::setResults(rvGetListOfTimeSteps& rv, Container<double> *allTimeSteps)
{
   vector<double>  time_steps;
   for (double *ts = allTimeSteps->firstp(); allTimeSteps->moreElements(); ts = allTimeSteps->nextp()) {
      time_steps.push_back(*ts);
   }
   rv.__set_time_steps(time_steps);
}

/*=============================================================================================================================*/
Container<double> *VELaSSCoMethods::GetListOfTimeSteps(rvGetListOfTimeSteps& rv, const std::string& analysisID)
/*=============================================================================================================================*/
{
   int startTime = GetTickCount();
   Container<double> *allTimeSteps = NULL;
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
         return NULL;
      }
      nodeRvGetListOfTimeSteps *retValueWithError = NULL;
      
      allTimeSteps = new(&ma)Container<double>(&ma, 20);

      for (int i = 0; i < nOfSubdomains; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetListOfTimeSteps *retVal = (nodeRvGetListOfTimeSteps *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         Container<double> timeStepList(&ma, retVal->ListOfTimeSteps);
         for (double *ts = timeStepList.firstp(); timeStepList.moreElements(); ts = timeStepList.nextp()) {
            bool found = false;
            for (double *ats = allTimeSteps->firstp(); allTimeSteps->moreElements(); ats = allTimeSteps->nextp()) {
               if (*ats == *ts ) {
                  found = true; break;
               }
            }
            if (! found) allTimeSteps->add(*ts);
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
         setResults(rv, allTimeSteps);
      }
   }
   int endTime = GetTickCount();
   return allTimeSteps;
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
void VELaSSCoMethods::calculateBoundingBox(const std::string &analysisID, const int numSteps,
   const double *lstSteps, const int64_t numVertexIDs, const int64_t *lstVertexIDs, double *return_bbox, std::string *return_error_str)
/*=============================================================================================================================*/
{
   int startTime, endTime, timeSubQueryExecytion, t2, t3;
   if (subQueries) {
      EDMULONG nOfSubdomains = subQueries->size();
      nodeInCalculateBoundingBox *inParams = new(&ma)nodeInCalculateBoundingBox(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeSteps->putRealAggr((double *)lstSteps, numSteps);
      inParams->vertexIDs->putIntegerAggr((EDMLONG *)lstVertexIDs, (EDMLONG)numVertexIDs);
      bool errorFound = false;

      startTime = GetTickCount();

#pragma omp parallel for 
      for (int i = 0; i < nOfSubdomains; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         if (!e) {
            int asdfsdg = 9999;
         }
         nodeRvCalculateBoundingBox *retVal = new(e->ema)nodeRvCalculateBoundingBox(e->ema, NULL);
         e->returnValues = retVal;
         ExecuteRemoteCppMethod(e, "CalculateBoundingBox", inParams, &errorFound);
      }
      t2 = GetTickCount();
      if (errorFound) {
         writeErrorMessageForSubQueries(*return_error_str);
         return;
      }
      nodeRvCalculateBoundingBox *retValueWithError = NULL;
      for (int i = 0; i < nOfSubdomains; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvCalculateBoundingBox *retVal = (nodeRvCalculateBoundingBox *)e->returnValues;
         if (strnNEQ(retVal->return_error_str->value.stringVal, "OK", 2)) {
            retValueWithError = retVal; break;
         }
      }
      t3 = GetTickCount();
      if (retValueWithError) {
         *return_error_str = retValueWithError->return_error_str->value.stringVal;
      } else {
         double *aggregated_bbox = (double *)ma.alloc(sizeof(double) * 6);
         aggregated_bbox[min_X] = aggregated_bbox[min_Y] = aggregated_bbox[min_Z] = DBL_MAX;
         aggregated_bbox[max_X] = aggregated_bbox[max_Y] = aggregated_bbox[max_Z] = DBL_MIN;
   
         for (int i = 0; i < nOfSubdomains; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvCalculateBoundingBox *retVal = (nodeRvCalculateBoundingBox *)e->returnValues;
            double *sub_bbox = retVal->return_bbox->value.realAggrVal;
            if (sub_bbox[min_X] < aggregated_bbox[min_X]) aggregated_bbox[min_X] = sub_bbox[min_X];
            if (sub_bbox[min_Y] < aggregated_bbox[min_Y]) aggregated_bbox[min_Y] = sub_bbox[min_Y];
            if (sub_bbox[min_Z] < aggregated_bbox[min_Z]) aggregated_bbox[min_Z] = sub_bbox[min_Z];
            if (sub_bbox[max_X] > aggregated_bbox[max_X]) aggregated_bbox[max_X] = sub_bbox[max_X];
            if (sub_bbox[max_Y] > aggregated_bbox[max_Y]) aggregated_bbox[max_Y] = sub_bbox[max_Y];
            if (sub_bbox[max_Z] > aggregated_bbox[max_Z]) aggregated_bbox[max_Z] = sub_bbox[max_Z];
         }
         *return_error_str = "";
         memmove(return_bbox, aggregated_bbox, sizeof(double) * 6);
         endTime = GetTickCount(); timeSubQueryExecytion = endTime - startTime;
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
               if (++nError < 100)
                  fprintf(triangleFile, "Merge error in VELaSSCoMethods::GetBoundaryOfLocalMesh.");
            }
         }
         p = t;
         fprintf(triangleFile, "%10llu%10llu%10llu%10llu%10llu%10llu\n", t->node_ids[0], t->node_ids[1], t->node_ids[2], t->node_ids_orig[0], t->node_ids_orig[1], t->node_ids_orig[2]);
      }
      fclose(triangleFile);
   }
}
/*=============================================================================================================================*/
void VELaSSCoMethods::calculateBoundaryOfLocalMesh(const int meshID, const std::string &elementType,
   const std::string &analysisID, const double stepValue, char *tempFileForNodeIds,
   std::string *return_binary_mesh, std::string *return_error_str)
   /*=============================================================================================================================*/
{
   int startTime, endTime, timeSubQueryExecytion, timeCollectingReturnValues;
   if (subQueries) {
      EDMULONG nOfSubdomains = subQueries->size();
      nodeInGetBoundaryOfLocalMesh *inParams = new(&ma)nodeInGetBoundaryOfLocalMesh(&ma, NULL);
      inParams->analysisID->putString((char*)analysisID.data());
      inParams->timeStep->putReal(stepValue);
      inParams->meshID->putString("Hallo world");
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
         writeErrorMessageForSubQueries(errorMsg); return_error_str->append(errorMsg);
         return;
      }
      endTime = GetTickCount(); timeSubQueryExecytion = endTime - startTime;
      nodeRvGetBoundaryOfLocalMesh *retValueWithError = NULL;

      EDMULONG tot_n_triangles = 0;
      for (int i = 0; i < nOfSubdomains; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvGetBoundaryOfLocalMesh *retVal = (nodeRvGetBoundaryOfLocalMesh *)e->returnValues;
         if (strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         tot_n_triangles += retVal->n_triangles->value.intVal;
      }
      if (retValueWithError) {
         return_error_str->append(retValueWithError->report->value.stringVal);
      } else {
         startTime = GetTickCount();
         EDMULONG n_triangles_pr_string = tot_n_triangles / 4;

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
               //writeTrianglesToFile(tfilena, triangleContainer);
            }
         }

#pragma omp parallel for
         for (int i = 0; i < nOfMergeJobs; i++) {
            mergeJobs[i]->merge();
         }
         MergeJob *lastMerge = new(&ma)MergeJob(&ma, nOfMergeJobs, 0x20000);
         for (int i = 0; i < nOfMergeJobs; i++) {
            lastMerge->addContainer(mergeJobs[i]->targetContainer);
         }
         lastMerge->merge();

         // Then coordinates for the nodes must be retrieved from the database.
         // Start by create an array of unique node ids
         EDMULONG maxID = 0, minID = 0xfffffffffffffff;
         Container<EDMVD::Triangle> *resultContainer = lastMerge->targetContainer;
         for (EDMVD::Triangle *t = resultContainer->firstp(); t; t = resultContainer->nextp()) {
            for (int i=0; i < 3; i++) {
               if (t->node_ids[i] > maxID) maxID = t->node_ids[i];
               if (t->node_ids[i] < minID) minID = t->node_ids[i];
            }
         }
         EDMULONG testBufSize = maxID - minID + 1;
         char *testBuf = (char*)ma.alloc(testBufSize);
         memset(testBuf, 0, testBufSize);
         for (EDMVD::Triangle *t = resultContainer->firstp(); t; t = resultContainer->nextp()) {
            for (int i=0; i < 3; i++) {
               testBuf[t->node_ids[i] - minID] = 1;
            }
         }
         EDMULONG nOfUniqueNodeIds = 0;
         for (int i = 0; i < testBufSize; i++) if (testBuf[i] == 1) nOfUniqueNodeIds++;
         EDMULONG *uniqueNodeIds = (EDMULONG *)ma.alloc(nOfUniqueNodeIds * sizeof(EDMULONG));
         EDMULONG nodeIx = 0;
         memset(testBuf, 0, testBufSize);
         for (EDMVD::Triangle *t = resultContainer->firstp(); t; t = resultContainer->nextp()) {
            for (int i=0; i < 3; i++) {
               if (testBuf[t->node_ids[i] - minID] == 0) {
                  uniqueNodeIds[nodeIx++] = t->node_ids[i]; testBuf[t->node_ids[i] - minID] = 1;
               }
            }
         }
         FILE *bbfp = fopen(tempFileForNodeIds , "wb");
         if (bbfp) {
            size_t nDoubleWritten = fwrite(uniqueNodeIds, sizeof(EDMULONG), nOfUniqueNodeIds, bbfp);
            fclose(bbfp);
         }

         nodeInGetNodeCoordinates *incoordinateParams = new(&ma)nodeInGetNodeCoordinates(&ma, NULL);
         incoordinateParams->nodeIdsFileName->putString(tempFileForNodeIds);
         incoordinateParams->nOfNodeIds->putInteger(nOfUniqueNodeIds);
         bool errorFound = false;

         startTime = GetTickCount();
         EDMLONG nOfPoints = 0;
         #pragma omp parallel for
         for (int i = 0; i < nOfSubdomains; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetNodeCoordinates *retVal = new(e->ema)nodeRvGetNodeCoordinates(e->ema, NULL);
            e->returnValues = retVal;
            ExecuteRemoteCppMethod(e, "GetNodeCoordinates", incoordinateParams, &errorFound);
            #pragma omp critical
            {
               nOfPoints += retVal->nOfNodesFound->value.intVal;
            }
         }
         if (errorFound) {
            string errorMsg;
            writeErrorMessageForSubQueries(errorMsg); return_error_str->append(errorMsg);
            return;
         }


         VELaSSCo::BoundaryBinaryMesh boundary_mesh;
         VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle *theTriangles = (VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle*)ma.alloc(lastMerge->targetContainer->size() * sizeof(VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle));
         VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle *prev, *vt = theTriangles;
         VELaSSCo::BoundaryBinaryMesh::MeshPoint *thePoints = (VELaSSCo::BoundaryBinaryMesh::MeshPoint*)ma.alloc(nOfPoints * sizeof(VELaSSCo::BoundaryBinaryMesh::MeshPoint));
         VELaSSCo::BoundaryBinaryMesh::MeshPoint *pt = thePoints;

         int nNodesInTriangle = 3;

         for (EDMVD::Triangle *t = resultContainer->firstp(); t; t = resultContainer->nextp()) {
            vt->_num_nodes = nNodesInTriangle;
            memcpy(vt->_nodes, t->node_ids_orig, sizeof(t->node_ids_orig));
            prev = vt; vt++;
         }

         EDMLONG nOfPointsReturned = 0;
         memset(testBuf, 0, testBufSize);
         for (int i = 0; i < nOfSubdomains; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetNodeCoordinates *retVal = (nodeRvGetNodeCoordinates *)e->returnValues;
            Container<EDMVD::Vertex> *vertices = new(&ma)Container<EDMVD::Vertex>(&ma, retVal->vertices);
            for (EDMVD::Vertex *v = vertices->firstp(); v; v = vertices->nextp()) {
               if (testBuf[v->id - minID] == 0) {
                  pt->_coords[0] = v->x; pt->_coords[1] = v->y; pt->_coords[2] = v->z;
                  pt->_id = v->id; nOfPointsReturned++;
                  testBuf[v->id - minID] = 1;
                  pt++;
               }
            }
         }
         //EDMLONG ntr = resultContainer->size();
         //vt = theTriangles;
         //for (int i=0; i < ntr; i++) {
         //    for (int k=0; k < 3; k++) {
         //       int j;
         //       pt = thePoints;
         //       for (j=0; j < nOfPointsReturned; j++) {
         //           if (vt->_nodes[k] == pt->_id)
         //               break;
         //           pt++;
         //       }
         //       if (j >= nOfPointsReturned) {
         //           EDMLONG errid = vt->_nodes[k];
         //           errid = 0;
         //       }
         //    }
         //    vt++;
         //}
         boundary_mesh.set(thePoints, nOfPointsReturned, theTriangles, resultContainer->size(), VELaSSCo::BoundaryBinaryMesh::NONE);
         *return_binary_mesh = boundary_mesh.toString();

         endTime = GetTickCount();
         timeCollectingReturnValues = endTime - startTime;
         char msgBuffer[2048];
         string report;
         sprintf(msgBuffer, "\n\nExecution time subqueries: %d\nOfSubdomainsution time building return values: %d\n",
            timeSubQueryExecytion, timeCollectingReturnValues, tot_n_triangles, n_triangles_pr_string);
         printExecutionReport(report); report += msgBuffer;

      }
   }
}

///*=============================================================================================================================*/
//bool VELaSSCoMethods::GetBoundaryOfLocalMesh(const std::string& analysisID, const double stepValue,
//   const int meshID, MergeJob **lastMergep, std::string *return_error_str)
///*=============================================================================================================================*/
//{
//   int startTime, endTime, timeSubQueryExecytion, timeCollectingReturnValues;
//   if (subQueries) {
//      EDMULONG nOfSubdomains = subQueries->size();
//      nodeInGetBoundaryOfLocalMesh *inParams = new(&ma)nodeInGetBoundaryOfLocalMesh(&ma, NULL);
//      inParams->analysisID->putString((char*)analysisID.data());
//      inParams->timeStep->putReal(stepValue);
//     // inParams->meshID->putString((char*)meshID.data());
//      bool errorFound = false;
//
//      startTime = GetTickCount();
//#pragma omp parallel for
//      for (int i = 0; i < nOfSubdomains; i++) {
//         EDMexecution *e = subQueries->getElementp(i);
//         nodeRvGetBoundaryOfLocalMesh *retVal = new(e->ema)nodeRvGetBoundaryOfLocalMesh(e->ema, NULL);
//         e->returnValues = retVal;
//         ExecuteRemoteCppMethod(e, "GetBoundaryOfSubMesh", inParams, &errorFound);
//      }
//      if (errorFound) {
//         string errorMsg;
//         writeErrorMessageForSubQueries(errorMsg); *return_error_str = errorMsg;
//         return false;
//      }
//      endTime = GetTickCount(); timeSubQueryExecytion = endTime - startTime;
//      nodeRvGetBoundaryOfLocalMesh *retValueWithError = NULL;
//
//      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
//      EDMULONG tot_n_triangles = 0;
//
//      for (int i = 0; i < nOfSubdomains; i++) {
//         EDMexecution *e = subQueries->getElementp(i);
//         nodeRvGetBoundaryOfLocalMesh *retVal = (nodeRvGetBoundaryOfLocalMesh *)e->returnValues;
//         if(strNEQ(retVal->status->value.stringVal, "OK")) {
//            retValueWithError = retVal; break;
//         }
//         tot_n_triangles += retVal->n_triangles->value.intVal;
//         //if (i == 0) {
//         //   nOfValuesPrVertex = retVal->nOfValuesPrVertex->value.intVal;
//         //} else if (nOfValuesPrVertex != retVal->nOfValuesPrVertex->value.intVal) {
//         //   THROW("Different number of values pr vertex in Subqueries in GetResultFromVerticesID.")
//         //}
//         //if (retVal->maxID->value.intVal > maxID) maxID = retVal->maxID->value.intVal;
//         //if (retVal->minID->value.intVal < minID) minID = retVal->minID->value.intVal;
//      }
//      if (retValueWithError) {
//         *return_error_str = retValueWithError->report->value.stringVal;
//         return false;
//      } else {
//         startTime = GetTickCount();
//         EDMULONG n_triangles_pr_string = tot_n_triangles / 4;
//         //std::vector<std::string> triangle_strings;
//         //EDMULONG n_triangles_in_this_string = n_triangles_pr_string;
//         //int stringNo = -1;
//         //std::string *cTriangleBuffer;
//         //char *bp;
//         int nOfCores = omp_get_max_threads();
//         int nOfMergeJobs = nOfCores;
//         int nOfContainersPrMergeJob = nOfSubdomains / nOfMergeJobs;
//         if (nOfContainersPrMergeJob < 2) {
//            nOfContainersPrMergeJob = 2; nOfMergeJobs = nOfSubdomains / 2;
//         }
//         int rest = nOfSubdomains % nOfMergeJobs;
//
//         MergeJob **mergeJobs = (MergeJob **)ma.alloc(nOfMergeJobs * sizeof(MergeJob *));
//         int cSubQuery = 0;
//
//         for (int i = 0; i < nOfMergeJobs; i++) {
//            MergeJob *mj = mergeJobs[i] = new(&ma)MergeJob(&ma, nOfContainersPrMergeJob + 1, 0x20000);
//            int nOfContainersInThisMergeJob = nOfContainersPrMergeJob;
//            if (rest > 0) {
//               rest--; nOfContainersInThisMergeJob++;
//            }
//            for (int j = 0; j < nOfContainersInThisMergeJob && cSubQuery < nOfSubdomains; j++) {
//               char tfilena[1024];
//               sprintf(tfilena, "TriangleFile_%d", cSubQuery);
//
//               EDMexecution *e = subQueries->getElementp(cSubQuery++);
//               nodeRvGetBoundaryOfLocalMesh *retVal = (nodeRvGetBoundaryOfLocalMesh *)e->returnValues;
//               Container<EDMVD::Triangle> *triangleContainer = new(&ma)Container<EDMVD::Triangle>(&ma, retVal->triangle_array);
//               mj->addContainer(triangleContainer);
//               //#ifdef _DEBUG
//               //writeTrianglesToFile(tfilena, triangleContainer);
//               //#endif
//            }
//         }
//
//         #pragma omp parallel for
//         for (int i = 0; i < nOfMergeJobs; i++) {
//            mergeJobs[i]->merge();
//         }
//         for (int i = 0; i < nOfSubdomains; i++) {
//            EDMexecution *e = subQueries->getElementp(i);
//            delete e->ema;
//            e->ema = NULL;
//         }
//         MergeJob *lastMerge = new(&ma)MergeJob(&ma, nOfMergeJobs, 0x20000);
//         for (int i = 0; i < nOfMergeJobs; i++) {
//            lastMerge->addContainer(mergeJobs[i]->targetContainer);
//         }
//         lastMerge->merge();
//
//         //#ifdef _DEBUG
//         //writeTrianglesToFile("triangleFile.txt", lastMerge->targetContainer);
//         //#endif
//
//         //vector<VELaSSCoSM::Triangle> triangles;
//         //triangles.reserve(lastMerge->targetContainer->size());
//         //int nNodesInTriangle = 3;
//         //Container<EDMVD::Triangle> *resultContainer = lastMerge->targetContainer;
//
//         //VELaSSCoSM::Triangle vt;
//         //for (EDMVD::Triangle *t = resultContainer->firstp(); t; t = resultContainer->nextp()) {
//         //   vector<VELaSSCoSM::NodeID> nodes;
//         //   nodes.reserve(nNodesInTriangle);
//         //   for (int i = 0; i < nNodesInTriangle; i++) {
//         //      nodes.push_back(t->node_ids[i]);
//         //   }
//         //   vt.__set_nodes(nodes);
//         //   triangles.push_back(vt);
//         //}
//
//         endTime = GetTickCount();
//         timeCollectingReturnValues = endTime - startTime;
//         //char msgBuffer[2048];
//         //string report;
//         //sprintf(msgBuffer, "\n\nExecution time subqueries: %d\nOfSubdomainsution time building return values: %d\n",
//         //   timeSubQueryExecytion, timeCollectingReturnValues, tot_n_triangles, n_triangles_pr_string);
//         //printExecutionReport(report); report += msgBuffer;
//
//         //rv.__set_status("OK");
//         //rv.__set_report(report);
//         //rv.__set_elements(triangles);
//         //OLI
//         //rv.__set_triangle_record_size(sizeof(EDMVD::Triangle));
//         //rv.__set_nTriangles(tot_n_triangles);
//         //rv.__set_triangles(triangle_strings);
//
//         return true;
//
//      }
//   }
//   return false;
//}
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
         Container<EDMVD::MeshInfoEDM> *aggregated_mesh_info = new(&ma)Container<EDMVD::MeshInfoEDM>(&ma);

         for (int i = 0; i < nOfSubdomains; i++) {
            int meshNumber = 1;
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvGetListOfMeshes *retVal = (nodeRvGetListOfMeshes *)e->returnValues;
            CMemoryAllocator resMa(retVal->mesh_info_list, false);

            relocateMeshInfo *mesh_info_relocator = (relocateMeshInfo *)resMa.getRelocateInfo();
            resMa.prepareForRelocationAfterTransfer();
            Container<EDMVD::MeshInfoEDM> *mesh_info_container = (Container<EDMVD::MeshInfoEDM>*)mesh_info_relocator->bufferInfo->relocatePointer((char*)mesh_info_relocator->meshes);
            mesh_info_container->relocateStructContainer(mesh_info_relocator->bufferInfo);

            for (EDMVD::MeshInfoEDM *mi = mesh_info_container->firstp(); mi; mi = mesh_info_container->nextp()) {
               mi->relocateThis(mesh_info_relocator->bufferInfo);
               EDMVD::MeshInfoEDM *aggrmi;
               for (aggrmi = aggregated_mesh_info->firstp(); aggrmi; aggrmi = aggregated_mesh_info->nextp()) {
                  if (strEQL(mi->name, aggrmi->name))
                     break;
               }
               if (aggrmi == NULL) {
                  aggrmi = aggregated_mesh_info->createNext(); aggrmi->name = mi->name;
                  aggrmi->meshUnits = mi->meshUnits;
                  aggrmi->meshColor = mi->meshColor;
                  aggrmi->coordsName = mi->coordsName;
                  aggrmi->nElements = aggrmi->nVertices = 0;
               }
               aggrmi->nElements += mi->nElements;
               aggrmi->nVertices += mi->nVertices;
            }
         }
         int meshNumber = 1;
         for (EDMVD::MeshInfoEDM *mi = aggregated_mesh_info->firstp(); mi; mi = aggregated_mesh_info->nextp()) {
            VELaSSCoSM::MeshInfo meshInfo;
            meshInfo.__set_name(mi->name); meshInfo.__set_nElements(mi->nElements);  meshInfo.__set_nVertices(mi->nVertices);
            VELaSSCoSM::ElementType elementType;
            //elementType.__set_shape(mi->elementType.shape);
            elementType.__set_shape(VELaSSCoSM::ElementShapeType::TetrahedraElement);
            elementType.__set_num_nodes(mi->nVertices);
            meshInfo.__set_elementType(elementType);
            meshInfo.__set_meshNumber(meshNumber++);
            meshInfo.__set_nElements(mi->nElements);

            meshInfos.push_back(meshInfo);
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
#ifndef NOT_NESTED

      EDMULONG nOfSubdomains = subQueries->size();
      bool errorFound = false;

      omp_set_num_threads(32);
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

      startTime = GetTickCount();

#pragma omp parallel for
      for (int i = 0; i < nOfSubdomains; i++) {
         int nt = omp_get_num_threads();
         EDMexecution *e = subQueries->getElementp(i);
         ExecuteRemoteCppMethod(e, "InjectFEMfiles", e->inParams, &errorFound);
#pragma omp critical
         if (! errorFound) {
            msgs->add(ma.allocString(((nodeRvInjectFiles *)e->returnValues)->report->value.stringVal));
         }
      }

#else

      bool errorFound = false;
      int nServers = nodeQueries->size();

      omp_set_nested(1); omp_set_dynamic(1); omp_set_num_threads(128);

      for (Container<EDMexecution> *jobList = nodeQueries->first(); jobList; jobList = nodeQueries->next()) {
         for (EDMexecution *cJob = jobList->firstp(); cJob; cJob = jobList->nextp()) {
            nodeInInjectFiles *inParams = new(&ma)nodeInInjectFiles(&ma, NULL);
            int i = 0, cModelno = FirstModelNo;
            for (char *fnf = FileNameFormats->first(); fnf && i < MAX_INJECT_FILES && cModelno <= LastModelNo; fnf = FileNameFormats->next()) {
               char fnbuf[2048];
               sprintf(fnbuf, fnf, cJob->modelNumber);
               inParams->attrPointerArr[i++]->putString(ma.allocString(fnbuf));
            }
            nodeRvInjectFiles *retVal = new(cJob->ema)nodeRvInjectFiles(cJob->ema, NULL);
            cJob->returnValues = retVal; cJob->inParams = inParams;
         }
      }

      startTime = GetTickCount();
      #pragma omp parallel num_threads(nServers)
      #pragma omp parallel for
      for (int i=0; i < nServers; i++) {
         Container<EDMexecution> *jobList = nodeQueries->getElement(i);
         EDMexecution *cJob, *firstJob = jobList->firstp();
         if (firstJob) {
            int nAppservers = firstJob->theEDMserver->nAppservers;
            #pragma omp parallel num_threads(nAppservers)
            #pragma omp parallel for
            for (int j=0; j < nAppservers; j++) {
               cJob = jobList->getElementp(j);
               ExecuteRemoteCppMethod(cJob, "InjectFEMfiles", cJob->inParams, &errorFound);
               #pragma omp critical
               if (! errorFound) {
                  msgs->add(ma.allocString(((nodeRvInjectFiles *)cJob->returnValues)->report->value.stringVal));
               }
            }
         }
      }

#endif
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
#ifdef _WINDOWS
   return "edm_plugin_1";
#else
   return "libedm_plugin_1.so";
#endif
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