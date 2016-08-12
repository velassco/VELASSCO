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
// Stored SimplfiedMesh
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

  bool scan_ok = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue,
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
  bool found = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue,
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

  // cell can only be 8MB big .... may be ( with 10MB it raises an exception)
  std::vector< std::string> lst_chunks_data;
  SplitBinaryMeshInChunks( lst_chunks_data, binary_mesh);

  std::string tableName = table_set._stored_vquery_metadata;

  StrMap attr;
  // adding metadata row:
  std::string metadataRowKey = createStoredMetadataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters);
  std::vector< Mutation> metadata_mutations;
  metadata_mutations.push_back( Mutation());
  metadata_mutations.back().column = "Q:vq";
  metadata_mutations.back().value = vqueryName;
  metadata_mutations.push_back( Mutation());
  metadata_mutations.back().column = "Q:qp";
  metadata_mutations.back().value = vqueryParameters;
  LOGGER_SM << " Accessing table '" << tableName << "' with" << std::endl;
  LOGGER_SM << "         rowKey = " << metadataRowKey << std::endl;
  LOGGER_SM << "   saving metadata cells with " << vqueryName.size() + vqueryParameters.size() << " bytes" << std::endl;
  std::string tmp_report = "   metadata row saved";
  try {
    _hbase_client->mutateRow( tableName, metadataRowKey, metadata_mutations, attr);
  } catch ( const IOError &ioe) {
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    tmp_report = tmp.str();
    ok = false;
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  } catch ( TException &tx) {
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
    ok = false;
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  }
  LOGGER_SM << "     report = " << tmp_report << std::endl;  

  tableName = table_set._stored_vquery_data;

  // adding data row:
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0); // only one simplified mesh, in partition 0
  std::vector< Mutation> data_mutations;
  data_mutations.push_back( Mutation());
  data_mutations.back().column = "Q:qrNum";
  {
    std::stringstream tmp;
    tmp << lst_chunks_data.size();
    data_mutations.back().value = tmp.str();
  }
  char tmp_col[ 100];
  for ( size_t idx = 0; idx < lst_chunks_data.size(); idx++) {
    data_mutations.push_back( Mutation());
    sprintf( tmp_col, "Q:qr_%d", ( int)idx); // at 8MB each chunk, i hope we'll never have 2*10^9 * 10 MB bytes !!!
    data_mutations.back().column = std::string( tmp_col);
    data_mutations.back().value = lst_chunks_data[ idx];
  }
  LOGGER_SM << " Accessing table '" << tableName << "' with" << std::endl;
  LOGGER_SM << "         rowKey = " << dataRowKey << std::endl;
  LOGGER_SM << "   saving data cells with " << binary_mesh.size() << " bytes in " << lst_chunks_data.size() << " separated qualifiers" << std::endl;
  tmp_report = "   data row saved";
  try {
    _hbase_client->mutateRow( tableName, dataRowKey, data_mutations, attr);
  } catch ( const IOError &ioe) {
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    tmp_report = tmp.str();
    ok = false;
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  } catch ( TException &tx) {
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
    ok = false;
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  }
  LOGGER_SM << "     report = " << tmp_report << std::endl;  

  return ok; // guess the rows are ok...  
}
