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

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include "boost/program_options.hpp" 

#include "..\..\ThriftInjector\Inject\Logger.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "..\..\ThriftInjector\Inject\ParseAsciiMesh.h"
#include "..\..\ThriftInjector\Inject\ParseAsciiResult.h"
#include "..\..\ThriftInjector\Inject\BinarySerialization.h"

using namespace boost::filesystem;
using boost::lexical_cast;
using boost::bad_lexical_cast;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

//using namespace apache::hadoop::hbase::thrift;

typedef std::vector<std::string> StrVec;
typedef std::map<std::string, std::string> StrMap;
//typedef std::vector<ColumnDescriptor> ColVec;
//typedef std::map<std::string,ColumnDescriptor> ColMap;
//typedef std::vector<TCell> CellVec;
//typedef std::map<std::string,TCell> CellMap;

typedef std::map<int, path> PathContainerType;


#define SUCCESS 0
#define ERROR_IN_COMMAND_LINE 1



#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

extern "C" {
#include "sdai.h"
#include "cpp10_EDM_interface.h"
}

#include "..\..\EDM_Interface\fem_schema_velassco_entityTypes.h"
#include "container.h"
#include "..\..\EDM_Interface\fem_schema_velassco.hpp"
using namespace fem;
using namespace std;



// TODO: reference additional headers your program requires here
