
// STL
#include <iostream>
#include <sstream>
#include <string>

// VELaSSCo
#include "AccessLib.h"
#include "Client.h"
#include "Helpers.h"
#include "DemoServer.h" // for the StartServer() function

namespace VELaSSCo
{

typedef std::map< VAL_SessionID, boost::shared_ptr<VELaSSCo::Client> > ClientMap;

}

VELaSSCo::ClientMap g_clients;

#define CHECK_SESSION_ID( id )                                       \
	do                                                               \
	{                                                                \
		const VELaSSCo::ClientMap::iterator it = g_clients.find(id); \
		if (it == g_clients.end())                                   \
			return VAL_INVALID_SESSION_ID;                           \
	}                                                                \
	while (0)

#define CHECK_QUERY_POINTER( ptr )               \
	do                                           \
	{                                            \
		if (ptr == NULL)                         \
			return VAL_INVALID_QUERY_PARAMETERS; \
	}                                            \
	while (0)

#define CATCH_ERROR               \
    catch (...)                   \
	{                             \
	        DEBUG( "CATCH_ERROR");	  \
		return VAL_UNKNOWN_ERROR; \
	}                             \


#ifdef __cplusplus
extern "C" {
#endif

VAL_Result VAL_API valUserLogin( /* in */
	                             const char*    url,
						  	     const char*    name,
							     const char*    password,

								 /* out */
								 VAL_SessionID *sessionID )
{
	try
	{
		// Generate new client instance
		boost::shared_ptr<VELaSSCo::Client> client( new VELaSSCo::Client() );

		// Try to log in on VELaSSCo server
		VAL_Result result = client->UserLogin(url, name, password, *sessionID);

		// If successful, we keep the client
		if (result == VAL_SUCCESS)
		{
			g_clients[*sessionID] = client;
		}

		return result;
	}
	CATCH_ERROR;
}

VAL_Result VAL_API valUserLogout( /* in */
								  VAL_SessionID sessionID )
{
	CHECK_SESSION_ID( sessionID );

	try
	{
		// Try to log out from VELaSSCo server
		VAL_Result result = g_clients[sessionID]->UserLogout(sessionID);

		// If successful, we remove the client
		if (result == VAL_SUCCESS)
		{
			g_clients.erase(sessionID);
		}

		return result;
	}
	CATCH_ERROR;
}

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
}

VAL_Result VAL_API valGetListOfModels( /* in */
				      VAL_SessionID   sessionID,
				      const char     *group_qualifier,
				      const char     *name_pattern,
				      /* out */
				      const char    **result_status,
				      const char    **result_list_of_models /* will be: "Name: model_1\nFullPath: path_1\nName: model_2..." */
				       ) {
  CHECK_SESSION_ID( sessionID );
  CHECK_QUERY_POINTER( group_qualifier );
  CHECK_QUERY_POINTER( name_pattern );
  CHECK_QUERY_POINTER( result_status );
  CHECK_QUERY_POINTER( result_list_of_models );
  
  try
    {
      std::stringstream  queryCommand;
      const std::string* queryData;
      
      // Build JSON command string
      queryCommand << "{\n"
		   << "  \"name\"           : \"" << "GetListOfModels" << "\",\n"
		   << "  \"groupQualifier\" : \"" << group_qualifier   << "\",\n"
		   << "  \"namePattern\"    : \"" << name_pattern      << "\"\n";
      queryCommand << "}\n";
      
      // Send command string and get back result data
      VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);

      // Give back pointers to actual binary data
      if (result == VAL_SUCCESS) {
	/* will be: "NumberOfModels: 1234\nName: model_1\nFullPath: path_1\nName: model_2..." */
	*result_status = queryData->c_str(); // eventually strdup() ...
      } else {
	// in case of error queryData has the error message from the data layer
	*result_status = queryData->c_str(); // eventually strdup() ...
      }

      return result;
    }
  CATCH_ERROR;
}


VAL_Result VAL_API valGetStatusDB( /* in */
				  VAL_SessionID   sessionID,
				  /* out */ 
				  const char **status) {

  CHECK_SESSION_ID( sessionID );
  
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

VAL_Result VAL_API valErrorMessage( /* in */
	                                VAL_Result   error,

									/* out */
									const char* *message )
{
	switch (error)
	{
		case VAL_SUCCESS:                       *message = "Success";                        break;

		/* General */
		case VAL_UNKNOWN_ERROR:                 *message = "Unknown error.";                 break;
		case VAL_INVALID_SESSION_ID:            *message = "Invalid session ID.";            break;
		case VAL_INVALID_QUERY_PARAMETERS:      *message = "Invalid query parameters.";		 break;

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

		default:                                *message = "Invalid error code.";            break;
	}

	return VAL_SUCCESS;
}

VAL_Result VAL_API valStartTestServer( const int server_port)
{
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
