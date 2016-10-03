
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
   EDMLONG32                              cTimestepId;

   Database                               *db;

   Model                                  *m; // The opened EDM model
   Repository                             *currentRepository;
   CMemoryAllocator                       model_ma;
   CMemoryAllocator                       *plugin_ma;
   dbSchema                               *currentSchema;
   char                                   *currentSchemaName;

   // Data when injecting from files
   char                                   line[10240];
   char                                   *bytesRead;
   char                                   *col[MAX_COLUMNS];
   bool                                   newResultName(char*);
   bool                                   newTimestep(double ts);
public:
   FILE                                   *fp;
   char                                   *injectorFileName;
   int                                    cLineno;
   EDMLONG                                maxNodeId;
   EDMLONG                                maxElementId;
   Container<char*>                       *resultNames;
   Container<double>                      *temesteps;

   FEM_InjectorHandler(dbSchema *s)  ;
   ~FEM_InjectorHandler()  ;
   
   void                                   setDatabase(Database *_db) { db = _db; }
   void                                   setCurrentSession(const char *sessionID) { } 
   void                                   setCurrentModel(const char *modelName);
   void                                   setCurrentRepository(Repository *r) { currentRepository = r; }
   void                                   setCurrentSchemaName(const char *sn) { currentSchemaName = (char*)sn; }
   int                                    scanInputLine();
   void                                   initAnalyze(CMemoryAllocator *ma);
   void                                   InjectMeshFile();
   void                                   AnalyzeMeshFile();
   void                                   InjectResultFile();
   void                                   AnalyzeResultFile();
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
