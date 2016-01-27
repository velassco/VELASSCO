// EDM_server_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "EDM_server_dll.h"




CMemoryAllocator dllMa(0x100000);
static CMemoryAllocator *resultInfoMemory = NULL;

/*=============================================================================================================================*/
char *handleError(CedmError *e)
/*=============================================================================================================================*/
{
   char *errMsg;
   if (e->message) {
      errMsg = e->message;
   } else {
      errMsg = edmiGetErrorText(e->rstat);
   }
   char buf[20480];
   sprintf(buf, "Error: %s - file: %s - line %lld", errMsg, e->fileName, e->lineNo);
   printf("\n\n%s\n\n", buf);
   return dllMa.allocString(buf);
}

/*===============================================================================================*/
EDMVD::ModelType getModelType(SdaiModel sdaiModelID)
/*===============================================================================================*/
{
   ModelType mType = mtUnknown;

   char *schemaName = NULL, *modelName = NULL;
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance), index = 0, numberOfHits = 1;
   SdaiInstance schemaID, edmModelID;

   void *sp = sdaiGetAttrBN(sdaiModelID, "edm_model", sdaiINSTANCE, &edmModelID);
   if (sp) {
      sp = sdaiGetAttrBN(edmModelID, "schema", sdaiINSTANCE, &schemaID);
      if (sp) {
         sp = sdaiGetAttrBN(sdaiModelID, "name", sdaiSTRING, &modelName);
         if (sp) {
            sp = sdaiGetAttrBN(schemaID, "name", sdaiSTRING, &schemaName);
            SdaiErrorCode rk = sdaiErrorQuery();
            if (sp && strEQL(schemaName, "DEM_SCHEMA_VELASSCO")) {
               mType = mtDEM;
            } else if (sp && strEQL(schemaName, "FEM_SCHEMA_VELASSCO")) {
               mType = mtFEM;
            }
         } 
         if (modelName) edmiFree(modelName);
         if (schemaName) edmiFree(schemaName);
      }
   }
   return mType;
}

/*===================================================================================================================*/
EDMLONG GetListOfAnalyses(Model *theModel, ModelType mt, nodervGetListOfAnalyses *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;

   try {
      char* np;

      Container<char*> *nameList = new(&dllMa)Container<char*>(&dllMa);
      if (mt == mtDEM) {
         Iterator<dem::Simulation*, dem::entityType> simIter(theModel->getObjectSet(dem::et_Simulation), theModel);
         for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
            nameList->add(dllMa.allocString(s->get_name()));
         }
         retVal->analysis_name_list->putStringContainer(nameList);
      } else if (mt == mtFEM) {
         Iterator<fem::ResultHeader*, fem::entityType> rhIter(theModel->getObjectSet(fem::et_ResultHeader), theModel);
         for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
            char* anName = rh->get_analysis();
            for (np = nameList->first(); np && strNEQ(anName, np); np = nameList->next());
            if (np == NULL) {
               nameList->add(dllMa.allocString(anName));
            }
         }
         retVal->analysis_name_list->putStringContainer(nameList);
      } else {
         emsg = "Model does not exist.";
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
      retVal->analysis_name_list->type = rptINTEGER;
   } else {
      retVal->status->putString("OK");
      retVal->report->type = rptINTEGER;
   }
   return rstat;
}

/*===================================================================================================================*/
EDMLONG GetListOfTimeSteps(Model *theModel, ModelType mt, nodeInGetListOfTimeSteps *inParam, nodeRvGetListOfTimeSteps *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;

   try {
      char *anid = inParam->analysisID->value.stringVal;
      Container<double> timeSteps(&dllMa);

      if (mt == mtDEM) {
         Iterator<dem::Simulation*, dem::entityType> simIter(theModel->getObjectSet(dem::et_Simulation), theModel);
         for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
            if (strEQL(s->get_name(), anid)) {
               Iterator<dem::Timestep*, dem::entityType> tsIter(s->get_consists_of(), theModel);
               for (dem::Timestep *ts = tsIter.first(); ts; ts = tsIter.next()) {
                  timeSteps.add(ts->get_time_value());
               }
            }
         }
         retVal->ListOfTimeSteps->putContainer(&timeSteps);
      } else {
         Iterator<fem::ResultHeader*, fem::entityType> rhIter(theModel->getObjectSet(fem::et_ResultHeader), theModel);
         for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
            if (strEQL(rh->get_analysis(), anid)) {
               timeSteps.add(rh->get_step());
            }
         }
         retVal->ListOfTimeSteps->putContainer(&timeSteps);
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
      retVal->ListOfTimeSteps->type = rptINTEGER;
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}

/*===================================================================================================================*/
EDMLONG GetListOfVerticesFromMesh(Model *theModel, ModelType mt, nodeInGetListOfVerticesFromMesh *inParam, nodeRvGetListOfVerticesFromMesh *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;

   try {
      startTime = GetTickCount();
      double xx = 1.;
      //for (EDMLONG i = 0; i < 0x100000000; i++) {
      //   xx = xx * xx;
      //}
      endTime = GetTickCount();
      printf("\n\nElapsed time for parallel execution is %d milliseconds\n\n", endTime - startTime);
      char *anid = inParam->analysisID->value.stringVal;
      Container<EDMVD::Vertex> vertices(&dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;

      if (mt == mtDEM) {
         //Iterator<dem::Simulation*, dem::entityType> simIter(theModel->getObjectSet(dem::et_Simulation), theModel);
         //for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
         //   if (anid && strEQL(s->get_name(), anid)) {
         //      Iterator<dem::Timestep*, dem::entityType> tsIter(s->get_consists_of(), theModel);
         //      for (dem::Timestep *ts = tsIter.first(); ts; ts = tsIter.next()) {
         //         timeSteps.add(ts->get_time_value());
         //      }
         //   }
         //}
         //retVal->ListOfTimeSteps->putContainer(&timeSteps);
      } else {
         Iterator<fem::Node*, fem::entityType> nodeIter(theModel->getObjectSet(fem::et_Node), theModel);
         for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
            EDMVD::Vertex *v = vertices.createNext();
            v->id = n->get_id(); v->x = n->get_x(); v->y = n->get_y(); v->z = n->get_z();
            if (v->id > maxID) maxID = v->id;
            if (v->id < minID) minID = v->id;
         }
         retVal->vertices->putContainer(&vertices);
         retVal->maxID->putInteger(maxID); retVal->minID->putInteger(minID);
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
      retVal->vertices->type = rptINTEGER;
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}
/*===================================================================================================================*/
fem::Analysis *getFEManalysis(char *anid, Iterator<fem::Analysis*, fem::entityType> *iterp)
/*===================================================================================================================*/
{
   fem::Analysis *cAnalysis;
   for (cAnalysis = iterp->first(); cAnalysis; cAnalysis = iterp->next()) {
      char *an = cAnalysis->get_name();
      if (an && strEQL(an, anid)) {
         return cAnalysis;
      }
   }
   return NULL;
}
static char *LocationTypes[] = { "OnNodes", "OnGaussPoints" };
static char *ValueTypes[] = { "Scalar", "Vector", "Matrix", "PlainDeformationMatrix", "MainMatrix", "LocalAxes", "ComplexScalar", "ComplexVector" };
/*===================================================================================================================*/
EDMLONG GetListOfResultsFromTimeStepAndAnalysis(Model *theModel, ModelType mt, nodeInGetListOfResultsFromTimeStepAndAnalysis *inParam,
   nodeRvGetListOfResultsFromTimeStepAndAnalysis *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;

   try {
      startTime = GetTickCount();


      char *anid = inParam->analysisID->value.stringVal;
      Container<EDMVD::Vertex> vertices(&dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;

      if (!resultInfoMemory) resultInfoMemory = new CMemoryAllocator(0x100000);
      if (mt == mtDEM) {
         emsg = "GetListOfResultsFromTimeStepAndAnalysis is not implemented for DEM models.";
      } else {
         // The iterator must declared here because the object found by getFEManalysis is allocated in memory of the iterator
         Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
         fem::Analysis *cAnalysis = getFEManalysis(anid, &analysisIter);
         if (cAnalysis) {
            resultInfoMemory->freeAllMemory();
            relocateResultInfo *relocateInfo = (relocateResultInfo *)resultInfoMemory->createRelocateInfo(sizeof(relocateResultInfo));
            Container<EDMVD::ResultInfo> *resInfo = new(resultInfoMemory)Container<EDMVD::ResultInfo>(resultInfoMemory, 16);
            relocateInfo->sResults = resInfo;

            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps(), theModel);
            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->stepValue->value.realVal) {
                  Iterator<fem::ResultHeader*, fem::entityType> resultIter(ts->get_results(), theModel);
                  for (fem::ResultHeader *rh = resultIter.first(); rh; rh = resultIter.next()) {
                     ResultInfo *ri = resInfo->createNext();
                     ri->name = resultInfoMemory->allocString(rh->get_name());
                     ri->type = resultInfoMemory->allocString(rh->exists_rType() ? ValueTypes[rh->get_rType()] : "");
                     ri->location = resultInfoMemory->allocString(rh->exists_location() ? LocationTypes[rh->get_location()] : "");
                     ri->gaussPointName = resultInfoMemory->allocString(rh->exists_gpName() ? rh->get_gpName() : "");
                     ri->coordinatesName = resultInfoMemory->allocString("");
                     ri->units = resultInfoMemory->allocString("");
                     ri->resultNumber = 0;
                     sdaiErrorQuery();
                     ri->componentNames = new(resultInfoMemory)Container<char*>(resultInfoMemory);
                     SdaiAggr cnAID = rh->get_compName_aggrId();
                     ri->nOfComponents = sdaiGetMemberCount(cnAID);
                     for (int ci = 0; ci < ri->nOfComponents; ci++) {
                        char *cn;
                        if (!edmiGetAggrElement(cnAID, ci, sdaiSTRING, &cn)) CHECK(sdaiErrorQuery());
                        ri->componentNames->add(resultInfoMemory->allocString(cn));
                        if (cn) edmiFree(cn);
                     }
                  }
               }
            }
            // Add info about the memory blocks within one memory block
            resultInfoMemory->prepareForRelocationBeforeTransfer();
            // Link the memory allocator to the return value.
            retVal->ResultInfoList->putCMemoryAllocator(resultInfoMemory);
         } else {
            emsg = "Analysis with specified name does not exist.";
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error3"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}


/*===================================================================================================================*/
EDMLONG GetResultFromVerticesID(Model *theModel, ModelType mt, nodeInGetResultFromVerticesID *inParam,
   nodeRvGetResultFromVerticesID *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;

   try {
      startTime = GetTickCount();
      bool timeStepFound = false, resultIdFound = false;

      char *analysisID = inParam->analysisID->value.stringVal;
      char *resultID = inParam->resultID->value.stringVal;
      Container<EDMVD::Vertex> vertices(&dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;

      if (!resultInfoMemory) resultInfoMemory = new CMemoryAllocator(0x100000);
      if (mt == mtDEM) {
         emsg = "GetListOfResultsFromTimeStepAndAnalysis is not implemented for DEM models.";
      } else {
         Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
         fem::Analysis *cAnalysis = getFEManalysis(analysisID, &analysisIter);
         if (cAnalysis) {
            resultInfoMemory->freeAllMemory();
            relocateResultOnVertex *relocateInfo = (relocateResultOnVertex *)resultInfoMemory->createRelocateInfo(sizeof(relocateResultOnVertex));
            Container<EDMVD::ResultOnVertex> *resultsOnVertices = new(resultInfoMemory)Container<EDMVD::ResultOnVertex>(resultInfoMemory, 1024);
            relocateInfo->vertexResults = resultsOnVertices;

            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps(), theModel);
            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->timeStep->value.realVal) {
                  timeStepFound = true;
                  Iterator<fem::ResultHeader*, fem::entityType> resultHeaderIter(ts->get_results(), theModel);
                  for (fem::ResultHeader *rh = resultHeaderIter.first(); rh; rh = resultHeaderIter.next()) {
                     char *rhname = rh->get_name();
                     if (rhname && strEQL(rhname, resultID)) {
                        resultIdFound = true;
                        int nOfValues = 3;
                        Iterator<fem::Result*, fem::entityType> resultIter(rh->get_values(), theModel);
                        fem::entityType resType;
                        for (void *vres = resultIter.first(&resType); vres; vres = resultIter.next(&resType)) {
                           ResultOnVertex *rov = resultsOnVertices->createNext();
                           rov->value = new(resultInfoMemory)Container<double>(resultInfoMemory, nOfValues);
                           fem::Result *res;
                           if (resType == fem::et_ScalarResult) {
                              fem::ScalarResult *scalar = (ScalarResult *)vres; res = scalar;
                              rov->value->add(scalar->get_val()); nOfValues = 1;
                           } else if (resType == fem::et_VectorResult) {
                              fem::VectorResult *vector = (VectorResult *)vres; res = vector;
                              SdaiAggr cnAID = vector->get_values_aggrId();
                              nOfValues = sdaiGetMemberCount(cnAID);
                              for (int vi = 0; vi < nOfValues; vi++) {
                                 double d;
                                 if (!edmiGetAggrElement(cnAID, vi, sdaiREAL, &d)) CHECK(sdaiErrorQuery());
                                 rov->value->add(d);
                              }
                           } else {
                              emsg = "Resulttype not supported in GetResultFromVerticesID.";
                           }
                           fem::Node *node = res->get_result_for();
                           rov->id = node ? node->get_id() : 0;
                           if (rov->id > maxID) maxID = rov->id;
                           if (rov->id < minID) minID = rov->id;
                        }
                     }
                  }
               }
            }
            retVal->maxID->putInteger(maxID); retVal->minID->putInteger(minID);
            // Add info about the memory blocks within one memory block
            resultInfoMemory->prepareForRelocationBeforeTransfer();
            // Link the memory allocator to the return value.
            retVal->result_list->putCMemoryAllocator(resultInfoMemory);
         } else {
            emsg = "Analysis with specified name does not exist.";
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error4"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}

extern "C" EDMLONG __declspec(dllexport) dll_main(char *repositoryName, char *modelName, char *methodName,
   EDMLONG nOfParameters, cppRemoteParameter *parameters, EDMLONG nOfReturnValues, cppRemoteParameter *returnValues)
{
   EdmiError rstat = OK;

   try {
      Database VELaSSCo_db("", "", "");
      Repository VELaSSCo_Repository(&VELaSSCo_db, repositoryName);
      Model VELaSSCo_model(&VELaSSCo_Repository, &dllMa, NULL);
      VELaSSCo_model.open(modelName, sdaiRO);
      ModelType vmt = getModelType(VELaSSCo_model.modelId);
      VELaSSCo_model.defineSchema(vmt == mtFEM ? (dbSchema*)&fem_schema_velassco_SchemaObject : (dbSchema*)&dem_schema_velassco_SchemaObject);

      if (strEQL(methodName, "GetListOfAnalyses")) {
         nodervGetListOfAnalyses *results = new(&dllMa)nodervGetListOfAnalyses(NULL, returnValues);
         if (nOfParameters != 0 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->analysis_name_list->type = rptUndefined;
         } else {
            rstat = GetListOfAnalyses(&VELaSSCo_model, vmt, results);
         }
      } else if (strEQL(methodName, "GetListOfTimeSteps")) {
         nodeRvGetListOfTimeSteps *results = new(&dllMa)nodeRvGetListOfTimeSteps(NULL, returnValues);
         nodeInGetListOfTimeSteps *inParams = new(&dllMa)nodeInGetListOfTimeSteps(NULL, parameters);
         if (nOfParameters != 1 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->ListOfTimeSteps->type = rptUndefined;
         } else {
            rstat = GetListOfTimeSteps(&VELaSSCo_model, vmt, inParams, results);
         }
      } else if (strEQL(methodName, "GetListOfVerticesFromMesh")) {
         nodeRvGetListOfVerticesFromMesh *results = new(&dllMa)nodeRvGetListOfVerticesFromMesh(NULL, returnValues);
         nodeInGetListOfVerticesFromMesh *inParams = new(&dllMa)nodeInGetListOfVerticesFromMesh(NULL, parameters);
         if (nOfParameters != 3 || nOfReturnValues != 5) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->vertices->type = rptUndefined;
         } else {
            rstat = GetListOfVerticesFromMesh(&VELaSSCo_model, vmt, inParams, results);
         }
      } else if (strEQL(methodName, "GetListOfResultsFromTimeStepAndAnalysis")) {
         nodeRvGetListOfResultsFromTimeStepAndAnalysis *results = new(&dllMa)nodeRvGetListOfResultsFromTimeStepAndAnalysis(NULL, returnValues);
         nodeInGetListOfResultsFromTimeStepAndAnalysis *inParams = new(&dllMa)nodeInGetListOfResultsFromTimeStepAndAnalysis(NULL, parameters);
         if (nOfParameters != 2 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->ResultInfoList->type = rptUndefined;
         } else {
            rstat = GetListOfResultsFromTimeStepAndAnalysis(&VELaSSCo_model, vmt, inParams, results);
         }
      } else if (strEQL(methodName, "GetResultFromVerticesID")) {
         nodeRvGetResultFromVerticesID *results = new(&dllMa)nodeRvGetResultFromVerticesID(NULL, returnValues);
         nodeInGetResultFromVerticesID *inParams = new(&dllMa)nodeInGetResultFromVerticesID(NULL, parameters);
         if (nOfParameters != 3 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->result_list->type = rptUndefined;
         } else {
            rstat = GetResultFromVerticesID(&VELaSSCo_model, vmt, inParams, results);
         }
      }
   } catch (CedmError *e) {
      rstat = e->rstat; delete e;
   } catch (...) {
      rstat = sdaiESYSTEM;
   }
   return rstat;
}

extern "C" EDMLONG __declspec(dllexport) dll_version()
{
   return 1;
}

extern "C" void  __declspec(dllexport) *dll_malloc(char *methodName, EDMLONG bufSize)
{
   return dllMa.alloc(bufSize);
}

extern "C" EDMLONG  __declspec(dllexport) dll_freeAll(char *methodName)
{
   dllMa.freeAllMemory();
   if (resultInfoMemory) {
      delete resultInfoMemory;  resultInfoMemory = NULL;
   }
   return OK;
}