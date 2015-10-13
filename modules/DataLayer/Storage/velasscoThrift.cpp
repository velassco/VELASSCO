#include "velasscoThrift.h"
#include "storageModule.h"
#include "Helpers.h"

VELaSSCoHandler::VELaSSCoHandler( const DL_SM_DB_TYPE db_type, const char *db_host, const int db_port) {
  storageModule *sm = storageModule::Instance();
  sm->startConnection( db_type, db_host, db_port);
}

VELaSSCoHandler::~VELaSSCoHandler() {
  storageModule *sm = storageModule::Instance();
  sm->stopConnection();
}

void VELaSSCoHandler::statusDL(std::string &_return)
{
  // _return = "statusDL \n";
    // printf("%s", _return.c_str());
  storageModule *sm = storageModule::Instance();
    _return = sm->getStatusDB();
    DEBUG( _return);
}

void VELaSSCoHandler::GetListOfModelNames(rvGetListOfModels &_return, const std::string &sessionID, const std::string &model_group_qualifier, const std::string &model_name_pattern) {
  std::string report;
  std::vector< FullyQualifiedModelName>listOfModelNames;
  std::string status = storageModule::Instance()->getListOfModelNames( report, listOfModelNames, 
								       sessionID, model_group_qualifier, model_name_pattern);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_models( listOfModelNames);
}

void VELaSSCoHandler::FindModelFS(rvOpenModel &_return, const std::string &sessionID, const std::string &unique_model_name_pattern) {
  std::string report;
  std::string modelID;
  std::vector< FullyQualifiedModelName>listOfModelNames;
  std::string status = storageModule::Instance()->findModelFS( report, modelID, sessionID, unique_model_name_pattern);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_modelID( modelID);
}

void VELaSSCoHandler::GetResultFromVerticesID(std::string &_return, const std::string &sessionID, const std::string &modelID, const std::string &analysisID, const double timeStep, const std::string &resultID, const std::string &listOfVertices)
{
    
    _return = storageModule::Instance()->getResultOnVertices (sessionID, modelID, analysisID, timeStep, resultID, listOfVertices);
    // _return = storageModule::Instance()->checkIfAllVerticesArePresent(listOfVertices, _return);
}

void VELaSSCoHandler::stopAll() {
  storageModule *sm = storageModule::Instance();
  sm->stopConnection();
  exit(0);
}
