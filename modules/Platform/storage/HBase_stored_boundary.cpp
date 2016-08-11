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

bool HBase::checkIfTableExists( const std::string &table_name) {
  std::vector< Text> lst_tables;
  _hbase_client->getTableNames( lst_tables);
  bool found = false;
  for ( std::vector< Text>::const_iterator itbl = lst_tables.begin(); itbl != lst_tables.end(); itbl++) {
    if ( table_name.compare( *itbl) == 0) {
      // check if enabled:
      if ( _hbase_client->isTableEnabled( table_name)) {
	found = true;
	break;
      }
    }
  }
  return found;
}

static bool getColumnQualifierStringFromRow( std::string &retValue, const TRowResult &rowResult, 
					     const char *columnFamily, const char *columnQualifier) {
  int num_data = 0;
  for ( CellMap::const_iterator it = rowResult.columns.begin(); 
	it != rowResult.columns.end(); ++it) {
    const char *cq_str = it->first.c_str();
    const char *separator = strchr( cq_str, ':');
    size_t len_CF = separator - cq_str;
    if ( separator && !strncmp( cq_str, columnFamily, len_CF)) {
      separator++;
      if ( !strcmp( separator, columnQualifier)) {
	retValue = std::string( it->second.value.data(), it->second.value.length());
	num_data++;
      }
    }
  }
  return num_data;
}

bool HBase::getColumnQualifierStringFromTable( std::string &retValue, 
					       const std::string &tableName, 
					       const std::string &startRowKey, const std::string &stopRowKey,
					       const char *columnFamily, const char *columnQualifier,
					       const std::string &logMessagePrefix) {
  static bool reset_tables = false; // to debug an so on...
  if ( reset_tables) {
    if ( checkIfTableExists( tableName)) {
      _hbase_client->disableTable( tableName);
      _hbase_client->deleteTable( tableName);
    }
    reset_tables = false;
  }
  if ( !checkIfTableExists( tableName)) {
    return false;
  }


  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  StrVec cols;
  std::string col_str = std::string( columnFamily) + ":" + columnQualifier;
  cols.push_back( col_str);
  bool scan_ok = true;
  retValue.clear();
  std::string tmp_report = "";
  LOGGER_SM << logMessagePrefix << " Accessing table '" << tableName << "' with" << std::endl;
  LOGGER_SM << "     startRowKey = " << startRowKey << std::endl;
  LOGGER_SM << "      stopRowKey = " << stopRowKey << std::endl;

  bool qualifier_found = false;
  try {
    ScannerID scan_id = _hbase_client->scannerOpenWithStop( tableName, 
							    startRowKey, stopRowKey, cols, m);
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
  	break;
      // process rowsResult
      for ( size_t i = 0; i < rowsResult.size(); i++) {
	// // check if the rowkey is our's ... should be ....
	// if ( rowsResult[ i].row.compare( 0, len_prefix_rowkey, prefixRowKey) != 0)
	//   continue; // break;

	qualifier_found = getColumnQualifierStringFromRow( retValue, rowsResult[ i], columnFamily, columnQualifier);
  	if ( qualifier_found) {
	  // i'm looking only for 1 string,
	  // so quitting
	  break;
  	} else {
	}
	// several rows = partitions so don't quit yet,
	// break;
      }
    } // while ( true)
    _hbase_client->scannerClose( scan_id);
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    tmp_report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  }
  // LOGGER_SM << "report = " << tmp_report << std::endl;  

  if ( qualifier_found) {
    if ( retValue.length() == 0) {
      // it's empty, but something has been read
    }
    if ( tmp_report.length() == 0) {
      std::stringstream tmp;
      size_t num_bytes = retValue.size();
      size_t dump_bytes = ( num_bytes > 80) ? 80 : num_bytes;
      tmp << "got value for " << col_str << " = ( " << num_bytes << " bytes): " << std::endl;
      tmp << Hexdump( retValue, dump_bytes);
      tmp_report = tmp.str();
    }
  } else {
      std::stringstream tmp;
      tmp << " Not found: " << col_str;
      tmp_report = tmp.str();
  }
  LOGGER_SM << logMessagePrefix << " " << tmp_report << std::endl;

  return scan_ok;
}


static int getColumnQualifierStringListFromRow( std::vector< std::string> &retLstValues, 
						const TRowResult &rowResult, 
						const char *columnFamily, const char *columnQualifierPrefix,
						const int numStringsToRetrieve) {
  int num_data = 0;
  size_t len_cqPrefix = strlen( columnQualifierPrefix);
  for ( CellMap::const_iterator it = rowResult.columns.begin(); 
	it != rowResult.columns.end(); ++it) {
    const char *cq_str = it->first.c_str();
    const char *separator = strchr( cq_str, ':');
    size_t len_CF = separator - cq_str;
    if ( separator && !strncmp( cq_str, columnFamily, len_CF)) {
      separator++;
      if ( !strncmp( separator, columnQualifierPrefix, len_cqPrefix)) {
	std::string strValue = std::string( it->second.value.data(), it->second.value.length());
	// not get index of vector
	separator += len_cqPrefix;
	int idx_chunk = 0;
	bool ok = false;
	if ( sscanf( separator, "%d", &idx_chunk) == 1) {
	  if ( idx_chunk < numStringsToRetrieve) {
	    retLstValues[ idx_chunk] = strValue;
	    num_data++;
	    ok = true;
	  }
	}
	if ( !ok) {
	  LOGGER_SM << "Wrong index value = " << it->first << std::endl;
	  LOGGER_SM << "     should be < " << numStringsToRetrieve << std::endl;
	}
      }
    }
  }
  return num_data;
}

bool HBase::getColumnQualifierStringListFromTable( std::vector< std::string> &retLstValues, 
						   const std::string &tableName, 
						   const std::string &startRowKey, const std::string &stopRowKey,
						   const char *columnFamily, const char *columnQualifierPrefix,
						   const int numStringsToRetrieve,
						   const std::string &logMessagePrefix) {
  bool reset_tables = false; // to debug an so on...
  if ( reset_tables) {
    if ( checkIfTableExists( tableName)) {
      _hbase_client->disableTable( tableName);
      _hbase_client->deleteTable( tableName);
    }
  }
  if ( !checkIfTableExists( tableName)) {
    return false;
  }


  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  StrVec cols;
  for ( int idx = 0; idx < numStringsToRetrieve; idx++) {
    std::stringstream col_str;
    col_str << std::string( columnFamily) << ":" << columnQualifierPrefix << idx;
    cols.push_back( col_str.str());
  }
  // just for logging messages:
  std::string col_str;
  {
    std::stringstream tmp;
    tmp << std::string( columnFamily) << ":" << columnQualifierPrefix << "[0.." << ( numStringsToRetrieve - 1) << "]";
    col_str = tmp.str();
  }
  bool scan_ok = true;
  retLstValues.resize( numStringsToRetrieve);
  std::string tmp_report = "";
  LOGGER_SM << logMessagePrefix << " Accessing table '" << tableName << "' with" << std::endl;
  LOGGER_SM << "     startRowKey = " << startRowKey << std::endl;
  LOGGER_SM << "      stopRowKey = " << stopRowKey << std::endl;

  int num_qualifier_found = 0;
  try {
    ScannerID scan_id = _hbase_client->scannerOpenWithStop( tableName, 
							    startRowKey, stopRowKey, cols, m);
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
  	break;
      // process rowsResult
      for ( size_t i = 0; i < rowsResult.size(); i++) {
	// // check if the rowkey is our's ... should be ....
	// if ( rowsResult[ i].row.compare( 0, len_prefix_rowkey, prefixRowKey) != 0)
	//   continue; // break;
	
	int num_found = getColumnQualifierStringListFromRow( retLstValues, rowsResult[ i], columnFamily, columnQualifierPrefix, numStringsToRetrieve);
  	if ( num_found) {
	  // looking for more
	  num_qualifier_found += num_found;
  	} else {
	}
	// several rows = partitions so don't quit yet,
	// break;
      }
    } // while ( true)
    _hbase_client->scannerClose( scan_id);
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    tmp_report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  }
  // LOGGER_SM << "report = " << tmp_report << std::endl;  
  
  if ( num_qualifier_found) {
    if ( retLstValues.size() == 0) {
      // it's empty, but something has been read
    }
    if ( tmp_report.length() == 0) {
      std::stringstream tmp;
      tmp << "got "<< num_qualifier_found << " values for " << col_str;
      if ( num_qualifier_found != numStringsToRetrieve) {
	tmp << "BUT ASKED FOR " << numStringsToRetrieve << " values !!!!!";
	retLstValues.clear(); // eventually
      }
      tmp_report = tmp.str();
    }
  } else {
    std::stringstream tmp;
    tmp << " Not found: " << col_str;
    tmp_report = tmp.str();
  }
  LOGGER_SM << logMessagePrefix << " " << tmp_report << std::endl;
  
  return scan_ok;
}


// retrieve only the 'Q' column family of the Simulations_VQuery_Results_Data table
bool HBase::getStoredVQueryExtraData( const std::string &sessionID,
				      const std::string &modelID,
				      const std::string &analysisID, const double stepValue,
				      const std::string &vqueryName, const std::string &vqueryParameters,
				      std::vector< std::string> *lst_vquery_results) { // list = 1 entry per partition
  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false;
  }

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

  if ( !checkIfTableExists( table_set._stored_vquery_data)) {
    return false;
  }


  std::string startRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0);
  std::string stopRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, ( int)0x7fffffff);
  std::string prefixRowKey = createStoredMetadataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters);

  std::string qrValue = "";
  bool scan_ok = getColumnQualifierStringFromTable( qrValue,
						    table_set._stored_vquery_data,
						    startRowKey, stopRowKey,
						    "Q", "qr", "Getting Q:qr value");
  if ( !scan_ok || !qrValue.size()) {
    return false;
  }

  if ( lst_vquery_results) {
    lst_vquery_results->push_back( qrValue);
  }

  return scan_ok;
}
// retrieve only the 'Q' column family of the Simulations_VQuery_Results_Data table
bool HBase::getStoredVQueryExtraDataSplitted( const std::string &sessionID,
					      const std::string &modelID,
					      const std::string &analysisID, const double stepValue,
					      const std::string &vqueryName, const std::string &vqueryParameters,
					      std::vector< std::string> *lst_vquery_results) { // list = 1 entry per partition
  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false;
  }

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

  if ( !checkIfTableExists( table_set._stored_vquery_data)) {
    return false;
  }


  std::string startRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0);
  std::string stopRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, ( int)0x7fffffff);
  std::string prefixRowKey = createStoredMetadataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters);

  std::string qrNumStr = "";
  bool scan_ok = getColumnQualifierStringFromTable( qrNumStr,
						    table_set._stored_vquery_data,
						    startRowKey, stopRowKey,
						    "Q", "qrNum", "Getting Q:qrNum value");
  if ( !scan_ok || !qrNumStr.size()) {
    return false;
  }
  int qrNum = 0;
  if ( sscanf( qrNumStr.c_str(), "%d", &qrNum) != 1) {
    LOGGER_SM << "Getting Q:qr value not a number = '" << qrNumStr << "'" << std::endl;
    return false;
  }

  LOGGER_SM << "Getting the " << qrNumStr << " pieces of the stored boundary mesh." << std::endl;
  std::vector< std::string> retLstValues;//( qrNum); // preallocate number of entries
  scan_ok = getColumnQualifierStringListFromTable( retLstValues,
						   table_set._stored_vquery_data,
						   startRowKey, stopRowKey,
						   "Q", "qr_", qrNum,
						   "Getting boundary mesh pieces");
  if ( !scan_ok || !retLstValues.size()) {
    return false;
  }

  // join boundary mesh pieces together:
  if ( lst_vquery_results) {
    std::string bin_mesh;
    for ( size_t idx = 0; idx < retLstValues.size(); idx++) {
      bin_mesh += retLstValues[ idx];
    }
    retLstValues.clear(); // make room for next push_back
    lst_vquery_results->push_back( bin_mesh);
  } else {
    retLstValues.clear();
  }

  return scan_ok;
}

bool HBase::deleteStoredRow( const std::string &tableName, const std::string &rowKey,
			     const std::string &logMessagePrefix) {
  StrMap attr;
  std::string tmp_report = "deleted from '" + tableName + "' row with key = " + rowKey;
  bool scan_ok = true;
  try {
    _hbase_client->deleteAllRow( tableName, rowKey, attr);
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    tmp_report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  }
  LOGGER_SM << tmp_report << std::endl;
  return scan_ok;
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

  bool scan_ok = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue,
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
  bool found = getStoredVQueryExtraDataSplitted( sessionID, modelID, analysisID, stepValue,
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

bool HBase::createStoredMetadataTable( const std::string &table_name) {
  // create metadata table
  ColVec col_fam;
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "M:";
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "G:";
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "R:";
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "Q:";
  std::string tmp_report = table_name + " created";
  bool ok = true;
  LOGGER_SM << "Creating table '" << table_name << "'" << std::endl;
  try {
    _hbase_client->createTable( table_name, col_fam);
    // _hbase_client->enableTable( table_name);
    ok = true;
  } catch ( const AlreadyExists &ae) {
    std::stringstream tmp;
    tmp << "Warning, already exists = " << ae.message;
    tmp_report = tmp.str();
    ok = false;
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
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
  return ok;
}

bool HBase::createStoredDataTable( const std::string &table_name) {
  // create data table
  ColVec col_fam;
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "M:";
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "R:";
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "Q:";
  std::string tmp_report = table_name + " created";
  bool ok = true;
  LOGGER_SM << "Creating table '" << table_name << "'" << std::endl;
  try {
    _hbase_client->createTable( table_name, col_fam);
    // _hbase_client->enableTable( table_name);
    ok = true;
  } catch ( const AlreadyExists &ae) {
    std::stringstream tmp;
    tmp << "Warning, already exists = " << ae.message;
    tmp_report = tmp.str();
    ok = false;
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
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
  return ok;
}

void SplitBinaryMeshInChunks( std::vector< std::string> &lst_chunks_data, const std::string &binary_mesh) {
  const size_t chunk_size = 8 * 1024 * 1024;
  size_t start_idx = 0;
  size_t end_idx = binary_mesh.size();
  for ( start_idx = 0; start_idx < end_idx; start_idx += chunk_size) {
    size_t cur_chunk_size = chunk_size;
    if ( start_idx + cur_chunk_size > end_idx)
      cur_chunk_size = end_idx - start_idx;
    lst_chunks_data.push_back( binary_mesh.substr( start_idx, cur_chunk_size));
  }
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
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0); // only one boundary mesh, in partition 0
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

////////////////////////////////////////////////////////////////////////
// Save & retrieve BoundingBox
////////////////////////////////////////////////////////////////////////

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
  bool doUpdateVELaSSCoModels = false;
  if ( doUpdateVELaSSCoModels) {
    StrMap attr;
    std::string tableName = table_set._list_models;
    std::string modelRowKey = createModelListRowKey( modelID);
    std::vector< Mutation> model_mutations;
    model_mutations.push_back( Mutation());
    model_mutations.back().column = "Properties:bb";
    std::string bbox_str( ( const char *)return_bbox, sizeof( double) * 6);
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

