/* -*- c++ -*- */
#ifndef DataLayerAccess_H
#define DataLayerAccess_H

#include <iostream> // pour std::cout
#include <string>   // pour std::string


#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

// no need for AbstractDB.h anymore ...
// #include "AbstractDB.h"
#include "HBase.h"
#include "base64.h"
#include "Analytics.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;
using namespace std;


//THIS COMMENTED CODE IS NOW LOCATED IN VELaSSCo_Operations.h
/*
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
  if ( !strcasecmp( str, "hbase")) {
  ret = DL_SM_DB_HBASE;
  } else if ( !strcasecmp( str, "edm")) {
  ret = DL_SM_DB_EDM;
  }
  return ret;
  }
*/

#include "VELaSSCo_Operations.h"
#include "VELaSSCoSM.h"

using namespace VELaSSCoSM;

// handles connection to Storage Module:
class DataLayerAccess : public VELaSSCo_Operations
{
public:
  static DataLayerAccess *Instance();
  bool startConnection( const char *data_layer_hostname, const int data_layer_port);
  bool stopConnection();
  
  void getStatusDB( std::string& _return);
  void getResultFromVerticesID( rvGetResultFromVerticesID &_return, 
				const std::string &sessionID, const std::string &modelID, 
				const std::string &analysisID, const double timeStep,
				const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID ); 
  void getCoordinatesAndElementsFromMesh( rvGetCoordinatesAndElementsFromMesh& _return,
					  const std::string& sessionID, const std::string& modelID, 
					  const std::string& analysisID, const double timeStep, const MeshInfo& meshID ) ;
  // session queries
  
  void userLogin( std::string &_return, const std::string &name, const std::string &role, const std::string &password);
  void userLogout( std::string &_return, const std::string &sessionID);
    
  void getListOfModels( rvGetListOfModels &_return,
			const std::string &sessionID, 
			const std::string &group_qualifier, const std::string &name_pattern);
  void openModel( rvOpenModel &_return,
		  const std::string &sessionID, 
		  const std::string &unique_name, const std::string &requested_access);
  void closeModel( std::string &_return,
		   const std::string &sessionID, 
		   const std::string &modelID);
  // direct result queries
  void getListOfMeshes( rvGetListOfMeshes &_return,
			const std::string &sessionID,
			const std::string &modelID,
			const std::string &analysisID,
			const double stepValue);
  void getListOfAnalyses( rvGetListOfAnalyses &_return,
			  const std::string &sessionID,
			  const std::string &modelID);
  /* as of OP-22.113 */
  void getListOfTimeSteps( rvGetListOfTimeSteps &_return,
			   const std::string &sessionID,
			   const std::string &modelID,
			   const std::string &analysisID,
			   const std::string &stepOptions, const int numSteps, const double *lstSteps);
  /* as of OP-22.115 */
  void getListOfResultsFromTimeStepAndAnalysis( rvGetListOfResults &_return,
						const std::string &sessionID,
						const std::string &modelID,
						const std::string &analysisID,
						const double stepValue);

  bool getResultInfoFromResultName( const std::string &sessionID, const std::string &modelID, 
				    const std::string &analysisID, const double stepValue, 
				    const std::string &resultName, ResultInfo &outResultInfo);

  /* as of OP-22.116 */
  void getListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
				  const std::string &sessionID, const std::string &modelID, 
				  const std::string &analysisID, const double stepValue, 
				  const int32_t meshID);
  void getListOfSelectedVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
					  const std::string &sessionID, const std::string &modelID, 
					  const std::string &analysisID, const double stepValue, 
					  const int32_t meshID, const std::vector<int64_t> &listOfVerticesID);

  void stopAll();
    
    
    
    
  //THis functions calls to analytics module  
  //INFO: Author Ivan Cores: This are the old funtions called directly from Server_result_analysis_queries.cpp to 
  //the analytics module. Now all queries are called throught the "Access" modules.
  void calculateBoundingBox( const std::string &sessionID, const std::string &modelID,
			     const std::string &analysisID, const int numSteps, const double *lstSteps,
			     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			     double *return_bbox, std::string *return_error_str);
  // needed by DeleteBoundingBox vquery
  void deleteStoredBoundingBox( const std::string &sessionID, const std::string &modelID, 
				const std::string &analysisID, const int numSteps, const double *lstSteps,
				const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
				std::string *return_error_str);

  void calculateDiscrete2Continuum( const std::string &sessionID, const std::string &modelID,
				    const std::string &analysisName, const std::string &staticMeshID, const std::string &tSOptions, 
				    const int numSteps, const double *lstSteps, const std::string &coarseGrainingMethod, 
				    const double width, const double cutoffFactor,
				    const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGoptions, 
				    const double deltaT, const bool doSpatialIntegral, const std::string &integralDimension, const std::string &integralDirection,
				    std::string *returnQueryOutcome, std::string *return_error_str);
  
  void calculateBoundaryOfAMesh( const std::string &sessionID,
				 const std::string &modelID,
				 const int meshID, const std::string &elementType,
				 const std::string &analysisID, const double stepValue,
				 std::string *return_binary_mesh, std::string *return_error_str);
  // needed by DeleteBoundaryOfAMesh vquery
  void deleteStoredBoundaryOfAMesh( const std::string &sessionID,
  				    const std::string &modelID,
  				    const int meshID, const std::string &elementType,
  				    const std::string &analysisID, const double stepValue,
  				    std::string *return_error_str);
  
  void calculateIsoSurface( const std::string &sessionID,
			    const std::string &modelID,
			    const int meshID,
			    const std::string &analysisID, const double stepValue,
			    const std::string &resultName, const int resultComp, const double isoValue,
			    std::string *return_binary_mesh, std::string *return_error_str);
  
  void calculateSimplifiedMesh( const std::string &sessionID,
				const std::string &modelID,
				const int meshID, const std::string &elementType,
				const std::string &analysisID, const double stepValue,
				const  std::string &parameters,
				std::string *return_binary_mesh, std::string *return_error_str);
  // needed by DeleteSimplifiedMesh vquery
  void deleteStoredSimplifiedMesh( const std::string &sessionID,
				   const std::string &modelID,
				   const int meshID, const std::string &elementType,
				   const std::string &analysisID, const double stepValue,
				   const  std::string &parameters,
				   std::string *return_error_str);

  void calculateSimplifiedMeshWithResult( const std::string &sessionID,
					  const std::string &modelID,
					  const int meshID, const std::string &elementType,
					  const std::string &analysisID, const double stepValue,
					  const std::string &parameters,
					  const std::string &resultID,
					  std::string *return_binary_mesh, 
					  std::string *return_binary_results,
					  std::string *return_error_str);
  // needed by DeleteSimplifiedMeshWithResult vquery
  void deleteStoredSimplifiedMeshWithResult( const std::string &sessionID,
					     const std::string &modelID,
					     const int meshID, const std::string &elementType,
					     const std::string &analysisID, const double stepValue,
					     const  std::string &parameters,
					     const std::string &resultID,
					     std::string *return_error_str);

  // needed by deleteAllCalculationsForThisModel
  void deleteAllStoredCalculationsForThisModel( const std::string &sessionID,
						const std::string &modelID,
						std::string *return_error_str);

  // This function calls to Analytics module.
  void calculateVolumeLRSplineFromBoundingBox(const std::string& sessionID,
					      const std::string& modelID,
					      const int meshID,
					      const std::string& resultID,
					      const double stepValue,
					      const std::string& analysisID,
					      const double* bBox, // ix doubles: min(x,y,z)-max(x,y,z)
					      double tolerance, // Use ptr to allow NULL?
					      int numSteps, // Use ptr to allow NULL?
					   /* out */
					      std::string *return_volume_lrspline,
					      std::string *result_statistics, // JSON format?
					      std::string *return_error_str);
  void deleteVolumeLRSplineFromBoundingBox(const std::string& sessionID,
					   const std::string& modelID,
					   const int meshID,
					   const std::string& resultID,
					   const double stepValue,
					   const std::string& analysisID,
					   const double* bBox,
					   const double tolerance,
					   const int numSteps,
					   std::string *return_error_str);
private:
  // DataLayerAccess(){};
  DataLayerAccess(DataLayerAccess const&){};
  // DataLayerAccess& operator=(DataLayerAccess const&){};

  DataLayerAccess(): _db( NULL) {};
  ~DataLayerAccess() { stopConnection();}    
  VELaSSCo::HBase *_db;
  
  static DataLayerAccess *m_pInstance;
  VELaSSCoSMClient *cli;
    
    
  boost::shared_ptr<TTransport> socket;
  boost::shared_ptr<TTransport> transport;
  boost::shared_ptr<TProtocol> protocol;
};

#endif
