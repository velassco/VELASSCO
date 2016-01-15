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

   char *schemaName, *modelName;
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
      }
   }
   return mType;
}


EDMLONG GetListOfAnalyses(Model *theModel, ModelType mt, nodervGetListOfAnalyses *retVal)
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
         if (nOfParameters != 0 || nOfReturnValues != 3) {
            rstat = -2;
         } else {
            //nodervGetListOfAnalyses *results = new(&dllMa)nodervGetListOfAnalyses(NULL, returnValues);
            //rstat = GetListOfAnalyses(&VELaSSCo_model, vmt, results);


            //Collection<char*> *nameList = new(&dllMa)Collection<char*>(&dllMa);
            //results->analysis_name_list->putStringCollection(nameList);
            //results->status->putString("OK");
            //results->report->type = rptINTEGER;
            returnValues[0].putInteger(0);
            returnValues[1].putInteger(1);
            returnValues[2].putInteger(2);



         }
      } else {
         rstat = -1;
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