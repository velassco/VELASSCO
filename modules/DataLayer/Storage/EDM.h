
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

	std::string getResultOnVertices( std::string sessionID,
                                     std::string modelID,
                                     std::string analysisID,
                                     double      timeStep,
                                     std::string resultID,
                                     std::string listOfVertices );
};

} 
