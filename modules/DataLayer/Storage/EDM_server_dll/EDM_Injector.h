#pragma once

//#include "targetver.h"
//#include <stdlib.h>
//#include <errno.h>
//#include <stdio.h>
//#include <tchar.h>
//#include <string>
//#include <map>
//
//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//
//extern "C" {
//#include "sdai.h"
//#include "cpp10_EDM_interface.h"
//}
//
//#include "dem_schema_velassco_entityTypes.h"
//#include "container.h"
//#include "dem_schema_velassco.hpp"
//using namespace dem;
//
//using namespace std;


class EDM_Injector
{
   Database                         *db;
protected:
   Model                            *m; // The opened EDM model
   Repository                       *currentRepository;
   CMemoryAllocator                 model_ma;
   CMemoryAllocator                 extra_ma;
   dbSchema                         *currentSchema;
   char                             *currentSchemaName;
   char                             *serverName;
   int                              serverPort;
public:
   EDM_Injector()
   {
      currentRepository = NULL; model_ma.init(0x100000); m = NULL;
   }
   void                             setDatabase(Database *_db) { db = _db; }
   void                             setCurrentSession(const char *sessionID) { } 
   void                             setCurrentModel(const char *modelName);
   void                             setCurrentRepository(Repository *r) { currentRepository = r; }
   void                             setCurrentSchemaName(const char *sn) { currentSchemaName = (char*)sn; }
   void                             setRemoteModel(char *modelName, char *serverName, int serverPort);
   void                             sendObjectsToServer();
};