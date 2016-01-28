
#include "stdafx.h"




/*==============================================================================================================================*/
EDMclusterServices::EDMclusterServices(Model *m)
/*==============================================================================================================================*/
{
   clusterModel = m; ourCluster = NULL; serverContextID = 0; clusterModelID = 0;
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
   char buf[2048], command[512], param1[512], param2[512], param3[512], param4[512], param5[512];
   ecl::ClusterModel *cClusterModel = NULL;
   ecl::ClusterRepository *cClusterRepository = NULL;
   ecl::EDMrepository *cEDMrepository = NULL;
   ecl::EDMmodel *cEDMmodel = NULL;
   ecl::EDMdatabase *cEDMdatabase = NULL;
   char *cClusterRepositoryName, *cClusterModelName, *cEDMmodelName;

   if (serverListFileName && (serverListFile = fopen(serverListFileName, "r"))) {
      clusterModel->open("EDMcluster", sdaiRW);
      CHECK(edmiDeleteModelContents(clusterModel->modelId));
      CMemoryAllocator *ma = clusterModel->ma;
      while (fgets(buf, sizeof(buf), serverListFile)){
         int nCoulmn = sscanf(buf, "%s %s %s %s %s %s", command, param1, param2, param3, param4, param5);
         if (strEQL(command, "EDMcluster")) {
            if (ourCluster == NULL) {
               ourCluster = newObject(ecl::EDMcluster);
               ourCluster->put_name(ma->allocString(param1));
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
            cEDMdatabase->put_server(srv);
            if (ourCluster) {
               ourCluster->put_databases_element(cEDMdatabase); ourCluster->put_servers_element(srv);
            }
         }
      }
      //clusterModel->writeAllObjectsToDatabase();
      clusterModel->close();
      //ma->reset();
      fclose(serverListFile);
   } else {
   }
}




/*==============================================================================================================================*/
EDMclusterExecution::EDMclusterExecution(EDMclusterServices *cs)
/*==============================================================================================================================*/
{
   theServer = cs; ma.init(0x10000); serverContexts = NULL;
}
/*==============================================================================================================================*/
void EDMclusterExecution::buildServerContexts(char *user, char *group, char *password)
/*==============================================================================================================================*/
{
   char serverContextName[2048];

   ma.reset();
   serverContexts = new(&ma)Container<SdaiServerContext>(&ma);
   EDMcluster *theCluster = theServer->getTheEDMcluster();

   //Iterator<ecl::EDMdatabase*, ecl::entityType>  dbIter(theCluster->get_databases(), theServer->clusterModel);
   for (ecl::EDMdatabase*db = theServer->databaseIter.first(); db; db = theServer->databaseIter.next()) {
      SdaiServerContext serverContextId;
      theServer->getUniqueServerContextID(serverContextName);
      ecl::EDMServer *srv = db->get_server();
      if (srv) {
         char *port = srv->get_Port();
         char *host = srv->get_Host();
         CHECK(edmiDefineServerContext(serverContextName, user, group, password, "TCP", port, host, NULL, NULL, NULL, NULL, NULL, &serverContextId));
         serverContexts->add(serverContextId);
      }
   }
}

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
   try {
      if (inputParameters && inputParameters->nOfAttributes) {
         CHECK(edmiRemoteExecuteCppMethod(execParams->srvCtxt, execParams->repositoryName, execParams->modelName, getPluginPath(), getPluginName(),
            methodName, 0, inputParameters->nOfAttributes, (RemoteParameter*)inputParameters->attrPointerArr, NULL, execParams->returnValues->nOfAttributes,
            (RemoteParameter*)execParams->returnValues->attrPointerArr, &ourMemoryAllocator, (void*)execParams->ema, NULL));
      } else {
         CHECK(edmiRemoteExecuteCppMethod(execParams->srvCtxt, execParams->repositoryName, execParams->modelName, getPluginPath(), getPluginName(),
            methodName, 0, 0, NULL, NULL, execParams->returnValues->nOfAttributes,
            (RemoteParameter*)execParams->returnValues->attrPointerArr, &ourMemoryAllocator, (void*)execParams->ema, NULL));
      }
   } catch (CedmError *e) {
      execParams->error = e; *errorFound = true;
   }
}
/*==============================================================================================================================*/
void EDMclusterExecution::writeErrorMessageForSubQueries(string &allMsg)
/*==============================================================================================================================*/
{
   SdaiString              serverContextName, userName, groupName, password, communicationType, edmServerPortNumber;
   SdaiString              edmServerHostName, edmiHttpTunnelName, edmiHttpTunnelPortNumber, edmiHttpTunnelHostName, proxyServerPortNumber, proxyServerName;


   for (EDMexecution *execInfo = subQueries->firstp(); execInfo; execInfo = subQueries->nextp()) {
      if (execInfo->error) {
         edmiGetServerContextProperties(execInfo->srvCtxt, &serverContextName, &userName, &groupName, &password, &communicationType,
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
/*
   modelID is object id of the ClusterModel object retrn by the open model method.
================================================================================================================================*/
{
   tEdmiInstData cmd;
   ecl::ClusterModel cm(theServer->clusterModel, theServer->clusterModel->initInstData(ecl::et_ClusterModel, &cmd));
   cm.setInstanceId(EDM_ATOI64(modelID.data()));
   if (cm.getEntityType() == ecl::et_ClusterModel) {
      Set<EDMmodel*> *theEDMmodels = cm.get_consists_of();
      if (theEDMmodels) {
         EDMLONG nOfEDMmodels = theEDMmodels->size();
         if (nOfEDMmodels > 0) {
            Iterator<EDMmodel*, ecl::entityType> modelIter(theEDMmodels, theServer->clusterModel);
            EDMmodel*m = modelIter.first();
            subQueries = new(&ma)Container<EDMexecution>(&ma, nOfEDMmodels);
            for (EDMLONG i = 0; m && i < nOfEDMmodels; i++) {
               EDMexecution *exp = subQueries->createNext();
               exp->modelName = m->get_name();
               ecl::EDMrepository *r = m->get_repository();
               exp->repositoryName = r ? r->get_name() : "";
               exp->ema = new CMemoryAllocator(0x100000);
               exp->srvCtxt = theServer->getServerContext("superuser", "", "v", m);
               exp->error = NULL;

               m = modelIter.next();
            }
            for (EDMexecution *e = subQueries->firstp(); e; e = subQueries->nextp()) {
               e = NULL;
            }
            for (EDMLONG i = 0; i < nOfEDMmodels; i++) {
               EDMexecution *e = subQueries->getElementp(i);
               e = NULL;
            }
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
SdaiServerContext EDMclusterServices::getServerContext(char *user, char *group, char *password, EDMmodel *m)
/*==============================================================================================================================*/
{
   char serverContextName[2048];
   SdaiServerContext srvctxt = 0;

   getUniqueServerContextID(serverContextName);
   EDMrepository* r = m->get_repository();
   EDMdatabase *db = r ? r->get_belongs_to() : NULL;
   EDMServer *srv = db ? db->get_server() : NULL;
   if (srv) {
      CHECK(edmiDefineServerContext(serverContextName, user, group, password, "TCP", srv->get_Port(), srv->get_Host(), NULL, NULL, NULL, NULL, NULL, &srvctxt));
   }
   return srvctxt;
}

