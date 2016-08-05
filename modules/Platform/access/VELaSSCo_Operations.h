/* -*- c++ -*- */
#ifndef CommonDataLayer_H
#define CommonDataLayer_H

#include <iostream> // pour std::cout
#include <string>   // pour std::string

using namespace std;

typedef enum {
  DL_SM_DB_UNKNOWN = 0,
  DL_SM_DB_HBASE = 1,
  DL_SM_DB_EDM = 2
} DL_SM_DB_TYPE;

inline const char *getStringFromDBType( DL_SM_DB_TYPE db) {
  const char *ret = "N/A";
  switch( db) {
  case DL_SM_DB_UNKNOWN:  ret = "unknown"; break;
  case DL_SM_DB_HBASE:    ret = "hbase"; break;
  case DL_SM_DB_EDM:      ret = "edm"; break;
  default:                ret = "N/A"; break;
  }
  return ret;
}

inline DL_SM_DB_TYPE getDBTypeFromString( const char *str) {
  DL_SM_DB_TYPE ret = DL_SM_DB_UNKNOWN;
#ifdef WIN32
  if (!_stricmp(str, "hbase")) {
    ret = DL_SM_DB_HBASE;
  } else if (!_stricmp(str, "edm")) {
    ret = DL_SM_DB_EDM;
  }
#else
  if ( !strcasecmp( str, "hbase")) {
    ret = DL_SM_DB_HBASE;
  } else if ( !strcasecmp( str, "edm")) {
    ret = DL_SM_DB_EDM;
  }
#endif
  return ret;
}

#include "VELaSSCoSM.h"

using namespace VELaSSCoSM;

// handles connection to Storage Module:
class VELaSSCo_Operations
{

public:

  //  virtual bool startConnection( const char *data_layer_hostname, const int data_layer_port);
  virtual bool stopConnection() = 0;

  virtual void getStatusDB( std::string& _return) = 0;
  virtual void getResultFromVerticesID( rvGetResultFromVerticesID &_return, 
					const std::string &sessionID, const std::string &modelID, 
					const std::string &analysisID, const double timeStep,
					const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID ) = 0; 
  virtual void getCoordinatesAndElementsFromMesh( rvGetCoordinatesAndElementsFromMesh& _return,
						  const std::string& sessionID, const std::string& modelID, 
						  const std::string& analysisID, const double timeStep, const MeshInfo& meshID ) = 0;
  // session queries
  
  virtual void userLogin( std::string &_return, const std::string &name, const std::string &role, const std::string &password) = 0;
  virtual void userLogout( std::string &_return, const std::string &sessionID) = 0;
    
  virtual void getListOfModels( rvGetListOfModels &_return,
				const std::string &sessionID, 
				const std::string &group_qualifier, const std::string &name_pattern) = 0;
  virtual void openModel( rvOpenModel &_return,
			  const std::string &sessionID, 
			  const std::string &unique_name, const std::string &requested_access) = 0;
  virtual void closeModel( std::string &_return,
			   const std::string &sessionID, 
			   const std::string &modelID) = 0;
  // direct result queries
  virtual void getListOfMeshes( rvGetListOfMeshes &_return,
				const std::string &sessionID,
				const std::string &modelID,
				const std::string &analysisID,
				const double stepValue) = 0;
  virtual void getListOfAnalyses( rvGetListOfAnalyses &_return,
				  const std::string &sessionID,
				  const std::string &modelID) = 0;

  virtual void getListOfTimeSteps( rvGetListOfTimeSteps &_return,
				   const std::string &sessionID,
				   const std::string &modelID,
				   const std::string &analysisID,
				   const std::string &stepOptions, const int numSteps, const double *lstSteps) = 0;

  virtual void getListOfResultsFromTimeStepAndAnalysis( rvGetListOfResults &_return,
							const std::string &sessionID,
							const std::string &modelID,
							const std::string &analysisID,
							const double stepValue) = 0;

  virtual void getListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
					  const std::string &sessionID, const std::string &modelID, 
					  const std::string &analysisID, const double stepValue, 
					  const int32_t meshInfo) = 0;

  //void stopAll();
    
    
    
    
  //THis functions calls to analytics module  
  //INFO: Author Ivan Cores: This are the old funtions called directly from Server_result_analysis_queries.cpp to 
  //the analytics module. Now all queries are called throught the "Access" modules.
  virtual void calculateBoundingBox( const std::string &sessionID, const std::string &modelID,
				     const std::string &analysisID, const int numSteps, const double *lstSteps,
				     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
				     double *return_bbox, std::string *return_error_str) = 0;

  virtual void calculateDiscrete2Continuum( const std::string &sessionID, const std::string &modelID,
					    const std::string &analysisName, const std::string &staticMeshID, const std::string &tSOptions, 
					    const int numSteps, const double *lstSteps, const std::string &coarseGrainingMethod, 
					    const double width, const double cutoffFactor,
					    const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGoptions, 
					    const double deltaT,
					    std::string *returnQueryOutcome, std::string *return_error_str) = 0;

  // not used anymore ...
  // virtual std::string MRgetListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
  // 						   const std::string &sessionID, const std::string &modelID, 
  // 						   const std::string &dataTableName,
  // 						   const std::string &analysisID, const double stepValue, 
  // 						   const int32_t meshID) = 0;
  
  virtual void calculateBoundaryOfAMesh( const std::string &sessionID,
					 const std::string &modelID,
					 const int meshID, const std::string &elementType,
					 const std::string &analysisID, const double stepValue,
					 std::string *return_binary_mesh, std::string *return_error_str) = 0;
  // Not needed at this level
  // virtual void getStoredBoundaryOfAMesh( const std::string &sessionID,
  // 					 const std::string &modelID,
  // 					 const int meshID, const std::string &elementType,
  // 					 const std::string &analysisID, const double stepValue,
  // 					 std::string *return_binary_mesh, std::string *return_error_str) = 0;
  // virtual void deleteStoredBoundaryOfAMesh( const std::string &sessionID,
  // 					    const std::string &modelID,
  // 					    const int meshID, const std::string &elementType,
  // 					    const std::string &analysisID, const double stepValue,
  // 					    std::string *return_error_str) = 0;

};

#endif
