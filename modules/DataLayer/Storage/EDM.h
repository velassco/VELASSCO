/* -*- c++ -*- */
#pragma once

// STD
#include <string>

//VELaSSCo
#include "AbstractDB.h"

namespace VELaSSCo
{
  
  class EDM : public AbstractDB
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
  };
  
} 
