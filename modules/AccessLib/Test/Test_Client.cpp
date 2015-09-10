#include <stdio.h>
#include <string.h>

// CLib
#include <cstdlib>

// STL
#include <iostream>

// VELaSSCo
#include "AccessLib.h"

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
  if ( argc == 2) {
    if ( askForHelp( argv[ 1])) {
      printf( "Usage: %s [ hostname [ port]]\n", argv[ 0]);
      return EXIT_FAILURE;
    }
    sprintf( hostname_port, "%s:9090", argv[ 1]);
  } else if ( argc == 3) {
    sprintf( hostname_port, "%s:%s", argv[ 1], argv[ 2]);
  } else {
    strcpy( hostname_port, "localhost:9090");
  }

  printf( "Connecting to '%s' ...\n", hostname_port);

  result = valUserLogin( hostname_port, "andreas", "1234", &sessionID);
  CheckVALResult(result);

  //
  // Test GetResultFromVerticesID()
  //

  const char*   modelID     = "d94ca29be534ca1ed578e90123b7"; // current DEM_box example in VELaSSCo_Models as of 10.11.2015, two days ago there where two models !
  const char*   resultID    = "MASS";
  const char*   analysisID  = "DEM";
  const int64_t vertexIDs[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
  const double  timeStep    = 10000.0;

  const int64_t* resultVertexIDs;
  const double*  resultValues;
  size_t         resultNumVertices;

  // This call does not comply with the VQuery form, but in the meantime ...
  result = valGetResultFromVerticesID(sessionID, modelID,
                                      resultID,
                                      analysisID,
                                      vertexIDs,
                                      timeStep,
                                      &resultVertexIDs,
                                      &resultValues,
                                      &resultNumVertices);
  CheckVALResult(result);

  //
  // Print received data
  //

  for (size_t i=0; i<resultNumVertices; i++)
  {
    std::cout << "Vertex: " << i;
    std::cout << "  ID: " << resultVertexIDs[i];
    std::cout << "  Values: [";
    for (size_t j=0; j<3; j++)
      std::cout << " " << resultValues[3*i+j];
    std::cout << " ]" << std::endl;
  }

  //
  // Test UserLogout()
  //

  result = valUserLogout(sessionID);
  CheckVALResult(result);

  return EXIT_SUCCESS;
}
