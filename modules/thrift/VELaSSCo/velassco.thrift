/*
 * Licensed ...
 * V 0.0 : BL - First thrift definition   
 */

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

}