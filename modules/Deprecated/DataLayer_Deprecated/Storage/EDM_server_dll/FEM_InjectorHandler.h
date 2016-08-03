
#include "EDM_Injector.h"

#define MAX_COLUMNS 20

class FEM_InjectorHandler : public EDM_Injector
{
   map<EDMULONG, fem::Mesh*>              meshes;
   map<EDMULONG, fem::Node*>              nodes;
   EDMULONG  nNodes;
   fem::Mesh                              *cMesh;
   fem::ResultHeader                      *cResultHeader;
   fem::Analysis                          *cAnalysis;
   fem::TimeStep                          *cTimeStep;

   // Data when injecting from files
   char                                   line[10240];
   char                                   *bytesRead;
   char                                   *col[MAX_COLUMNS];
public:
   FILE                                   *fp;
   char                                   *injectorFileName;
   int                                    cLineno;

   FEM_InjectorHandler(dbSchema *s)  {
      currentSchema = s; cLineno = 0; cAnalysis = NULL; cTimeStep = NULL;
   }
   
   int scanInputLine();
   void InjectMeshFile();
   void InjectResultFile();
   char *readNextLine();
   void flushObjectsAndClose();
   void DeleteCurrentModelContent();
   void InitiateFileInjection();
   void printError(char *msg) {
      printf("Error: %s\n   File: %s, line %d", msg, injectorFileName, cLineno);
   }
   fem::Analysis                          *getAnalysis(char *name);
   fem::TimeStep                          *getTimeStep(fem::Analysis *an, double ts);
};
