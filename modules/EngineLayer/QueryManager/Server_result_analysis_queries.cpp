
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
  double bbox[ 6];
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
