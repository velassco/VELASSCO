
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
 * 0xx SQ - Session Queries
 */

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
	API_TRACE;
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

	API_TRACE;
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
  
  API_TRACE;
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
	*result_list_of_models = queryData->c_str(); // eventually strdup() ...
	*result_status = "Ok"; // eventually strdup() ...
      } else {
	// in case of error queryData has the error message from the data layer
	*result_status = queryData->c_str(); // eventually strdup() ...
      }

      return result;
    }
  CATCH_ERROR;
}
  
  VAL_Result VAL_API valOpenModel( /* in */
				  VAL_SessionID   sessionID,
				  const char     *unique_model_name, /* accepted Hbase_TableName:FullPath_as_in_Properties_fp */
				  const char     *requested_access, /* unused at the moment */
				  /* out */
				  const char    **result_status,
				  const char    **result_modelID  /* a 16-digit binary ID or a 32-digit hexadecimal */
				   ) {
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( unique_model_name );
    CHECK_QUERY_POINTER( requested_access );
  
	API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string* queryData;
      
	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"            : \"" << "OpenModel" << "\",\n"
		     << "  \"uniqueName\"      : \"" << unique_model_name   << "\",\n"
		     << "  \"requestedAccess\" : \"" << requested_access      << "\"\n";
	queryCommand << "}\n";
      
	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  /* will be: "NumberOfModels: 1234\nName: model_1\nFullPath: path_1\nName: model_2..." */
	  *result_modelID = queryData->c_str(); // eventually strdup() ...
	  *result_status = "Ok"; // eventually strdup() ...
	} else {
	  // in case of error queryData has the error message from the data layer
	  *result_status = queryData->c_str(); // eventually strdup() ...
	}

	return result;
      }
    CATCH_ERROR;
  }

  VAL_Result VAL_API valCloseModel( /* in */
				   VAL_SessionID   sessionID,
				   const char*     modelID,
				   /* out */
				   const char    **result_status) {
    CHECK_SESSION_ID( sessionID );
    CHECK_QUERY_POINTER( modelID );
  
	API_TRACE;
    try
      {
	std::stringstream  queryCommand;
	const std::string* queryData;
      
	// Build JSON command string
	queryCommand << "{\n"
		     << "  \"name\"            : \"" << "CloseModel" << "\",\n"
		     << "  \"modelID\"    : \"" << modelID                   << "\"\n";
	queryCommand << "}\n";
      
	// Send command string and get back result data
	VAL_Result result = g_clients[sessionID]->Query(sessionID, queryCommand.str(), queryData);

	// Give back pointers to actual binary data
	if (result == VAL_SUCCESS) {
	  *result_status = "Ok"; // eventually strdup() ...
	} else {
	  // in case of error queryData has the error message from the data layer
	  *result_status = queryData->c_str(); // eventually strdup() ...
	}
	return result;
      }
    CATCH_ERROR;
  }

#ifdef __cplusplus
}
#endif
