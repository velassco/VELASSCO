// EDM_server_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "EDM_server_dll.h"



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


CMemoryAllocator dllMa(0x100000);


EDMLONG GetListOfAnalyses(Model *theModel, rvGetListOfAnalyses *retVal)
{
   EdmiError rstat = OK;

   //try {
   //   CHECK(edmiChangeOpenMode(modelId, sdaiRO));
   //   if (getModelType(modelId) == mtDEM) {
   //      Iterator<dem::Simulation*, dem::entityType> simIter(dmc->getObjectSet(dem::et_Simulation), dmc);
   //      for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
   //         analysisNames.push_back(s->get_name());
   //      }
   //      _return.__set_status("OK"); _return.__set_analyses(analysisNames);
   //   } else if (emc->type == mtFEM) {
   //      FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
   //      Iterator<fem::ResultHeader*, fem::entityType> rhIter(fmc->getObjectSet(fem::et_ResultHeader), fmc);
   //      map<string, int> names;
   //      for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
   //         char *name = rh->get_analysis();
   //         if (names[rh->get_analysis()] == NULL) {
   //            names[rh->get_analysis()] = 1; analysisNames.push_back(rh->get_analysis());
   //         }
   //      }
   //      _return.__set_status("OK"); _return.__set_analyses(analysisNames);
   //      thelog->logg(0, "status=OK\n\n");
   //   }
   //   } else {
   //      char *emsg = "Model does not exist.";
   //      _return.__set_status("Error"); _return.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
   //   }
   //} catch (CedmError *e) {
   //   string errMsg;
   //   handleError(errMsg, e);
   //   _return.__set_status("Error"); _return.__set_report(errMsg);
   //}

   return rstat;
}




extern "C" EDMLONG __declspec(dllexport) dll_main(char *repositoryName, char *modelName, char *methodName,
   EDMLONG nOfParameters, tRemoteParameter *parameters, EDMLONG nOfReturnValues, tRemoteParameter *returnValues)
{
   EdmiError rstat = OK;

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
         rvGetListOfAnalyses *results = new(&dllMa)rvGetListOfAnalyses(NULL, returnValues);
         rstat = GetListOfAnalyses(&VELaSSCo_model, results);
      }
   } else {
      rstat = -1;
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

extern "C" void  __declspec(dllexport) dll_freeAll(char *methodName)
{
   dllMa.freeAllMemory();
}