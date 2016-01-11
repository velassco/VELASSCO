// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#define _CRT_SECURE_NO_WARNINGS 1

#include "gen-cpp\VELaSSCoSM.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::concurrency;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCoSM;

using namespace  std;


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

extern "C" {
#include "sdai.h"
#include "cpp10_EDM_interface.h"
}



#include "..\..\..\..\EDM_Interface\dem_schema_velassco_entityTypes.h"
#include "..\..\..\..\EDM_Interface\fem_schema_velassco_entityTypes.h"
#include "..\..\..\..\EDM_Interface\EDMcluster_entityTypes.h"
#include "container.h"
#include "..\..\..\..\EDM_Interface\dem_schema_velassco.hpp"
#include "..\..\..\..\EDM_Interface\fem_schema_velassco.hpp"
#include "..\..\..\..\EDM_Interface\EDMcluster.hpp"

using namespace dem;
using namespace fem;
using namespace std;
using namespace ecl;

#include "EDMclusterServices.h"




typedef struct BoundingBox {
   double min_x, min_y, min_z, max_x, max_y, max_z;
} BoundingBox;

// TODO: reference additional headers your program requires here
