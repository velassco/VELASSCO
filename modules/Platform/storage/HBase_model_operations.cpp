#include <stdio.h>

// CLib
#include <cmath>
#include <cstddef>

// STD
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

using namespace std;
using namespace VELaSSCo;

/////////////////////////////////
// Curl + Json
/////////////////////////////////
#include <curl/curl.h>
#include "cJSON.h"

// returns true if there are models on the table to be parsed
bool HBase::parseListOfModelNames( std::string &report,
				   std::vector< FullyQualifiedModelName> &listOfModelNames, 
				   std::string buffer, const std::string &table_name) {
  bool there_are_models = false;
  std::stringstream result;
  cJSON *json = cJSON_Parse( buffer.c_str());

  if ( !json) {
    // some error parsing the buffer
    report = "Error parsing:\n" + buffer;
    return false;
  }  

  // setbuf( stdout, NULL);
  // printf( "Number of elements = %d\n", cJSON_GetArraySize(json));
  for (int i = 0; i < cJSON_GetArraySize(json); i ++ ) {
    // Get each row element
    cJSON *row = cJSON_GetArrayItem ( json, i);
    // Read all elements of each row
    // printf( "Number of rows = %d\n", cJSON_GetArraySize(row));
    int num_models = cJSON_GetArraySize(row);
    there_are_models = ( num_models > 0);
    for (int j = 0; j < num_models; j ++ ) {
      // printf( "row # %d\n", j);
      //A Hbase key  is composed by two elements: key and cell (which contains data)
      cJSON *keyC = cJSON_GetArrayItem (row, j);
      cJSON *keyJ = cJSON_GetArrayItem (keyC, 0);
      // Row key:
      string key = base64_decode(keyJ->valuestring);
      cJSON *cellsJ = cJSON_GetArrayItem (keyC, 1);
      // cJSON * elem  = cJSON_CreateObject();
      // printf( "keyC = %s\n", keyC->valuestring);
      // printf( "keyJ(decod) = %s", Hexdump( key).c_str());

      FullyQualifiedModelName model_info;
      model_info.__set_modelID( key);
      model_info.__set_location( "Hbase:" + table_name);
      for (int k = 0; k < cJSON_GetArraySize(cellsJ); k++) {
	// printf( "   cell # %d\n", k);
	cJSON *contents = cJSON_GetArrayItem (cellsJ, k);
	// contents is an array with 3 items: ( value, timestap, and "$")
	// printf( "   contents size %d\n", cJSON_GetArraySize( contents));
	// Colum Qualifier of the form: ColumFamily:qualifier      for instance Properties:nm
	cJSON *cq = cJSON_GetArrayItem (contents, 0);
	string tmp_cq = base64_decode(cq->valuestring);
	// printf( "      elem(decod) = %s\n", tmp_cq.c_str());
	cJSON *value = cJSON_GetArrayItem (contents, 2);
	string tmp_value = base64_decode( value->valuestring);
	// printf( "      elem(decod) = %s\n", tmp_value.c_str());
	if ( tmp_cq == "Properties:nm") {
	  model_info.__set_name( tmp_value);
	  // printf( "--> added\n");
	} else if ( tmp_cq == "Properties:fp") {
	  model_info.__set_full_path( tmp_value);
	  // printf( "--> added\n");
	}
      }
      listOfModelNames.push_back( model_info);
    }
  }
  // fflush( stdout);
  if ( !there_are_models) {
    report = "No models";
  }
  return there_are_models;
}

void printRow(const TRowResult &rowResult) {
  char hex_string[ 1024];
  std::cout << "row: " << ModelID_DoHexStringConversionIfNecesary( rowResult.row, hex_string, 1024) << ", cols: ";
  for (CellMap::const_iterator it = rowResult.columns.begin(); 
      it != rowResult.columns.end(); ++it) {
    std::cout << "   col_value = ( " << it->first << " , " << it->second.value << " )" << endl;
  }
}

static bool getModelInfoFromRow( FullyQualifiedModelName &model_info, const TRowResult &rowResult,
				 const std::string &table_name) {
  model_info.__set_modelID( rowResult.row); // the key is the modelID;

  int num_filled = 1; // key has already been added
  for (CellMap::const_iterator it = rowResult.columns.begin(); 
       it != rowResult.columns.end(); ++it) {
    if ( it->first == "Properties:nm") {
      model_info.__set_name( it->second.value);
      num_filled++; // another value
    } else if ( it->first == "Properties:fp") {
      model_info.__set_full_path( it->second.value);
      num_filled++; // another value
    }
  }
  model_info.__set_location( "Hbase:" + table_name);
  return num_filled == 3;
}

std::string HBase::getListOfModelNames_curl( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					     const std::string &sessionID, const std::string &model_group_qualifier, 
					     const std::string &model_name_pattern) {
  LOGGER_SM << "getListOfModelNames CURL : =====" << std::endl;
  LOGGER_SM << "S " << sessionID              << std::endl;
  LOGGER_SM << "G " << model_group_qualifier  << std::endl; // in Hbase we use it as full_path_pattern ( Properties:fp)
  LOGGER_SM << "P " << model_name_pattern     << std::endl; // model name pattern ( Properties:nm)

  string table_name;
  if ( !model_group_qualifier.size() || ( model_group_qualifier == "*")) {
    table_name = "VELaSSCo_Models";
  } else {
    table_name = model_group_qualifier;
  }

  string cmd = "http://" + _db_host + ":8880/";
  cmd += table_name + "/";

  std::stringstream key;
  key << "*"; // we have to parse through all models
  cmd += key.str();
  cout << cmd << endl;
  
  CurlCommand do_curl;
  string buffer;
  
  bool ok = do_curl.Evaluate( buffer, cmd);
  string result;
  if ( ok) {
    LOGGER_SM << "**********\n";    
    LOGGER_SM << buffer << std::endl;
    LOGGER_SM << "**********\n";
    LOGGER_SM << "WARNING: the actual parsing of the name pattern is not being done !!!" << std::endl;
    bool there_are_models = parseListOfModelNames( report, listOfModelNames, buffer, table_name);
    if ( there_are_models) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    LOGGER_SM << "ERROR**********\n";    
    LOGGER_SM << buffer << std::endl;
    LOGGER_SM << "ERROR**********\n";
    result = "Error";
    report = "HBase::getListOfModelNames CURL could not evaluate command.";
  }

  return result;
}

bool HBase::getListOfModelInfoFromTables( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					  const std::string &table_name, const std::string &model_name_pattern) {
  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  // TScan ts;
  // std::stringstream filter;
  // filter.str("");
  // ts.__set_filterString(filter.str());
  StrVec cols;
  cols.push_back( "Properties:nm"); // name of model
  cols.push_back( "Properties:fp"); // full path
  ScannerID scan_id = _hbase_client->scannerOpen( table_name, "", cols, m);
  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);
  bool scan_ok = true;
  try {
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
	break;
      // process rowsResult
      // LOGGER_SM << "number of rows = " << rowsResult.size() << endl;
      for ( size_t i = 0; i < rowsResult.size(); i++) {
	// convert to return type
	FullyQualifiedModelName model_info;
	bool ok = getModelInfoFromRow( model_info, rowsResult[ i], table_name);
	if ( ok) {
	  listOfModelNames.push_back( model_info);
	}
      }
    }
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << report << std::endl;
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << report << std::endl;
  }
  _hbase_client->scannerClose( scan_id);

  return scan_ok;

}

std::string HBase::getListOfModelNames_thrift( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					       const std::string &sessionID, const std::string &model_group_qualifier, 
					       const std::string &model_name_pattern) {
  LOGGER_SM << "getListOfModelNames THRIFT: =====" << std::endl;
  LOGGER_SM << "S " << sessionID              << std::endl;
  LOGGER_SM << "G " << model_group_qualifier  << std::endl; // in Hbase we use it as full_path_pattern ( Properties:fp)
  LOGGER_SM << "P " << model_name_pattern     << std::endl; // model name pattern ( Properties:nm)

  string table_name;
  bool scan_ok = true;
  bool print_existing_tables = false;
  std::vector< Text> lst_tables;
  LOGGER_SM << "getListOfModelNames THRIFT: 00=====" << std::endl;
  _hbase_client->getTableNames(lst_tables);
  if ( !model_group_qualifier.size() || ( model_group_qualifier == "*")) {
    table_name = "VELaSSCo_Models";
    // loop through all 4 tables:
    std::vector< std::string> lst_model_tables = getModelListTables();
    for ( std::vector< std::string>::const_iterator it = lst_model_tables.begin(); it != lst_model_tables.end(); it++) {
      // check if it exists and it is enabled:
      bool found = false;
      const std::string &model_table = *it;
      for ( std::vector< Text>::const_iterator itbl = lst_tables.begin(); itbl != lst_tables.end(); itbl++) {
		if ( model_table.compare( *itbl) == 0) {
		  // check if enabled:
		  if ( _hbase_client->isTableEnabled( model_table)) {
			found = true;
			break;
		  }
		}
      }
      if ( !found)
		continue;
      bool my_scan_ok = getListOfModelInfoFromTables( report, listOfModelNames, *it, model_name_pattern);
      scan_ok |= my_scan_ok;
    }
    print_existing_tables = true;
  } else {
    table_name = model_group_qualifier;
    scan_ok = getListOfModelInfoFromTables( report, listOfModelNames, table_name, model_name_pattern);
  }

  string result;
  if ( scan_ok) {
    LOGGER_SM << "**********\n";
    LOGGER_SM << "WARNING: the actual parsing of the name pattern is not being done !!!" << std::endl;
    bool there_are_models = listOfModelNames.size();
    if ( there_are_models) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    LOGGER_SM << "ERROR**********\n";
    result = "Error";
    report = "HBase::getListOfModelNames THRIFT could not scan.";
    if ( print_existing_tables) {
      // report tables found:
      LOGGER_SM << "Tables found:" << std::endl;
      for ( std::vector< Text>::const_iterator it = lst_tables.begin(); it != lst_tables.end(); it++) {
	LOGGER_SM << " " << *it << ",";
      }
      LOGGER_SM << std::endl;
    }
  }

  return result;
}

std::string HBase::getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					const std::string &sessionID, const std::string &model_group_qualifier, 
					const std::string &model_name_pattern) {
  // return HBase::getListOfModelNames_curl( report, listOfModelNames,
  // 					  sessionID, model_group_qualifier, model_name_pattern);
  return HBase::getListOfModelNames_thrift( report, listOfModelNames,
					    sessionID, model_group_qualifier, model_name_pattern);
}



std::vector< std::string> HBase::getModelListTables() const {
  std::vector< std::string> lst;
  lst.push_back( "VELaSSCo_Models");
  lst.push_back( "VELaSSCo_Models_V4CIMNE");
  lst.push_back( "VELaSSCo_Models_V4CIMNE_Test");
  lst.push_back( "Test_VELaSSCo_Models");
  lst.push_back( "T_VELaSSCo_Models");
  return lst;
}

bool HBase::storeTableNames( const std::string &sessionID, const std::string &modelID, 
			     const std::string &model_table_name) {
  const DicTableModels::iterator it = _table_models.find( sessionID + modelID);
  if ( it != _table_models.end()) {
    // if present delete it ....
    _table_models.erase( it);
  }

  std::string metadata_table_name = "", data_table_name = "";
  std::string stored_vquery_metadata_table_name = "", stored_vquery_data_table_name = "";
  bool known = true;

  if ( model_table_name == "VELaSSCo_Models") {
    metadata_table_name = "Simulations_Metadata";
    data_table_name = "Simulations_Data";
    stored_vquery_metadata_table_name = "Simulations_VQuery_Results_Metadata";
    stored_vquery_data_table_name = "Simulations_VQuery_Results_Data";

  } else if ( model_table_name == "VELaSSCo_Models_V4CIMNE") {
    metadata_table_name = "Simulations_Metadata_V4CIMNE";
    data_table_name = "Simulations_Data_V4CIMNE";
    stored_vquery_metadata_table_name = "Simulations_VQuery_Results_Metadata_V4CIMNE";
    stored_vquery_data_table_name = "Simulations_VQuery_Results_Data_V4CIMNE";

  } else if ( model_table_name == "VELaSSCo_Models_V4CIMNE_Test") {
    metadata_table_name = "Simulations_Metadata_V4CIMNE_Test";
    data_table_name = "Simulations_Data_V4CIMNE_Test";
    stored_vquery_metadata_table_name = "Simulations_VQuery_Results_Metadata_V4CIMNE_Test";
    stored_vquery_data_table_name = "Simulations_VQuery_Results_Data_V4CIMNE_Test";

  } else if ( model_table_name == "Test_VELaSSCo_Models") {
    metadata_table_name = "Test_Simulations_Metadata";
    data_table_name = "Test_Simulations_Data";
    stored_vquery_metadata_table_name = "Test_Simulations_VQuery_Results_Metadata";
    stored_vquery_data_table_name = "Test_Simulations_VQuery_Results_Data";

  } else if ( model_table_name == "T_VELaSSCo_Models") {
    metadata_table_name = "T_Simulations_Metadata";
    data_table_name = "T_Simulations_Data";
    stored_vquery_metadata_table_name = "T_Simulations_VQuery_Results_Metadata";
    stored_vquery_data_table_name = "T_Simulations_VQuery_Results_Data";

  } else {
    known = false;
  }
  if ( known) {
    TableModelEntry entry = { model_table_name, metadata_table_name, data_table_name,
			      stored_vquery_metadata_table_name, stored_vquery_data_table_name};
    _table_models[ sessionID + modelID] = entry;
  }
  return known;
}


std::string HBase::findModel( std::string &report, std::string &return_modelID, 
			      FullyQualifiedModelName &return_model_info,
			      const std::string &sessionID, const std::string &unique_model_name_pattern, 
			      const std::string &requested_access) {
  /* unique_mode_name_pattern is of the form  (hbase = TableName:Properties-fp:Properties-nm) */
  // strip "unique_model_name_pattern" down to
  // TableName:model_path:model_name
  // model_path corresponds to Properties:fp
  // model_name corresponds to Properties:nm

  bool select_first_model = false;
  bool select_first_path = false;
  char *table_to_use = NULL;
  char *path_to_search = NULL;
  char *name_to_search = NULL;
  char *separator = NULL;
  if ( unique_model_name_pattern.length() == 0) {
    select_first_model = true;
    select_first_path = true;
    table_to_use = strdup( "VELaSSCo_Models");
    // may be look through all 4 tables: like in getListOfModels
    // std::vector< std::string> lst_tables = getModelListTables();
    // for ( std::vector< std::string>::const_iterator it = lst_tables.begin(); it != lst_tables.end(); it++) {
    //   scan_ok = getListOfModelInfoFromTables( report, listOfModelNames, *it, model_name_pattern);
    //   if ( !scan_ok)
    // 	break;
    // }
  } else { // process unique_model_name_pattern
    table_to_use = strdup( unique_model_name_pattern.c_str()); // to have enough space ...
    path_to_search = strdup( unique_model_name_pattern.c_str()); // to have enough space ...
    name_to_search = strdup( unique_model_name_pattern.c_str()); // to have enough space ...

    // options: ModelName, /Full/Path:ModelName or TableName:/Full/Path:ModelName

    separator = strchr( table_to_use, ':');
    if ( !separator) { // unique_model_name_pattern == ModelName
      select_first_path = true;
    } else {
      char *separator2 = strchr( separator + 1, ':');
      if ( !separator2) { // unique_model_name_pattern == /Full/Path:ModelName
	free( table_to_use);
	table_to_use = strdup( "VELaSSCo_Models");
	separator = strchr( path_to_search, ':');
	*separator = '\0';
	separator++;
	strcpy( name_to_search, separator);
      } else { // unique_model_name_pattern == TableName:/Full/Path:ModelName
	*separator = '\0';
	separator++;
	strcpy( path_to_search, separator);
	separator2 = strchr( path_to_search, ':');
	*separator2 = '\0';
	separator2++;
	strcpy( name_to_search, separator2);
      }
    }
  } // end processing unique_model_name_pattern
  if ( name_to_search && !strcasecmp( name_to_search, "*")) {
    select_first_model = true;
  }
  if ( path_to_search && !strcasecmp( path_to_search, "*")) {
    select_first_path = true;
  }

  std::string modelID_to_return;
  FullyQualifiedModelName model_info_to_return;
  LOGGER_SM << "findModel THRIFT: ====="       << std::endl;
  LOGGER_SM << "S " << sessionID                 << std::endl;
  LOGGER_SM << "U " << unique_model_name_pattern << std::endl;
  LOGGER_SM << "A " << requested_access << std::endl;
  LOGGER_SM << "T " << table_to_use                << std::endl; // table name
  LOGGER_SM << "P " << path_to_search                << std::endl; // model path ( Properties:fp)
  LOGGER_SM << "N " << name_to_search                << std::endl; // model name ( Properties:nm)
  LOGGER_SM << "WARNING: StorageModule::FindModel requested_access not used at the moment." << std::endl;

  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  // TScan ts;
  // std::stringstream filter;
  // filter.str("");
  // ts.__set_filterString(filter.str());
  StrVec cols;
  cols.push_back( "Properties:nm"); // name of model
  cols.push_back( "Properties:fp"); // full path
  ScannerID scan_id = _hbase_client->scannerOpen( table_to_use, "", cols, m);
  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_to_use, ts, m);
  bool scan_ok = true;
  bool found = false;
  bool there_are_models = false; // no conform with the vquery and operation forms
  try {
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( !found) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
	break;
      // process rowsResult
      // LOGGER_SM << "number of rows = " << rowsResult.size() << endl;
      for ( size_t i = 0; i < rowsResult.size(); i++) {
	// convert to return type
	FullyQualifiedModelName model_info;
	bool ok = getModelInfoFromRow( model_info, rowsResult[ i], table_to_use);
	if ( ok) {
	  if ( select_first_path || ( model_info.full_path == path_to_search)) {
	    if ( select_first_model || ( model_info.name == name_to_search)) {
	      modelID_to_return = model_info.modelID;
	      model_info_to_return = model_info;
	      found = true;
	      break;
	    }
	  }
	  there_are_models = true;
	}
      }
    }
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << report << std::endl;
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << report << std::endl;
  }
  _hbase_client->scannerClose( scan_id);

  string result;
  if ( scan_ok) {
    LOGGER_SM << "**********\n";
    LOGGER_SM << "WARNING: the actual parsing of the name pattern is not being done !!!" << std::endl;
    if ( found) {
      // let's store the info and check if tablename is known to retrieve the data
      bool known_table_name = storeTableNames( sessionID, modelID_to_return, table_to_use);
      // needs to be deleted when logout or close model ....
      if ( known_table_name) {
	result = "Ok";
	return_modelID = modelID_to_return;
	return_model_info = model_info_to_return;
      } else {
	result = "Error";
	report = "Unknown table name: " + std::string( table_to_use ? table_to_use : "N/A");
      }
    } else {
      result = "Error";
      if ( there_are_models) {
	report = "Not found";
      } else {
	report = "No models";
      }
    }
  } else {
    LOGGER_SM << "ERROR**********\n";
    result = "Error";
    report = "HBase::findModel THRIFT could not scan.";
  }

  // cout << "*** Result = " << result << endl;
  // cout << "*** Report = " << report << endl;

  free( table_to_use);
  free( path_to_search);

  return result;
}
