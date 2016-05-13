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

  void calculateBoundingBox( const std::string &sessionID, const std::string &modelID, const std::string &dataTableName,
			     const std::string &analysisID, const int numSteps, const double *lstStO:\projects\VELaSSCo\SVN_src\modules\EngineLayer\QueryManager\Server_direct_result_queries.cpp
eps,
			     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			     double *return_bbox, std::string *return_error_str);

  void calculateDiscrete2Continuum( const std::string &sessionID, const std::string &modelID,
				    const std::string &analysisName, const std::string &staticMeshID, const std::string &tSOptions, 
				    const int numSteps, const double *lstSteps, const std::string &coarseGrainingMethod, const double width, const double cutoffFactor,
				    const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGoptions, const double deltaT,
				    std::string *returnQueryOutcome, std::string *return_error_str);

  std::string MRgetListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
				    const std::string &sessionID, const std::string &modelID, 
				    const std::string &dataTableName,
				    const std::string &analysisID, const double stepValue, 
				    const int32_t meshID);
  
  void calculateBoundaryOfAMesh( const std::string &sessionID, const std::string &DataLayer_sessionID,
				 const std::string &modelID, const std::string &dataTableName,
				 const int meshID, const std::string &elementType,
				 const std::string &analysisID, const double stepValue,
				 std::string *return_binary_mesh, std::string *return_error_str);

private:
  AnalyticsModule() {};
  AnalyticsModule( AnalyticsModule const&) {};
  AnalyticsModule &operator=( AnalyticsModule const&) { return *this;};
    
private:
  static AnalyticsModule *m_pInstance;
};

#endif // _ANALYTICS_MODULE_H_
