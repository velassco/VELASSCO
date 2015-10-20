#include <sstream>
#include <stddef.h>  // defines NULL


#include "queryManagerModule.h"
#include "cJSON.h"
#include "Helpers.h"

// Global static pointer used to ensure a single instance of the class.
queryManagerModule* queryManagerModule::m_pInstance = NULL;


queryManagerModule* queryManagerModule::Instance()
{
    // Only allow one instance of class to be generated.
    if (!m_pInstance)
    {
        m_pInstance = new queryManagerModule;
    }
    return m_pInstance;
}


bool queryManagerModule::startConnection( const char *data_layer_hostname, const int data_layer_port)
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

bool queryManagerModule::stopConnection()
{
    try
    {
        
        transport->close();
    } catch (TException& tx) {
        cout << "ERROR: " << tx.what() << endl;
    }
    return true;
}

void queryManagerModule::getStatusDB( std::string& _return) {
    try {
      cli->statusDL( _return);
    } catch (TException& tx) {
      cout << "ERROR: " << tx.what() << endl;
    }
}


void queryManagerModule::getListOfModels( rvGetListOfModels &_return,
					  const std::string &sessionID, 
					  const std::string &model_group_qualifier, const std::string &model_name_pattern) {
  try {
    cli->GetListOfModelNames( _return, sessionID, model_group_qualifier, model_name_pattern);
  } catch ( TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}

void queryManagerModule::openModel( rvOpenModel &_return,
				    const std::string &sessionID, 
				    const std::string &unique_name, const std::string &requested_access) {
  try {
    cli->FindModelFS( _return, sessionID, unique_name); // requested_access not used at the momemnt
    cout << "WARNING: queryManagerModule::openModel requested_access not used at the moment." << endl;
  } catch ( TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}

void queryManagerModule::getResultFromVerticesID(std::string& _return,  std::string sessionID,  std::string modelID,  std::string analysisID,  double timeStep,  std::string resultID,  std::string listOfVertices)
{
    try
    {
        
        cli->GetResultFromVerticesID(_return, sessionID, modelID, analysisID, timeStep, resultID, listOfVertices);
        cout << "########## getStatus - " << _return << endl;
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }

}

void queryManagerModule::stopAll()
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
