#ifndef VELASSCOTHRIFT_H
#define VELASSCOTHRIFT_H

//#include "gen-cpp/VELaSSCo.h"
#include "VELaSSCo.h"

class VELaSSCoHandler : virtual public VELaSSCoIf
{
public:
    VELaSSCoHandler();
    
    /**
     * Return the status of the different services
     * which run on the Data Layer.
     */
    void statusDL(std::string& _return) ;
    
    /**
     *  Return the status of the different services
     *  which run on the Data Layer.
     *   @param string sessionID -
     *  @param string modelID -
     *  @param string sessionID -
     *  @param string analysisID -
     *  @param double timeStep -
     *  @param string resultID -
     *  @param string listOfVertices -
     *  @return string - returns a structured list of avialbe vertices,
     * 					with the attached list of double
     * 					if errors occur the contect is also returned here?
     */
    void GetResultFormVerticesID(std::string& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::string& listOfVertices) ;

    /**
     * Stop Data Layer
     */
    void stopAll();
    
};

#endif
