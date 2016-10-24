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

void SplitBinaryDataInChunks( std::vector< std::string> &lst_chunks_data, const std::string &binary_data) {
  const size_t chunk_size = 8 * 1024 * 1024;
  size_t start_idx = 0;
  size_t end_idx = binary_data.size();
  for ( start_idx = 0; start_idx < end_idx; start_idx += chunk_size) {
    size_t cur_chunk_size = chunk_size;
    if ( start_idx + cur_chunk_size > end_idx)
      cur_chunk_size = end_idx - start_idx;
    lst_chunks_data.push_back( binary_data.substr( start_idx, cur_chunk_size));
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

bool HBase::checkIfTableExists( const std::string &tableName) {
  std::vector< Text> lst_tables;
  _hbase_client->getTableNames( lst_tables);
  bool found = false;
  for ( std::vector< Text>::const_iterator itbl = lst_tables.begin(); itbl != lst_tables.end(); itbl++) {
    if ( tableName.compare( *itbl) == 0) {
      // check if enabled:
      if ( _hbase_client->isTableEnabled( tableName)) {
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
					      int partitionID,
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


  std::string startRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, partitionID);// 0);
  std::string stopRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, partitionID + 1);// ( int)0x7fffffff);
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

  std::string dataTypeMsg = vqueryName + " data";
  LOGGER_SM << "Getting the " << qrNumStr << " pieces of calculated " << dataTypeMsg << "." << std::endl;
  std::vector< std::string> retLstValues;//( qrNum); // preallocate number of entries
  scan_ok = getColumnQualifierStringListFromTable( retLstValues,
						   table_set._stored_vquery_data,
						   startRowKey, stopRowKey,
						   "Q", "qr_", qrNum,
						   "Getting " + dataTypeMsg + " pieces");
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
  std::string tmp_report = "deleted from '" + tableName + "'\n        row with key = " + rowKey;
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

bool HBase::createStoredMetadataTable( const std::string &tableName) {
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
  std::string tmp_report = tableName + " created";
  bool ok = true;
  LOGGER_SM << "Creating table '" << tableName << "'" << std::endl;
  try {
    _hbase_client->createTable( tableName, col_fam);
    // _hbase_client->enableTable( tableName);
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

bool HBase::createStoredDataTable( const std::string &tableName) {
  // create data table
  ColVec col_fam;
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "M:";
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "R:";
  col_fam.push_back( ColumnDescriptor());
  col_fam.back().name = "Q:";
  std::string tmp_report = tableName + " created";
  bool ok = true;
  LOGGER_SM << "Creating table '" << tableName << "'" << std::endl;
  try {
    _hbase_client->createTable( tableName, col_fam);
    // _hbase_client->enableTable( tableName);
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

bool HBase::getStoredListRowKeysForThisModel( const std::string &tableName, 
					      const std::string &startRowKey, const std::string &stopRowKey,
					      const std::string &logMessagePrefix,						  
					      std::vector< std::string> &lstRowKeysForThisModel) {
  if ( !checkIfTableExists( tableName)) {
    return false;
  }

  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  StrVec cols;
  bool scan_ok = true;

  std::string tmp_report = "";
  LOGGER_SM << logMessagePrefix << " Accessing from table '" << tableName << "' with" << std::endl;
  LOGGER_SM << "     startRowKey = " << startRowKey << std::endl;
  LOGGER_SM << "      stopRowKey = " << stopRowKey << std::endl;

  size_t numFoundRowKeys = 0;
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
	lstRowKeysForThisModel.push_back( rowsResult[ i].row);
      }
    } // while ( true)
    _hbase_client->scannerClose( scan_id);
    
    numFoundRowKeys = lstRowKeysForThisModel.size();
    {
      std::stringstream tmp;
      tmp << "Found " << numFoundRowKeys << " rowkeys";
      if ( numFoundRowKeys > 0) {
	for ( size_t i = 0; i < numFoundRowKeys; i++) {
	  tmp << std::endl << "   " << lstRowKeysForThisModel[ i];
	}
      }
      tmp_report = tmp.str();
    }
    scan_ok = true;
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

  LOGGER_SM << logMessagePrefix << " " << tmp_report << std::endl;

  return scan_ok;
}

bool HBase::getStoredMetadataRowKeysForThisModel( const std::string &tableName, const std::string &modelID,			  
						  std::vector< std::string> &lstRowKeysForThisModel) {
  if ( !checkIfTableExists( tableName)) {
    return false;
  }

  // build start and stop rowkeys:
  // like this:
  // modelID                           Analysis StepValue         MD5( VQueryName + VQueryParameters)
  // 12345678901234567890123456789012  324 123  1234567890123456  12345678901234567890123456789012
  // 60515e0100000000504f5e0100000000  00000000 0000000000000000  18818eb6d6d757c138604669c00c4c4d  
  // std::string metadataRowKey = createStoredMetadataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters);
  std::string startRowKey = modelID + std::string( 8, '0') + std::string( 16, '0') + std::string( 32, '0');
  // stopkey like this:
  // modelID+1, Analysis = 0, StepValue = 0; MD5 = 0:
  // 60515e0100000000504f5e0100000001  00000000 0000000000000000  18818eb6d6d757c138604669c00c4c4d
  std::string nextModelID = GetStopKeyFromModelID( modelID);
  std::string stopRowKey = nextModelID + std::string( 8, '0') + std::string( 16, '0') + std::string( 32, '0');

  return getStoredListRowKeysForThisModel( tableName, startRowKey, stopRowKey, "Getting stored metadata rowkeys", lstRowKeysForThisModel);
}

bool HBase::getStoredDataRowKeysForThisModel( const std::string &tableName, const std::string &modelID,			  
						  std::vector< std::string> &lstRowKeysForThisModel) {
  if ( !checkIfTableExists( tableName)) {
    return false;
  }

  // build start and stop rowkeys:
  // like this:
  // modelID                           Analysis StepValue         MD5( VQueryName + VQueryParameters) PartitionID
  // 12345678901234567890123456789012  324 123  1234567890123456  12345678901234567890123456789012    12345678
  // 60515e0100000000504f5e0100000000  00000000 0000000000000000  18818eb6d6d757c138604669c00c4c4d    7fffffff
  // std::string metadataRowKey = createStoredMetadataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters);
  std::string startRowKey = modelID + std::string( 8, '0') + std::string( 16, '0') + std::string( 32, '0') + std::string( 8, '0');
  // stopkey like this:
  // modelID+1, Analysis = 0, StepValue = 0; MD5 = 0:
  // 60515e0100000000504f5e0100000001  00000000 0000000000000000  18818eb6d6d757c138604669c00c4c4d
  std::string nextModelID = GetStopKeyFromModelID( modelID);
  std::string stopRowKey = nextModelID + std::string( 8, '0') + std::string( 16, '0') + std::string( 32, '0') + std::string( 8, '0');

  return getStoredListRowKeysForThisModel( tableName, startRowKey, stopRowKey, "Getting stored data rowkeys", lstRowKeysForThisModel);
}

bool HBase::deleteAllStoredCalculationsForThisModel( const std::string &sessionID,
						     const std::string &modelID,
						     std::string *return_error_str) {
  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false; // no information about tables for this user and this session
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

  // need to get all rowkeys and then
  // foreach rk in lst_rowkeys do 
  //   _hbase_client->deleteAllRow( tableName, rowKey, attr); 
  bool ok = true;
  if ( ok) {
    std::vector< std::string> lstMetadataRowKeys;
    ok = getStoredMetadataRowKeysForThisModel( table_set._stored_vquery_metadata, modelID, lstMetadataRowKeys);
    for ( size_t i = 0; i < lstMetadataRowKeys.size(); i++) {
      deleteStoredRow( table_set._stored_vquery_metadata, lstMetadataRowKeys[ i], "deleteAllStoredCalculationsForThisModel");
    }
  }
  if ( ok) {
    std::vector< std::string> lstDataRowKeys;
    ok = getStoredDataRowKeysForThisModel( table_set._stored_vquery_data, modelID, lstDataRowKeys);
    for ( size_t i = 0; i < lstDataRowKeys.size(); i++) {
      deleteStoredRow( table_set._stored_vquery_data, lstDataRowKeys[ i], "deleteAllStoredCalculationsForThisModel");
    }
  }
  return ok;
}

bool HBase::storeQueryInfoInMetadataTable( const std::string &sessionID,
					   const std::string &modelID,
					   const std::string &analysisID, const double stepValue,
					   const std::string &vqueryName, const std::string &vqueryParameters) {
  bool ok = true;
  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false;
  }
  std::string tableName = table_set._stored_vquery_metadata;
  if ( !checkIfTableExists( table_set._stored_vquery_metadata)) {
    ok = createStoredMetadataTable( table_set._stored_vquery_metadata);
    if ( !ok) return false;
  }
  
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
  // LOGGER_SM << " Accessing table '" << tableName << "' with" << std::endl;
  // LOGGER_SM << "         rowKey = " << metadataRowKey << std::endl;
  LOGGER_SM << "   storing metadata cells with " << vqueryName.size() + vqueryParameters.size() << " bytes" << std::endl;
  bool storeVerbose = false;
  ok = storeMutationsInTable( tableName, metadataRowKey, metadata_mutations, "storeQueryInfoInMetadataTable", storeVerbose);
  
  return ok;
}

bool HBase::storeMutationsInTable( const std::string &tableName, const std::string &rowKey,
        const std::vector< Mutation> &lstMutations, const std::string &errorPrefixMessage, bool storeVerbose) {
  // adding data row:
  StrMap attr;
  const Mutation &firstMutation = lstMutations[ 0];
  if ( storeVerbose) {
      LOGGER_SM << errorPrefixMessage << ": "
              << " Accessing table '" << tableName << "' with" 
              << " rowKey = " << rowKey
              << " to save " << lstMutations.size() << " mutations: " 
              << " column=" << firstMutation.column << " value=" << firstMutation.value << " ..." << std::endl;
  }
  std::string tmp_report = "row saved";
  bool ok = true;
  try {
    _hbase_client->mutateRow( tableName, rowKey, lstMutations, attr);
  } catch ( const IOError &ioe) {
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    tmp_report = errorPrefixMessage + ": " + tmp.str();
    ok = false;
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  } catch ( TException &tx) {
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = errorPrefixMessage + ": " + tmp.str();
    ok = false;
    LOGGER_SM << "EXCEPTION: " << tmp_report << std::endl;
  }
  if ( storeVerbose) {
      LOGGER_SM << errorPrefixMessage << ": " << " report = " << tmp_report << std::endl;  
  }
  return ok;
}
    
bool HBase::storeQueryDataInDataTable( const std::string &sessionID,
				       const std::string &modelID,
				       const std::string &analysisID, const double stepValue,
				       int partitionID,
				       const std::string &vqueryName, const std::string &vqueryParameters,
				       const std::string &vqueryData) {
  bool ok = true;
  TableModelEntry table_set;
  if ( !getVELaSSCoTableNames( sessionID, modelID, table_set)) {
    return false;
  }
  if ( !checkIfTableExists( table_set._stored_vquery_data)) {
    // create data table
    ok = createStoredMetadataTable( table_set._stored_vquery_data);
    if ( !ok) return false;
  }

  std::string tableName = table_set._stored_vquery_data;

  // cell can only be 8MB big .... may be ( with 10MB it raises an exception)
  std::vector< std::string> lst_chunks_data;
  SplitBinaryDataInChunks( lst_chunks_data, vqueryData);

  // adding data row:
  StrMap attr;
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, partitionID);
  std::vector< Mutation> data_mutations;
  data_mutations.push_back( Mutation());
  data_mutations.back().column = "Q:qrNum";
  {
    std::stringstream tmp;
    tmp << lst_chunks_data.size();
    data_mutations.back().value = tmp.str();
  }
  char tmp_col[ 100];
  // too many mutations also causes an IOException 
  // so we write every 32 mutations ( 8 * 32 = 256 MB) into HBase
  LOGGER_SM << "   storing data cells with " << vqueryData.size() << " bytes in " << lst_chunks_data.size() << " separated qualifiers" << std::endl;
  bool storeVerbose = false;
  int numChunks = 0;
  for ( size_t idx = 0; idx < lst_chunks_data.size(); idx++) {
    data_mutations.push_back( Mutation());
    sprintf( tmp_col, "Q:qr_%d", ( int)idx); // at 8MB each chunk, i hope we'll never have 2*10^9 * 10 MB bytes !!!
    data_mutations.back().column = std::string( tmp_col);
    data_mutations.back().value = lst_chunks_data[ idx];
    numChunks++;
    if ( numChunks == 32) {
      ok = storeMutationsInTable( tableName, dataRowKey, data_mutations, "storeQueryDataInDataTable", storeVerbose);
      if ( !ok) 
	break;
      LOGGER_SM << "   stored " << ( idx + 1) << " chunks." << std::endl;
      data_mutations.clear();
    }
  }
  // LOGGER_SM << " Accessing table '" << tableName << "' with" << std::endl;
  // LOGGER_SM << "         rowKey = " << dataRowKey << std::endl;
  if ( data_mutations.size() > 0) {
    ok = storeMutationsInTable( tableName, dataRowKey, data_mutations, "storeQueryDataInDataTable", storeVerbose);
    if ( ok) {
      LOGGER_SM << "   stored " << lst_chunks_data.size() << " chunks." << std::endl;
    }
  }
  
  return ok;
}

