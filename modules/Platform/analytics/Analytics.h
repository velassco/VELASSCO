/* -*- c++ -*- */
#ifndef _ANALYTICS_MODULE_H_
#define _ANALYTICS_MODULE_H_

#include <iostream>
#include <string>

// may be i don't need this:
#include "VELaSSCoSM.h"
using namespace VELaSSCoSM;

// handles connection to Storage Module:
class AnalyticsModule
{
public:
  static AnalyticsModule *getInstance();
  void setHBaseConnection( VELaSSCo::HBase *db) { _hbaseDB = db;}

  void getResultFromVerticesID( const std::string &sessionID, const std::string &modelID, const std::string &dataTableName,
			     const std::string &analysisID, const int numSteps, const double *lstSteps,
			     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			     double *return_bbox, std::string *return_error_str);

  void calculateBoundingBox( const std::string &sessionID, const std::string &modelID, const std::string &dataTableName,
			     const std::string &analysisID, const int numSteps, const double *lstSteps,
			     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			     double *return_bbox, std::string *return_error_str);

  void calculateDiscrete2Continuum( const std::string &sessionID, const std::string &modelID,
				    const std::string &analysisName, const std::string &staticMeshID, const std::string &tSOptions, 
				    const int numSteps, const double *lstSteps, const std::string &coarseGrainingMethod, 
				    const double width, const double cutoffFactor,
				    const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGoptions, 
				    const double deltaT, const bool doSpatialIntegral, const std::string &integralDimension, const std::string &integralDirection,
				    std::string *returnQueryOutcome, std::string *return_error_str);

  std::string MRgetListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
				    const std::string &sessionID, const std::string &modelID, 
				    const std::string &dataTableName,
				    const std::string &analysisID, const double stepValue, 
				    const int32_t meshID);
  
  void calculateBoundaryOfAMesh( const std::string &sessionID,
				 const std::string &modelID, const std::string &dataTableName,
				 const int meshID, const std::string &elementType,
				 const std::string &analysisID, const double stepValue,
				 std::string *return_binary_mesh, std::string *return_error_str);
  
  void calculateSimplifiedMesh( const std::string &sessionID,
				const std::string &modelID, const std::string &dataTableName,
				const int meshID, const std::string &elementType,
				const std::string &analysisID, const double stepValue,
				const std::string &parameters,
				std::string *return_binary_mesh, std::string *return_error_str);

  void calculateIsoSurface(const std::string &sessionID,
			   const std::string &modelID, const std::string &dataTableName,
			   const int meshID, const std::string &analysisID, const double stepValue,
			   const std::string &resultName, const int resultComp, const double isoValue,
			   std::string *return_binary_mesh, std::string *return_error_str);

  void calculateSimplifiedMeshWithResult( const std::string &sessionID,
					  const std::string &modelID, const std::string &dataTableName,
					  const int meshID, const std::string &elementType,
					  const std::string &analysisID, const double stepValue,
					  const std::string &parameters, const std::string &resultName,
					  std::string *return_binary_mesh, std::string *return_result_values,
					  std::string *return_error_str);

  bool checkAndCompletePartitionResults( const std::string &sessionID,
					 const std::string &modelID, const std::string &dataTableName,
					 const int meshID, const std::string &elementType,
					 const std::string &analysisID, const double stepValue,
					 const std::string &parameters, const std::string &resultName,
					 std::string *return_error_str);

  void createVolumeLRSplineFromBoundingBox(const std::string& sessionID,
					   const std::string& modelID,
					   const std::string &dataTableName,
					   const int meshID,
					   const std::string& resultID,
					   const double stepValue,
					   const std::string& analysisID,
					   const double* bBox,
					   const double tolerance,
					   const int numSteps,
					   std::string *return_binary_volume_lrspline,
					   std::string *result_statistics,
					   std::string *return_error_str);

  

private:
  AnalyticsModule(): _hbaseDB( NULL) {};
  AnalyticsModule( AnalyticsModule const&) {};
  AnalyticsModule &operator=( AnalyticsModule const&) { return *this;};
    
private:
  static AnalyticsModule *m_pInstance;
  VELaSSCo::HBase *_hbaseDB;
};

#endif // _ANALYTICS_MODULE_H_
