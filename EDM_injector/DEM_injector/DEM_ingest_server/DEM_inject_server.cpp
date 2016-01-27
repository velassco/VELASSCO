// DEM_ingest_server.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <time.h>



using namespace  ::DEM_Inject;

#include "DEM_InjectorHandler.h"
#include "FEM_InjectorHandler.h"


char *allocString(char *s)
{
   char *d = (char*)malloc(strlen(s) + 1); strcpy(d, s);
   if (*d == '\"') d++;
   if (d[strlen(d) - 1] == '\"') d[strlen(d) - 1] = 0;
   return d;
}


int main(int argc, char **argv)
{
   int rstat;
   char errTxt[1024];
   FILE *paraMfile = NULL;
   int startTime = GetTickCount();
   bool batchMode = false;

   try {
      char *command, *dbFolder = "", *dbName = "", *dbPassword = "", *model = "", *repository = NULL;

      if (argc > 2) {
         command = allocString(argv[1]);
      }
      if ((argc == 2 || argc == 3) && (paraMfile = fopen(argv[1], "r"))) {
         char line[2048], a[1024], b[1024];

         for (int i = 0; i < 6 && fgets(line, sizeof(line), paraMfile); i++) {
            sscanf(line, "%s %s", a, b);
            if (strEQL(a, "command")) command = allocString(b);
            else if (strEQL(a, "dbFolder")) dbFolder = allocString(b);
            else if (strEQL(a, "dbName")) dbName = allocString(b);
            else if (strEQL(a, "dbPassword")) dbPassword = allocString(b);
            else if (strEQL(a, "model")) model = allocString(b);
            else if (strEQL(a, "repository")) repository = allocString(b);
         }
         if (argc == 3 && strEQL(argv[2], "batchMode")) batchMode = true;
      } else if (argc >= 5 && (strEQL(command, "Server") || strEQL(command, "Files") || strEQL(command, "FEMfiles"))) {
         dbFolder = argv[2];
         dbName = argv[3];
         dbPassword = argv[4];
      } else if (argc == 4 && strEQL(command, "Client")) {
      } else {
         printf("The VELaSSCo Data Injector for EDM has the command line parameters:\n   1. Command - can be either \"Server\", \"Files\", \"Help\", \n   2. Database folder\n   3. Database name\n   4. Database password\n");
         printf("\n   If Command is \"Server\", parameter no 5 is communication port number.\n");
         printf("   If Command is \"Files\", parameter no 5 is model name and the following\n   parameters are file names of the files that shall be injected into EDM.\n");
         exit(0);
      }
      int port = 9090;
      char *repositoryName;

      if (repository) {
         repositoryName = repository;
      } else if (strEQL(command, "FEMfiles")) {
         repositoryName = "FEM_models";
      } else {
         repositoryName = "DEM_models";
      }

      if (strEQL(command, "RemoteInject")) {
         /* Injection is done by the database server via the dll Injector_dll */

      } else {
         /* Injection is done to the standalone Database VELaSSCo_db */
         DEM_InjectorHandler demInjector(&dem_schema_velassco_SchemaObject);
         FEM_InjectorHandler femInjector(&fem_schema_velassco_SchemaObject);
         Database VELaSSCo_db(dbFolder, dbName, dbPassword);
         Repository demRepository(&VELaSSCo_db, repositoryName);
         char commandline[2048], filecommand[1024], file_name[1024], param1[1024], param2[1024], param3[1024];

         if (strEQL(command, "Server") || strEQL(command, "Files")) {
            demInjector.setCurrentSchemaName("dem_schema_velassco");
            demInjector.setDatabase(&VELaSSCo_db);
            VELaSSCo_db.open();
            demRepository.open(sdaiRW);
            demInjector.setCurrentRepository(&demRepository);
         } else if (strEQL(command, "FEMfiles")) {
            femInjector.setCurrentSchemaName("fem_schema_velassco");
            femInjector.setDatabase(&VELaSSCo_db);
            VELaSSCo_db.open();
            demRepository.open(sdaiRW);
            femInjector.setCurrentRepository(&demRepository);
         }
         if (strEQL(command, "Server")) {
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
         } else if (strEQL(command, "FEMfiles")) {
            femInjector.setCurrentModel(model);
            femInjector.DeleteCurrentModelContent();
            if (paraMfile) {
               while (fgets(commandline, sizeof(commandline), paraMfile)){
                  int nCoulmn = sscanf(commandline, "%s %s %s %s", filecommand, file_name, param2, param3);
                  if (strEQL(filecommand, "File")) {
                     if (nCoulmn == 2){
                        femInjector.injectorFileName = file_name;
                        femInjector.fp = fopen(file_name, "r"); femInjector.cLineno = 0;
                        if (femInjector.fp) {
                           int extPos = strlen(file_name) - 9;
                           if (extPos > 0 && strnEQL(file_name + extPos, ".post.msh", 9)) {
                              femInjector.InjectMeshFile();
                           } else if (extPos > 0 && strnEQL(file_name + extPos, ".post.res", 9)) {
                              femInjector.InjectResultFile();
                           }
                           fclose(femInjector.fp);
                        } else {
                           femInjector.printError("Illegal FEM file name");
                        }
                     } else {
                        femInjector.printError("Illegal number of parameters in File command.");
                     }
                  }
               }
               fclose(paraMfile);
               femInjector.flushObjectsAndClose();
            }
         } else if (strEQL(command, "Files") || strEQL(command, "Client")) {
            if (strEQL(command, "Files")) {
               demInjector.setCurrentModel(model);
               demInjector.DeleteCurrentModelContent();
            } else {
               demInjector.setRemoteModel(argv[2], "localhost", 9090);
               paraMfile = fopen(argv[3], "r");
            }
            demInjector.InitiateFileInjection();

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
               if (strEQL(command, "Files")) {
                  demInjector.flushObjectsAndClose();
               } else {
                  demInjector.sendObjectsToServer();
               }
            } else {
               for (int i = 6; i < argc; i++) {
                  demInjector.InjectFile(argv[i]);
               }
               demInjector.sendObjectsToServer();
            }
         }
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
   int endTime = GetTickCount();
   printf("\n\nTime used: %d milliseconds\n", endTime - startTime);
   if (!batchMode) {
      printf("\n\nEnter a character to stop the program.\n");
      getchar();
   }
   exit(0);
}



