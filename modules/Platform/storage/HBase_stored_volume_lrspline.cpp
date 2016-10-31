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

#include "HBase_stored_common.h"


void HBase::getStoredVolumeLRSpline( const std::string& sessionID,
				     const std::string& modelID,
				     const int meshID,
				     const std::string& resultID,
				     const double stepValue,
				     const std::string& analysisID,
				     const double* bBox,
				     const double tolerance,
				     const int numSteps,
				     std::string *binary_volume_lrspline,
				     std::string *result_statistics,
				     std::string *return_error_str) {

  std::string vqueryName = "GetVolumeLRSpline";
  std::stringstream vqueryParametersStream;
  // Highly sensitive to numerical precision for the bounding box ...
  vqueryParametersStream << modelID << " " << meshID << " " << resultID << " " << stepValue <<
    " \"" << analysisID << "\" " << bBox[0] << " " << bBox[1] << " " << bBox[2] << " " << 
    bBox[3] << " " << bBox[4] << " " << bBox[5] << " " << tolerance << " " << numSteps;
  const std::string &vqueryParameters = vqueryParametersStream.str();
  std::vector< std::string> lst_lrsplines; // should only be one !!!

  //  double stepValue = 0.0; // Not used, different steps is encapsulated in the vqueryParameters.
  // @@sbr201609 Why not use getStoredVQueryExtraDataSplitted()? Use stepValue == 0.0?
  bool scan_ok = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 0,
						   vqueryName, vqueryParameters,
						   &lst_lrsplines);
  if ( !scan_ok) {
    *return_error_str = std::string( "Stored volume LRSpline not found.");
    return;
  }
  if ( scan_ok && ( lst_lrsplines.size() > 0)) {
    if ( binary_volume_lrspline) {
      *binary_volume_lrspline = lst_lrsplines[0];
      // @@sbr201609 Not setting the statistics! Not included yet.
    }
    // *return_error_str = std::string( "Ok");
  } else {
    *return_error_str = std::string( "Stored Volume LRSpline not found.");
  }

 return;
}


bool HBase::deleteStoredVolumeLRSpline( const std::string& sessionID,
					const std::string& modelID,
					const int meshID,
					const std::string& resultID,
					const double stepValue,
					const std::string& analysisID,
					const double* bBox,
					const double tolerance,
					const int numSteps,
					std::string *return_error_str) {

  //  DEBUG("SINTEF: " << __FILE__ << __FUNCTION__ << ": Under construction!");

  std::string vqueryName = "GetVolumeLRSpline";
  std::stringstream vqueryParametersStream;
  // Highly sensitive to numerical precision for the bounding box ...
  vqueryParametersStream << modelID << " " << meshID << " " << resultID << " " << stepValue <<
    " \"" << analysisID << "\" " << bBox[0] << " " << bBox[1] << " " << bBox[2] << " " << 
    bBox[3] << " " << bBox[4] << " " << bBox[5] << " " << tolerance << " " << numSteps;
  const std::string &vqueryParameters = vqueryParametersStream.str();
  // VolumeLRBspline stored in partitionID = 0
  bool found = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 0,
						 vqueryName, vqueryParameters, 
						 NULL); // we don't want the data just to check if it's there
  if ( !found) {
    // already deleted?
    // *return_error_str = std::string( "Stored boundary mesh not found.");
    return_error_str->clear();
    return true; // not/never stored
  }

  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false; // no information about tables for this user and this session
  }

  LOGGER_SM << "DELETING stored volume lrspline for '" << vqueryParameters << "'" << std::endl;
  // reset tables:
  bool reset_tables = false;
  if ( reset_tables) {
    if ( checkIfTableExists( table_set._stored_vquery_metadata)) {
      _hbase_client->disableTable(  table_set._stored_vquery_metadata);
      _hbase_client->deleteTable(  table_set._stored_vquery_metadata);
    }
    if ( checkIfTableExists( table_set._stored_vquery_data)) {
      _hbase_client->disableTable(  table_set._stored_vquery_data);
      _hbase_client->deleteTable(  table_set._stored_vquery_data);
    }
  }

  // delete the row in the metadata table
  if ( !checkIfTableExists( table_set._stored_vquery_metadata)) {
    return true; // not created yet? i.e. already deleted...
  }

  std::string tableName = table_set._stored_vquery_metadata;
  std::string metadataRowKey = createStoredMetadataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters);
  bool ok = deleteStoredRow( tableName, metadataRowKey, "deleting stored Metadata row");

  // now delete the row in the data table
  if ( !checkIfTableExists( table_set._stored_vquery_data)) {
    return true; // not created yet? i.e. already deleted...
  }
  tableName = table_set._stored_vquery_data;
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0); // only one volume lrspline, in partition 0
  ok = deleteStoredRow( tableName, dataRowKey, "deleting stored Data row");
  return ok;

}


bool HBase::saveVolumeLRSpline( const std::string& sessionID,
				const std::string& modelID,
				const int meshID,
				const std::string& resultID,
				const double stepValue,
				const std::string& analysisID,
				const double* bBox,
				const double tolerance,
				const int numSteps,
				const std::string &binary_volume_lrspline,
				const std::string &result_statistics,
				std::string *return_error_str) {

  TableModelEntry table_set;
  // We search through the map _table_models for the table_set corresponding to 'sessionID + modelID'.
  // We should store our info in the 2 query tables:
  // - _stored_vquery_metadata: For storing the parameters passed to the query.
  // - _stored_vquery_data: For storing the binary result.
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false;
  }

  // @@sbr201609 Storing both binary_volume_lrspline and stats in the same row? Or use a query subname
  // like "GetVolumeLRSpline_statistics"?
  // @@sbr201609 Should we use another syntax for the bBox array?
  std::string vqueryName = "GetVolumeLRSpline";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << resultID << " " << stepValue <<
    " \"" << analysisID << "\" " << bBox[0] << " " << bBox[1] << " " << bBox[2] << " " << 
    bBox[3] << " " << bBox[4] << " " << bBox[5] << " " << tolerance << " " << numSteps;
  const std::string &vqueryParameters = vqueryParametersStream.str();

  LOGGER_SM << "Saving volume lrspline for '" << vqueryParameters << "'" << std::endl;
  // create tables if needed
  bool ok = false;
  // reset tables:
  bool reset_tables = false;
  if ( reset_tables) {
    if ( checkIfTableExists( table_set._stored_vquery_metadata)) {
      _hbase_client->disableTable(  table_set._stored_vquery_metadata);
      _hbase_client->deleteTable(  table_set._stored_vquery_metadata);
    }
    if ( checkIfTableExists( table_set._stored_vquery_data)) {
      _hbase_client->disableTable(  table_set._stored_vquery_data);
      _hbase_client->deleteTable(  table_set._stored_vquery_data);
    }
  }
  if ( !checkIfTableExists( table_set._stored_vquery_metadata)) {
    ok = createStoredMetadataTable( table_set._stored_vquery_metadata);
    if ( !ok) return false;
  }
  if ( !checkIfTableExists( table_set._stored_vquery_data)) {
    // create data table
    ok = createStoredMetadataTable( table_set._stored_vquery_data);
    if ( !ok) return false;
  }

  ok = storeQueryInfoInMetadataTable( sessionID, modelID, analysisID, stepValue, vqueryName, vqueryParameters);
  if (!ok) {
    return false;
  }

  // binary mesh stored in partitionID = 0
  ok = storeQueryDataInDataTable( sessionID, modelID, analysisID, stepValue, 0, vqueryName,
				  vqueryParameters, binary_volume_lrspline);

  return ok; // guess the rows are ok...  
}

bool HBase::alreadyStoredVolumeLRSpline( const std::string& sessionID,
					 const std::string& modelID,
					 const int meshID,
					 const std::string& resultID,
					 const double stepValue,
					 const std::string& analysisID,
					 const double* bBox,
					 const double tolerance,
					 const int numSteps,
					 std::string *return_error_str) {

  
  std::string vqueryName = "GetVolumeLRSpline";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << resultID << " " << stepValue <<
    " \"" << analysisID << "\" " << bBox[0] << " " << bBox[1] << " " << bBox[2] << " " << 
    bBox[3] << " " << bBox[4] << " " << bBox[5] << " " << tolerance << " " << numSteps;
  // VolumeLRBspline stored in partitionID = 0
  bool found = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 0,
						 vqueryName, vqueryParametersStream.str(), 
						 NULL); // we don't want the data just to check if it's there

  return found;
}
