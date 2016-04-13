/* -*- c++ -*- */
#pragma once

#include <assert.h>

// STL
#include <sstream>

// Thrift
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>

// Boost
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>


// Generated code
#include "../thrift/QueryManager/gen-cpp/QueryManager.h"
#include "Compression.h"

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
  std::string GetMetaDataTableName() const;
  std::string GetDataTableName() const;
  void GetTableNames( std::string &model_table, std::string &metadata_table, std::string &data_table) const;
};

inline std::string QMS_FullyQualifiedModelName::GetModelsTableName() const {
  std::string model, metadata, data;
  GetTableNames( model, metadata, data);
  return model;
}
inline std::string QMS_FullyQualifiedModelName::GetMetaDataTableName() const {
  std::string model, metadata, data;
  GetTableNames( model, metadata, data);
  return metadata;
}
inline std::string QMS_FullyQualifiedModelName::GetDataTableName() const {
  std::string model, metadata, data;
  GetTableNames( model, metadata, data);
  return data;
}

// to have information about sessions
struct User
{
  std::string loginName;
  std::string loginTime;
  // we have a int64 SessionId between AccessLib and QueryManager
  // but a std::string between QueryManager and DataLayer
  std::string dataLayerSessionID;
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
  static ::apache::thrift::server::TSimpleServer *m_simpleServer;
  static ::apache::thrift::server::TThreadedServer *m_threadedServer;
  UserMap m_users;
  ModelMap m_models;
  // handles compression between QM and AL
  // at the moment the QM only compresses data to AL
  VL_Compression m_compression;
  bool m_compression_enabled;

  bool ValidSessionID( const SessionID &sessionID) const;
  // here we have a int64 SessionId between AccessLib and QueryManager
  // but a std::string between QueryManager and DataLayer
  std::string GetQueryManagerSessionID( const SessionID &sessionID) const;
  std::string GetDataLayerSessionID( const SessionID &sessionID) const;
  // support simulation data in several tables: 
  //    VELaSSCo_Models & co.
  //    VELaSSCo_Models_V4CIMNE & co.
  //    Test_VELaSSCo_Models & co.
  //    T_VELaSSCo_Models & co.
  bool GetModelInfo( const SessionID &sessionID, const ModelID &model, QMS_FullyQualifiedModelName &ret) const;
  bool GetTableNames( const SessionID &sessionID, const ModelID &modelID,
		      std::string &model_table, std::string &metadata_table, std::string &data_table) const;
  std::string GetModelsTableName( const SessionID &sessionID, const ModelID &modelID) const;
  std::string GetMetaDataTableName( const SessionID &sessionID, const ModelID &modelID) const;
  std::string GetDataTableName( const SessionID &sessionID, const ModelID &modelID) const;
  
  // queries:
  /* Session Queries */
  void UserLogin(UserLogin_Result& _return, const std::string& url, const std::string& name, const std::string& password);
  void UserLogout(UserLogout_Result& _return, const SessionID sessionID);

  /* all other Session, Direct Result and Result Analysis queries ges through Query() and then Manage... */
  void Query(Query_Result& _return, const SessionID sessionID, const std::string& query);
  
  /* Monitoring Queries */  
  void GetStatusDB(StatusDB_Result& _return, const SessionID sessionID);
  void StopVELaSSCo(StopVELaSSCo_Result& _return, const SessionID sessionID);
  void ManageGetConfiguration( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageSetConfiguration( Query_Result &_return, const SessionID sessionID, const std::string& query);

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
  void ManageGetMeshDrawData( Query_Result& _return, const SessionID sessionID, const std::string& query );
  /* Result Analysis Queries */
  void ManageGetBoundingBox( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetDiscrete2Continuum( Query_Result &_return, const SessionID sessionID, const std::string& query);
  void ManageGetBoundaryOfAMesh( Query_Result &_return, const SessionID sessionID, const std::string& query);

  // to get the string representation of the thrift type ElementShapeType::type
  const char *getStrFromElementType( const ElementShapeType::type &elem);
public:
  static void SetSimpleServer( ::apache::thrift::server::TSimpleServer *simpleServer) {
    if ( m_simpleServer)
      delete m_simpleServer;
    m_simpleServer = simpleServer;
  }
  static void SetThreadedServer( ::apache::thrift::server::TThreadedServer *threadedServer) {
    if ( m_threadedServer)
      delete m_threadedServer;
    m_threadedServer = threadedServer;
  }
  void SetDefaultCompression() {
    m_compression.setCompressionType( VL_Compression::CompressionType::Zlib);
    m_compression.setCompressionLevel( 1);
    m_compression.setCompressionThreshold( VL_COMPRESSION_MINIMUM_DATA_SIZE_TO_COMPRESS);
    m_compression_enabled = false; // compression disabled until enabled explicitly by the AccessLib
  }
}; // class QueryManagerServer


inline bool QueryManagerServer::ValidSessionID( const SessionID &sessionID) const {
  const UserMap::const_iterator it = m_users.find(sessionID);
  return (it != m_users.end());
}
inline std::string QueryManagerServer::GetQueryManagerSessionID( const SessionID &sessionID) const {
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  return sessionIDStr.str();
}
inline std::string QueryManagerServer::GetDataLayerSessionID( const SessionID &sessionID) const {
  std::string ret( "");
  if ( ValidSessionID( sessionID)) {
    const std::string &dl_session = m_users.at( sessionID).dataLayerSessionID;
    ret = ( dl_session.size() != 0) ? dl_session : GetQueryManagerSessionID( sessionID);
  }
  return ret;
}
inline bool QueryManagerServer::GetModelInfo( const SessionID &sessionID, const ModelID &model, 
					      QMS_FullyQualifiedModelName &ret) const {
  OpenModelKey key = { sessionID, model};
  const ModelMap::const_iterator it = m_models.find( key);
  if ( it != m_models.end()) {
    ret = it->second;
    return true;
  } else {
    return false;
  }
}

inline bool QueryManagerServer::GetTableNames( const SessionID &sessionID, const ModelID &modelID,
					       std::string &model_table, std::string &metadata_table, std::string &data_table) const {
  QMS_FullyQualifiedModelName mi;
  bool found = this->GetModelInfo( sessionID, modelID, mi);
  if ( found) {
    mi.GetTableNames( model_table, metadata_table, data_table);
  }
  return found;
}
inline std::string QueryManagerServer::GetModelsTableName( const SessionID &sessionID, const ModelID &modelID) const {
  std::string model, metadata, data;
  bool found = this->GetTableNames( sessionID, modelID, model, metadata, data);
  // assert( found);
  if ( !found) model = "";
  return model;
}
inline std::string QueryManagerServer::GetMetaDataTableName( const SessionID &sessionID, const ModelID &modelID) const {
  std::string model, metadata, data;
  bool found = this->GetTableNames( sessionID, modelID, model, metadata, data);
  // assert( found);
  if ( !found) metadata = "";
  return metadata;
}
inline std::string QueryManagerServer::GetDataTableName( const SessionID &sessionID, const ModelID &modelID) const {
  std::string model, metadata, data;
  bool found = this->GetTableNames( sessionID, modelID, model, metadata, data);
  // assert( found);
  if ( !found) data = "";
  return data;
}

template <typename T>
std::vector<T> as_vector(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
{
    std::vector<T> result;
    
    BOOST_FOREACH(boost::property_tree::ptree::value_type it, pt.get_child(key)) {
        result.push_back(it.second.get_value<T>());
    }
    
    return result;
}

template <typename T>
std::string as_string(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
{
    std::stringstream ss;

    BOOST_FOREACH(boost::property_tree::ptree::value_type it, pt.get_child(key)) {
		ss << (ss.str().size()?",":"") << it.second.get_value<T>();
    }
    
    return std::string("[") + ss.str() + "]";
}

extern int StartServer( const int server_port);
