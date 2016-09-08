
#include "stdafx.h"

#include "VELaSSCoMethods.h"
#include "../edm_plugin/EDM_plugin_1.h"
#include "EDM_interface.h"
#include "CLogger.h"
#include "VELaSSCo_Operations.h"
#include "VELaSSCoHandler.h"


/*=============================================================================================================================*/
void VELaSSCoHandler::InjectData(char *commandFileName)
/*
   Command              - InjectFileSequence
                          The cluster model must exist and the physical EDM models must have equal names except for an integer
                          in the model name increasing from 0 to number of submodels - 1.
   ClusterRepository    - repository name
   ClusterModel         - model name
   EDMmodelNameFormat   - Example: Barcelona_%d
   FirstModelNo         - integer 
   LastModelNo          - integer
   FileNameFormat       - Example: E:\VELaSSCo\installation\testdata\Barcelona\Barcelona_fullmodel_8m_100M_ascii_%d.post.msh
   FileNameFormat       - Example: E:\VELaSSCo\installation\testdata\Barcelona\Barcelona_fullmodel_8m_100M_ascii_%d.post.res
===============================================================================================================================*/
{
   SdaiInstance      modelID;
   EDMLONG           rstat;
   SdaiInteger       nOfFileNameMatches;
   char              *Command = NULL, *ClusterRepository = NULL, *ClusterModel = NULL, *EDMmodelNameFormat = NULL;
   int               FirstModelNo = -1, LastModelNo = -1;
   VELaSSCoMethods   theQuery(theCluster);

   try {
      thelog->logg(1, "\n-->InjectData, commandFileName=%s\n\n", commandFileName);
      FILE *ifp = fopen(commandFileName, "r");
      if (ifp) {
         char line[2048], a[1024], b[1024];
         Container <char*> messages(&model_ma);
         Container<char*> FileNameFormats(&model_ma);

         while (fgets(line, sizeof(line), ifp)) {
            a[0] = 0; b[0] = 0;
            sscanf(line, "%s %s", a, b);
            if (strEQL(a, "Command")) Command = model_ma.allocString(b);
            else if (strEQL(a, "ClusterRepository")) ClusterRepository = model_ma.allocString(b);
            else if (strEQL(a, "ClusterModel")) ClusterModel = model_ma.allocString(b);
            else if (strEQL(a, "EDMmodelNameFormat")) EDMmodelNameFormat = model_ma.allocString(b);
            else if (strEQL(a, "FirstModelNo")) FirstModelNo = atol(b);
            else if (strEQL(a, "LastModelNo")) LastModelNo = atol(b);
            else if (strEQL(a, "FileNameFormat")) FileNameFormats.add(model_ma.allocString(b));
            thelog->logg(1, "    %s", line);
         }
         if (ClusterRepository && ClusterModel && EDMmodelNameFormat && FileNameFormats.size() > 0 && FirstModelNo >= 0 && LastModelNo >= 0) {
            modelID = getClusterModelID(ClusterRepository, ClusterModel, &rstat, &nOfFileNameMatches);
            if (nOfFileNameMatches == 1) {
               if (strEQL(Command, "InjectFileSequence")) {
                  if (theQuery.OpenClusterModelAndPrepareExecution(modelID, EDMmodelNameFormat, FirstModelNo, LastModelNo)) {
                     theQuery.InjectFileSequence(&FileNameFormats, FirstModelNo, LastModelNo, EDMmodelNameFormat, &messages);
                     thelog->logg(0, "    Injection of data finished.\n");
                     for (char *msg = messages.first(); msg; msg = messages.next()) {
                        thelog->logg(1, "    %s\n", msg);
                     }
                  }
               } else if (strEQL(Command, "CreateCubeFromFileSequence")) {
                  if (theQuery.OpenClusterModelAndPrepareExecution(modelID, EDMmodelNameFormat, FirstModelNo, LastModelNo)) {
                     theQuery.CreateCubeFromFileSequence(&FileNameFormats, FirstModelNo, LastModelNo, EDMmodelNameFormat, &messages);
                     thelog->logg(0, "    Create Cube from File Sequence finished.\n");
                     for (char *msg = messages.first(); msg; msg = messages.next()) {
                        thelog->logg(1, "    %s\n", msg);
                     }
                  }
               } else {
                  thelog->logg(1, "    Illegal command in file %s\n\n", commandFileName);
               }
            } else {
               thelog->logg(2, "   Not existing or ambiguous model name %s.%s\n\n", ClusterRepository, ClusterModel);
            }
         } else {
            thelog->logg(1, "   Missing input parameters in file %s\n\n", commandFileName);
         }
         fclose(ifp);
      } else {
         thelog->logg(1, "   Not possible to open file %s\n\n", commandFileName);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      thelog->logg(1, "   Error detected, message=%s\n\n", errMsg.data());
   }
}

/*=============================================================================================================================*/
void VELaSSCoMethods::CreateCubeFromFileSequence(Container<char*> *FileNameFormats, int FirstModelNo, int LastModelNo,
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
#pragma omp parallel for
      for (int i = 0; i < nOfSubdomains; i++) {
         e = subQueries->getElementp(i);
         ExecuteRemoteCppMethod(e, "AnalyzeFEMfiles", e->inParams, &errorFound);
      }

      nodeRvInjectFiles *retValueWithError = NULL;
      EDMLONG maxNodeId = 0, maxElementId = 0;
      for (int i = 0; i < nOfSubdomains; i++) {
         EDMexecution *e = subQueries->getElementp(i);
         nodeRvInjectFiles *retVal = (nodeRvInjectFiles *)e->returnValues;
         if(strNEQ(retVal->status->value.stringVal, "OK")) {
            retValueWithError = retVal; break;
         }
         if (retVal->maxNodeId->value.intVal > maxNodeId) maxNodeId = retVal->maxNodeId->value.intVal;
         if (retVal->maxElementId->value.intVal < maxElementId) maxElementId = retVal->maxElementId->value.intVal;
      }
      char t[1024];
      if (retValueWithError) {
         msgs->add("Error - ");
         msgs->add(retValueWithError->report->value.stringVal);
      } else {
         sprintf(t, "   Max node id = %llu, max element id = %llu\n", maxNodeId, maxElementId);
         Container<char*> *resultNames = new(&ma)Container<char*>(&ma);
         Container<double> *timesteps = new(&ma)Container<double>(&ma);
         msgs->add(ma.allocString(t));
         for (int i = 0; i < nOfSubdomains; i++) {
            EDMexecution *e = subQueries->getElementp(i);
            nodeRvInjectFiles *retVal = (nodeRvInjectFiles *)e->returnValues;
            Container<char*> *resultNamesFromOneFile = new(&ma)Container<char*>(&ma, retVal->resultNames);
            char *thisResult, *rn;
            for (thisResult = resultNamesFromOneFile->first(); thisResult; thisResult = resultNamesFromOneFile->next()) {
               for (rn = resultNames->first(); rn; rn = resultNames->next()) {
                  if (strEQL(rn, thisResult)) break;
               }
               if (rn == NULL) {
                  resultNames->add(thisResult);
               }
            }

            Container<double> *timestepsFromOneFile = new(&ma)Container<double>(&ma, retVal->timesteps);
            double *thisTimestep, *cts;
            for (thisTimestep = timestepsFromOneFile->firstp(); thisTimestep; thisTimestep = timestepsFromOneFile->nextp()) {
               for (cts = timesteps->firstp(); cts; cts = timesteps->nextp()) {
                  if (*cts == *thisTimestep) break;
               }
               if (cts == NULL) {
                  timesteps->add(*thisTimestep);
               }
            }
            /* Then we know how many timesteps and result types are the input asci files. Then we can start creating "cube file".*/

         }
      }
      endTime = GetTickCount();
      sprintf(t, "   Time used %d millisec", endTime - startTime);
      msgs->add(ma.allocString(t));
   }
}

