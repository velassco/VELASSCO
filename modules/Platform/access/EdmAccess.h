/* -*- c++ -*- */
#ifndef EDMACCESS_H
#define EDMACCESS_H

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


#include "VELaSSCo_Operations.h"
#include "VELaSSCoSM.h"

using namespace VELaSSCoSM;

// handles connection to Storage Module:
class EdmAccess : public VELaSSCo_Operations
{
public:
    static EdmAccess *Instance();

    bool startConnection( const char *data_layer_hostname, const int data_layer_port);
    bool stopConnection();

    void getStatusDB( std::string& _return);
    void getResultFromVerticesID( rvGetResultFromVerticesID &_return, 
				  const std::string &sessionID, const std::string &modelID, 
				  const std::string &analysisID, const double timeStep,
				  const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID ); 
	void getCoordinatesAndElementsFromMesh( rvGetCoordinatesAndElementsFromMesh& _return,
                  const std::string& sessionID, const std::string& modelID, 
                  const std::string& analysisID, const double timeStep, const MeshInfo& meshID ) ;
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

    void getListOfTimeSteps( rvGetListOfTimeSteps &_return,
			     const std::string &sessionID,
			     const std::string &modelID,
			     const std::string &analysisID,
			     const std::string &stepOptions, const int numSteps, const double *lstSteps);

	void getListOfResultsFromTimeStepAndAnalysis( rvGetListOfResults &_return,
						const std::string &sessionID,
						const std::string &modelID,
						const std::string &analysisID,
						const double stepValue);

	void getListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
				  const std::string &sessionID, const std::string &modelID, 
				  const std::string &analysisID, const double stepValue, 
				  const int32_t meshInfo);

//    void stopAll();
    void calculateBoundingBox( const std::string &sessionID, const std::string &modelID, const std::string &dataTableName,
			     const std::string &analysisID, const int numSteps, const double *lstSteps,
			     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			     double *return_bbox, std::string *return_error_str);

  void calculateDiscrete2Continuum( const std::string &sessionID, const std::string &modelID,
				    const std::string &analysisName, const std::string &staticMeshID, const std::string &tSOptions, 
				    const int numSteps, const double *lstSteps, const std::string &coarseGrainingMethod, 
				    const double width, const double cutoffFactor,
				    const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGoptions, 
				    const double deltaT, const bool doSpatialIntegral, const std::string &integralDimension, const std::string &integralDirection,
				    std::string *returnQueryOutcome, std::string *return_error_str);
  
  void calculateBoundaryOfAMesh( const std::string &sessionID, const std::string &DataLayer_sessionID,
				 const std::string &modelID, const std::string &dataTableName,
				 const int meshID, const std::string &elementType,
				 const std::string &analysisID, const double stepValue,
				 std::string *return_binary_mesh, std::string *return_error_str);
    
    
    
  	//EdmAccess(){};
  	
private:
    EdmAccess(){};
    EdmAccess(EdmAccess const&){};
    
    static EdmAccess *m_pInstance;
    VELaSSCoSMClient *cli;
    
    
    boost::shared_ptr<TTransport> socket;
    boost::shared_ptr<TTransport> transport;
    boost::shared_ptr<TProtocol> protocol;
    
};

#endif
