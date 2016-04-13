
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

#include "Curl_cmd.h"

using namespace std;
using namespace VELaSSCo;

/////////////////////////////////
// Curl + Json
/////////////////////////////////
#include <curl/curl.h>
#include "cJSON.h"

std::string HBase::parseStatusDB( std::string b) {
  std::stringstream result;

  cJSON *json = cJSON_Parse( b.c_str());
  if ( !json) 
    return "";
  cJSON *live = cJSON_GetObjectItem( json, "LiveNodes");
  cJSON *dead = cJSON_GetObjectItem( json, "DeadNodes");

  int num_live = live ? cJSON_GetArraySize( live) : 0;
  int num_dead = dead ? cJSON_GetArraySize( dead) : 0;

  result << num_live + num_dead << " servers: " << num_live << " live and " << num_dead << " dead." << endl;

  result << "        " << num_live << " live servers: ";
  for ( int i = 0; i < num_live; i++) {
    cJSON *server = cJSON_GetArrayItem( live, i);
    if ( server) {
      result << cJSON_GetObjectItem( server, "name")->valuestring;
    } else {
      result << "Unknown server";
    }
    if ( i < num_live - 1) result << ", ";
    else result << endl;
  }
  result << "        " << num_dead << " dead servers: ";
  for ( int i = 0; i < num_dead; i++) {
    cJSON *server = cJSON_GetArrayItem( dead, i);
    if ( server) {
      // result << cJSON_GetObjectItem( server, "name")->valuestring;
      result << until_comma( server->valuestring);
    } else {
      result << "Unknown server";
    }
    if ( i < num_dead - 1) result << ", ";
    else result << endl;
  }
  cJSON_Delete( json);

  return result.str();

}

std::string HBase::getStatusDB() {
  // return "HBase::Status\n";
  // http://pez001:8880/status/cluster
  LOGGER_SM << "getStatusDB: =====" << std::endl;
  string cmd = "http://" + _db_host + ":8880/";
  cmd += "status/cluster";
  cout << cmd << endl;

  CurlCommand do_curl;
  string buffer;
  
  bool ok = do_curl.Evaluate( buffer, cmd);
  
  string result;
  if ( ok) {
    // LOGGER_SM << "**********\n";    
    // LOGGER_SM << buffer << std::endl;
    // LOGGER_SM << "**********\n";    
    
    result = parseStatusDB( buffer);
  } else {
    LOGGER_SM << "ERROR**********\n";    
    LOGGER_SM << buffer << std::endl;
    LOGGER_SM << "ERROR**********\n";    
    result = buffer;
  }
  return result;

}

	
