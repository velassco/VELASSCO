/* -*- c++ -*- */
#pragma once

// STD
#include <string>

//VELaSSCo
#include "AbstractDB.h"

namespace VELaSSCo
{

  class HBase : public AbstractDB
  {
  
  public:
    HBase(): _hbase_client( NULL), _socket( NULL), _transport( NULL), _protocol( NULL) {}
    ~HBase() { stopConnection(); }

    bool startConnection( const char *DB_hostname, const int DB_port);
    bool stopConnection();

    std::string getStatusDB();

    std::string getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
				     const std::string &sessionID, const std::string &model_group_qualifier, 
				     const std::string &model_name_pattern);
    std::string getListOfModelNames_curl( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					  const std::string &sessionID, const std::string &model_group_qualifier, 
					  const std::string &model_name_pattern);
    std::string getListOfModelNames_thrift( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					    const std::string &sessionID, const std::string &model_group_qualifier, 
					    const std::string &model_name_pattern);

    std::string getResultOnVertices( const std::string &sessionID,  const std::string &modelID, 
				     const std::string &analysisID, const double       timeStep,  
				     const std::string &resultID,   const std::string &listOfVertices );
    std::string getResultOnVertices_curl( const std::string &sessionID,  const std::string &modelID, 
					  const std::string &analysisID, const double       timeStep,  
					  const std::string &resultID,   const std::string &listOfVertices );
    std::string getResultOnVertices_thrift( const std::string &sessionID,  const std::string &modelID, 
					    const std::string &analysisID, const double       timeStep,  
					    const std::string &resultID,   const std::string &listOfVertices );
  private:

    double fRand(double fMin, double fMax);
    std::string parseStatusDB( std::string b);
    std::string parse1DEM(std::string b, std::string LOVertices);
    // returns true if there are models on the table to be parsed
    bool parseListOfModelNames( std::string &report,
				std::vector< FullyQualifiedModelName> &listOfModelNames,
				std::string buffer);

  private:
    HbaseClient *_hbase_client;
    boost::shared_ptr<TTransport> *_socket;
    boost::shared_ptr<TTransport> *_transport;
    boost::shared_ptr<TProtocol> *_protocol;
  };

  typedef std::vector<std::string> StrVec;
} // namespace VELaSSCo

void printRow(const TRowResult &rowResult);
