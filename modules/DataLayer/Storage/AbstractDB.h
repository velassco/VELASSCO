/* -*- c++ -*- */
#pragma once

// STD
#include <string>
#include <vector>

/////////////////////////////////
// Thrift
/////////////////////////////////
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

/////////////////////////////////
// HBase Thrift 1
/////////////////////////////////
#include "Hbase.h"
using namespace  ::apache::hadoop::hbase::thrift;
typedef std::map<std::string,TCell> CellMap;

#include "VELaSSCoSM.h"
using namespace VELaSSCoSM;
// class FullyQualifiedModelName;

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

    virtual bool startConnection( const char *DB_hostname, const int DB_port) = 0;
    virtual bool stopConnection() = 0;

    /* 
     * status of the DB engines: HBase or EDM
     */
    virtual std::string getStatusDB() = 0;

    virtual std::string getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					     const std::string &sessionID, const std::string &model_group_qualifier, 
					     const std::string &model_name_pattern) = 0;
    virtual std::string findModel( std::string &report, std::string &modelID, FullyQualifiedModelName &model_info,
				   const std::string &sessionID, const std::string &unique_model_name_pattern, 
				   const std::string &requested_access) = 0;
    virtual std::string getListOfMeshes( std::string &report, std::vector< MeshInfo> &listOfMeshes,
					 const std::string &sessionID, const std::string &modelID,
					 const std::string &analysisID, const double stepValue) = 0;
    virtual std::string getListOfAnalyses( std::string &report, std::vector< std::string> &listOfAnalyses,
					   const std::string &sessionID, const std::string &modelID) = 0;
    virtual std::string getListOfSteps( std::string &report, std::vector< double> &listOfSteps,
					const std::string &sessionID, const std::string &modelID,
					const std::string &analysisID) = 0;

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
    virtual std::string getResultOnVertices( const std::string &sessionID,
                                             const std::string &modelID,
                                             const std::string &analysisID,
                                             const double       timeStep,
                                             const std::string &resultID,
                                             const std::string &listOfVertices ) = 0;
                                             
    /*
     * Access the database and return a list o vertices, and elements which have created the mesh of a dataset.
     * The result is a string. Each line represents a vertex and is defined as:
     * 
     * <Num_Vertices> <X-coord 0> <Y-coord 0> <Z-coord 0> <X-coord 1> <Y-coord 1> <Z-coord 1> <X-coord 2> <Y-coord 2> <Z-coord 2> <Num_Indices> <Index 0> <Index 1> <Index 2> ...
     * 
     * For example:
     * 
     *  4 -1 -1 -1 1 -1 -1 1 1 -1 -1 1 -1
     *  5 1 0 2 3 -1
     * 
     * describes a plane made of 4 vertices, and 2 faces. -1 in the end declares end of TRIANGLE_STRIP.
     * 
     */                                         
    virtual std::string getCoordinatesAndElementsFromMesh(const std::string &sessionID,
                                             const std::string &modelID,
                                             const std::string &analysisID,
                                             const double       timeStep,
                                             const std::string &resultID) = 0;
  };

}
