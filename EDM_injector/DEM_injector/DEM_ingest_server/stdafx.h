// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <errno.h>
#include <string>


#include "DEM_Injector.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace  std;


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

extern "C" {
#include "sdai.h"
#include "cpp10_EDM_interface.h"
}

#include "..\..\..\EDM_Interface\dem_schema_velassco_entityTypes.h"
#include "container.h"
#include "..\..\..\EDM_Interface\dem_schema_velassco.hpp"
using namespace dem;
using namespace std;



// TODO: reference additional headers your program requires here