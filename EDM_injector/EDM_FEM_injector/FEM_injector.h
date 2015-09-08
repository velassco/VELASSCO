#include "..\..\EDM_Interface\EDM_interface.h"

class FEM_Injector : public EDM_interface
{
   map<GID::IdNode, fem::Node *> nodeMap;
public:
   FEM_Injector(dbSchema *s)  {
      currentSchema = s;
   }
   void                    storeFEM_mesh(GID::MeshResultType &mesh);
   void                    storeFEM_results(GID::ResultContainerType &results);
   void                    writeAndClose();
   void                    buildFemSearchStructures(char *modelName);
};
