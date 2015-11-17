
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

// ***************************************************************************
//
// Actual implementation of QueryManager functions.
//
// This is a derived class, so that our own code does not get overwritten
// when the Thrift interface needs to be regenerated.
//
// ***************************************************************************

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
  } else if ( name == "GetListOfModels") {
    ManageGetListOfModels( _return, sessionID, query);
  } else if ( name == "OpenModel") {
    ManageOpenModel( _return, sessionID, query);
  } else if ( name == "CloseModel") {
    ManageCloseModel( _return, sessionID, query);
    /* Direct Result Queries */
  } else if ( name == "GetListOfMeshes") {
    ManageGetListOfMeshes( _return, sessionID, query);
    /* Result Analysis Queries */
  } else if ( name == "GetBoundingBox") {
    ManageGetBoundingBox( _return, sessionID, query);
  } else {
    _return.__set_result( (Result::type)VAL_INVALID_QUERY );
    
    LOGGER                                    << std::endl;
    LOGGER << "Output:"                       << std::endl;
    LOGGER << "  result : " << _return.result << std::endl;
    
    return;
  }
}

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

/* first queries */

void QueryManagerServer::ManageGetResultFromVerticesID( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  std::string name       = pt.get<std::string>("name");
  std::string modelID    = pt.get<std::string>("modelID");
  std::string resultID   = pt.get<std::string>("resultID");
  std::string analysisID = pt.get<std::string>("analysisID");
  std::string vertexIDs  = as_string< size_t>( pt, "vertexIDs");
  double      timeStep   = pt.get<double>("timeStep");
  
  stringstream listOfVertices;
  listOfVertices << "{\"id\":" << "[]" /*vertexIDs*/ << "}";
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::string _return_;
  
  //std::cout << "S " << sessionID  << std::endl;
  //std::cout << "M " << modelID    << std::endl;
  //std::cout << "R " << resultID   << std::endl;
  //std::cout << "A " << analysisID << std::endl;
  //std::cout << "V " << vertexIDs  << std::endl;
  //std::cout << "T " << timeStep   << std::endl;
  
  DataLayerAccess::Instance()->getResultFromVerticesID(_return_ ,sessionIDStr.str() ,modelID ,analysisID ,timeStep ,resultID ,listOfVertices.str());
  
  std::vector<int64_t> resultVertexIDs;
  std::vector<double>  resultValues;
		  
  std::cout << _return_ << std::endl;
		  
  std::istringstream iss(_return_);
  while (iss)
    {
      int64_t id;
      double  value[3];
		      
      std::string line;
      std::getline(iss, line);
      std::istringstream(line) >> id >> value[0] >> value[1] >> value[2];
		      
      resultVertexIDs.push_back(id);
      resultValues.push_back(value[0]);
      resultValues.push_back(value[1]);
      resultValues.push_back(value[2]);
    }
		  
  // Pack into string
  std::string result;
  std::ostringstream oss;
  oss << (resultValues.size() / 3) << " 3" << "\n";
  result += oss.str();
  result += std::string((char*)(&resultVertexIDs[0]), sizeof(int64_t)*resultVertexIDs.size());
  result += std::string((char*)(&resultValues[0]),    sizeof(double)*resultValues.size());
  _return.__set_data(result); 
		  
  // Generate example result data
  if (0)
    {
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
    }
		  
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
}

/* 0xx Session queries */

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

/* 1xx - Direct Result Queries */

void QueryManagerServer::ManageGetListOfMeshes( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  
  std::string name       = pt.get<std::string>("name");
  std::string modelID    = pt.get<std::string>( "modelID");
  std::string analysisID = pt.get<std::string>( "analysisID");
  double stepValue       = pt.get< double>( "stepValue");
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::cout << "S  -" << sessionID        << "-" << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;

  rvGetListOfMeshes _return_;
  DataLayerAccess::Instance()->getListOfMeshes( _return_,
						sessionIDStr.str(), modelID, analysisID, stepValue);
  
  std::cout << _return_ << std::endl;

  if ( _return_.status == "Error") {
    if ( _return_.report == "No models") {
      _return.__set_result( (Result::type)VAL_NO_MODELS_IN_PLATFORM);
    } else {
      _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    }
    _return.__set_data( _return_.report);
  } else { // status == "Ok"
    if ( _return_.meshInfos.size() == 0) {
      _return.__set_result( (Result::type)VAL_NO_MODEL_MATCHES_PATTERN);
    } else {
      _return.__set_result( (Result::type)VAL_SUCCESS );
      // process data:
      // foreach model in _return_.meshInfos
      //   return { model.name, model.fullpath}
      // _return.__set_data( _return_.meshInfos.srt());
      std::ostringstream oss;
      oss << "NumberOfModels: " << _return_.meshInfos.size() << std::endl;
      // C++11 : for ( auto &it : _return_.meshInfos)
      char hex_string[ 1024];
      for ( std::vector< MeshInfo>::iterator it = _return_.meshInfos.begin();
          it != _return_.meshInfos.end(); it++) {
	oss << "Name: " << it->name << std::endl;
	oss << "ElementType" << it->elementType.shape << std::endl;
	oss << "NumberOfVerticesPerElement: " <<  it->elementType.num_nodes << std::endl;
	oss << "NumberOfVertices: " << it->nVertices << std::endl;
	oss << "NumberOfElements: " << it->nElements << std::endl;
	oss << "Units: " << it->meshUnits << std::endl;
	oss << "Color: " << it->meshColor << std::endl;

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

/* 2xx - Result Analysis Queries */

void QueryManagerServer::ManageGetBoundingBox( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // double bbox[ 6] = { -0.5, -0.5, -0.5, 0.5, 0.5, 0.5};
  // _return.__set_data( std::string( ( const char *)&bbox[ 0], 6 * sizeof( double)));
  // _return.__set_result( (Result::type)VAL_SUCCESS );

  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string modelID            = pt.get<std::string>( "modelID");
  int64_t numVertexIDs           = pt.get< int64_t>( "numVertexIDs");
  // can be very large, eventually it can be stored in base64-encoding compressed byte-buffer
  std::string lstVertexIDs       = as_string< size_t>( pt, "lstVertexIDs");
  std::string analysisID         = pt.get<std::string>( "analysisID");
  std::string stepOptions        = pt.get<std::string>( "stepOptions");
  int numSteps                   = pt.get< int>( "numSteps");
  // can be very large, eventually it can be stored in base64-encoding compressed byte-buffer
  std::string strSteps           = as_string< size_t>( pt, "lstSteps");
  std::vector< double> lstSteps  = as_vector< double>( pt, "lstSteps");
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::cout << "S  -" << sessionID        << "-" << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "nV -" << numVertexIDs       << "-" << std::endl;
  std::cout << "Vs -" << lstVertexIDs       << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "SO -" << stepOptions       << "-" << std::endl;
  std::cout << "nS -" << numSteps       << "-" << std::endl;
  std::cout << "Ss -" << strSteps       << "-" << std::endl;

  // in theory should check first if it has already been calculated by doing a
  // DataLayerAccess::Instance()->getBoundingBox( sessionID, query);
  // and access the Simulations_VQuery_Results_Metadata and Simulations_VQuery_Results_Data tables
  
  // not implemented yet:
  // std::string _return_; // status
  // DataLayerAccess::Instance()->getListOfTimeSteps( _return_,
  // 						      sessionIDStr.str(), modelID,
  // 						      analysisID,
  // 						      stepOptions, numSteps, lstSteps);
  // parse _return_ into a double *lstSteps
  std::string simulation_data_table_name = GetDataTableName( sessionID, modelID);
 
 double bbox[6];
  std::string error_str;
  try {
    AnalyticsModule::getInstance()->calculateBoundingBox( sessionIDStr.str(), modelID,
							  simulation_data_table_name,
							  // analysisID, numSteps, lstSteps,
							  "", 0, NULL,
							  // numVertexIDs, lstVertexIDs,
							  0, NULL,
							  &bbox[ 0], &error_str);
    GraphicsModule *graphics = GraphicsModule::getInstance();
    // just to link to the GraphicsModule;
  } catch ( TException &e) {
    std::cout << "CATCH_ERROR 1: " << e.what() << std::endl;
  } catch ( exception &e) {
    std::cout << "CATCH_ERROR 2: " << e.what() << std::endl;
  }
  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( std::string( ( const char *)&bbox[ 0], 6 * sizeof( double)));
  } else {
    _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    _return.__set_data( error_str);
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << Hexdump( _return.data, 128) << std::endl;
}


void QueryManagerServer::ManageGetDiscrete2Continuum( Query_Result &_return, const SessionID sessionID, const std::string& query) {
 
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  
  // get parameters: // as in AnalyticsRAQ.cpp
  std::string modelID            = pt.get<std::string>( "modelID");
  std::string analysisName         = pt.get<std::string>( "analysisName");
  std::string stepOptions        = pt.get<std::string>( "stepOptions");
  int numSteps                   = pt.get<int>( "numSteps");
  
  std::vector<double> lstSteps = as_vector<double>(pt, "lstSteps");
  assert(lstSteps.size() == (size_t)numSteps);

  std::string staticMeshID       = pt.get<std::string>("staticMeshID");
  std::string coarseGrainingMethod = pt.get<std::string>("coarseGrainingMethod");
  double width     	         = pt.get< double>("width");
  double cutoffFactor     	 = pt.get< double>("cutoffFactor");
  bool processContacts           = pt.get< bool>("processContacts");
  bool doTemporalAVG             = pt.get< bool>("doTemporalAVG");
  std::string temporalAVGOptions = pt.get<std::string>("temporalAVGOptions");
  double deltaT    =	pt.get< double>("deltaT");
 
 
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::cout << "S  -" << sessionID        << "-" << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "AN -" << analysisName     << "-" << std::endl;
  std::cout << "SM -" << staticMeshID     << "-" << std::endl;
  std::cout << "TS -" << stepOptions   << "-" << std::endl;
  std::cout << "nTS  -" << numSteps      << "-" << std::endl;
  std::cout << "TSt  -" << as_string<size_t>( pt, "lstSTeps")      << "-" << std::endl;
  std::cout << "CG  -" << coarseGrainingMethod        << "-" << std::endl;
  std::cout << "Wd -" << width << "-" << std::endl;
  std::cout << "Co -" << cutoffFactor << "-" << std::endl;
  std::cout << "Pc -" << processContacts  << "-" << std::endl;
  std::cout << "DTA -" << doTemporalAVG   << "-" << std::endl;
  std::cout << "TAO -" << temporalAVGOptions  << "-" << std::endl;
  std::cout << "DT -" <<  deltaT << "-" << std::endl;
  
  std::string query_outcome;
  std::string error_str;
  
  try {
    // not implemented yet:
    // std::string _return_; // status
    // DataLayerAccess::Instance()->getListOfTimeSteps( _return_,
    // 						      sessionIDStr.str(), modelID,
    // 						      analysisID,
    // 						      stepOptions, numSteps, lstSteps);
    // parse _return_ into a double *lstSteps
    
	AnalyticsModule::getInstance()->calculateDiscrete2Continuum (sessionIDStr.str(), modelID,
								 analysisName, staticMeshID, stepOptions, numSteps, lstSteps.data(),
								 coarseGrainingMethod, width, cutoffFactor, processContacts, 
								 doTemporalAVG, temporalAVGOptions, deltaT, &query_outcome, &error_str);
							  
    GraphicsModule *graphics = GraphicsModule::getInstance();
  } 
	catch ( TException &e) {
    std::cout << "CATCH_ERROR 1: " << e.what() << std::endl;
  } catch ( exception &e) {
    std::cout << "CATCH_ERROR 2: " << e.what() << std::endl;
  }
  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data(query_outcome);
  } else {
    _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    _return.__set_data(error_str);
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  //LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
}

int StartServer( const int server_port) {
  LOGGER << "Starting VELaSSCo Server..." << std::endl;

  int port = server_port;
  LOGGER << "  using port: " << port << std::endl;

  boost::shared_ptr<QueryManagerServer> handler(new QueryManagerServer());
  boost::shared_ptr<TProcessor> processor(new QueryManagerProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  DEBUG( "  before serving ...");
  server.serve();
  DEBUG( "  after serving ...");
  return 0;
}
