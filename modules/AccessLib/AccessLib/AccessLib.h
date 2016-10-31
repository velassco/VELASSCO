/* -*- c++ -*- */
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

//#include "../../EngineLayer/QueryManager/commonFiles/RealTimeFormat.h"
// #include "../../Platform/commonFiles/RealTimeFormat.h"
#include "RealTimeFormat.h"

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
	VAL_INVALID_QUERY                 = 0x0004,

	/* UserLogin */
	VAL_WRONG_URL                     = 0x0100,
	VAL_USER_NOT_ACCEPTED             = 0x0101,
	VAL_SYSTEM_NOT_AVAILABLE          = 0x0102,

	/* UserLogout */
	VAL_LOGOUT_UNSUCCESSFUL           = 0x0200,

	/* GetListOfModels */
	VAL_NO_MODELS_IN_PLATFORM         = 0x0300,
	VAL_NO_MODEL_MATCHES_PATTERN      = 0x0301,

	/* OpenModel */
	/* CloseModel */
	
	/* GetListOfMeshes */
	VAL_NO_MESH_INFORMATION_FOUND     = 0x0401,
	/* GetListOfAnalyses */
	VAL_NO_ANALYSIS_INFORMATION_FOUND = 0x0402,
	/* GetListOfTimeSteps */
	VAL_NO_STEP_INFORMATION_FOUND     = 0x0403,
	/* GetListOfResults */
	VAL_NO_RESULT_INFORMATION_FOUND   = 0x0404,

	/* GetResultFromVerticesID */
	VAL_RESULT_ID_NOT_AVAILABLE       = 0x1000,
	VAL_SOME_VERTEX_IDS_NOT_AVAILABLE = 0x1001,

	/* GetBoundingBox */
	// also uses VAL_SOME_VERTEX_IDS_NOT_AVAILABLE
	VAL_MODEL_ID_NOT_FOUND              = 0x2000,

	/* GetBoundaryOfAMesh */
	VAL_MESH_ID_NOT_FOUND               = 0x2001,
	VAL_BOUNDARY_MESH_NOT_FOUND         = 0x2002,
	/* GetSimplifiedMesh */
	VAL_SIMPLIFIED_MESH_NOT_FOUND       = 0x2003,

	/* GetDiscrete2ContinuumOfAModel */
	// also uses VAL_INVALID_MODEL_ID

	/* GetVolumeLRSplineFromBoundingBox */
	VAL_VOLUME_LRSPLINE_NOT_FOUND       = 0x2004,

} VAL_Result;

typedef int64_t VAL_SessionID;

#ifdef __cplusplus
extern "C" {
#endif

  /*
   * 0xx SQ - Session Queries
   */

  /**
   * The user provides credentials to get access to VELaSSCo data.  The
   * credentials are verified, and, if approved, a session identifier is
   * returned.
   * url may end have this parameter '?multiuser' for a multi-user connection
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
  
  VAL_Result VAL_API valGetListOfModels( /* in */
					VAL_SessionID   sessionID,
					const char     *model_group_qualifier,
					const char     *model_name_pattern,
					/* out */
					const char    **status,
					const char    **list_of_models
					/* will be: "NumberOfModels: 1234\nName: model_1\nFullPath: path_1\nModelID: 12abc\nLocation: DB_type\nName: model_2..." */
					 );					
  
  VAL_Result VAL_API valOpenModel( /* in */
				  VAL_SessionID   sessionID,
				  const char     *unique_model_name, /* accepted Hbase_TableName:FullPath_as_in_Properties_fp */
				  const char     *requested_acces, /* unused at the moment */
				  /* out */
				  const char    **status,
				  const char    **modelID  /* a 16-digit binary ID or a 32-digit hexadecimal */
				   );
  VAL_Result VAL_API valCloseModel( /* in */
				  VAL_SessionID   sessionID,
				  const char*     modelID,
				  /* out */
				  const char    **status);

  /*
   * 1xx DRQ - Direct Result Queries
   */
  VAL_Result VAL_API valGetListOfMeshes(  /* in */
					VAL_SessionID   sessionID,
					const char     *modelID,
					const char     *analysisID, /* if == "" then the static meshes are returned */
					double          stepValue,  /* otherwise the meshes for analysisID+StepValue are returned */
					/* out */
					const char    **status,
					const char    **list_of_meshes
					/* will be: "NumberOfMeshes: 1234\nName: mesh_1\nElementType: Tetrahedra\n...\nName: model_2..." */
					/* the information returned is ElementType, NumberOfVerticesPerElement, NumberOfVertices, NumberOfElements, Units, Color, ... */
					  );
  VAL_Result VAL_API valGetListOfAnalyses(  /* in */
					  VAL_SessionID   sessionID,
					  const char     *modelID,
					  /* out */
					  const char    **status,
					  const char    **list_of_analyses
					  /* will be: "Analysis name 1\nAnalysis name 2\n...\nAnalysis name N" */
					    );
  VAL_Result VAL_API valGetListOfTimeSteps(  /* in */
					   VAL_SessionID   sessionID,
					   const char     *modelID,
					   const char     *analysisID,
					   /* out */
					   const char    **result_status,
					   size_t         *num_steps,
					   const double  **lst_steps
					     );
  VAL_Result VAL_API valGetListOfResults(  /* in */
					 VAL_SessionID   sessionID,
					 const char     *modelID,
					 const char     *analysisID,
					 double          stepValue,
					 /* out */
					 const char    **result_status,
					 const char    **lst_of_results
					/* will be: "NumberOfResults: 1234\nName: result_1\nResultType: Scalar\n...\nName: model_2..." */
					/* the information returned is ResultType, NumberOfComponents, ComponentNames, Location, GaussPointName, CoordinatesName, Units, ... */
					   );
  
  VAL_Result VAL_API valGetMeshDrawData( /* in */
						VAL_SessionID     sessionID,
						const char*       modelID,
						const char*       analysisID,
						double            timeStep,
						const char*       meshID,

						/* out */
            const char                      **result_status,
            const VELaSSCo::RTFormat::File  **result_mesh_draw_data
            );
  
  /*
   * 2xx RAQ - Result Analysis Queries
   */
  
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
               const char*    *result_status,
						   const int64_t* *resultVertexIDs,
						   const double*  *resultValues,
						   size_t         *resultNumVertices );
  
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
				       const char     **resultErrorStr); // in case of error

  VAL_Result VAL_API valGetDiscrete2Continuum( /* in */
					      VAL_SessionID   sessionID,
					      const char     *modelID,
					      const char     *analysisName,
					      const char     *staticMeshID,
					      const char     *stepOptions,  // ALL, SINGLE, INTERVAL, FREQUENCY
					      const double   *lstSteps, //ALL (Ignored), SINGLE (1 double), INTERVAL (2 doubles), FREQUENCY (1 double)
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
					      const char     **resultErrorStr); // in case of error

  VAL_Result VAL_API valGetBoundaryOfAMesh( /* in */
					   VAL_SessionID   sessionID,
					   const char     *modelID,
					   const char     *meshID,
					   const char     *analysisID,
					   const double    stepValue,
					   /* out */
					   const char     **resultMesh,
					   // binary data with the mesh vertices and elements
					   // the resultMesh format is described in BoundaryBinaryMesh.h
					   size_t         *resultMeshByteSize,
					   const char     **resultErrorStr); // in case of error
  VAL_Result VAL_API valDeleteBoundaryOfAMesh( /* in */
					   VAL_SessionID   sessionID,
					   const char     *modelID,
					   const char     *meshID,
					   const char     *analysisID,
					   const double    stepValue,
					   /* out */
					   const char     **resultErrorStr); // in case of error
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
					  const char     **resultErrorStr); // in case of error

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
    );

  // at the moment it only works with tetrahedral meshes.
  VAL_Result VAL_API valGetIsoSurface( /* in */
				      VAL_SessionID   sessionID,
				      const char     *modelID,
				      const char     *meshName,
				      const char     *analysisID,
				      const double    stepValue,
				      const char     *resultName,
				      const int      resultComp,
				      const double   isoValue,
				      /* out */
				      const char     **resultMesh,
				      // binary data with the mesh vertices and elements
				      // the resultMesh format is described in BoundaryBinaryMesh.h
				      // we store the resulting simplified tetras as Quads in this format
				      size_t         *resultMeshByteSize,
				      const char     **resultErrorStr); // in case of error

  // at the moment it only works with tetrahedral meshes.
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
					   const char     **resultErrorStr); // in case of error
  VAL_Result VAL_API valDeleteSimplifiedMesh( /* in */
					     VAL_SessionID   sessionID,
					     const char     *modelID,
					     const char     *meshID,
					     const char     *analysisID,
					     const double    stepValue,
					     const char     *parameters, // as in valGetSimplifiedMesh
					     /* out */
					     const char     **resultErrorStr); // in case of error

  // at the moment it only works with tetrahedral meshes.
  VAL_Result VAL_API valGetSimplifiedMeshWithResult( /* in */
						    VAL_SessionID   sessionID,
						    const char     *modelID,
						    const char     *meshID,
						    const char     *analysisID,
						    const double    stepValue,
						    const char     *parameters,  // as in valGetSimplifiedMesh
						    const char     *resultId,
						    /* out */
						    const char     **resultMesh,  // as in valGetSimplifiedMesh
						    size_t         *resultMeshByteSize,
						    const double   **resultValues, // result values interpolated/averaged for the simplified mesh
						    const char     **resultErrorStr); // in case of error
  VAL_Result VAL_API valDeleteSimplifiedMeshWithResult( /* in */
						       VAL_SessionID   sessionID,
						       const char     *modelID,
						       const char     *meshID,
						       const char     *analysisID,
						       const double    stepValue,
						       const char     *parameters, // as in valGetSimplifiedMesh
						       const char     *resultId,
						       /* out */
						       const char     **resultErrorStr); // in case of error
  VAL_Result VAL_API valDeleteAllCalculationsForThisModel( /* in */
							  VAL_SessionID   sessionID,
							  const char     *modelID,
							  /* out */
							  const char     **resultErrorStr); // in case of error

  VAL_Result VAL_API valGetVolumeLRSplineFromBoundingBox( /* in */
							 VAL_SessionID   sessionID,
							 const char     *modelID,
							 const char     *meshID,
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
							 const char    **resultErrorStr); // in case of error
  VAL_Result VAL_API valDeleteVolumeLRSplineFromBoundingBox( /* in */
							    VAL_SessionID   sessionID,
							    const char     *modelID,
							    const char     *meshID,
							    const char     *resultID,
							    const double    stepValue,
							    const char     *analysisID,
							    const double   *bBox, // 6 doubles: min(x,y,z)-max(x,y,z)
							    const double   tolerance, // Use ptr to allow NULL?
							    const int      numSteps, // Use ptr to allow NULL?
							    /* out */
							    const char    **resultErrorStr); // in case of error

  /**
   * Translate a numerical result code into an error message string.
   * The memory for the string does not need to be released by the user.
   */
  VAL_Result VAL_API valErrorMessage(            /* in */
				     VAL_Result      error,
				     
				     /* out */
				     const char*    *message );

  /*
   * 4xx MQ - Monitoring Queries
   */

  VAL_Result VAL_API valGetStatusDB(  /* in */
				    VAL_SessionID   sessionID,
				    /* out */ 
				    const char **status);
  VAL_Result VAL_API valStopVELaSSCo(  /* in */
				     VAL_SessionID   sessionID,
				     /* out */ 
				     const char **status);
  VAL_Result VAL_API valGetConfiguration( /* in */
					 VAL_SessionID   sessionID,
					 const char     *parameter,
					 /* out */ 
					 const char    **status,
					 const char    **parameter_value);
  VAL_Result VAL_API valSetConfiguration( /* in */
					 VAL_SessionID   sessionID,
					 const char     *parameter,
					 const char     *value,
					 /* out */ 
					 const char    **status);
  /**
   * API testing. 
   */
  VAL_Result VAL_API valStartTestServer( const int server_port);
  
#ifdef __cplusplus
}
#endif

#endif
