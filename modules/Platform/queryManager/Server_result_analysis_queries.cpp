
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

// BoostGraphicsModule::getInstance
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
//#include "Analytics.h"
#include "Graphics.h"
#include "VELaSSCo_Operations.h"

#include "Server.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCo;

extern VELaSSCo_Operations *queryServer;

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
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "nV -" << numVertexIDs       << "-" << std::endl;
  std::cout << "Vs -" << lstVertexIDs       << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "SO -" << stepOptions       << "-" << std::endl;
  std::cout << "nS -" << numSteps       << "-" << std::endl;
  std::cout << "Ss -" << strSteps       << "-" << std::endl;

  // in theory should check first if it has already been calculated by doing a
  // queryServer->getBoundingBox( sessionID, query);
  // and access the Simulations_VQuery_Results_Metadata and Simulations_VQuery_Results_Data tables
  
  // not implemented yet:
  // std::string _return_; // status
  // queryServer->getListOfTimeSteps( _return_,
  // 						      dl_sessionID, modelID,
  // 						      analysisID,
  // 						      stepOptions, numSteps, lstSteps);
  // parse _return_ into a double *lstSteps
  double bbox[ 6];
  std::string error_str;
  try {
    //AnalyticsModule::getInstance()->calculateBoundingBox( GetQueryManagerSessionID( sessionID), modelID,
    queryServer->calculateBoundingBox( GetQueryManagerSessionID( sessionID), modelID,
				       // analysisID, numSteps, lstSteps,
				       "", 0, NULL,
				       // numVertexIDs, lstVertexIDs,
				       0, NULL,
				       &bbox[ 0], &error_str);
    // GraphicsModule *graphics = GraphicsModule::getInstance();
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
 
 
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "AN -" << analysisName     << "-" << std::endl;
  std::cout << "SM -" << staticMeshID     << "-" << std::endl;
  std::cout << "TS -" << stepOptions   << "-" << std::endl;
  std::cout << "nTS  -" << numSteps      << "-" << std::endl;
  //std::cout << "TSt  -" << as_string<size_t>( pt, "lstSteps")      << "-" << std::endl;
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
    // queryServer->getListOfTimeSteps( _return_,
    // 						      dl_sessionID, modelID,
    // 						      analysisID,
    // 						      stepOptions, numSteps, lstSteps);
    // parse _return_ into a double *lstSteps
    
    //AnalyticsModule::getInstance()->calculateDiscrete2Continuum ( GetQueryManagerSessionID( sessionID), modelID,
    queryServer->calculateDiscrete2Continuum ( GetQueryManagerSessionID( sessionID), modelID,
								 analysisName, staticMeshID, stepOptions, numSteps, lstSteps.data(),
								 coarseGrainingMethod, width, cutoffFactor, processContacts, 
								 doTemporalAVG, temporalAVGOptions, deltaT, &query_outcome, &error_str);
							  
    // GraphicsModule *graphics = GraphicsModule::getInstance();
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



void QueryManagerServer::ManageGetBoundaryOfAMesh( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string name       = pt.get<std::string>( "name");
  std::string modelID    = pt.get<std::string>( "modelID");
  std::string meshName     = pt.get<std::string>( "meshID"); // in fact it's the mesh name
  std::string analysisID = pt.get<std::string>( "analysisID");
  double stepValue       = pt.get< double>( "stepValue");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "Msh-" << meshName           << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;

  // in theory should check first if it has already been calculated by doing a
  // queryServer->getBoundingBox( sessionID, query);
  // and access the Simulations_VQuery_Results_Metadata and Simulations_VQuery_Results_Data tables

  std::string error_str;

  // from the mesh name, get the Mesh number
  // eventually the Mesh information could have been cached ...

  std::cout << "looking for the Mesh " << meshName << " in order to get it's id" << std::endl;
  rvGetListOfMeshes _return_;
  queryServer->getListOfMeshes( _return_, dl_sessionID, modelID, analysisID, stepValue);
  int meshID = -1;
  std::string elementType = "";
  if ( _return_.meshInfos.size() == 0) {
    _return.__set_result( (Result::type)VAL_NO_MESH_INFORMATION_FOUND);
    error_str = "There is no mesh metadata.";
  } else {
    for ( std::vector< MeshInfo>::iterator it = _return_.meshInfos.begin();
          it != _return_.meshInfos.end(); it++) {
      if ( AreEqualNoCase( it->name, meshName)) {
	meshID = it->meshNumber;
	elementType = getStrFromElementType( it->elementType.shape);
	break;
      }
    }
    if ( meshID == -1) { // not found
      error_str = "Mesh name " + meshName + " not in metadata.";
      std::cout << error_str << std::endl;
    }
  }
  
  std::string binary_mesh = "";
  if ( error_str.length() == 0) {
    std::cout << "Mesh name " << meshName << " has mesh number = " << meshID << " and elementType = " << elementType << std::endl;
    try {
      //AnalyticsModule::getInstance()->calculateBoundaryOfAMesh( GetQueryManagerSessionID( sessionID), 
      queryServer->calculateBoundaryOfAMesh( GetQueryManagerSessionID( sessionID), 
					     modelID,
					     meshID, elementType,
					     analysisID, stepValue, 
					     &binary_mesh, &error_str);
      // GraphicsModule *graphics = GraphicsModule::getInstance();
      // just to link to the GraphicsModule;
    } catch ( TException &e) {
      std::cout << "CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "CATCH_ERROR 2: " << e.what() << std::endl;
    }
  }

  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( binary_mesh);
  } else {
    _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    _return.__set_data( error_str);
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  boundary_mesh = ( " << _return.data.length() << " bytes)" << std::endl;
  if ( error_str.length() == 0) {
    LOGGER << "  data   : \n" << Hexdump( _return.data, 128) << std::endl;
  } else {
    LOGGER << "  error  : \n" << _return.data << std::endl;
  }
}

void QueryManagerServer::ManageDeleteBoundaryOfAMesh( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string name       = pt.get<std::string>( "name");
  std::string modelID    = pt.get<std::string>( "modelID");
  std::string meshName     = pt.get<std::string>( "meshID"); // in fact it's the mesh name
  std::string analysisID = pt.get<std::string>( "analysisID");
  double stepValue       = pt.get< double>( "stepValue");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "Msh-" << meshName           << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;

  std::string error_str;

  _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR); // default value

  // from the mesh name, get the Mesh number
  // eventually the Mesh information could have been cached ...
  std::cout << "looking for the Mesh " << meshName << " in order to get it's id" << std::endl;
  rvGetListOfMeshes _return_;
  queryServer->getListOfMeshes( _return_, dl_sessionID, modelID, analysisID, stepValue);
  int meshID = -1;
  std::string elementType = "";
  if ( _return_.meshInfos.size() == 0) {
    _return.__set_result( (Result::type)VAL_NO_MESH_INFORMATION_FOUND);
    error_str = "There is no mesh metadata.";
  } else {
    for ( std::vector< MeshInfo>::iterator it = _return_.meshInfos.begin();
          it != _return_.meshInfos.end(); it++) {
      if ( AreEqualNoCase( it->name, meshName)) {
	meshID = it->meshNumber;
	elementType = getStrFromElementType( it->elementType.shape);
	break;
      }
    }
    if ( meshID == -1) { // not found
      error_str = "Mesh name " + meshName + " not in metadata.";
      std::cout << error_str << std::endl;
    }
  }
  
  bool storedBoundaryFound = false;
  if ( error_str.length() == 0) {
    std::cout << "Mesh name " << meshName << " has mesh number = " << meshID << " and elementType = " << elementType << std::endl;
    // std::string binary_mesh = "";
    try {
      //AnalyticsModule::getInstance()->calculateBoundaryOfAMesh( GetQueryManagerSessionID( sessionID),
      error_str = "";
      queryServer->getStoredBoundaryOfAMesh( GetQueryManagerSessionID( sessionID), 
					     modelID,
					     meshID, elementType,
					     analysisID, stepValue, 
					     // &binary_mesh,
					     NULL, // we don't want the mesh, only check if it's there
					     &error_str);
      if ( error_str.length() == 0) {
	storedBoundaryFound = true;
      }
      if ( storedBoundaryFound) {
	queryServer->deleteStoredBoundaryOfAMesh( GetQueryManagerSessionID( sessionID), 
						  modelID,
						  meshID, elementType,
						  analysisID, stepValue, 
						  &error_str);
      }
      // GraphicsModule *graphics = GraphicsModule::getInstance();
      // just to link to the GraphicsModule;
    } catch ( TException &e) {
      std::cout << "CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "CATCH_ERROR 2: " << e.what() << std::endl;
    }
  }

  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    // _return.__set_data( binary_mesh);
  } else {
    if ( !storedBoundaryFound) {
      _return.__set_result( (Result::type)VAL_BOUNDARY_MESH_NOT_FOUND);
    }
    _return.__set_data( error_str);
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  if ( error_str.length() != 0) {
    LOGGER << "  error  : \n" << _return.data << std::endl;
  }
}

