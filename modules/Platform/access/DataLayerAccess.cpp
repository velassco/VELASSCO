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
	_db = (AbstractDB*)  new HBase();
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
    cout << "ERROR: " << tx.what() << endl;
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
					    const std::string &dataTableName,
					    const std::string &analysisID, const int numSteps, const double *lstSteps,
					    const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
					    double *return_bbox, std::string *return_error_str) {
	    
    AnalyticsModule::getInstance()->calculateBoundingBox( sessionID, modelID,
							  dataTableName,
							  analysisID, numSteps, lstSteps,
							  numVertexIDs, lstVertexIDs,
							  return_bbox, return_error_str);					    
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



std::string DataLayerAccess::MRgetListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &return_data, 
						   const std::string &sessionID, const std::string &modelID, 
						   const std::string &dataTableName,
						   const std::string &analysisID, const double stepValue, 
						   const int32_t meshID) {
						   
	return AnalyticsModule::getInstance()->MRgetListOfVerticesFromMesh(return_data,
							sessionID, modelID,
							dataTableName,
							analysisID, stepValue,
							meshID);
}



void DataLayerAccess::calculateBoundaryOfAMesh( const std::string &sessionID, const std::string &DataLayer_sessionID,
						const std::string &modelID, const std::string &dataTableName,
						const int meshID, const std::string &elementType,
						const std::string &analysisID, const double stepValue,
						std::string *return_binary_mesh, std::string *return_error_str) {
									
	AnalyticsModule::getInstance()->calculateBoundaryOfAMesh( sessionID, 
								DataLayer_sessionID,
								modelID,
								dataTableName,
								meshID, elementType,
								analysisID, stepValue, 
								return_binary_mesh, return_error_str);										
}






