#include <sstream>
#include <stddef.h>  // defines NULL


#include "queryManagerModule.h"
#include "cJSON.h"

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


bool queryManagerModule::startConnection()
{
    socket= boost::shared_ptr<TTransport>(new TSocket("localhost", 26266));
    transport = boost::shared_ptr<TTransport>(new TBufferedTransport(socket));
    protocol = boost::shared_ptr<TProtocol>(new TBinaryProtocol(transport));
    cli = new VELaSSCoClient(protocol);
    try
    {
        
        transport->open();
        getStatus();
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

void queryManagerModule::getStatus()
{
    try
    {
        
        string _return;
        cli->statusDL(_return);
        cout << "\t########## getStatus - " << _return << endl;
    }
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
}


void queryManagerModule::getResultFormVerticesID(std::string _return,  std::string sessionID,  std::string modelID,  std::string analysisID,  double timeStep,  std::string resultID,  std::string listOfVertices)
{
    try
    {
        
        cli->GetResultFormVerticesID(_return, sessionID, modelID, analysisID, timeStep, resultID, listOfVertices);
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