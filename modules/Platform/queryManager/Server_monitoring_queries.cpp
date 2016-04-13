
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
#include "../AccessLib/AccessLib/AccessLib.h"
#include "Helpers.h"

// Generated code
#include "../thrift/QueryManager/gen-cpp/QueryManager.h"

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

void QueryManagerServer::GetStatusDB(StatusDB_Result& _return, const SessionID sessionID) {
  LOGGER                              << std::endl;
  LOGGER << "----- GetStatusDB() -----" << std::endl;

  LOGGER                                << std::endl;
  LOGGER << "Input:"                        << std::endl;
  LOGGER << "  sessionID : "   << sessionID << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }

  std::string status;
  DataLayerAccess::Instance()->getStatusDB( status);
  _return.__set_result( (Result::type)VAL_SUCCESS );
  _return.__set_status( status );

  LOGGER                                          << std::endl;
  LOGGER << "Output:"                             << std::endl;
  LOGGER << "  result    : " << _return.result    << std::endl;
  LOGGER << "  status    : " << _return.status << std::endl;
}

void QueryManagerServer::StopVELaSSCo(StopVELaSSCo_Result& _return, const SessionID sessionID) {
  LOGGER                              << std::endl;
  LOGGER << "----- StopVELaSSCo() -----" << std::endl;

  LOGGER                                << std::endl;
  LOGGER << "Input:"                        << std::endl;
  LOGGER << "  sessionID : "   << sessionID << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }

  std::string status;
  // DataLayerAccess::Instance()->stopVELaSSCo( status);
  // do a user logout first
  UserLogout_Result logout_result;
  this->UserLogout( logout_result, sessionID);

  // stop Data Layer
  DataLayerAccess::Instance()->stopAll( );
  _return.__set_result( (Result::type)VAL_SUCCESS );
  status = " StorageModule and QueryManager servers stopped.";
  _return.__set_status( status );

  LOGGER                                          << std::endl;
  LOGGER << "Output:"                             << std::endl;
  LOGGER << "  result    : " << _return.result    << std::endl;
  LOGGER << "  status    : " << _return.status << std::endl;
  this->m_simpleServer->stop();
  this->SetSimpleServer( NULL);
}

void QueryManagerServer::ManageGetConfiguration( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string key_str      = pt.get<std::string>( "Key");
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S  -" << sessionID        << std::endl;
  std::cout << "K  -" << key_str       << std::endl;
  
  const char *key = key_str.c_str();
  if ( !key || !*key || ( key_str.size() == 0) || !strcasecmp( key, "List") || !strcasecmp( key, "All") || !strcasecmp( key, "help")) {
    // returns all possible options with their value, separated by '\n'
    _return.__set_result( (Result::type)VAL_SUCCESS );
    ostringstream oss;
    oss << "CompressionType" << std::endl << m_compression.getCompressionTypeString() << std::endl;
    oss << "CompressionLevel" << std::endl << m_compression.getCompressionLevel() << std::endl;
    oss << "CompressionThreshold" << std::endl << m_compression.getCompressionThreshold() << std::endl;
    oss << "CompressionEnabled" << std::endl << ( m_compression_enabled ? "1" : "0") << std::endl;
    // supported compression schemes
    oss << "CompressionList" << std::endl << m_compression.getCompressionTypeList() << std::endl;
    _return.__set_data( oss.str());
  } else if ( !strcasecmp( key, "CompressionType")) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( m_compression.getCompressionTypeString());
  } else if ( !strcasecmp( key, "CompressionLevel")) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( std::to_string( ( long long int)m_compression.getCompressionLevel()));
  } else if ( !strcasecmp( key, "CompressionThreshold")) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( std::to_string( ( long long int)m_compression.getCompressionThreshold()));
  } else if ( !strcasecmp( key, "CompressionList")) {
    // supported compression schemes
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( m_compression.getCompressionTypeList());
  } else if ( !strcasecmp( key, "CompressionEnabled")) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( ( m_compression_enabled ? "1" : "0"));
  } else {
    _return.__set_result( (Result::type)VAL_INVALID_QUERY_PARAMETERS);
    _return.__set_data( std::string( "Unknown configuration parameter: ") + key_str);
  }
  LOGGER << "Output:"                       << std::endl;
  LOGGER << "  result : " << _return.result << std::endl;
  LOGGER << "  data   : " << _return.data   << std::endl;

}

void QueryManagerServer::ManageSetConfiguration( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string key_str      = pt.get<std::string>( "Key");
  std::string val_str      = pt.get<std::string>( "Value");
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S  -" << sessionID        << std::endl;
  std::cout << "K  -" << key_str       << std::endl;
  std::cout << "V  -" << val_str       << std::endl;
  
  const char *key = key_str.c_str();
  const char *value = val_str.c_str();
  bool wrong_value = false;
  if ( !strcasecmp( key, "CompressionType")) {
    // _return.__set_result( (Result::type)VAL_SUCCESS );
    // _return.__set_data( m_compression.getCompressionTypeString());
    char comp_type[ 5]; // compression type should have 4 chars only
    comp_type[ 0] = comp_type[ 1] = comp_type[ 2] = comp_type[ 3] = ' '; // initialize to spaces
    comp_type[ 4] = '\0';
    for ( int i = 0; i < 4; i++) {
      comp_type[ i] = ( value[ i] ? value[ i] : ' '); // trainling spaces if needed
    }
    bool ok = m_compression.setCompressionTypeFromString( comp_type);
    if ( ok) {
      _return.__set_result( (Result::type)VAL_SUCCESS );
      LOGGER << "   Compresison type set to: " << value << std::endl;
    } else {
      wrong_value = true;
    }
  } else if ( !strcasecmp( key, "CompressionLevel")) {
    // _return.__set_result( (Result::type)VAL_SUCCESS );
    // _return.__set_data( std::to_string( m_compression.getCompressionLevel()));
    int level = 1;
    int n = sscanf( value, "%d", &level);
    if ( n == 1 && ( level >= 1) && ( level <= 9)) {
      m_compression.setCompressionLevel( level);
      _return.__set_result( (Result::type)VAL_SUCCESS );
      LOGGER << "   Compresison level set to: " << level << std::endl;
    } else {
      wrong_value = true;
    }
  } else if ( !strcasecmp( key, "CompressionThreshold")) {
    // _return.__set_result( (Result::type)VAL_SUCCESS );
    // _return.__set_data( std::to_string( m_compression.getCompressionLevel()));
    size_t mem_minimum = 100; // absolute minimum
    int n = sscanf( value, "%ld", &mem_minimum);
    if ( n == 1 && ( mem_minimum >= 1)) {
      m_compression.setCompressionThreshold( mem_minimum);
      _return.__set_result( (Result::type)VAL_SUCCESS );
      LOGGER << "   Compresison threshold set to: " << GetNiceSizeString( mem_minimum) << std::endl;
    } else {
      wrong_value = true;
    }
  } else if ( !strcasecmp( key, "CompressionEnabled") || !strcasecmp( key, "CompressionEnable")) {
    int flag = 1;
    int n = sscanf( value, "%d", &flag);
    if ( n == 1) {
      m_compression_enabled = ( flag ? true : false);
      _return.__set_result( (Result::type)VAL_SUCCESS );
      LOGGER << "   Compresison " << ( flag ? "enabled" : "disabled") << std::endl;
    } else {
      wrong_value = true;
    }
  } else {
    _return.__set_result( (Result::type)VAL_INVALID_QUERY_PARAMETERS);
    _return.__set_data( std::string( "Unknown configuration parameter: ") + key_str);
  }
  if ( wrong_value) {
    _return.__set_result( (Result::type)VAL_INVALID_QUERY_PARAMETERS);
    _return.__set_data( std::string( "Unknown configuration value: ") + val_str + std::string( " for parameter: ") + key_str);
  }
  LOGGER << "Output:"                       << std::endl;
  LOGGER << "  result : " << _return.result << std::endl;
  LOGGER << "  data   : " << _return.data   << std::endl;
}
