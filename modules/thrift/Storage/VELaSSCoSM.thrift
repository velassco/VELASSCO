/*
 * Licensed ...
 * V 0.0 : BL - First thrift definition   
 */

/**
   Namespace dl (Data Layer Interface)
 */
// namespace cpp dli
namespace cpp VELaSSCoSM
namespace java VELaSSCoSM

// the names will also be used as String identifiers
// take care of that in DataLayer/Storage/HBase_result_queries.cpp
enum ElementShapeType {
  UnknownElement = 0,
  PointElement = 1,
  LineElement = 2,
  TriangleElement = 3,
  QuadrilateralElement = 4,
  TetrahedraElement = 5,
  HexahedraElement = 6,
  PrismElement = 7,
  PyramidElement = 8,
  SphereElement = 9,
  CircleElement = 10,
  ComplexParticleElement = 11
}

struct ElementType {
  1: ElementShapeType   shape, // to avoid ambiguity meshinfo.type.type
  2: i32                num_nodes
}

struct Point {
   1: double                              x
   2: double                              y
   3: double                              z
}

typedef i64 NodeID;
struct Vertex {
   1: NodeID                              id
   2: double                              x
   3: double                              y
   4: double                              z
}

struct Element {
   1: i64                                 id
   2: list<NodeID>                          nodes_ids
   // 2: list<i64>                           nodes_ids
}

struct ElementAttrib {
   1: i64                                 id
   2: string                              name
   3: list<double>                        value
}

struct ElementGroup {
   1: i64                                 id
   2: i64                                 group_id
}

struct MeshInfo {
   1: string                              name
   2: ElementType                         elementType // to avoid ambiguity meshinfo.type.type
   3: i64                                 nVertices
   4: i64                                 nElements
   5: string                              meshUnits
   6: string                              meshColor
   7: i32                                 meshNumber // as of M:mXXXXXXnm
   8: string                              coordsName // as in M:mXXXXXXcn coordinates name, qualifier prefix
}

struct Mesh {
  // eventually MeshInfo
   1: string                              name     
   2: i64                                 numberOfVertices
   3: i64                                 dimension
   4: ElementType                         type
   5: list<Vertex>                        nodes
   6: list<Element>                       elements
}

struct ResultInfo {
  1: string        name
  2: string        type
  3: i32           numberOfComponents
  4: list< string> componentNames
  5: string        location
  6: string        gaussPointName
  7: string        coordinatesName
  8: string        units
  9: i32           resultNumber // as of R:rXXXXXXnm
}

struct ResultOnVertex {
  1: i64                                  id
  2: list<double>                         value
  3: binary                               bvalue
}

struct Triangle {
   1: list<NodeID>                          nodes
}


/**
   The following structs are used as return values for the service methods. The name of the structs shall have the
   prefix rv (for Return Value) plus the method name. The name of the return value struct of the GetElementOfPointsInSpace
   method will then be rvGetElementOfPointsInSpace.
*/


struct rvGetElementOfPointsInSpace {
   1: string                              status
   2: string                              report
   3: list<Element>                       elements
}

/**
   The following structs are used as return values for the service methods. The name of the structs shall have the
   prefix rv (for Return Value) plus the method name. The name of the return value struct of the GetElementOfPointsInSpace
   method will then be rvGetElementOfPointsInSpace.
*/


struct rvGetBoundaryOfLocalMesh {
   1: string                              status
   2: string                              report
   3: list<Triangle>                      elements
}


struct rvGetResultFromVerticesID_A {
   1: string                              status
   2: string                              report
   3: i64                                 nVertices
   4: i64                                 nValuesPrVertex
   5: list<i64>                           vertexIDs
   6: list<double>                        resultValues
}

struct VertexResult {
   1: i64                                 vertexID
   2: list<double>                        resuls
}


struct rvGetResultFromVerticesID_B {
   1: string                              status
   2: string                              report
   3: list<VertexResult>                  vertexResults
}



struct ModelInfo {
   1: string                              name
   2: string                              repository
   3: string                              owner
   4: string                              created
   5: string                              accessRights
   6: string                              administrators
   7: string                              underlyingSchema
   8: i64                                 nObjectsInModel
   9: i64                                 dataSize
   10: string                             stringEncoding
   11: string                             options
}


// struct rvGetListOfModels {
//    1: string                              status
//    2: string                              report
//    3: list<ModelInfo>                     models
// }

// as of OP-22.055 GetListOfModelNames
struct FullyQualifiedModelName {
   1: string name        // HBase: Properties-nm
   2: string full_path   // HBase: Properties-fp
   3: binary modelID
   4: string location   // EDM or HBase:TableName
}

// as of OP-22.055 GetListOfModelNames
struct rvGetListOfModels {
   1: string                              status // OK / Error
   2: string                              report // Error report
   3: list< FullyQualifiedModelName>      models
}


struct rvGetThumbnailOfAModel {
   1: string                              status
   2: string                              report
   3: binary                              imageFile
}


// struct rvOpenModel {
//    1: string                              status
//    2: string                              report
//    3: string                              modelID
// }

struct rvOpenModel {
   1: string status // OK / Error
   2: string report // Error report
   3: binary modelID
   4: FullyQualifiedModelName model_info
}


struct rvGetListOfAnalyses {
   1: string                              status
   2: string                              report
   3: list<string>                        analyses
}


struct rvGetListOfTimeSteps {
   1: string                              status
   2: string                              report
   3: list<double>                        time_steps
}


struct rvGetListOfMeshes {
   1: string                              status
   2: string                              report
   3: list<MeshInfo>                      meshInfos
}

struct rvGetListOfResults {
   1: string            status
   2: string            report
   3: list< ResultInfo> result_list
}

struct rvGetListOfVerticesFromMesh {
   1: string            status
   2: string            report
   3: list<Vertex>      vertex_list
}

/*
   rvGetResultFromVerticesID
   The binary result_array is an array of result_record. A result_record is declared
   as follows in C++ like fasion:
   
   struct result_record {
      EDMULONG                            id;
      double                              value[n_values_pr_record];
   };
   
   The query must set the values so that:
   n_result_records * result_record_size == sizeof(result_array) 
*/
struct rvGetResultFromVerticesID {
  1: string             status
  2: string             report
  3: i64                n_values_pr_record
  4: i64                result_record_size
  5: i64                n_result_records
  6: binary             result_array
}

struct rvGetCoordinatesAndElementsFromMesh {
  1: string status
  2: string report
  3: list<Vertex>        vertex_list
  4: list<Element>       element_list
  5: list<ElementAttrib> element_attrib_list
  6: list<ElementGroup>  element_group_info_list
}

// provides service VELaSSCo Storage Module
service VELaSSCoSM
{
  /**
   Return the status of the different services 
   which run on the Data Layer.
   */
  string	statusDL(),
  
  /**
   Data Query operations
   */
  // OP-22.055 GetListOfModelNames includes CO-22.03 GetListOfModels
  rvGetListOfModels GetListOfModelNames( 1: string sessionID, 
    2: string model_group_qualifier,    // in Hbase we use it as full_path_pattern ( Properties:fp)
    3: string model_name_pattern),       // model name pattern ( Properties:nm)
    
  // as of OP-24.058 FindModelFS ( should be OpenModel !!!)
  rvOpenModel FindModel( 1: string sessionID, 
    // model name pattern (hbase = TableName:Properties-fp:Properties-nm) 
    //     options: ModelName, /Full/Path:ModelName or TableName:/Full/Path:ModelName
    2: string unique_model_name_pattern,  
    3: string requested_access)     

  /**
   Return the status of the different services 
   which run on the Data Layer.
   @return string - returns a structured list of avialbe vertices, 
   with the attached list of double 
   if errors occur the contect is also returned here?
   */
   rvGetResultFromVerticesID	GetResultFromVerticesID(
     1: string sessionID
     2: string modelID
     3: string analysisID
     4: double timeStep
     5: string resultID
     6: list<i64> listOfVertices ),
  
  
  /**
   Return the coordinates and elements of a model's mesh.
   @return string - returns a structured list of vertices and elements of a model's mesh.
   if errors occur the contect is also returned here?
   */  
  rvGetCoordinatesAndElementsFromMesh 	GetCoordinatesAndElementsFromMesh(
	1: string   sessionID, 
	2: string   modelID, 
	3: string   analysisID, 
	4: double   timeStep, 
	5: MeshInfo meshInfo),
    
    /**
     Stop Data Layer
     */							 	
    void stopAll()

    
    /**
     as of OP-22.117
     Returns a list of meshes present for the given time-step of that analysis.
     If analysis == "" and step-value == -1 then the list will be of the 'static' meshes.
     If analysis != "" and step-value != -1 then the list will be of the 'dynamic' meshes
     that are present on that step-values of that analysis.
     */
    rvGetListOfMeshes GetListOfMeshes(
      1: string                           sessionID
      2: string                           modelID
      3: string                           analysisID
      4: double                           stepValue ),
      
      /**
       as of OP-22.112
       Retrieves the list of time steps for a given model and analysis.
       */
      rvGetListOfAnalyses GetListOfAnalyses(
	1: string                           sessionID
	2: string                           modelID ),

	/**
	 Retrieves the list of time steps for a given model and analysis.
	 */
	rvGetListOfTimeSteps GetListOfTimeSteps(
	  1: string                           sessionID
	  2: string                           modelID
	  3: string                           analysisID ),
	  
	/**
	 Retrieves the list of results for a given model, analysis and step-value
	 as of OP-22.115
	 */
	rvGetListOfResults GetListOfResultsFromTimeStepAndAnalysis(
	  1: string                           sessionID
	  2: string                           modelID
	  3: string                           analysisID
	  4: double                           stepValue),


	  /**
	   Extract a list of vertices from the open model and the selected meshID
	   as of OP-22.116 
	   */
	  rvGetListOfVerticesFromMesh GetListOfVerticesFromMesh(
	  1: string                           sessionID
	  2: string                           modelID
	  3: string                           analysisID
	  4: double                           stepValue
	  5: i32                              meshID // actually it's the coordsID, i.e. cXXXXX ...
	    ),

  /**
   Return the status of the different services 
   which run on the Data Layer.
   @return string - returns a structured list of avialbe vertices, 
   with the attached list of double 
   if errors occur the contect is also returned here?
   string	GetResultFormVerticesID(1: string sessionID,
     2: string modelID,
     3: string analysisID,
     4: double timeStep,
     5: string resultID,
     6: string listOfVertices ),
     */


// unimplemented in HBase:
    /**
     returns a session if if the user exists with the specified password and the specified role or an empty role.
     */
    string UserLogin (
      1: string                           user_name,
      2: string                           role,
      3: string                           password),
      

      
      /**
       Stop access to the system by a given session id and release all resources held by that session
       */
      string UserLogout (
	1: string                           sessionID
	),



	// /**
	//  Returns a list of names of data sets that are available from the VELaSSCo platform
	//  and - optionally - their properties.
	//  */
	// rvGetListOfModels GetListOfModels (
	//   1: string                           sessionID
	//   2: string                           groupQualifier
	//   3: string                           modelNamePattern
	//   4: string                           options ),
	// 
	// 
	//   /**
	//    Returns a model GUID (from now on ModelID). The model host may do housekeeping actions,
	//    such as caching, and update its session model accordingly..
	//    */
	//   rvOpenModel OpenModel(
	//     1: string                           sessionID
	//     2: string                           modelName
	//     6: string                           requestedAccess ),


	    /**
	     Description: Removes the possibility to access a model via a previously assigned
	     GUID (OpenModel). Corresponding housekeeping is wrapped up.
	     */
	    string CloseModel(
	      1: string                           sessionID
	      2: string                           modelID ),



	      /**
	       Description: Store a new thumbnail of a model
	       */
	      string SetThumbnailOfAModel (
		1: string                           sessionID
		2: string                           modelID
		3: binary                           imageFile ),


		/**
		 Description: Return thumbnail of a model.
		 */
		rvGetThumbnailOfAModel GetThumbnailOfAModel (
		  1: string                           sessionID
		  2: string                           modelID ),



		  /**
		   For each point in the input parameter points, the method returns data about the element that contains the point.
		   The number of elements in the returned list of elements shall be the same as the number of points in the input parameter.
		   If the method does not find an element for a point it shall return a dummy element with id equal to -1.
		   */
		  rvGetElementOfPointsInSpace GetElementOfPointsInSpace (
		    1: string                           sessionID
		    2: string                           modelName
		    3: list<Point>                      points),


		    /**
		     GetBoundaryOfLocalMesh will get the partial tetrahedral elements, calculate and output the unique triangles,
		     i.e. triangles that are not shared between two or more elements.
		     */
		    rvGetBoundaryOfLocalMesh GetBoundaryOfLocalMesh(
		      1: string                           sessionID
		      2: string                           modelID
		      3: string                           meshID
		      4: string                           analysisID
		      5: double                           time_step ),


		// /**
		//  Given a list of vertices id's from the model, vertexIDs, GetResultFromVerticesID will get
		//  the result value of a given type (resultID), for each vertex id of the list.
		//  */
		// rvGetResultFromVerticesID_B GetResultFromVerticesID(
		// 	1: string                           sessionID
		// 	2: string                           modelID
		// 	3: string                           coordinatesSet
		// 	4: list<i64>                        vertexIDs
		// 	5: string                           resultID
		// 	6: double                           time_step
		// 	7: string                           analysisID ),



}
