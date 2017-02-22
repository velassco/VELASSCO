
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
#include "../analytics/streamlineUtils/VELaSSCo_Data.h"

// Streamline tracing utilities
#include "../analytics/streamlineUtils/StreamTracer.h"
#include "../analytics/streamlineUtils/Streamline.h"
#include "../analytics/streamlineUtils/UnstructDataset.h"

// B-Spline Approximation utilities
#include "../analytics/bsplineapprox.h"

// Generated code
#include "../../thrift/QueryManager/gen-cpp/QueryManager.h"

#include "DataLayerAccess.h"
//#include "Analytics.h"
#include "Graphics.h"
#include "VELaSSCo_Operations.h"

#include "Server.h"

#include <omp.h>

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
    std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
  } catch ( exception &e) {
    std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
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

void QueryManagerServer::ManageDeleteBoundingBox( Query_Result &_return, const SessionID sessionID, const std::string& query) {
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

  std::string error_str;
  _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR); // default value

  try {
    //AnalyticsModule::getInstance()->calculateBoundaryOfAMesh( GetQueryManagerSessionID( sessionID),
    error_str = "";
    queryServer->deleteStoredBoundingBox( GetQueryManagerSessionID( sessionID), modelID,
					  // analysisID, numSteps, lstSteps,
					  "", 0, NULL,
					  // numVertexIDs, lstVertexIDs,
					  0, NULL,
					  &error_str);
    
    // GraphicsModule *graphics = GraphicsModule::getInstance();
    // just to link to the GraphicsModule;
  } catch ( TException &e) {
    std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
  } catch ( exception &e) {
    std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
  }

  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    // _return.__set_data( binary_mesh);
  } else {
    _return.__set_data( error_str);
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  if ( error_str.length() != 0) {
    LOGGER << "  error  : \n" << _return.data << std::endl;
  }
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
  bool doSpatialIntegral         = pt.get< bool>("doSpatialIntegral");
  std::string integralDimension  = pt.get<std::string>("integralDimension");
  std::string integralDirection  = pt.get<std::string>("integralDirection");
 
 
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
  std::cout << "DSI" << doSpatialIntegral << "-" << std::endl;
  std::cout << "IDm" << integralDimension << "-" << std::endl;
  std::cout << "IDi" << integralDirection << "-" << std::endl;
  
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
					       doTemporalAVG, temporalAVGOptions, deltaT, doSpatialIntegral,
					       integralDimension, integralDirection, &query_outcome, &error_str);
							  
    // GraphicsModule *graphics = GraphicsModule::getInstance();
  } catch ( TException &e) {
    std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
  } catch ( exception &e) {
    std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
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

  // get MeshID and elementType from MeshName:
  std::string error_str;
  std::string elementType = "";
  int meshID = -1;
  MeshInfo meshInfo;
  VAL_Result resultStatus = GetMeshInfoFromMeshName( dl_sessionID, modelID, analysisID, stepValue,
						     meshName, meshInfo, error_str);
  if ( resultStatus != VAL_SUCCESS) {
    // error
    _return.__set_result( ( Result::type)resultStatus);
  } else {
    meshID = meshInfo.meshNumber;
    elementType = getStrFromElementType( meshInfo.elementType.shape);
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
      std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
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

  // get MeshID and elementType from MeshName:
  std::string elementType = "";
  int meshID = -1;
  MeshInfo meshInfo;
  VAL_Result resultStatus = GetMeshInfoFromMeshName( dl_sessionID, modelID, analysisID, stepValue,
						     meshName, meshInfo, error_str);
  if ( resultStatus != VAL_SUCCESS) {
    // error
    _return.__set_result( ( Result::type)resultStatus);
  } else {
    meshID = meshInfo.meshNumber;
    elementType = getStrFromElementType( meshInfo.elementType.shape);
  }
  
  bool storedBoundaryFound = false;
  if ( error_str.length() == 0) {
    std::cout << "Mesh name " << meshName << " has mesh number = " << meshID << " and elementType = " << elementType << std::endl;
    // std::string binary_mesh = "";
    try {
      //AnalyticsModule::getInstance()->calculateBoundaryOfAMesh( GetQueryManagerSessionID( sessionID),
      error_str = "";
      queryServer->deleteStoredBoundaryOfAMesh( GetQueryManagerSessionID( sessionID), 
						modelID,
						meshID, elementType,
						analysisID, stepValue, 
						&error_str);
      // GraphicsModule *graphics = GraphicsModule::getInstance();
      // just to link to the GraphicsModule;
    } catch ( TException &e) {
      std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
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

void QueryManagerServer::ManageGetIsoSurface(Query_Result &_return, const SessionID sessionID, const std::string& query)
{
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string name       = pt.get<std::string>( "name");
  std::string modelID    = pt.get<std::string>( "modelID");
  std::string meshName     = pt.get<std::string>( "meshName"); // in fact it's the mesh name
  std::string analysisID = pt.get<std::string>( "analysisID");
  double stepValue       = pt.get< double>( "stepValue");
  std::string resultName = pt.get<std::string>( "resultName");
  int resultComp       = pt.get<int>( "resultComp");
  double isoValue       = pt.get<double>( "isoValue");


  std::string dl_sessionID = GetDataLayerSessionID(sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "Msh-" << meshName           << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;
  std::cout << "Rn -" << resultName       << "-" << std::endl;
  std::cout << "Rc -" << resultComp       << "-" << std::endl;
  std::cout << "Iv -" << isoValue       << "-" << std::endl;

  // static mesh:
  std::string static_analysisID = "";
  double static_stepValue = 0.0;

  // get MeshID and elementType from MeshName:
  std::string error_str;
  std::string elementType = "";
  int meshID = -1;
  MeshInfo meshInfo;
  VAL_Result resultStatus = GetMeshInfoFromMeshName( dl_sessionID, modelID, static_analysisID, static_stepValue,
						     meshName, meshInfo, error_str);
  if ( resultStatus != VAL_SUCCESS) {
    // error
    _return.__set_result( ( Result::type)resultStatus);
  } else {
    meshID = meshInfo.meshNumber;
    elementType = getStrFromElementType( meshInfo.elementType.shape);
  }

  std::string binary_mesh = "";
  if ( error_str.length() == 0) {
    std::cout << "Mesh name " << meshName << " has mesh number = " << meshID << " and elementType = " << elementType << std::endl;
    try {
      queryServer->calculateIsoSurface(GetQueryManagerSessionID(sessionID), 
				       modelID,
				       meshID,
				       analysisID, stepValue, 
				       resultName, resultComp, isoValue,
				       &binary_mesh, &error_str);
      // GraphicsModule *graphics = GraphicsModule::getInstance();
      // just to link to the GraphicsModule;
    } catch ( TException &e) {
      std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
    }
  }

  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( binary_mesh);
  } else {
    _return.__set_result((Result::type)VAL_UNKNOWN_ERROR);
    _return.__set_data(error_str);
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  isosurface = ( " << _return.data.length() << " bytes)" << std::endl;
  if ( error_str.length() == 0) {
    LOGGER << "  data   : \n" << Hexdump( _return.data, 128) << std::endl;
  } else {
    LOGGER << "  error  : \n" << _return.data << std::endl;
  }
}

void QueryManagerServer::ManageGetSimplifiedMesh( Query_Result &_return, const SessionID sessionID, const std::string& query) {
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
  std::string parameters = pt.get<std::string>( "parameters");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "Msh-" << meshName           << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;
  std::cout << "Par-" << parameters       << "-" << std::endl;

  // in theory should check first if it has already been calculated by doing a
  // queryServer->getBoundingBox( sessionID, query);
  // and access the Simulations_VQuery_Results_Metadata and Simulations_VQuery_Results_Data tables

  // get MeshID and elementType from MeshName:
  std::string error_str;
  std::string elementType = "";
  int meshID = -1;
  MeshInfo meshInfo;
  VAL_Result resultStatus = GetMeshInfoFromMeshName( dl_sessionID, modelID, analysisID, stepValue,
						     meshName, meshInfo, error_str);
  if ( resultStatus != VAL_SUCCESS) {
    // error
    _return.__set_result( ( Result::type)resultStatus);
  } else {
    meshID = meshInfo.meshNumber;
    elementType = getStrFromElementType( meshInfo.elementType.shape);
  }
  
  std::string binary_mesh = "";
  if ( error_str.length() == 0) {
    std::cout << "Mesh name " << meshName << " has mesh number = " << meshID << " and elementType = " << elementType << std::endl;
    try {
      //AnalyticsModule::getInstance()->calculateSimplifiedMesh( GetQueryManagerSessionID( sessionID), 
      queryServer->calculateSimplifiedMesh( GetQueryManagerSessionID( sessionID), 
					    modelID,
					    meshID, elementType,
					    analysisID, stepValue, 
					    parameters,
					    &binary_mesh, &error_str);
      // GraphicsModule *graphics = GraphicsModule::getInstance();
      // just to link to the GraphicsModule;
    } catch ( TException &e) {
      std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
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
  LOGGER << "  simplified_mesh = ( " << _return.data.length() << " bytes)" << std::endl;
  if ( error_str.length() == 0) {
    LOGGER << "  data   : \n" << Hexdump( _return.data, 128) << std::endl;
  } else {
    LOGGER << "  error  : \n" << _return.data << std::endl;
  }
}

void QueryManagerServer::ManageDeleteSimplifiedMesh( Query_Result &_return, const SessionID sessionID, const std::string& query) {
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
  std::string parameters = pt.get<std::string>( "parameters");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "Msh-" << meshName           << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;
  std::cout << "Par-" << parameters       << "-" << std::endl;

  std::string error_str;

  _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR); // default value

  // get MeshID and elementType from MeshName:
  std::string elementType = "";
  int meshID = -1;
  MeshInfo meshInfo;
  VAL_Result resultStatus = GetMeshInfoFromMeshName( dl_sessionID, modelID, analysisID, stepValue,
						     meshName, meshInfo, error_str);
  if ( resultStatus != VAL_SUCCESS) {
    // error
    _return.__set_result( ( Result::type)resultStatus);
  } else {
    meshID = meshInfo.meshNumber;
    elementType = getStrFromElementType( meshInfo.elementType.shape);
  }
  
  bool storedSimplifiedFound = false;
  if ( error_str.length() == 0) {
    std::cout << "Mesh name " << meshName << " has mesh number = " << meshID << " and elementType = " << elementType << std::endl;
    // std::string binary_mesh = "";
    try {
      //AnalyticsModule::getInstance()->calculateSimplifiedMesh( GetQueryManagerSessionID( sessionID),
      error_str = "";
      queryServer->deleteStoredSimplifiedMesh( GetQueryManagerSessionID( sessionID), 
					       modelID,
					       meshID, elementType,
					       analysisID, stepValue, 
					       parameters,
					       &error_str);
      // GraphicsModule *graphics = GraphicsModule::getInstance();
      // just to link to the GraphicsModule;
    } catch ( TException &e) {
      std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
    }
  }

  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    // _return.__set_data( binary_mesh);
  } else {
    if ( !storedSimplifiedFound) {
      _return.__set_result( (Result::type)VAL_SIMPLIFIED_MESH_NOT_FOUND);
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
void QueryManagerServer::ManageGetSimplifiedMeshWithResult( Query_Result &_return, const SessionID sessionID, const std::string& query) {
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
  std::string parameters = pt.get<std::string>( "parameters");
  std::string resultID    = pt.get<std::string>( "resultID");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "Msh-" << meshName           << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;
  std::cout << "Par-" << parameters       << "-" << std::endl;
  std::cout << "Res-" << resultID       << "-" << std::endl;

  // in theory should check first if it has already been calculated by doing a
  // queryServer->getBoundingBox( sessionID, query);
  // and access the Simulations_VQuery_Results_Metadata and Simulations_VQuery_Results_Data tables

  // static mesh:
  std::string static_analysisID = "";
  double static_stepValue = 0.0;
  // get MeshID and elementType from MeshName:
  std::string error_str;
  std::string elementType = "";
  int meshID = -1;
  MeshInfo meshInfo;
  VAL_Result resultStatus = GetMeshInfoFromMeshName( dl_sessionID, modelID, static_analysisID, static_stepValue,
						     meshName, meshInfo, error_str);
  if ( resultStatus != VAL_SUCCESS) {
    // error
    _return.__set_result( ( Result::type)resultStatus);
  } else {
    meshID = meshInfo.meshNumber;
    elementType = getStrFromElementType( meshInfo.elementType.shape);
  }
  
  std::string binary_mesh = "";
  std::string binary_results = "";
  if ( error_str.length() == 0) {
    std::cout << "Mesh name " << meshName << " has mesh number = " << meshID << " and elementType = " << elementType << std::endl;
    try {
      //AnalyticsModule::getInstance()->calculateSimplifiedMesh( GetQueryManagerSessionID( sessionID), 
      queryServer->calculateSimplifiedMeshWithResult( GetQueryManagerSessionID( sessionID), 
						      modelID,
						      meshID, elementType,
						      analysisID, stepValue, 
						      parameters,
						      resultID,
						      &binary_mesh, &binary_results,
						      &error_str);
      // GraphicsModule *graphics = GraphicsModule::getInstance();
      // just to link to the GraphicsModule;
    } catch ( TException &e) {
      std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
    }
  }

  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    int64_t binaryMeshSize = ( int64_t)binary_mesh.size();
    _return.__set_data( std::string( ( const char *)&binaryMeshSize, sizeof( int64_t)) + binary_mesh + binary_results);
  } else {
    _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    _return.__set_data( error_str);
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  simplified_mesh = ( " << _return.data.length() << " bytes)" << std::endl;
  if ( error_str.length() == 0) {
    LOGGER << "  data   : \n" << Hexdump( _return.data, 128) << std::endl;
  } else {
    LOGGER << "  error  : \n" << _return.data << std::endl;
  }
}

void QueryManagerServer::ManageDeleteSimplifiedMeshWithResult( Query_Result &_return, const SessionID sessionID, const std::string& query) {
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
  std::string parameters = pt.get<std::string>( "parameters");
  std::string resultID    = pt.get<std::string>( "resultID");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "Msh-" << meshName           << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;
  std::cout << "Par-" << parameters       << "-" << std::endl;
  std::cout << "Res-" << resultID       << "-" << std::endl;

  std::string error_str;

  _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR); // default value

  // static mesh:
  std::string static_analysisID = "";
  double static_stepValue = 0.0;
  // get MeshID and elementType from MeshName:
  std::string elementType = "";
  int meshID = -1;
  MeshInfo meshInfo;
  VAL_Result resultStatus = GetMeshInfoFromMeshName( dl_sessionID, modelID, static_analysisID, static_stepValue,
						     meshName, meshInfo, error_str);
  if ( resultStatus != VAL_SUCCESS) {
    // error
    _return.__set_result( ( Result::type)resultStatus);
  } else {
    meshID = meshInfo.meshNumber;
    elementType = getStrFromElementType( meshInfo.elementType.shape);
  }
  
  bool storedSimplifiedFound = false;
  if ( error_str.length() == 0) {
    std::cout << "Mesh name " << meshName << " has mesh number = " << meshID << " and elementType = " << elementType << std::endl;
    // std::string binary_mesh = "";
    try {
      //AnalyticsModule::getInstance()->calculateSimplifiedMesh( GetQueryManagerSessionID( sessionID),
      error_str = "";
      queryServer->deleteStoredSimplifiedMeshWithResult( GetQueryManagerSessionID( sessionID), 
							 modelID,
							 meshID, elementType,
							 analysisID, stepValue, 
							 parameters,
							 resultID,
							 &error_str);
      // GraphicsModule *graphics = GraphicsModule::getInstance();
      // just to link to the GraphicsModule;
    } catch ( TException &e) {
      std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
    }
  }

  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    // _return.__set_data( binary_mesh);
  } else {
    if ( !storedSimplifiedFound) {
      _return.__set_result( (Result::type)VAL_SIMPLIFIED_MESH_NOT_FOUND);
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

void QueryManagerServer::ManageDeleteAllCalculationsForThisModel( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string name       = pt.get<std::string>( "name");
  std::string modelID    = pt.get<std::string>( "modelID");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;

  std::string error_str;

  _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR); // default value

  try {
    error_str = "";
    queryServer->deleteAllStoredCalculationsForThisModel( GetQueryManagerSessionID( sessionID), 
							  modelID,
							  &error_str);
  } catch ( TException &e) {
    std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
  } catch ( exception &e) {
    std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
  }

  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    // _return.__set_data( binary_mesh);
  } else {
    _return.__set_data( error_str);
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  if ( error_str.length() != 0) {
    LOGGER << "  error  : \n" << _return.data << std::endl;
  }
}


void QueryManagerServer::ManageGetVolumeLRSplineFromBoundingBox( Query_Result &_return,
								 const SessionID sessionID, const std::string& query) {

  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string name         = pt.get<std::string>( "name");
  std::string modelID      = pt.get<std::string>( "modelID");
  std::string meshName     = pt.get<std::string>( "meshID"); // in fact it's the mesh name
  std::string resultID     = pt.get<std::string>( "resultID");
  double stepValue         = pt.get< double>( "stepValue");
  std::string analysisID   = pt.get<std::string>( "analysisID");
  std::vector<double> bBox = as_vector<double>( pt, "bBox");
  double tolerance         = pt.get< double>( "tolerance");
  int numSteps          = pt.get< int>( "numSteps");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "R-" << resultID           << "-" << std::endl;
  std::cout << "sV -" << stepValue       << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "bB -" << bBox[0] << " " << bBox[1] << " " << bBox[2] << " " << 
    bBox[3] << " " << bBox[4] << " " << bBox[5] << "-" << std::endl;
  std::cout << "T -" << tolerance       << "-" << std::endl;
  std::cout << "nS -" << numSteps       << "-" << std::endl;

  _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR); // default value

  // static mesh:
  std::string static_analysisID = "";
  double static_stepValue = 0.0;
  // get MeshID and elementType from MeshName:
  std::string error_str;
  std::string elementType = "";
  int meshID = -1;
  MeshInfo meshInfo;
  VAL_Result resultStatus = GetMeshInfoFromMeshName( dl_sessionID, modelID, static_analysisID, static_stepValue,
						     meshName, meshInfo, error_str);
  if ( resultStatus != VAL_SUCCESS) {
    // error
    _return.__set_result( ( Result::type)resultStatus);
  } else {
    meshID = meshInfo.meshNumber;
    elementType = getStrFromElementType( meshInfo.elementType.shape);
  }  

  std::string binary_volume_lrspline = "";
  std::string result_statistics;
  if ( error_str.length() == 0) {
    try {
      queryServer->calculateVolumeLRSplineFromBoundingBox( GetQueryManagerSessionID( sessionID), modelID, meshID,
							   resultID, stepValue, analysisID, &bBox[0], tolerance, numSteps,
							   /* out */
							   &binary_volume_lrspline,
							   &result_statistics,
							   &error_str);
      // GraphicsModule *graphics = GraphicsModule::getInstance();
      // just to link to the GraphicsModule;
    } catch ( TException &e) {
      std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
    } catch ( exception &e) {
      std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
    }
    if ( error_str.length() == 0) {
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": The error_str was not set, success!");
      _return.__set_result( (Result::type)VAL_SUCCESS );
      _return.__set_data( binary_volume_lrspline );//result_statistics );
      // @@sbr201609 Skipping the statistics for now.
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
}


void QueryManagerServer::ManageDeleteVolumeLRSplineFromBoundingBox( Query_Result &_return,
								    const SessionID sessionID, const std::string& query) {

  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string name         = pt.get<std::string>( "name");
  std::string modelID      = pt.get<std::string>( "modelID");
  std::string meshName     = pt.get<std::string>( "meshID"); // in fact it's the mesh name
  std::string resultID     = pt.get<std::string>( "resultID");
  double stepValue         = pt.get< double>( "stepValue");
  std::string analysisID   = pt.get<std::string>( "analysisID");
  std::vector<double> bBox = as_vector<double>( pt, "bBox");
  double tolerance         = pt.get< double>( "tolerance");
  double numSteps          = pt.get< double>( "numSteps");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "R-" << resultID           << "-" << std::endl;
  std::cout << "sV -" << stepValue       << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "bB -" << bBox[0] << " " << bBox[1] << " " << bBox[2] << " " << 
    bBox[3] << " " << bBox[4] << " " << bBox[5] << "-" << std::endl;
  std::cout << "T -" << tolerance       << "-" << std::endl;
  std::cout << "nS -" << numSteps       << "-" << std::endl;

  _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR); // default value

  // static mesh:
  std::string static_analysisID = "";
  double static_stepValue = 0.0;
  // get MeshID and elementType from MeshName:
  std::string error_str;
  std::string elementType = "";
  int meshID = -1;
  MeshInfo meshInfo;
  VAL_Result resultStatus = GetMeshInfoFromMeshName( dl_sessionID, modelID, static_analysisID, static_stepValue,
						     meshName, meshInfo, error_str);
  if ( resultStatus != VAL_SUCCESS) {
    // error
    _return.__set_result( ( Result::type)resultStatus);
  } else {
    meshID = meshInfo.meshNumber;
    elementType = getStrFromElementType( meshInfo.elementType.shape);
  }

  bool storedVolumeLRSplineFound = false; // @@sbr201609 This value is never changed ...
  try {
    //AnalyticsModule::getInstance()->calculateBoundaryOfAMesh( GetQueryManagerSessionID( sessionID),
    error_str = "";
    queryServer->deleteVolumeLRSplineFromBoundingBox( GetQueryManagerSessionID( sessionID), modelID, meshID,
						      resultID, stepValue, analysisID, &bBox[0], tolerance, numSteps,
						      &error_str);
    // GraphicsModule *graphics = GraphicsModule::getInstance();
    // just to link to the GraphicsModule;
  } catch ( TException &e) {
    std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
  } catch ( exception &e) {
    std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
  }

  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    // _return.__set_data( binary_mesh);
  } else {
    if ( !storedVolumeLRSplineFound) {
      _return.__set_result( (Result::type)VAL_VOLUME_LRSPLINE_NOT_FOUND);
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

void QueryManagerServer::ManageDoStreamlinesWithResult( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // double bbox[ 6] = { -0.5, -0.5, -0.5, 0.5, 0.5, 0.5};
  // _return.__set_data( std::string( ( const char *)&bbox[ 0], 6 * sizeof( double)));
  // _return.__set_result( (Result::type)VAL_SUCCESS );

  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  // get parameters:
  std::string modelID               = pt.get<std::string>( "modelID");
  std::string analysisID            = pt.get<std::string>( "analysisID");
  double      timeStep              = pt.get<double>( "stepValue");
  std::string resultID              = pt.get<std::string>( "resultID");
  int64_t     numSeedingPoints      = pt.get<int64_t>( "numSeedingPoints");
  std::string seedingPointsStr      = pt.get<std::string>( "seedingPoints");
  std::string integrationMethod     = pt.get<std::string>( "integrationMethod");
  int64_t     integrationSteps      = pt.get<int64_t>( "maxIntegrationSteps");
  double      stepSize              = pt.get<double>( "stepSize" );
  double      maxStreamlinesLength  = pt.get<double>( "maxStreamLineLength");
  std::string tracingDirection      = pt.get<std::string>( "tracingDirection");
  std::string interpolateInCellsStr = pt.get<std::string>( "interpolateInCells" );
  std::string adaptiveStepping      = pt.get<std::string>( "adaptiveStepping");
  std::string bSplineApproxStr      = pt.get<std::string>( "approxBSpline" );
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::string seedingPointsData = base64_decode( seedingPointsStr);
  size_t seedingPointsCount = seedingPointsData.length() / sizeof( double);
  double *lstSeedingPoints = ( double*)seedingPointsData.data();
  //std::vector<glm:> listOfVertices( lst_vertexIDs, lst_vertexIDs + num_vertexIDs);
  

  std::cout << "S       " << sessionID              << std::endl;
  std::cout << "dlS     " << dl_sessionID           << std::endl;
  std::cout << "M      -" << modelID                << "-" << std::endl;
  std::cout << "anlID  -" << analysisID             << "-" << std::endl;
  std::cout << "stpVal -" << timeStep               << "-" << std::endl;
  std::cout << "Rid    -" << resultID               << "-" << std::endl;
  std::cout << "nSPs   -" << numSeedingPoints       << "-" << std::endl;
  std::cout << "intMet -" << integrationMethod      << "-" << std::endl;
  std::cout << "intNStp-" << integrationSteps       << "-" << std::endl;
  std::cout << "intStp -" << stepSize               << "-" << std::endl;
  std::cout << "maxLen -" << maxStreamlinesLength   << "-" << std::endl;
  std::cout << "tDir   -" << tracingDirection       << "-" << std::endl;
  std::cout << "intCl  -" << interpolateInCellsStr  << "-" << std::endl;
  std::cout << "adStp  -" << adaptiveStepping       << "-" << std::endl;
  std::cout << "apSpl  -" << bSplineApproxStr       << "-" << std::endl;

  std::cout << "SPnts  -\n";
  for(size_t i = 0; i < 10; i++){
    std::cout << lstSeedingPoints[i] << " ";
    if(i % 3 == 2) std::cout << std::endl;
  }
  std::cout << "\n\n";

  std::string error_str;
  //std::cout << "looking for the Mesh " << meshName << " in order to get it's id" << std::endl;
	  rvGetListOfMeshes _return_;
	  queryServer->getListOfMeshes( _return_, dl_sessionID, modelID, "", 0.0);
	  MeshInfo meshInfo;
	  meshInfo.meshNumber = -1;      //<<< To mark the meshInfo as non-initalized.
	  std::string elementType = "";
	  if ( _return_.meshInfos.size() == 0) {
		_return.__set_result( (Result::type)VAL_NO_MESH_INFORMATION_FOUND);
		error_str = "There is no mesh metadata.";
	  } else {
      std::vector< MeshInfo>::iterator it = _return_.meshInfos.begin();
      meshInfo = *it;
		  /*for ( std::vector< MeshInfo>::iterator it = _return_.meshInfos.begin();
			  it != _return_.meshInfos.end(); it++) {
		      if ( AreEqualNoCase( it->name, meshName)) {
			      meshInfo = (*it);
			      break;
		      }
		    }
		  if ( meshInfo.meshNumber == -1 ) { // not found
		    error_str = "Mesh name " + meshName + " not in metadata.";
		    std::cout << error_str << std::endl;
		  }*/
  }
  std::cout << meshInfo.name << " is picked.\n";

  std::ostringstream oss;
  try {

    UnstructDataset dataset;
    std::vector<Streamline> streamlines;

    std::string localCachedFile = "/tmp/"+meshInfo.name+".bin";
    std::string localCachedAccelFile = "/tmp/"+meshInfo.name+"_accel.bin";

    if(!dataset.loadBinary(localCachedFile)){
    // struct Vertex {
    //   1: NodeID                              id
    //   2: double                              x
    //   3: double                              y
    //   4: double                              z
    // }
  #if 1
    std::vector<VELASSCO::Coord>    coords;
    std::vector<VELASSCO::Vector3D> results;
    std::vector<VELASSCO::Cell>     cells;

    // struct rvGetListOfVerticesFromMesh {
    //   1: string            status
    //   2: string            report
    //   3: list<Vertex>      vertex_list
    // }
      rvGetListOfVerticesFromMesh return_vertices;
      queryServer->getListOfVerticesFromMesh( return_vertices, dl_sessionID, modelID , "" , 0.0, meshInfo.meshNumber );

      std::vector<int64_t> vertexIDs(return_vertices.vertex_list.size());
      coords.resize(return_vertices.vertex_list.size());
      for(int64_t i = 0; i < static_cast<int64_t>(return_vertices.vertex_list.size()); i++){
        coords[i].idx = return_vertices.vertex_list[i].id;
        coords[i].coord[0] = return_vertices.vertex_list[i].x;
        coords[i].coord[1] = return_vertices.vertex_list[i].y;
        coords[i].coord[2] = return_vertices.vertex_list[i].z;
        vertexIDs[i] = return_vertices.vertex_list[i].id;
      }

      // struct rvGetCoordinatesAndElementsFromMesh {
      //   1: string status
      //   2: string report
      //   3: list<Vertex>        vertex_list			// not used.
      //   4: list<Element>       element_list
      //   5: list<ElementAttrib> element_attrib_list
      //   6: list<ElementGroup>  element_group_info_list
      // }
      rvGetCoordinatesAndElementsFromMesh _return_;
      queryServer->getCoordinatesAndElementsFromMesh( _return_, dl_sessionID, modelID ,"" , 0.0, meshInfo );
      
      // struct Element {
      // 1: i64                                   id
      // 2: list<NodeID>                          nodes_ids
      cells.resize(_return_.element_list.size());
      for(size_t cellID = 0; cellID <= _return_.element_list.size(); cellID++){
        for(size_t n = 0; n < _return_.element_list[cellID].nodes_ids.size(); n++){
          cells[cellID].cornerIndices.push_back(_return_.element_list[cellID].nodes_ids[n]);
        }
      }

      // struct ResultOnVertex {
      //   1: i64                                  id
      //   2: list<double>                         value
      //   3: binary                               bvalue
      // }
      std::string result_report;
      //std::vector<ResultOnVertex> resultOnVertices;
      rvGetResultFromVerticesID _return_results;
      queryServer->getResultFromVerticesID(_return_results, dl_sessionID, modelID, analysisID, timeStep, resultID, vertexIDs);
      results.resize(_return_results.result_list.size());
      for(size_t r = 0; r < _return_results.result_list.size(); r++){
        size_t c = 0;
        results[r].idx = _return_results.result_list[r].id;
        for(; c < _return_results.result_list[r].value.size(); c++){
          results[r].vector[c] = _return_results.result_list[r].value[c];
        }
        for(; c < 3; c++){
          results[r].vector[c] = 0.0;
        }
      }

      dataset.reset(coords, results, cells);
      dataset.saveBinary(localCachedFile);
    }

#if 1
    for(int64_t i = 0; i < numSeedingPoints; i++){
      streamlines.push_back(Streamline(glm::dvec3(lstSeedingPoints[3 * i + 0], lstSeedingPoints[3 * i + 1], lstSeedingPoints[3 * i + 2])));
    }
 #else
    streamlines.push_back(Streamline(glm::dvec3(9000.0, 2300.0, 2400.0)));
#endif

    // Acceleration Structure Computation
    if(!dataset.loadAccelBinary(localCachedAccelFile)){
      dataset.computeAccel();
      dataset.saveAccelBinary(localCachedAccelFile);
    }
    
    StreamTracer tracer;
    SurfaceParameters params;

    if(tracingDirection == "FORWARD") {
      params.traceDirection         = TraceDirection::TD_FORWARD;
      //std::cout << "  tracing direction = FORWARD\n";
    } else if (tracingDirection == "BACKWARD") {
      params.traceDirection         = TraceDirection::TD_BACKWARD;  
      //std::cout << "  tracing direction = BACKWARD\n";
    } else {
      params.traceDirection         = TraceDirection::TD_BOTH;
      //std::cout << "  tracing direction = BOTH\n";
    }

    if(adaptiveStepping == "OFF"){
      params.doStepAdaptation       = false;
      //std::cout << "  step adaptaton = OFF\n";
    } else {
      params.doStepAdaptation       = true;
      //std::cout << "  step adaptaton = ON\n";
    }

    if(integrationMethod == "EULER"){
      params.traceIntegrationMethod = IntegrationMethod::IM_EULER;
      //std::cout << "  integration method = euler\n";
    } else {
      params.traceIntegrationMethod = IntegrationMethod::IM_CASHKARP;
      //std::cout << "  integration method = cash karp\n";
    }

   
    params.traceStepSize              = stepSize;
    params.traceMaxSteps              = integrationSteps;
    params.traceSpecialBackStepsLimit = 10;
    //params.traceMaxLen            = maxStreamlinesLength;

    tracer.setParameters(params);

    // ==========================================
    // ==       Tracing streamlines            ==
    // ==========================================
    std::cout << "Tracing streamlines..." << std::endl;
    tracer.traceStreamline(&dataset, streamlines, params.traceStepSize);
    std::cout << "Tracing streamlines...Done." << std::endl;

    //const std::vector<glm::dvec3>& retPoints  = streamlines[0].points();
    //const std::vector<glm::dvec3>& retResults = streamlines[0].results();
    //for(size_t i = 0; i < retPoints.size(); i++){
    //  std::cout << retPoints[i].x << " " << retPoints[i].y << " " << retPoints[i].z << std::endl;
    //  std::cout << retResults[i].x << " " << retResults[i].y << " " << retResults[i].z << std::endl;
    //}
  
#else
    for(int s = 0; s < 10; s++){
      streamlines.push_back(Streamline(glm::dvec3(0.0)));
      int randLen = rand() % 100000;
      for(int i = 0; i < randLen; i++){
        glm::dvec3 p, r;
        p.x = (rand() % 1000) / 1000.0;
        p.y = (rand() % 1000) / 1000.0;
        p.z = (rand() % 1000) / 1000.0;

        r.x = (rand() % 1000) / 1000.0;
        r.y = (rand() % 1000) / 1000.0;
        r.z = (rand() % 1000) / 1000.0;

        streamlines[s].addFrontPoint(p, r);
      }
    }
#endif

    bool approximateStreamlinesWithBSplines = bSplineApproxStr == "ON" ? true : false;

    if(!approximateStreamlinesWithBSplines){

      // ==========================================
      // == No approximation of streamlines case ==
      // ==========================================

      size_t nRetStreamlines = streamlines.size();

      if(nRetStreamlines > 0){

        oss.write((char*)&nRetStreamlines, sizeof(size_t));
        for(size_t i = 0; i < streamlines.size(); i++){
          size_t streamlineLen = streamlines[i].points().size();
          oss.write((char*)&streamlineLen, sizeof(size_t));
        }

        for(size_t i = 0; i < streamlines.size(); i++){
          const std::vector<glm::dvec3>& retPoints = streamlines[i].points();
          if(retPoints.size() > 0)
            oss.write((char*)retPoints.data(), retPoints.size() * sizeof(glm::dvec3));
        }

        for(size_t i = 0; i < streamlines.size(); i++){
          const std::vector<glm::dvec3>& retResults  = streamlines[i].results();
          if(retResults.size() > 0)
            oss.write((char*)retResults.data(), retResults.size() * sizeof(glm::dvec3));
        }
      }
    } else {

      // ============================================
      // == Approximate streamlines with B-Spline  ==
      // ============================================
    
      size_t nRetStreamlines = streamlines.size();

      if(nRetStreamlines > 0){

        std::vector<std::vector<Eigen::Vector3d>> controlPoints(nRetStreamlines);

        std::cout << "Approximating Streamlines as B-Spline... "<< std::endl;
//#pragma omp parallel
{
		    int tid         = omp_get_thread_num();
		    int numThreads  = omp_get_num_threads();
		    int chunk_size  = static_cast<int>(std::floor((double)nRetStreamlines/(double)numThreads));
		    int chunk_share = tid == numThreads - 1 ? nRetStreamlines - (numThreads - 1) * chunk_size : chunk_size;
        for(int i = 0; i < chunk_share; i++){
          size_t streamlineIdx = tid * chunk_size + i;

          std::vector<Eigen::Vector3d> streamlinePoints;
          const std::vector<glm::dvec3>& retPoints = streamlines[streamlineIdx].points();
          for(size_t p = 0; p < retPoints.size(); p++)
             streamlinePoints.push_back(Eigen::Vector3d(retPoints[p].x, retPoints[p].y, retPoints[p].z));
          BSplineApprox bsplineApproximator = BSplineApprox(streamlinePoints);
          controlPoints[streamlineIdx].clear();
          controlPoints[streamlineIdx] = bsplineApproximator.getBSplineControlPoints();

          //std::cout << "Number of streamline points are reduced from " << retPoints.size() << " to " << controlPoints[i].size() << std::endl;

        }
}
        std::cout << "Approximating Streamlines as B-Spline...Done."<< std::endl;

        // =====================================
        // == Detecting Suspicous Streamlines
        // =====================================
        std::map<size_t, bool> suspicous;
        size_t nStreamlines = 0;
        for(size_t i = 0; i < nRetStreamlines; i++){
          if(controlPoints[i].size() <= 3){
            suspicous[i] = true;
            continue;
          }

          double streamlineLength = 0.0;
          for(size_t j = 1; j < controlPoints[i].size(); j++){
            streamlineLength += (controlPoints[i][j - 1] - controlPoints[i][j]).norm();
          }
          if(streamlineLength > 10000){
            suspicous[i] = true;
            continue;
          }

          suspicous[i] = false;

          // Counting the number of valid streamlines (B-Splines)
          nStreamlines++;
        }

        //oss.write((char*)&nRetStreamlines, sizeof(size_t));
        oss.write((char*)&nStreamlines, sizeof(size_t));
        
        for(size_t i = 0; i < nRetStreamlines; i++){
          size_t streamlineLen = controlPoints[i].size();
          // Ignoring invalid streamlines (B-Splines)
          if(suspicous[i]) continue;
          oss.write((char*)&streamlineLen, sizeof(size_t));
        }

        for(size_t i = 0; i < nRetStreamlines; i++){
          const std::vector<Eigen::Vector3d>& retPoints = controlPoints[i];
          // Ignoring invalid streamlines (B-Splines)
          if(suspicous[i]) continue;
          oss.write((char*)retPoints.data(), retPoints.size() * sizeof(Eigen::Vector3d));
        }

       
#if 1
        // ===================================
        // == Sampling the results
        // ===================================
        std::cout << "Sampling the results on the streamline..." << std::endl;
        for(size_t i = 0; i < nRetStreamlines; i++){
          const std::vector<glm::dvec3>& retPoints  = streamlines[i].points();
          const std::vector<glm::dvec3>& retResults = streamlines[i].results();

          // Ignoring invalid streamlines (B-Splines)
          if(suspicous[i]) continue;

          const size_t nResultSamples = 1024;
          std::vector<glm::dvec3> sampledResults(nResultSamples, glm::dvec3(0.0));

          double streamlineLength = 0.0;
          for(size_t i = 1; i < retPoints.size(); i++)
            streamlineLength += glm::length(retPoints[i] - retPoints[i-1]);

          double currStreamlineLength = 0.0;
          int idx = 1;
          double percentage = static_cast<double>(idx) / static_cast<double>(nResultSamples - 1);
          sampledResults[0]                   = retResults[0];
          sampledResults[nResultSamples - 1]  = retResults.back();

          size_t j = 0;
          while(j < retResults.size() - 1){
#if 0
            double currPointPercentage = static_cast<double>(j+0) / static_cast<double>(retResults.size());
            double nextPointPercentage = static_cast<double>(j+1) / static_cast<double>(retResults.size());
#else
            double currPointPercentage = currStreamlineLength / streamlineLength;
            double nextPointPercentage = (currStreamlineLength + glm::length(retPoints[j+1] - retPoints[j])) / streamlineLength;
#endif
            if(percentage >= currPointPercentage && percentage <= nextPointPercentage){
              double t = (percentage - currPointPercentage) / (nextPointPercentage - currPointPercentage);

              glm::dvec3 r = t * retResults[j+1] + (1 - t) * retResults[j];
              sampledResults[idx] = r;
              
              //std::cout << percentage << std::endl;

              idx++;
              percentage = static_cast<double>(idx) / static_cast<double>(nResultSamples - 1);
              if(idx >= nResultSamples - 1) break;
            } else {
              j++;
              currStreamlineLength += glm::length(retPoints[j+1] - retPoints[j]);
            }
          }

          if(sampledResults.size() > 0)
            oss.write((char*)sampledResults.data(), sampledResults.size() * sizeof(glm::dvec3));

        }

        std::cout << "Sampling the results on the streamline...Done." << std::endl;
#else 
        for(size_t i = 0; i < nRetStreamlines; i++){
          const std::vector<Eigen::Vector3d>& retPoints = controlPoints[i];
          if(retPoints.size() > 0)
            oss.write((char*)retPoints.data(), retPoints.size() * sizeof(Eigen::Vector3d));
        }
#endif
      }

    }

  } catch ( TException &e) {
    std::cout << "EXCEPTION CATCH_ERROR 1: " << e.what() << std::endl;
  } catch ( exception &e) {
    std::cout << "EXCEPTION CATCH_ERROR 2: " << e.what() << std::endl;
  }

  
  if ( error_str.length() == 0) {
    _return.__set_result( (Result::type)VAL_SUCCESS );
    _return.__set_data( oss.str() );
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