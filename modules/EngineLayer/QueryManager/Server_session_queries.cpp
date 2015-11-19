
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

// ***************************************************************************
//
// Actual implementation of QueryManager functions.
//
// This is a derived class, so that our own code does not get overwritten
// when the Thrift interface needs to be regenerated.
//
// ***************************************************************************

/* 0xx Session queries */

void QueryManagerServer::UserLogin(UserLogin_Result& _return, const std::string& url, const std::string& name, const std::string& password)
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

void QueryManagerServer::UserLogout(UserLogout_Result& _return, const SessionID sessionID)
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

void QueryManagerServer::ManageGetListOfModels( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  
  std::string name               = pt.get<std::string>("name");
  std::string group_qualifier    = pt.get<std::string>("groupQualifier");
  std::string name_pattern       = pt.get<std::string>("namePattern");
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::cout << "S " << sessionID       << std::endl;
  std::cout << "G " << group_qualifier << std::endl;
  std::cout << "P " << name_pattern    << std::endl;
  
  rvGetListOfModels _return_;
  DataLayerAccess::Instance()->getListOfModels( _return_,
						   sessionIDStr.str(), group_qualifier, name_pattern);
  		  
  std::cout << _return_ << std::endl;

  if ( _return_.status == "Error") {
    if ( _return_.report == "No models") {
      _return.__set_result( (Result::type)VAL_NO_MODELS_IN_PLATFORM);
    } else {
      _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    }
    _return.__set_data( _return_.report);
  } else { // status == "Ok"
    if ( _return_.models.size() == 0) {
      _return.__set_result( (Result::type)VAL_NO_MODEL_MATCHES_PATTERN);
    } else {
      _return.__set_result( (Result::type)VAL_SUCCESS );
      // process data:
      // foreach model in _return_.models
      //   return { model.name, model.fullpath}
      // _return.__set_data( _return_.models.srt());
      std::ostringstream oss;
      oss << "NumberOfModels: " << _return_.models.size() << std::endl;
      // C++11 : for ( auto &it : _return_.models)
      char hex_string[ 1024];
      for ( std::vector< FullyQualifiedModelName>::iterator it = _return_.models.begin();
          it != _return_.models.end(); it++) {
	oss << "Name: " << it->name << std::endl;
	oss << "FullPath: " << it->full_path << std::endl;
	// oss << "ModelID: " << ToHexString( hex_string, 1024, it->modelID.c_str(), it->modelID.size()) << std::endl;
	oss << "ModelID: " << ModelID_DoHexStringConversionIfNecesary( it->modelID, hex_string, 1024) << std::endl;
	oss << "Location: " << it->location << std::endl; // EDM or HBase:table_name

      }
      _return.__set_data( oss.str());
    }
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}

void QueryManagerServer::ManageOpenModel( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  
  std::string unique_name        = pt.get<std::string>("uniqueName");
  std::string requested_access   = pt.get<std::string>("requestedAccess");
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::cout << "S " << sessionID        << std::endl;
  std::cout << "P " << unique_name      << std::endl;
  std::cout << "A " << requested_access << std::endl;
  
  rvOpenModel _return_;
  DataLayerAccess::Instance()->openModel( _return_,
					     sessionIDStr.str(), unique_name, requested_access);
  		  
  std::cout << _return_ << std::endl;

  if ( _return_.status == "Error") {
    if ( _return_.report == "No models") {
      _return.__set_result( (Result::type)VAL_NO_MODELS_IN_PLATFORM);
    } else if ( _return_.report == "Not found") {
      _return.__set_result( (Result::type)VAL_NO_MODEL_MATCHES_PATTERN);
    } else {
      _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    }
    _return.__set_data( _return_.report);
  } else {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( _return_.modelID);

    // store OpenModel info
    OpenModelKey key = { sessionID, _return_.modelID};
    m_models[ key] = QMS_FullyQualifiedModelName( _return_.model_info);
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  char hex_string[ 1024];
  LOGGER << "  data   : \n" << ModelID_DoHexStringConversionIfNecesary( _return.data, hex_string, 1024) << std::endl;
}

void QueryManagerServer::ManageCloseModel( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  
  std::string modelID    = pt.get<std::string>("modelID");
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  // std::cout << "S " << sessionID        << std::endl;
  // std::cout << "M " << modelID    << std::endl;
  
  // Nothing to do in the Data Layer ...
  // rvOpenModel _return_;
  // DataLayerAccess::Instance()->openModel( _return_,
  // 					     sessionIDStr.str(), unique_name, requested_access);
  // std::cout << _return_ << std::endl;

  _return.__set_result( (Result::type)VAL_SUCCESS );

  // delete OpenModel info
  OpenModelKey key = { sessionID, modelID};
  m_models.erase( key);
}