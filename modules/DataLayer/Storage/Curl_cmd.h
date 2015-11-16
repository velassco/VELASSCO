/* -*- c++ -*- */
// CLib
#include <cmath>
#include <cstddef>

// STD
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

// Curl
#include <curl/curl.h>

// VELaSSCo
#include "HBase.h"

#include "cJSON.h"
#include "base64.h"

class CurlCommand {
 public:
  CurlCommand() {
    *m_errorBuffer = '\0';
    m_buffer = "";
  }
  bool Evaluate( std::string &result, const std::string &cmd);
  const char *getErrorBuffer() const { return m_errorBuffer;}
  

 private:
  static int writer(char *data, size_t size, size_t nmemb,
		    std::string *writerData) {
    if ( writerData == NULL)
      return 0;
    writerData->append(data, size*nmemb);
    return size * nmemb;
  }

 private:  
  char m_errorBuffer[ CURL_ERROR_SIZE];
  std::string m_buffer;
};

inline bool CurlCommand::Evaluate( std::string &result, const std::string &cmd) {
  bool curl_ok = false;
  try {    
    CURL *curl = NULL;
    CURLcode res;
    
    *m_errorBuffer = '\0';
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
      res = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, m_errorBuffer);
      if(res != CURLE_OK) {
	fprintf(stderr, "CURLOPT_ERRORBUFFER failed: %s\n", curl_easy_strerror(res));
      }
    
      res = curl_easy_setopt(curl, CURLOPT_URL, cmd.c_str());
      if(res != CURLE_OK) {
	fprintf(stderr, "CURLOPT_URL failed: %s\n", curl_easy_strerror(res));
      }
    
      res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      if(res != CURLE_OK) {
	fprintf(stderr, "CURLOPT_FOLLOWLOCATION failed: %s\n", curl_easy_strerror(res));
      }
      res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlCommand::writer);
      if(res != CURLE_OK) {
	fprintf(stderr, "CURLOPT_WRITEFUNCTION failed: %s\n", curl_easy_strerror(res));
      }
    
      res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_buffer);
      if(res != CURLE_OK) {
	fprintf(stderr, "CURLOPT_WRITEDATA failed: %s\n", curl_easy_strerror(res));
      }
    
      struct curl_slist *chunk = NULL;
      chunk = curl_slist_append(chunk, "Accept: application/json;");
      res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
      if(res != CURLE_OK) {
	fprintf(stderr, "CURLOPT_HTTPHEADER failed: %s\n", curl_easy_strerror(res));
      }
    
      res = curl_easy_perform(curl);
      if(res != CURLE_OK) {
	fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      }
    
      curl_easy_cleanup(curl);
      curl_slist_free_all(chunk);   
    }
    if ( res == CURLE_OK) {
      result = m_buffer;
    }
    
    curl_ok = ( res == CURLE_OK);
  } catch (...) {
    curl_ok = false;
    result = "ERROR: Curl error evaluating " + cmd;
  }
  return curl_ok;
}
  
inline char *until_comma( char *str) {
  for ( char *c = str; c && *c; c++) {
    if ( *c == ',') {
      *c = '\0';
      break;
    }
  }
  return str;
}
