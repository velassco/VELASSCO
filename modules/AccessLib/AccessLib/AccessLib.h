
#ifndef VELASSCO_ACCESS_LIB_H
#define VELASSCO_ACCESS_LIB_H

#ifdef __cplusplus
#  include <cstdint>
#  include <cstddef>
#else
#  include <stdint.h>
#  include <stddef.h>
#endif

#ifndef VAL_API
#  if defined(_WIN32)
#    define VAL_API /* __declspec(dllimport) */ // no DLL for now
#  else
#    define VAL_API
#  endif
#endif

/**
 * List of function result codes. Non-zero codes indicate an error.
 */
typedef enum
{
	VAL_SUCCESS                       = 0,

	/* General */
	VAL_UNKNOWN_ERROR                 = 0x0001,
	VAL_INVALID_SESSION_ID	          = 0x0002,
	VAL_INVALID_QUERY_PARAMETERS      = 0x0003,

	/* UserLogin */
	VAL_WRONG_URL                     = 0x0100,
	VAL_USER_NOT_ACCEPTED             = 0x0101,
	VAL_SYSTEM_NOT_AVAILABLE          = 0x0102,

	/* UserLogout */
	VAL_LOGOUT_UNSUCCESSFUL           = 0x0200,

	/* GetResultFromVerticesID */
	VAL_RESULT_ID_NOT_AVAILABLE       = 0x0300,
	VAL_SOME_VERTEX_IDS_NOT_AVAILABLE = 0x0301

} VAL_Result;

typedef int64_t VAL_SessionID;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The user provides credentials to get access to VELaSSCo data.  The
 * credentials are verified, and, if approved, a session identifier is
 * returned.
 */
VAL_Result VAL_API valUserLogin(               /* in */
	                                           const char*     url,
						  	                   const char*     name,
							                   const char*     password,

											   /* out */
								               VAL_SessionID  *sessionID );

/**
 * Logout from the VELaSSCo platform.
 * After successfully logging out, the session identifier is invalid.
 */
VAL_Result VAL_API valUserLogout(              /* in */
								               VAL_SessionID   sessionID );

/**
 * Given a list of vertices IDs from the model, get the result value of a
 * given type of result for each vertex id of the list.
 */
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
											   size_t         *resultNumVertices );

/**
 * Translate a numerical result code into an error message string.
 * The memory for the string does not need to be released by the user.
 */
VAL_Result VAL_API valErrorMessage(            /* in */
	                                           VAL_Result      error,

									           /* out */
									           const char*    *message );

  VAL_Result VAL_API valGetStatusDB(  /* in */
				    VAL_SessionID   sessionID,
				    /* out */ 
				    const char **status);
  
/**
 * API testing. 
 */
VAL_Result VAL_API valStartTestServer( const int server_port);

#ifdef __cplusplus
}
#endif

#endif
