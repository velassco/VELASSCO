// EDM_server_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "EDM_server_dll.h"




CMemoryAllocator dllMa(0x100000);

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
   delete e;
   return dllMa.allocString(errMsg);
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

      Collection<char*> *nameList = new(&dllMa)Collection<char*>(&dllMa);
      if (mt == mtDEM) {
         Iterator<dem::Simulation*, dem::entityType> simIter(theModel->getObjectSet(dem::et_Simulation), theModel);
         for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
            nameList->add(dllMa.allocString(s->get_name()));
         }
         retVal->analysis_name_list->putStringCollection(nameList);
      } else if (mt == mtFEM) {
         Iterator<fem::ResultHeader*, fem::entityType> rhIter(theModel->getObjectSet(fem::et_ResultHeader), theModel);
         for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
            char* anName = rh->get_analysis();
            for (np = nameList->first(); np && strNEQ(anName, np); np = nameList->next());
            if (np == NULL) {
               nameList->add(dllMa.allocString(anName));
            }
         }
         retVal->analysis_name_list->putStringCollection(nameList);
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
      Collection<double> timeSteps(&dllMa);

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
         retVal->ListOfTimeSteps->putCollection(&timeSteps);
      } else {
         Iterator<fem::ResultHeader*, fem::entityType> rhIter(theModel->getObjectSet(fem::et_ResultHeader), theModel);
         for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
            if (strEQL(rh->get_analysis(), anid)) {
               timeSteps.add(rh->get_step());
            }
         }
         retVal->ListOfTimeSteps->putCollection(&timeSteps);
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

   try {
      char *anid = inParam->analysisID->value.stringVal;
      Collection<EDMVD::Vertex> vertices(&dllMa, 1024);

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
         //retVal->ListOfTimeSteps->putCollection(&timeSteps);
      } else {
         Iterator<fem::Node*, fem::entityType> nodeIter(theModel->getObjectSet(fem::et_Node), theModel);
         for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
            EDMVD::Vertex *v = vertices.createNext();
            v->id = n->get_id(); v->x = n->get_x(); v->y = n->get_y(); v->z = n->get_z();
         }
         retVal->vertices->putCollection(&vertices);
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
            results->analysis_name_list->type = rptINTEGER;
         } else {
            rstat = GetListOfAnalyses(&VELaSSCo_model, vmt, results);
         }
      } else if (strEQL(methodName, "GetListOfTimeSteps")) {
         nodeRvGetListOfTimeSteps *results = new(&dllMa)nodeRvGetListOfTimeSteps(NULL, returnValues);
         nodeInGetListOfTimeSteps *inParams = new(&dllMa)nodeInGetListOfTimeSteps(NULL, parameters);
         if (nOfParameters != 1 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->ListOfTimeSteps->type = rptINTEGER;
         } else {
            rstat = GetListOfTimeSteps(&VELaSSCo_model, vmt, inParams, results);
         }
      } else if (strEQL(methodName, "GetListOfVerticesFromMesh")) {
         nodeRvGetListOfVerticesFromMesh *results = new(&dllMa)nodeRvGetListOfVerticesFromMesh(NULL, returnValues);
         nodeInGetListOfVerticesFromMesh *inParams = new(&dllMa)nodeInGetListOfVerticesFromMesh(NULL, parameters);
         if (nOfParameters != 3 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->vertices->type = rptINTEGER;
         } else {
            rstat = GetListOfVerticesFromMesh(&VELaSSCo_model, vmt, inParams, results);
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
   return OK;
}