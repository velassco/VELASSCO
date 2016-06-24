
#include "EDM_Injector.h"

#define MAX_COLUMNS 20

class FEM_InjectorHandler
{
   map<EDMULONG, fem::Mesh*>              meshes;
   map<EDMULONG, fem::Node*>              nodes;
   EDMULONG  nNodes;
   fem::Mesh                              *cMesh;
   fem::ResultHeader                      *cResultHeader;
   fem::Analysis                          *cAnalysis;
   fem::TimeStep                          *cTimeStep;

   Database                               *db;

   Model                                  *m; // The opened EDM model
   Repository                             *currentRepository;
   CMemoryAllocator                       model_ma;
   dbSchema                               *currentSchema;
   char                                   *currentSchemaName;

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
      currentRepository = NULL; model_ma.init(0x100000); m = NULL;
}
   
   void                                   setDatabase(Database *_db) { db = _db; }
   void                                   setCurrentSession(const char *sessionID) { } 
   void                                   setCurrentModel(const char *modelName);
   void                                   setCurrentRepository(Repository *r) { currentRepository = r; }
   void                                   setCurrentSchemaName(const char *sn) { currentSchemaName = (char*)sn; }
   int                                    scanInputLine();
   void                                   InjectMeshFile();
   void                                   InjectResultFile();
   char                                   *readNextLine();
   void                                   flushObjectsAndClose();
   void                                   DeleteCurrentModelContent();
   void                                   InitiateFileInjection();
   void                                   printError(char *msg) {
      printf("Error: %s\n   File: %s, line %d", msg, injectorFileName, cLineno);
   }
   fem::Analysis                          *getAnalysis(char *name);
   fem::TimeStep                          *getTimeStep(fem::Analysis *an, double ts);
};
