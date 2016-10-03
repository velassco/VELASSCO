
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
	    // std::string file_name = std::string( "/tmp/valGetResultFromVerticesID_") + resultID + ".bin";
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
                                              const bool     doSpatialIntegral,
                                              const char     *integralDimension, // 1D                                                                                                                                                                                                                                       
                                              const char     *integralDirection, // X, Y, Z 
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
	queryCommand << "  \"deltaT\" : \"" << deltaT << "\",\n"; 
	queryCommand << "  \"doSpatialIntegral\" : \"" << doSpatialIntegral << "\",\n";
	queryCommand << "  \"integralDimension\" : \"" << integralDimension << "\",\n";
	queryCommand << "  \"integralDirection\" : \"" << integralDirection << "\"\n";
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
	  // std::string file_name = std::string( "/tmp/valGetBoundaryOfAMesh_") + meshID + ".bin";
	  // dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
	} else {
	  *resultErrorStr = queryData ? queryData->c_str() : "Undefined error";
	}

	return result;
      }
    CATCH_ERROR;
  }

  VAL_Result VAL_API valDeleteBoundaryOfAMesh( /* in */
					      VAL_SessionID   sessionID,
					      const char     *modelID,
					      const char     *meshID,
					      const char     *analysisID,
					      const double    stepValue,
					      /* out */
					      const char     **resultErrorStr) { // in case of error
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( meshID );
  CHECK_QUERY_POINTER( analysisID );
  CHECK_QUERY_POINTER( resultErrorStr );
    
  *resultErrorStr = NULL;
  
  API_TRACE;
  try
    {
      std::stringstream  queryCommand;
      const std::string *queryData = NULL;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"       : \"" << "DeleteBoundaryOfAMesh" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		   << "  \"meshID\"     : \"" << meshID                   << "\",\n"
		   << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		   << "  \"stepValue\"  : \"" << stepValue                  << "\"\n";
      queryCommand << "}\n";
      
      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);
      
      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
	// to debug and test:
	// std::string file_name = std::string( "/tmp/valGetBoundaryOfAMesh_") + meshID + ".bin";
	// dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
      } else {
	*resultErrorStr = queryData->c_str();
      }
      
      return result;
    }
  CATCH_ERROR;
  }

  VAL_Result VAL_API valDeleteBoundingBox( /* in */
					  VAL_SessionID   sessionID,
					  const char     *modelID,
					  const int64_t  *lstVertexIDs,
					  const int64_t   numVertexIDs,
					  const char     *analysisID,
					  const char     *stepOptions,
					  const double   *lstSteps,
					  const int       numSteps,
					  /* out */
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
    CHECK_QUERY_POINTER( resultErrorStr );
    
    *resultErrorStr = NULL;
    
    API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string *queryData = NULL;

	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"         : \"" << "DeleteBoundingBox" << "\",\n"
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
	  // *resultBBox = ( const double *)queryData->data();
	} else {
	  *resultErrorStr = queryData->c_str();
	}

	return result;
      }
    CATCH_ERROR;
  }

  VAL_Result VAL_API valDoStreamlinesWithResults( /* in */
            VAL_SessionID  sessionID,
            const char*    modelID,
            const char*    analysisID,
            const double   stepValue,
            const char*    resultID,
            const int64_t  numSeedingPoints,
            const double*  seedingPoints,
            const char*    integrationMethod,    // "EULER", "RUNGE-KUTTA4", or "CASH-KARP"
            const double   maxStreamLineLength,
            const char*    tracingDirection,     // "FORWARD", "BACKWARD", or "FORWARD-BACKWARD"
            const char*    adaptiveStepping,     // "ON" or "OFF"

            /* out */
            const char                        **result_status,
            size_t*                           num_streamlines,
            const size_t**                    lengths,
            const double**                    vertices,
            const double**                    results
  ) {

    CHECK_SESSION_ID(sessionID);
    CHECK_QUERY_POINTER(modelID);
    CHECK_QUERY_POINTER(analysisID);
    CHECK_QUERY_POINTER(resultID);
    CHECK_QUERY_POINTER(seedingPoints);
    CHECK_QUERY_POINTER(result_status);
    CHECK_QUERY_POINTER(num_streamlines);
    CHECK_QUERY_POINTER(lengths);
    CHECK_QUERY_POINTER(vertices);
    CHECK_QUERY_POINTER(results);

    *result_status           = NULL;
    *lengths                 = NULL;
    *vertices                = NULL;
    *results                 = NULL;

    API_TRACE;
    try
    {
      std::stringstream  queryCommand;
      const std::string *queryData = NULL;

      // Build JSON command string
      queryCommand << "{\n"
        << "  \"name\"                : \"" << "DoStreamlinesWithResults"                                                       << "\",\n"
        << "  \"modelID\"             : \"" << modelID                                                                          << "\",\n"
        << "  \"analysisID\"          : \"" << analysisID                                                                       << "\",\n"
        << "  \"stepValue\"           : \"" << stepValue                                                                        << "\",\n"
        << "  \"resultID\"            : \"" << resultID                                                                         << "\",\n"
        << "  \"numSeedingPoints\"    : \"" << numSeedingPoints                                                                 << "\",\n"
        << "  \"seedingPoints\"       : \"" << base64_encode((const char*)seedingPoints, 3 * numSeedingPoints * sizeof(double)) << "\",\n"
        << "  \"integrationMethod\"   : \"" << integrationMethod                                                                << "\",\n"
        << "  \"maxStreamLineLength\" : \"" << maxStreamLineLength                                                              << "\",\n"
        << "  \"tracingDirection\"    : \"" << tracingDirection                                                                 << "\",\n"
        << "  \"adaptiveStepping\"    : \"" << adaptiveStepping                                                                 << "\"\n";
      queryCommand << "}\n";

      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);

      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
        *result_status = (const char *)queryData->data();

	    // to debug and test:
	    // std::string file_name = std::string( "/tmp/valGetResultFromVerticesID_") + resultID + ".bin";
	    // dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
	    
	    std::istringstream in(*queryData);

	    // this is a worng corrections:
	    in.read((char*)num_streamlines, sizeof(size_t));
	    //in >> numVertices >> numElements >> std::ws;
      //in >> *num_streamlines;
	    
      std::cout << *num_streamlines << std::endl;
      
	    if(*num_streamlines > 0){
	      const size_t offsetLengths = (size_t)in.tellg();
                        
        *lengths   = reinterpret_cast<const size_t*>(&((*queryData)[offsetLengths]));
        size_t numVertices = 0;
        for(size_t i = 0; i < *num_streamlines; i++){
          std::cout << (*lengths)[i];
          numVertices += (*lengths)[i];
        }
        
        std::cout << "Num Vertices = " << numVertices << std::endl;
        
        const size_t offsetVertexIDs = offsetLengths   + *num_streamlines*sizeof(size_t);
	      const size_t offsetValues    = offsetVertexIDs + 3*numVertices*sizeof(double);
	      
	      *vertices   = (const double*)(&((*queryData)[offsetVertexIDs]));
	      *results    = (const double*) (&((*queryData)[offsetValues]));
        
	    } else {
	      *lengths   = NULL;//nullptr;
	      *vertices      = NULL;//nullptr;
        *results   = NULL;//nullptr
	      *num_streamlines = 0;
	    }

      *result_status = "Ok";

        

        // to debug and test:
        // std::string file_name = std::string( "/tmp/valGetSimplifiedMesh_") + meshID + ".bin";
        // dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
      }
      else {
        *result_status = queryData ? queryData->c_str() : "No returned data: (NULL)";
      }

      return result;
    }
    CATCH_ERROR;


  }

  
  VAL_Result VAL_API valGetSimplifiedMesh( /* in */
					   VAL_SessionID   sessionID,
					   const char     *modelID,
					   const char     *meshID,
					   const char     *analysisID,
					   const double    stepValue,
					   const char     *parameters, 
					   // parameters: a list of parameters for the simplified mesh algorith
					   // one parameter per line, each line with "keyword (space) value(s)\n"
					   // for instance:
					   // "GridSize=1024;MaximumNumberOfElements=10000000;BoundaryWeight=100.0;"
					   /* out */
					   const char     **resultMesh,
					   // binary data with the mesh vertices and elements
					   // the resultMesh format is described in BoundaryBinaryMesh.h
					   // we store the resulting simplified tetras as Quads in this format
					   size_t         *resultMeshByteSize,
					   const char     **resultErrorStr) { // in case of error
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( meshID );
  CHECK_QUERY_POINTER( analysisID );
  CHECK_QUERY_POINTER( parameters );
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
		   << "  \"name\"       : \"" << "GetSimplifiedMesh" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		   << "  \"meshID\"     : \"" << meshID                   << "\",\n"
		   << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		   << "  \"stepValue\"  : \"" << stepValue                  << "\",\n"
		   << "  \"parameters\" : \"" << parameters                << "\"\n";
      queryCommand << "}\n";

	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  *resultMesh = ( const char *)queryData->data();
	  *resultMeshByteSize = queryData->length();

	  // to debug and test:
	  // std::string file_name = std::string( "/tmp/valGetSimplifiedMesh_") + meshID + ".bin";
	  // dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
	} else {
	  *resultErrorStr = queryData ? queryData->c_str() : "No returned data: (NULL)";
	}

	return result;
      }
    CATCH_ERROR;
  }

  VAL_Result VAL_API valDeleteSimplifiedMesh( /* in */
					      VAL_SessionID   sessionID,
					      const char     *modelID,
					      const char     *meshID,
					      const char     *analysisID,
					      const double    stepValue,
					      const char     *parameters, 
					      // parameters: a list of parameters for the simplified mesh algorith
					      // one parameter per line, each line with "keyword (space) value(s)\n"
					      // for instance:
					      // "GridSize=1024;MaximumNumberOfElements=10000000;BoundaryWeight=100.0;"
					      /* out */
					      const char     **resultErrorStr) { // in case of error
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( meshID );
  CHECK_QUERY_POINTER( analysisID );
  CHECK_QUERY_POINTER( parameters );
  CHECK_QUERY_POINTER( resultErrorStr );
    
  *resultErrorStr = NULL;
  
  API_TRACE;
  try
    {
      std::stringstream  queryCommand;
      const std::string *queryData = NULL;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"       : \"" << "DeleteSimplifiedMesh" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		   << "  \"meshID\"     : \"" << meshID                   << "\",\n"
		   << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		   << "  \"stepValue\"  : \"" << stepValue                  << "\",\n"
		   << "  \"parameters\" : \"" << parameters                << "\"\n";
      queryCommand << "}\n";
      
      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);
      
      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
	// to debug and test:
	// std::string file_name = std::string( "/tmp/valGetSimplifiedMesh_") + meshID + ".bin";
	// dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
      } else {
	*resultErrorStr = queryData->c_str();
      }
      
      return result;
    }
  CATCH_ERROR;
  }

  VAL_Result VAL_API valGetSimplifiedMeshWithResult( /* in */
						    VAL_SessionID   sessionID,
						    const char     *modelID,
						    const char     *meshID,
						    const char     *analysisID,
						    const double    stepValue,
						    const char     *parameters,   // as in valGetSimplifiedMesh
						    const char     *resultID,
						    /* out */
						    const char     **resultMesh,  // as in valGetSimplifiedMesh
						    size_t         *resultMeshByteSize,
						    const double   **resultValues, // result values interpolated/averaged for the simplified mesh
						    const char     **resultErrorStr) { // in case of error
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( meshID );
  CHECK_QUERY_POINTER( analysisID );
  CHECK_QUERY_POINTER( parameters );
  CHECK_QUERY_POINTER( resultID );
  CHECK_QUERY_POINTER( resultMesh );
  CHECK_QUERY_POINTER( resultMeshByteSize );
  CHECK_QUERY_POINTER( resultValues );
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
		   << "  \"name\"       : \"" << "GetSimplifiedMeshWithResult" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		   << "  \"meshID\"     : \"" << meshID                   << "\",\n"
		   << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		   << "  \"stepValue\"  : \"" << stepValue                  << "\",\n"
		   << "  \"parameters\" : \"" << parameters                << "\",\n"
		   << "  \"resultID\"   : \"" << resultID                << "\"\n";
      queryCommand << "}\n";

	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  // compound result: binaryMeshSize + binaryMesh + binaryResults
	  size_t off = 0;
	  int64_t binaryMeshSize = 0;
	  const char *returnData = queryData->data();
	  memcpy( &binaryMeshSize, returnData, sizeof( int64_t)); 
	  off += sizeof( int64_t);
	  *resultMesh = &returnData[ off]; 
	  off += ( size_t)binaryMeshSize;
	  *resultMeshByteSize = ( size_t)binaryMeshSize;
	  *resultValues = (const double*) &returnData[ off];
	  // to debug and test:
	  // std::string file_name = std::string( "/tmp/valGetSimplifiedMesh_") + meshID + ".bin";
	  // dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
	} else {
	  *resultErrorStr = queryData ? queryData->c_str() : "No returned data: (NULL)";
	}

	return result;
      }
    CATCH_ERROR;
  }

  VAL_Result VAL_API valDeleteSimplifiedMeshWithResult( /* in */
						       VAL_SessionID   sessionID,
						       const char     *modelID,
						       const char     *meshID,
						       const char     *analysisID,
						       const double    stepValue,
						       const char     *parameters,    // as in valGetSimplifiedMesh
						       const char     *resultID,
						       /* out */
						       const char     **resultErrorStr) { // in case of error
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( meshID );
  CHECK_QUERY_POINTER( analysisID );
  CHECK_QUERY_POINTER( parameters );
  CHECK_QUERY_POINTER( resultID );
  CHECK_QUERY_POINTER( resultErrorStr );
    
  *resultErrorStr = NULL;
  
  API_TRACE;
  try
    {
      std::stringstream  queryCommand;
      const std::string *queryData = NULL;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"       : \"" << "DeleteSimplifiedMesh" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		   << "  \"meshID\"     : \"" << meshID                   << "\",\n"
		   << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		   << "  \"stepValue\"  : \"" << stepValue                  << "\",\n"
		   << "  \"parameters\" : \"" << parameters                << "\",\n"
		   << "  \"resultID\"   : \"" << resultID                << "\"\n";
      queryCommand << "}\n";
      
      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);
      
      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
	// to debug and test:
	// std::string file_name = std::string( "/tmp/valGetSimplifiedMesh_") + meshID + ".bin";
	// dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
      } else {
	*resultErrorStr = queryData->c_str();
      }
      
      return result;
    }
  CATCH_ERROR;
  }

  VAL_Result VAL_API valDeleteAllCalculationsForThisModel( /* in */
							  VAL_SessionID   sessionID,
							  const char     *modelID,
							  /* out */
							  const char     **resultErrorStr) { // in case of error
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( resultErrorStr );
    
  *resultErrorStr = NULL;
  
  API_TRACE;
  try
    {
      std::stringstream  queryCommand;
      const std::string *queryData = NULL;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"       : \"" << "DeleteAllCalculationsForThisModel" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\"\n";
      queryCommand << "}\n";
      
      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);
      
      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
	// to debug and test:
	// std::string file_name = std::string( "/tmp/valGetSimplifiedMesh_") + meshID + ".bin";
	// dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
      } else {
	*resultErrorStr = queryData->c_str();
      }
      
      return result;
    }
  CATCH_ERROR;
  }

  VAL_Result VAL_API valGetVolumeLRSplineFromBoundingBox( /* in */
							 VAL_SessionID   sessionID,
							 const char     *modelID,
							 const char     *resultID,
							 const double    stepValue,
							 const char     *analysisID,
							 const double   *bBox, // 6 doubles: min(x,y,z)-max(x,y,z)
							 const double   tolerance, // Use ptr to allow NULL?
							 const int      numSteps, // Use ptr to allow NULL?
							 /* out */
							 const char*    *resultBinaryLRSpline,
							 size_t         *resultBinaryLRSplineSize,
							 const char*    *resultStatistics,
							 size_t         *resultStatisticsSize,
							 const char    **resultErrorStr) { // in case of error
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( modelID );
    CHECK_QUERY_POINTER( resultID );
    CHECK_QUERY_POINTER( analysisID );
    CHECK_QUERY_POINTER( bBox );
    // CHECK_QUERY_POINTER( tolerance );
    // CHECK_QUERY_POINTER( numSteps );
    CHECK_QUERY_POINTER( resultBinaryLRSpline );
    CHECK_QUERY_POINTER( resultStatistics );
    CHECK_QUERY_POINTER( resultErrorStr );
    
    *resultBinaryLRSpline = NULL;
    *resultStatistics = NULL;
    *resultErrorStr = NULL;

    API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string *queryData = NULL;

	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"       : \"" << "ComputeVolumeLRSplineFromBoundingBox" << "\",\n"
		     << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		     << "  \"resultID\"     : \"" << resultID                   << "\",\n"
		     << "  \"stepValue\"  : \"" << stepValue                  << "\",\n"
		     << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		     << "  \"bBox\" : [" << bBox[0] << "," << bBox[1] << "," << bBox[2] <<
	  "," << bBox[3] << "," << bBox[4] << "," << bBox[5] << "],\n"
		     << "  \"tolerance\" : \"" << tolerance                << "\",\n"
		     << "  \"numSteps\" : \"" << numSteps                << "\"\n";
	queryCommand << "}\n";

	// g_clients is a map of shared_ptr<VELaSSCo::Client> objects.
	VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
		": The query returned with SUCCESS!");
	  // *resultMesh = ( const char *)queryData->data();
	  // *resultMeshByteSize = queryData->length();
	  *resultBinaryLRSpline = ( const char *) queryData->data();
	  *resultBinaryLRSplineSize = queryData->length();
	  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
		": # bytes in the binary blob: ");
	  bool fetch_statistics = false;
	  if (fetch_statistics) {
	    *resultStatistics = ( const char *) (&(queryData->data()[sizeof(int64_t)]));
	  } else {
	    *resultStatisticsSize = 0;
	    // DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    // 	  ": Not fetching statistics!");
	  }
	  //	    *resultBBox = ( const double *)queryData->data();

	  // to debug and test:
	  // std::string file_name = std::string( "/tmp/valComputeVolumeLRSplineFromBoundingBox_") + resultID + ".bin";
	  // dumpVQueryResult( file_name.c_str(), queryData->data(), queryData->length());
	} else {
	  if (queryData != NULL) {
	    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
		  ": The query failed, but queryData contains the error message.");
	    *resultErrorStr = queryData->c_str();
	  } else {
	    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
		  ": The query failed and the error message was not added to queryData!");
	  }
	}

	return result;
      }
    CATCH_ERROR;
  }

  VAL_Result VAL_API valDeleteVolumeLRSplineFromBoundingBox( /* in */
							    VAL_SessionID   sessionID,
							    const char     *modelID,
							    const char     *resultID,
							    const double    stepValue,
							    const char     *analysisID,
							    const double   *bBox, // 6 doubles: min(x,y,z)-max(x,y,z)
							    const double   tolerance, // Use ptr to allow NULL?
							    const int      numSteps, // Use ptr to allow NULL?
							    /* out */
							    const char    **resultErrorStr) { // in case of error
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( modelID );
    CHECK_QUERY_POINTER( resultID );
    CHECK_QUERY_POINTER( analysisID );
    CHECK_QUERY_POINTER( bBox );
    // CHECK_QUERY_POINTER( tolerance );
    // CHECK_QUERY_POINTER( numSteps );
    CHECK_QUERY_POINTER( resultErrorStr );
    
    *resultErrorStr = NULL;

    API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string *queryData = NULL;

	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"       : \"" << "DeleteVolumeLRSplineFromBoundingBox" << "\",\n"
		     << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		     << "  \"resultID\"     : \"" << resultID                   << "\",\n"
		     << "  \"stepValue\"  : \"" << stepValue                  << "\",\n"
		     << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		     << "  \"bBox\" : [" << bBox[0] << "," << bBox[1] << "," << bBox[2] <<
	  "," << bBox[3] << "," << bBox[4] << "," << bBox[5] << "],\n"
		     << "  \"tolerance\" : \"" << tolerance                << "\",\n"
		     << "  \"numSteps\" : \"" << numSteps                << "\"\n";
	queryCommand << "}\n";

	// g_clients is a map of shared_ptr<VELaSSCo::Client> objects.
	VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);

      
	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  // to debug and test:
	  // std::string file_name = std::string( "/tmp/valGetSimplifiedMesh_") + meshID + ".bin";
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
