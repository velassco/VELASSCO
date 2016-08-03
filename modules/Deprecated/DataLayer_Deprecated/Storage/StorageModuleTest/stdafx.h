// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "..\EDM\gen-cpp\VELaSSCoSM.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>


using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace VELaSSCoSM;

using namespace boost;

extern "C" {
#include "sdai.h"
#include "cpp10_EDM_interface.h"
}

#include "container.h"

#include "CppParameterClass.h"


#define strEQL(a, b) ! strcmp(a, b)
#define strnEQL(a, b, n) ! EDM_STRNCMP(a, b, n)
#define strNEQ(a, b) strcmp(a, b)
#define strnNEQ(a, b, n) EDM_STRNCMP(a, b, n)
#define safecpy(d, s) if (EDM_STRLEN(s) < EDM_SIZEOF(d)) strcpy(d, s); else {EDM_STRNCPY(d, s, EDM_SIZEOF(d) - 1); d[EDM_SIZEOF(d) - 1] = 0; }
#define safe_strcpy(d, s) if (EDM_STRLEN(s) >= EDM_SIZEOF(d)) {EDM_STRNCPY(d, s, EDM_SIZEOF(d)-1); d[EDM_SIZEOF(d)-1] = 0; } else strcpy(d, s);

#define CHECK(body) {SdaiErrorCode local_rstat = body; if (local_rstat) { cppError(local_rstat, __FILE__, __LINE__); }}
#define sdaiERRCHECK(body) {sdaiErrorQuery(); body; SdaiErrorCode rstat = sdaiErrorQuery(); if (rstat) { cppError(rstat, __FILE__, __LINE__);}}
#define THROW(rstat) { cppError(rstat, __FILE__, __LINE__); }
#define THROW1(rstat) { cppError(rstat); }
#define _THROW_(rstat) { cppError(rstat, __FILE__, __LINE__); }
#define CHECKerr(body) {SdaiErrorCode rstat = body; if (rstat) goto err;}


// TODO: reference additional headers your program requires here
