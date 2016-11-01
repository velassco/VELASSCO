// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WINDOWS
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif /* _WINDOWS */
#include <stdlib.h>
#include <map>

extern "C" {
#include "sdai.h"
#include "cpp10_EDM_interface.h"
}

#include "../../thrift/Storage/gen-cpp/VELaSSCoSM.h"
#include "../../thrift/Storage/gen-cpp/VELaSSCoSM_types.h"

#include "dem_schema_velassco_entityTypes.h"
#include "fem_schema_velassco_entityTypes.h"
#include "container.h"
#include "dem_schema_velassco.hpp"
#include "fem_schema_velassco.hpp"

#include "CppParameterClass.h"
//#include "Matrix.h"
#include "../edm_qm/CLogger.h"

using namespace dem;
using namespace fem;
using namespace std;

#ifdef _WINDOWS
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#ifdef _MSC_VER 
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

// TODO: reference additional headers your program requires here
