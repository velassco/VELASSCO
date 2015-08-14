/*
 * Licensed ...
 * V 0.0 : BL - First thrift definition   
 */

/**
   Namespace dl (Data Layer Interface)
 */
namespace cpp dli




enum ElementType {
  LINE = 1,
  TETRAHEDRA = 2,
  TRIANGLE = 3,
  SPHERE = 4
}


struct Point {
   1: double                              x
   2: double                              y
   3: double                              z
}

struct Node {
   1: i64                                 id
   2: double                              x
   3: double                              y
   4: double                              z
}

struct Element {
   1: i64                                 id
   2: list<Node>                          nodes
}

struct Mesh {
   1: string                              name     
   2: i64                                 numberOfNodes
   3: i64                                 dimension
   4: ElementType                         type
   5: list<Node>                          nodes
   6: list<Element>                       elements
}

struct Triangle {
   1: list<Node>                          nodes
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


service VELaSSCo 
{
	
	/**
	Return the status of the different services 
	which run on the Data Layer.
	@return string - returns a structured list of avialbe vertices, 
		with the attached list of double 
		if errors occur the contect is also returned here?
   	*/
	string	GetResultFormVerticesID(1: string sessionID,
									2: string modelID,
								 	3: string analysisID,
								 	4: double timeStep,
								 	5: string resultID,
								 	6: string listOfVertices ),

/**
   returns a session if if the user exists with the specified password and the specified role or an empty role.
 */
   string UserLogin (
      1: string                           user_name,
      2: string                           role,
      3: string                           password)



/**
   Stop access to the system by a given session id and release all resources held by that session
 */
   string UserLogout (
      1: string                           sessionID)


/**
   For each point in the input parameter points, the method returns data about the element that contains the point.
   The number of elements in the returned list of elements shall be the same as the number of points in the input parameter.
   If the method does not find an element for a point it shall return a dummy element with id equal to -1.
 */
   rvGetElementOfPointsInSpace GetElementOfPointsInSpace (
      1: string                           sessionID
      2: string                           modelName
      3: list<Point>                      points)


/**
   GetBoundaryOfLocalMesh will get the partial tetrahedral elements, calculate and output the unique triangles,
   i.e. triangles that are not shared between two or more elements.
 */
   rvGetBoundaryOfLocalMesh GetBoundaryOfLocalMesh(
      1: string                           sessionID
      2: string                           modelName
      3: string                           meshID
      4: string                           analysisID
      5: double                           time_step )


}