
// ***
// *** Whithin the VELaSSCo AccessLib this server is only for testing purposes.
// ***

#include <assert.h>

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
#include "../../AccessLib/AccessLib/AccessLib.h"
#include "Helpers.h"

// Generated code
#include "../../thrift/QueryManager/gen-cpp/QueryManager.h"

#include "DataLayerAccess.h"
#include "Analytics.h"
#include "Graphics.h"

#include "Server.h"

// Testing includes
#include "RealTimeFormat.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCo;

/* 1xx - Direct Result Queries */

/* first queries */

void QueryManagerServer::ManageGetResultFromVerticesID( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  std::string name       = pt.get<std::string>("name");
  std::string modelID    = pt.get<std::string>("modelID");
  std::string resultID   = pt.get<std::string>("resultID");
  std::string analysisID = pt.get<std::string>("analysisID");
  std::string vertexIDs  = as_string< size_t>( pt, "vertexIDs");
  double      timeStep   = pt.get<double>("timeStep");
  
  stringstream listOfVertices;
  listOfVertices << "{\"id\":" << "[]" /*vertexIDs*/ << "}";
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  // std::cout << "S   " << sessionID        << std::endl;
  // std::cout << "dlS " << dl_sessionID     << std::endl;
  
  std::string _return_;
  
  //std::cout << "S " << sessionID  << std::endl;
  //std::cout << "M " << modelID    << std::endl;
  //std::cout << "R " << resultID   << std::endl;
  //std::cout << "A " << analysisID << std::endl;
  //std::cout << "V " << vertexIDs  << std::endl;
  //std::cout << "T " << timeStep   << std::endl;
  
  DataLayerAccess::Instance()->getResultFromVerticesID(_return_ , dl_sessionID,modelID ,analysisID ,timeStep ,resultID ,listOfVertices.str());
  
  std::vector<int64_t> resultVertexIDs;
  std::vector<double>  resultValues;
		  
  std::cout << _return_ << std::endl;
		  
  std::istringstream iss(_return_);
  while (iss)
    {
      int64_t id;
      double  value[3];
		      
      std::string line;
      std::getline(iss, line);
      std::istringstream(line) >> id >> value[0] >> value[1] >> value[2];
		      
      resultVertexIDs.push_back(id);
      resultValues.push_back(value[0]);
      resultValues.push_back(value[1]);
      resultValues.push_back(value[2]);
    }
		  
  // Pack into string
  std::string result;
  std::ostringstream oss;
  oss << (resultValues.size() / 3) << " 3" << "\n";
  result += oss.str();
  result += std::string((char*)(&resultVertexIDs[0]), sizeof(int64_t)*resultVertexIDs.size());
  result += std::string((char*)(&resultValues[0]),    sizeof(double)*resultValues.size());
  _return.__set_data(result); 
		  
  // Generate example result data
  if (0)
    {
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
    }
		  
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
}

void QueryManagerServer::ManageGetMeshDrawData( Query_Result& _return, const SessionID sessionID, const std::string& query ) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);

  std::string name        = pt.get<std::string>("name");
  std::string modelID     = pt.get<std::string>("modelID");
  //std::string resultID    = pt.get<std::string>("resultID");
  std::string analysisID  = pt.get<std::string>("analysisID");
  unsigned    partitionID = pt.get<unsigned>("partitionID");
  double      timeStep    = pt.get<double>("timeStep");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  // std::cout << "S   " << sessionID        << std::endl;
  // std::cout << "dlS " << dl_sessionID     << std::endl;
  
  std::string _return_;
  
  //std::cout << "S " << sessionID  << std::endl;
  //std::cout << "M " << modelID    << std::endl;
  //std::cout << "R " << resultID   << std::endl;
  //std::cout << "A " << analysisID << std::endl;
  //std::cout << "V " << vertexIDs  << std::endl;
  //std::cout << "T " << timeStep   << std::endl;
  
  std::string simulation_data_table_name = GetModelsTableName( sessionID, modelID.c_str());
  std::cout << "simulation data table = " << simulation_data_table_name << endl;
  
  const bool test = false;
  if(test){
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

	  file.data.description       = (uint8_t*) description;
	  file.data.meta              = 0;

	  file.data.vertexDefinitions = (uint8_t*)vertices;
	  file.data.vertexAttributes  = 0;
	  file.data.edgeDefinitions   = 0;
	  file.data.edgeAttributes    = 0;
	  file.data.faceDefinitions   = (uint8_t*)faces;
	  file.data.faceAttributes    = 0;
	  file.data.cellDefinitions   = 0;
	  file.data.cellAttributes    = 0;

	  // Pack into string
	  std::ostringstream oss;
	  oss << file;  

	  _return.__set_data( oss.str() ); 
	  _return.__set_result( (Result::type)VAL_SUCCESS );
  } else {
	  DataLayerAccess::Instance()->getCoordinatesAndElementsFromMesh( _return_, dl_sessionID, modelID ,analysisID ,timeStep ,partitionID);
	  
	  const int num_vertices = 2704;
	  std::vector<float> vertices( 6 * num_vertices );
	  std::vector<int>   faces;
	  
	  size_t i = 0;
	  while( i < _return_.size() ){
		  
		  if(_return_[i] == 't'){
			  
			  int64_t indices[3];
			  indices[0] = static_cast<int>(*((int64_t*)(&_return_[i+1 ])));
			  indices[1] = static_cast<int>(*((int64_t*)(&_return_[i+9 ])));
			  indices[2] = static_cast<int>(*((int64_t*)(&_return_[i+17])));
			  
			  faces.push_back(static_cast<int>(indices[0]));
			  faces.push_back(static_cast<int>(indices[1]));
			  faces.push_back(static_cast<int>(indices[2]));
			  faces.push_back(-1);
			  
			  //cout << "Indices = " << indices[0] << " " << indices[1] << " " << indices[2] << endl;
			  
			  
			  
			  i += 25;
			  
		  }
		  if(_return_[i] == 'v'){
			  
			  int64_t indx = *((int64_t*)(&_return_[i+1]));
			  
			  double coords[3];
			  coords[0] = static_cast<float>(*((double*)(&_return_[i+9 ])));
			  coords[1] = static_cast<float>(*((double*)(&_return_[i+17])));
			  coords[2] = static_cast<float>(*((double*)(&_return_[i+25])));
			  
			  vertices[6 * indx + 0] = coords[0];
			  vertices[6 * indx + 1] = coords[1];
			  vertices[6 * indx + 2] = coords[2];
			  vertices[6 * indx + 3] = 1.0f;
			  vertices[6 * indx + 4] = 1.0f;
			  vertices[6 * indx + 5] = 1.0f;
			  
			  
			  //cout << "Coordinates[ " << indx << " ] = " << coords[0] << " " << coords[1] << " " << coords[2] << endl;
			  
			  i += 33;
		  }
		  
	  }
	  
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

	  file.header.vertexDefinitionsBytes  = vertices.size() * sizeof(float);
	  file.header.vertexAttributesBytes   = 0;
	  file.header.edgeDefinitionsBytes    = 0;
	  file.header.edgeAttributesBytes     = 0;
	  file.header.faceDefinitionsBytes    = faces.size() * sizeof(int);
	  file.header.faceAttributesBytes     = 0;
	  file.header.cellDefinitionsBytes    = 0;
	  file.header.cellAttributesBytes     = 0;

	  file.data.description       = (uint8_t*) description;
	  file.data.meta              = 0;

	  file.data.vertexDefinitions = (uint8_t*)vertices.data();
	  file.data.vertexAttributes  = 0;
	  file.data.edgeDefinitions   = 0;
	  file.data.edgeAttributes    = 0;
	  file.data.faceDefinitions   = (uint8_t*)faces.data();
	  file.data.faceAttributes    = 0;
	  file.data.cellDefinitions   = 0;
	  file.data.cellAttributes    = 0;

	  // Pack into string
	  std::ostringstream oss;
	  oss << file;  

	  _return.__set_data( oss.str() ); 
	  _return.__set_result( (Result::type)VAL_SUCCESS );
	  
  }

  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
}

static const char *getStrFromElementType( const ElementShapeType::type &elem) {
  const char *str_elem = "Unkown";
  if ( elem == ElementShapeType::type::UnknownElement)	             str_elem = "Unknown";	      	
  else if ( elem == ElementShapeType::type::PointElement)	     str_elem = "Point";	        
  else if ( elem == ElementShapeType::type::LineElement)	     str_elem = "Line";	        
  else if ( elem == ElementShapeType::type::TriangleElement)	     str_elem = "Triangle";	        
  else if ( elem == ElementShapeType::type::QuadrilateralElement)    str_elem = "Quadrilateral";    
  else if ( elem == ElementShapeType::type::TetrahedraElement)       str_elem = "Tetrahedra";       
  else if ( elem == ElementShapeType::type::HexahedraElement)	     str_elem = "Hexahedra";        
  else if ( elem == ElementShapeType::type::PrismElement)	     str_elem = "Prism";	        
  else if ( elem == ElementShapeType::type::PyramidElement)	     str_elem = "Pyramid";	        
  else if ( elem == ElementShapeType::type::SphereElement)	     str_elem = "Sphere";	        
  else if ( elem == ElementShapeType::type::CircleElement)	     str_elem = "Circle";	        
  else if ( elem == ElementShapeType::type::ComplexParticleElement)  str_elem = "ComplexParticle";  
  return str_elem;
}

void QueryManagerServer::ManageGetListOfMeshes( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  
  std::string name       = pt.get<std::string>("name");
  std::string modelID    = pt.get<std::string>( "modelID");
  std::string analysisID = pt.get<std::string>( "analysisID");
  double stepValue       = pt.get< double>( "stepValue");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;

  rvGetListOfMeshes _return_;
  DataLayerAccess::Instance()->getListOfMeshes( _return_,
						dl_sessionID, modelID, analysisID, stepValue);
  
  std::cout << _return_ << std::endl;

  if ( _return_.status == "Error") {
    const std::string not_found( "Not found");
    if ( AreEqualNoCaseSubstr( _return_.report, not_found, not_found.size())) {
      _return.__set_result( (Result::type)VAL_NO_MESH_INFORMATION_FOUND);
    } else {
      _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    }
    _return.__set_data( _return_.report);
  } else { // status == "Ok"
    if ( _return_.meshInfos.size() == 0) {
      _return.__set_result( (Result::type)VAL_NO_MODEL_MATCHES_PATTERN);
    } else {
      _return.__set_result( (Result::type)VAL_SUCCESS );
      // process data:
      // foreach model in _return_.meshInfos
      //   return { model.name, model.fullpath}
      // _return.__set_data( _return_.meshInfos.srt());
      std::ostringstream oss;
      oss << "NumberOfMeshes: " << _return_.meshInfos.size() << std::endl;
      // C++11 : for ( auto &it : _return_.meshInfos)
      // char hex_string[ 1024];
      for ( std::vector< MeshInfo>::iterator it = _return_.meshInfos.begin();
          it != _return_.meshInfos.end(); it++) {
	oss << "Name: " << it->name << std::endl;
	oss << "ElementType: " << getStrFromElementType( it->elementType.shape) << std::endl;
	oss << "NumberOfVerticesPerElement: " <<  it->elementType.num_nodes << std::endl;
	oss << "NumberOfVertices: " << it->nVertices << std::endl;
	oss << "NumberOfElements: " << it->nElements << std::endl;
	oss << "Units: " << it->meshUnits << std::endl;
	oss << "Color: " << it->meshColor << std::endl;
	oss << "MeshNumber: " << it->meshNumber << std::endl;

      }
      _return.__set_data( oss.str());
    }
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
} // ManageGetListOfMeshes

void QueryManagerServer::ManageGetListOfAnalyses( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  
  std::string name       = pt.get<std::string>("name");
  std::string modelID    = pt.get<std::string>( "modelID");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;

  rvGetListOfAnalyses _return_;
  DataLayerAccess::Instance()->getListOfAnalyses( _return_,
						  dl_sessionID, modelID);
  
  std::cout << _return_ << std::endl;
  
  if ( _return_.status == "Error") {
    const std::string not_found( "Not found");
    if ( AreEqualNoCaseSubstr( _return_.report, not_found, not_found.size())) {
      _return.__set_result( (Result::type)VAL_NO_ANALYSIS_INFORMATION_FOUND);
    } else {
      _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    }
    _return.__set_data( _return_.report);
  } else { // status == "Ok"
    if ( _return_.analyses.size() == 0) {
      _return.__set_result( (Result::type)VAL_NO_MODEL_MATCHES_PATTERN);
    } else {
      _return.__set_result( (Result::type)VAL_SUCCESS );
      // process data:
      std::ostringstream oss;
      for ( std::vector< std::string>::iterator it = _return_.analyses.begin();
          it != _return_.analyses.end(); it++) {
	oss << *it << std::endl;
      }
      _return.__set_data( oss.str());
    }
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;

} // ManageGetListOfAnalyses

void QueryManagerServer::ManageGetListOfTimeSteps( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  
  std::string name       = pt.get<std::string>( "name");
  std::string modelID    = pt.get<std::string>( "modelID"); 
  std::string analysisID = pt.get<std::string>( "analysisID");
 
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;

  rvGetListOfTimeSteps _return_;
  DataLayerAccess::Instance()->getListOfTimeSteps( _return_,
						   dl_sessionID, modelID, analysisID,
						   "ALL", 0, NULL);
  
  // std::cout << _return_ << std::endl;
  
  if ( _return_.status == "Error") {
    const std::string not_found( "Not found");
    if ( AreEqualNoCaseSubstr( _return_.report, not_found, not_found.size())) {
      _return.__set_result( (Result::type)VAL_NO_STEP_INFORMATION_FOUND);
    } else {
      _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    }
    _return.__set_data( _return_.report);
  } else { // status == "Ok"
    if ( _return_.time_steps.size() == 0) {
      _return.__set_result( (Result::type)VAL_NO_MODEL_MATCHES_PATTERN);
    } else {
      _return.__set_result( (Result::type)VAL_SUCCESS );
      // process data:
      // std::ostringstream oss;
      // // has to be passed as binary !!!!
      // for ( std::vector< double>::iterator it = _return_.time_steps.begin();
      //     it != _return_.time_steps.end(); it++) {
      // 	oss << *it << std::endl;
      // }
      // _return.__set_data( oss.str());
      _return.__set_data( std::string( ( char *)_return_.time_steps.data(), _return_.time_steps.size() * sizeof( double)));
    }
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << Hexdump( _return.data) << std::endl;
  LOGGER << "  # steps: "   << _return_.time_steps.size() << std::endl;
  if ( _return_.time_steps.size() > 0)
    LOGGER << "  step 0 : " << _return_.time_steps[ 0] << std::endl;
  if ( _return_.time_steps.size() > 1)
    LOGGER << "  step 1 : " << _return_.time_steps[ 1] << std::endl;
  if ( _return_.time_steps.size() > 2)
    LOGGER << "  step 2 : " << _return_.time_steps[ 2] << std::endl;

} // ManageGetListOfTimeSteps

void QueryManagerServer::ManageGetListOfResults( Query_Result &_return, const SessionID sessionID, const std::string& query) {
  // Parse query JSON
  std::istringstream ss(query);
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss, pt);
  
  std::string name       = pt.get<std::string>("name");
  std::string modelID    = pt.get<std::string>( "modelID");
  std::string analysisID = pt.get<std::string>( "analysisID");
  double stepValue       = pt.get< double>( "stepValue");
  
  std::string dl_sessionID = GetDataLayerSessionID( sessionID);

  std::cout << "S   " << sessionID        << std::endl;
  std::cout << "dlS " << dl_sessionID     << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;

  rvGetListOfResults _return_;
  DataLayerAccess::Instance()->getListOfResultsFromTimeStepAndAnalysis( _return_,
									dl_sessionID, 
									modelID, analysisID, stepValue);
  
  std::cout << _return_ << std::endl;

  if ( _return_.status == "Error") {
    const std::string not_found( "Not found");
    if ( AreEqualNoCaseSubstr( _return_.report, not_found, not_found.size())) {
      _return.__set_result( (Result::type)VAL_NO_RESULT_INFORMATION_FOUND);
    } else {
      _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
    }
    _return.__set_data( _return_.report);
  } else { // status == "Ok"
    if ( _return_.result_list.size() == 0) {
      _return.__set_result( (Result::type)VAL_NO_MODEL_MATCHES_PATTERN);
    } else {
      _return.__set_result( (Result::type)VAL_SUCCESS );
      // process data:
      // foreach model in _return_.result_list
      //   return { model.name, model.fullpath}
      // _return.__set_data( _return_.result_list.srt());
      std::ostringstream oss;
      oss << "NumberOfResults: " << _return_.result_list.size() << std::endl;
      // C++11 : for ( auto &it : _return_.result_list)
      
      /* the information returned is ResultType, NumberOfComponents, ComponentNames, Location, GaussPointName, CoordinatesName, Units, ... */
      // char hex_string[ 1024];
      for ( std::vector< ResultInfo>::iterator it = _return_.result_list.begin();
          it != _return_.result_list.end(); it++) {
	oss << "Name: " << it->name << std::endl;
	oss << "ResultType: " << it->type << std::endl;
	oss << "NumberOfComponents: " <<  it->numberOfComponents << std::endl;
	oss << "ComponentNames: ";
	for ( size_t i = 0; i < it->componentNames.size(); i++) {
	  if ( i) oss << ", ";
	  oss << it->componentNames[ i];
	}
	oss << std::endl;
	oss << "Location: " << it->location << std::endl;
	oss << "GaussPointName: " << it->gaussPointName << std::endl;
	oss << "CoordinatesName: " << it->coordinatesName << std::endl;
	oss << "Units: " << it->units << std::endl;
	// not yet ...
	oss << "ResultNumber: " << it->resultNumber << std::endl;
      }
      _return.__set_data( oss.str());
    }
  }
		  
  LOGGER                                             << std::endl;
  LOGGER << "Output:"                                << std::endl;
  LOGGER << "  result : "   << _return.result        << std::endl;
  // LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
  LOGGER << "  data   : \n" << _return.data << std::endl;
} // ManageGetListOfMeshes
