
// ***
// *** Whithin the VELaSSCo AccessLib this server is only for testing purposes.
// ***

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
#include "AccessLib.h"
#include "Helpers.h"

// Generated code
#include "../../thrift/QueryManager/gen-cpp/QueryManager.h"



#include "DemoServer.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCo;


// ***************************************************************************
//
// Actual implementation of QueryManager functions.
//
// This is a derived class, so that our own code does not get overwritten
// when the Thrift interface needs to be regenerated.
//
// ***************************************************************************

void QM_DemoServer::UserLogin(UserLogin_Result& _return, const std::string& url, const std::string& name, const std::string& password)
{
  LOGGER                              << std::endl;
  LOGGER << "----- UserLogin() -----" << std::endl;

  LOGGER                                << std::endl;
  LOGGER << "Input:"                    << std::endl;
  LOGGER << "  url      : " << url      << std::endl;
  LOGGER << "  name     : " << name     << std::endl;
  LOGGER << "  password : " << password << std::endl;

  if (   (name == "velassco" && password == "VELaSSCo")
	 || (name == "andreas" && password == "1234")
	 || (name == "miguel" && password == "1234") )
    {
      // Login time
      std::string time = boost::posix_time::to_simple_string(boost::posix_time::second_clock::universal_time());

      // Generate a session id randomly
      SessionID sessionID;
      do
	{
	  const boost::uuids::uuid uuid = boost::uuids::random_generator()();
	  sessionID = *((SessionID*)(uuid.data));
	}
      while (ValidSessionID(sessionID));

      // Store user
      User user = { name, time };
      m_users[sessionID] = user;

      // Return session id
      _return.__set_result( (Result::type)VAL_SUCCESS );
      _return.__set_sessionID( sessionID );
    }		
  else
    {
      _return.__set_result( (Result::type)VAL_USER_NOT_ACCEPTED );
      _return.__set_sessionID( 0 );
    }

  LOGGER                                          << std::endl;
  LOGGER << "Output:"                             << std::endl;
  LOGGER << "  result    : " << _return.result    << std::endl;
  LOGGER << "  sessionID : " << _return.sessionID << std::endl;
}

void QM_DemoServer::UserLogout(UserLogout_Result& _return, const SessionID sessionID)
{
  LOGGER                               << std::endl;
  LOGGER << "----- UserLogout() -----" << std::endl;

  LOGGER                                  << std::endl;
  LOGGER << "Input:"                      << std::endl;
  LOGGER << "  sessionID : " << sessionID << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }

  // Remove session ID
  {
    m_users.erase(sessionID);
  }

  _return.__set_result( (Result::type)VAL_SUCCESS );

  LOGGER                                    << std::endl;
  LOGGER << "Output:"                       << std::endl;
  LOGGER << "  result : " << _return.result << std::endl;
}

void QM_DemoServer::Query(Query_Result& _return, const SessionID sessionID, const std::string& query)
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

  if ( name == "GetResultFromVerticesID") {
    ManageGetResultFromVerticesID( _return, sessionID, query);
  } else if ( name == "GetListOfModels") {
    ManageGetListOfModels( _return, sessionID, query);
  } else if ( name == "OpenModel") {
    ManageOpenModel( _return, sessionID, query);
  } else if ( name == "GetBoundingBox") {
    ManageGetBoundingBox( _return, sessionID, query);
  } else {
    _return.__set_result( (Result::type)VAL_INVALID_QUERY );
    
    LOGGER                                    << std::endl;
    LOGGER << "Output:"                       << std::endl;
    LOGGER << "  result : " << _return.result << std::endl;
    LOGGER << "  Invalid Query" << std::endl;
    
    return;
  }
}

void QM_DemoServer::GetStatusDB(StatusDB_Result& _return, const SessionID sessionID) {
  LOGGER                              << std::endl;
  LOGGER << "----- GetStatusDB() -----" << std::endl;

  LOGGER                                << std::endl;
  LOGGER << "Input:"                      << std::endl;
  LOGGER << "  sessionID : " << sessionID << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }
  // Return session id
  _return.__set_result( (Result::type)VAL_SUCCESS );
  _return.__set_status( (std::string)"Test_Server OK");

  LOGGER                                          << std::endl;
  LOGGER << "Output:"                             << std::endl;
  LOGGER << "  result    : " << _return.result    << std::endl;
  LOGGER << "  status    : " << _return.status << std::endl;
}

void QM_DemoServer::ManageGetResultFromVerticesID( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  // std::istringstream ss(query);
  // boost::property_tree::ptree pt;
  // boost::property_tree::read_json(ss, pt);
  // 
  // std::string name       = pt.get<std::string>("name");
  // std::string modelID    = pt.get<std::string>("modelID");
  // std::string resultID   = pt.get<std::string>("resultID");
  // std::string analysisID = pt.get<std::string>("analysisID");
  // std::string vertexIDs  = as_string<size_t>(pt, "vertexIDs");
  // double      timeStep   = pt.get<double>("timeStep");
  // 
  // stringstream listOfVertices;
  // listOfVertices << "{\"id\":" << "[]" /*vertexIDs*/ << "}";
  // 
  // std::stringstream sessionIDStr;
  // sessionIDStr << sessionID;
  // 
  // std::string _return_;
  // 
  // std::cout << "S " << sessionID  << std::endl;
  // std::cout << "M " << modelID    << std::endl;
  // std::cout << "R " << resultID   << std::endl;
  // std::cout << "A " << analysisID << std::endl;
  // std::cout << "V " << vertexIDs  << std::endl;
  // std::cout << "T " << timeStep   << std::endl;
  
  std::vector<int64_t> resultVertexIDs;
  std::vector<double>  resultValues;
  
  // vertex #1
  resultVertexIDs.push_back(6);
  resultValues.push_back(-0.105564);
  resultValues.push_back(-0.287896);
  resultValues.push_back(-0.377642);
  // vertex #2
  resultVertexIDs.push_back(7);
  resultValues.push_back( 0.259839);
  resultValues.push_back(-0.366375);
  resultValues.push_back(-0.377652);
  
  // Pack into string
  std::string result;
  result += "2 3\n";
  result += std::string((char*)(&resultVertexIDs[0]), sizeof(int64_t)*resultVertexIDs.size());
  result += std::string((char*)(&resultValues[0]),    sizeof(double)*resultValues.size());
  _return.__set_data(result); 
  
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
}

void QM_DemoServer::ManageGetListOfModels( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // // Parse query JSON
  // std::istringstream ss(query);
  // boost::property_tree::ptree pt;
  // boost::property_tree::read_json(ss, pt);
  // 
  // std::string name               = pt.get<std::string>("name");
  // std::string group_qualifier    = pt.get<std::string>("groupQualifier");
  // std::string name_pattern       = pt.get<std::string>("namePattern");
  // 
  // std::stringstream sessionIDStr;
  // sessionIDStr << sessionID;
  // 
  // std::cout << "S " << sessionID       << std::endl;
  // std::cout << "G " << group_qualifier << std::endl;
  // std::cout << "P " << name_pattern    << std::endl;
  // 
  // 
  // rvGetListOfModels _return_;
  // queryManagerModule::Instance()->getListOfModels( _return_,
  // 						   sessionIDStr.str(), group_qualifier, name_pattern);
  // 		  
  // std::cout << _return_ << std::endl;

  std::ostringstream oss;
  oss << "NumberOfModels: " << 2 << std::endl;
  oss << "Name: " << "DEM_examples/FluidizedBed_small" << std::endl;
  oss << "FullPath: " << "/localfs/home/velassco/common/simulation_files/DEM_examples/FluidizedBed_small" << std::endl;
  oss << "Name: " << "fine_mesh-ascii_" << std::endl;
  oss << "FullPath: " << "../../../../../VELASSCO-Data/Telescope_128subdomains_ascii" << std::endl;
  _return.__set_data( oss.str());
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}

void QM_DemoServer::ManageOpenModel( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  _return.__set_data( "00112233445566778899aabbccddeeff");
  _return.__set_result( (Result::type)VAL_SUCCESS );

  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}

void QM_DemoServer::ManageCloseModel( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  _return.__set_result( (Result::type)VAL_SUCCESS );
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
}

void QM_DemoServer::ManageGetBoundingBox( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  double bbox[ 6] = { -0.5, -0.5, -0.5, 0.5, 0.5, 0.5};
  _return.__set_data( std::string( ( const char *)&bbox[ 0], 6 * sizeof( double)));
  _return.__set_result( (Result::type)VAL_SUCCESS );

  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  LOGGER << "  data   : \n" << Hexdump( _return.data, 128) << std::endl;
}


int StartServer( const int server_port) {
  LOGGER << "Starting VELaSSCo Server..." << std::endl;

  int port = server_port;
  LOGGER << "  using port: " << port << std::endl;

  shared_ptr<QM_DemoServer> handler(new QM_DemoServer());
  shared_ptr<TProcessor> processor(new QueryManagerProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  DEBUG( "  before serving ...");
  server.serve();
  DEBUG( "  after serving ...");
  return 0;
}
