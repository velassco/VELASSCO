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

static bool getExtraDataFromRow( std::vector< std::string> &lst_qr_data, const TRowResult &rowResult) {
  int num_data = 0;
  for ( CellMap::const_iterator it = rowResult.columns.begin(); 
	it != rowResult.columns.end(); ++it) {
    const char *cq_str = it->first.c_str();
    const char CF = cq_str[ 0];
    // cq_str[ 1] should be ':'
    // const char subC = cq_str[ 2];
    if ( CF == 'Q') {
      if ( strcmp( &cq_str[ 3], "qr")) {
	// lst_qr_data.push_back( base64_decode( std::string( it->second.value.data(), it->second.value.length())));
	lst_qr_data.push_back( std::string( it->second.value.data(), it->second.value.length()));
	num_data++;
      }
    }
  }
  return num_data;
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
  const size_t len_prefix_rowkey = prefixRowKey.length();

  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  StrVec cols;  
  
  // we are looking for the extra data, i.e. the column family 'Q', qualifier 'qr'
  cols.push_back( std::string( "Q:qr"));
  bool scan_ok = true;
  std::vector< std::string> lst_qr_data;
  std::string tmp_report;
  LOGGER_SM << "\tAccessing table '" << table_set._stored_vquery_data << "' with" << std::endl;
  LOGGER_SM << "\t startRowKey = " << startRowKey << std::endl;
  LOGGER_SM << "\t  stopRowKey = " << stopRowKey << std::endl;

  try {
    ScannerID scan_id = _hbase_client->scannerOpenWithStop( table_set._stored_vquery_data, 
							    startRowKey, stopRowKey, cols, m);
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
  	break;
      // process rowsResult
      for ( size_t i = 0; i < rowsResult.size(); i++) {
	// check if the rowkey is our's ... should be ....
	if ( rowsResult[ i].row.compare( 0, len_prefix_rowkey, prefixRowKey) != 0)
	  continue; // break;
  	bool ok = getExtraDataFromRow( lst_qr_data, rowsResult[ i]);
  	if ( ok) {
  	  // getMeshInfoFromRow.push_back( model_info);
	  // getMeshInfoFromRow( tmp_lst_meshes, rowsResult[ i]);
  	} else {
	  // nothing read? 
	  // eventually provide an error
	}
	if ( ok && !lst_vquery_results)  {
	  // we just wanted to know if there is something
	  break;
	}
	// several rows = partitions so don't quit yet,
	// break;
      }
    } // while ( true)

    if ( lst_vquery_results) {
      // the calles wants to save the data
      if ( lst_qr_data.size() == 0) {
	// nothing found
	// scan_ok = false; // scan was ok but nothing found ...
	std::stringstream tmp;
	tmp << "Not found: Query Extra data for '" << vqueryName << " " << vqueryParameters << "'";
	tmp_report = tmp.str();
      } else {
	std::stringstream tmp;
	tmp << "Found: Query Extra data for '" << vqueryName << " " << vqueryParameters << "'";
	tmp_report = tmp.str();
	*lst_vquery_results = lst_qr_data;
      }
    }
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
  LOGGER_SM << "report = " << tmp_report << std::endl;  

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
  LOGGER_SM << "   saving metadata cells with " << vqueryName.size() + vqueryParameters.size() << " bytes" << std::endl;
  std::string tmp_report = "   metadata row saved";
  try {
    _hbase_client->mutateRow( table_set._stored_vquery_metadata, metadataRowKey, metadata_mutations, attr);
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

  // adding data row:
  std::string dataRowKey = createStoredDataRowKey( modelID, analysisID, stepValue, vqueryName, vqueryParameters, 0); // only one boundary mesh, in partition 0
  std::vector< Mutation> data_mutations;
  // std::string enc_mesh = base64_encode( binary_mesh.data(), binary_mesh.size());
  const std::string &enc_mesh = binary_mesh;
  data_mutations.push_back( Mutation());
  data_mutations.back().column = "Q:qr";
  data_mutations.back().value = enc_mesh;
  LOGGER_SM << "   saving data cells with " << enc_mesh.size() << " bytes" << std::endl;
  tmp_report = "   data row saved";
  try {
    _hbase_client->mutateRow( table_set._stored_vquery_data, dataRowKey, data_mutations, attr);
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
