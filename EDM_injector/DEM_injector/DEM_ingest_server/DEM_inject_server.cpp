// DEM_ingest_server.cpp : Defines the entry point for the console application.

#include "stdafx.h"




// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "DEM_Injector.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace  std;

using namespace  ::DEM_Inject;

#include "DEM_InjectorHandler.h"




int main(int argc, char **argv)
{
   int rstat;
   char errTxt[1024];
   
   if (argc < 5 || (strNEQ(argv[1], "Server") && strNEQ(argv[1], "Files"))) {
      printf("The VELaSSCo Data Injector for EDM has the command line parameters:\n   1. Command - can be either \"Server\", \"Files\", \"Help\", \n   2. Database folder\n   3. Database name\n   4. Database password\n");
      printf("\n   If Command is \"Server\", parameter no 5 is communication port number.\n");
      printf("   If Command is \"Files\", parameter no 5 is model name and the following\n   parameters are file names of the files that shall be injected into EDM.\n");
      exit(0);
   }

   try {
      int port = 9090;

      DEM_InjectorHandler demInjector(&dem_schema_velassco_SchemaObject);

      Database VELaSSCo_db(argv[2], argv[3], argv[4]);
      Repository demRepository(&VELaSSCo_db, "DEM_models");
      demInjector.setCurrentSchemaName("dem_schema_velassco");

      demInjector.setDatabase(&VELaSSCo_db);
      VELaSSCo_db.open();
      demRepository.open(sdaiRW);
      demInjector.setCurrentRepository(&demRepository);
      
      if (strEQL(argv[1], "Server")) {
         boost::shared_ptr<DEM_InjectorHandler> handler(&demInjector);
         boost::shared_ptr<TProcessor> processor(new DEM_InjectorProcessor(handler));
         boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
         boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
         boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

         WSADATA wsaData = {};
         WORD wVersionRequested = MAKEWORD(2, 2);
         int err = WSAStartup(wVersionRequested, &wsaData);
 
         TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
         server.serve();
      } else if (strEQL(argv[1], "Files")) {
         char commandline[2048], command[1024], param1[1024], param2[1024], param3[1024];

         demInjector.setCurrentModel(argv[5]);
         demInjector.DeleteCurrentModelContent();
         demInjector.InitiateFileInjection();
         FILE *paraMfile = fopen(argv[6], "r");
         if (paraMfile) {
            while (fgets(commandline, sizeof(commandline), paraMfile)){
               int nCoulmn = sscanf(commandline, "%s %s %s %s", command, param1, param2, param3);
               if (strEQL(command, "File")) {
                  if (nCoulmn == 2){
                     demInjector.InjectFile(param1);
                  } else {
                     printf("Illegal number of parameters in File command\n");
                  }
               } else if (strEQL(command, "Mesh")) {
                  if (nCoulmn == 4){
                     demInjector.InjectMesh(param1, param2, param3);
                  } else {
                     printf("Illegal number of parameters in Mesh command\n");
                  }
               }
            }
            fclose(paraMfile);
         } else {
            for (int i = 6; i < argc; i++) {
               demInjector.InjectFile(argv[i]);
            }
         }
         demInjector.flushObjectsAndClose();
      }
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
   printf("\n\nEnter a character to stop the program.\n");
   getchar();
   exit(0);
}



