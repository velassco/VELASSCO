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
			     const std::string &analysisID, const int numSteps, const double *lstSteps,
			     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			     double *return_bbox, std::string *return_error_str);

  /*void calculateDiscrete2Continuum(const std::string &sessionID, const std::string &modelID,
				const std::string &analysisName, const std::string &staticMeshID, 
 				const std::string &timestepOptions, const double *timesteps,
				const std::string &CGMethod, const double width, const double cutoffFactor,
			  	const bool processContacts, const bool doTemporalAverage, const std::string &TAOptions,
			  	const std::string &prefixHBaseTableToUse, std:: *returnQueryOutcome, std::string *return_error_str);*/	

private:
  AnalyticsModule() {};
  AnalyticsModule( AnalyticsModule const&) {};
  AnalyticsModule &operator=( AnalyticsModule const&) {};
    
private:
  static AnalyticsModule *m_pInstance;
};

#endif // _ANALYTICS_MODULE_H_
