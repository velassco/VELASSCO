#include <sstream>
#include <stddef.h>  // defines NULL
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "Helpers.h"
#include "Analytics.h"

// Global static pointer used to ensure a single instance of the class.
AnalyticsModule *AnalyticsModule::m_pInstance = NULL;

AnalyticsModule *AnalyticsModule::getInstance() {
  // Only allow one instance of class to be generated.
  std::cout << "in AnalyticsModule::getInstance" << std::endl;
  if ( !m_pInstance) {
    m_pInstance = new AnalyticsModule;
  }
  return m_pInstance;
}

void AnalyticsModule::calculateBoundingBox( const std::string &sessionID, const std::string &modelID,
					    const std::string &analysisID, const int numSteps, const double *lstSteps,
					    const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
					    double *return_bbox, std::string *return_error_str) {
  // return_bbox[ 0] = -0.7;
  // return_bbox[ 1] = -0.7;
  // return_bbox[ 2] = -0.7;
  // return_bbox[ 3] =  0.7;
  // return_bbox[ 4] =  0.7;
  // return_bbox[ 5] =  0.7;
  // *return_error_str = "Ok";
  // at the moment only CLI interface:

  // running java:
  char cmd_line[ 1024];
  sprintf( cmd_line, "java -jar ../Analytics/GetBoundingBoxOfAModel.jar %s", modelID.c_str());
  int ret = system( cmd_line);
  // output in '../bbox/part-r-00000'
  FILE *fi = fopen( "bbox/part-r-00000", "r");
  if ( !fi) {
    *return_error_str = std::string( "Problems executing ") + __FUNCTION__ + std::string( " job.");
    return;
  }
  char keyword[ 1024];
  double value = 0.0;
  int num_values = 0;
  for ( int i = 0; i < 6; i++) {
    int n = fscanf( fi, "%s %lf", keyword, &value);
    if ( n == 2) {
      if (      !strcasecmp( keyword, "min_x")) { return_bbox[ 0] = value; num_values++; }
      else if ( !strcasecmp( keyword, "min_y")) { return_bbox[ 1] = value; num_values++; }
      else if ( !strcasecmp( keyword, "min_z")) { return_bbox[ 2] = value; num_values++; }
      else if ( !strcasecmp( keyword, "max_x")) { return_bbox[ 3] = value; num_values++; }
      else if ( !strcasecmp( keyword, "max_y")) { return_bbox[ 4] = value; num_values++; }
      else if ( !strcasecmp( keyword, "max_z")) { return_bbox[ 5] = value; num_values++; }
    }
  }
  fclose( fi);
  if ( num_values != 6) {
    *return_error_str = std::string( "Problems with ") + FUNCTION_NAME + std::string( " results.");
  }
}
