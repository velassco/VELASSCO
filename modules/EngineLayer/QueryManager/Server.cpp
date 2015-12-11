
// ***
// *** Whithin the VELaSSCo AccessLib this server is only for testing purposes.
// ***

#include <assert.h>

// STL
#include <sstream>

// Thrift
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
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
#include "Analytics.h"
#include "Graphics.h"

#include "Server.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCo;

// internal information:
std::string QMS_FullyQualifiedModelName::GetDBType () const {
  // this.location is of the form: EDM or HBase:table_name
  const std::string &location = this->location;
  std::size_t found = location.find_first_of( ":");
  if ( found == std::string::npos)
    found = location.length();
  else
    found--; // get rid of ':'
  std::string ret = location.substr( 0, found);
  assert( AreEqualNoCase( ret, "edm") || AreEqualNoCase( ret, "hbase"));
  return ret;
}

void QMS_FullyQualifiedModelName::GetTableNames( std::string &model_table, std::string &metadata_table, std::string &data_table) const {
  const std::string &location = this->location;
  std::size_t found = location.find_first_of( ":");
  bool no_table_name = false;
  if ( found == std::string::npos) {
    no_table_name = true;
    found = location.length();
  }
  std::string db_type = location.substr( 0, found);
  if ( AreEqualNoCase( db_type, "hbase")) {
    std::string table_name;
    if ( no_table_name) {
      table_name = "VELaSSCo_Models";
    } else {
      table_name = location.substr( found + 1, location.length());
    }
    if ( AreEqualNoCase( table_name, "VELaSSCo_Models")) {
      model_table = "VELaSSCo_Models";
      metadata_table = "Simulations_Metadata";
      data_table = "Simulations_Data";
    } else if ( AreEqualNoCase( table_name, "VELaSSCo_Models_V4CIMNE")) {
      model_table = "VELaSSCo_Models_V4CIMNE";
      metadata_table = "Simulations_Metadata_V4CIMNE";
      data_table = "Simulations_Data_V4CIMNE";
    } else if ( AreEqualNoCase( table_name, "VELaSSCo_Models_V4CIMNE_Test")) {
      model_table = "VELaSSCo_Models_V4CIMNE_Test";
      metadata_table = "Simulations_Metadata_V4CIMNE_Test";
      data_table = "Simulations_Data_V4CIMNE_Test";
    } else if ( AreEqualNoCase( table_name, "Test_VELaSSCo_Models")) {
      model_table == "Test_VELaSSCo_Models";
      metadata_table = "Test_Simulations_Metadata";
      data_table = "Test_Simulations_Data";
    } else if ( AreEqualNoCase( table_name, "T_VELaSSCo_Models")) {
      model_table == "T_VELaSSCo_Models";
      metadata_table = "T_Simulations_Metadata";
      data_table = "T_Simulations_Data";
    } else {
      assert( 0 && "Unkonwn data table name"); //  + table_name
    }
  }
}

/*
 * function offered by the QueryManager 
 */
void QueryManagerServer::Query(Query_Result& _return, const SessionID sessionID, const std::string& query)
{
  LOGGER                          << std::endl;
  LOGGER << "----- Query() -----" << std::endl;

  LOGGER                                    << std::endl;
  LOGGER << "Input:"                        << std::endl;
  LOGGER << "  sessionID : "   << sessionID << std::endl;
  LOGGER << "  query     : \n" << query     << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }

  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  std::string name       = pt.get<std::string>("name");

  /* The first query */
  if ( name == "GetResultFromVerticesID") {
    ManageGetResultFromVerticesID( _return, sessionID, query);
    /* Session Queries */
  } else if ( name == "GetMeshDrawData") {
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
    /* Result Analysis Queries */
  } else if ( name == "GetBoundingBox") {
    ManageGetBoundingBox( _return, sessionID, query);
  } else if ( name == "GetDiscrete2Continuum") {
    ManageGetDiscrete2Continuum(_return, sessionID, query);
  }
  else {
    _return.__set_result( (Result::type)VAL_INVALID_QUERY );
    
    LOGGER                                    << std::endl;
    LOGGER << "Output:"                       << std::endl;
    LOGGER << "  result : " << _return.result << std::endl;
    
    return;
  }
}

TSimpleServer *QueryManagerServer::m_simpleServer = NULL;

int StartServer( const int server_port) {
  LOGGER << "Starting VELaSSCo Server..." << std::endl;

  int port = server_port;
  LOGGER << "  using port: " << port << std::endl;

  boost::shared_ptr<QueryManagerServer> handler(new QueryManagerServer());
  boost::shared_ptr<TProcessor> processor(new QueryManagerProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer *server = new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
  handler->SetSimpleServer( server);
  DEBUG( "  before serving ...");
  server->serve();
  DEBUG( "  after serving ...");
  exit( 0);
  return 0;
}
