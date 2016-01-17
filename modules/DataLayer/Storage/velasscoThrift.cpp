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

void VELaSSCoHandler::FindModel( rvOpenModel &_return, const std::string &sessionID, 
				 const std::string &unique_model_name_pattern, const std::string &requested_access) {
  std::string report;
  std::string modelID;
  FullyQualifiedModelName model_info;
  std::string status = storageModule::Instance()->findModel( report, modelID, model_info,
							     sessionID, unique_model_name_pattern, requested_access);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_modelID( modelID);
  _return.__set_model_info( model_info);
}

void VELaSSCoHandler::GetResultFromVerticesID( rvGetResultFromVerticesID &_return, 
				  const std::string &sessionID, const std::string &modelID, 
				  const std::string &analysisID, const double timeStep,
				  const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID )
{
  std::string report;
  std::vector< ResultOnVertex > listOfResults;
  std::string status  = storageModule::Instance()->getResultFromVerticesID (report, listOfResults, sessionID, modelID, analysisID, timeStep, resultID, listOfVerticesID );
  _return.__set_status( status );
  _return.__set_report( report );
  _return.__set_result_list( listOfResults );
}
 
void VELaSSCoHandler::GetCoordinatesAndElementsFromMesh(rvGetCoordinatesAndElementsFromMesh& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const int32_t meshID)
{
	std::string                  report;
	MeshInfo                     meshInfo;
	std::vector< Vertex        > listOfVertices;
	std::vector< Element       > listOfElements;
	std::vector< ElementAttrib > listOfElementAttribs;
	std::vector< ElementGroup  > listOfElementGroupInfo;
	std::string status = 
	  storageModule::Instance()->getCoordinatesAndElementsFromMesh (
		report, meshInfo, listOfVertices, listOfElements, listOfElementAttribs, listOfElementGroupInfo,
	    sessionID, modelID, analysisID, timeStep, meshID
	  );
	_return.__set_status( status );
	_return.__set_report( report );
	_return.__set_meshInfo( meshInfo );
	_return.__set_vertex_list ( listOfVertices );
	_return.__set_element_list( listOfElements );
	_return.__set_element_attrib_list( listOfElementAttribs );
	_return.__set_element_group_info_list( listOfElementGroupInfo );		
}

void VELaSSCoHandler::GetListOfMeshes( rvGetListOfMeshes &_return, 
				       const std::string &sessionID, const std::string &modelID, 
				       const std::string &analysisID, const double stepValue) {
  std::string report;
  std::vector< MeshInfo>listOfMeshes;
  std::string status = storageModule::Instance()->getListOfMeshes( report, listOfMeshes,
								   sessionID, modelID, analysisID, stepValue);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_meshInfos( listOfMeshes);
}

void VELaSSCoHandler::GetListOfAnalyses( rvGetListOfAnalyses &_return, 
					 const std::string &sessionID, const std::string &modelID) {
  std::string report;
  std::vector< std::string>listOfAnalyses;
  std::string status = storageModule::Instance()->getListOfAnalyses( report, listOfAnalyses, sessionID, modelID);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_analyses( listOfAnalyses);
}

void VELaSSCoHandler::GetListOfTimeSteps( rvGetListOfTimeSteps &_return, 
					  const std::string &sessionID, const std::string &modelID, 
					  const std::string &analysisID) {
  std::string report;
  std::vector< double>listOfSteps;
  std::string status = storageModule::Instance()->getListOfSteps( report, listOfSteps, sessionID, modelID, analysisID);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_time_steps( listOfSteps);
}

void VELaSSCoHandler::GetListOfResultsFromTimeStepAndAnalysis( rvGetListOfResults &_return, 
							       const std::string &sessionID, const std::string &modelID, 
							       const std::string &analysisID, const double stepValue) {
  std::string report;
  std::vector< ResultInfo>listOfResults;
  std::string status = storageModule::Instance()->getListOfResults( report, listOfResults,
								    sessionID, modelID, analysisID, stepValue);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_result_list( listOfResults);
}

void VELaSSCoHandler::GetListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
						 const std::string &sessionID, const std::string &modelID, 
						 const std::string &analysisID, const double stepValue, 
						 const int32_t meshID) {
  std::string report;
  std::vector< Vertex> listOfVertices;
  std::string status = storageModule::Instance()->getListOfVerticesFromMesh( report, listOfVertices,
									     sessionID, modelID, 
									     analysisID, stepValue,
									     meshID);
  _return.__set_status( status);
  _return.__set_report( report);
  _return.__set_vertex_list( listOfVertices);
  
}

void VELaSSCoHandler::stopAll() {
  storageModule *sm = storageModule::Instance();
  sm->stopConnection();
  exit(0);
}
