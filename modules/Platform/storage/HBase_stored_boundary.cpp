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

static std::string getBBOXParametersString( const std::string &modelID, 
					    const std::string &analysisID, const int numSteps, const double *lstSteps,
					    const int64_t numVertexIDs, const int64_t *lstVertexIDs) {
  std::stringstream vqueryParametersStream;
  std::string stepsParameter = "N/A";
  if ( numSteps && lstSteps) {
    stepsParameter = computeMD5( std::string( ( const char *)lstSteps, sizeof( double) * ( size_t)numSteps));
  }
  std::string verticesParameter = "All";
  if ( numVertexIDs && lstVertexIDs) {
    verticesParameter = computeMD5( std::string( ( const char *)lstVertexIDs, sizeof( int64_t) * ( size_t)numVertexIDs));
  }
  vqueryParametersStream << modelID << " \"" << analysisID << "\" " 
			 << numSteps << " " << stepsParameter << " " 
			 << numVertexIDs << verticesParameter;
  return vqueryParametersStream.str();
}

////////////////////////////////////////////////////////////////////////
// Stored BoundaryMesh
////////////////////////////////////////////////////////////////////////

void HBase::getStoredBoundaryOfAMesh( const std::string &sessionID,
				      const std::string &modelID,
				      const int meshID, const std::string &elementType,
				      const std::string &analysisID, const double stepValue,
				      std::string *return_binary_mesh, std::string *return_error_str) {
  std::string vqueryName = "GetBoundaryMesh";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" << analysisID << "\" " << stepValue;
  std::vector< std::string> lst_boundaries; // should only be one !!!

  // get number of pieces, i.e. metadata Q:qrNum
  // ?not needed? eventually retrieve all i can ....

  // binary mesh stored in partitionID = 0
  bool scan_ok = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 0,
						   vqueryName, vqueryParametersStream.str(),
						   &lst_boundaries);
  if ( !scan_ok) {
    *return_error_str = std::string( "Stored boundary mesh not found.");
    return;
  }
  if ( scan_ok && ( lst_boundaries.size() > 0)) {
    if ( return_binary_mesh) {
      *return_binary_mesh = lst_boundaries[ 0];
    }
    // *return_error_str = std::string( "Ok");
  } else {
    *return_error_str = std::string( "Stored boundary mesh not found.");
  }
}

bool HBase::deleteStoredBoundaryOfAMesh( const std::string &sessionID,
					 const std::string &modelID,
					 const int meshID, const std::string &elementType,
					 const std::string &analysisID, const double stepValue,
					 std::string *return_error_str) {
  std::string vqueryName = "GetBoundaryMesh";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" << analysisID << "\" " << stepValue;
  // binary mesh stored in partitionID = 0
  bool found = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue, 0,
						 vqueryName, vqueryParametersStream.str(), 
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

  const std::string &vqueryParameters = vqueryParametersStream.str();

  LOGGER_SM << "DELETING stored boundary of a mesh for '" << vqueryParameters << "'" << std::endl;
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
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0); // only one boundary mesh, in partition 0
  ok = deleteStoredRow( tableName, dataRowKey, "deleting stored Data row");
  return ok;
}

bool HBase::saveBoundaryOfAMesh( const std::string &sessionID,
				 const std::string &modelID,
				 const int meshID, const std::string &elementType,
				 const std::string &analysisID, const double stepValue,
				 const std::string &binary_mesh, std::string *return_error_str) {
  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false;
  }

  std::string vqueryName = "GetBoundaryMesh";
  std::stringstream vqueryParametersStream;
  vqueryParametersStream << modelID << " " << meshID << " " << elementType << " \"" << analysisID << "\" " << stepValue;
  const std::string &vqueryParameters = vqueryParametersStream.str();

  LOGGER_SM << "Saving boundary of a mesh for '" << vqueryParameters << "'" << std::endl;
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
  // binary mesh stored in partitionID = 0
  ok = storeQueryDataInDataTable( sessionID, modelID, analysisID, stepValue, 0, vqueryName, vqueryParameters, binary_mesh);
  return ok; // guess the rows are ok...  
}

////////////////////////////////////////////////////////////////////////
// Save & retrieve BoundingBox
////////////////////////////////////////////////////////////////////////

void HBase::getStoredBoundingBox( const std::string &sessionID, const std::string &modelID, 
				  const std::string &analysisID, const int numSteps, const double *lstSteps,
				  const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
				  double *return_bbox, std::string *return_error_str) {
  std::string vqueryName = "GetBoundingBox";
  std::string vqueryParameters = getBBOXParametersString( modelID, analysisID, numSteps, lstSteps, numVertexIDs, lstVertexIDs);
  std::vector< std::string> lst_bboxes; // should only be one !!!

  double stepValue = 0.0; // dummy step
  bool scan_ok = getStoredVQueryExtraData( sessionID, modelID, analysisID, stepValue,
					   vqueryName, vqueryParameters,
					   &lst_bboxes);
  if ( !scan_ok) {
    *return_error_str = std::string( "Stored bounding box not found.");
    return;
  }
  if ( scan_ok && ( lst_bboxes.size() > 0)) {
    if ( return_bbox) {
      memcpy( return_bbox, lst_bboxes[ 0].data(), 6 * sizeof( double));
    }
    // *return_error_str = std::string( "Ok");
  } else {
    *return_error_str = std::string( "Stored Bounding Box not found.");
  }
}

bool HBase::deleteStoredBoundingBox( const std::string &sessionID, const std::string &modelID, 
				     const std::string &analysisID, const int numSteps, const double *lstSteps,
				     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
				     std::string *return_error_str) {
  std::string vqueryName = "GetBoundingBox";
  std::string vqueryParameters = getBBOXParametersString( modelID, analysisID, numSteps, lstSteps, numVertexIDs, lstVertexIDs);
  double stepValue = 0.0; // dummy step
  bool found = getStoredVQueryExtraData( sessionID, modelID, analysisID, stepValue,
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

  LOGGER_SM << "Deleting stored bounding box for '" << vqueryParameters << "'" << std::endl;
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
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0); // only one boundary mesh, in partition 0
  ok = deleteStoredRow( tableName, dataRowKey, "deleting stored Data row");
  return ok;
}

bool HBase::saveBoundingBox( const std::string &sessionID, const std::string &modelID, 
			     const std::string &analysisID, const int numSteps, const double *lstSteps,
			     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			     const double *return_bbox, std::string *return_error_str) {
  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false;
  }
  
  std::string vqueryName = "GetBoundingBox";
  std::string vqueryParameters = getBBOXParametersString( modelID, analysisID, numSteps, lstSteps, numVertexIDs, lstVertexIDs);

  LOGGER_SM << "Saving bounding box for '" << vqueryParameters << "'" << std::endl;
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

  double stepValue = 0.0; // dummy step
  // adding metadata row:
  {
    StrMap attr;
    std::string tableName = table_set._stored_vquery_metadata;
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
  }

  // cell can only be 8MB big .... may be ( 6 doubles < 8MB)
  // adding data row:
  {
    StrMap attr;
    std::string tableName = table_set._stored_vquery_data;
    std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0); // only one boundary mesh, in partition 0
    std::vector< Mutation> data_mutations;
    data_mutations.push_back( Mutation());
    data_mutations.back().column = "Q:qr";
    std::string bbox_str( ( const char *)return_bbox, sizeof( double) * 6);
    data_mutations.back().value = bbox_str;
    LOGGER_SM << " Accessing table '" << tableName << "' with" << std::endl;
    LOGGER_SM << "         rowKey = " << dataRowKey << std::endl;
    LOGGER_SM << "   saving data cells with " << bbox_str.size() << " bytes" << std::endl;
    std::string tmp_report = "   data row saved";
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
  }

  // and now adding to the VELaSSCo_Models table as Properties:bb
  bool doUpdateVELaSSCoModels = true;
  if ( doUpdateVELaSSCoModels) {
    StrMap attr;
    std::string tableName = table_set._list_models;
    std::string modelRowKey = createModelListRowKey( modelID);
    std::vector< Mutation> model_mutations;
    model_mutations.push_back( Mutation());
    model_mutations.back().column = "Properties:bb";
    // store doubles in big-endian:
    double big_endian_bbox[ 6];
    big_endian_bbox[ 0] = byteSwap< double>( return_bbox[ 0]);
    big_endian_bbox[ 1] = byteSwap< double>( return_bbox[ 1]);
    big_endian_bbox[ 2] = byteSwap< double>( return_bbox[ 2]);
    big_endian_bbox[ 3] = byteSwap< double>( return_bbox[ 3]);
    big_endian_bbox[ 4] = byteSwap< double>( return_bbox[ 4]);
    big_endian_bbox[ 5] = byteSwap< double>( return_bbox[ 5]);
    // std::string bbox_str( ( const char *)return_bbox, sizeof( double) * 6);
    std::string bbox_str( ( const char *)big_endian_bbox, sizeof( double) * 6);
    model_mutations.back().value = bbox_str;
    LOGGER_SM << " Accessing table '" << tableName << "' with" << std::endl;
    LOGGER_SM << "         rowKey = " << modelRowKey << std::endl;
    LOGGER_SM << "   saving model cells with " << bbox_str.size() << " bytes" << std::endl;
    std::string tmp_report = "   model cell row saved";
    try {
      _hbase_client->mutateRow( tableName, modelRowKey, model_mutations, attr);
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
  }
  return ok; // guess the rows are ok...  
}

