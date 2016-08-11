#include <stdio.h>

// CLib
#include <cmath>
#include <cstddef>

// STD
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

// #include <unordered_set>
// needs an ordered set:
#include <set>

// Curl
#include <curl/curl.h>

// VELaSSCo
#include "HBase.h"
//#include "VELaSSCoSM.h" // for class FullyQualifiedModelName
#include "cJSON.h"
#include "base64.h"

#include "Curl_cmd.h"
#include "Helpers.h"
#include "Extras.h"
#include "Crono.h"

#define WITH_PARSE_COMPONENTS

#ifdef WITH_PARSE_COMPONENTS
#include "BinarySerialization.h"
#endif


#define DO_CRONO   true

#define DO_ALWAYS_BYTE_SWAP

using namespace std;
using namespace VELaSSCo;

/////////////////////////////////
// Curl + Json
/////////////////////////////////
#include <curl/curl.h>
#include "cJSON.h"

/* GetListOfVerticesFromMesh */
static bool getVerticesFromRow( std::vector< Vertex> &listOfVertices, const TRowResult &rowResult, const int meshID) {
  int num_vertices = 0;
  for ( CellMap::const_iterator it = rowResult.columns.begin(); 
	it != rowResult.columns.end(); ++it) {
    const char *cq_str = it->first.c_str();
    const char CF = cq_str[ 0];
    // cq_str[ 1] should be ':'
    const char subC = cq_str[ 2];
    if ( CF == 'M') {
      if ( it->first.length() > 3) { // for c1xxxx
	int mesh_number = 0;
	int n = sscanf( &cq_str[ 3], "%d", &mesh_number);
	if ( n == 1) {
	  // in fact not needed as it should always be meshID ( == coordsID)
	  const char *pinfo = &cq_str[ 3];
	  while ( *pinfo && isdigit( *pinfo))
	    pinfo++;
	  if ( subC == 'c') { // right qualifier, coordinates
	    // coordinates are of the form cXXXXXX_ID, where ID is binary indes ...
	    int64_t node_id;
	    double node_x, node_y, node_z;
	    pinfo++; // the '_'
	    node_id = byteSwap< int64_t>( *( int64_t *)pinfo);
	    double *coords = ( double *)it->second.value.data();
	    node_x = byteSwap< double>( coords[ 0]);
	    node_y = byteSwap< double>( coords[ 1]);
	    node_z = byteSwap< double>( coords[ 2]);
	    
	    if(mesh_number == meshID){
			Vertex mesh_vertex;
			mesh_vertex.__set_id( node_id);
			mesh_vertex.__set_x( node_x);
			mesh_vertex.__set_y( node_y);
			mesh_vertex.__set_z( node_z);
			listOfVertices.push_back( mesh_vertex);
			num_vertices++;
		}
	  }
	}
      }
    }
  }
  return num_vertices;
}

bool HBase::getListOfSelectedVerticesFromTables( std::string &report, std::vector< Vertex> &listOfVertices,
						 const std::string &data_table,  
						 const std::string &modelID,
						 const std::string &analysisID, const double stepValue, 
						 const int32_t meshID,  // it's actually the coordsID as in cXXXXX...
						 const std::vector<int64_t> &listOfVerticesID,
						 const char *format) { // for the stepvalue hex string
  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  StrVec cols;

  // as in getResultFromVerticesIDFromTables --> build a list of column qualifiers:
  char buf[ 100];
  std::cout << "Making list of vertices column names...\n";
  for(size_t i = 0; i < listOfVerticesID.size(); i++) {
    std::ostringstream oss;
    sprintf( buf, "M:c%06d_", meshID );
    //sprintf( buf, "M:c%06d_", meshID ); // vertices are cMMMMMM_Nodeid, like c000001_0000000000000001
    oss << buf;
    int64_t vertexID = byteSwap<int64_t>( listOfVerticesID[i] );
    oss.write( (char*)&vertexID, sizeof(int64_t));
    //LOGGER_SM << "=====================> " << oss.str() << std::endl;
    cols.push_back( oss.str() );
  }
  std::cout << "Making list of vertices column names (Done)...\n";

  double my_stepValue = ( analysisID == "") ? 0.0 : stepValue; // rowkeys for static meshes have stepValue == 0.0
  std::string prefixRowKey = createMetaRowKey( modelID, analysisID, my_stepValue, format);
  std::string startRowKey = createDataRowKey( modelID, analysisID, my_stepValue, 0, format); // partitionID = 0
  std::string stopRowKey = createDataRowKey( modelID, analysisID, my_stepValue, ( int)0x7fffffff, format); // partitionID = biggest ever
  const size_t len_prefix_rowkey = prefixRowKey.length();
  ScannerID scan_id = _hbase_client->scannerOpenWithStop( data_table, startRowKey, stopRowKey, cols, m);

  LOGGER_SM << "\tAccessing table '" << data_table << "' with" << std::endl;
  LOGGER_SM << "\t startRowKey = " << startRowKey << std::endl;
  LOGGER_SM << "\t  stopRowKey = " << stopRowKey << std::endl;

  bool scan_ok = true;
  try {
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
  	break;
      // process rowsResult
      for ( size_t i = 0; i < rowsResult.size(); i++) {
  	// convert to return type
	// check if the rowkey is our's ... should be ....
	if ( rowsResult[ i].row.compare( 0, len_prefix_rowkey, prefixRowKey) != 0)
	  continue; // break;
  	bool ok = getVerticesFromRow( listOfVertices, rowsResult[ i], meshID);
  	if ( ok) {
  	  // getMeshInfoFromRow.push_back( model_info);
	  // getMeshInfoFromRow( tmp_lst_meshes, rowsResult[ i]);
  	} else {
	  // nothing read? 
	  // eventually provide an error
	}
	// several rows = partitions so don't quit yet,
	// break;
      }
    } // while ( true)

    if ( listOfVertices.size() == 0) {
      // nothing found
      // scan_ok = false; // scan was ok but nothing found ...
      std::stringstream tmp;
      if ( analysisID != "") {
	tmp << "Not found: Vertices for Dynamic Mesh" 
	    << " for Analysis = '" << analysisID << "'"
	    << " and stepValue = " << stepValue;
      } else {
	tmp << "Not found: Vertices for Static Mesh" ;
      }
      report = tmp.str();
    }
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << report << std::endl;
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << report << std::endl;
  }
  _hbase_client->scannerClose( scan_id);
  
  return scan_ok;
}

std::string HBase::getListOfSelectedVerticesFromMesh( std::string &report, std::vector< Vertex> &listOfVertices,
					      const std::string &sessionID, const std::string &modelID, 
					      const std::string &analysisID, const double stepValue, 
					      const int32_t meshID, const std::vector<int64_t> &listOfVerticesID) {
  LOGGER_SM << "getListOfSelectedVerticesFromMesh THRIFT: =====" << std::endl;
  LOGGER_SM << "S  - " << sessionID              << std::endl;
  LOGGER_SM << "M  - " << modelID                << std::endl;
  LOGGER_SM << "An - " << analysisID              << std::endl;
  LOGGER_SM << "Sv - " << stepValue              << std::endl;
  LOGGER_SM << "Msh- " << meshID                 << std::endl;
  if ( listOfVerticesID.size() == 0) {
    LOGGER_SM << "Msh- " << meshID                 << std::endl;
    std::string result = "Error";
    report = "No VerticesID provided.";
    return result;
  }
  size_t last = listOfVerticesID.size();
  LOGGER_SM << "verticesID - " << last << " vertices: ";
  if ( last > 10) last = 10;
  for ( size_t idx = 0; idx < last; idx++) {
    std::cerr << listOfVerticesID[ idx] << ", ";
  }
  std::cerr << "..." << std::endl;

  string table_name;
  bool scan_ok = true;

  // look into the modelInfo table to get the correct table name
  TableModelEntry table_set;
  bool found = getVELaSSCoTableNames( sessionID, modelID, table_set);
  if ( found) {
    // by default hexstrings are lower case but some data has been injected as upper case !!!
    scan_ok = getListOfSelectedVerticesFromTables( report, listOfVertices, table_set._data, modelID, analysisID, stepValue, meshID, listOfVerticesID);
    if ( scan_ok && ( listOfVertices.size() == 0)) {
      // try with uppercase
      scan_ok = getListOfSelectedVerticesFromTables( report, listOfVertices, table_set._data, modelID, analysisID, stepValue, meshID, listOfVerticesID, "%02X");
    }
  } else {
    scan_ok = false;
  }
  string result;
  if ( scan_ok) {
    LOGGER_SM << "**********\n";
    bool there_are_vertices = listOfVertices.size();
    if ( there_are_vertices) {
      result = "Ok";
    } else {
      result = "Error";
    }

    // remove repeated vertices:
    LOGGER_SM << "Processing " << listOfVertices.size() << " retrieved results." << std::endl;
    // remove repeated vertices:
    size_t orig_num_vertices = listOfVertices.size();
    std::unordered_map< int64_t, Vertex> resultOnVertexListMap;
    for ( size_t i = 0; i < listOfVertices.size(); i++) {
      const Vertex &vertexInfo = listOfVertices[ i];
      int64_t vertexId = vertexInfo.id;
      resultOnVertexListMap[ vertexId] = vertexInfo;
    }
    
    // get only asked vertices and 
    listOfVertices.clear();
    for ( size_t i = 0; i < listOfVerticesID.size(); i++) {
      int64_t vertexId = listOfVerticesID[ i];
      std::unordered_map< int64_t, Vertex>::const_iterator it_vertexInfo = resultOnVertexListMap.find( vertexId);
      if ( it_vertexInfo == resultOnVertexListMap.end()) {
	// LOGGER_SM << "not found";
      } else {
	// LOGGER_SM << " found";
	listOfVertices.push_back( it_vertexInfo->second );
      }
    }
  
    LOGGER_SM << "Found " << listOfVertices.size() << " verticesID from the retrieved " << orig_num_vertices << " ones." << std::endl; 


  } else {
    LOGGER_SM << "ERROR**********\n";
    result = "Error";
    report = "HBase::getListOfVerticesFromMesh THRIFT could not scan.";
  }

  return result;
}
/* end GetListOfResults */
