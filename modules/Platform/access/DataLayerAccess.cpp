#include <sstream>
#include <stddef.h>  // defines NULL

#include "HBase.h"

#include "DataLayerAccess.h"
#include "cJSON.h"
#include "Helpers.h"
#include "base64.h"

using namespace VELaSSCo;

// Global static pointer used to ensure a single instance of the class.
DataLayerAccess* DataLayerAccess::m_pInstance = NULL;


DataLayerAccess* DataLayerAccess::Instance()
{
  // Only allow one instance of class to be generated.
  if (!m_pInstance)
    {
      m_pInstance = new DataLayerAccess;
    }
  return m_pInstance;
}


bool DataLayerAccess::startConnection( const char *data_layer_hostname, const int data_layer_port)
{
  /*  DEBUG( "Connecting to Data Layer at " << data_layer_hostname << ":" << data_layer_port);
      socket= boost::shared_ptr<TTransport>(new TSocket( data_layer_hostname, data_layer_port));
      //    transport = boost::shared_ptr<TTransport>(new TBufferedTransport(socket));
      transport = boost::shared_ptr<TTransport>(new TFramedTransport(socket));
      protocol = boost::shared_ptr<TProtocol>(new TBinaryProtocol(transport));
      cli = new VELaSSCoSMClient(protocol);
      try
      {
        
      transport->open();
      // just testing connection:
      // string status;
      // getStatusDB( status);
      // cout << "\t########## getStatus - " << status << endl;
      // now it's a VQuery so it can be issued by the Viz Client
      }
      catch (TException& tx)
      {
      cout << "ERROR: " << tx.what() << endl;
      }
    
      return true;
  */

  //stopConnection(); // close any previous connections
  // no need for AbstractDB.h anymore ...
  // _db = (AbstractDB*)  new HBase();
  _db = new HBase();
  return _db->startConnection( data_layer_hostname, data_layer_port);
}

bool DataLayerAccess::stopConnection()
{
  /* try
     {
        
     transport->close();
     } catch (TException& tx) {
     cout << "ERROR: " << tx.what() << endl;
     }*/
  bool ok = true;
  if ( _db) {
    ok = _db->stopConnection();
    delete _db;
    _db = NULL;
  }
  return ok;
}

void DataLayerAccess::getStatusDB( std::string& _return) {
  _return = _db->getStatusDB();
}

void DataLayerAccess::userLogin( std::string &_return, 
				 const std::string &name, const std::string &role, const std::string &password) {
  /*  try {
      cli->UserLogin( _return, name, role, password);
      } catch (TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
}

void DataLayerAccess::userLogout( std::string &_return, const std::string &sessionID) {
  /*  try {
      cli->UserLogout( _return, sessionID);
      } catch (TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
}

void DataLayerAccess::getListOfModels( rvGetListOfModels &_return,
				       const std::string &sessionID, 
				       const std::string &model_group_qualifier, const std::string &model_name_pattern) {
  /*  try {
      cli->GetListOfModelNames( _return, sessionID, model_group_qualifier, model_name_pattern);
      } catch ( TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
  std::string report;
  std::vector< FullyQualifiedModelName>listOfModelNames;
  std::string status = _db->getListOfModelNames( report, listOfModelNames, sessionID, 
						 model_group_qualifier, model_name_pattern);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_models( listOfModelNames);
  
}

void DataLayerAccess::openModel( rvOpenModel &_return,
				 const std::string &sessionID, 
				 const std::string &unique_name, const std::string &requested_access) {
  /*  try {
      cli->FindModel( _return, sessionID, unique_name, requested_access); // requested_access not used at the momemnt
      } catch ( TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
  std::string report;
  std::string modelID;
  FullyQualifiedModelName model_info;
  std::string status = _db->findModel( report, modelID, model_info, sessionID, unique_name, requested_access);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_modelID( modelID);
  _return.__set_model_info( model_info);
}

void DataLayerAccess::closeModel( std::string &_return,
				  const std::string &sessionID, 
				  const std::string &modelID) {
  /*  try {
      cli->CloseModel( _return, sessionID, modelID); // requested_access not used at the momemnt
      } catch ( TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
}

void DataLayerAccess::getResultFromVerticesID( rvGetResultFromVerticesID &_return,
					       const std::string &sessionID, const std::string &modelID, 
					       const std::string &analysisID, const double timeStep,
					       const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID ){
  /*    try
	{
        
        cli->GetResultFromVerticesID( _return, sessionID, modelID, analysisID, timeStep, resultID, listOfVerticesID );
        // cout << "########## getResultFromVerticesID - " << _return << endl;
	}
	catch (TException& tx)
	{
        cout << "ERROR: " << tx.what() << endl;
	}*/
  std::string report;
  std::vector< ResultOnVertex > listOfResults;
  std::string status  = _db->getResultFromVerticesID(report, listOfResults, sessionID, modelID, analysisID, timeStep, resultID, listOfVerticesID);	
  _return.__set_status( status );
  _return.__set_report( report );
  _return.__set_result_list( listOfResults );

}

void DataLayerAccess::getCoordinatesAndElementsFromMesh( 
							rvGetCoordinatesAndElementsFromMesh& _return,
							const std::string& sessionID, const std::string& modelID, const std::string& analysisID, 
							const double timeStep, const MeshInfo& meshInfo )
{
  /*	try
	{
        
        cli->GetCoordinatesAndElementsFromMesh( _return, sessionID, modelID, analysisID, timeStep, meshInfo );
        //cout << "########## getCoordinatesAndElementsFromMesh - " << _return << endl;
	}
	catch (TException& tx)
	{
        cout << "ERROR: " << tx.what() << endl;
	}*/
  std::string                  report;
  std::vector< Vertex        > listOfVertices;
  std::vector< Element       > listOfElements;
  std::vector< ElementAttrib > listOfElementAttribs;
  std::vector< ElementGroup  > listOfElementGroupInfo;
  std::string status = _db->getCoordinatesAndElementsFromMesh(
							      report, listOfVertices, listOfElements, listOfElementAttribs, listOfElementGroupInfo, 
							      sessionID, modelID, analysisID, timeStep, meshInfo);
  _return.__set_status( status );
  _return.__set_report( report );
  _return.__set_vertex_list ( listOfVertices );
  _return.__set_element_list( listOfElements );
  _return.__set_element_attrib_list( listOfElementAttribs );
  _return.__set_element_group_info_list( listOfElementGroupInfo );	
}
                  
void DataLayerAccess::getListOfMeshes( rvGetListOfMeshes &_return,
				       const std::string &sessionID,
				       const std::string &modelID,
				       const std::string &analysisID,
				       const double stepValue) {
  /*  try {
      cli->GetListOfMeshes( _return, sessionID, modelID, analysisID, stepValue);
      } catch ( TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
  std::string report;
  std::vector< MeshInfo>listOfMeshes;
  std::string status = _db->getListOfMeshes( report, listOfMeshes, sessionID, modelID, analysisID, stepValue);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_meshInfos( listOfMeshes);
}

void DataLayerAccess::getListOfAnalyses( rvGetListOfAnalyses &_return,
					 const std::string &sessionID,
					 const std::string &modelID) {
  /*  try {
      cli->GetListOfAnalyses( _return, sessionID, modelID);
      } catch ( TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
  std::string report;
  std::vector< std::string>listOfAnalyses;
  std::string status = _db->getListOfAnalyses( report, listOfAnalyses, sessionID, modelID);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_analyses( listOfAnalyses);
}

/* as of OP-22.113 */
void DataLayerAccess::getListOfTimeSteps( rvGetListOfTimeSteps &_return,
					  const std::string &sessionID,
					  const std::string &modelID,
					  const std::string &analysisID,
					  const std::string &stepOptions, const int numSteps, const double *lstSteps) {
  // step_list if step_options == SINGLE( 1), INTERVAL( 2), SET( N_steps)
  // cout << "ERROR: " << FUNCTION_NAME << " NOT IMPLEMENTED YET." << endl;
  try {
  
  
  
    //cli->GetListOfTimeSteps( _return, sessionID, modelID, analysisID);
    
    
    std::string report;
    std::vector< double>listOfSteps;
    std::string status = _db->getListOfSteps( report, listOfSteps, sessionID, modelID, analysisID);
    _return.__set_status( status);
    _return.__set_report( report);
    _return.__set_time_steps( listOfSteps);
    
    
    
    
    if ( _return.time_steps.size() > 0) {
      // now process the output according to stepOptions, numSteps and lstSteps
      if ( !strcasecmp( stepOptions.c_str(), "all")) {
	// all of them are ok
      } else if ( !strcasecmp( stepOptions.c_str(), "interval") && lstSteps && ( numSteps >= 2)) {
	// loop through _return.time_steps and get the interval
	std::vector< double> tmp;
	double min = lstSteps[ 0];
	double max = lstSteps[ 1];
	if ( max < min) { double tmp = min; min = max; max = tmp;}
	for ( std::vector< double>::iterator it = _return.time_steps.begin();
	      it != _return.time_steps.end(); it++) {
	  if ( ( min < *it) && ( *it < max)) {
	    tmp.push_back( *it);
	  }
	}
	_return.__set_time_steps( tmp);
      } else if ( ( !strcasecmp( stepOptions.c_str(), "single") || 
		    !strcasecmp( stepOptions.c_str(), "set"))
		  && lstSteps && numSteps) {
	// loop through _return.time_steps and see if all are present.
	std::vector< double> tmp;
	for ( std::vector< double>::iterator it = _return.time_steps.begin();
	      it != _return.time_steps.end(); it++) {
	  for ( int i = 0; i < numSteps; i++) {
	    if ( *it == lstSteps[ i]) {
	      tmp.push_back( *it);
	    }
	  }
	}
	_return.__set_time_steps( tmp);
      } else {
	// not possible, returning the findings
      }
    }
  } catch ( TException& tx) {
    cout << "EXCEPTION ERROR: " << tx.what() << endl;
  }
}

/* as of OP-22.115 */
void DataLayerAccess::getListOfResultsFromTimeStepAndAnalysis( rvGetListOfResults &_return,
							       const std::string &sessionID,
							       const std::string &modelID,
							       const std::string &analysisID,
							       const double stepValue) {
  /*  try {
      cli->GetListOfResultsFromTimeStepAndAnalysis( _return, sessionID, modelID, analysisID, stepValue);
      } catch ( TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
  std::string report;
  std::vector< ResultInfo>listOfResults;
  std::string status = _db->getListOfResults( report, listOfResults, sessionID, modelID, analysisID, stepValue);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_result_list( listOfResults);
}

bool DataLayerAccess::getResultInfoFromResultName( const std::string &sessionID, const std::string &modelID, 
						   const std::string &analysisID, const double stepValue, 
						   const std::string &resultName, ResultInfo &outResultInfo) {
  return _db->getResultInfoFromResultName( sessionID, modelID, analysisID, stepValue, resultName, 
					   outResultInfo);
}

/* as of OP-22.116 */
void DataLayerAccess::getListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
						 const std::string &sessionID, const std::string &modelID, 
						 const std::string &analysisID, const double stepValue, 
						 const int32_t meshID) {
  /*  try {
      cli->GetListOfVerticesFromMesh( _return, sessionID, modelID, analysisID, stepValue, meshID);
      } catch ( TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
  std::string report;
  std::vector< Vertex> listOfVertices;
  std::string status = _db->getListOfVerticesFromMesh( report, listOfVertices, sessionID, modelID, analysisID, stepValue, meshID);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_vertex_list( listOfVertices);
}

void DataLayerAccess::getListOfSelectedVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
							 const std::string &sessionID, const std::string &modelID, 
							 const std::string &analysisID, const double stepValue, 
							 const int32_t meshID, const std::vector<int64_t> &listOfVerticesID) {
  /*  try {
      cli->GetListOfVerticesFromMesh( _return, sessionID, modelID, analysisID, stepValue, meshID);
      } catch ( TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
      }*/
  std::string report;
  std::vector< Vertex> listOfVertices;
  std::string status = _db->getListOfSelectedVerticesFromMesh( report, listOfVertices, sessionID, modelID, analysisID, stepValue, meshID, listOfVerticesID);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_vertex_list( listOfVertices);
}

void DataLayerAccess::stopAll()
{
  /*    try
	{
        
        cli->stopAll();
        cout << "\t########## stopAll "  << endl;
	}
	catch (TException& tx)
	{
        cout << "ERROR: " << tx.what() << endl;
	}
	this->stopConnection();*/
}





void DataLayerAccess::calculateBoundingBox( const std::string &sessionID, const std::string &modelID, 
					    const std::string &analysisID, const int numSteps, const double *lstSteps,
					    const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
					    double *return_bbox, std::string *return_error_str) {
  if ( return_bbox) {
    _db->getStoredBoundingBox( sessionID, modelID, analysisID, numSteps, lstSteps,
			       numVertexIDs, lstVertexIDs,
			       return_bbox, return_error_str);
    if ( return_error_str->length() != 0) { // nothing found
      *return_error_str = ""; // reset error string
      HBase::TableModelEntry table_name_set;
      if ( _db->getVELaSSCoTableNames(sessionID, modelID, table_name_set)) {
	return_error_str->clear();
	AnalyticsModule::getInstance()->calculateBoundingBox( sessionID, modelID,
							      table_name_set._data,
							      analysisID, numSteps, lstSteps,
							      numVertexIDs, lstVertexIDs,
							      return_bbox, return_error_str);
      }
      if ( return_error_str->length() == 0) { // i.e. no error
	std::string save_err_str;
	_db->saveBoundingBox( sessionID, modelID, analysisID, numSteps, lstSteps,
			      numVertexIDs, lstVertexIDs,
			      return_bbox, &save_err_str);
      }
    }
  }
}

// needed by DeleteBoundingBox vquery
void DataLayerAccess::deleteStoredBoundingBox( const std::string &sessionID, const std::string &modelID, 
					       const std::string &analysisID, const int numSteps, const double *lstSteps,
					       const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
					       std::string *return_error_str) {
  _db->getStoredBoundingBox( sessionID, modelID, analysisID, numSteps, lstSteps,
			     numVertexIDs, lstVertexIDs,
			     NULL, return_error_str);
  if ( return_error_str->length() == 0) { // i.e. bounding box found
    _db->deleteStoredBoundingBox( sessionID, modelID, analysisID, numSteps, lstSteps,
				  numVertexIDs, lstVertexIDs,
				  return_error_str);
  } else {
    return_error_str->clear(); // not found, // already deleted?
  }
}



void DataLayerAccess::calculateDiscrete2Continuum(const std::string &sessionID, const std::string &modelID,
						  const std::string &analysisID, const std::string &staticMeshID, 
						  const std::string &stepOptions, const int numSteps, const double *lstSteps,
						  const std::string &cGMethod, const double width, const double cutoffFactor,
						  const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGOptions,
						  const double deltaT, std::string *returnQueryOutcome, std::string *return_error_str) {
  
  AnalyticsModule::getInstance()->calculateDiscrete2Continuum( sessionID, modelID,
							       analysisID, staticMeshID, 
							       stepOptions, numSteps, lstSteps,
							       cGMethod, width, cutoffFactor, 
							       processContacts, doTemporalAVG, temporalAVGOptions, 
							       deltaT, returnQueryOutcome, return_error_str);	
}

// std::string DataLayerAccess::MRgetListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &return_data, 
// 							  const std::string &sessionID, const std::string &modelID, 
// 							  const std::string &dataTableName,
// 							  const std::string &analysisID, const double stepValue, 
// 							  const int32_t meshID) {
//     return AnalyticsModule::getInstance()->MRgetListOfVerticesFromMesh(return_data,
// 								       sessionID, modelID,
// 								       dataTableName,
// 								       analysisID, stepValue,
// 								       meshID);
// }

// #include "../../AccessLib/AccessLib/BoundaryBinaryMesh.h"
void DataLayerAccess::calculateBoundaryOfAMesh( const std::string &sessionID,
						const std::string &modelID,
						const int meshID, const std::string &elementType,
						const std::string &analysisID, const double stepValue,
						std::string *return_binary_mesh, std::string *return_error_str) {
  if ( return_binary_mesh) {
    return_binary_mesh->clear();
    _db->getStoredBoundaryOfAMesh( sessionID, modelID, meshID, elementType, analysisID, stepValue,
				   return_binary_mesh, return_error_str);
    if ( return_binary_mesh->length() == 0) { // nothing found
      *return_error_str = ""; // reset error string
      HBase::TableModelEntry table_name_set;
      if ( _db->getVELaSSCoTableNames( sessionID, modelID, table_name_set)) {
	AnalyticsModule::getInstance()->calculateBoundaryOfAMesh( sessionID, modelID, table_name_set._data,
								  meshID, elementType, analysisID, stepValue, 
								  return_binary_mesh, return_error_str);
	if ( return_binary_mesh->length() != 0) {
	  std::string save_err_str;
	  _db->saveBoundaryOfAMesh( sessionID, modelID, meshID, elementType, analysisID, stepValue,
				    *return_binary_mesh, &save_err_str);
	}
      }
    }
    // testing and getting the telescope skin meshin Boundary Binary Mesh Format
    // if ( return_binary_mesh->length() != 0) {
    //   BoundaryBinaryMesh *bbm = new BoundaryBinaryMesh;
    //   // bool ok = bbm->fromString( return_binary_mesh->data(), return_binary_mesh->size(), VELaSSCo::BoundaryBinaryMesh::STATIC);
    //   // ok = bbm->toFile( "/tmp/telescope.bbm");
    //   // if ( !ok) {
    //   // 	printf( "Error\n");
    //   // } else {
    //   // 	printf( "Test OK\n");
    //   // }
    //   // bbm->reset();
    //   bool ok = bbm->fromFile( "/tmp/telescope.bbm");
    //   if ( !ok) {
    // 	printf( "Error\n");
    //   } else {
    // 	printf( "Test OK\n");
    //   }
    //   bbm->renumberVerticesIDs( 1);
    //   *return_binary_mesh = bbm->toString();
    //   delete bbm;
    // }
  }
}

// needed by DeleteBoundaryOfAMesh vquery
void DataLayerAccess::deleteStoredBoundaryOfAMesh( const std::string &sessionID,
						   const std::string &modelID,
						   const int meshID, const std::string &elementType,
						   const std::string &analysisID, const double stepValue,
						   std::string *return_error_str) {
  _db->getStoredBoundaryOfAMesh( sessionID, 
				 modelID,
				 meshID, elementType,
				 analysisID, stepValue, 
				 NULL, return_error_str);
  if ( return_error_str->length() == 0) { // i.e. boundary mesh was found
    _db->deleteStoredBoundaryOfAMesh( sessionID,
				      modelID,
				      meshID, elementType,
				      analysisID, stepValue, 
				      return_error_str);
  } else {
    return_error_str->clear(); // not found, // already deleted?
  }
}



void DataLayerAccess::calculateSimplifiedMesh( const std::string &sessionID,
					       const std::string &modelID,
					       const int meshID, const std::string &elementType,
					       const std::string &analysisID, const double stepValue,
					       const std::string &parameters,
					       std::string *return_binary_mesh, std::string *return_error_str) {
  if ( return_binary_mesh) {
    return_binary_mesh->clear();
    _db->getStoredSimplifiedMesh( sessionID, modelID, meshID, elementType, analysisID, stepValue, parameters,
				  return_binary_mesh, return_error_str);
    if ( return_binary_mesh->length() == 0) { // nothing found
      *return_error_str = ""; // reset error string
      HBase::TableModelEntry table_name_set;
      if ( _db->getVELaSSCoTableNames( sessionID, modelID, table_name_set)) {
	AnalyticsModule::getInstance()->calculateSimplifiedMesh( sessionID, modelID, table_name_set._data, 
								 meshID, elementType, analysisID, stepValue, parameters,
								 return_binary_mesh, return_error_str);
	if ( return_binary_mesh->length() != 0) {
	  std::string save_err_str;
	  _db->saveSimplifiedMesh( sessionID, modelID, meshID, elementType, analysisID, stepValue, parameters,
				   *return_binary_mesh, &save_err_str);
	}
      }
    }
  }
}

// needed by DeleteSimplifiedMesh vquery
void DataLayerAccess::deleteStoredSimplifiedMesh( const std::string &sessionID,
						  const std::string &modelID,
						  const int meshID, const std::string &elementType,
						  const std::string &analysisID, const double stepValue,
						  const std::string &parameters,
						  std::string *return_error_str) {
  _db->getStoredSimplifiedMesh( sessionID, modelID, meshID, elementType, analysisID, stepValue, parameters,
				NULL, return_error_str);
  if ( return_error_str->length() == 0) { // i.e. boundary mesh was found
    _db->deleteStoredSimplifiedMesh( sessionID, modelID, meshID, elementType, analysisID, stepValue, parameters,
				     return_error_str);
  } else {
    return_error_str->clear(); // not found, // already deleted?
  }
}


void DataLayerAccess::calculateSimplifiedMeshWithResult( const std::string &sessionID,
							 const std::string &modelID,
							 const int meshID, const std::string &elementType,
							 const std::string &analysisID, const double stepValue,
							 const std::string &parameters,
							 const std::string &resultID,
							 std::string *return_binary_mesh, 
							 std::string *return_binary_results,
							 std::string *return_error_str) {
  if ( return_binary_mesh) {
    return_binary_mesh->clear();
    return_binary_results->clear();
    // _db->getStoredSimplifiedMeshWithResults( sessionID, modelID, meshID, elementType, analysisID, stepValue, parameters, resultID,
    // 					     return_binary_mesh, return_binary_results, return_error_str);
    if ( return_binary_mesh->length() == 0) { // nothing found
      *return_error_str = ""; // reset error string
      HBase::TableModelEntry table_name_set;
      if ( _db->getVELaSSCoTableNames( sessionID, modelID, table_name_set)) {
  	AnalyticsModule::getInstance()->calculateSimplifiedMeshWithResult( sessionID, modelID, table_name_set._data, 
  									   meshID, elementType, analysisID, stepValue, parameters, resultID,
  									   return_binary_mesh, return_binary_results, return_error_str);
  	// Do not store yet the simplified mesh, as it is  not implemented
  	// and to avoid storing garbage
  	// if ( return_binary_mesh->length() != 0) {
  	//   std::string save_err_str;
  	//   _db->saveSimplifiedMesh( sessionID, modelID, meshID, elementType, analysisID, stepValue, parameters,
  	// 			   *return_binary_mesh, &save_err_str);
  	// }
      }
    }
  }
}

// needed by DeleteSimplifiedMeshWithResult vquery
void DataLayerAccess::deleteStoredSimplifiedMeshWithResult( const std::string &sessionID,
							    const std::string &modelID,
							    const int meshID, const std::string &elementType,
							    const std::string &analysisID, const double stepValue,
							    const  std::string &parameters,
							    const std::string &resultID,
							    std::string *return_error_str) {
  // _db->getStoredSimplifiedMeshWithResult( sessionID, modelID, meshID, elementType, analysisID, stepValue, parameters, resultID,
  // 					  NULL, NULL, return_error_str);
  // if ( return_error_str->length() == 0) { // i.e. boundary mesh was found
  //   _db->deleteStoredSimplifiedMeshWithResult( sessionID, modelID, meshID, elementType, analysisID, stepValue, parameters, resultID,
  // 					       return_error_str);
  // } else {
  //   return_error_str->clear(); // not found, // already deleted?
  // }
}


// needed by deleteAllCalculationsForThisModel
void DataLayerAccess::deleteAllStoredCalculationsForThisModel( const std::string &sessionID,
							       const std::string &modelID,
							       std::string *return_error_str) {
  _db->deleteAllStoredCalculationsForThisModel( sessionID, modelID,
						return_error_str);
}


void DataLayerAccess::calculateVolumeLRSplineFromBoundingBox(const std::string& sessionID,
							     const std::string& modelID,
							     const std::string& resultID,
							     const double stepValue,
							     const std::string& analysisID,
							     const double* bBox, // ix doubles: min(x,y,z)-max(x,y,z)
							     const double tolerance, // Use ptr to allow NULL?
							     const int numSteps, // Use ptr to allow NULL?
							     /* out */
							     std::string *return_binary_volume_lrspline,
							     std::string *result_statistics, // JSON format?
							     std::string *return_error_str)
{

  _db->getStoredVolumeLRSpline( sessionID, modelID, resultID, stepValue,
				analysisID, bBox, tolerance, numSteps,
				return_binary_volume_lrspline,
				result_statistics,
				return_error_str);

  if (return_error_str->length() != 0) {
    // This means that the result has not been computed before.
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": Failed to get stored volume lr spline: " << return_error_str);
  }

  if ( return_binary_volume_lrspline->length() == 0) { // nothing found
    *return_error_str = ""; // reset error string

    HBase::TableModelEntry table_name_set;
    //    std::string binary_volumelr = "";
    if ( _db->getVELaSSCoTableNames(sessionID, modelID, table_name_set)) {
      return_error_str->clear();
      //      std::cout << "data table name: " << table_name_set._data << std::endl;
      AnalyticsModule::getInstance()->createVolumeLRSplineFromBoundingBox( sessionID, modelID,
									   table_name_set._data,
									   resultID, stepValue,
									   analysisID, bBox, tolerance,
									   numSteps,
									   return_binary_volume_lrspline,
									   result_statistics,
									   return_error_str);
    } else {
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": Did not enter the Analytics module!");
    }

    bool store_in_hbase = false;
    if (store_in_hbase && (return_binary_volume_lrspline->length() != 0)) {
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": We appear to have received a non-empty binary blob! Success for this function call at least!");
      // We store a config file on the xml format (but stored binary?), which includes:
      // modelID + stepValue + bBox + tolerance + numSteps + resultStatistics.
      std::string save_err_str;
      _db->saveVolumeLRSpline( sessionID, modelID, resultID, stepValue,
			       analysisID, bBox, tolerance, numSteps,
			       *return_binary_volume_lrspline,
			       *result_statistics,
			       &save_err_str);
    } else {
      if (return_binary_volume_lrspline->length() != 0) {
	DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	      ": The binary blob was created but not stored!");
      } else {
	DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	      ": The binary blob was not created!");
      }
    }
  }
}


void DataLayerAccess::deleteVolumeLRSplineFromBoundingBox(const std::string& sessionID,
							     const std::string& modelID,
							     const std::string& resultID,
							     const double stepValue,
							     const std::string& analysisID,
							     const double* bBox, // ix doubles: min(x,y,z)-max(x,y,z)
							     const double tolerance, // Use ptr to allow NULL?
							     const int numSteps, // Use ptr to allow NULL?
							     std::string *return_error_str) {
  _db->getStoredVolumeLRSpline( sessionID, 
				modelID,
				resultID,
				stepValue,
				analysisID,
				bBox, tolerance, numSteps,
				NULL, NULL, return_error_str);
  if ( return_error_str->length() == 0) { // i.e. boundary mesh was found
    _db->deleteStoredVolumeLRSpline( sessionID, 
				     modelID,
				     resultID, stepValue,
				     analysisID, bBox, tolerance, numSteps,
				     return_error_str);
  } else {
    return_error_str->clear(); // not found, // already deleted?
  }
}
