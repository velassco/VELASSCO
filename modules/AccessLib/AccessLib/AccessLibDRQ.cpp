
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
 * 1xx DRQ - Direct Result Queries
 */

#ifdef __cplusplus
extern "C" {
#endif

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
	*result_list_of_meshes = queryData->c_str(); // eventually strdup() ...
	*result_status = "Ok"; // eventually strdup() ...
      } else {
	// in case of error queryData has the error message from the data layer
	*result_status = queryData->c_str(); // eventually strdup() ...
      }
      return result;
    }
  CATCH_ERROR;
} // valGetListOfMeshes

#ifdef __cplusplus
}
#endif
