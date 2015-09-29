#include "velasscoThrift.h"
#include "storageModule.h"
#include "Helpers.h"

VELaSSCoHandler::VELaSSCoHandler()
{
    
}

void VELaSSCoHandler::statusDL(std::string& _return)
{
    _return = "statusDL \n";
    // printf("%s", _return.c_str());
    _return = storageModule::Instance()->getStatusDB();
    DEBUG( _return);
}

void VELaSSCoHandler::GetResultFormVerticesID(std::string& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::string& listOfVertices)
{
    
    _return = storageModule::Instance()->getResultOnVertices (sessionID, modelID, analysisID, timeStep, resultID, listOfVertices);
    // _return = storageModule::Instance()->checkIfAllVerticesArePresent(listOfVertices, _return);
}

void VELaSSCoHandler::stopAll()
{
    exit(0);
}
