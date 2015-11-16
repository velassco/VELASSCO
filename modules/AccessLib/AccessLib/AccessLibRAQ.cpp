
// STL
#include <iostream>
#include <sstream>
#include <string>

// VELaSSCo
#include "AccessLib.h"
#include "Client.h"
#include "Helpers.h"
#include "DemoServer.h" // for the StartServer() function

#include "AccessLibCommon.h"

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
	queryCommand << "  \"vertexIDs\"  : [";
	const int64_t *ip = vertexIDs;
	while (*ip != 0) {
	  queryCommand << *ip++;
	  if (*ip != 0)
	    queryCommand << ",";
	}
	queryCommand << "],\n";
	queryCommand << "  \"timeStep\"   : "  << timeStep << "\n";
	queryCommand << "}\n";

	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS)
	  {
	    size_t numVertices;
	    size_t numElements;

	    std::istringstream in(*queryData);
	    in >> numVertices >> numElements >> std::ws;

	    const size_t offsetVertexIDs = (size_t)in.tellg();
	    const size_t offsetValues    = offsetVertexIDs + numVertices*sizeof(int64_t);

	    *resultVertexIDs   = (const int64_t*)(&((*queryData)[offsetVertexIDs]));
	    *resultValues      = (const double*) (&((*queryData)[offsetValues]));
	    *resultNumVertices = numVertices;
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
					      const char     *analysisID,
					      const char     *staticMeshID,
					      const char     *stepOptions, 
					      const double   *lstSteps,
					      const int       numSteps,
					      const char     *CoarseGrainingMethod,
					      const double     width,
					      const double     cutoffFactor,
					      const bool     processContacts,
					      const bool     doTemporalAVG,
					      const char     *TemporalAVGOptions,
					      const char     *HBaseToUse, /* Not in the VQ-213 form, need to specify table? to store results or get the static mesh?*/
					      /* out */
					      const char   **queryOutcome,  /* Not in the current VQ-213 form, in the previous one says that here comes the list of reslts for the vertices of the Input mesh */
					      const char     **resultErrorStr) { // in case of error
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( modelID );
    CHECK_QUERY_POINTER( analysisID );
    CHECK_QUERY_POINTER( stepOptions );
    if ( !AreEqualNoCase(stepOptions, "all") ) {
      // if stepOptions != "all" then numSteps should be != 0 and lstSteps should have something
      CHECK_NON_ZERO_VALUE( numSteps);
      CHECK_QUERY_POINTER( lstSteps);
    }

    /* D2C parameter pointers */
    CHECK_QUERY_POINTER( staticMeshID );
    CHECK_QUERY_POINTER( CoarseGrainingMethod );
    CHECK_QUERY_POINTER( TemporalAVGOptions );
    CHECK_QUERY_POINTER( HBaseToUse );

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
		     << "  \"name\"         : \"" << "GetBoundingBox" << "\",\n"
		     << "  \"modelID\"      : \"" << modelID          << "\",\n";
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
	queryCommand << "],\n";
	/* D2C parameters: */
	queryCommand << "  \"staticMeshID\" : \"" <<       staticMeshID << "\",\n";       
	queryCommand << "  \"stepOptions\" : \"" <<        stepOptions << "\",\n";        
	queryCommand << "  \"lstSteps\" : \"" << 	   lstSteps << "\",\n";	       
	queryCommand << "  \"numSteps\" : \"" << 	   numSteps << "\",\n";	       
	queryCommand << "  \"CoarseGrainingMethod\" : \"" << 	   CoarseGrainingMethod << "\",\n";	       
	queryCommand << "  \"width\" : \"" << 	           width << "\",\n";	       
	queryCommand << "  \"cutoffFactor\" : \"" <<       cutoffFactor << "\",\n";       
	queryCommand << "  \"processContacts\" : \"" <<    processContacts << "\",\n";    
	queryCommand << "  \"doTemporalAVG\" : \"" <<      doTemporalAVG << "\",\n";      
	queryCommand << "  \"TemporalAVGOptions\" : \"" << TemporalAVGOptions << "\",\n"; 
	queryCommand << "  \"HBaseToUse\" : \"" <<         HBaseToUse << "\"\n";         
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



#ifdef __cplusplus
}
#endif
