
#include "..\..\..\EDM_Interface\EDM_interface.h"

#define MAX_COLUMNS 20

class FEM_InjectorHandler : public EDM_interface
{
   map<EDMULONG, fem::Mesh*>              meshes;
   map<EDMULONG, fem::Node*>              nodes;
   EDMULONG  nNodes;
   fem::Mesh                              *cMesh;
   fem::ResultHeader                      *cResultHeader;
   fem::ResultBlock                       *cResultBlock;

   // Data when injecting from files
   char                                   line[10240];
   char                                   *bytesRead;
   char                                   *col[MAX_COLUMNS];
public:
   FILE                                   *fp;
   char                                   *injectorFileName;
   int                                    cLineno;

   FEM_InjectorHandler(dbSchema *s)  {
      currentSchema = s; cLineno = 0;
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
};
