#include <sstream>
#include <stddef.h>  // defines NULL

#include "DataLayerAccess.h"
#include "cJSON.h"
#include "Helpers.h"

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
  DEBUG( "Connecting to Data Layer at " << data_layer_hostname << ":" << data_layer_port);
    socket= boost::shared_ptr<TTransport>(new TSocket( data_layer_hostname, data_layer_port));
    transport = boost::shared_ptr<TTransport>(new TBufferedTransport(socket));
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
}

bool DataLayerAccess::stopConnection()
{
    try
    {
        
        transport->close();
    } catch (TException& tx) {
        cout << "ERROR: " << tx.what() << endl;
    }
    return true;
}

void DataLayerAccess::getStatusDB( std::string& _return) {
    try {
      cli->statusDL( _return);
    } catch (TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
    }
}


void DataLayerAccess::getListOfModels( rvGetListOfModels &_return,
					  const std::string &sessionID, 
					  const std::string &model_group_qualifier, const std::string &model_name_pattern) {
  try {
    cli->GetListOfModelNames( _return, sessionID, model_group_qualifier, model_name_pattern);
  } catch ( TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}

void DataLayerAccess::openModel( rvOpenModel &_return,
				    const std::string &sessionID, 
				    const std::string &unique_name, const std::string &requested_access) {
  try {
    cli->FindModel( _return, sessionID, unique_name, requested_access); // requested_access not used at the momemnt
  } catch ( TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}

void DataLayerAccess::getResultFromVerticesID(std::string& _return,  std::string sessionID,  std::string modelID,  std::string analysisID,  double timeStep,  std::string resultID,  std::string listOfVertices)
{
    try
    {
        
        cli->GetResultFromVerticesID(_return, sessionID, modelID, analysisID, timeStep, resultID, listOfVertices);
        cout << "########## getResultFromVerticesID - " << _return << endl;
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }

}

void DataLayerAccess::getCoordinatesAndElementsFromMesh( std::string& _return, std::string sessionID, std::string modelID, std::string analysisID, double timeStep, int32_t resultID)
{
    try
    {
        cli->GetCoordinatesAndElementsFromMesh( _return, sessionID, modelID, analysisID, timeStep, resultID );
        cout << "########## getMeshDrawData - " << _return << endl;
    }
    catch(TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
}

void DataLayerAccess::getListOfMeshes( rvGetListOfMeshes &_return,
				       const std::string &sessionID,
				       const std::string &modelID,
				       const std::string &analysisID,
				       const double stepValue) {
  try {
    cli->GetListOfMeshes( _return, sessionID, modelID, analysisID, stepValue);
  } catch ( TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}

void DataLayerAccess::getListOfAnalyses( rvGetListOfAnalyses &_return,
					 const std::string &sessionID,
					 const std::string &modelID) {
  try {
    cli->GetListOfAnalyses( _return, sessionID, modelID);
  } catch ( TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
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
    cli->GetListOfTimeSteps( _return, sessionID, modelID, analysisID);
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

void DataLayerAccess::stopAll()
{
    try
    {
        
        cli->stopAll();
        cout << "\t########## stopAll "  << endl;
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
    stopConnection();
    exit(0);
}
