
#include "stdafx.h"


#include "EDM_Injector.h"


/*===============================================================================================*/
void EDM_Injector::setCurrentModel(const char *modelName)
/*===============================================================================================*/
{
   if (m == NULL) {
      m = new Model(currentRepository, &model_ma, currentSchema);
      EdmiError rstat = edmiDeleteModelBN(currentRepository->getRepositoryId(), (char*)modelName);
      SdaiModel modelID = edmiCreateModelBN(currentRepository->getRepositoryId(), (char*)modelName,
         currentSchemaName, NO_INSTANCE_REFERENCES | M_PACKED_MODEL);
      m->open((char*)modelName, sdaiRW);
   }
   CHECK(edmiStartModelWriteTransaction(m->modelId));
}
