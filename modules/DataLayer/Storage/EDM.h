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
