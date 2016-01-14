
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
      ClusterRepository *cr = (ClusterRepository *)cm->getFirstReferencing(ecl::et_ClusterRepository);
      if (strEQL(cm->get_name(), name) && cr && strEQL(cr->get_name(), repositoryName)) {
         return cm;
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
   char *cClusterRepositoryName, *cClusterModelName, *cEDMrepositoryName, *cEDMmodelName;

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
   serverContexts = new(&ma)Collection<SdaiServerContext>(&ma);
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
