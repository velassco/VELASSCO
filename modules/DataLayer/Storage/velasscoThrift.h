/* -*- c++ -*- */
#ifndef VELASSCOTHRIFT_H
#define VELASSCOTHRIFT_H

//#include "gen-cpp/VELaSSCo.h"
#include "VELaSSCoSM.h"
#include "storageModule.h"

using namespace VELaSSCoSM;

class VELaSSCoHandler : virtual public VELaSSCoSMIf
{
public:
  VELaSSCoHandler( const DL_SM_DB_TYPE db_type, const char *db_host, const int db_port);
  ~VELaSSCoHandler();

  /**
   * Return the status of the different services
   * which run on the Data Layer.
   */
  void statusDL(std::string& _return) ;

  /**
   * Data Query operations
   * 
   * @param sessionID
   * @param model_group_qualifier
   * @param model_name_pattern
   */
  void GetListOfModelNames(rvGetListOfModels &_return, const std::string &sessionID, const std::string &model_group_qualifier, const std::string &model_name_pattern) ;
  /* unique_mode_name_pattern is of the form 
   * model name pattern (hbase = TableName:Properties-fp:Properties-nm) 
   *     options: ModelName, /Full/Path:ModelName or TableName:/Full/Path:ModelName
   */
  void FindModel(rvOpenModel &_return, const std::string &sessionID, const std::string &unique_model_name_pattern, const std::string &requested_access);
    
  /**
   *  Return the status of the different services
   *  which run on the Data Layer.
   *   @param string sessionID -
   *  @param string modelID -
   *  @param string sessionID -
   *  @param string analysisID -
   *  @param double timeStep -
   *  @param string resultID -
   *  @param string listOfVertices -
   *  @return string - returns a structured list of avialbe vertices,
   * 					with the attached list of double
   * 					if errors occur the contect is also returned here?
   */
  void GetResultFromVerticesID(std::string &_return, 
			       const std::string &sessionID, const std::string &modelID, 
			       const std::string &analysisID, const double timeStep, 
			       const std::string &resultID, const std::string &listOfVertices) ;
			       
  void GetCoordinatesAndElementsFromMesh(std::string& _return, 
				   const std::string& sessionID, const std::string& modelID, 
				   const std::string& analysisID, const double timeStep, const int32_t partitionID);
				   
 // void GetCoordinatesAndElementsFromMesh(std::string& _return, 
 //                  const std::string& sessionID, const std::string& modelID, 
//                   const std::string& analysisID, const double timeStep, const int32_t partitionID);		     

  void GetListOfMeshes( rvGetListOfMeshes &_return, 
			const std::string &sessionID, const std::string &modelID, 
			const std::string &analysisID, const double stepValue);

  void GetListOfAnalyses( rvGetListOfAnalyses &_return, 
			  const std::string &sessionID, const std::string &modelID);
  void GetListOfTimeSteps( rvGetListOfTimeSteps &_return, 
			   const std::string &sessionID, const std::string &modelID, 
			   const std::string &analysisID);


  /**
   * Stop Data Layer
   */
  void stopAll();
    
  /* To be implemented */
  void UserLogin(std::string&, const std::string&, const std::string&, const std::string&) {};
  void UserLogout(std::string&, const std::string&) {};
  void CloseModel(std::string&, const std::string&, const std::string&) {};
  void SetThumbnailOfAModel(std::string&, const std::string&, const std::string&, const std::string&) {};
  void GetThumbnailOfAModel(rvGetThumbnailOfAModel&, const std::string&, const std::string&) {};
  void GetElementOfPointsInSpace(rvGetElementOfPointsInSpace&, const std::string&, const std::string&, const std::vector<Point, std::allocator<Point> >&) {};
  void GetBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh&, const std::string&, const std::string&, const std::string&, const std::string&, double) {};

};

#endif
