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


////////////////////////////////////////////////////////////////////////
// Stored SimplifiedMesh
////////////////////////////////////////////////////////////////////////

void HBase::getStoredSimplifiedMesh( const std::string &sessionID,
				     const std::string &modelID,
				     const int meshID, const std::string &elementType,
				     const std::string &analysisID, const double stepValue,
				     const std::string &parameters, 
				     std::string *return_binary_mesh, std::string *return_error_str) {
  std::string vqueryName = "GetSimplifiedMesh";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" << analysisID << "\" " << stepValue << " \"" << parameters << "\"";
  std::vector< std::string> lst_simplified_meshes; // should only be one !!!

  // get number of pieces, i.e. metadata Q:qrNum
  // ?not needed? eventually retrieve all i can ....

  // binary mesh stored in partitionID = 0
  bool scan_ok = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 0,
						   vqueryName, vqueryParametersStream.str(),
						   &lst_simplified_meshes);
  if ( !scan_ok) {
    *return_error_str = std::string( "Stored simplified mesh not found.");
    return;
  }
  if ( scan_ok && ( lst_simplified_meshes.size() > 0)) {
    if ( return_binary_mesh) {
      *return_binary_mesh = lst_simplified_meshes[ 0];
    }
    // *return_error_str = std::string( "Ok");
  } else {
    *return_error_str = std::string( "Stored simplified mesh not found.");
  }
}

bool HBase::deleteStoredSimplifiedMesh( const std::string &sessionID,
					const std::string &modelID,
					const int meshID, const std::string &elementType,
					const std::string &analysisID, const double stepValue,
					const std::string &parameters, 
					std::string *return_error_str) {
  std::string vqueryName = "GetSimplifiedMesh";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" << analysisID << "\" " << stepValue << " \"" << parameters << "\"";
  // binary mesh stored in partitionID = 0
  bool found = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 0,
						 vqueryName, vqueryParametersStream.str(), 
						 NULL); // we don't want the data just to check if it's there
  if ( !found) {
    // already deleted?
    // *return_error_str = std::string( "Stored simplified mesh not found.");
    return_error_str->clear();
    return true; // not/never stored
  }

  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false; // no information about tables for this user and this session
  }

  const std::string &vqueryParameters = vqueryParametersStream.str();

  LOGGER_SM << "DELETING stored simplified mesh for '" << vqueryParameters << "'" << std::endl;
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
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0); // only one simplified mesh, in partition 0
  ok = deleteStoredRow( tableName, dataRowKey, "deleting stored Data row");
  return ok;
}

bool HBase::saveSimplifiedMesh( const std::string &sessionID,
				const std::string &modelID,
				const int meshID, const std::string &elementType,
				const std::string &analysisID, const double stepValue,
				const std::string &parameters, 
				const std::string &binary_mesh, std::string *return_error_str) {
  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false;
  }

  std::string vqueryName = "GetSimplifiedMesh";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" << analysisID << "\" " << stepValue << " \"" << parameters << "\"";
  const std::string &vqueryParameters = vqueryParametersStream.str();

  LOGGER_SM << "Saving simplified mesh for '" << vqueryParameters << "'" << std::endl;
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
  if ( !ok) return false;
  ok = storeQueryDataInDataTable( sessionID, modelID, analysisID, stepValue, 0, vqueryName, vqueryParameters, binary_mesh);
  return ok; // guess the rows are ok...  
}


////////////////////////////////////////////////////////////////////////
// Stored SimplifiedMeshWithResult
////////////////////////////////////////////////////////////////////////

void HBase::getStoredSimplifiedMeshWithResult( const std::string &sessionID,
					       const std::string &modelID,
					       const int meshID, const std::string &elementType,
					       const std::string &analysisID, const double stepValue,
					       const std::string &parameters, const std::string &resultID,
					       std::string *return_binary_mesh, std::string *return_binary_results, 
					       std::string *return_error_str) {
  std::string vqueryName = "GetSimplifiedMeshWithResult";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" 
			 << analysisID << "\" " << stepValue << " \"" << parameters << "\"" << " \"" << resultID << "\"";
  std::vector< std::string> lst_simplified_dataes; // should only be one !!!

  // get number of pieces, i.e. metadata Q:qrNum
  // ?not needed? eventually retrieve all i can ....

  // binary mesh stored in partitionID = 0
  bool scan_ok = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 0,
						   vqueryName, vqueryParametersStream.str(),
						   &lst_simplified_dataes);
  if ( !scan_ok || ( lst_simplified_dataes.size() <= 0)) {
    *return_error_str = std::string( "Stored mesh data from simplified mesh with results not found.");
    return;
  }
  if ( return_binary_mesh) {
    *return_binary_mesh = lst_simplified_dataes[ 0];
  }
  lst_simplified_dataes.clear();
  // binary results stored in partitionID = 1
  scan_ok = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 1,
						   vqueryName, vqueryParametersStream.str(),
						   &lst_simplified_dataes);
  if ( !scan_ok || ( lst_simplified_dataes.size() <= 0)) {
    *return_error_str = std::string( "Stored results data from simplified mesh with results not found.");
  }
  if ( return_binary_results) {
    *return_binary_results = lst_simplified_dataes[ 0];
  }
}

bool HBase::deleteStoredSimplifiedMeshWithResult( const std::string &sessionID,
					const std::string &modelID,
					const int meshID, const std::string &elementType,
					const std::string &analysisID, const double stepValue,
					const std::string &parameters, const std::string &resultID,
					std::string *return_error_str) {
  std::string vqueryName = "GetSimplifiedMeshWithResult";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" 
			 << analysisID << "\" " << stepValue << " \"" << parameters << "\"" << " \"" << resultID << "\"";
  // binary mesh stored in partitionID = 0
  bool found = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 0,
						 vqueryName, vqueryParametersStream.str(), 
						 NULL); // we don't want the data just to check if it's there
  // binary results stored in partitionID = 1
  bool found2 = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 1,
						 vqueryName, vqueryParametersStream.str(), 
						 NULL); // we don't want the data just to check if it's there
  if ( !found && !found2) {
    // already deleted?
    // *return_error_str = std::string( "Stored simplified mesh not found.");
    return_error_str->clear();
    return true; // not/never stored
  }

  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false; // no information about tables for this user and this session
  }

  const std::string &vqueryParameters = vqueryParametersStream.str();

  LOGGER_SM << "DELETING stored simplified mesh for '" << vqueryParameters << "'" << std::endl;
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
  // binary mesh in partition 0
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0);
  ok = deleteStoredRow( tableName, dataRowKey, "deleting stored Data row");
  // binary results in partition 1
  dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 1);
  ok = deleteStoredRow( tableName, dataRowKey, "deleting stored Data row");
  return ok;
}

bool HBase::saveSimplifiedMeshWithResult( const std::string &sessionID,
					  const std::string &modelID,
					  const int meshID, const std::string &elementType,
					  const std::string &analysisID, const double stepValue,
					  const std::string &parameters, const std::string &resultID,
					  const std::string &binary_mesh, const std::string &binary_results, 
					  std::string *return_error_str) {
  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false;
  }

  std::string vqueryName = "GetSimplifiedMeshWithResult";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" 
			 << analysisID << "\" " << stepValue << " \"" << parameters << "\"" << " \"" << resultID << "\"";
  const std::string &vqueryParameters = vqueryParametersStream.str();

  LOGGER_SM << "Saving simplified mesh for '" << vqueryParameters << "'" << std::endl;
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
  if ( !ok) return false;

  // binary mesh in partition 0
  ok = storeQueryDataInDataTable( sessionID, modelID, analysisID, stepValue, 0, vqueryName, vqueryParameters, binary_mesh);
  // binary results in partition 1
  ok = storeQueryDataInDataTable( sessionID, modelID, analysisID, stepValue, 1, vqueryName, vqueryParameters, binary_results);
  return ok; // guess the rows are ok...  
}
