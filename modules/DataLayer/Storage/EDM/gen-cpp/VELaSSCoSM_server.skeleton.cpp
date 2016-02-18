// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "VELaSSCoSM.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCoSM;

class VELaSSCoSMHandler : virtual public VELaSSCoSMIf {
 public:
  VELaSSCoSMHandler() {
    // Your initialization goes here
  }

  /**
   * Return the status of the different services
   * which run on the Data Layer.
   */
  void statusDL(std::string& _return) {
    // Your implementation goes here
    printf("statusDL\n");
  }

  /**
   * Data Query operations
   * 
   * @param sessionID
   * @param model_group_qualifier
   * @param model_name_pattern
   */
  void GetListOfModelNames(rvGetListOfModels& _return, const std::string& sessionID, const std::string& model_group_qualifier, const std::string& model_name_pattern) {
    // Your implementation goes here
    printf("GetListOfModelNames\n");
  }

  void FindModel(rvOpenModel& _return, const std::string& sessionID, const std::string& unique_model_name_pattern, const std::string& requested_access) {
    // Your implementation goes here
    printf("FindModel\n");
  }

  /**
   * Return the status of the different services
   * which run on the Data Layer.
   * @return string - returns a structured list of avialbe vertices,
   * with the attached list of double
   * if errors occur the contect is also returned here?
   * 
   * @param sessionID
   * @param modelID
   * @param analysisID
   * @param timeStep
   * @param resultID
   * @param listOfVertices
   */
  void GetResultFromVerticesID(rvGetResultFromVerticesID& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::vector<int64_t> & listOfVertices) {
    // Your implementation goes here
    printf("GetResultFromVerticesID\n");
  }

  /**
   * Return the coordinates and elements of a model's mesh.
   * @return string - returns a structured list of vertices and elements of a model's mesh.
   * if errors occur the contect is also returned here?
   * 
   * @param sessionID
   * @param modelID
   * @param analysisID
   * @param timeStep
   * @param meshInfo
   */
  void GetCoordinatesAndElementsFromMesh(rvGetCoordinatesAndElementsFromMesh& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const MeshInfo& meshInfo) {
    // Your implementation goes here
    printf("GetCoordinatesAndElementsFromMesh\n");
  }

  /**
   * Stop Data Layer
   */
  void stopAll() {
    // Your implementation goes here
    printf("stopAll\n");
  }

  /**
   * as of OP-22.117
   * Returns a list of meshes present for the given time-step of that analysis.
   * If analysis == "" and step-value == -1 then the list will be of the 'static' meshes.
   * If analysis != "" and step-value != -1 then the list will be of the 'dynamic' meshes
   * that are present on that step-values of that analysis.
   * 
   * @param sessionID
   * @param modelID
   * @param analysisID
   * @param stepValue
   */
  void GetListOfMeshes(rvGetListOfMeshes& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double stepValue) {
    // Your implementation goes here
    printf("GetListOfMeshes\n");
  }

  /**
   * as of OP-22.112
   * Retrieves the list of time steps for a given model and analysis.
   * 
   * @param sessionID
   * @param modelID
   */
  void GetListOfAnalyses(rvGetListOfAnalyses& _return, const std::string& sessionID, const std::string& modelID) {
    // Your implementation goes here
    printf("GetListOfAnalyses\n");
  }

  /**
   * Retrieves the list of time steps for a given model and analysis.
   * 
   * @param sessionID
   * @param modelID
   * @param analysisID
   */
  void GetListOfTimeSteps(rvGetListOfTimeSteps& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID) {
    // Your implementation goes here
    printf("GetListOfTimeSteps\n");
  }

  /**
   * Retrieves the list of results for a given model, analysis and step-value
   * as of OP-22.115
   * 
   * @param sessionID
   * @param modelID
   * @param analysisID
   * @param stepValue
   */
  void GetListOfResultsFromTimeStepAndAnalysis(rvGetListOfResults& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double stepValue) {
    // Your implementation goes here
    printf("GetListOfResultsFromTimeStepAndAnalysis\n");
  }

  /**
   * Extract a list of vertices from the open model and the selected meshID
   * as of OP-22.116
   * 
   * @param sessionID
   * @param modelID
   * @param analysisID
   * @param stepValue
   * @param meshID
   */
  void GetListOfVerticesFromMesh(rvGetListOfVerticesFromMesh& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double stepValue, const int32_t meshID) {
    // Your implementation goes here
    printf("GetListOfVerticesFromMesh\n");
  }

  /**
   * returns a session if if the user exists with the specified password and the specified role or an empty role.
   * 
   * @param user_name
   * @param role
   * @param password
   */
  void UserLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password) {
    // Your implementation goes here
    printf("UserLogin\n");
  }

  /**
   * Stop access to the system by a given session id and release all resources held by that session
   * 
   * @param sessionID
   */
  void UserLogout(std::string& _return, const std::string& sessionID) {
    // Your implementation goes here
    printf("UserLogout\n");
  }

  /**
   * Description: Removes the possibility to access a model via a previously assigned
   * GUID (OpenModel). Corresponding housekeeping is wrapped up.
   * 
   * @param sessionID
   * @param modelID
   */
  void CloseModel(std::string& _return, const std::string& sessionID, const std::string& modelID) {
    // Your implementation goes here
    printf("CloseModel\n");
  }

  /**
   * Description: Store a new thumbnail of a model
   * 
   * @param sessionID
   * @param modelID
   * @param imageFile
   */
  void SetThumbnailOfAModel(std::string& _return, const std::string& sessionID, const std::string& modelID, const std::string& imageFile) {
    // Your implementation goes here
    printf("SetThumbnailOfAModel\n");
  }

  /**
   * Description: Return thumbnail of a model.
   * 
   * @param sessionID
   * @param modelID
   */
  void GetThumbnailOfAModel(rvGetThumbnailOfAModel& _return, const std::string& sessionID, const std::string& modelID) {
    // Your implementation goes here
    printf("GetThumbnailOfAModel\n");
  }

  /**
   * For each point in the input parameter points, the method returns data about the element that contains the point.
   * The number of elements in the returned list of elements shall be the same as the number of points in the input parameter.
   * If the method does not find an element for a point it shall return a dummy element with id equal to -1.
   * 
   * @param sessionID
   * @param modelName
   * @param points
   */
  void GetElementOfPointsInSpace(rvGetElementOfPointsInSpace& _return, const std::string& sessionID, const std::string& modelName, const std::vector<Point> & points) {
    // Your implementation goes here
    printf("GetElementOfPointsInSpace\n");
  }

  /**
   * GetBoundaryOfLocalMesh will get the partial tetrahedral elements, calculate and output the unique triangles,
   * i.e. triangles that are not shared between two or more elements.
   * 
   * @param sessionID
   * @param modelID
   * @param meshID
   * @param analysisID
   * @param time_step
   */
  void GetBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& _return, const std::string& sessionID, const std::string& modelID, const std::string& meshID, const std::string& analysisID, const double time_step) {
    // Your implementation goes here
    printf("GetBoundaryOfLocalMesh\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<VELaSSCoSMHandler> handler(new VELaSSCoSMHandler());
  shared_ptr<TProcessor> processor(new VELaSSCoSMProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

