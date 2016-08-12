#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <stddef.h>  // defines NULL
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "Helpers.h"
#include "DataLayerAccess.h" // to retrieve the mesh vertices for GetBoundaryMesh()
#include "Analytics.h"
// the format is shared between the QueryManager, the AccessLibrary and th visualiztion client
// so it's located in the AccessLibrary folder
#include "../../AccessLib/AccessLib/BoundaryBinaryMesh.h"

#include "AnalyticsCommon.h"

#pragma message( "WARNING: ")
#pragma message( "WARNING: yarn and hdfs should be in the $PATH and not hard coded in the source.")
#pragma message( "WARNING: look at GetFullHBaseConfigurationFilename() for a similar approach: PathToYarnAndHDFS()")
#pragma message( "WARNING: ")

static std::string demo_simplified_mesh() {
  // This is code from the DemoServer :
  VELaSSCo::BoundaryBinaryMesh::MeshPoint lst_vertices[] = {
    {  1, {  0.0000000000000000e+00,  0.0000000000000000e+00,  3.0000000000000000e+00}},
    {  2, {  0.0000000000000000e+00,  2.5000000000000000e-01,  2.2500000000000000e+00}},
    {  3, { -2.1650634706020355e-01, -1.2500000000000000e-01,  2.2500000000000000e+00}},
    {  4, {  2.1650634706020355e-01, -1.2500000000000000e-01,  2.2500000000000000e+00}},
    {  5, {  0.0000000000000000e+00,  5.0000000000000000e-01,  1.5000000000000000e+00}},
    {  6, { -4.3301269412040710e-01, -2.5000000000000000e-01,  1.5000000000000000e+00}},
    {  7, {  4.3301269412040710e-01, -2.5000000000000000e-01,  1.5000000000000000e+00}},
    {  8, { -1.1780370958149433e-02, -7.9105973243713379e-02,  8.0925619602203369e-01}},
    {  9, {  0.0000000000000000e+00,  7.5000000000000000e-01,  7.5000000000000000e-01}},
    { 10, { -6.4951902627944946e-01, -3.7500000000000000e-01,  7.5000000000000000e-01}},
    { 11, {  6.4951902627944946e-01, -3.7500000000000000e-01,  7.5000000000000000e-01}},
    { 12, { -6.7148506641387939e-01,  3.8768208026885986e-01,  6.7390751838684082e-01}},
    { 13, { -8.9912943757042285e-10, -7.7536416053771973e-01,  6.7390751838684082e-01}},
    { 14, {  6.7148506641387939e-01,  3.8768208026885986e-01,  6.7390751838684082e-01}},
    { 15, { -1.1102230246251565e-16, -2.2204460492503131e-16,  0.0000000000000000e+00}},
    { 16, {  0.0000000000000000e+00,  1.0000000000000000e+00,  0.0000000000000000e+00}},
    { 17, { -8.6602538824081421e-01, -5.0000000000000000e-01,  0.0000000000000000e+00}},
    { 18, {  8.6602538824081421e-01, -5.0000000000000000e-01,  0.0000000000000000e+00}},
    { 19, { -6.4278763532638550e-01,  7.6604443788528442e-01,  0.0000000000000000e+00}},
    { 20, { -9.8480772972106934e-01,  1.7364817857742310e-01,  0.0000000000000000e+00}},
    { 21, { -3.4202015399932861e-01, -9.3969261646270752e-01,  0.0000000000000000e+00}},
    { 22, {  3.4202015399932861e-01, -9.3969261646270752e-01,  0.0000000000000000e+00}},
    { 23, {  9.8480772972106934e-01,  1.7364817857742310e-01,  0.0000000000000000e+00}},
    { 24, {  6.4278763532638550e-01,  7.6604443788528442e-01,  0.0000000000000000e+00}}
  };
  VELaSSCo::BoundaryBinaryMesh::BoundaryQuadrilateral lst_tetrahedra[] = {
    { 4, {  2,  1,  3,  4}},
    { 4, {  3,  2,  4,  7}},
    { 4, {  2,  3,  6,  7}},
    { 4, { 20, 19, 12, 15}},
    { 4, { 24, 23, 14, 15}},
    { 4, { 22, 21, 13, 15}},
    { 4, { 23, 18, 11, 15}},
    { 4, { 19, 16,  9, 15}},
    { 4, { 21, 17, 10, 15}},
    { 4, { 18, 22, 11, 15}},
    { 4, { 17, 20, 10, 15}},
    { 4, { 16, 24,  9, 15}},
    { 4, { 20, 12, 10, 15}},
    { 4, { 22, 13, 11, 15}},
    { 4, { 24, 14,  9, 15}},
    { 4, { 14, 23, 11, 15}},
    { 4, { 13, 21, 10, 15}},
    { 4, { 12, 19,  9, 15}},
    { 4, { 10, 12,  6,  8}},
    { 4, { 10, 12,  8, 15}},
    { 4, {  6, 10,  8, 13}},
    { 4, { 10,  8, 13, 15}},
    { 4, {  8, 13, 15, 11}},
    { 4, {  8, 13, 11,  7}},
    { 4, { 11,  8,  7, 14}},
    { 4, { 11,  8, 14, 15}},
    { 4, {  8,  6, 13,  7}},
    { 4, {  8,  6,  7,  5}},
    { 4, {  8,  6,  5, 12}},
    { 4, {  6,  7,  5,  2}},
    { 4, {  7,  8,  5, 14}},
    { 4, {  9, 14,  5,  8}},
    { 4, {  9, 14,  8, 15}},
    { 4, {  5,  9,  8, 12}},
    { 4, {  9,  8, 12, 15}}
  };
  // we store tetrahedra as quadrilaterals in the BoundaryBinaryMesh
  VELaSSCo::BoundaryBinaryMesh demo_mesh;
  int64_t num_vertices = ( int64_t)( sizeof( lst_vertices) / sizeof( VELaSSCo::BoundaryBinaryMesh::MeshPoint));
  int64_t num_tetrahedra = ( int64_t)( sizeof( lst_tetrahedra) / sizeof( VELaSSCo::BoundaryBinaryMesh::BoundaryQuadrilateral));
  demo_mesh.set( lst_vertices, num_vertices, lst_tetrahedra, num_tetrahedra, VELaSSCo::BoundaryBinaryMesh::STATIC);
  // end of code from DemoServer.cpp
  return demo_mesh.toString();
}

static bool getBoundaryQuadrilateralsFromJavaOutput( const char *filename, 
						std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryQuadrilateral> &lst_triangles) {
 FILE *fi = fopen( filename, "r");
 bool ok = true;

 // format of file is:
 // int32 num_nodes, int64 nodes[ num_nodes]\n
 VELaSSCo::BoundaryBinaryMesh::BoundaryQuadrilateral triangle;
 ssize_t n_read = 0;
 size_t hexa_data_line_size = 64 * 1024;
 char *hexa_data_line = ( char *)malloc( hexa_data_line_size * sizeof( char));
 size_t binary_data_size = 32 * 1024;
 char *binary_data = ( char *)malloc( binary_data_size * sizeof( char));
 while ( !feof( fi)) {
   n_read = getline( &hexa_data_line, &hexa_data_line_size, fi);
   if ( n_read == -1) {
     if ( !feof( fi)) {
       DEBUG( "getBoundaryQuadrilateralsFromJavaOutput: error reading MR output file");
       ok = false; 
     }
     break;
   }

   if ( FromHexString( binary_data, binary_data_size, hexa_data_line, hexa_data_line_size)) {
     DEBUG( "getBoundaryQuadrilateralsFromJavaOutput: error translating hexadecimal string to binary data.");
     ok = false; 
     break;
   }

   triangle._num_nodes = byteSwap< int>( *( int *)&binary_data[ 0]);
   // assert( triangle._num_nodes == 3);
   if ( triangle._num_nodes != 3) {
     // ok = false; //just finished reading to show something ...
     DEBUG( "getBoundaryQuadrilateralsFromJavaOutput: read num_nodes != 3.");
     break;
   }

   triangle._nodes[ 0] = byteSwap< int64_t>( *( int64_t *)&binary_data[  4]); // sizeof( int)
   triangle._nodes[ 1] = byteSwap< int64_t>( *( int64_t *)&binary_data[ 12]); // sizeof( int) + sizeof( int64_t)
   triangle._nodes[ 2] = byteSwap< int64_t>( *( int64_t *)&binary_data[ 20]); // sizeof( int) + 2 * sizeof( int64_t)

   lst_triangles.push_back( triangle);
 } // !feof( fi)

 fclose( fi);
 return ok;
}

static bool getBoundaryVerticesFromDataLayerOutput( const std::vector< Vertex> &DL_vertices, 
						    const std::unordered_set< int64_t> &lst_UsedNodeIDs,
						    std::vector< VELaSSCo::BoundaryBinaryMesh::MeshPoint> &lst_vertices) {

  LOGGER << "--> before getBoundaryVerticesFromDataLayerOutput" << std::endl;

  for ( std::vector< Vertex>::const_iterator it = DL_vertices.begin(); it != DL_vertices.end(); ++it) {
    // in theory this is check is not needed as we use getListOfSelectedVerticesFromMesh
    // but for the other cases, and it seems that it does not add a significant overhead ...
    if ( lst_UsedNodeIDs.find( it->id) != lst_UsedNodeIDs.end()) {
      VELaSSCo::BoundaryBinaryMesh::MeshPoint tmp;
      tmp._id = it->id;
      tmp._coords[ 0] = it->x;
      tmp._coords[ 1] = it->y;
      tmp._coords[ 2] = it->z;
      lst_vertices.push_back( tmp);
    }
  }
  LOGGER << "--> after getBoundaryVerticesFromDataLayerOutput" << std::endl;
  return lst_vertices.size() != 0;
}

static bool getListOfUsedNodeIDs( std::unordered_set< int64_t> &lst_UsedNodeIDs,
				  const std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryQuadrilateral> &lst_triangles) {
  for ( std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryQuadrilateral>::const_iterator it = lst_triangles.begin();
	it != lst_triangles.end(); ++it) {
    lst_UsedNodeIDs.insert( it->_nodes[ 0]);
    lst_UsedNodeIDs.insert( it->_nodes[ 1]);
    lst_UsedNodeIDs.insert( it->_nodes[ 2]);
  }
  return lst_UsedNodeIDs.size() != 0;
}

void AnalyticsModule::calculateSimplifiedMesh( const std::string &sessionID,
					       const std::string &modelID, const std::string &dataTableName,
					       const int meshID, const std::string &elementType,
					       const std::string &analysisID, const double stepValue,
					       const std::string &parameters,
					       std::string *return_binary_mesh, std::string *return_error_str) {
  bool return_demo_mesh = true;
  if ( return_demo_mesh) {
    *return_binary_mesh = demo_simplified_mesh();
    return;
  }
  // at the moment only CLI interface:
  // modelID, if it's binary, convert it to 32-digit hexastring:
  char model_ID_hex_string[ 1024];
  std::string cli_modelID = ModelID_DoHexStringConversionIfNecesary( modelID, model_ID_hex_string, 1024);

  // remove local
  // MR output in '../boundary_mesh_sessionID_modelID/_SUCCESS'
  // output in '../boundary_mesh_sessionID_modelID/part-r-00000'
  // error in '../boundary_mesh_sessionID_modelID/error.txt'
  // unlink( "boundary_mesh_sessionID_modelID/part-r-00000");
  // unlink( "boundary_mesh_sessionID_modelID/error.txt");
  // unlink( "boundary_mesh_sessionID_modelID/_SUCCESS");
  // rmdir( "boundary_mesh_sessionID_modelID");
  // delete local temporary files
  std::string yarn_output_folder = ToLower( "boundary_mesh_" + sessionID + "_" + cli_modelID);
  std::string local_tmp_folder = create_tmpdir();
  std::string local_output_folder = local_tmp_folder + "/" + yarn_output_folder;
  recursive_rmdir( yarn_output_folder.c_str());

  //GetSimplifiedMesh/dist/GetSimplifiedMesh.jar 1 60069901000000006806990100000000 Simulations_Data_V4CIMNE 1 Tetrahedra static
  std::string analytics_program = GetFullAnalyticsQualifier( "GetSimplifiedMesh");

  bool use_yarn = true;//false;;//true;
  // running java:
  int ret_cmd = 0;
  char meshIDstr[ 100];
  sprintf( meshIDstr, "%d", meshID);
  if ( !use_yarn) {
    std::string cmd_line = "java -jar " + analytics_program + " " + GetFullHBaseConfigurationFilename() + " " + 
      sessionID + " " + cli_modelID + " " + dataTableName + " " + meshIDstr + " " + elementType + " static" ;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    local_output_folder = yarn_output_folder;
  } else { 
    // Using yarn:
    // execute and copy to localdir the result's files
    // running Yarn:
    std::string cmd_line = HADOOP_YARN + " jar " + analytics_program + " " + GetFullHBaseConfigurationFilename() + " " + 
      sessionID + " " + cli_modelID + " " + dataTableName + " " + meshIDstr + " " + elementType + " static" ;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    // output in '../boundary_mesh_sessionID_modelID/part-r-00000' but in hdfs
    // error in '../boundary_mesh_sessionID_modelID/error.txt'
    // copy it to local:
    // cmd_line = hadoop_bin + "/hdfs dfs -copyToLocal boundary_mesh .";
    cmd_line = HADOOP_HDFS + " dfs -copyToLocal " + yarn_output_folder + " " + local_tmp_folder;
    ret_cmd = system( cmd_line.c_str());
    if ( ret_cmd == -1) {
      DEBUG( "Is 'yarn' and 'hdfs' in the path?\n");
    }
  }

  // ret_cmd is -1 on error

  // output in '../boundary_mesh_sessionID_modelID/part-r-00000'
  // error in '../boundary_mesh_sessionID_modelID/error.txt'
  char filename[ 8192];
  sprintf( filename, "%s/part-r-00000", local_output_folder.c_str());
  FILE *fi = fopen( filename, "rb");
 
  if (!fi) {
    // try reading error file
    bool errorfile_read = false;
    sprintf( filename, "%s/error.txt", local_output_folder.c_str());
/*    fi = fopen( filename, "r");
    if (fi) {
      const size_t size_buffer = 1024 * 1024;
      char buffer[ size_buffer + 1];
      char *ok = fgets( buffer, size_buffer, fi);
      fclose( fi);
      if ( ok) {
	*return_error_str = std::string( buffer);
	errorfile_read = true;
      }
    }*/
    std::ifstream ifs(filename);
    if (ifs.is_open()) {
    	std::stringstream buffer;
    	buffer << ifs.rdbuf();
    	*return_error_str = buffer.str();
    	errorfile_read = true;
    }
	ifs.close();
    
    if ( !errorfile_read) {
      *return_error_str = std::string( "Problems executing ") + __FUNCTION__ + 
	( use_yarn ? " Yarn" : " Java") + std::string( " job.");
      if ( use_yarn) {
	*return_error_str += std::string( "\nIs 'yarn' and 'hdfs' in the path?");
      }
    }
    return;
  }
  fclose( fi);

  std::string step_error = "";

  std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryQuadrilateral> lst_triangles;
  bool ok = getBoundaryQuadrilateralsFromJavaOutput( filename, lst_triangles);
  if ( !ok) step_error = "error in getBoundaryQuadrilateralsFromJavaOutput";

  // get the unique Node IDs used in the skin mesh
  std::unordered_set< int64_t> lst_UsedNodeIDs;
  if ( ok) {
    // get only the vertices we need
    ok = getListOfUsedNodeIDs( lst_UsedNodeIDs, lst_triangles);
  }
  if ( !ok) step_error = "error in getListOfUsedNodeIDs";

  // needs to get the vertices from the DataLayer ...
  rvGetListOfVerticesFromMesh return_data;
  if ( ok) {
    bool use_data_layer = true;
    if ( use_data_layer) {
      std::cout << "doing DataLayer::getListOfSelectedVerticesFromMesh" << std::endl;
      std::vector< int64_t> lstVertexIds;
      for ( std::unordered_set< int64_t>::const_iterator itr = lst_UsedNodeIDs.begin(); itr != lst_UsedNodeIDs.end(); itr++) {
	lstVertexIds.push_back( *itr);
      }
      DataLayerAccess::Instance()->getListOfSelectedVerticesFromMesh( return_data,
								      sessionID,
								      modelID, analysisID, stepValue,
								      meshID, lstVertexIds);
    } else {
      // the MapReduce version
      std::cout << "doing MapReduce::getListOfVerticesFromMesh" << std::endl;
      std::string error_str = MRgetListOfVerticesFromMesh( return_data,
							   sessionID, modelID, 
							   dataTableName,
							   analysisID, stepValue, 
							   meshID);
      ok = ( error_str.length() == 0);
    }
    
    std::cout << "     status: " << return_data.status << std::endl;
    if ( return_data.status == "Error") {
      ok = false;
      // const std::string not_found( "Not found");
      // if ( AreEqualNoCaseSubstr( return_data.report, not_found, not_found.size())) {
      //   _return.__set_result( (Result::type)VAL_NO_RESULT_INFORMATION_FOUND);
      // } else {
      //   _return.__set_result( (Result::type)VAL_UNKNOWN_ERROR);
      // }
    } else { // status == "Ok"
      // if ( return_data.vertex_list.size() == 0) {
      //   _return.__set_result( (Result::type)VAL_NO_MODEL_MATCHES_PATTERN);
      // }
    }
  }
  if ( !ok) step_error = "error in getListOfVerticesFromMesh";

  std::vector< VELaSSCo::BoundaryBinaryMesh::MeshPoint> lst_vertices;
  VELaSSCo::BoundaryBinaryMesh boundary_mesh;
  if ( ok) {
    ok = getBoundaryVerticesFromDataLayerOutput( return_data.vertex_list, lst_UsedNodeIDs, lst_vertices);
  }
  if ( !ok) step_error = "error in getBoundaryVerticesFromDataLayerOutput";
  if ( ok) {
    boundary_mesh.set( lst_vertices.data(), lst_vertices.size(), lst_triangles.data(), lst_triangles.size(), VELaSSCo::BoundaryBinaryMesh::STATIC);
  }
  if ( !ok) step_error = "error in boundary_mesh.set( ...)";

  // verify output:
  if ( !ok || !boundary_mesh.getNumVertices() || !boundary_mesh.getNumTriangles()) {
    // try reading error file
    bool errorfile_read = false;
    sprintf( filename, "%s/error.txt", local_output_folder.c_str());
/*    fi = fopen( filename, "r");
    if ( fi) {
      const size_t size_buffer = 1024 * 1024;
      char buffer[ size_buffer + 1];
      char *ok = fgets( buffer, size_buffer, fi);
      fclose( fi);
      if ( ok) {
	*return_error_str = std::string( buffer);
	errorfile_read = true;
      }
    }*/
    std::ifstream ifs(filename);
    if (ifs.is_open()) {
    	std::stringstream buffer;
    	buffer << ifs.rdbuf();
    	*return_error_str = buffer.str();
    	errorfile_read = true;
    }
	ifs.close();
		
    if ( !errorfile_read) {
      *return_error_str = std::string( "Problems with ") + FUNCTION_NAME + 
	( use_yarn ? " Yarn" : " Java") + std::string( " results:\n");
      if ( use_yarn) {
	*return_error_str += std::string( "Is 'yarn' and 'hdfs' in the path?\n");
      }
      if ( !ok) {
	*return_error_str += "\treading analytics output file\n";
	*return_error_str += "\tSTEP " + step_error + "\n";
      }
      char tmp[ 200];
      sprintf( tmp, "%"PRIi64, boundary_mesh.getNumVertices());
      *return_error_str += std::string( "\tnumber of vertices = ") + std::string( tmp);
      sprintf( tmp, "%"PRIi64, boundary_mesh.getNumTriangles());
      *return_error_str += std::string( " number of triangles = ") + std::string( tmp) + "\n";
    }
  } else {
    std::cout << "boundary mesh has " 
	      << boundary_mesh.getNumTriangles() << " triangles and " 
	      << boundary_mesh.getNumVertices() << " vertices." << std::endl;
    *return_binary_mesh = boundary_mesh.toString();
  }

  DEBUG( "Deleting output files ...");
  if ( use_yarn) {
    std::string cmd_line = HADOOP_HDFS + " dfs -rm -r " + yarn_output_folder;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    recursive_rmdir( local_tmp_folder.c_str());
  }

  // delete local tmp files ...
  DEBUG( "Deleting temporary files ...");
  recursive_rmdir( yarn_output_folder.c_str());
}
