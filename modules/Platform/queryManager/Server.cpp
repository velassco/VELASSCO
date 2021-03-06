
// ***
// *** Whithin the VELaSSCo AccessLib this server is only for testing purposes.
// ***

#include <assert.h>

// STL
#include <sstream>

// Thrift
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

// Boost
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

// VELaSSCo
// AccessLib.h is only used for the return codes ( enum values)
#include "../../AccessLib/AccessLib/AccessLib.h"
#include "Helpers.h"

// Generated code
#include "../../thrift/QueryManager/gen-cpp/QueryManager.h"

#include "DataLayerAccess.h"
//#include "Analytics.h"
#include "Graphics.h"
#include "Server.h"
#include "globalSettings.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCo;

// used for the common operations, like:
// GetMeshInfoFromMeshName
extern VELaSSCo_Operations *queryServer;

/*
 * function offered by the QueryManager 
 */
void QueryManagerServer::Query(Query_Result& _return, const SessionID sessionID, const std::string& query)
{
  LOGGER                          << std::endl;
  LOGGER << "----- Query() -----" << std::endl;

  LOGGER                                    				<< std::endl;
  LOGGER << "Input:"                        				<< std::endl;
  LOGGER << "  sessionID : "   << sessionID 				<< std::endl;
  LOGGER << "  query     : \n" << Strdump(query, 1024)      << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }

  std::string name       = "";
  // Parse query JSON
  try {
    std::istringstream ss(query);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);
    name       = pt.get<std::string>("name");
  } catch ( ...) {
    LOGGER << "EXCEPTION parsing JSON queryData" << std::endl;
    _return.__set_result( (Result::type)VAL_INVALID_QUERY_PARAMETERS);
    _return.__set_data( "EXCEPTION parsing JSON queryData");
    return;
  }

  /* The first query */
  if ( name == "GetResultFromVerticesID") {
    ManageGetResultFromVerticesID( _return, sessionID, query);
    /* Session Queries */
  } else if( name == "GetEvolutionOfVertex") {
    ManageGetEvolutionOfVertex( _return, sessionID, query);
  }else if ( name == "GetMeshDrawData") {
    ManageGetMeshDrawData( _return, sessionID, query);
  }else if ( name == "GetListOfModels") {
    ManageGetListOfModels( _return, sessionID, query);
  } else if ( name == "OpenModel") {
    ManageOpenModel( _return, sessionID, query);
  } else if ( name == "CloseModel") {
    ManageCloseModel( _return, sessionID, query);
    /* Direct Result Queries */
  } else if ( name == "GetListOfMeshes") {
    ManageGetListOfMeshes( _return, sessionID, query);
  } else if ( name == "GetListOfAnalyses") {
    ManageGetListOfAnalyses( _return, sessionID, query);
  } else if ( name == "GetListOfTimeSteps") {
    ManageGetListOfTimeSteps( _return, sessionID, query);
  } else if ( name == "GetListOfResults") {
    ManageGetListOfResults( _return, sessionID, query);
  } else if ( name == "GetMeshVertices") {
    ManageGetMeshVertices( _return, sessionID, query);
    /* Result Analysis Queries */
  } else if ( name == "GetBoundingBox") {
    ManageGetBoundingBox( _return, sessionID, query);
  } else if ( name == "DeleteBoundingBox") {
    ManageDeleteBoundingBox( _return, sessionID, query);
  } else if ( name == "GetDiscrete2Continuum") {
    ManageGetDiscrete2Continuum(_return, sessionID, query);
  } else if ( name == "GetBoundaryOfAMesh") {
    ManageGetBoundaryOfAMesh( _return, sessionID, query);
  } else if ( name == "DeleteBoundaryOfAMesh") {
    ManageDeleteBoundaryOfAMesh( _return, sessionID, query);
  } else if ( name == "GetIsoSurface") {
    ManageGetIsoSurface( _return, sessionID, query);
  } else if ( name == "GetSimplifiedMesh") {
    ManageGetSimplifiedMesh( _return, sessionID, query);
  } else if ( name == "DeleteSimplifiedMesh") {
    ManageDeleteSimplifiedMesh( _return, sessionID, query);
  } else if ( name == "GetSimplifiedMeshWithResult") {
    ManageGetSimplifiedMeshWithResult( _return, sessionID, query);
  } else if ( name == "DeleteSimplifiedMeshWithResult") {
    ManageDeleteSimplifiedMeshWithResult( _return, sessionID, query);
  } else if ( name == "DeleteAllCalculationsForThisModel") {
    ManageDeleteAllCalculationsForThisModel( _return, sessionID, query);
  } else if ( name == "GetConfiguration") {
    ManageGetConfiguration( _return, sessionID, query);
  } else if ( name == "SetConfiguration") {
    ManageSetConfiguration( _return, sessionID, query);
  } else if ( name == "ComputeVolumeLRSplineFromBoundingBox") {
    ManageGetVolumeLRSplineFromBoundingBox( _return, sessionID, query);
  } else if ( name == "DeleteVolumeLRSplineFromBoundingBox") {
    ManageDeleteVolumeLRSplineFromBoundingBox( _return, sessionID, query);
  } else if ( name == "DoStreamlinesWithResults" ) {
    ManageDoStreamlinesWithResult( _return, sessionID, query );
  } else {
    _return.__set_result( (Result::type)VAL_INVALID_QUERY );
    
    LOGGER                                    << std::endl;
    LOGGER << "Output:"                       << std::endl;
    LOGGER << "  result : " << _return.result << std::endl;
  }
  size_t num_bytes = _return.data.size();
  LOGGER << "  --> result size: " << GetNiceSizeString( sizeof( _return.result) + num_bytes) << " to send back." << std::endl;

  // compress data:
  if ( m_compression_enabled && num_bytes) {
    std::string *compressed_result = NULL;
    bool ok = m_compression.doCompress( _return.data, &compressed_result);
    LOGGER << "  --> compressed with " << ( ok ? "no error" : "ERROR") << std::endl;
    if ( compressed_result) {
      int lvl = m_compression.getCompressionLevel();
      if ( m_compression.getCompressionType() != VL_Compression::Zlib) {
	lvl = 0;
      }
      std::string type = m_compression.getCompressionTypeString();
      if ( lvl)
	type += " " + std::to_string( ( long long int)lvl);
      double percent = 100.0 * ( double)( compressed_result->size() - 12) / ( double)num_bytes;
      LOGGER << "        to " << GetNiceSizeString( compressed_result->size() - 12) // header
	     << " from " << GetNiceSizeString( num_bytes) << " ( " << percent << "% "
	     << type << " )" << std::endl; // header
      _return.__set_data( *compressed_result);
      delete compressed_result;
      // useful to debug:
      // LOGGER << "  data   : \n" << Hexdump(_return.data, 128) << std::endl;
      compressed_result = NULL;
    }
  }
}

TSimpleServer *QueryManagerServer::m_simpleServer = NULL;
TThreadedServer *QueryManagerServer::m_threadedServer = NULL;

int StartServer( const int server_port) {
  LOGGER << "Starting VELaSSCo Server..." << std::endl;


  // AUTHOR: iCores. This code allows multi-user and single-user behaviour.
  bool multiUserSupport = getMultiUserSetting() ? true : false;
	char *valueEnvV = NULL;
	valueEnvV = getenv("MULTIUSER_VELASSCO");
	if (valueEnvV)
	{
		if (atoi(valueEnvV))
		{
			std::cout << "\nWARNING: Multiuser support enabled. Under development ... \n" << std::endl;		
			multiUserSupport =  true;
		}
	}	

	if (multiUserSupport)
	{
		//New behaviour. Multiple users per port.
		  int port = server_port;
		  LOGGER << "  using port: " << port << std::endl;

		  boost::shared_ptr<QueryManagerServer> handler(new QueryManagerServer());
		  boost::shared_ptr<TProcessor> processor(new QueryManagerProcessor(handler));
		  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
		  boost::shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
		  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

		  
		//FIXME: NOT SURE IF THIS IS NECCESSARY OR NOT, ThreadManager only for TThreadedPoolServer? FUTURE OPTIMIZATION?
		// using thread pool with maximum 64 threads to handle incoming requests. Client hangs if there is no more thread available in the thread pool.
		//  const int workerCount = 64;
		//  boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(workerCount);
		//  boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
		//  threadManager->threadFactory(threadFactory);
		//  threadManager->start();
		// END FIXME
		  
		  TThreadedServer *server = new TThreadedServer(processor, serverTransport, transportFactory, protocolFactory);
		  
		  handler->SetThreadedServer( server);
		  handler->SetDefaultCompression();
		  DEBUG( "  Launching TThreadedServer, before serving ...");
		  server->serve();
		  DEBUG( "  after serving ...");
		  DEBUG( "  deleting server ...");
                  handler->SetThreadedServer( NULL); // delete server
	}
	else
	{
		//Old behaviour. Sigle user per port.
		  int port = server_port;
		  LOGGER << "  using port: " << port << std::endl;

		  boost::shared_ptr<QueryManagerServer> handler(new QueryManagerServer());
		  boost::shared_ptr<TProcessor> processor(new QueryManagerProcessor(handler));
		  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
		  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
		  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

		  TSimpleServer *server = new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
		  handler->SetSimpleServer( server);
		  handler->SetDefaultCompression();
		  DEBUG( "  before serving (Simple server) ...");
		  server->serve();
		  DEBUG( "  after serving ...");
		  DEBUG( "  deleting server ...");
                  handler->SetSimpleServer( NULL); // delete server
	}
	// END: AUTHOR: iCores. This code allows multi-user and single-user behaviour.
  
  DEBUG( "DONE");
  exit( 0); // need this to exit QueryManager daemon
  return 0; // return only, will close connections but still remain asking for commands interactively ...
}

// returns -1 if not found, and an error messare in str_error
// resultStatus may be one of: 
//   VAL_NO_MESH_INFORMATION_FOUND, VAL_NO_MESH_INFORMATION_FOUND, VAL_NO_MESH_INFORMATION_FOUND
VAL_Result QueryManagerServer::GetMeshInfoFromMeshName( const std::string &dl_sessionID, const std::string &modelID, 
							const std::string &analysisID, double stepValue,
							const std::string &meshName, 
							MeshInfo &meshInfo,
							std::string &error_str) {
  // from the mesh name, get the Mesh Info ( number, element type, ...)
  // eventually the Mesh information could have been cached ...
  VAL_Result resultStatus = VAL_UNKNOWN_ERROR;

  std::cout << "looking for the Mesh " << meshName << " in order to get it's id" << std::endl;
  rvGetListOfMeshes returnQueryData;
  queryServer->getListOfMeshes( returnQueryData, dl_sessionID, modelID, analysisID, stepValue);
  int meshID = -1;
  if ( returnQueryData.meshInfos.size() == 0) {
    resultStatus = VAL_NO_MESH_INFORMATION_FOUND;
    error_str = "There is no mesh metadata.";
    std::cout << error_str << std::endl;
  } else {
    for ( std::vector< MeshInfo>::iterator it = returnQueryData.meshInfos.begin();
          it != returnQueryData.meshInfos.end(); it++) {
      if ( AreEqualNoCase( it->name, meshName)) {
	meshID = it->meshNumber;
	meshInfo = *it;
	resultStatus = VAL_SUCCESS;
	std::cout << "  found = " << meshID << std::endl;
	break;
      }
    }
    if ( meshID == -1) { // not found
      resultStatus = VAL_MESH_ID_NOT_FOUND;
      error_str = "Mesh name " + meshName + " not in metadata.";
      std::cout << error_str << std::endl;
    }
  }

  return resultStatus;
}

