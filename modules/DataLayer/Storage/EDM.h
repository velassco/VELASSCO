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
    std::string getListOfAnalyses( std::string &report, std::vector< std::string> &listOfAnalyses,
				   const std::string &sessionID, const std::string &modelID) {
      std::string resultString( "Not implemented");
      return resultString;
    }
    std::string getListOfSteps( std::string &report, std::vector< double> &listOfSteps,
				const std::string &sessionID, const std::string &modelID,
				const std::string &analysisID) {
      std::string resultString( "Not implemented");
      return resultString;
    }
    std::string getListOfResults( std::string &report, std::vector< ResultInfo> &listOfResults,
				  const std::string &sessionID, const std::string &modelID,
				  const std::string &analysisID, const double stepValue) {
      std::string resultString( "Not implemented");
      return resultString;
    }

    std::string getListOfVerticesFromMesh( std::string &report, std::vector< Vertex> &listOfVertices,
					   const std::string &sessionID, const std::string &modelID, 
					   const std::string &analysisID, const double stepValue, 
					   const int32_t meshID) {
      std::string resultString( "Not implemented");
      return resultString;
    }
    
    std::string getResultOnVertices( const std::string &sessionID,
                                     const std::string &modelID,
                                     const std::string &analysisID,
                                     const double       timeStep,
                                     const std::string &resultID,
                                     const std::string &listOfVertices );
                                     
    std::string getCoordinatesAndElementsFromMesh( const std::string &sessionID,
                                     const std::string &modelID,
                                     const std::string &analysisID,
                                     const double       timeStep,
                                     const int32_t      partitionID    );
                                         
  private:
    EDMClient *_edm_client;
    boost::shared_ptr<TTransport> _socket;
    boost::shared_ptr<TTransport> _transport;
    boost::shared_ptr<TProtocol> _protocol;  
  };
} 
