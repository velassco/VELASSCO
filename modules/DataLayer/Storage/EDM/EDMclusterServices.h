


#define MAX_PAR 20
/*================================================================================================*/
class EDMclusterServices
/*!
   EDMclusterServices is on object for "binding" together independant EDM databases that runs on many
   machines - runs on cluster of machines. 
   EDMclusterServices provides methods/queries - search and update on collection of models (data sets).

   Collection of models can be constituted in several ways. EDMclusterServices support persistant 
   collections af models by the concepts ClusterRepository and ClusterModel. A ClusterRepository is a
   collection of EDM repositories in the databases that forms the cluster. A ClusterModel is collection
   of EDM models in a ClusterRepository.
   The persistent cluster information is defined by the schema EDMcluster and the data is stored in the
   repository.model Cluster.Cluster
*/
{
   Iterator<ecl::EDMdatabase*, ecl::entityType>       databaseIter;
   ecl::EDMcluster                                    *ourCluster;

   EDMULONG                                           serverContextID;
   SdaiModel                                          clusterModelID;
public:
   Model                                              *clusterModel;
/*!
*/
   EDMclusterServices(Model *m);
/*!
   startServices assumes that the database servers that constitute the cluster is started before this
   method is executed.
*/
   void                       startServices();
   void                       initClusterModel(char *serverListFileName);
   ecl::EDMcluster            *getTheEDMcluster() { return ourCluster; }
   void                       getUniqueServerContextID(char *idBuf);
   SdaiModel                  getClusterModelID();
   ecl::ClusterModel          *getClusterModel(const char *name, const char *repositoryName);
   ecl::ClusterRepository     *getClusterRepository(const char *name);
};

/*================================================================================================*/
/*!
EDMclusterExecution represents one execution of a method on a cluster of EDM databases.

*/
class EDMclusterExecution
{
protected:
   CMemoryAllocator                 ma;
   EDMclusterServices               *theServer;
   EDMULONG                         nParameters;
   tSdaiSelect                      params[MAX_PAR];
   void                             init()
   {
      nParameters = 0; params[0].nTypes = 0;
   }
public:
   Collection<SdaiServerContext>    *serverContexts;
   /*!
   Execution on a ClusterModel
   */
   EDMclusterExecution(EDMclusterServices *cs);
   /*!
   addInputParameter.
   */
   void                             addInputParameter(SdaiPrimitiveType ptype, void *param);
   void                             buildServerContexts(char *user, char *group, char *password);
   void                             buildServerContexts(char *user, char *group, char *password, ClusterModel *cm);
};


#define newObject(className) new(clusterModel)className(clusterModel)

#ifdef CPPP
/*====================================================================================*/
/*   edmiRemoteExecuteCppMethod                                                       */
/*====================================================================================*/
EdmiError edmiRemoteExecuteCppMethod(SdaiServerContext       serverContextId,
                                     SdaiString              remoteRepositoryName,
                                     SdaiString              remoteModelName,
                                     SdaiString              pluginName,
                                     SdaiString              methodName,
                                     SdaiOptions             options,         /* No options defined yet */
                                     SdaiInteger             nOfParameters,
                                     ExtendedSelect          *parameters,
                                     ExtendedSelect          returnValue,
                                     SdaiInvocationId        *edmiInvocationId)
{
   EDMLONG   rstat;


   ENTER_FUNCTION
      rstat = doRemoteWebServiceOperation(serverContextId, wsExecuteWebService, edmiInvocationId,
      serverAddress, url, inputSoapMsg, options, logDescription, outputSoapMsg);
   LEAVE_FUNCTION
      return(rstat);
}





if (pMessEx != NULL) {
   messSize = EDM_SIZEOF(tCRemoteLargeObjectOperationsEx) - EDM_SIZEOF(pMessEx->Strings) + messIndex + EDM_SIZEOF(EDMLONG);
} else {
   messSize = EDM_SIZEOF(tCRemoteLargeObjectOperations) - EDM_SIZEOF(pMess->Strings) + messIndex + EDM_SIZEOF(EDMLONG);
}
pMess->CmndHeader.CmndSize = messSize + pMess->fsize;
for (retries = 0; retries < svc->communicationRetries; retries++) {
   rstat = connectToServer(svc, APPLICATION_SERVER, pMess);
   if (!rstat) {
      rstat = remoteSendCommandHeaderToServer(svc, pMess, messSize);
      if (!rstat) {
         if (pMess->fsize) {
            rstat = remoteCopyFileToServer(svc, new_sfileName, pMess->fsize);
         }
         if (!rstat) {
            rstat = remoteGetReplyHeaderFromServer(svc, R_RemoteLargeObjectOperations, EDM_SIZEOF(tRRemoteLargeObjectOperations) - EDM_SIZEOF(pResp->Strings));
            if ((rstat) && (rstat != edmiE_CLIENT_START_TIMEOUT)) {
               GOTO_ERR;
            }
         }
      }
   }
   if (!rstat) {
      break;
   }
   if ((svc->fixedPortNumber) || (retries == svc->communicationRetries - 1)) {
      GOTO_ERR;
   }
   if (rstat = getNewTcpConnection(svc, pMess, rstat)) {
      GOTO_ERR;
   }
}


#endif 