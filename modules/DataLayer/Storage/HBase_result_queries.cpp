#include <stdio.h>

// CLib
#include <cmath>
#include <cstddef>

// STD
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Curl
#include <curl/curl.h>

// VELaSSCo
#include "HBase.h"
#include "VELaSSCoSM.h" // for class FullyQualifiedModelName
#include "cJSON.h"
#include "base64.h"

#include "Curl_cmd.h"
#include "Helpers.h"
#include "Extras.h"

using namespace std;
using namespace VELaSSCo;

/////////////////////////////////
// Curl + Json
/////////////////////////////////
#include <curl/curl.h>
#include "cJSON.h"

// remember also to actualize the QueryManager/Server_direct_result_queries.cpp
static ElementShapeType::type getElementTypeFromStr( const std::string &str) {
  const char *str_elem = str.c_str();
  ElementShapeType::type ret( ElementShapeType::type::UnknownElement); // it !found return this
  if ( !strcasecmp( str_elem, "Unknown"))	      	ret = ElementShapeType::type::UnknownElement;
  else if ( !strcasecmp( str_elem, "Point"))	        ret = ElementShapeType::type::PointElement;
  else if ( !strcasecmp( str_elem, "Line"))	        ret = ElementShapeType::type::LineElement;
  else if ( !strcasecmp( str_elem, "Triangle"))	        ret = ElementShapeType::type::TriangleElement;
  else if ( !strcasecmp( str_elem, "Quadrilateral"))    ret = ElementShapeType::type::QuadrilateralElement;
  else if ( !strcasecmp( str_elem, "Tetrahedra"))       ret = ElementShapeType::type::TetrahedraElement;
  else if ( !strcasecmp( str_elem, "Hexahedra"))        ret = ElementShapeType::type::HexahedraElement;
  else if ( !strcasecmp( str_elem, "Prism"))	        ret = ElementShapeType::type::PrismElement;
  else if ( !strcasecmp( str_elem, "Pyramid"))	        ret = ElementShapeType::type::PyramidElement;
  else if ( !strcasecmp( str_elem, "Sphere"))	        ret = ElementShapeType::type::SphereElement;
  else if ( !strcasecmp( str_elem, "Circle"))	        ret = ElementShapeType::type::CircleElement;
  else if ( !strcasecmp( str_elem, "ComplexParticle"))  ret = ElementShapeType::type::ComplexParticleElement;
  return ret;
}

static bool getMeshInfoFromRow( std::map< int, MeshInfo> &map_mesh_info, const TRowResult &rowResult) {
  // M:un for units
  // M:c* prefix for c123456nm c123456nc for name of coordinates set and number of coordinates = vertices = nodes
  // M:N* prefix fir m123456nm (name) m123456cn (coord.set.name) m123456et (elemType)
  //                 m123456ne (numberOfElements) m123456nn (numNodesPerElem) m123456cl (color)
  // MeshInfo has:
  //   std::string name;
  //   ElementType elementType;
  //   int64_t nVertices;
  //   int64_t nElements;
  //   std::string meshUnits;
  //   std::string meshColor;
  //   int32_t meshNumber;
  
  int num_meshes = 0;
  std::string units( "");
  for ( CellMap::const_iterator it = rowResult.columns.begin(); 
	it != rowResult.columns.end(); ++it) {
    if ( it->first == "M:un") {
      units = it->second.value;
      continue;
    }
    const char *cq_str = it->first.c_str();
    const char CF = cq_str[ 0];
    // cq_str[ 1] should be ':'
    const char subC = cq_str[ 2];
    if ( CF == 'M') {
      if ( it->first.length() > 3) { // for m1cl
	int mesh_number = 0;
	int n = sscanf( &cq_str[ 3], "%d", &mesh_number);
	if ( n == 1) {
	  std::map< int, MeshInfo>::iterator it_mesh = map_mesh_info.find( mesh_number);
	  if ( it_mesh == map_mesh_info.end()) {
	    num_meshes++;
	    MeshInfo tmp;
	    tmp.__set_meshNumber( mesh_number);
	    map_mesh_info[ mesh_number] = tmp;
	    it_mesh = map_mesh_info.find( mesh_number);
	  }
	  MeshInfo &current_mesh = it_mesh->second;
	  const char *pinfo = &cq_str[ 3];
	  while ( *pinfo && isdigit( *pinfo))
	    pinfo++;
	  if ( subC == 'c') {
	    if ( !strcmp( pinfo, "nc")) {
	      // data is in binary format ...
	      int64_t num = *( int64_t *)it->second.value.data();
	      // needs to be swapped !!!!!!!!
	      current_mesh.__set_nVertices( byteSwap< int64_t>( num));
	      current_mesh.__set_meshNumber( mesh_number);
	      // }
	    }
	  } else if ( subC == 'm') {
	    if ( !strcmp( pinfo, "nm")) {
	      // can also be 
	      // current_mesh.name = it->second.value;
	      current_mesh.__set_name( it->second.value);
	      current_mesh.__set_meshNumber( mesh_number);
	    } else if ( !strcmp( pinfo, "et")) {
	      current_mesh.elementType.__set_shape( getElementTypeFromStr( it->second.value));
	      current_mesh.__set_meshNumber( mesh_number);
	    } else if ( !strcmp( pinfo, "ne")) {
	      // data is in binary format ...
	      int64_t num = *( int64_t *)it->second.value.data();
	      // needs to be swapped !!!!!!!!
	      current_mesh.__set_nElements( byteSwap< int64_t>( num));
	      current_mesh.__set_meshNumber( mesh_number);
	    } else if ( !strcmp( pinfo, "nn")) {
	      // data is in binary format ...
	      int num = *( int *)it->second.value.data();
	      // needs to be swapped !!!!!!!!
	      current_mesh.elementType.__set_num_nodes( byteSwap< int>( num));
	      current_mesh.__set_meshNumber( mesh_number);
	    } else if ( !strcmp( pinfo, "cl")) {
	      current_mesh.__set_meshColor( it->second.value);
	      current_mesh.__set_meshNumber( mesh_number);
	    }
	  } else {
	    // error should be here !!!
	    // because they are of the form mXXXXXXss or cXXXXXXss
	  }
	} else {
	  // error should be here !!!
	  // because they are of the form mXXXXXXss or cXXXXXXss
	}
      } else {
	// error should be here !!!
	// because they are of the form mXXXXXXss or cXXXXXXss
      }
    } else { // CF != 'M'
      // error should be here !!!
    }
  }
  return num_meshes;
}
// b8a687f0c9dcdc1f12317e52ca9afc2900000003DEM0000000000000000--------
// b8a687f0c9dcdc1f12317e52ca9afc2900000003DEM000000000000000000000000
bool HBase::getListOfMeshInfoFromTables( std::string &report, std::vector< MeshInfo> &listOfMeshes,
					 const std::string &metadata_table, const std::string &modelID,
					 const std::string &analysisID, const double stepValue) {
  // do the can on the metadata table ...

  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  // TScan ts;
  // std::stringstream filter;
  // filter.str("");
  // ts.__set_filterString(filter.str());
  StrVec cols;
  cols.push_back( "M"); // Mesh column family with
  // M:un for units
  // M:c* prefix for c123456nm c123456nc for name of coordinates set and number of coordinates = vertices = nodes
  // M:N* prefix fir m123456nm (name) m123456cn (coord.set.name) m123456et (elemType)
  //                 m123456ne (numberOfElements) m123456nn (numNodesPerElem) m123456cl (color)
  // ScannerID scannerOpen( Test tableName, Test startRow, list< Text> columns, map< Text, Text> attributes)
  // has to build the rowkey.... 
  // Metadata rowkeys = modelId + AnalysisID + StepNumber
  double my_stepValue = ( analysisID == "") ? 0.0 : stepValue; // rowkeys for static meshes have stepValue == 0.0
  std::string rowKey = createRowKey( modelID, analysisID, my_stepValue);
  ScannerID scan_id = _hbase_client->scannerOpen( metadata_table, rowKey, cols, m);
  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);
  std::map< int, MeshInfo> map_mesh_info;
  bool scan_ok = true;
  try {
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
  	break;
      // process rowsResult
      // std::cout << "number of rows = " << rowsResult.size() << endl;
      for ( size_t i = 0; i < rowsResult.size(); i++) {
  	// convert to return type
	if ( rowsResult[ i].row != rowKey)
	  continue; // break;
  	bool ok = getMeshInfoFromRow( map_mesh_info, rowsResult[ i]);
  	if ( ok) {
	  // in theory only 1 row ....
  	  // getMeshInfoFromRow.push_back( model_info);
	  // getMeshInfoFromRow( tmp_lst_meshes, rowsResult[ i]);
  	}
	break;
      }
    } // while ( true)

    if ( map_mesh_info.size() != 0) {
      // eventually pass the array-like tmp_lst_meshes to the list lst_mesh_info
      for ( std::map< int, MeshInfo>::const_iterator it = map_mesh_info.begin();
	    it != map_mesh_info.end();
	    it++) {
	listOfMeshes.push_back( it->second);
      }
    } else {
      // nothing found
      // scan_ok = false; // scan was ok but nothing found ...
      std::stringstream tmp;
      if ( analysisID != "") {
	tmp << "Dynamic Mesh nformation not found" 
	    << " for Analysis = '" << analysisID << "'"
	    << " and stepValue = " << stepValue;
      } else {
	tmp << "Static Mesh nformation not found" ;
      }
      report = tmp.str();
    }
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    report = tmp.str();
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    report = tmp.str();
  }
  _hbase_client->scannerClose( scan_id);
  
  return scan_ok;
}

std::string HBase::getListOfMeshes( std::string &report, std::vector< MeshInfo> &listOfMeshes,
				    const std::string &sessionID, const std::string &modelID,
				    const std::string &analysisID, const double stepValue) {

  std::cout << "getListOfMMeshes THRIFT: =====" << std::endl;
  std::cout << "S  - " << sessionID              << std::endl;
  std::cout << "M  - " << modelID                << std::endl;
  std::cout << "An - " << analysisID              << std::endl;
  std::cout << "Sv - " << stepValue              << std::endl;

  string table_name;
  bool scan_ok = true;

  // look into the modelInfo table to get the correct table name
  TableModelEntry table_set;
  bool found = getTableNames( sessionID, modelID, table_set);
  if ( found) {
    scan_ok = getListOfMeshInfoFromTables( report, listOfMeshes, table_set._metadata, modelID, analysisID, stepValue);
  } else {
    scan_ok = false;
  }
  string result;
  if ( scan_ok) {
    std::cout << "**********\n";
    bool there_are_meshes = listOfMeshes.size();
    if ( there_are_meshes) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    std::cout << "ERROR**********\n";
    result = "Error";
    report = "HBase::getListOfMeshes THRIFT could not scan.";
  }

  return result;
}
