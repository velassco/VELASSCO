
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
};

} 
