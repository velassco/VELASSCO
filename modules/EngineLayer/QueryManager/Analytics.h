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

  void calculateDiscrete2Continuum( const std::string &sessionID, const std::string &modelID,
				    const std::string &analysisID, const std::string &staticMeshID, 
				    const std::string &stepOptions, const int numSteps, const double *lstSteps, /* only lstSteps is in the operation form */
				    const std::string &CoarseGrainingMethod, const double width, const double cutoffFactor,
				    const bool processContacts, const bool doTemporalAVG, const std::string &TemporalAVGoptions,
				    const std::string &prefixHBaseTableToUse, /* now it's a prefix, not a hbase table name !!! */
				    std::string *returnQueryOutcome, /* also not in the form, as the prefixHBaseTableToUse */
				    std::string *return_error_str);

private:
  AnalyticsModule() {};
  AnalyticsModule( AnalyticsModule const&) {};
  AnalyticsModule &operator=( AnalyticsModule const&) { return *this;};
    
private:
  static AnalyticsModule *m_pInstance;
};

#endif // _ANALYTICS_MODULE_H_
