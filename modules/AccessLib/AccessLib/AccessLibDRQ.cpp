
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

/*
 * 1xx DRQ - Direct Result Queries
 */

#ifdef __cplusplus
extern "C" {
#endif

  VAL_Result VAL_API valGetMeshDrawData( /* in */
						VAL_SessionID   sessionID,
						const char*     modelID,
						//const char*     resultID,
						const char*     analysisID,
						double          timeStep,
						const char*     meshID,

						/* out */
            const char                        **result_status,
            const VELaSSCo::RTFormat::File    **result_mesh_draw_data
            )
  {
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( modelID );
    //CHECK_QUERY_POINTER( resultID );
    CHECK_QUERY_POINTER( analysisID );
    CHECK_QUERY_POINTER( meshID );
    CHECK_QUERY_POINTER( result_status );
    CHECK_QUERY_POINTER( result_mesh_draw_data );

	API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string* queryData = NULL;

	// Build JSON command string
	queryCommand << "{\n"
				<< "  \"name\"       : \"" << "GetMeshDrawData" << "\",\n"
				<< "  \"modelID\"    : \"" << modelID           << "\",\n"
				//<< "  \"resultID\"   : \"" << resultID          << "\",\n"
				<< "  \"analysisID\" : \"" << analysisID        << "\",\n"
				<< "  \"timeStep\"   : "   << timeStep 			<< ",\n"
				<< "  \"meshID\": \""        << meshID       << "\"\n"
				<< "}\n";

	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS)
	{
    std::istringstream in(*queryData);

    VELaSSCo::RTFormat::File* file = new VELaSSCo::RTFormat::File;
    in >> *file;

    *result_mesh_draw_data = file;
    *result_status = "Ok";
	}
  else
  {
    // in case of error queryData has the error message from the data layer
    *result_status = queryData ? queryData->c_str() : "Error undefined";
  }

	return result;
      }
    CATCH_ERROR;
  } // valGetMeshDrawData

VAL_Result VAL_API valGetListOfMeshes(  /* in */
				      VAL_SessionID   sessionID,
				      const char     *modelID,
				      const char     *analysisID, /* if == "" then the static meshes are returned */
				      double          stepValue,  /* otherwise the meshes for analysisID+StepValue are returned */
				      /* out */
				      const char    **result_status,
				      const char    **result_list_of_meshes
				      /* will be: "NumberOfMeshes: 1234\nName: mesh_1\nElementType: Tetrahedra\n...\nName: model_2..." */
				      /* the information returned is ElementType, NumberOfVerticesPerElement, NumberOfVertices, NumberOfElements, Units, Color, ... */
					) {
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( analysisID );
  CHECK_QUERY_POINTER( result_status );
  CHECK_QUERY_POINTER( result_list_of_meshes );
  
  API_TRACE;
  try
    {
      std::stringstream  queryCommand;
      const std::string* queryData;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"       : \"" << "GetListOfMeshes" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		   << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		   << "  \"stepValue\" : \"" << stepValue                  << "\"\n";
      queryCommand << "}\n";
      
      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);
      
      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
	/* will be: "NumberOfModels: 1234\nName: model_1\nFullPath: path_1\nName: model_2..." */
	*result_list_of_meshes = queryData->c_str();
	*result_status = "Ok";
      } else {
	// in case of error queryData has the error message from the data layer
	*result_status = queryData->c_str();
      }
      return result;
    }
  CATCH_ERROR;
} // valGetListOfMeshes

VAL_Result VAL_API valGetListOfAnalyses(  /* in */
					VAL_SessionID   sessionID,
					const char     *modelID,
					/* out */
					const char    **result_status,
					const char    **result_list_of_analyses
					/* will be: "Analysis name 1\nAnalysis name 2\n...\nAnalysis name N" */
					  ) {
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( result_status );
  CHECK_QUERY_POINTER( result_list_of_analyses );
  
  API_TRACE;
  try
    {
      std::stringstream  queryCommand;
      const std::string* queryData;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"       : \"" << "GetListOfAnalyses" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\"\n";
      queryCommand << "}\n";
      
      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);
      
      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
	/* will be: "Analysis name 1\nAnalysis name 2\n...\nAnalysis name N" */
	*result_list_of_analyses = queryData->c_str();
	*result_status = "Ok";
      } else {
	// in case of error queryData has the error message from the data layer
	*result_status = queryData->c_str();
      }
      return result;
    }
  CATCH_ERROR;
} // valGetListOfAnalyses

VAL_Result VAL_API valGetListOfTimeSteps(  /* in */
					 VAL_SessionID   sessionID,
					 const char     *modelID,
					 const char     *analysisID,
					 /* out */
					 const char    **result_status,
					 size_t         *result_num_steps,
					 const double  **result_list_of_steps
					   ) {
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( analysisID );
  CHECK_QUERY_POINTER( result_status );
  CHECK_QUERY_POINTER( result_list_of_steps );
  
  API_TRACE;
  try
    {
      std::stringstream  queryCommand;
      const std::string* queryData;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"       : \"" << "GetListOfTimeSteps" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		   << "  \"analysisID\" : \"" << analysisID                << "\"\n";
      queryCommand << "}\n";
      
      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);
      
      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
	/* will be: "Analysis name 1\nAnalysis name 2\n...\nAnalysis name N" */
	*result_num_steps = queryData->size() / sizeof( double); // size() is in bytes ....
	*result_list_of_steps = ( const double *)queryData->data();
	*result_status = "Ok";
      } else {
	// in case of error queryData has the error message from the data layer
	*result_status = queryData->c_str();
      }
      return result;
    }
  CATCH_ERROR;
} // valGetListOfTimeSteps


VAL_Result VAL_API valGetListOfResults(  /* in */
				       VAL_SessionID   sessionID,
				       const char     *modelID,
				       const char     *analysisID,
				       double          stepValue,
				       /* out */
				       const char    **result_status,
				       const char    **result_list_of_results
				       /* will be: "NumberOfResults: 1234\nName: result_1\nResultType: Scalar\n...\nName: model_2..." */
				       /* the information returned is ResultType, NumberOfComponents, ComponentNames, Location, GaussPointName, CoordinatesName, Units, ... */
					 ) {
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( modelID );
  CHECK_QUERY_POINTER( analysisID );
  CHECK_QUERY_POINTER( result_status );
  CHECK_QUERY_POINTER( result_list_of_results );
  
  API_TRACE;
  try
    {
      std::stringstream  queryCommand;
      const std::string* queryData;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"       : \"" << "GetListOfResults" << "\",\n"
		   << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		   << "  \"analysisID\" : \"" << analysisID                << "\",\n"
		   << "  \"stepValue\"   : "  << stepValue << "\n";
      queryCommand << "}\n";
      
      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);
      
      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
	/* will be: "NumberOfResults: 1234\nName: result_1\nResultType: Scalar\n...\nName: model_2..." */
	/* the information returned is ResultType, NumberOfComponents, ComponentNames, Location, GaussPointName, CoordinatesName, Units, ... */
	*result_list_of_results = queryData->c_str();
	*result_status = "Ok";
      } else {
	// in case of error queryData has the error message from the data layer
	*result_status = queryData->c_str();
      }
      return result;
    }
  CATCH_ERROR;
} // valGetListOfResults

  
VAL_Result VAL_API valGetMeshVertices( /* in */
				      VAL_SessionID     sessionID,
				      const char       *modelID,
				      const char       *analysisID,
				      double            timeStep,
				      const char       *meshID,
				      const int64_t    *vertexIDs, /* may be NULL */
				      
				      /* out */
				      const char      **result_status,
				      const int64_t   **resultVertexIDs,
				      const double    **resultVertexCoordinates, /* x1y1z1x2y2z2x3y3z3x4y4z4...*/
				      size_t           *resultNumVertices
				       ) {
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( modelID );
    CHECK_QUERY_POINTER( meshID );
    CHECK_QUERY_POINTER( analysisID );
    // vertexIDs may be null to retrieve all vertices
    // CHECK_QUERY_POINTER( vertexIDs );
    CHECK_QUERY_POINTER( resultVertexIDs );
    CHECK_QUERY_POINTER( resultVertexCoordinates );
    CHECK_QUERY_POINTER( resultNumVertices );
    CHECK_QUERY_POINTER( result_status ); 

    API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string* queryData;

	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"       : \"" << "GetMeshVertices" << "\",\n"
		     << "  \"modelID\"    : \"" << modelID                   << "\",\n"
		     << "  \"meshID\"   : \"" << meshID                  << "\",\n"
		     << "  \"analysisID\" : \"" << analysisID                << "\",\n";
	queryCommand << "  \"stepValue\"   : "  << timeStep << ",\n";
	// encode the vertexID list in a base64 string
	// list vertexIDs is ends with a '0', so count items
	size_t num_vertexIDs = 0;
	if ( vertexIDs) {
	  for ( ; vertexIDs[ num_vertexIDs] != 0; num_vertexIDs++) {
	  }
	}
	// if no vertexIDs are provided, base64_encode return an empty string
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
	    
	    std::istringstream in(*queryData);
	    // this is a worng corrections:
	    in.read((char*)&numVertices, 8);
	    
	    if ( numVertices > 0) {
	      const size_t offsetVertexIDs = (size_t)in.tellg();
	      const size_t offsetVertexCoordinates = offsetVertexIDs + numVertices * sizeof(int64_t);
	      
	      *resultVertexIDs   = (const int64_t*)(&((*queryData)[offsetVertexIDs]));
	      *resultVertexCoordinates = (const double*) (&((*queryData)[offsetVertexCoordinates]));
	      *resultNumVertices = numVertices;
	    } else {
	      *resultVertexIDs   = NULL;//nullptr;
	      *resultVertexCoordinates      = NULL;//nullptr;
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
} // valGetMeshVertices

#ifdef __cplusplus
}
#endif
