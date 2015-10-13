/* -*- c++ -*- */
#ifndef STORAGEMODULE_H
#define STORAGEMODULE_H

#include <iostream> // pour std::cout
#include <string>   // pour std::string
#include <vector>
#include <random>
#include <math.h> 

/////////////////////////////////
// Base 64
/////////////////////////////////
#include "base64.h"

#include "AbstractDB.h"

using namespace std;
class FullyQualifiedModelName;

typedef enum {
  DL_SM_DB_UNKNOWN = 0,
  DL_SM_DB_HBASE = 1,
  DL_SM_DB_EDM = 2
} DL_SM_DB_TYPE;

inline const char *getStringFromDBType( DL_SM_DB_TYPE db) {
  const char *ret = "N/A";
  switch( db) {
  case DL_SM_DB_UNKNOWN:  ret = "unknown"; break;
  case DL_SM_DB_HBASE:    ret = "hbase"; break;
  case DL_SM_DB_EDM:      ret = "edm"; break;
  default:                ret = "N/A"; break;
  }
  return ret;
}

inline DL_SM_DB_TYPE getDBTypeFromString( const char *str) {
  DL_SM_DB_TYPE ret = DL_SM_DB_UNKNOWN;
  if ( !strcasecmp( str, "hbase")) {
    ret = DL_SM_DB_HBASE;
  } else if ( !strcasecmp( str, "edm")) {
    ret = DL_SM_DB_EDM;
  }
  return ret;
}

class storageModule
{
public:
  static storageModule* Instance();

  bool startConnection( DL_SM_DB_TYPE db_type, const char *DB_hostname, const int DB_port);
  bool stopConnection();

  string getStatusDB();
  string getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
			      const std::string &sessionID, const std::string &model_group_qualifier, 
			      const std::string &model_name_pattern);
  string findModelFS( std::string &report, std::string &modelID, 
		      const std::string &sessionID, const std::string &unique_model_name_pattern);
  string getResultOnVertices( std::string sessionID,
			      std::string modelID,
			      std::string analysisID,
			      double timeStep,
			      std::string resultID,
			      std::string listOfVertices);
    
  string checkIfAllVerticesArePresent(std::string listOfVertices, string contentToCheck);

private:
  storageModule(): _db( NULL) {};
  ~storageModule() { stopConnection();}

  // storageModule(storageModule const&){};
  // storageModule& operator=(storageModule const&){};
  static storageModule* m_pInstance;
  double fRand(double fMin, double fMax);
    
  string parse1DEM(string, string);

  VELaSSCo::AbstractDB *_db;
};

#endif
