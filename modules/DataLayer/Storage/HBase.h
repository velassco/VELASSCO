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
  
    std::string getStatusDB();

    std::string getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
				     const std::string &sessionID, const std::string &model_group_qualifier, 
				     const std::string &model_name_pattern);

    std::string getResultOnVertices( std::string sessionID,
                                     std::string modelID,
                                     std::string analysisID,
                                     double      timeStep,
                                     std::string resultID,
                                     std::string listOfVertices );
  private:

    double fRand(double fMin, double fMax);
    std::string parseStatusDB( std::string b);
    std::string parse1DEM(std::string b, std::string LOVertices);
    // returns true if there are models on the table to be parsed
    bool parseListOfModelNames( std::string &report,
				std::vector< FullyQualifiedModelName> &listOfModelNames,
				std::string buffer);
  };

} 
