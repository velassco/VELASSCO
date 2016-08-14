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

// common functions

bool getColumnQualifierStringFromRow( std::string &retValue, const TRowResult &rowResult, 
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

int getColumnQualifierStringListFromRow( std::vector< std::string> &retLstValues, 
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
  
// End of common functions

// Common HBase:: functions

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

// End of Common HBase:: functions

bool HBase::deleteAllStoredCalculations( const std::string &sessionID,
					 const std::string &modelID,
					 std::string *return_error_str) {
  // need to get all rowkeys and then
  // foreach rk in lst_rowkeys do 
  //   _hbase_client->deleteAllRow( tableName, rowKey, attr);
  return false;
}
