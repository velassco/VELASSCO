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

// Global static pointer used to ensure a single instance of the class.
AnalyticsModule *AnalyticsModule::m_pInstance = NULL;

AnalyticsModule *AnalyticsModule::getInstance() {
  // Only allow one instance of class to be generated.
  std::cout << "in AnalyticsModule::getInstance" << std::endl;
  if ( !m_pInstance) {
    m_pInstance = new AnalyticsModule;
  }
  return m_pInstance;
}

// on acuario:
//#define HADOOP_HOME "/localfs/home/velassco/common/hadoop/bin/"
//on Eddie

// yarn and hdfs should be in the $PATH !!! and not hard coded in the source. 
// We'll need a configuration file eventually: $HOME/.velassco
#ifdef ACUARIO
#define HADOOP_HOME
#else
#define HADOOP_HOME "/exports/applications/apps/community/VELaSSCo/hadoop-2.7.0/bin/"
#endif

#pragma message( "WARNING: ")
#pragma message( "WARNING: yarn and hdfs should be in the $PATH and not hard coded in the source.")
#pragma message( "WARNING: look at GetFullHBaseConfigurationFilename() for a similar approach: PathToYarnAndHDFS()")
#pragma message( "WARNING: ")


#define HADOOP_YARN std::string( HADOOP_HOME"yarn")
#define HADOOP_HDFS std::string( HADOOP_HOME"hdfs")

#define USE_HOSTNAME_SUFFIX

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include "globalSettings.h"

static int unlink_cb( const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
  int rv = remove( fpath);
  // if ( rv)
  //   perror( fpath);
  return rv;
}
static int recursive_rmdir( const char *path) {
  return nftw( path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

std::string GetFullAnalyticsQualifier( const std::string &jar_name_no_extension) {
#ifdef USE_HOSTNAME_SUFFIX
  const size_t max_buffer = 10240;
  char buffer[ max_buffer];
  int res = gethostname( buffer, max_buffer);
  if ( res) {
    strcpy( buffer, "NO_NAME");
  }
  std::string suffix = "_" + std::string( buffer);
#else // USE_HOSTNAME_SUFFIX
  std::string suffix = "";
#endif // USE_HOSTNAME_SUFFIX
//  return "../Analytics/" + jar_name_no_extension + suffix + ".jar";
  const char *base_dir = getVELaSSCoBaseDir();
  return std::string( base_dir ? base_dir : ".") + "/AnalyticsYarnJobs/" + jar_name_no_extension + suffix + ".jar";
}

std::string GetFullHBaseConfigurationFilename() {
  const size_t max_buffer = 10240;
  char buffer[ max_buffer];
  int res = gethostname( buffer, max_buffer);
  if ( res) {
    strcpy( buffer, "NO_NAME");
  }
  std::string conf_file = "NotConfigured";
  if ( !strcasecmp( buffer, "pez001")) {
    conf_file = "/localfs/home/velassco/common/hbase/conf/hbase-site.xml";
  } else if ( !strcasecmp( buffer, "velassco-cluster01.ecdf.ed.ac.uk")) {
    // Eddie cluster
    conf_file = "/exports/applications/apps/community/VELaSSCo/hbase-1.1.2/conf/hbase-site.xml";
  } else {
    LOGGER << "ERROR: Unable to get the HBase configuration file for host: " << buffer << std::endl;
  }
  return conf_file;
}

void AnalyticsModule::calculateBoundingBox( const std::string &sessionID, const std::string &modelID, 
					    const std::string &dataTableName,
					    const std::string &analysisID, const int numSteps, const double *lstSteps,
					    const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
					    double *return_bbox, std::string *return_error_str) {
  // return_bbox[ 0] = -0.7;
  // return_bbox[ 1] = -0.7;
  // return_bbox[ 2] = -0.7;
  // return_bbox[ 3] =  0.7;
  // return_bbox[ 4] =  0.7;
  // return_bbox[ 5] =  0.7;
  // *return_error_str = "Ok";

  // at the moment only CLI interface:
  // modelID, if it's binary, convert it to 32-digit hexastring:
  char model_ID_hex_string[ 1024];
  std::string cli_modelID = ModelID_DoHexStringConversionIfNecesary( modelID, model_ID_hex_string, 1024);

  // remove local
  // MR output in '../bbox_sessionID_modelID/_SUCCESS'
  // output in '../bbox_sessionID_modelID/part-r-00000'
  // error in '../bbox_sessionID_modelID/error.txt'
  // unlink( "bbox_sessionID_modelID/part-r-00000");
  // unlink( "bbox_sessionID_modelID/error.txt");
  // unlink( "bbox_sessionID_modelID/_SUCCESS");
  // rmdir( "bbox_sessionID_modelID");
  // delete local temporary files
  std::string output_folder = ToLower( "bbox_" + sessionID + "_" + cli_modelID);
  recursive_rmdir( output_folder.c_str());
  std::string analytics_program = GetFullAnalyticsQualifier( "GetBoundingBoxOfAModel");

  bool use_yarn = true;;
  // running java:
  int ret_cmd = 0;
  if ( !use_yarn) {
  std::string cmd_line = "java -jar " + analytics_program + " " + GetFullHBaseConfigurationFilename() + " " + 
      sessionID + " " + cli_modelID + " " + dataTableName;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
  } else { 
    // Using yarn:
    // execute and copy to localdir the result's files
    // running Yarn:
  std::string cmd_line = HADOOP_YARN + " jar " + analytics_program + " " + GetFullHBaseConfigurationFilename() + " " + 
    sessionID + " " + cli_modelID + " " + dataTableName;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    // output in '../bbox_sessionID_modelID/part-r-00000' but in hdfs
    // error in '../bbox_sessionID_modelID/error.txt'
    // copy it to local:
    // cmd_line = hadoop_bin + "/hdfs dfs -copyToLocal bbox .";
    cmd_line = HADOOP_HDFS + " dfs -copyToLocal " + output_folder + " .";
    ret_cmd = system( cmd_line.c_str());
    if ( ret_cmd == -1) {
      DEBUG( "Is 'yarn' and 'hdfs' in the path?\n");
    }
  }

  // ret_cmd is -1 on error

  // output in '../bbox_sessionID_modelID/part-r-00000'
  // error in '../bbox_sessionID_modelID/error.txt'
  char filename[ 8192];
  sprintf( filename, "%s/part-r-00000", output_folder.c_str());
  FILE *fi = fopen( filename, "r");
  
  if (!fi) {
    // try reading error file
    bool errorfile_read = false;
    sprintf( filename, "%s/error.txt", output_folder.c_str());
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
    }    */
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
  
  // read output
  char keyword[ 1024];
  double value = 0.0;
  int num_values = 0;
  for ( int i = 0; i < 6; i++) {
    int n = fscanf( fi, "%s %lf", keyword, &value);
    if ( n == 2) {
      if (      !strcasecmp( keyword, "min_x")) { return_bbox[ 0] = value; num_values++; }
      else if ( !strcasecmp( keyword, "min_y")) { return_bbox[ 1] = value; num_values++; }
      else if ( !strcasecmp( keyword, "min_z")) { return_bbox[ 2] = value; num_values++; }
      else if ( !strcasecmp( keyword, "max_x")) { return_bbox[ 3] = value; num_values++; }
      else if ( !strcasecmp( keyword, "max_y")) { return_bbox[ 4] = value; num_values++; }
      else if ( !strcasecmp( keyword, "max_z")) { return_bbox[ 5] = value; num_values++; }
    }
  }
  fclose( fi);
  // verify output:
  if ( num_values != 6) {
    // try reading error file
    bool errorfile_read = false;
    sprintf( filename, "%s/error.txt", output_folder.c_str());
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
	( use_yarn ? " Yarn" : " Java") + std::string( " results.");
      if ( use_yarn) {
	*return_error_str += std::string( "\nIs 'yarn' and 'hdfs' in the path?");
      }
    }
  }

  if ( use_yarn) {
    std::string cmd_line = HADOOP_HDFS + " dfs -rmdir --ignore-fail-on-non-empty " + output_folder;
    ret_cmd = system( cmd_line.c_str());
  }
  // delete local tmp files ...
  recursive_rmdir( output_folder.c_str());
}


void AnalyticsModule::calculateDiscrete2Continuum(const std::string &sessionID, const std::string &modelID,
						  const std::string &analysisID, const std::string &staticMeshID, 
						  const std::string &stepOptions, const int numSteps, const double *lstSteps,
						  const std::string &cGMethod, const double width, const double cutoffFactor,
						  const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGOptions,
						  const double deltaT, std::string *returnQueryOutcome, std::string *return_error_str) {
  bool use_yarn = true;;
  
  if (use_yarn) {
	
    std::string parameters = modelID + " " + analysisID + " " + staticMeshID + " " + stepOptions + " ";
    
    std::stringstream s;
    
    // numSteps can be 0 or n
    if ( numSteps == 1)
      s << " " << lstSteps[0] << " ";
    else 
      s  << lstSteps[0] << "," << lstSteps[1] << " ";
    parameters+= s.str();
    
    std::stringstream s_width;
    s_width << width;
    
    std::stringstream s_cutoff;
    s_cutoff << cutoffFactor;
    
    std::string proc_cont;
    
    if (processContacts)
      proc_cont = "true";
    else
      proc_cont = "false";
    
    std::string do_temp_avg;
    
    if (doTemporalAVG)
      do_temp_avg = "true";
    else
      do_temp_avg = "false";
    
    // // maybe this is the width or the TemporalAVGOptions
    std::stringstream s_deltaT;
    // s_deltaT << deltaT;
    s_deltaT << deltaT;

    parameters+= cGMethod +  " " + s_width.str() + " " + s_cutoff.str() + " " + proc_cont + " " + do_temp_avg + " " + temporalAVGOptions + " " + s_deltaT.str();
    
    std::string cmd_line = HADOOP_YARN + " jar " + GetFullAnalyticsQualifier( "GetDiscrete2ContinuumOfAModel") + " " + parameters;
    
    int ret = 0;
    ret = system(cmd_line.c_str());
    DEBUG( cmd_line.c_str());
    if ( ret == -1) {
      DEBUG( "Is 'yarn' and 'hdfs' in the path?\n");
    }
    
    //result stored in HBase and outcome in a file
//    FILE *fi = fopen( "D2C/queryOutcome", "r");
//    if (!fi) {
  	std::ifstream fi("D2C/queryOutcome");
    if (!fi.is_open()) {
      // try reading error file
      bool errorfile_read = false;
/*    fi = fopen( "D2C/queryError", "r");
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
        std::ifstream ifs("D2C/queryError");
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
    //reading query outcome from file
    //*return_error_str = std::string( "NO ERROR");
    else {
      //QUERY OK
      // not using yarn ...
      
/*		const size_t size_buffer = 1024;
      char buffer[ size_buffer + 1];
      char *ok = fgets( buffer, size_buffer, fi);
      fclose( fi);
		
      if ( ok)
		*returnQueryOutcome = std::string( buffer);
*/
		std::stringstream buffer_ok;
		buffer_ok << fi.rdbuf();
		*returnQueryOutcome = buffer_ok.str();
		fi.close();
	}
   
    //remove recursevely folder D2C
   
  }

}

bool getMeshVerticesFromJavaOutput( const char *filename, rvGetListOfVerticesFromMesh &return_data) {
  std::vector< Vertex> &lst_vertices = return_data.vertex_list;
  FILE *fi = fopen( filename, "r");
  bool ok = true;
  
  // format of file is:
  // int32 num_nodes, int64 nodes[ num_nodes]\n
  ssize_t n_read = 0;
  size_t hexa_data_line_size = 64 * 1024;
  char *hexa_data_line = ( char *)malloc( hexa_data_line_size * sizeof( char));
  size_t binary_data_size = 32 * 1024;
  char *binary_data = ( char *)malloc( binary_data_size * sizeof( char));
  while ( !feof( fi)) {
    n_read = getline( &hexa_data_line, &hexa_data_line_size, fi);
    if ( n_read == -1) {
      if ( !feof( fi)) {
	return_data.__set_report( "getMeshVerticesFromJavaOutput: error reading MR output file");
	DEBUG( "getMeshVerticesFromJavaOutput: error reading MR output file");
	ok = false; 
      }
      break;
    }

   if ( FromHexString( binary_data, binary_data_size, hexa_data_line, hexa_data_line_size)) {
     return_data.__set_report( "getMeshVerticesFromJavaOutput: error translating hexadecimal string to binary data.");
     DEBUG( "getMeshVerticesFromJavaOutput: error translating hexadecimal string to binary data.");
     ok = false; 
     break;
   }

   int64_t node_id;
   double node_x, node_y, node_z;
   node_id = byteSwap< int64_t>( *( int64_t *)&binary_data[ 0]);
   node_x = byteSwap< double>( *( double *)&binary_data[  8]);
   node_y = byteSwap< double>( *( double *)&binary_data[ 16]);
   node_z = byteSwap< double>( *( double *)&binary_data[ 24]);
   Vertex mesh_vertex;
   mesh_vertex.__set_id( node_id);
   mesh_vertex.__set_x( node_x);
   mesh_vertex.__set_y( node_y);
   mesh_vertex.__set_z( node_z);

   lst_vertices.push_back( mesh_vertex);
 } // !feof( fi)

 fclose( fi);
 return ok;
}

std::string AnalyticsModule::MRgetListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &return_data, 
							  const std::string &sessionID, const std::string &modelID, 
							  const std::string &dataTableName,
							  const std::string &analysisID, const double stepValue, 
							  const int32_t meshID) {
  // at the moment only CLI interface:
  // modelID, if it's binary, convert it to 32-digit hexastring:
  char model_ID_hex_string[ 1024];
  std::string cli_modelID = ModelID_DoHexStringConversionIfNecesary( modelID, model_ID_hex_string, 1024);

  // remove local
  // MR output in '../list_vertices_sessionID_modelID/_SUCCESS'
  // output in '../list_vertices_sessionID_modelID/part-r-00000'
  // error in '../list_vertices_sessionID_modelID/error.txt'
  // unlink( "list_vertices_sessionID_modelID/part-r-00000");
  // unlink( "list_vertices_sessionID_modelID/error.txt");
  // unlink( "list_vertices_sessionID_modelID/_SUCCESS");
  // rmdir( "list_vertices_sessionID_modelID");
  // delete local temporary files
  std::string output_folder = ToLower( "list_vertices_" + sessionID + "_" + cli_modelID);
  recursive_rmdir( output_folder.c_str());

  //GetBoundaryOfAMesh/dist/GetBoundaryOfAMesh.jar 1 60069901000000006806990100000000 Simulations_Data_V4CIMNE 1 static
  std::string analytics_program = GetFullAnalyticsQualifier( "GetListOfVerticesFromMesh");

  bool use_yarn = true;//false; // true;;
  // running java:
  int ret_cmd = 0;
  char meshIDstr[ 100];
  sprintf( meshIDstr, "%d", meshID);
  if ( !use_yarn) {
    std::string cmd_line = "java -jar " + analytics_program + " " + GetFullHBaseConfigurationFilename() + " " + 
      sessionID + " " + cli_modelID + " " + dataTableName + " " + meshIDstr + " static" ;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
  } else { 
    // Using yarn: execute and copy to localdir the result's files
    std::string cmd_line = HADOOP_YARN + " jar " + analytics_program + " " + GetFullHBaseConfigurationFilename() + " " + 
      sessionID + " " + cli_modelID + " " + dataTableName + " " + meshIDstr + " static" ;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    // output in '../list_vertices_sessionID_modelID/part-r-00000' but in hdfs
    // error in '../list_vertices_sessionID_modelID/error.txt'
    // copy it to local:
    // cmd_line = hadoop_bin + "/hdfs dfs -copyToLocal bbox .";
    cmd_line = HADOOP_HDFS + " dfs -copyToLocal " + output_folder + " .";
    ret_cmd = system( cmd_line.c_str());
    if ( ret_cmd == -1) {
      DEBUG( "Is 'yarn' and 'hdfs' in the path?\n");
    }
  }

  // ret_cmd is -1 on error

  // output in '../list_vertices_sessionID_modelID/part-r-00000'
  // error in '../list_vertices_sessionID_modelID/error.txt'
  char filename[ 8192];
  sprintf( filename, "%s/part-r-00000", output_folder.c_str());
  FILE *fi = fopen( filename, "rb");
 
  std::string return_error_str = "";
  if (!fi) {
    // try reading error file
    bool errorfile_read = false;
    sprintf( filename, "%s/error.txt", output_folder.c_str());
/*    fi = fopen( filename, "r");
      if (fi) {
      const size_t size_buffer = 1024 * 1024;
      char buffer[ size_buffer + 1];
      char *ok = fgets( buffer, size_buffer, fi);
      fclose( fi);
      if ( ok) {
	return_error_str = std::string( buffer);
	errorfile_read = true;
      }
    }*/
    std::ifstream ifs(filename);
    if (ifs.is_open()) {
    	std::stringstream buffer;
    	buffer << ifs.rdbuf();
    	return_error_str = buffer.str();
    	errorfile_read = true;
    }
    
    if ( !errorfile_read) {
      return_error_str = std::string( "Problems executing ") + __FUNCTION__ + 
	( use_yarn ? " Yarn" : " Java") + std::string( " job.");
      if ( use_yarn) {
	return_error_str += std::string( "\nIs 'yarn' and 'hdfs' in the path?");
      }
    }
    return return_error_str;
  }
  fclose( fi);


  std::string step_error = "";
  bool ok = getMeshVerticesFromJavaOutput( filename, return_data);
  if ( !ok) step_error = "error in getMeshVerticesFromJavaOutput";

  // verify output:
  if ( !ok) {
    return_error_str += "\treading analytics output file\n";
    return_error_str += "\tSTEP " + step_error + "\n";
    return_data.__set_status( "Error");
  } else {
    std::cout << "read " << return_data.vertex_list.size() << " vertices." << std::endl;
    return_data.__set_status( "Ok");
  }
  
  if ( use_yarn) {
    std::string cmd_line = HADOOP_HDFS + " dfs -rmdir --ignore-fail-on-non-empty " + output_folder;
    ret_cmd = system( cmd_line.c_str());
  }
  
  // delete local tmp files ...
  recursive_rmdir( output_folder.c_str());

  return return_error_str;
}





static std::string demo_boundary_mesh() {
  // This is code from the DemoServer :
  VELaSSCo::BoundaryBinaryMesh::MeshPoint lst_vertices[] = {
    {  0, {  0.000000,  1.000000,  0.000000}},
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
    { 17, {  0.000000, -1.000000,  0.000000}}
  };
  VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle lst_triangles[] = {
    { 3, { 17, 13, 16}},
    { 3, {  6,  8, 13}},
    { 3, {  8,  9, 16}},
    { 3, {  8, 16, 13}},
    { 3, {  8,  6,  2}},
    { 3, {  4,  9,  8}},
    { 3, {  2,  0,  4}},
    { 3, {  2,  4,  8}},
    { 3, { 17, 16, 14}},
    { 3, {  9, 10, 16}},
    { 3, { 10,  7, 14}},
    { 3, { 10, 14, 16}},
    { 3, { 10,  9,  4}},
    { 3, {  3,  7, 10}},
    { 3, {  4,  0,  3}},
    { 3, {  4,  3, 10}},
    { 3, { 17, 15, 13}},
    { 3, {  5, 11, 15}},
    { 3, { 11,  6, 13}},
    { 3, { 11, 13, 15}},
    { 3, { 11,  5,  1}},
    { 3, {  2,  6, 11}},
    { 3, {  1,  0,  2}},
    { 3, {  1,  2, 11}},
    { 3, { 17, 14, 15}},
    { 3, {  7, 12, 14}},
    { 3, { 12,  5, 15}},
    { 3, { 12, 15, 14}},
    { 3, { 12,  7,  3}},
    { 3, {  1,  5, 12}},
    { 3, {  3,  0,  1}},
    { 3, {  3,  1, 12}}
  };
  VELaSSCo::BoundaryBinaryMesh demo_mesh;
  int64_t num_vertices = ( int64_t)( sizeof( lst_vertices) / sizeof( VELaSSCo::BoundaryBinaryMesh::MeshPoint));
  int64_t num_triangles = ( int64_t)( sizeof( lst_triangles) / sizeof( VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle));
  demo_mesh.set( lst_vertices, num_vertices, lst_triangles, num_triangles, VELaSSCo::BoundaryBinaryMesh::STATIC);
  // end of code from DemoServer.cpp
  return demo_mesh.toString();
}

bool getBoundaryTrianglesFromJavaOutput( const char *filename, 
					 std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle> &lst_triangles) {
 FILE *fi = fopen( filename, "r");
 bool ok = true;

 // format of file is:
 // int32 num_nodes, int64 nodes[ num_nodes]\n
 VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle triangle;
 ssize_t n_read = 0;
 size_t hexa_data_line_size = 64 * 1024;
 char *hexa_data_line = ( char *)malloc( hexa_data_line_size * sizeof( char));
 size_t binary_data_size = 32 * 1024;
 char *binary_data = ( char *)malloc( binary_data_size * sizeof( char));
 while ( !feof( fi)) {
   n_read = getline( &hexa_data_line, &hexa_data_line_size, fi);
   if ( n_read == -1) {
     if ( !feof( fi)) {
       DEBUG( "getBoundaryTrianglesFromJavaOutput: error reading MR output file");
       ok = false; 
     }
     break;
   }

   if ( FromHexString( binary_data, binary_data_size, hexa_data_line, hexa_data_line_size)) {
     DEBUG( "getBoundaryTrianglesFromJavaOutput: error translating hexadecimal string to binary data.");
     ok = false; 
     break;
   }

   triangle._num_nodes = byteSwap< int>( *( int *)&binary_data[ 0]);
   // assert( triangle._num_nodes == 3);
   if ( triangle._num_nodes != 3) {
     // ok = false; //just finished reading to show something ...
     DEBUG( "getBoundaryTrianglesFromJavaOutput: read num_nodes != 3.");
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

bool getBoundaryVerticesFromDataLayerOutput( const std::vector< Vertex> &DL_vertices, 
					     const std::unordered_set< int64_t> &lst_UsedNodeIDs,
					     std::vector< VELaSSCo::BoundaryBinaryMesh::MeshPoint> &lst_vertices) {
  for ( std::vector< Vertex>::const_iterator it = DL_vertices.begin(); it != DL_vertices.end(); ++it) {
    if ( lst_UsedNodeIDs.find( it->id) != lst_UsedNodeIDs.end()) {
      VELaSSCo::BoundaryBinaryMesh::MeshPoint tmp;
      tmp._id = it->id;
      tmp._coords[ 0] = it->x;
      tmp._coords[ 1] = it->y;
      tmp._coords[ 2] = it->z;
      lst_vertices.push_back( tmp);
    }
  }
  return lst_vertices.size() != 0;
}

bool getListOfUsedNodeIDs( std::unordered_set< int64_t> &lst_UsedNodeIDs,
			    const std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle> &lst_triangles) {
  for ( std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle>::const_iterator it = lst_triangles.begin();
	it != lst_triangles.end(); ++it) {
    lst_UsedNodeIDs.insert( it->_nodes[ 0]);
    lst_UsedNodeIDs.insert( it->_nodes[ 1]);
    lst_UsedNodeIDs.insert( it->_nodes[ 2]);
  }
  return lst_UsedNodeIDs.size() != 0;
}

void AnalyticsModule::calculateBoundaryOfAMesh( const std::string &sessionID, const std::string &DataLayer_sessionID,
						const std::string &modelID, const std::string &dataTableName,
						const int meshID, const std::string &elementType,
						const std::string &analysisID, const double stepValue,
						std::string *return_binary_mesh, std::string *return_error_str) {
  bool return_demo_mesh = false;
  if ( return_demo_mesh) {
    *return_binary_mesh = demo_boundary_mesh();
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
  std::string output_folder = ToLower( "boundary_mesh_" + sessionID + "_" + cli_modelID);
  recursive_rmdir( output_folder.c_str());

  //GetBoundaryOfAMesh/dist/GetBoundaryOfAMesh.jar 1 60069901000000006806990100000000 Simulations_Data_V4CIMNE 1 Tetrahedra static
  std::string analytics_program = GetFullAnalyticsQualifier( "GetBoundaryOfAMesh");

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
    cmd_line = HADOOP_HDFS + " dfs -copyToLocal " + output_folder + " .";
    ret_cmd = system( cmd_line.c_str());
    if ( ret_cmd == -1) {
      DEBUG( "Is 'yarn' and 'hdfs' in the path?\n");
    }
  }

  // ret_cmd is -1 on error

  // output in '../boundary_mesh_sessionID_modelID/part-r-00000'
  // error in '../boundary_mesh_sessionID_modelID/error.txt'
  char filename[ 8192];
  sprintf( filename, "%s/part-r-00000", output_folder.c_str());
  FILE *fi = fopen( filename, "rb");
 
  if (!fi) {
    // try reading error file
    bool errorfile_read = false;
    sprintf( filename, "%s/error.txt", output_folder.c_str());
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

  std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle> lst_triangles;
  bool ok = getBoundaryTrianglesFromJavaOutput( filename, lst_triangles);
  if ( !ok) step_error = "error in getBoundaryTrianglesFromJavaOutput";

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
								      DataLayer_sessionID,
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
    sprintf( filename, "%s/error.txt", output_folder.c_str());
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

  if ( use_yarn) {
    std::string cmd_line = HADOOP_HDFS + " dfs -rmdir --ignore-fail-on-non-empty " + output_folder;
    ret_cmd = system( cmd_line.c_str());
  }

  // delete local tmp files ...
  recursive_rmdir( output_folder.c_str());
}
