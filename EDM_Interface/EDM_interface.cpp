

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <map>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

extern "C" {
#include "sdai.h"
#include "cpp10_EDM_interface.h"
}

#include "container.h"

using namespace std;

#include "EDM_interface.h"


/*===============================================================================================*/
void EDM_interface::setCurrentModel(const char *modelName)
/*===============================================================================================*/
{
   m = models[modelName];
   if (m == NULL) {
      m = new Model(currentRepository, &model_ma, currentSchema);
      models[modelName] = m;
      SdaiModel modelID = sdaiCreateModelBN(currentRepository->getRepositoryId(), (char*)modelName, currentSchemaName);
      m->open((char*)modelName, sdaiRW);
   }
}
/*===============================================================================================*/
void EDM_interface::setRemoteModel(char *modelName, char *_serverName, int _serverPort)
/*===============================================================================================*/
{
   m = models[modelName];
   if (m == NULL) {
      sm = new SerializableModel(currentRepository, &model_ma, currentSchema, &extra_ma);
      models[modelName] = m = sm;
   }
   serverName = _serverName;
   serverPort = _serverPort;
}
/*===============================================================================================*/
void EDM_interface::sendObjectsToServer()
/*===============================================================================================*/
{
   sm->serializeObjectsInBuffer();
}