// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


extern "C" {
#include "sdai.h"
#include "cpp10_EDM_interface.h"
}

#include "../EDM/gen-cpp/VELaSSCoSM.h"

#include "dem_schema_velassco_entityTypes.h"
#include "fem_schema_velassco_entityTypes.h"
#include "container.h"
#include "dem_schema_velassco.hpp"
#include "fem_schema_velassco.hpp"

#include "CppParameterClass.h"

using namespace dem;
using namespace fem;
using namespace VELaSSCoSM;


// TODO: reference additional headers your program requires here
