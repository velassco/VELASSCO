
#pragma once

// STD
#include <string>

namespace VELaSSCo
{

/**
 * 
 * This class provides a wrapper for different database management systems
 * such as EDM or HBase.
 * 
 */

class AbstractDB
{
	
public:

  /* 
   * status of the DB engines: HBase or EDM
   */
  virtual std::string getStatusDB() = 0;

    /*
     * Access the database and return a list o vertices with attributes.
	 * The result is a string. Each line represents a vertex and is defined as:
	 * 
	 * <VertexID> <X-coord> <Y-coord> <Z-coord> <attribute 1> <attribute 2> ...
	 * 
	 * For example:
	 * 
	 *  0  0.334 1.236 5.9385  2.5
	 *  1  7.334 0.236 4.9385  1.5
	 * 
	 * describes two vertices with IDs 0 and 1. Here just one attribute per vertex is used.
	 * 
	 */

	virtual std::string getResultOnVertices( std::string sessionID,
                                             std::string modelID,
                                             std::string analysisID,
                                             double      timeStep,
                                             std::string resultID,
                                             std::string listOfVertices ) = 0;
};

}
