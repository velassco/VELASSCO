// EDM_plug_in.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\..\..\..\EDM_Interface\EDM_interface.h"
#include "EDMmodelCache.h"
#include "CLogger.h"
#include "VELaSSCoHandler.h"
#include "VELaSSCoMethods.h"

/*    Deliverables
      3.5 - Jotne
      Deadline:   Feb. 15.

*/
int main(int argc, char* argv[])
{
   int rstat;
   char errTxt[1024];

   if (argc != 5 && argc != 6) {
      printf("The VELaSSCo Data Layer server shall have four command line parameters:\n   1. Communcation port\n   2. Database folder\n   3. Database name\n   4. Database password");
      printf("\nOptional prameter: 5. File name for Cluster database init file");
      exit(0);
   }
   int port = atol(argv[1]);
   VELaSSCoHandler *ourVELaSSCoHandler = new VELaSSCoHandler();
   boost::shared_ptr<VELaSSCoHandler> handler(ourVELaSSCoHandler);
   boost::shared_ptr<TProcessor> processor(new VELaSSCoSMProcessor(handler));
   boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
   boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
   boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

   try {
      WSADATA wsaData = {};
      WORD wVersionRequested = MAKEWORD(2, 2);
      int err = WSAStartup(wVersionRequested, &wsaData);

      Database VELaSSCo_db(argv[2], argv[3], argv[4]);
      Repository demRepository(&VELaSSCo_db, "DEM_models");
      Repository femRepository(&VELaSSCo_db, "FEM_models");
      VELaSSCo_db.open();
      demRepository.open(sdaiRO);
      femRepository.open(sdaiRO);
      ourVELaSSCoHandler->cDEMrep = &demRepository;
      ourVELaSSCoHandler->cFEMrep = &femRepository;

#ifndef Cluster
      SdaiRepository repositoryId;
      CMemoryAllocator ma(0x100000);
      int rstat = edmiCreateRepository("EDMcluster", &repositoryId);
      if (rstat == OK) {
         rstat = edmiCreateModelBN(repositoryId, "EDMcluster", "EDMcluster", 0);
      }
      Repository clusterRepository(&VELaSSCo_db, "EDMcluster");
      Model clusterModel(&clusterRepository, &ma, &EDMcluster_SchemaObject);
      clusterModel.open("EDMcluster", sdaiRW);
      VELaSSCoCluster ourCluster(&clusterModel);
      if (argc == 6) {
         ourCluster.initClusterModel(argv[5]);
      }
      ourCluster.startServices();

      VELaSSCoMethods findAllModels(&ourCluster);
      //findAllModels.buildServerContexts("superuser", "", "v");
      ////findAllModels.ListModels();
      ////findAllModels.ValidateModels();
      //findAllModels.getBoundingBox();

      ourVELaSSCoHandler->defineCluster(&ourCluster);
#endif /* Cluster */


      //ourVELaSSCoHandler->InitQueryCaches();
      FILE *logFile = fopen("EDMstorageModule.log", "w");
      CLoggWriter    ourLogger(logFile, true, true);

      ourLogger.logg(0, "The EDM VELaSSCo Data.Layer is ready to execute queries.\n\n");
      ourVELaSSCoHandler->defineLogger(&ourLogger);

      TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);

      //boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(64);
      //boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
      //threadManager->threadFactory(threadFactory);
      //threadManager->start();

      //TThreadedServer server(processor,
      //   serverTransport,
      //   transportFactory,
      //   protocolFactory);

      server.serve();
   } catch (CedmError *e) {
      rstat = e->rstat;
      if (e->message) {
         strncpy(errTxt, e->message, sizeof(errTxt));
      } else {
         strncpy(errTxt, edmiGetErrorText(rstat), sizeof(errTxt));
      }
      delete e;
      printf(errTxt);
   } catch (int thrownRstat) {
      strncpy(errTxt, edmiGetErrorText(thrownRstat), sizeof(errTxt));
      printf(errTxt);
   }
   exit(0);
}

