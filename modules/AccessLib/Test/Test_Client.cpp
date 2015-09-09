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

int main(int argc, char* argv[])
{
	VAL_Result    result;
	VAL_SessionID sessionID;

	//
	// Test UserLogin()
	//

	char hostname_port[ 1024];
	if ( argc == 2) {
	  if ( !strncasecmp( argv[ 1], "-h", 2) ||  !strncasecmp( argv[ 1], "--h", 3)) {
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

	const char*   modelID     = "myModelID";
	const char*   resultID    = "myResultID";
	const char*   analysisID  = "myAnalysisID";
	const int64_t vertexIDs[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
	const double  timeStep    = 100;

	const int64_t* resultVertexIDs;
	const double*  resultValues;
	size_t         resultNumVertices;

	result = valGetResultFromVerticesID(sessionID, "myModelID",
		                                           "myResultID",
												   "myAnalysisID",
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
