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

using namespace  ::VELaSSCo;

class QM_DemoServer : virtual public QueryManagerIf {
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
  
  /* Session Queries */
  void UserLogin(UserLogin_Result& _return, const std::string& url, const std::string& name, const std::string& password);
  void UserLogout(UserLogout_Result& _return, const SessionID sessionID);
  
  /* all other Session, Direct Result and Result Analysis queries ges through Query() and then Manage... */
  void Query(Query_Result& _return, const SessionID sessionID, const std::string& query);

  /* Monitoring Queries */  
  void GetStatusDB(StatusDB_Result& _return, const SessionID sessionID);
  void StopVELaSSCo( StopVELaSSCo_Result &_return, const SessionID sessionID);
  /* Session Queries */
  void ManageGetListOfModels( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageOpenModel( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageCloseModel( Query_Result &_return, const SessionID sessionID, const std::string& query);
  /* Direct Result Queries */
  void ManageGetResultFromVerticesID( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetListOfMeshes( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetListOfAnalyses( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetListOfTimeSteps( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetListOfResults( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetMeshDrawData( Query_Result & _return, const SessionID sessionID, const std::string& query);
  /* Result Analysis Queries */
  void ManageGetBoundingBox( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetBoundaryOfAMesh( Query_Result &_return, const SessionID sessionID, const std::string& query);
};

extern int StartServer( const int server_port);
