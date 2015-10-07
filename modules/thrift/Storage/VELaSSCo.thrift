/*
 * Licensed ...
 * V 0.0 : BL - First thrift definition   
 */

// as of OP-22.055 GetListOfModelNames
struct FullyQualifiedModelName {
   1: string name
   2: string full_path
   3: binary modelID
}

// as of OP-22.055 GetListOfModelNames
struct rvGetListOfModels {
   1: string                              status // OK / Error
   2: string                              report // Error report
   3: list< FullyQualifiedModelName>      models
}

service VELaSSCo 
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
    3: string model_name_pattern),      // model name pattern ( Properties:nm)

  /**
   Return the status of the different services 
   which run on the Data Layer.
   @return string - returns a structured list of avialbe vertices, 
   with the attached list of double 
   if errors occur the contect is also returned here?
   */
  string	GetResultFromVerticesID(1: string sessionID,
    2: string modelID,
    3: string analysisID,
    4: double timeStep,
    5: string resultID,
    6: string listOfVertices ),
    /**
     Stop Data Layer
     */							 	
    void stopAll()
  }
