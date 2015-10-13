/* -*- c++ -*- */
#pragma once

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


// Generated code
#include "../../thrift/QueryManager/gen-cpp/QueryManager.h"

// ***************************************************************************
//
// Actual implementation of QueryManager functions.
//
// This is a derived class, so that our own code does not get overwritten
// when the Thrift interface needs to be regenerated.
//
// ***************************************************************************
// actual QueryManagerServer ( exposed to the accesslibrary of the visualization client)
using namespace  ::VELaSSCo;

class QueryManagerServer : virtual public QueryManagerIf {
  struct User
  {
    std::string loginName;
    std::string loginTime;
  };
  
  typedef std::map<SessionID, User> UserMap;
  UserMap m_users;
  
  bool ValidSessionID(SessionID sessionID)
  {
    const UserMap::iterator it = m_users.find(sessionID);
    return (it != m_users.end());
  }
  
  void UserLogin(UserLogin_Result& _return, const std::string& url, const std::string& name, const std::string& password);
  
  void UserLogout(UserLogout_Result& _return, const SessionID sessionID);
  
  void Query(Query_Result& _return, const SessionID sessionID, const std::string& query);
  
  void GetStatusDB(StatusDB_Result& _return, const SessionID sessionID);

  void ManageGetResultFromVerticesID( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetListOfModels( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageOpenModel( Query_Result &_return, const SessionID sessionID, const std::string& query);
};

extern int StartServer( const int server_port);
