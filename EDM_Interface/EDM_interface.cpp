

#include "stdafx.h"

#include "EDM_interface.h"



void EDM_interface::setCurrentModel(const char *modelName)
{
   m = models[modelName];
   if (m == NULL) {
      models[modelName] = m = new Model(currentRepository, &model_ma, currentSchema);
      SdaiModel modelID = sdaiCreateModelBN(currentRepository->getRepositoryId(), (char*)modelName, currentSchemaName);
      m->open((char*)modelName, sdaiRW);
   }
}