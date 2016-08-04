#include <stdio.h>

// CLib
#include <cmath>
#include <cstddef>

// STD
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

// #include <unordered_set>
// needs an ordered set:
#include <set>

// Curl
#include <curl/curl.h>

// VELaSSCo
#include "HBase.h"
//#include "VELaSSCoSM.h" // for class FullyQualifiedModelName
#include "cJSON.h"
#include "base64.h"

#include "Curl_cmd.h"
#include "Helpers.h"
#include "Extras.h"
#include "Crono.h"

#define WITH_PARSE_COMPONENTS

#ifdef WITH_PARSE_COMPONENTS
#include "BinarySerialization.h"
#endif

using namespace std;
using namespace VELaSSCo;

/////////////////////////////////
// Curl + Json
/////////////////////////////////
#include <curl/curl.h>
#include "cJSON.h"

// retrieve only the 'Q' column family of the Simulations_VQuery_Results_Data table
bool HBase::getStoredVQueryExtraData( const std::string &sessionID,
				      const std::string &modelID,
				      const std::string &analysisID, const double stepValue,
				      const std::string &vqueryName, const std::string &vqueryParameters,
				      std::vector< std::string> *lst_vquery_results) { // list = 1 entry per partition
  TableModelEntry table_set;
  bool found = getTableNames( sessionID, modelID, table_set);
  if ( !found) {
    return false;
  }
  return false;
}

void HBase::getStoredBoundaryOfAMesh( const std::string &sessionID,
				      const std::string &modelID,
				      const int meshID, const std::string &elementType,
				      const std::string &analysisID, const double stepValue,
				      std::string *return_binary_mesh, std::string *return_error_str) {
  std::string vqueryName = "GetBoundaryMesh";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" << analysisID << "\" " << stepValue;
  std::vector< std::string> lst_boundaries; // should only be one !!!
  bool found = getStoredVQueryExtraData( sessionID, modelID, analysisID, stepValue,
					 vqueryName, vqueryParametersStream.str(),
					 &lst_boundaries);
  if ( !found) {
    return_error_str = new std::string( "Stored boundary mesh not found.");
    return;
  }
  if ( found) {
  }
  return_error_str = new std::string( "Stored boundary mesh not found.");
}

void HBase::deleteStoredBoundaryOfAMesh( const std::string &sessionID,
					 const std::string &modelID,
					 const int meshID, const std::string &elementType,
					 const std::string &analysisID, const double stepValue,
					 std::string *return_error_str) {
  std::string vqueryName = "GetBoundaryMesh";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" << analysisID << "\" " << stepValue;
  bool found = getStoredVQueryExtraData( sessionID, modelID, analysisID, stepValue,
					 vqueryName, vqueryParametersStream.str(), 
					 NULL); // we don't want the data just to check if it's there
  if ( !found) {
    return_error_str = new std::string( "Stored boundary mesh not found.");
    return;
  }
  return_error_str = new std::string( "Stored boundary mesh not found.");
}
