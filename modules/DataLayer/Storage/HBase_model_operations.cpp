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
#include "VELaSSCo.h" // for class FullyQualifiedModelName
#include "cJSON.h"
#include "base64.h"

#include "Curl_cmd.h"
#include "Helpers.h"

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
				   std::string buffer) {
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
      cJSON * elem  = cJSON_CreateObject();
      // printf( "keyC = %s\n", keyC->valuestring);
      // printf( "keyJ(decod) = %s", Hexdump( key).c_str());

      FullyQualifiedModelName model_info;
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

std::string HBase::getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					const std::string &sessionID, const std::string &model_group_qualifier, 
					const std::string &model_name_pattern) {
  std::cout << "getListOfModelNames: =====" << std::endl;
  std::cout << "S " << sessionID              << std::endl;
  std::cout << "G " << model_group_qualifier  << std::endl; // in Hbase we use it as full_path_pattern ( Properties:fp)
  std::cout << "P " << model_name_pattern     << std::endl; // model name pattern ( Properties:nm)
	
  string cmd = "http://pez001:8880/";
  if ( !model_group_qualifier.size() || ( model_group_qualifier == "*")) {
    cmd += "VELaSSCo_Models";
  } else {
    cmd += model_group_qualifier;
  }
  cmd += "/";

  std::stringstream key;
  key << "*"; // we have to parse through all models
  cmd += key.str();
  cout << cmd << endl;
  
  CurlCommand do_curl;
  string buffer;
  
  bool ok = do_curl.Evaluate( buffer, cmd);
  string result;
  if ( ok) {
    std::cout << "**********\n";    
    std::cout << buffer << std::endl;
    std::cout << "**********\n";
    std::cout << "WARNING: the actual parsing of the name pattern is not being done !!!" << std::endl;
    bool there_are_models = parseListOfModelNames( report, listOfModelNames, buffer);
    if ( there_are_models) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    std::cout << "ERROR**********\n";    
    std::cout << buffer << std::endl;
    std::cout << "ERROR**********\n";
    result = "Error";
    report = "HBase::getListOfModelNames could not evaluate CURL command.";
  }

  return result;
}


