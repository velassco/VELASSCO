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

  void calculateBoundingBox( const std::string &sessionID, const std::string &modelID,
			     const std::string &analysisID, const int numSteps, const double *lstSteps,
			     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			     double *return_bbox, std::string *return_error_str);

private:
  AnalyticsModule() {};
  AnalyticsModule( AnalyticsModule const&) {};
  AnalyticsModule &operator=( AnalyticsModule const&) {};
    
private:
  static AnalyticsModule *m_pInstance;
};

#endif // _ANALYTICS_MODULE_H_
