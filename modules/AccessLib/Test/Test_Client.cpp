#include <stdio.h>
#include <string.h>

// CLib
#include <cstdlib>

// STL
#include <iostream>

// VELaSSCo
#include "AccessLib.h"

// from Helpers.h
  // returns NULL if dst_len is too short, otherwise return dst
  static const char *ToHexString( char *dst, size_t dst_len, const char *src, const size_t src_len) {
    if ( !dst) return NULL;
    int isrc = 0;
    for ( int idst = 0; 
	  ( isrc < src_len) && ( idst < dst_len - 1); 
	  isrc++, idst += 2) {
      sprintf( &dst[ idst], "%02x", ( unsigned char)src[ isrc]);
    }
    // if all chars converted, then return dst
    return ( isrc == src_len) ? dst : NULL;
  }
  static std::string ModelID_DoHexStringConversionIfNecesary( const std::string &modelID, char *tmp_buf, size_t size_tmp_buf) {
    if ( modelID.length() == 16) {
      return ( std::string) ToHexString( tmp_buf, size_tmp_buf, modelID.c_str(), modelID.size());
    } else {
      return modelID;
    }
  }

void CheckVALResult(VAL_Result result)
{
  if (result != VAL_SUCCESS)
  {
    const char* message;
    valErrorMessage(result, &message);

    std::cout << "VELaSSCo ERROR: " << std::endl;
    std::cout << "  " << message    << std::endl;

    exit(EXIT_FAILURE);
  }
}


#ifdef _WIN32
#ifndef strcasecmp
#define strcasecmp  _stricmp
#endif
#endif // _WIN32

bool askForHelp( const char *txt) {
  return !strcasecmp( txt, "-h") || !strcasecmp( txt, "--h") || !strcasecmp( txt, "-help") || !strcasecmp( txt, "--help");
}


int main(int argc, char* argv[])
{
  VAL_Result    result;
  VAL_SessionID sessionID;

  //
  // Test UserLogin()
  //

  char hostname_port[ 1024];
  int remote_port = 9990; // default port
  int velassco_default_port  = 26267;
  if ( argc == 2) {
    if ( askForHelp( argv[ 1])) {
      printf( "Usage: %s [ hostname [ port (default %d, VELaSSCo EngineLayer/QueryManager default port = %d)]]\n", 
	      argv[ 0], remote_port, velassco_default_port);
      return EXIT_FAILURE;
    }
    sprintf( hostname_port, "%s:%d", argv[ 1], remote_port);
  } else if ( argc == 3) {
    sprintf( hostname_port, "%s:%s", argv[ 1], argv[ 2]);
  } else {
    strcpy( hostname_port, "localhost:9990");
  }

  printf( "Connecting to '%s' ...\n", hostname_port);

  result = valUserLogin( hostname_port, "andreas", "1234", &sessionID);
  CheckVALResult(result);

  // Test StatusDB

  const char *status = NULL;
  result = valGetStatusDB( sessionID, &status);
  CheckVALResult(result);
  std::cout << "status = " << status << std::endl;

  //
  // Test GetListOfModels()
  //
  const char *return_list = NULL;
  const char *group_qualifier = "";
  const char *name_pattern = "*";
  result = valGetListOfModels( sessionID, group_qualifier, name_pattern, &status, &return_list);
  CheckVALResult(result);
  std::cout << "in VELaSSCo_models:" << std::endl;
  std::cout << "   status = " << status << std::endl;
  std::cout << "   model_list = " << return_list << std::endl;
  group_qualifier = "Test_VELaSSCo_Models";
  result = valGetListOfModels( sessionID, group_qualifier, name_pattern, &status, &return_list);
  CheckVALResult(result);
  std::cout << "in VELaSSCo_Models_V4CIMNE:" << std::endl;
  std::cout << "   status = " << status << std::endl;
  std::cout << "   model_list = " << return_list << std::endl;

  // result = valGetStatusDB( sessionID, &status);
  // CheckVALResult(result);
  // std::cout << "status = " << status << std::endl;

  // 
  // Test OpenModel

  // const char *unique_name = ""; // can be empty to get the first one
  // const char *unique_name = "Test_VELaSSCo_Models:"; // or using only the table's name and get the first one
  const char *unique_name = "VELaSSCo_Models:/localfs/home/velassco/common/simulation_files/DEM_examples/FluidizedBed_small.p3c"; // at the moment, as Properties::nm are not unique we'll use Properties:fp
  const char *access = "";
  const char *return_modelID = NULL;
  result = valOpenModel( sessionID, unique_name, access, &status, &return_modelID);
  std::cout << "OpenModel: " << std::endl;
  std::cout << "   status = " << status << std::endl;
  char hex_string[ 1024];
  std::cout << "   model_modelID = " << ModelID_DoHexStringConversionIfNecesary( return_modelID, hex_string, 1024) << std::endl;

  //
  // Test GetResultFromVerticesID()
  //

  // const char*   modelID     = "d94ca29be534ca1ed578e90123b7"; // current DEM_box example in VELaSSCo_Models as of 10.11.2015, two days ago there where two models !
  // const char*   resultID    = "MASS";
  // const char*   analysisID  = "DEM";
  // const int64_t vertexIDs[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
  // const double  timeStep    = 10000.0;
  // 
  // const int64_t* resultVertexIDs;
  // const double*  resultValues;
  // size_t         resultNumVertices;
  // 
  // // This call does not comply with the VQuery form, but in the meantime ...
  // result = valGetResultFromVerticesID(sessionID, modelID,
  //                                     resultID,
  //                                     analysisID,
  //                                     vertexIDs,
  //                                     timeStep,
  //                                     &resultVertexIDs,
  //                                     &resultValues,
  //                                     &resultNumVertices);
  // CheckVALResult(result);
  // 
  // //
  // // Print received data
  // //
  // 
  // for (size_t i=0; i<resultNumVertices; i++)
  // {
  //   std::cout << "Vertex: " << i;
  //   std::cout << "  ID: " << resultVertexIDs[i];
  //   std::cout << "  Values: [";
  //   for (size_t j=0; j<3; j++)
  //     std::cout << " " << resultValues[3*i+j];
  //   std::cout << " ]" << std::endl;
  // }

  //
  // Test UserLogout()
  //

  result = valUserLogout(sessionID);
  CheckVALResult(result);

  return EXIT_SUCCESS;
}
