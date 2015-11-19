
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
#include "test/plydatareader.h"
#include "test/RealTimeFormat.h"

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
  //std::string vertexIDs  = as_string< size_t>( pt, "vertexIDs");
  double      timeStep   = pt.get<double>("timeStep");
  
  //stringstream listOfVertices;
  //listOfVertices << "{\"id\":" << "[]" /*vertexIDs*/ << "}";
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::string _return_;
  
  //std::cout << "S " << sessionID  << std::endl;
  //std::cout << "M " << modelID    << std::endl;
  //std::cout << "R " << resultID   << std::endl;
  //std::cout << "A " << analysisID << std::endl;
  //std::cout << "V " << vertexIDs  << std::endl;
  //std::cout << "T " << timeStep   << std::endl;
  
  //DataLayerAccess::Instance()->getResultFromVerticesID(_return_ ,sessionIDStr.str() ,modelID ,analysisID ,timeStep ,resultID ,listOfVertices.str());
  
  std::vector<int64_t> resultVertexIDs;
  std::vector<double>  resultValues;
		  
  std::cout << _return_ << std::endl;
		  
//  std::istringstream iss(_return_);
//  while (iss)
//    {
//      int64_t id;
//      double  value[3];
//		      
//      std::string line;
//      std::getline(iss, line);
//      std::istringstream(line) >> id >> value[0] >> value[1] >> value[2];
//		      
//      resultVertexIDs.push_back(id);
//      resultValues.push_back(value[0]);
//      resultValues.push_back(value[1]);
//      resultValues.push_back(value[2]);
//    }
		  
  // Pack into string
//  std::string result;
//  std::ostringstream oss;
//  oss << (resultValues.size() / 3) << " 3" << "\n";
//  result += oss.str();
//  result += std::string((char*)(&resultVertexIDs[0]), sizeof(int64_t)*resultVertexIDs.size());
//  result += std::string((char*)(&resultValues[0]),    sizeof(double)*resultValues.size());
//  _return.__set_data(result); 
//		  
//  // Generate example result data
//  if (0)
//    {
//      std::vector<int64_t> resultVertexIDs;
//      std::vector<double>  resultValues;
//		      
//      // vertex #1
//      resultVertexIDs.push_back(6);
//      resultValues.push_back(-0.105564);
//      resultValues.push_back(-0.287896);
//      resultValues.push_back(-0.377642);
//      // vertex #2
//      resultVertexIDs.push_back(7);
//      resultValues.push_back( 0.259839);
//      resultValues.push_back(-0.366375);
//      resultValues.push_back(-0.377652);
//		      
//      // Pack into string
//      std::string result;
//      result += "2 3\n";
//      result += std::string((char*)(&resultVertexIDs[0]), sizeof(int64_t)*resultVertexIDs.size());
//      result += std::string((char*)(&resultValues[0]),    sizeof(double)*resultValues.size());
//      _return.__set_data(result); 
//    }
		  
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

  std::string name       = pt.get<std::string>("name");
  std::string modelID    = pt.get<std::string>("modelID");
  std::string resultID   = pt.get<std::string>("resultID");
  std::string analysisID = pt.get<std::string>("analysisID");
  std::string vertexIDs  = as_string< size_t>( pt, "vertexIDs");
  double      timeStep   = pt.get<double>("timeStep");
  
  stringstream listOfVertices;
  listOfVertices << "{\"id\":" << "[]" /*vertexIDs*/ << "}";
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::string _return_;
  
  //std::cout << "S " << sessionID  << std::endl;
  //std::cout << "M " << modelID    << std::endl;
  //std::cout << "R " << resultID   << std::endl;
  //std::cout << "A " << analysisID << std::endl;
  //std::cout << "V " << vertexIDs  << std::endl;
  //std::cout << "T " << timeStep   << std::endl;
  
  bool test = true;
  if(test){
    PlyDataReader::getSingletonPtr()->readDataInfo("cow.ply", nullptr, 0);
    int nFaces = PlyDataReader::getSingletonPtr()->getNumFaces();
    int nVertices = PlyDataReader::getSingletonPtr()->getNumVertices();

    float* vertices = new float[6 * nVertices]; // position, normal.
    int* faces = new int[4 * nFaces];
    int* indices = new int[3 * nFaces];

    PlyDataReader::getSingletonPtr()->readData(vertices, indices);

    // every strip.
    for (int i = 0; i < nFaces; i++){
      faces[4 * i + 0] = indices[3 * i + 0];
      faces[4 * i + 1] = indices[3 * i + 1];
      faces[4 * i + 2] = indices[3 * i + 2];
      faces[4 * i + 3] = -1;
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

    file.header.vertexDefinitionsBytes = sizeof(vertices);
    file.header.vertexAttributesBytes = 0;
    file.header.edgeDefinitionsBytes = 0;
    file.header.edgeDefinitionsBytes = 0;
    file.header.faceDefinitionsBytes = sizeof(faces);
    file.header.faceDefinitionsBytes = 0;
    file.header.cellDefinitionsBytes = 0;
    file.header.cellDefinitionsBytes = 0;

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
    std::string result;
    std::ostringstream oss;
    result += std::string((char*)(&file.header),                  sizeof(VELaSSCo::RTFormat::Header));
    result += std::string((char*)(&file.data.description),        file.header.descriptionBytes);
    result += std::string((char*)(&file.data.vertexDefinitions),  file.header.vertexDefinitionsBytes);
    result += std::string((char*)(&file.data.vertexAttributes),   file.header.vertexAttributesBytes);
    result += std::string((char*)(&file.data.edgeDefinitions),    file.header.edgeDefinitionsBytes);
    result += std::string((char*)(&file.data.edgeAttributes),     file.header.edgeAttributesBytes);
    result += std::string((char*)(&file.data.faceDefinitions),    file.header.faceDefinitionsBytes);
    result += std::string((char*)(&file.data.faceAttributes),     file.header.faceAttributesBytes);
    result += std::string((char*)(&file.data.cellDefinitions),    file.header.cellDefinitionsBytes);
    result += std::string((char*)(&file.data.cellAttributes),     file.header.cellAttributesBytes);

    _return.__set_data(result); 

    // clean up the data
    delete[] faces;
    delete[] vertices;
    delete[] indices;
  }
		  
  _return.__set_result( (Result::type)VAL_SUCCESS );
		  
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
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::cout << "S  -" << sessionID        << "-" << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;
  std::cout << "An -" << analysisID       << "-" << std::endl;
  std::cout << "Sv -" << stepValue       << "-" << std::endl;

  rvGetListOfMeshes _return_;
  DataLayerAccess::Instance()->getListOfMeshes( _return_,
						sessionIDStr.str(), modelID, analysisID, stepValue);
  
  std::cout << _return_ << std::endl;

  if ( _return_.status == "Error") {
    if ( _return_.report == "No models") {
      _return.__set_result( (Result::type)VAL_NO_MODELS_IN_PLATFORM);
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
      oss << "NumberOfModels: " << _return_.meshInfos.size() << std::endl;
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
  
  std::stringstream sessionIDStr;
  sessionIDStr << sessionID;
  
  std::cout << "S  -" << sessionID        << "-" << std::endl;
  std::cout << "M  -" << modelID          << "-" << std::endl;

  rvGetListOfAnalyses _return_;
  DataLayerAccess::Instance()->getListOfAnalyses( _return_,
						  sessionIDStr.str(), modelID);
  
  std::cout << _return_ << std::endl;
  
  if ( _return_.status == "Error") {
    if ( _return_.report == "No models") {
      _return.__set_result( (Result::type)VAL_NO_MODELS_IN_PLATFORM);
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

