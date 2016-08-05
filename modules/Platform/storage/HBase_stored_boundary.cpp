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

bool HBase::checkIfTableExists( const std::string table_name) {
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

// static bool getExtraDataFromRow( std::vector< std::string> &lst_qr_data, const TRowResult &rowResult) {
//   int num_data = 0;
//   for ( CellMap::const_iterator it = rowResult.columns.begin(); 
// 	it != rowResult.columns.end(); ++it) {
//     const char *cq_str = it->first.c_str();
//     const char CF = cq_str[ 0];
//     // cq_str[ 1] should be ':'
//     // const char subC = cq_str[ 2];
//     if ( CF == 'Q') {
//       if ( strcmp( &cq_str[ 3], "qr")) {
// 	// lst_qr_data.push_back( base64_decode( std::string( it->second.value.data(), it->second.value.length())));
// 	lst_qr_data.push_back( std::string( it->second.value.data(), it->second.value.length()));
// 	num_data++;
//       }
//     }
//   }
//   return num_data;
// }

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
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
  }
  // LOGGER_SM << "report = " << tmp_report << std::endl;  

  if ( qualifier_found) {
    if ( retValue.length() == 0) {
      // it's empty, but something has been read
    }
    if ( tmp_report.length() == 0) {
      std::stringstream tmp;
      tmp << "got value for " << col_str << " = '" << retValue << "'";
      tmp_report = tmp.str();
    }
  } else {
    LOGGER_SM << logMessagePrefix << " Not found: " << col_str << std::endl;
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
    tmp << std::string( columnFamily) << ":" << columnQualifierPrefix << "[0.." << numStringsToRetrieve << "]";
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
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
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
    LOGGER_SM << logMessagePrefix << " Not found: " << col_str << std::endl;
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
  std::string bin_mesh;
  for ( size_t idx = 0; idx < retLstValues.size(); idx++) {
    bin_mesh += retLstValues[ idx];
  }
  retLstValues.clear();
  lst_vquery_results->push_back( bin_mesh);

  return scan_ok;
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

  // get number of pieces, i.e. metadata Q:qrNum
  // ?not needed? eventually retrieve all i can ....

  bool scan_ok = getStoredVQueryExtraData( sessionID, modelID, analysisID, stepValue,
					 vqueryName, vqueryParametersStream.str(),
					 &lst_boundaries);
  if ( !scan_ok) {
    return_error_str = new std::string( "Stored boundary mesh not found.");
    return;
  }
  if ( scan_ok && ( lst_boundaries.size() > 0)) {
    *return_binary_mesh = lst_boundaries[ 0];
    return_error_str = new std::string( "Ok");
  } else {
    return_error_str = new std::string( "Stored boundary mesh not found.");
  }
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
  } catch ( const IOError &ioe) {
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    tmp_report = tmp.str();
    ok = false;
  } catch ( TException &tx) {
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
    ok = false;
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
  } catch ( const IOError &ioe) {
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    tmp_report = tmp.str();
    ok = false;
  } catch ( TException &tx) {
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
    ok = false;
  }
  LOGGER_SM << "     report = " << tmp_report << std::endl;  
  return ok;
}

void SplitBinaryMeshInChunks( std::vector< std::string> &lst_chunks_data, const std::string &binary_mesh) {
  const size_t chunk_size = 5*1024*1024;
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

  // cell can only be 10MB big .... may be
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
  } catch ( TException &tx) {
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
    ok = false;
  }
  LOGGER_SM << "     report = " << tmp_report << std::endl;  

  tableName = table_set._stored_vquery_data;

  // adding data row:
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0); // only one boundary mesh, in partition 0
  std::vector< Mutation> data_mutations;
  // std::string enc_mesh = base64_encode( binary_mesh.data(), binary_mesh.size());
  // data_mutations.push_back( Mutation());
  // data_mutations.back().column = "Q:qr";
  // data_mutations.back().value = enc_mesh;
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
    sprintf( tmp_col, "Q:qr_%d", ( int)idx); // at 10MB each chunk, i hope we'll never have 2*10^9 * 10 MB bytes !!!
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
  } catch ( TException &tx) {
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    tmp_report = tmp.str();
    ok = false;
  }
  LOGGER_SM << "     report = " << tmp_report << std::endl;  

  return ok; // guess the rows are ok...  
}
