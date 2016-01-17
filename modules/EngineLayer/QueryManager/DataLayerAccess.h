/* -*- c++ -*- */
#ifndef DataLayerAccess_H
#define DataLayerAccess_H

#include <iostream> // pour std::cout
#include <string>   // pour std::string


#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;
using namespace std;


#include "VELaSSCoSM.h"

using namespace VELaSSCoSM;

// handles connection to Storage Module:
class DataLayerAccess
{
public:
    static DataLayerAccess *Instance();
    bool startConnection( const char *data_layer_hostname, const int data_layer_port);
    bool stopConnection();

    void getStatusDB( std::string& _return);
    void getResultFromVerticesID( rvGetResultFromVerticesID &_return, 
				  const std::string &sessionID, const std::string &modelID, 
				  const std::string &analysisID, const double timeStep,
				  const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID ); 
	void getCoordinatesAndElementsFromMesh( rvGetCoordinatesAndElementsFromMesh& _return,
                  std::string sessionID, std::string modelID, std::string analysisID, double timeStep, int32_t meshID ) ;
  // session queries
  
    void userLogin( std::string &_return, const std::string &name, const std::string &role, const std::string &password);
    void userLogout( std::string &_return, const std::string &sessionID);
    
    void getListOfModels( rvGetListOfModels &_return,
			  const std::string &sessionID, 
			  const std::string &group_qualifier, const std::string &name_pattern);
    void openModel( rvOpenModel &_return,
		    const std::string &sessionID, 
		    const std::string &unique_name, const std::string &requested_access);
  void closeModel( std::string &_return,
		   const std::string &sessionID, 
		   const std::string &modelID);
  // direct result queries
    void getListOfMeshes( rvGetListOfMeshes &_return,
			  const std::string &sessionID,
			  const std::string &modelID,
			  const std::string &analysisID,
			  const double stepValue);
    void getListOfAnalyses( rvGetListOfAnalyses &_return,
			    const std::string &sessionID,
			    const std::string &modelID);
  /* as of OP-22.113 */
    void getListOfTimeSteps( rvGetListOfTimeSteps &_return,
			     const std::string &sessionID,
			     const std::string &modelID,
			     const std::string &analysisID,
			     const std::string &stepOptions, const int numSteps, const double *lstSteps);
  /* as of OP-22.115 */
  void getListOfResultsFromTimeStepAndAnalysis( rvGetListOfResults &_return,
						const std::string &sessionID,
						const std::string &modelID,
						const std::string &analysisID,
						const double stepValue);
  /* as of OP-22.116 */
  void getListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
				  const std::string &sessionID, const std::string &modelID, 
				  const std::string &analysisID, const double stepValue, 
				  const int32_t meshID);

    void stopAll();
    
private:
    DataLayerAccess(){};
    DataLayerAccess(DataLayerAccess const&){};
  // DataLayerAccess& operator=(DataLayerAccess const&){};
    
    static DataLayerAccess *m_pInstance;
    VELaSSCoSMClient *cli;
    
    
    boost::shared_ptr<TTransport> socket;
    boost::shared_ptr<TTransport> transport;
    boost::shared_ptr<TProtocol> protocol;
};

#endif
