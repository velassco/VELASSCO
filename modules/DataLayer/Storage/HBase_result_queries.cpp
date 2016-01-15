#include <stdio.h>

// CLib
#include <cmath>
#include <cstddef>

// STD
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// #include <unordered_set>
// needs an ordered set:
#include <set>

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
#include "Crono.h"

#define WITH_PARSE_COMPONENTS

#ifdef WITH_PARSE_COMPONENTS
#include "BinarySerialization.h"
#endif


#define DO_CRONO   true

using namespace std;
using namespace VELaSSCo;

/////////////////////////////////
// Curl + Json
/////////////////////////////////
#include <curl/curl.h>
#include "cJSON.h"

/* getListOfMeshes */
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
  // M:m* prefix fir m123456nm (name) m123456cn (coord.set.name) m123456et (elemType)
  //                 m123456ne (numberOfElements) m123456nn (numNodesPerElem) m123456cl (color)
  // MeshInfo has:
  //   std::string name;
  //   ElementType elementType;
  //   int64_t nVertices;
  //   int64_t nElements;
  //   std::string meshUnits;
  //   std::string meshColor;
  //   int32_t meshNumber;
  //   std::string coordsName;
  
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
	      // current_mesh.__set_nVertices( byteSwap< int64_t>( num));
	      current_mesh.__set_nVertices( byteSwapIfNeedPositive< int64_t>( num, MAXIMUM_ALLOWED_I64_NUMBER));
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
	      current_mesh.__set_nElements( byteSwapIfNeedPositive< int64_t>( num, MAXIMUM_ALLOWED_I64_NUMBER));
	      current_mesh.__set_meshNumber( mesh_number);
	    } else if ( !strcmp( pinfo, "nn")) {
	      // data is in binary format ...
	      int num = *( int *)it->second.value.data();
	      // needs to be swapped !!!!!!!!
	      // maximum number of nodes per element ( around 27 for quadratic hexaedrons, but for the future...)
	      current_mesh.elementType.__set_num_nodes( byteSwapIfNeedPositive< int>( num, 1000));
	      current_mesh.__set_meshNumber( mesh_number);
	    } else if ( !strcmp( pinfo, "cl")) {
	      current_mesh.__set_meshColor( it->second.value);
	      current_mesh.__set_meshNumber( mesh_number);
	    } else if ( !strcmp( pinfo, "cn")) {
	      current_mesh.__set_coordsName( it->second.value);
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

bool HBase::getListOfMeshInfoFromTables( std::string &report, std::vector< MeshInfo> &listOfMeshes,
					 const std::string &metadata_table, const std::string &modelID,
					 const std::string &analysisID, const double stepValue, const char *format) {
  // do the scan on the metadata table ...

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
  // M:m* prefix fir m123456nm (name) m123456cn (coord.set.name) m123456et (elemType)
  //                 m123456ne (numberOfElements) m123456nn (numNodesPerElem) m123456cl (color)
  // ScannerID scannerOpen( Test tableName, Test startRow, list< Text> columns, map< Text, Text> attributes)
  // has to build the rowkey.... 
  // Metadata rowkeys = modelId + AnalysisID + StepNumber
  double my_stepValue = ( analysisID == "") ? 0.0 : stepValue; // rowkeys for static meshes have stepValue == 0.0
  std::string rowKey = createMetaRowKey( modelID, analysisID, my_stepValue, format);
  const size_t len_rowkey = rowKey.length();
  ScannerID scan_id = _hbase_client->scannerOpen( metadata_table, rowKey, cols, m);
  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);

  std::cout << "\tAccessing table '" << metadata_table << "' with rowkey = " << rowKey << std::endl;

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
	// if ( rowsResult[ i].row.compare( 0, len_rowkey, rowKey) != 0)
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
	tmp << "Not found: Dynamic Mesh information" 
	    << " for Analysis = '" << analysisID << "'"
	    << " and stepValue = " << stepValue;
      } else {
	tmp << "Not found: Static Mesh information" ;
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
    // by default hexstrings are lower case but some data has been injected as upper case !!!
    scan_ok = getListOfMeshInfoFromTables( report, listOfMeshes, table_set._metadata, modelID, analysisID, stepValue);
    if ( scan_ok && ( listOfMeshes.size() == 0)) {
      // try with uppercase
      scan_ok = getListOfMeshInfoFromTables( report, listOfMeshes, table_set._metadata, modelID, analysisID, stepValue, "%02X");
    }

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
/* end of getListOfMeshes */

/* getListOfAnalyses */
static bool getAnalysisNameFromMetadataRowKey( std::set< std::string> &analysisNames, const std::string &rowkey) {
  // only need to get AnalysisName from RowKey
  bool ok = true;
  // rowKeys in Metadata tables have
  // 32 chars = modelID in hexadecimal string
  // 8 chars = N = length of analysisName in hexadecimal string
  // N chars = AnalysisName
  // 16 chars = step value (double) in hexadecimal string
  if ( rowkey.length() < 56)
    return false; // at least 56 bytes for static meshes !!!
  int analysis_length = 0;
  size_t offset = 32; // length modelID in hexadecimal
  const std::string &hexa_length = rowkey.substr( offset, 2 * sizeof( int));
  FromHexStringSwap( ( char *)&analysis_length, sizeof( int), hexa_length.c_str(), 2 * sizeof( int));
  if ( ( analysis_length > 0) && ( analysis_length < 1024 * 1024)) { // some sanity check ...
    offset += 2 * sizeof( int);
    // analysisNames.insert( rowkey.substr( offset, end));
    std::string name( rowkey.substr( offset, analysis_length));
    analysisNames.insert( name);
    ok = true;
  } else {
    ok = false;
  }
  return ok;
}

std::string GetStopKeyFromModelID( const std::string &modelID) {
  std::string stop( modelID);
  size_t idx = stop.size() - 1;
  char last = stop[ idx] + 1; // it's hex, so it can be always + 1
  stop.replace( idx, 1, 1, last);
  return stop;
}

bool HBase::getListOfAnalysesNamesFromTables( std::string &report, std::vector< std::string> &listOfAnalyses,
					      const std::string &metadata_table, const std::string &modelID) {
  // do the scan on the metadata table ...
  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  // TScan ts;
  // std::stringstream filter;
  // filter.str("");
  // ts.__set_filterString(filter.str());
  StrVec cols;
  // cols.push_back( "R"); // Mesh column family with, 
  // theoretially all Metadata should start with "R:r000001nm", i.e. at least 1 result defined!
  cols.push_back( "R:r000001nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  // we only need the analyses names which are in the rowKeys
  // ScannerID scannerOpen( Test tableName, Test startRow, list< Text> columns, map< Text, Text> attributes)
  // has to build the rowkey.... 
  // Metadata rowkeys = modelId + AnalysisID + StepNumber
  const size_t tmp_buf_size = 256;
  char tmp_buf[ tmp_buf_size];
  std::string rowKeyPrefix( ModelID_DoHexStringConversionIfNecesary( modelID, tmp_buf, tmp_buf_size));
  const size_t len_prefix = rowKeyPrefix.length();
  ScannerID scan_id = _hbase_client->scannerOpen( metadata_table, rowKeyPrefix, cols, m);

  // it's actually faster with the above than with the below code ...
  // TScan ts;
  // std::stringstream filter;
  // filter.str( "FirstKeyOnlyFilter()");
  // ts.__set_filterString(filter.str());
  // ts.__set_startRow( rowKeyPrefix);
  // // std::cout << "start key = " << rowKeyPrefix << std::endl;
  // // std::cout << " stop key = " << GetStopKeyFromModelID( rowKeyPrefix) << std::endl;
  // ts.__set_stopRow( GetStopKeyFromModelID( rowKeyPrefix));
  // ScannerID scan_id = _hbase_client->scannerOpenWithScan( metadata_table, ts, m);
  std::set< std::string> analysisNames;
  bool scan_ok = true;
  int num_rows = 0;
  try {
    Crono clk( CRONO_WALL_TIME);
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
  	break;
      // process rowsResult
      for ( size_t i = 0; i < rowsResult.size(); i++) {
  	// get only rows with the same rowKeyPrefix ( modelID)
	// if ( strncasecmp( rowsResult[ i].row.c_str(), rowKeyPrefix.c_str(), rowKeyPrefix.length())) {
	if ( rowsResult[ i].row.compare( 0, len_prefix, rowKeyPrefix) != 0) {
	  // we'll start with the correct ones, 
	  // once they are different, then there are no more...
	  break;
	}
  	bool ok = getAnalysisNameFromMetadataRowKey( analysisNames, rowsResult[ i].row);
	if ( !ok) {
	  // something wrong with the rowkey ...
	  break;
	}
	num_rows++;
      }
    } // while ( true)

    const bool do_crono = DO_CRONO;
    if ( do_crono)
      std::cout << "Number of scanned rows = " << num_rows << " in " << clk.fin() << " s." << std::endl;
    if ( analysisNames.size() != 0) {
      listOfAnalyses = std::vector< std::string>( analysisNames.begin(), analysisNames.end());
    } else {
      // nothing found
      // scan_ok = false; // scan was ok but nothing found ...
      report = "Not found: Analysis information";
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

std::string HBase::getListOfAnalyses( std::string &report, std::vector< std::string> &listOfAnalyses,
				      const std::string &sessionID, const std::string &modelID) {

  std::cout << "getListOfAnalyses THRIFT: =====" << std::endl;
  std::cout << "S  - " << sessionID              << std::endl;
  std::cout << "M  - " << modelID                << std::endl;

  string table_name;
  bool scan_ok = true;

  // look into the modelInfo table to get the correct table name
  TableModelEntry table_set;
  bool found = getTableNames( sessionID, modelID, table_set);
  if ( found) {
    scan_ok = getListOfAnalysesNamesFromTables( report, listOfAnalyses, table_set._metadata, modelID);
  } else {
    scan_ok = false;
  }
  string result;
  if ( scan_ok) {
    std::cout << "**********\n";
    bool there_are_analyses = listOfAnalyses.size();
    if ( there_are_analyses) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    std::cout << "ERROR**********\n";
    result = "Error";
    report = "HBase::getListOfAnalyses THRIFT could not scan.";
  }

  return result;
}

static bool getStepValueFromMetadataRowKey( std::set< double> &stepValues_set, const std::string &rowkey,
					    const size_t rowPrefix_len) {
  // rowPrefix already has modelID + length + Analysisname
  // only need to get StepValue from RowKey
  // rowKeys in Metadata tables have
  // 32 chars = modelID in hexadecimal string
  // 8 chars = N = length of analysisName in hexadecimal string
  // N chars = AnalysisName
  // 16 chars = step value (double) in hexadecimal string
  if ( rowkey.length() < 56)
    return false; // at least 56 bytes for static meshes !!!
  const size_t offset = rowPrefix_len; // length modelID + length + analysis
  const std::string &hexa_length = rowkey.substr( offset, 2 * sizeof( double));
  double stepValue = 0.0;
  FromHexStringSwap( ( char *)&stepValue, sizeof( double), hexa_length.c_str(), 2 * sizeof( double));
  stepValues_set.insert( stepValue);
  return true;
}

bool HBase::getListOfStepsFromTables( std::string &report, std::vector< double> &listOfSteps,
				      const std::string &metadata_table, const std::string &modelID,
				      const std::string &analysisID) {
  // do the scan on the metadata table ...
  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  StrVec cols;
  // cols.push_back( "R"); // Mesh column family with, 
  // theoretially all Metadata should start with "R:r000001nm", i.e. at least 1 result defined!
  // sometimes r1 is not in other steps ....
  cols.push_back( "R:r000001nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  cols.push_back( "R:r000002nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  cols.push_back( "R:r000003nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  cols.push_back( "R:r000004nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  cols.push_back( "R:r000005nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  cols.push_back( "R:r000006nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  cols.push_back( "R:r000007nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  cols.push_back( "R:r000008nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  cols.push_back( "R:r000009nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  // above is a bad trick ... the ideal would be to hace the ModelInfo/Analysis+Steps+Results cached in the sessionID
  // and if it's not present, the get the full info once and return the asked info (piecewise).
  // this way the time will be spent only once, in the first hbase/GetList...Analyses query 
  // we only need the step_values which are in the rowKeys
  // ScannerID scannerOpen( Test tableName, Test startRow, list< Text> columns, map< Text, Text> attributes)
  // has to build the rowkey.... 
  // Metadata rowkeys = modelId + AnalysisID + StepNumber
  std::string rowKeyPrefix = createMetaRowKeyPrefix( modelID, analysisID);
  const size_t len_prefix = rowKeyPrefix.length();
  ScannerID scan_id = _hbase_client->scannerOpen( metadata_table, rowKeyPrefix, cols, m);
  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);
  std::set< double> stepValues_set;
  bool scan_ok = true;
  int num_rows = 0;
  try {
    Crono clk( CRONO_WALL_TIME);
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
  	break;
      // process rowsResult
      for ( size_t i = 0; i < rowsResult.size(); i++) {
  	// get only rows with the same rowKeyPrefix ( modelID)
	// if ( strncasecmp( rowsResult[ i].row.c_str(), rowKeyPrefix.c_str(), rowKeyPrefix.length())) {
	if ( rowsResult[ i].row.compare( 0, len_prefix, rowKeyPrefix) != 0) {
	  // we'll start with the correct ones, 
	  // once they are different, then there are no more...
	  break;
	}
  	bool ok = getStepValueFromMetadataRowKey( stepValues_set, rowsResult[ i].row, len_prefix);
	if ( !ok) {
	  // something wrong with the rowkey ...
	  break;
	}
	num_rows++;
      }
    } // while ( true)

    const bool do_crono = DO_CRONO;
    if ( do_crono)
      std::cout << "Number of scanned rows = " << num_rows << " in " << clk.fin() << " s." << std::endl;
    if ( stepValues_set.size() != 0) {
      listOfSteps = std::vector< double>( stepValues_set.begin(), stepValues_set.end());
    } else {
      // nothing found
      // scan_ok = false; // scan was ok but nothing found ...
      report = "Not found: Step information";
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

std::string HBase::getListOfSteps( std::string &report, std::vector< double> &listOfSteps,
				   const std::string &sessionID, const std::string &modelID,
				   const std::string &analysisID) {

  std::cout << "getListOfSteps THRIFT: =====" << std::endl;
  std::cout << "S  - " << sessionID              << std::endl;
  std::cout << "M  - " << modelID                << std::endl;
  std::cout << "An - " << analysisID              << std::endl;

  string table_name;
  bool scan_ok = true;

  // look into the modelInfo table to get the correct table name
  TableModelEntry table_set;
  bool found = getTableNames( sessionID, modelID, table_set);
  if ( found) {
    scan_ok = getListOfStepsFromTables( report, listOfSteps, table_set._metadata, modelID, analysisID);
  } else {
    scan_ok = false;
  }
  string result;
  if ( scan_ok) {
    std::cout << "**********\n";
    bool there_are_steps = listOfSteps.size();
    if ( there_are_steps) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    std::cout << "ERROR**********\n";
    result = "Error";
    report = "HBase::getListOfSteps THRIFT could not scan.";
  }

  return result;
}
/* end GetListOfAnalyses */

/* GetListOfResults */
static bool getResultInfoFromRow( std::map< int, ResultInfo> &map_result_info, const TRowResult &rowResult) {
  // R:r* prefix for r123456nm Name, r123456rt Type, r123456nc #comp, r123456cn CompNames,
  //                 r123456lc Location, r123456gp GPname,  r123456co CoordsName,  r123456un Units
  // ResultInfo has:
  //   std::string name;
  //   std::string type;
  //   int32_t numberOfComponents;
  //   std::vector<std::string>  componentNames;
  //   std::string location;
  //   std::string gaussPointName;
  //   std::string coordinatesName;
  //   std::string units;
  int num_results = 0;
  for ( CellMap::const_iterator it = rowResult.columns.begin(); 
	it != rowResult.columns.end(); ++it) {
    const char *cq_str = it->first.c_str();
    const char CF = cq_str[ 0];
    // cq_str[ 1] should be ':'
    const char subC = cq_str[ 2];
    if ( CF == 'R') {
      if ( it->first.length() > 3) { // for r1nm
	int result_number = 0;
	int n = sscanf( &cq_str[ 3], "%d", &result_number);
	if ( n == 1) {
	  std::map< int, ResultInfo>::iterator it_mesh = map_result_info.find( result_number);
	  if ( it_mesh == map_result_info.end()) {
	    num_results++;
	    ResultInfo tmp;
	    tmp.__set_resultNumber( result_number);
	    map_result_info[ result_number] = tmp;
	    it_mesh = map_result_info.find( result_number);
	  }
	  ResultInfo &current_result = it_mesh->second;
	  const char *pinfo = &cq_str[ 3];
	  while ( *pinfo && isdigit( *pinfo))
	    pinfo++;
	  // subC should be always 'r'
	  if ( subC == 'r') {
	    if ( !strcmp( pinfo, "nm")) {
	      // can also be 
	      // current_result.name = it->second.value;
	      current_result.__set_name( it->second.value);
	      current_result.__set_resultNumber( result_number);
	    } else if ( !strcmp( pinfo, "rt")) {
	      current_result.__set_type( it->second.value);
	      current_result.__set_resultNumber( result_number);
	    } else if ( !strcmp( pinfo, "nc")) {
	      // data is in binary format ...
	      int num = *( int *)it->second.value.data();
	      // needs to be swapped !!!!!!!!
	      // maximum number of components are 6, but for the future ...
	      current_result.__set_numberOfComponents( byteSwapIfNeedPositive< int>( num, 100));
	      current_result.__set_resultNumber( result_number);
	    } else if ( !strcmp( pinfo, "cn")) {
	      // component names, still we've to agree on the list format, as they are different:
	      // " kjkl", "l2kjlkj"  or { "klsadjfkls", "lkjkl"} or [] or ...
	      std::vector<std::string> lst_components;
#ifdef WITH_PARSE_COMPONENTS
              boost::uint32_t pos = 0;
              GID::BinaryDeserializerNative deser;
              deser.SetEndianness( GID::BigEndian );
              while( pos < it->second.value.length() )
                {
                std::string str;
                try 
                  {
                  pos = deser.Read( it->second.value, str, pos );
                  }
                catch( ... )
                  {
                  std::cout << "Exception caught!" << std::endl;
                  std::cout << BinarySerializer::BinToHex( it->second.value ) 
                            << std::endl;
                  lst_components.clear( );
                  lst_components.push_back( it->second.value );
                  break;
                  }
                lst_components.push_back( str );
                }
#else
	      lst_components.push_back( it->second.value); // needs to
                                                           // be parsed
                                                           // ....
#endif
	      current_result.__set_componentNames( lst_components);
	      current_result.__set_resultNumber( result_number);
	    } else if ( !strcmp( pinfo, "lc")) {
	      current_result.__set_location( it->second.value);
	      current_result.__set_resultNumber( result_number);
	    } else if ( !strcmp( pinfo, "gp")) {
	      current_result.__set_gaussPointName( it->second.value);
	      current_result.__set_resultNumber( result_number);
	    } else if ( !strcmp( pinfo, "co")) {
	      current_result.__set_coordinatesName( it->second.value);
	      current_result.__set_resultNumber( result_number);
	    } else if ( !strcmp( pinfo, "un")) {
	      current_result.__set_units( it->second.value);
	      current_result.__set_resultNumber( result_number);
	    }
	  } else {
	    // error should be here !!!
	    // because they are of the form rXXXXXXss
	  }
	} else {
	  // error should be here !!!
	  // because they are of the form rXXXXXXss
	}
      } else {
	// error should be here !!!
	// because they are of the form rXXXXXXss
      }
    } else { // CF != 'R'
      // error should be here !!!
    }
  }
  return num_results;
}  

bool HBase::getListOfResultsFromTables( std::string &report, std::vector< ResultInfo> &listOfResults,
					const std::string &metadata_table, const std::string &modelID,
					const std::string &analysisID, const double stepValue, const char *format) {
  // do the scan on the metadata table ...
  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  StrVec cols;
  cols.push_back( "R"); // Mesh column family with, 
  // theoretially all Metadata should start with "R:r000001nm", i.e. at least 1 result defined!
  // we are looking for all result information ...
  // cols.push_back( "R:r000001nm"); // this halves the time wrt "R", which is 4x faster than no cols.
  // Metadata rowkeys = modelId + AnalysisID + StepNumber
  std::string rowKey = createMetaRowKey( modelID, analysisID, stepValue, format);
  const size_t len_rowkey = rowKey.length();
  
  std::cout << "\tAccessing table '" << metadata_table << "' with rowkey = " << rowKey << std::endl;
  
  ScannerID scan_id = _hbase_client->scannerOpen( metadata_table, rowKey, cols, m);
  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);
  std::map< int, ResultInfo> map_result_info;
  bool scan_ok = true;
  int num_rows = 0;
  try {
    Crono clk( CRONO_WALL_TIME);
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
  	break;
      // process rowsResult
      for ( size_t i = 0; i < rowsResult.size(); i++) {
  	// get only rows with the same rowKeyPrefix ( modelID)
	// if ( strncasecmp( rowsResult[ i].row.c_str(), rowKeyPrefix.c_str(), rowKeyPrefix.length())) {
	if ( rowsResult[ i].row.compare( 0, len_rowkey, rowKey) != 0) {
	  // we'll start with the correct ones, 
	  // once they are different, then there are no more...
	  break;
	}
  	bool ok = getResultInfoFromRow( map_result_info, rowsResult[ i]);
	if ( !ok) {
	  // something wrong with the rowkey ...
	  break;
	}
	num_rows++;
      }
    } // while ( true)

    const bool do_crono = DO_CRONO;
    if ( do_crono)
      std::cout << "Number of scanned rows = " << num_rows << " in " << clk.fin() << " s." << std::endl;
    if ( map_result_info.size() != 0) {
      // eventually pass the array-like tmp_lst_meshes to the list lst_mesh_info
      for ( std::map< int, ResultInfo>::const_iterator it = map_result_info.begin();
	    it != map_result_info.end();
	    it++) {
	listOfResults.push_back( it->second);
      }
    } else {
      // nothing found
      // scan_ok = false; // scan was ok but nothing found ...
      report = "Not found: Result Information";
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

std::string HBase::getListOfResults( std::string &report, std::vector< ResultInfo> &listOfResults,
				     const std::string &sessionID, const std::string &modelID,
				     const std::string &analysisID, const double stepValue) {
  std::cout << "getListOfResults THRIFT: =====" << std::endl;
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
    // by default hexstrings are lower case but some data has been injected as upper case !!!
    scan_ok = getListOfResultsFromTables( report, listOfResults, table_set._metadata, modelID, analysisID, stepValue);
    if ( scan_ok && ( listOfResults.size() == 0)) {
      // try with uppercase
      scan_ok = getListOfResultsFromTables( report, listOfResults, table_set._metadata, modelID, analysisID, stepValue, "%02X");
    }
  } else {
    scan_ok = false;
  }
  string result;
  if ( scan_ok) {
    std::cout << "**********\n";
    bool there_are_results = listOfResults.size();
    if ( there_are_results) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    std::cout << "ERROR**********\n";
    result = "Error";
    report = "HBase::getListOfResults THRIFT could not scan.";
  }

  return result;
}
/* end GetListOfResults */

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
  return num_vertices;
}

bool HBase::getListOfVerticesFromTables( std::string &report, std::vector< Vertex> &listOfVertices,
					 const std::string &data_table,  
					 const std::string &modelID,
					 const std::string &analysisID, const double stepValue, 
					 const int32_t meshID,  // it's actually the coordsID as in cXXXXX...
					 const char *format) { // for the stepvalue hex string
  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  // TScan ts;
  // std::stringstream filter;
  // filter.str("");
  // ts.__set_filterString(filter.str());
  StrVec cols;
  // char buf[ 100];
  // these filters: do not work, should check another way ...
  // sprintf( buf, "M:c%06d*", meshID);
  // sprintf( buf, "M:c%06d", meshID);
  // cols.push_back( buf); // Mesh column family with
  cols.push_back( "M");
  // M:un for units
  // M:c* prefix for c123456nm c123456nc for name of coordinates set and number of coordinates = vertices = nodes
  // M:m* prefix fir m123456nm (name) m123456cn (coord.set.name) m123456et (elemType)
  //                 m123456ne (numberOfElements) m123456nn (numNodesPerElem) m123456cl (color)
  // ScannerID scannerOpen( Test tableName, Test startRow, list< Text> columns, map< Text, Text> attributes)
  // has to build the rowkey.... 
  // Metadata rowkeys = modelId + AnalysisID + StepNumber
  double my_stepValue = ( analysisID == "") ? 0.0 : stepValue; // rowkeys for static meshes have stepValue == 0.0
  std::string prefixRowKey = createMetaRowKey( modelID, analysisID, my_stepValue, format);
  std::string startRowKey = createDataRowKey( modelID, analysisID, my_stepValue, 0, format); // partitionID = 0
  std::string stopRowKey = createDataRowKey( modelID, analysisID, my_stepValue, ( int)0x7fffffff, format); // partitionID = biggest ever
  const size_t len_prefix_rowkey = prefixRowKey.length();
  ScannerID scan_id = _hbase_client->scannerOpenWithStop( data_table, startRowKey, stopRowKey, cols, m);
  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);

  std::cout << "\tAccessing table '" << data_table << "' with";
  std::cout << "\t startRowKey = " << startRowKey << std::endl;
  std::cout << "\t  stopRowKey = " << stopRowKey << std::endl;

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
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    report = tmp.str();
  }
  _hbase_client->scannerClose( scan_id);
  
  return scan_ok;
}

std::string HBase::getListOfVerticesFromMesh( std::string &report, std::vector< Vertex> &listOfVertices,
					      const std::string &sessionID, const std::string &modelID, 
					      const std::string &analysisID, const double stepValue, 
					      const int32_t meshID) {
  std::cout << "getListOfVerticesFromMesh THRIFT: =====" << std::endl;
  std::cout << "S  - " << sessionID              << std::endl;
  std::cout << "M  - " << modelID                << std::endl;
  std::cout << "An - " << analysisID              << std::endl;
  std::cout << "Sv - " << stepValue              << std::endl;
  std::cout << "Msh- " << meshID                 << std::endl;

  string table_name;
  bool scan_ok = true;

  // look into the modelInfo table to get the correct table name
  TableModelEntry table_set;
  bool found = getTableNames( sessionID, modelID, table_set);
  if ( found) {
    // by default hexstrings are lower case but some data has been injected as upper case !!!
    scan_ok = getListOfVerticesFromTables( report, listOfVertices, table_set._data, modelID, analysisID, stepValue, meshID);
    if ( scan_ok && ( listOfVertices.size() == 0)) {
      // try with uppercase
      scan_ok = getListOfVerticesFromTables( report, listOfVertices, table_set._data, modelID, analysisID, stepValue, meshID, "%02X");
    }
  } else {
    scan_ok = false;
  }
  string result;
  if ( scan_ok) {
    std::cout << "**********\n";
    bool there_are_vertices = listOfVertices.size();
    if ( there_are_vertices) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    std::cout << "ERROR**********\n";
    result = "Error";
    report = "HBase::getListOfVerticesFromMesh THRIFT could not scan.";
  }

  return result;
}
/* end GetListOfResults */

