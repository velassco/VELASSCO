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

  std::string getStatusDB();
  std::string getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
			      const std::string &sessionID, const std::string &model_group_qualifier, 
			      const std::string &model_name_pattern);
  std::string findModel( std::string &report, std::string &modelID, FullyQualifiedModelName &model_info,
		    const std::string &sessionID, const std::string &unique_model_name_pattern, 
		    const std::string &requested_access);
  string getResultOnVertices( std::string sessionID,
			      std::string modelID,
			      std::string analysisID,
			      double timeStep,
			      std::string resultID,
			      std::string listOfVertices);
  std::string getListOfMeshes( std::string &report, std::vector< MeshInfo> &listOfMeshes,
			  const std::string &sessionID, const std::string &modelID,
			  const std::string &analysisID, const double stepValue);
  std::string getListOfAnalyses( std::string &report, std::vector< std::string> &listOfAnalyses,
				 const std::string &sessionID, const std::string &modelID);
    
  std::string checkIfAllVerticesArePresent(std::string listOfVertices, string contentToCheck);

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
