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

class QMS_FullyQualifiedModelName: public FullyQualifiedModelName {
public:
  QMS_FullyQualifiedModelName( ) {};
  QMS_FullyQualifiedModelName( const QMS_FullyQualifiedModelName &inf): FullyQualifiedModelName( ( FullyQualifiedModelName)inf) {};
  QMS_FullyQualifiedModelName( const FullyQualifiedModelName &inf): FullyQualifiedModelName( inf) {};
  std::string GetDBType () const;
  std::string GetModelsTableName() const;
  std::string GetDataTableName() const;
  std::string GetMetaDataTableName() const;
  void GetTableNames( std::string &model_Table, std::string &metadata_table, std::string &data_Table) const;
};

// to have information about sessions
struct User
{
  std::string loginName;
  std::string loginTime;
};
typedef std::map< SessionID, User> UserMap;

// to have information about the OpenedModels ...
typedef std::string ModelID;
typedef struct OpenModelKey {
  SessionID sessionID;
  ModelID modelID;
  friend bool operator<( const OpenModelKey &k1, const OpenModelKey &k2) {
    if ( k1.sessionID != k2.sessionID) {
      return k1.sessionID < k2.sessionID ;
    } else {
      return k1.modelID < k2.modelID ;
    }
  }
} OpenModelKey;
typedef std::map< OpenModelKey, QMS_FullyQualifiedModelName> ModelMap; // key = SessionID + ModelID

class QueryManagerServer : virtual public QueryManagerIf {
  UserMap m_users;
  ModelMap m_models;

  bool ValidSessionID( const SessionID &sessionID) const {
    const UserMap::const_iterator it = m_users.find(sessionID);
    return (it != m_users.end());
  }
  bool GetModelInfo( const SessionID &sessionID, const ModelID &model, QMS_FullyQualifiedModelName &ret) const {
    OpenModelKey key = { sessionID, model};
    const ModelMap::const_iterator it = m_models.find( key);
    if ( it != m_models.end()) {
      ret = it->second;
      return true;
    } else {
      return false;
    }
  }
  
  // queries:
  void UserLogin(UserLogin_Result& _return, const std::string& url, const std::string& name, const std::string& password);
  
  void UserLogout(UserLogout_Result& _return, const SessionID sessionID);
  
  void Query(Query_Result& _return, const SessionID sessionID, const std::string& query);
  
  void GetStatusDB(StatusDB_Result& _return, const SessionID sessionID);

  void ManageGetResultFromVerticesID( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetListOfModels( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageOpenModel( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageCloseModel( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetBoundingBox( Query_Result &_return, const SessionID sessionID, const std::string& query);
};

extern int StartServer( const int server_port);
