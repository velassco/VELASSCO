
#include "stdafx.h"

#include <omp.h>


/*==============================================================================================================================*/
EDMclusterServices::EDMclusterServices(Model *m)
/*==============================================================================================================================*/
{
   clusterModel = m; ourCluster = NULL; serverContextID = 0; clusterModelID = 0;
   clusterMa.init(0x1000);
   serverContexts = new(&clusterMa)Container<EDMserverContext>(&clusterMa, 64);
   lastServerContext = NULL;

}
/*==============================================================================================================================*/
void EDMclusterServices::getUniqueServerContextID(char *idBuf)
/*==============================================================================================================================*/
{
   EDMULONG id = ++serverContextID;
   sprintf(idBuf, "sc%llu", id);
}
/*==============================================================================================================================*/
SdaiModel EDMclusterServices::getClusterModelID()
/*==============================================================================================================================*/
{
   if (!clusterModelID) {
      sdaiErrorQuery();
      SdaiRepository theRep = sdaiOpenRepositoryBN("EDMcluster");
      if (!theRep) CHECK(sdaiErrorQuery());
      clusterModelID = sdaiOpenModelBN(theRep, "EDMcluster", sdaiRW);
      if (!clusterModelID) CHECK(sdaiErrorQuery());
   }
   return clusterModelID;
}
/*==============================================================================================================================*/
ecl::ClusterModel *EDMclusterServices::getClusterModel(const char *name, const char *repositoryName)
/*==============================================================================================================================*/
{
   Iterator<ecl::ClusterModel*, ecl::entityType> clIter(clusterModel->getObjectSet(ecl::et_ClusterModel), clusterModel);
   for (ecl::ClusterModel *cm = clIter.first(); cm; cm = clIter.next()) {
      if (strEQL(cm->get_name(), name)) {
         ClusterRepository *cr = cm->get_belongs_to();
         if (cr) {
            if (strEQL(cr->get_name(), repositoryName)) {
               return cm;
            }
         }
      }
   }
   return NULL;
}
/*==============================================================================================================================*/
ecl::ClusterRepository *EDMclusterServices::getClusterRepository(const char *name)
/*==============================================================================================================================*/
{
   Iterator<ecl::ClusterRepository*, ecl::entityType> clIter(clusterModel->getObjectSet(ecl::et_ClusterRepository), clusterModel);
   for (ecl::ClusterRepository *cr = clIter.first(); cr; cr = clIter.next()) {
      if (strEQL(cr->get_name(), name)) {
         return cr;
      }
   }
   return NULL;
}

/*==============================================================================================================================*/
void EDMclusterServices::startServices()
/*==============================================================================================================================*/
{
   clusterModel->open("EDMcluster", sdaiRW);
   //clusterModel->defineObjectSet(ecl::et_EDMcluster, 2, true);
   //clusterModel->defineObjectSet(ecl::et_ClusterRepository, 8, true);
   //clusterModel->defineObjectSet(ecl::et_ClusterModel, 8, true);
   //clusterModel->defineObjectSet(ecl::et_EDMdatabase, 8, true);
   //clusterModel->readAllObjectsToMemory();
   Iterator<ecl::EDMcluster*, ecl::entityType> clIter(clusterModel->getObjectSet(ecl::et_EDMcluster), clusterModel);
   ourCluster = clIter.first();
   if (!ourCluster) THROW("EDMclusterServices::startServices() - No EDMcluser object defined.");
   databaseIter.init(clusterModel->getObjectSet(ecl::et_EDMdatabase), 0, clusterModel);
}
/*==============================================================================================================================*/
void EDMclusterServices::initClusterModel(char *serverListFileName)
/*==============================================================================================================================*/
{
   FILE *serverListFile;
   char buf[2048], command[512], param1[512], param2[512], param3[512], param4[512], param5[512], param6[512];
   ecl::ClusterModel *cClusterModel = NULL;
   ecl::ClusterRepository *cClusterRepository = NULL;
   ecl::EDMrepository *cEDMrepository = NULL;
   ecl::EDMmodel *cEDMmodel = NULL;
   ecl::EDMdatabase *cEDMdatabase = NULL;
   char *cClusterRepositoryName, *cClusterModelName, *cEDMmodelName;

   printf("\ninit_file=%s\n", serverListFileName);
   if (serverListFileName) {
      serverListFile = fopen(serverListFileName, "r");
      if (serverListFile) {
         printf("init_file opened\n", serverListFileName);
         clusterModel->open("EDMcluster", sdaiRW);
         printf("EDMclusterServices::initClusterModel - 1\n");
         CHECK(edmiDeleteModelContents(clusterModel->modelId));
         CMemoryAllocator *ma = clusterModel->ma;
         while (fgets(buf, sizeof(buf), serverListFile)){
            printf("init_file_line=%s\n", buf);
            int nCoulmn = sscanf(buf, "%s %s %s %s %s %s %s", command, param1, param2, param3, param4, param5, param6);
            if (strEQL(command, "EDMcluster")) {
               if (ourCluster == NULL) {
                  ourCluster = newObject(ecl::EDMcluster);
                  char *thisName = ma->allocString(param1);
                  ourCluster->put_name(thisName);
               }
            } else if (strEQL(command, "ClusterRepository")) {
               cClusterRepository = newObject(ecl::ClusterRepository); cClusterRepository->put_name(ma->allocString(param1));
               if (ourCluster) {
                  cClusterRepository->put_platform(ourCluster);
               }
            } else if (strEQL(command, "ClusterModel")) {
               cClusterModel = newObject(ecl::ClusterModel); cClusterModel->put_name(ma->allocString(param1));
               if (cClusterRepository) {
                  cClusterModel->put_belongs_to(cClusterRepository);
               }
            } else if (strEQL(command, "EDMrepository")) {
               cEDMrepository = newObject(ecl::EDMrepository); cEDMrepository->put_name(ma->allocString(param1));
               if (cEDMdatabase) {
                  cEDMrepository->put_belongs_to(cEDMdatabase);
               }
            } else if (strEQL(command, "EDMmodel")) {
               cClusterRepositoryName = param1; cClusterModelName = param2; cEDMmodelName = param3;
               cEDMmodel = newObject(ecl::EDMmodel); cEDMmodel->put_name(ma->allocString(cEDMmodelName));
               ecl::ClusterModel *ccm = getClusterModel(cClusterModelName, cClusterRepositoryName);
               if (ccm) {
                  cEDMmodel->put_clusterModel(ccm);
               }
               if (cEDMrepository) {
                  cEDMmodel->put_repository(cEDMrepository);
               }
            } else if (strEQL(command, "EDMdatabase")) {
               // EDMdatabase "O:\projects\VELaSSCo\SVN_src\EDM_plug_in\db_cluster\db1" VELaSSCo VELaSSCo loaclhost 9090
               cEDMdatabase = newObject(ecl::EDMdatabase);
               cEDMdatabase->put_path(ma->allocString(param1)); cEDMdatabase->put_name(ma->allocString(param2));
               cEDMdatabase->put_password(ma->allocString(param3));
               ecl::EDMServer *srv = newObject(ecl::EDMServer);
               srv->put_Host(ma->allocString(param4)); srv->put_Port(ma->allocString(param5));
               srv->put_nAppservers(atol(ma->allocString(param6)));
               cEDMdatabase->put_server(srv);
               if (ourCluster) {
                  ourCluster->put_databases_element(cEDMdatabase); ourCluster->put_servers_element(srv);
               }
            }
         }
         Iterator<ecl::ClusterModel*, ecl::entityType> clusterModelIter(clusterModel->getObjectSet(ecl::et_ClusterModel), clusterModel);
         for (ClusterModel *cm = clusterModelIter.first(); cm; cm = clusterModelIter.next()) {
            countNofAppsrvsOfModel(cm);
         }
         clusterModel->close();

         fclose(serverListFile);
      } else {
         perror("error: ");
         printf("\ncannot open file=%s.\n", serverListFileName);
      }
   } else {
      printf("\nno init_file.\n");
   }
}




/*==============================================================================================================================*/
EDMclusterExecution::EDMclusterExecution(EDMclusterServices *cs)
/*==============================================================================================================================*/
{
   theServer = cs; ma.init(0x10000); subQueries = NULL;
}

/*==============================================================================================================================*/
EDMclusterExecution::~EDMclusterExecution()
/*==============================================================================================================================*/
{
   if (subQueries) {
      for (EDMexecution *exp = subQueries->firstp(); exp; exp = subQueries->nextp()) {
         if (exp->ema) delete exp->ema;
         exp->ema = NULL;
      }
      for (EDMexecution *exp = subQueries->firstp(); exp; exp = subQueries->nextp()) {
         if (exp->serverCtxtRecord) exp->serverCtxtRecord->inUse = false;
      }
   }
   ma.freeAllMemory();
}
///*==============================================================================================================================*/
//void EDMclusterExecution::buildServerContexts(char *user, char *group, char *password)
///*==============================================================================================================================*/
//{
//   char serverContextName[2048];
//
//   ma.reset();
//   serverContexts = new(&ma)Container<SdaiServerContext>(&ma);
//   EDMcluster *theCluster = theServer->getTheEDMcluster();
//
//   //Iterator<ecl::EDMdatabase*, ecl::entityType>  dbIter(theCluster->get_databases(), theServer->clusterModel);
//   for (ecl::EDMdatabase*db = theServer->databaseIter.first(); db; db = theServer->databaseIter.next()) {
//      SdaiServerContext serverContextId;
//      theServer->getUniqueServerContextID(serverContextName);
//      ecl::EDMServer *srv = db->get_server();
//      if (srv) {
//         char *port = srv->get_Port();
//         char *host = srv->get_Host();
//         CHECK(edmiDefineServerContext(serverContextName, user, group, password, "TCP", port, host, NULL, NULL, NULL, NULL, NULL, &serverContextId));
//         serverContexts->add(serverContextId);
//      }
//   }
//}

/*==============================================================================================================================*/
void* ourMemoryAllocator(SdaiVoid _ma, EDMLONG size)
/*==============================================================================================================================*/
{
   CMemoryAllocator* ma = (CMemoryAllocator*)_ma;
   return ma->alloc(size);
}
/*==============================================================================================================================*/
void EDMclusterExecution::ExecuteRemoteCppMethod(EDMexecution *execParams, SdaiString methodName, CppParameterClass *inputParameters,
   bool *errorFound)
/*==============================================================================================================================*/
{
   EDMLONG rstat = OK;
   int startTime = GetTickCount();
   
   try {
      int nTimeouts = 0; execParams->executionTime = -1;
      do {
         if (inputParameters && inputParameters->nOfAttributes) {
            rstat = edmiRemoteExecuteCppMethod(execParams->serverCtxtRecord->srvCtxt, execParams->repositoryName, execParams->modelName, getPluginPath(), getPluginName(),
               methodName, 0, inputParameters->nOfAttributes, (RemoteParameter*)inputParameters->attrPointerArr, NULL, execParams->returnValues->nOfAttributes,
               (RemoteParameter*)execParams->returnValues->attrPointerArr, &ourMemoryAllocator, (void*)execParams->ema, NULL);
         } else {
            rstat = edmiRemoteExecuteCppMethod(execParams->serverCtxtRecord->srvCtxt, execParams->repositoryName, execParams->modelName, getPluginPath(), getPluginName(),
               methodName, 0, 0, NULL, NULL, execParams->returnValues->nOfAttributes,
               (RemoteParameter*)execParams->returnValues->attrPointerArr, &ourMemoryAllocator, (void*)execParams->ema, NULL);
         }
         if (rstat && rstat != edmiE_CLIENT_START_TIMEOUT) {
            *errorFound = true; execParams->error = new CedmError(rstat, __FILE__, __LINE__);
         }
      } while (rstat == edmiE_CLIENT_START_TIMEOUT && ++nTimeouts < 50);
   } catch (CedmError *e) {
      execParams->error = e; *errorFound = true;
   }
   int endTime = GetTickCount();
   execParams->executionTime = endTime - startTime;
}
/*==============================================================================================================================*/
void EDMclusterExecution::writeErrorMessageForSubQueries(string &allMsg)
/*==============================================================================================================================*/
{
   SdaiString              serverContextName, userName, groupName, password, communicationType, edmServerPortNumber;
   SdaiString              edmServerHostName, edmiHttpTunnelName, edmiHttpTunnelPortNumber, edmiHttpTunnelHostName, proxyServerPortNumber, proxyServerName;


   for (EDMexecution *execInfo = subQueries->firstp(); execInfo; execInfo = subQueries->nextp()) {
      if (execInfo->error) {
         edmiGetServerContextProperties(execInfo->serverCtxtRecord->srvCtxt, &serverContextName, &userName, &groupName, &password, &communicationType,
            &edmServerPortNumber, &edmServerHostName, &edmiHttpTunnelName, &edmiHttpTunnelPortNumber, &edmiHttpTunnelHostName,
            &proxyServerPortNumber, &proxyServerName);
         char errMsg[0x4000]; 
         char *exeptionMsg = execInfo->error->message ? execInfo->error->message : edmiGetErrorText(execInfo->error->rstat);
         sprintf(errMsg, "On server %s:%s - error: %s\n", edmServerHostName, edmServerPortNumber, exeptionMsg);
         allMsg += errMsg;
         delete execInfo->error; execInfo->error = NULL;
      }
   }
}
/*==============================================================================================================================*/
bool EDMclusterExecution::OpenClusterModelAndPrepareExecution(const std::string& modelID)
/*==============================================================================================================================*/
{
   return OpenClusterModelAndPrepareExecution(EDM_ATOI64(modelID.data()), NULL, 0, 0);
}
/*==============================================================================================================================*/
void EDMclusterServices::countNofAppsrvsOfModel(ClusterModel *cm)
/*==============================================================================================================================*/
{   
   EDMLONG srvIds[1024], nsrv = 0, i;

   if (cm) {
      int nAppservers = 0;
      Iterator<EDMmodel*, ecl::entityType> modelIter(cm->get_consists_of());
      for (EDMmodel*m = modelIter.first(); m; m = modelIter.next()) {
         EDMrepository *rep = m->get_repository();
         if (rep) {
            EDMdatabase *db = rep->get_belongs_to();
            if (db) {
               EDMServer *srv = db->get_server();
               if (srv) {
                  EDMLONG srvId = srv->getInstanceId();
                  for (i=0; i < nsrv && srvId != srvIds[i]; i++) ;
                  if (i == nsrv) {
                     srvIds[nsrv++] = srvId;
                     if (srv->exists_nAppservers())
                        nAppservers += srv->get_nAppservers();
                  }
               }
            }
         }
      }
      cm->put_nAppservers(nAppservers);
   }
}
/*==============================================================================================================================*/
void EDMclusterExecution::setOptimalNumberOfThreads()
/*==============================================================================================================================*/
{
   if (nAppservers > 0) 
      omp_set_num_threads(nAppservers);
}
/*==============================================================================================================================*/
bool EDMclusterExecution::OpenClusterModelAndPrepareExecution(SdaiModel modelID, char *ModelNameFormat, int FirstModelNo, int LastModelNo)
/*
   modelID is object id of the ClusterModel object retrn by the open model method.
================================================================================================================================*/
{   
   tEdmiInstData cmd;

   theServer->clusterModel->reset();
   ecl::ClusterModel cm(theServer->clusterModel, theServer->clusterModel->initInstData(ecl::et_ClusterModel, &cmd));
   cm.setInstanceId(modelID);
   if (cm.getEntityType() == ecl::et_ClusterModel) {
      Set<EDMmodel*> *theEDMmodels = cm.get_consists_of();
      nAppservers = cm.exists_nAppservers() ? cm.get_nAppservers() : 0;
      if (theEDMmodels) {
         EDMLONG nOfEDMmodels = theEDMmodels->size();
         if (nOfEDMmodels > 0) {
            if (nAppservers > nOfEDMmodels)
               nAppservers = nOfEDMmodels;
            Iterator<EDMmodel*, ecl::entityType> modelIter(theEDMmodels);
            EDMmodel*m = modelIter.first();
            subQueries = new(&ma)Container<EDMexecution>(&ma, nOfEDMmodels);
            int nextModelNo = FirstModelNo;
            char modelName[2048];

            for (EDMLONG i = 0; i < nOfEDMmodels; i++) {
               if (m) {
                  EDMexecution *exp = NULL;
                  char *cModelName = m->get_name();
                  if (ModelNameFormat) {
                     sprintf(modelName, ModelNameFormat, nextModelNo);
                     if (strEQL(modelName, cModelName)) {
                        exp = subQueries->createNext(); exp->modelName = cModelName;
                        exp->modelNumber = nextModelNo;  nextModelNo++;
                     }
                  } else {
                     exp = subQueries->createNext(); exp->modelName = cModelName;
                  }
                  if (exp) {
                     ecl::EDMrepository *r = m->get_repository();
                     exp->repositoryName = r ? r->get_name() : (char*)"";
                     exp->ema = new CMemoryAllocator(0x100000);
                     exp->serverCtxtRecord = theServer->getServerContext("superuser", "", "v", m);
                     exp->error = NULL;
                     if (ModelNameFormat && nextModelNo > LastModelNo)
                        break;
                  }
                  m = modelIter.next();
               }
            }
            setOptimalNumberOfThreads();
            return true;
         }
      }
      THROW("Empty model");
   } else {
      THROW("Invalid modelId");
   }
   return true;
}
/*==============================================================================================================================*/
void EDMclusterExecution::printExecutionReport(string &msg)
/*==============================================================================================================================*/
{
   struct ExecutionTimeInfo {
      ExecutionTimeInfo    *next;
      char                 *ipAddress;
      int                  nExecutions;
      int                  minExecutionTime;
      int                  maxExecutionTime;
      int                  averageExecutionTime;
   };
   SdaiString              serverContextName, userName, groupName, password, communicationType, edmServerPortNumber;
   SdaiString              edmServerHostName, edmiHttpTunnelName, edmiHttpTunnelPortNumber, edmiHttpTunnelHostName, proxyServerPortNumber, proxyServerName;
   char maBuf[0x8000];
   CMemoryAllocator ma(0x8000, maBuf, sizeof(maBuf));

   int nServers = 0;
   ExecutionTimeInfo *firstExecInfo = NULL, **etip, *eti;

   for (EDMexecution *exec = subQueries->firstp(); exec; exec = subQueries->nextp()) {
      edmiGetServerContextProperties(exec->serverCtxtRecord->srvCtxt, &serverContextName, &userName, &groupName, &password, &communicationType,
         &edmServerPortNumber, &edmServerHostName, &edmiHttpTunnelName, &edmiHttpTunnelPortNumber, &edmiHttpTunnelHostName,
         &proxyServerPortNumber, &proxyServerName);
      for (etip = &firstExecInfo; *etip && strNEQ((*etip)->ipAddress, edmServerHostName); etip = &(*etip)->next);
      if (*etip == NULL) {
         eti = (ExecutionTimeInfo*)ma.allocZeroFilled(sizeof(ExecutionTimeInfo)); *etip = eti;
         eti->ipAddress = ma.allocString(edmServerHostName); eti->minExecutionTime = 0x7fffffff;
      } else {
         eti = *etip;
      }
      eti->nExecutions++;
      if (exec->executionTime > eti->maxExecutionTime) eti->maxExecutionTime = exec->executionTime;
      if (exec->executionTime < eti->minExecutionTime) eti->minExecutionTime = exec->executionTime;
      eti->averageExecutionTime += exec->executionTime;
   }

   msg += "Execution time info:\n         server     min     max average   nexec\n";
   for (eti = firstExecInfo; eti; eti = eti->next) {
      eti->averageExecutionTime = eti->averageExecutionTime / eti->nExecutions;
      char buf[4096];
      sprintf(buf, "%15s%8d%8d%8d%8d\n", eti->ipAddress, eti->minExecutionTime, eti->maxExecutionTime, eti->averageExecutionTime, eti->nExecutions);
      msg += buf;
   }
}


/*==============================================================================================================================*/
EDMserverContext *EDMclusterServices::getServerContext(char *user, char *group, char *password, EDMmodel *m)
/*==============================================================================================================================*/
{
   EDMrepository                       * r = m->get_repository();
   EDMdatabase                         *db = r ? r->get_belongs_to() : NULL;
   EDMServer                           *srv = db ? db->get_server() : NULL;

   return getServerContext(user, group, password, srv);
}
/*==============================================================================================================================*/
EDMserverContext *EDMclusterServices::getServerContext(char *user, char *group, char *password, EDMServer *srv)
/*==============================================================================================================================*/
{
   char                                serverContextName[2048];
   SdaiServerContext                   srvctxt = 0;
   EDMserverContext                    *startServerContext = lastServerContext, *cSrvCtxt;

   if (srv) {
      char *host = srv->get_Host();
      char *port = srv->get_Port();
      do {
         cSrvCtxt = serverContexts->nextp();
         if (!cSrvCtxt)
            cSrvCtxt = serverContexts->firstp();
         if (cSrvCtxt == startServerContext) {
            getUniqueServerContextID(serverContextName);
            lastServerContext = serverContexts->createNext();
            lastServerContext->inUse = true;
            lastServerContext->port = clusterMa.allocString(port);
            lastServerContext->host = clusterMa.allocString(host);
            CHECK(edmiDefineServerContext(serverContextName, user, group, password, "TCP", port, host, NULL, NULL, NULL, NULL, NULL, &lastServerContext->srvCtxt));
            cSrvCtxt = serverContexts->nextp();
            return lastServerContext;
         } else if (!cSrvCtxt->inUse && strEQL(cSrvCtxt->port, port) && strEQL(cSrvCtxt->host, host)) {
            lastServerContext = cSrvCtxt; cSrvCtxt->inUse = true; return cSrvCtxt;
         }
      } while (true);
   }
   return NULL;
}
/*==============================================================================================================================*/
void EDMclusterServices::listAllEDMservers()
/*==============================================================================================================================*/
{
}
/*==============================================================================================================================*/
void EDMclusterServices::stopAllEDMservers()
/*==============================================================================================================================*/
{
   tEdmiInstData cmd;
   EDMserverContext *srvCtxts[1000];
   int nServers = 0;

   clusterModel->reset();
   Iterator<ecl::EDMServer*, ecl::entityType> serverIter(clusterModel->getObjectSet(ecl::et_EDMServer), clusterModel);
   for (ecl::EDMServer *srv = serverIter.first(); srv && nServers < 1000; srv = serverIter.next()) {
      srvCtxts[nServers++] = getServerContext("superuser", "", "v", srv);
   }
#pragma omp parallel for
   for (int i = 0; i < nServers; i++) {
      EdmiError rstat = edmiRemoteStopServer(srvCtxts[i]->srvCtxt, "v", NULL, FORCE_TO_TERMINATE | ALL_SERVERS, NULL);
      if (rstat) {
#pragma omp critical
         printf("error=%llu\n", rstat);
      }
   }
}