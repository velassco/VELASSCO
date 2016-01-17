
// ***
// *** Whithin the VELaSSCo AccessLib this server is only for testing purposes.
// ***

// STL
#include <sstream>

// Thrift
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

// Boost
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

// VELaSSCo
// AccessLib.h is only used for the return codes ( enum values)
#include "AccessLib.h"
#include "Helpers.h"

// Generated code
#include "../../thrift/QueryManager/gen-cpp/QueryManager.h"
#include "../../EngineLayer/QueryManager/RealTimeFormat.h"

// the format is shared between the QueryManager, the AccessLibrary and th visualiztion client
// so it's located in the AccessLibrary folder
#include "BoundaryBinaryMesh.h"



#include "DemoServer.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCo;

// global variable server:
TSimpleServer *G_DemoServer = NULL;

// ***************************************************************************
//
// Actual implementation of QueryManager functions.
//
// This is a derived class, so that our own code does not get overwritten
// when the Thrift interface needs to be regenerated.
//
// ***************************************************************************

void QM_DemoServer::UserLogin(UserLogin_Result& _return, const std::string& url, const std::string& name, const std::string& password)
{
  LOGGER                              << std::endl;
  LOGGER << "----- UserLogin() -----" << std::endl;

  LOGGER                                << std::endl;
  LOGGER << "Input:"                    << std::endl;
  LOGGER << "  url      : " << url      << std::endl;
  LOGGER << "  name     : " << name     << std::endl;
  LOGGER << "  password : " << password << std::endl;

  if (   (name == "velassco" && password == "VELaSSCo")
	 || (name == "andreas" && password == "1234")
	 || (name == "miguel" && password == "1234") )
    {
      // Login time
      std::string time = boost::posix_time::to_simple_string(boost::posix_time::second_clock::universal_time());

      // Generate a session id randomly
      SessionID sessionID;
      do
	{
	  const boost::uuids::uuid uuid = boost::uuids::random_generator()();
	  sessionID = *((SessionID*)(uuid.data));
	}
      while (ValidSessionID(sessionID));

      // Store user
      User user = { name, time };
      m_users[sessionID] = user;

      // Return session id
      _return.__set_result( (Result::type)VAL_SUCCESS );
      _return.__set_sessionID( sessionID );
    }		
  else
    {
      _return.__set_result( (Result::type)VAL_USER_NOT_ACCEPTED );
      _return.__set_sessionID( 0 );
    }

  LOGGER                                          << std::endl;
  LOGGER << "Output:"                             << std::endl;
  LOGGER << "  result    : " << _return.result    << std::endl;
  LOGGER << "  sessionID : " << _return.sessionID << std::endl;
}

void QM_DemoServer::UserLogout(UserLogout_Result& _return, const SessionID sessionID)
{
  LOGGER                               << std::endl;
  LOGGER << "----- UserLogout() -----" << std::endl;

  LOGGER                                  << std::endl;
  LOGGER << "Input:"                      << std::endl;
  LOGGER << "  sessionID : " << sessionID << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }

  // Remove session ID
  {
    m_users.erase(sessionID);
  }

  _return.__set_result( (Result::type)VAL_SUCCESS );

  LOGGER                                    << std::endl;
  LOGGER << "Output:"                       << std::endl;
  LOGGER << "  result : " << _return.result << std::endl;
}

void QM_DemoServer::Query(Query_Result& _return, const SessionID sessionID, const std::string& query)
{
  LOGGER                          << std::endl;
  LOGGER << "----- Query() -----" << std::endl;

  LOGGER                                    << std::endl;
  LOGGER << "Input:"                        << std::endl;
  LOGGER << "  sessionID : "   << sessionID << std::endl;
  LOGGER << "  query     : \n" << query     << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }

  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  std::string name       = pt.get<std::string>("name");

  if ( name == "GetResultFromVerticesID") {
    ManageGetResultFromVerticesID( _return, sessionID, query);
  } else if ( name == "GetListOfModels") {
    ManageGetListOfModels( _return, sessionID, query);
  } else if ( name == "GetMeshDrawData") {
    ManageGetMeshDrawData( _return, sessionID, query );
  } else if ( name == "OpenModel") {
    ManageOpenModel( _return, sessionID, query);
  } else if ( name == "CloseModel") {
    ManageCloseModel( _return, sessionID, query);
  } else if ( name == "GetListOfMeshes") {
    ManageGetListOfMeshes( _return, sessionID, query);
  } else if ( name == "GetListOfAnalyses") {
    ManageGetListOfAnalyses( _return, sessionID, query);
  } else if ( name == "GetListOfTimeSteps") {
    ManageGetListOfTimeSteps( _return, sessionID, query);
  } else if ( name == "GetBoundingBox") {
    ManageGetBoundingBox( _return, sessionID, query);
  } else if ( name == "GetBoundaryOfAMesh") {
    ManageGetBoundaryOfAMesh( _return, sessionID, query);
  } else {
    _return.__set_result( (Result::type)VAL_INVALID_QUERY );
    
    LOGGER                                    << std::endl;
    LOGGER << "Output:"                       << std::endl;
    LOGGER << "  result : " << _return.result << std::endl;
    LOGGER << "  Invalid Query" << std::endl;
    
    return;
  }
}

void QM_DemoServer::GetStatusDB( StatusDB_Result &_return, const SessionID sessionID) {
  LOGGER                              << std::endl;
  LOGGER << "----- GetStatusDB() -----" << std::endl;

  LOGGER                                << std::endl;
  LOGGER << "Input:"                      << std::endl;
  LOGGER << "  sessionID : " << sessionID << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }
  // Return session id
  _return.__set_result( (Result::type)VAL_SUCCESS );
  _return.__set_status( (std::string)"Test_Server OK");

  LOGGER                                          << std::endl;
  LOGGER << "Output:"                             << std::endl;
  LOGGER << "  result    : " << _return.result    << std::endl;
  LOGGER << "  status    : " << _return.status << std::endl;
}

void QM_DemoServer::StopVELaSSCo( StopVELaSSCo_Result &_return, const SessionID sessionID) {
  LOGGER                              << std::endl;
  LOGGER << "----- StopVELaSSCo() -----" << std::endl;

  LOGGER                                << std::endl;
  LOGGER << "Input:"                      << std::endl;
  LOGGER << "  sessionID : " << sessionID << std::endl;

  // Check session ID
  if (!ValidSessionID(sessionID))
    {
      _return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

      LOGGER                                    << std::endl;
      LOGGER << "Output:"                       << std::endl;
      LOGGER << "  result : " << _return.result << std::endl;

      return;
    }
  // Return session id
  _return.__set_result( (Result::type)VAL_SUCCESS );
  _return.__set_status( (std::string)"Test_Server Stopped");

  LOGGER                                          << std::endl;
  LOGGER << "Output:"                             << std::endl;
  LOGGER << "  result    : " << _return.result    << std::endl;
  LOGGER << "  status    : " << _return.status << std::endl;

  if ( G_DemoServer) {
    G_DemoServer->stop();
    delete G_DemoServer;
    G_DemoServer = NULL;
  }
}

void QM_DemoServer::ManageGetResultFromVerticesID( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  // std::istringstream ss(query);
  // boost::property_tree::ptree pt;
  // boost::property_tree::read_json(ss, pt);
  // 
  // std::string name       = pt.get<std::string>("name");
  // std::string modelID    = pt.get<std::string>("modelID");
  // std::string resultID   = pt.get<std::string>("resultID");
  // std::string analysisID = pt.get<std::string>("analysisID");
  // std::string vertexIDs  = as_string<size_t>(pt, "vertexIDs");
  // double      timeStep   = pt.get<double>("timeStep");
  // 
  // stringstream listOfVertices;
  // listOfVertices << "{\"id\":" << "[]" /*vertexIDs*/ << "}";
  // 
  // std::stringstream sessionIDStr;
  // sessionIDStr << sessionID;
  // 
  // std::string _return_;
  // 
  // std::cout << "S " << sessionID  << std::endl;
  // std::cout << "M " << modelID    << std::endl;
  // std::cout << "R " << resultID   << std::endl;
  // std::cout << "A " << analysisID << std::endl;
  // std::cout << "V " << vertexIDs  << std::endl;
  // std::cout << "T " << timeStep   << std::endl;
  
  std::vector<int64_t> resultVertexIDs;
  std::vector<double>  resultValues;
  
  // vertex #1
  resultVertexIDs.push_back(6);
  resultValues.push_back(-0.105564);
  resultValues.push_back(-0.287896);
  resultValues.push_back(-0.377642);
  // vertex #2
  resultVertexIDs.push_back(7);
  resultValues.push_back( 0.259839);
  resultValues.push_back(-0.366375);
  resultValues.push_back(-0.377652);
  
  // Pack into string
  std::string result;
  result += "2 3\n";
  result += std::string((char*)(&resultVertexIDs[0]), sizeof(int64_t)*resultVertexIDs.size());
  result += std::string((char*)(&resultValues[0]),    sizeof(double)*resultValues.size());
  _return.__set_data(result); 
  
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
}

void QM_DemoServer::ManageGetListOfModels( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // // Parse query JSON
  // std::istringstream ss(query);
  // boost::property_tree::ptree pt;
  // boost::property_tree::read_json(ss, pt);
  // 
  // std::string name               = pt.get<std::string>("name");
  // std::string group_qualifier    = pt.get<std::string>("groupQualifier");
  // std::string name_pattern       = pt.get<std::string>("namePattern");
  // 
  // std::stringstream sessionIDStr;
  // sessionIDStr << sessionID;
  // 
  // std::cout << "S " << sessionID       << std::endl;
  // std::cout << "G " << group_qualifier << std::endl;
  // std::cout << "P " << name_pattern    << std::endl;
  // 
  // 
  // rvGetListOfModels _return_;
  // queryManagerModule::Instance()->getListOfModels( _return_,
  // 						   sessionIDStr.str(), group_qualifier, name_pattern);
  // 		  
  // std::cout << _return_ << std::endl;

  std::ostringstream oss;
  oss << "NumberOfModels: " << 2 << std::endl;
  oss << "Name: " << "DEM_examples/FluidizedBed_small" << std::endl;
  oss << "FullPath: " << "/localfs/home/velassco/common/simulation_files/DEM_examples/FluidizedBed_small" << std::endl;
  oss << "Name: " << "fine_mesh-ascii_" << std::endl;
  oss << "FullPath: " << "../../../../../VELASSCO-Data/Telescope_128subdomains_ascii" << std::endl;
  _return.__set_data( oss.str());
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}

void QM_DemoServer::ManageOpenModel( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  _return.__set_data( "00112233445566778899aabbccddeeff");
  _return.__set_result( (Result::type)VAL_SUCCESS );

  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}

void QM_DemoServer::ManageCloseModel( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  _return.__set_result( (Result::type)VAL_SUCCESS );
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
}

void QM_DemoServer::ManageGetListOfMeshes( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  /* will be: "NumberOfMeshes: 1234\nName: mesh_1\nElementType: Tetrahedra\n...\nName: model_2..." */
  /* the information returned is ElementType, NumberOfVerticesPerElement, NumberOfVertices, NumberOfElements, Units, Color, ... */
  std::ostringstream oss;
  oss << "NumberOfMeshes: " << 2 << std::endl;
  oss << "Name: " << "Body" << std::endl;
  oss << "ElementType: " << "Triangle" << std::endl;
  oss << "NumberOfVerticesPerElement: " << "3" << std::endl;
  oss << "NumberOfVertices: " << "1234" << std::endl;
  oss << "NumberOfElements: " << "2468" << std::endl;
  oss << "Units: " << "m" << std::endl;
  oss << "Color: " << "#bbbbbb" << std::endl;
  oss << "MeshNumber: " << 1 << std::endl;
  oss << "CoordsName: " << "c000001" << std::endl;
  oss << "Name: " << "Air" << std::endl;
  oss << "ElementType: " << "Tetrahedra" << std::endl;
  oss << "NumberOfVerticesPerElement: " << "4" << std::endl;
  oss << "NumberOfVertices: " << "1234000" << std::endl;
  oss << "NumberOfElements: " << "2468000" << std::endl;
  oss << "Units: " << "m" << std::endl;
  oss << "Color: " << "#2277ee" << std::endl;
  oss << "MeshNumber: " << 2 << std::endl;
  oss << "CoordsName: " << "c000001" << std::endl;
  _return.__set_data( oss.str());
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}

void QM_DemoServer::ManageGetMeshDrawData( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  /* will be: a structure call RealTimeFormat */
  std::ostringstream oss;

  int nFaces = 12;
  int nVertices = 8;

  float vertices[] = 
  {

    // # Vertex 0
    -1.0f,  -1.0f,  -1.0f,
    -0.57f, -0.57f, -0.57f,

    // # Vertex 1
     1.0f,  -1.0f,  -1.0f,
     0.57f, -0.57f, -0.57f,

    // # Vertex 2
     1.0f,  -1.0f,   1.0f,
     0.57f, -0.57f,  0.57f,

    // # Vertex 3
    -1.0f,  -1.0f,   1.0f,
    -0.57f, -0.57f,  0.57f,

    // # Vertex 4
    -1.0f,   1.0f,  -1.0f,
    -0.57f,  0.57f, -0.57f,

    // # Vertex 5
     1.0f,   1.0f,  -1.0f,
     0.57f,  0.57f, -0.57f,

    // # Vertex 6
     1.0f,   1.0f,   1.0f,
     0.57f,  0.57f,  0.57f,

    // # Vertex 7
    -1.0f,   1.0f,   1.0f,
    -0.57f,  0.57f,  0.57f,

  };
  int faces [] = 
  {
    // # Strip 0
    0, 4, 1, 5, 2, 6, 3, 7, 0, 4, -1,

    // # Strip 1
    0, 1, 3, 2, -1,

    // # Strip 2
    4, 5, 7, 6, -1,
  };

  char description[] =
    "# TEST PLY DATA                    \n"
    "VertexDefinition = position, normal\n"
    "vertexDefinitionType = float       \n"

    "OgLPrimitiveRestartIndex = -1      \n";

  VELaSSCo::RTFormat::File file;

  file.header.magic[0] = 'V';
  file.header.magic[1] = 'E';
  file.header.magic[2] = 'L';
  file.header.magic[3] = 'a';
  file.header.magic[4] = 'S';
  file.header.magic[5] = 'S';
  file.header.magic[6] = 'C';
  file.header.magic[7] = 'o';

  file.header.version = 100;
  file.header.descriptionBytes = sizeof(description);

  file.header.metaBytes = 0;

  file.header.vertexDefinitionsBytes  = sizeof(vertices);
  file.header.vertexAttributesBytes   = 0;
  file.header.edgeDefinitionsBytes    = 0;
  file.header.edgeAttributesBytes     = 0;
  file.header.faceDefinitionsBytes    = sizeof(faces);
  file.header.faceAttributesBytes     = 0;
  file.header.cellDefinitionsBytes    = 0;
  file.header.cellAttributesBytes     = 0;

  file.data.description.resize(file.header.descriptionBytes);
  file.data.meta.resize(0);
  file.data.vertexDefinitions.resize(file.header.vertexDefinitionsBytes);
  file.data.vertexAttributes.resize(0);
  file.data.edgeDefinitions.resize(0);
  file.data.edgeAttributes.resize(0);
  file.data.faceDefinitions.resize(file.header.faceDefinitionsBytes);
  file.data.faceAttributes.resize(0);
  file.data.cellDefinitions.resize(0);
  file.data.cellAttributes.resize(0);

  memcpy( (char*)file.data.description.data(), (const char*)description, file.header.vertexDefinitionsBytes );
  memcpy( (char*)file.data.vertexDefinitions.data(), (const char*)vertices, file.header.vertexDefinitionsBytes );
  memcpy( (char*)file.data.faceDefinitions.data(), (const char*)faces, file.header.faceDefinitionsBytes );

  // Pack into string
  oss << file;  

  _return.__set_data( oss.str() ); 
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}

void QM_DemoServer::ManageGetListOfAnalyses( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  /* will be: "Analysis name 1\nAnalysis name 2\n...\nAnalysis name N" */
  std::ostringstream oss;
  oss << "DEM" << std::endl;
  oss << "Geometry" << std::endl;
  oss << "Time analysis" << std::endl;
  _return.__set_data( oss.str());
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}

void QM_DemoServer::ManageGetListOfTimeSteps( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  /* will be: "TimeStep 1\nTimeStep 2\n...\nTimeStep N" */
  std::ostringstream oss;
  oss << "0.0" << std::endl;
  oss << "10000" << std::endl;
  oss << "20000" << std::endl;
  oss << "30000" << std::endl;
  _return.__set_data( oss.str());
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}

void QM_DemoServer::ManageGetListOfResults( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  /* will be: "NumberOfResults: 1234\nName: result_1\nResultType: Scalar\n...\nName: model_2..." */
  /* the information returned is ResultType, NumberOfComponents, ComponentNames, Location, GaussPointName, CoordinatesName, Units, ... */
  std::ostringstream oss;
  oss << "NumberOfResults: 3" << std::endl;
  oss << "Name: PartitionID" << std::endl;
  oss << "ResultType: Scalar" << std::endl;
  oss << "NumberOfComponents: 1" << std::endl;
  oss << "ComponentNames: \"\"" << std::endl;
  oss << "Location: OnNodes" << std::endl;
  oss << "GaussPointName: \"\"" << std::endl;
  oss << "Units: \"\"" << std::endl;
  oss << "Name: Velocity" << std::endl;
  oss << "ResultType: Vector" << std::endl;
  oss << "NumberOfComponents: 3" << std::endl;
  oss << "ComponentNames: \"v-x\", \"v-y\", \"v-z\"" << std::endl;
  oss << "Location: OnNodes" << std::endl;
  oss << "GaussPointName: \"\"" << std::endl;
  oss << "Units: \"m/s\"" << std::endl;
  oss << "Name: Vorticity" << std::endl;
  oss << "ResultType: Scalar" << std::endl;
  oss << "NumberOfComponents: 1" << std::endl;
  oss << "ComponentNames: \"\"" << std::endl;
  oss << "Location: OnNodes" << std::endl;
  oss << "GaussPointName: \"\"" << std::endl;
  oss << "Units: \"1/s\"" << std::endl;
  _return.__set_data( oss.str());
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
}


void QM_DemoServer::ManageGetBoundingBox( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  double bbox[ 6] = { -0.5, -0.5, -0.5, 0.5, 0.5, 0.5};
  _return.__set_data( std::string( ( const char *)&bbox[ 0], 6 * sizeof( double)));
  _return.__set_result( (Result::type)VAL_SUCCESS );

  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  LOGGER << "  data   : \n" << Hexdump( _return.data, 128) << std::endl;
}

void QM_DemoServer::ManageGetBoundaryOfAMesh( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  VELaSSCo::BoundaryBinaryMesh::MeshPoint lst_vertices[] = {
    {  1, {  0.707107,  0.707107,  0.000000}},
    {  2, {  0.000000,  0.707107, -0.707107}},
    {  3, { -0.000000,  0.707107,  0.707107}},
    {  4, { -0.707107,  0.707107, -0.000000}},
    {  5, {  1.000000, -0.000000,  0.000000}},
    {  6, {  0.000000, -0.000000, -1.000000}},
    {  7, { -0.000000, -0.000000,  1.000000}},
    {  8, { -0.707107, -0.000000, -0.707107}},
    {  9, { -1.000000, -0.000000, -0.000000}},
    { 10, { -0.707107, -0.000000,  0.707107}},
    { 11, {  0.707107, -0.000000, -0.707107}},
    { 12, {  0.707107, -0.000000,  0.707107}},
    { 13, {  0.000000, -0.707107, -0.707107}},
    { 14, { -0.000000, -0.707107,  0.707107}},
    { 15, {  0.707107, -0.707107,  0.000000}},
    { 16, { -0.707107, -0.707107, -0.000000}},
    { 17, {  0.000000, -1.000000,  0.000000}},
    { 18, {  0.000000,  1.000000,  0.000000}}
  };
  VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle lst_triangles[] = {
    { 3, { 17, 13, 16}},
    { 3, {  6,  8, 13}},
    { 3, {  8,  9, 16}},
    { 3, {  8, 16, 13}},
    { 3, {  8,  6,  2}},
    { 3, {  4,  9,  8}},
    { 3, {  2, 18,  4}},
    { 3, {  2,  4,  8}},
    { 3, { 17, 16, 14}},
    { 3, {  9, 10, 16}},
    { 3, { 10,  7, 14}},
    { 3, { 10, 14, 16}},
    { 3, { 10,  9,  4}},
    { 3, {  3,  7, 10}},
    { 3, {  4, 18,  3}},
    { 3, {  4,  3, 10}},
    { 3, { 17, 15, 13}},
    { 3, {  5, 11, 15}},
    { 3, { 11,  6, 13}},
    { 3, { 11, 13, 15}},
    { 3, { 11,  5,  1}},
    { 3, {  2,  6, 11}},
    { 3, {  1, 18,  2}},
    { 3, {  1,  2, 11}},
    { 3, { 17, 14, 15}},
    { 3, {  7, 12, 14}},
    { 3, { 12,  5, 15}},
    { 3, { 12, 15, 14}},
    { 3, { 12,  7,  3}},
    { 3, {  1,  5, 12}},
    { 3, {  3, 18,  1}},
    { 3, {  3,  1, 12}}
  };
  VELaSSCo::BoundaryBinaryMesh demo_mesh;
  int64_t num_vertices = ( int64_t)( sizeof( lst_vertices) / sizeof( VELaSSCo::BoundaryBinaryMesh::MeshPoint));
  int64_t num_triangles = ( int64_t)( sizeof( lst_triangles) / sizeof( VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle));
  demo_mesh.set( lst_vertices, num_vertices, lst_triangles, num_triangles, VELaSSCo::BoundaryBinaryMesh::STATIC);

  std::string demo_mesh_str( demo_mesh.toString());

  _return.__set_data( demo_mesh_str);
  _return.__set_result( (Result::type)VAL_SUCCESS );

  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  LOGGER << "  mesh : " << num_vertices << " vertices and " << num_triangles << " triangles" << std::endl;
  LOGGER << "         ( " << demo_mesh_str.length() << " bytes)" << std::endl;
  LOGGER << "     sizeof() MeshPoint = " << sizeof( VELaSSCo::BoundaryBinaryMesh::MeshPoint) 
	 << ", BoundaryTriangle = " << sizeof( VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle) << std::endl;
  LOGGER << "  data   : \n" << Hexdump( _return.data, 128) << std::endl;
}


int StartServer( const int server_port) {
  LOGGER << "Starting VELaSSCo Server..." << std::endl;

  int port = server_port;
  LOGGER << "  using port: " << port << std::endl;

  shared_ptr<QM_DemoServer> handler(new QM_DemoServer());
  shared_ptr<TProcessor> processor(new QueryManagerProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  // TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  G_DemoServer = new TSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
  DEBUG( "  before serving ...");
  G_DemoServer->serve();
  DEBUG( "  after serving ...");
  return 0;
}
