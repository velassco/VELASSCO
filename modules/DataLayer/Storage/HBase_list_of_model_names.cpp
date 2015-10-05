
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

#include "cJSON.h"
#include "base64.h"

using namespace std;
using namespace VELaSSCo;

/////////////////////////////////
// Curl + Json
/////////////////////////////////
#include <curl/curl.h>
#include "cJSON.h"

std::string HBase::getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					const std::string &sessionID, const std::string &model_group_qualifier, 
					const std::string &model_name_pattern) {
  std::cout << "getListOfModelNames: =====" << std::endl;
  std::cout << "S " << sessionID              << std::endl;
  std::cout << "G " << model_group_qualifier  << std::endl; // in Hbase we use it as full_path_pattern ( Properties:fp)
  std::cout << "P " << model_name_pattern     << std::endl; // model name pattern ( Properties:nm)
	
  string cmd = "http://pez001:8880/";
  cmd += "VELaSSCo_Models";
  cmd += "/";

  std::stringstream key;
  key << "*"; // we have to parse through all models
  cmd += key.str();
  cout << cmd << endl;

  report = "HBase::getListOfModelNames not implemented.";
  return "Error";
}


