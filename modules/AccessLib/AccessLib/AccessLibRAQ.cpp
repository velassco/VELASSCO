
// STL
#include <iostream>
#include <sstream>
#include <string>

// VELaSSCo
#include "AccessLib.h"
#include "Client.h"
#include "Helpers.h"
#include "DemoServer.h" // for the StartServer() function
#include "base64.h"

#include "AccessLibCommon.h"

void dumpVQueryResult( const char *file_name, const char *bytes, size_t len_bytes) {
  FILE *fo = fopen( file_name, "wb");
  // size_t num_written_bytes = 
  fwrite( bytes, len_bytes, sizeof( char), fo);
  fclose( fo);
  // assert( num_written_bytes == len_bytes);
  char tmp[ 1024];
  sprintf( tmp, "%g KBytes written.", ( double)len_bytes / 1024.0);
  DEBUG( std::string( "dumpVQueryResult '") + file_name + "' done: " + tmp);
}

/*
 * 2xx RAQ - Result Analysis Queries
 */

#ifdef __cplusplus
extern "C" {
#endif

  VAL_Result VAL_API valGetResultFromVerticesID( /* in */
						VAL_SessionID   sessionID,
						const char*     modelID,
						const char*     resultID,
						const char*     analysisID,
						const int64_t*  vertexIDs,
						double          timeStep,

						/* out */
            const char*    *result_status,
						const int64_t* *resultVertexIDs,
						const double*  *resultValues,
						size_t         *resultNumVertices )
  {
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( modelID );
    CHECK_QUERY_POINTER( resultID );
    CHECK_QUERY_POINTER( analysisID );
    CHECK_QUERY_POINTER( vertexIDs );
    CHECK_QUERY_POINTER( resultVertexIDs );
    CHECK_QUERY_POINTER( resultValues );
    CHECK_QUERY_POINTER( resultNumVertices );
    CHECK_QUERY_POINTER( result_status ); 

    API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string* queryData;

	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"       : \"" << "GetResultFromVerticesID" << "\",\n"
		     << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		     << "  \"resultID\"   : \"" << resultID                  << "\",\n"
		     << "  \"analysisID\" : \"" << analysisID                << "\",\n";
	queryCommand << "  \"timeStep\"   : "  << timeStep << ",\n";
	// encode the vertexID list in a base64 string
	// queryCommand << "  \"vertexIDs\"  : [";
	// const int64_t *ip = vertexIDs;
	// while (*ip != 0) {
	//   queryCommand << *ip++;
	//   if (*ip != 0)
	//     queryCommand << ",";
	// }
	// queryCommand << "],\n";
	// list vertexIDs is ends with a '0', so count items
	size_t num_vertexIDs = 0;
	for ( ; vertexIDs[ num_vertexIDs] != 0; num_vertexIDs++) {
	}
	queryCommand << "  \"vertexIDs\"  : \"" 
		     << base64_encode( ( const char *)vertexIDs, num_vertexIDs * sizeof( int64_t))
		     << "\"\n";
	queryCommand << "}\n";

	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS)
	  {
	    size_t numVertices;
	    size_t numElements;

	    // to debug and test:
	    // std::string file_name = std::string( "/tmp/valGetResultFromVerticesID_") + \
	    //   resultID + ".bin";
	    // dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
	    
	    std::istringstream in(*queryData);
	    // this is a worng corrections:
	    in.read((char*)&numVertices, 8);
	    in.read((char*)&numElements, 8);
	    //in >> numVertices >> numElements >> std::ws;
	    
	    if(numVertices > 0 && numElements > 0){
	      const size_t offsetVertexIDs = (size_t)in.tellg();
	      const size_t offsetValues    = offsetVertexIDs + numVertices*sizeof(int64_t);
	      
	      *resultVertexIDs   = (const int64_t*)(&((*queryData)[offsetVertexIDs]));
	      *resultValues      = (const double*) (&((*queryData)[offsetValues]));
	      *resultNumVertices = numVertices;
	    } else {
	      *resultVertexIDs   = NULL;//nullptr;
	      *resultValues      = NULL;//nullptr;
	      *resultNumVertices = 0;
	    }

      *result_status = "Ok";

	  }
  else 
  {
	  *result_status = queryData->c_str();
  }
	
	return result;
      }
    CATCH_ERROR;
  } // valGetResultFromVerticesID
  
  VAL_Result VAL_API valGetBoundingBox( /* in */
				       VAL_SessionID   sessionID,
				       const char     *modelID,
				       const int64_t  *lstVertexIDs,
				       const int64_t   numVertexIDs,
				       const char     *analysisID,
				       const char     *stepOptions,
				       const double   *lstSteps,
				       const int       numSteps,
				       /* out */
				       const double   **resultBBox, // ix doubles: min(x,y,z)-max(x,y,z)
				       const char     **resultErrorStr) { // in case of error
    
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( modelID );
    if ( numVertexIDs) {
      CHECK_QUERY_POINTER( lstVertexIDs );
    }
    CHECK_QUERY_POINTER( analysisID );
    CHECK_QUERY_POINTER( stepOptions );
    if ( !AreEqualNoCase(stepOptions, "all") ) {
      // if stepOptions != "all" then numSteps should be != 0 and lstSteps should have something
      CHECK_NON_ZERO_VALUE( numSteps);
      if ( AreEqualNoCase( stepOptions, "SINGLE")) {
	CHECK_VALUE( numSteps, 1);
      } else if ( AreEqualNoCase( stepOptions, "INTERVAL")) {
	CHECK_VALUE( numSteps, 2);
      }
      CHECK_QUERY_POINTER( lstSteps);
    }
    CHECK_QUERY_POINTER( resultBBox );
    CHECK_QUERY_POINTER( resultErrorStr );
    
    *resultBBox = NULL;
    *resultErrorStr = NULL;

    API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string *queryData = NULL;

	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"         : \"" << "GetBoundingBox" << "\",\n"
		     << "  \"modelID\"      : \"" << modelID          << "\",\n"
		     << "  \"numVertexIDs\" : \"" << numVertexIDs     << "\",\n"
		     << "  \"lstVertexIDs\" : [";
	// can be very large, eventually it can be stored in base64-encoding compressed byte-buffer
	for ( int64_t i = 0; i < numVertexIDs; i++) {
	  if ( i) queryCommand << ",";
	  queryCommand << lstVertexIDs[ i];
	}
	queryCommand << "],\n";
	queryCommand << "  \"analysisID\" : \"" << analysisID       << "\",\n";
	queryCommand << "  \"stepOptions\" : \"" << stepOptions     << "\",\n";
	queryCommand << "  \"numSteps\" : \"" << numSteps     << "\",\n";
	queryCommand << "  \"lstSteps\" : [";
	// can be very large, eventually it can be stored in base64-encoding compressed byte-buffer
	if ( !AreEqualNoCase(stepOptions, "all") && numSteps) {
	  for ( int i = 0; i < numSteps; i++) {
	    if ( i) queryCommand << ",";
	    queryCommand << lstSteps[ i];
	  }
	}
	queryCommand << "]\n";
	queryCommand << "}\n";

	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	    *resultBBox = ( const double *)queryData->data();
	} else {
	  *resultErrorStr = queryData->c_str();
	}

	return result;
      }
    CATCH_ERROR;
  }

  VAL_Result VAL_API valGetDiscrete2Continuum( /* in */
					      VAL_SessionID   sessionID,
					      const char     *modelID,
					      const char     *analysisName,
					      const char     *staticMeshID,
					      const char     *stepOptions,  // ALL, SINGLE, INTERVAL
					      const double   *lstSteps, //ALL (Ignored), SINGLE (1 double), INTERVAL (2 doubles)
					      const int       numSteps, // the size of lstSteps SINGLE(1)
					      const char     *CoarseGrainingMethod,
					      const double   width,
					      const double   cutoffFactor,
					      const bool     processContacts,
					      const bool     doTemporalAVG,
					      const char     *TemporalAVGOptions, //ALL, TEMPORALWINDOW
						  const double   deltaT,
					      /* out */
					      const char   	**queryOutcome,  
					      const char     **resultErrorStr) { // in case of error
    
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( modelID );
    CHECK_QUERY_POINTER( analysisName );
    CHECK_QUERY_POINTER( staticMeshID );
    CHECK_QUERY_POINTER( stepOptions );
    // CHECK_QUERY_POINTER( lstSteps );
    // CHECK_NON_ZERO_VALUE(numSteps);
    
    if ( !AreEqualNoCase(stepOptions, "ALL") ) {
      // if stepOptions != "all" then numSteps should be != 0 and lstSteps should have something
    //  CHECK_NON_ZERO_VALUE( numSteps);
    //  CHECK_QUERY_POINTER( lstSteps);
    }
    // int numSteps = 0;
    if ( AreEqualNoCase( stepOptions, "SINGLE")) {
      // numSteps = 1;
      CHECK_VALUE( numSteps, 1);
    } else if ( AreEqualNoCase( stepOptions, "INTERVAL")) {
      // numSteps = 2;
      CHECK_VALUE( numSteps, 2);
    }
    
    /* D2C parameter pointers */
    //CHECK_QUERY_POINTER( staticMeshID );
    CHECK_QUERY_POINTER( CoarseGrainingMethod );
    CHECK_QUERY_POINTER( TemporalAVGOptions );
    
    CHECK_QUERY_POINTER( queryOutcome );
    CHECK_QUERY_POINTER( resultErrorStr );
    
    *queryOutcome = NULL;
    *resultErrorStr = NULL;
    
    API_TRACE;
    try
      {
	
	std::stringstream  queryCommand;
	const std::string *queryData = NULL;

	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"         : \"" << "GetDiscrete2Continuum" << "\",\n"
		     << "  \"modelID\"      : \"" <<  modelID          << "\",\n";
	queryCommand << "  \"analysisName\" : \"" << analysisName       << "\",\n";
	queryCommand << "  \"staticMeshID\" : \"" <<  staticMeshID << "\",\n";       
	queryCommand << "  \"stepOptions\" : \"" <<   stepOptions << "\",\n";    
	queryCommand << "  \"numSteps\" : \"" << numSteps     << "\",\n";    
	queryCommand << "  \"lstSteps\" : [";
	// can be very large, eventually it can be stored in base64-encoding compressed byte-buffer
//	if ( !AreEqualNoCase(stepOptions, "all") && numSteps) {
	  for ( int i = 0; i < numSteps; i++) {
	    if (i) queryCommand << ",";
	    queryCommand << lstSteps[i];
	  }
//	}
	queryCommand << "],\n";
	/* D2C parameters: */

	queryCommand << "  \"coarseGrainingMethod\" : \"" << 	   CoarseGrainingMethod << "\",\n";	       
	queryCommand << "  \"width\" : \"" << 	           width << "\",\n";	       
	queryCommand << "  \"cutoffFactor\" : \"" <<       cutoffFactor << "\",\n";       
	queryCommand << "  \"processContacts\" : \"" <<    processContacts << "\",\n";    
	queryCommand << "  \"doTemporalAVG\" : \"" <<      doTemporalAVG << "\",\n";      
	queryCommand << "  \"temporalAVGOptions\" : \"" << TemporalAVGOptions << "\",\n"; 
	queryCommand << "  \"deltaT\" : \"" << deltaT << "\"\n"; 
	queryCommand << "}\n";

	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  // *queryOutcome = queryData->data();
	  *queryOutcome = queryData->c_str();
	} else {
	  *resultErrorStr = queryData->c_str();
	}

	return result;
      }
    CATCH_ERROR;
  }


  VAL_Result VAL_API valGetBoundaryOfAMesh( /* in */
					   VAL_SessionID   sessionID,
					   const char     *modelID,
					   const char     *meshID,
					   const char     *analysisID,
					   const double    stepValue,
				       /* out */
					   const char     **resultMesh, // binary data with the mesh vertices and elements
					   size_t         *resultMeshByteSize,
					   const char     **resultErrorStr) { // in case of error
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( meshID );
  CHECK_QUERY_POINTER( analysisID );
  CHECK_QUERY_POINTER( resultMesh );
  CHECK_QUERY_POINTER( resultMeshByteSize );
  CHECK_QUERY_POINTER( resultErrorStr );
    
  *resultMesh = NULL;
  *resultErrorStr = NULL;
  
  API_TRACE;
  try
    {
      std::stringstream  queryCommand;
      const std::string *queryData = NULL;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"       : \"" << "GetBoundaryOfAMesh" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		   << "  \"meshID\"     : \"" << meshID                   << "\",\n"
		   << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		   << "  \"stepValue\"  : \"" << stepValue                  << "\"\n";
      queryCommand << "}\n";

	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  *resultMesh = ( const char *)queryData->data();
	  *resultMeshByteSize = queryData->length();

	  // to debug and test:
	  // std::string file_name = std::string( "/tmp/valGetBoundaryOfAMesh_") + \
	  //   meshID + ".bin";
	  // dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
	} else {
	  *resultErrorStr = queryData->c_str();
	}

	return result;
      }
    CATCH_ERROR;
  }


#ifdef __cplusplus
}
#endif
