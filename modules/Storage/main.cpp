#include <stdlib.h>
#include <stdio.h>

#include "VELaSSCo.h"
#include "hbaseaccess.h"

#include "hbaseaccess.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace dli; // 'VELaSSCo.h'

class VELaSSCoHandler : virtual public VELaSSCoIf {
public:
    
    hbaseaccess* hba;
    VELaSSCoHandler()
    {
        hba = new hbaseaccess();
    }
    
    /**
     * Return the status of the different services
     * which run on the Data Layer.
     * @return string - returns a structured list of avialbe vertices,
     * 	with the attached list of double
     * 	if errors occur the contect is also returned here?
     *
     * @param sessionID
     * @param modelID
     * @param analysisID
     * @param timeStep
     * @param resultID
     * @param listOfVertices
     */
  void GetResultFormVerticesID(std::string& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::string& listOfVertices)
    {
        // Your implementation goes here
      vector< int>lst_v_ids;
      // parse listOfVertices into lst_v_ids
      _return = hba->GetResultOnVerticesId( ( const char *)sessionID.data(), ( const char *)modelID.data(), ( const char *)analysisID.data(), timeStep, ( const char *)resultID.data(), 
					    lst_v_ids.data(), lst_v_ids.size());
    }
    
  /**
   * returns a session if if the user exists with the specified password and the specified role or an empty role.
   * 
   * @param user_name
   * @param role
   * @param password
   */
  void UserLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password) {
  }

  /**
   * Stop access to the system by a given session id and release all resources held by that session
   * 
   * @param sessionID
   */
  void UserLogout(std::string& _return, const std::string& sessionID) {
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
  }

  /**
   * GetBoundaryOfLocalMesh will get the partial tetrahedral elements, calculate and output the unique triangles,
   * i.e. triangles that are not shared between two or more elements.
   * 
   * @param sessionID
   * @param modelName
   * @param meshID
   * @param analysisID
   * @param time_step
   */
  void GetBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& _return, const std::string& sessionID, const std::string& modelName, const std::string& meshID, const std::string& analysisID, const double time_step) {
  }

};

using namespace std;
int main(int argc, char **argv)
{
    int port = 6664;
    boost::shared_ptr<VELaSSCoHandler> handler(new VELaSSCoHandler());
    boost::shared_ptr<TProcessor> processor(new VELaSSCoProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
    
    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();
    return 0;
      return 0;
}


