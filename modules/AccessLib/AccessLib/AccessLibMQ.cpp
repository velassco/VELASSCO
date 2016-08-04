
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
 * 4xx MQ - Monitoring Queries
 */

#ifdef __cplusplus
extern "C" {
#endif

VAL_Result VAL_API valGetStatusDB( /* in */
				  VAL_SessionID   sessionID,
				  /* out */ 
				  const char **status) {

  CHECK_SESSION_ID( sessionID );
  
	API_TRACE;
	try
	{
		// Try to log out from VELaSSCo server
		const std::string *str_status;
		VAL_Result result = g_clients[sessionID]->GetStatusDB( sessionID, str_status);
		*status = str_status->c_str(); // eventually strdup() ...
		return result;
	}
	CATCH_ERROR;
  }

  VAL_Result VAL_API valStopVELaSSCo( /* in */
				     VAL_SessionID   sessionID,
				     /* out */ 
				     const char **status) {
    
    CHECK_SESSION_ID( sessionID );
    
    API_TRACE;
    try
      {
	// Try to log out from VELaSSCo server
	const std::string *str_status;
	VAL_Result result = g_clients[sessionID]->StopVELaSSCo( sessionID, str_status);
	*status = str_status->c_str(); // eventually strdup() ...
	return result;
      }
    CATCH_ERROR;
  }


  VAL_Result VAL_API valGetConfiguration( /* in */
					 VAL_SessionID   sessionID,
					 const char     *parameter,
					 /* out */ 
					 const char    **result_status,
					 const char    **result_parameter_value) {
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( parameter );
    
    API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string* queryData;
      
	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\" : \"" << "GetConfiguration" << "\",\n"
		     << "  \"Key\"  : \"" << parameter   << "\"\n";
	queryCommand << "}\n";
      
	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  *result_status = "Ok"; // eventually strdup() ...
	  *result_parameter_value = queryData->c_str(); // eventually strdup() ...
	} else {
	  // in case of error queryData has the error message from the data layer
	  *result_status = queryData->c_str(); // eventually strdup() ...
	}

	return result;
      }
    CATCH_ERROR;
  }

  VAL_Result VAL_API valSetConfiguration( /* in */
					 VAL_SessionID   sessionID,
					 const char     *parameter,
					 const char     *value,
					 /* out */ 
					 const char    **result_status) {
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( parameter );
    CHECK_QUERY_POINTER( value );
    
    API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string* queryData;
      
	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"  : \"" << "SetConfiguration" << "\",\n"
		     << "  \"Key\"   : \"" << parameter   << "\",\n"
		     << "  \"Value\" : \"" << value   << "\"\n";
	queryCommand << "}\n";
      
	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query( sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  *result_status = "Ok"; // eventually strdup() ...
	  
	  // is flag passed is "CompressionEnabled" we should enable that on our side too.
	  if (  !strcasecmp( parameter, "CompressionEnabled") || !strcasecmp( parameter, "CompressionEnable")) {
	    int flag = 1;
	    int n = sscanf( value, "%d", &flag);
	    if ( n == 1) {
	      g_clients[sessionID]->SetCompressionEnabledFlag( flag ? true : false);
	      LOGGER << "   Compresison " << ( flag ? "enabled" : "disabled") << std::endl;
	    }
	  }
	} else {
	  // in case of error queryData has the error message from the data layer
	  *result_status = queryData->c_str(); // eventually strdup() ...
	}

	return result;
      }
    CATCH_ERROR;
  }

VAL_Result VAL_API valErrorMessage( /* in */
	                                VAL_Result   error,

									/* out */
									const char* *message )
{
	API_TRACE;
	switch (error)
	{
	case VAL_SUCCESS:                       *message = "Success";                        break;
	  
	  /* General */
	case VAL_UNKNOWN_ERROR:                 *message = "Unknown error.";                 break;
	case VAL_INVALID_SESSION_ID:            *message = "Invalid session ID.";            break;
	case VAL_INVALID_QUERY_PARAMETERS:      *message = "Invalid query parameters.";		 break;
	case VAL_INVALID_QUERY:                 *message = "Invalid/unknown query.";		 break;
	  
	  /* UserLogin */
	case VAL_WRONG_URL:                     *message = "Wrong URL.";                     break;
	case VAL_USER_NOT_ACCEPTED:             *message = "User not accepted.";             break;
	case VAL_SYSTEM_NOT_AVAILABLE:          *message = "System not available.";          break;
	  
	  /* UserLogout */
	case VAL_LOGOUT_UNSUCCESSFUL:           *message = "Logout unsuccessful.";           break;
	  
	  /* GetListOfModels */
	case VAL_NO_MODELS_IN_PLATFORM:         *message = "No models in platform.";         break;
	case VAL_NO_MODEL_MATCHES_PATTERN:      *message = "No models match with pattern.";  break;
	  
	  /* GetResultFromVerticesID */
	case VAL_RESULT_ID_NOT_AVAILABLE:       *message = "Result ID not available.";       break;
	case VAL_SOME_VERTEX_IDS_NOT_AVAILABLE: *message = "Some vertex IDs not available."; break;
	  
	  /* GetListOfMeshes */
	case VAL_NO_MESH_INFORMATION_FOUND:     *message = "No mesh information found.";     break;
	  /* GetListOfAnalyses */
	case VAL_NO_ANALYSIS_INFORMATION_FOUND: *message = "No analysis information found."; break;
	  /* GetListOfTimeSteps */
	case VAL_NO_STEP_INFORMATION_FOUND:     *message = "No step information found.";     break;
	  /* GetListOfResults */
	case VAL_NO_RESULT_INFORMATION_FOUND:   *message = "No result information found.";   break;

	  /* GetBoundingBox */
	case VAL_MODEL_ID_NOT_FOUND:            *message = "Model ID not found.";   break;

	  /* GetBoundaryOfAMesh & DeleteBoundaryOfAMesh */
	case VAL_MESH_ID_NOT_FOUND:             *message = "Mesh ID not found.";   break;
	case VAL_BOUNDARY_MESH_NOT_FOUND:       *message = "Boundary mesh not found.";   break;
	default:                                *message = "Invalid error code.";            break;
	}
	
	return VAL_SUCCESS;
}

VAL_Result VAL_API valStartTestServer( const int server_port)
{
	API_TRACE;
	try
	{
	  DEBUG( "before StartServer");
		StartServer( server_port);
		DEBUG( "after StartServer");
		return VAL_SUCCESS;
	}
	CATCH_ERROR;
}

#ifdef __cplusplus
}
#endif
