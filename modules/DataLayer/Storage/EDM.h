/* -*- c++ -*- */
#pragma once

// STD
#include <string>

//VELaSSCo
#include "AbstractDB.h"

// EDMClient to be defined by EDM's thrift interface ....
class EDMClient;

namespace VELaSSCo
{
  
  class EDM : public AbstractDB
  {
    
  public:
    EDM(): _edm_client( NULL) {}
    ~EDM() { stopConnection(); }

    bool startConnection( const char *DB_hostname, const int DB_port);
    bool stopConnection();

    std::string getStatusDB();
    
    std::string getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
				     const std::string &sessionID, const std::string &model_group_qualifier, 
				     const std::string &model_name_pattern);
    std::string findModel( std::string &report, std::string &modelID, FullyQualifiedModelName &model_info,
			   const std::string &sessionID, const std::string &unique_model_name_pattern, const std::string &requested_access);

    std::string getListOfMeshes( std::string &report, std::vector< MeshInfo> &listOfMeshes,
				 const std::string &sessionID, const std::string &modelID,
				 const std::string &analysisID, const double stepValue) {
      std::string resultString( "Not implemented");
      return resultString;
    }
    
    std::string getResultOnVertices( const std::string &sessionID,
                                     const std::string &modelID,
                                     const std::string &analysisID,
                                     const double       timeStep,
                                     const std::string &resultID,
                                     const std::string &listOfVertices );
  private:
    EDMClient *_edm_client;
    boost::shared_ptr<TTransport> _socket;
    boost::shared_ptr<TTransport> _transport;
    boost::shared_ptr<TProtocol> _protocol;  
  };
} 
