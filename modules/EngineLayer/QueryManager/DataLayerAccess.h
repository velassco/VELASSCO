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
    void getResultFromVerticesID( std::string& _return, 
				  std::string sessionID, std::string modelID, std::string analysisID, 
				  double timeStep, std::string resultID,  std::string listOfVertices) ;
    void getListOfModels( rvGetListOfModels &_return,
			  const std::string &sessionID, 
			  const std::string &group_qualifier, const std::string &name_pattern);
    void openModel( rvOpenModel &_return,
		    const std::string &sessionID, 
		    const std::string &unique_name, const std::string &requested_access);
    void getListOfTimeSteps( std::string& _return,
			     const std::string &sessionID,
			     const std::string &analysisID,
			     const std::string &stepOptions, const int numSteps, const double *lstSteps);
			   
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
