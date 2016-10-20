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

#pragma message( "WARNING: ")
#pragma message( "WARNING: yarn and hdfs should be in the $PATH and not hard coded in the source.")
#pragma message( "WARNING: look at GetFullHBaseConfigurationFilename() for a similar approach: PathToYarnAndHDFS()")
#pragma message( "WARNING: ")


void AnalyticsModule::getResultFromVerticesID( const std::string &sessionID, const std::string &modelID, 
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
  // yarn_output_folder is relative ...
  std::string yarn_output_folder = ToLower( "bbox_" + sessionID + "_" + cli_modelID);
  std::string local_tmp_folder = create_tmpdir();
  std::string local_output_folder = local_tmp_folder + "/" + yarn_output_folder;
  recursive_rmdir( yarn_output_folder.c_str());
  std::string analytics_program = GetFullAnalyticsQualifier( "test" );

  bool use_yarn = true;;
  // running java:
  int ret_cmd = 0;
  if ( !use_yarn) {
  std::string cmd_line = "java -jar " + analytics_program + " " + GetFullHBaseConfigurationFilename() + " " + 
      sessionID + " " + cli_modelID + " " + dataTableName;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    local_output_folder = yarn_output_folder;
  } else { 
    // Using yarn:
    // execute and copy to localdir the result's files
    // running Yarn:
    
    
    std::cout << "Full HBase Configuration: "         << GetFullHBaseConfigurationFilename() << std::endl;
    std::cout << "Session ID:      " << sessionID     << std::endl;
    std::cout << "Model ID:        " << cli_modelID   << std::endl;
    std::cout << "Data Table Name: " << dataTableName << std::endl;
    
    std::string cmd_line = HADOOP_YARN + " jar " + analytics_program + " " + GetFullHBaseConfigurationFilename() + " " + 
    sessionID + " " + cli_modelID + " " + dataTableName;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    // output in '../bbox_sessionID_modelID/part-r-00000' but in hdfs
    // error in '../bbox_sessionID_modelID/error.txt'
    // copy it to local:
    // cmd_line = hadoop_bin + "/hdfs dfs -copyToLocal bbox .";
    cmd_line = HADOOP_HDFS + " dfs -copyToLocal " + yarn_output_folder + " " + local_tmp_folder;
    ret_cmd = system( cmd_line.c_str());
    if ( ret_cmd == -1) {
      DEBUG( "Is 'yarn' and 'hdfs' in the path?\n");
    }
  }

  // ret_cmd is -1 on error

  // output in '../bbox_sessionID_modelID/part-r-00000'
  // error in '../bbox_sessionID_modelID/error.txt'
  char filename[ 8192];
  sprintf( filename, "%s/part-r-00000", local_output_folder.c_str());
  FILE *fi = fopen( filename, "r");
  
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
	( use_yarn ? " Yarn" : " Java") + std::string( " results.");
      if ( use_yarn) {
	*return_error_str += std::string( "\nIs 'yarn' and 'hdfs' in the path?");
      }
    }
  }

  DEBUG( "Deleting output files ...");
  if ( use_yarn) {
    std::string cmd_line = HADOOP_HDFS + " dfs -rm -r " + yarn_output_folder;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
  recursive_rmdir( local_tmp_folder.c_str());
  }
  // delete local tmp files ...
  DEBUG( "Deleting local temporary files ...");
  recursive_rmdir( yarn_output_folder.c_str());
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
  // yarn_output_folder is relative ...
  std::string yarn_output_folder = ToLower( "bbox_" + sessionID + "_" + cli_modelID);
  std::string local_tmp_folder = create_tmpdir();
  std::string local_output_folder = local_tmp_folder + "/" + yarn_output_folder;
  recursive_rmdir( yarn_output_folder.c_str());
  std::string analytics_program = GetFullAnalyticsQualifier( "GetBoundingBoxOfAModel");

  bool use_yarn = true;;
  // running java:
  int ret_cmd = 0;
  if ( !use_yarn) {
  std::string cmd_line = "java -jar " + analytics_program + " " + GetFullHBaseConfigurationFilename() + " " + 
      sessionID + " " + cli_modelID + " " + dataTableName;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    local_output_folder = yarn_output_folder;
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
    cmd_line = HADOOP_HDFS + " dfs -copyToLocal " + yarn_output_folder + " " + local_tmp_folder;
    ret_cmd = system( cmd_line.c_str());
    if ( ret_cmd == -1) {
      DEBUG( "Is 'yarn' and 'hdfs' in the path?\n");
    }
  }

  // ret_cmd is -1 on error

  // output in '../bbox_sessionID_modelID/part-r-00000'
  // error in '../bbox_sessionID_modelID/error.txt'
  char filename[ 8192];
  sprintf( filename, "%s/part-r-00000", local_output_folder.c_str());
  FILE *fi = fopen( filename, "r");
  
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
	( use_yarn ? " Yarn" : " Java") + std::string( " results.");
      if ( use_yarn) {
	*return_error_str += std::string( "\nIs 'yarn' and 'hdfs' in the path?");
      }
    }
  }

  DEBUG( "Deleting output files ...");
  if ( use_yarn) {
    std::string cmd_line = HADOOP_HDFS + " dfs -rm -r " + yarn_output_folder;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    recursive_rmdir( local_tmp_folder.c_str());
  }
  // delete local tmp files ...
  DEBUG( "Deleting local temporary files ...");
  recursive_rmdir( yarn_output_folder.c_str());
}


void AnalyticsModule::calculateDiscrete2Continuum(const std::string &sessionID, const std::string &modelID,
						  const std::string &analysisID, const std::string &staticMeshID, 
						  const std::string &stepOptions, const int numSteps, const double *lstSteps,
						  const std::string &cGMethod, const double width, const double cutoffFactor,
						  const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGOptions,
						  const double deltaT,  const bool doSpatialIntegral, const std::string &integralDimension, 
						  const std::string &integralDirection, std::string *returnQueryOutcome, std::string *return_error_str) {
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


    // spatial integral
    std::string do_spatial_int;
    if (doSpatialIntegral)
      do_spatial_int = "true";
    else
      do_spatial_int = "false";


    parameters+= cGMethod +  " " + s_width.str() + " " + s_cutoff.str() + " " + proc_cont + " " + do_temp_avg + " " + temporalAVGOptions + " " + s_deltaT.str() + " " + do_spatial_int + " " + integralDimension + " " + integralDirection;
    
    std::string cmd_line = HADOOP_YARN + " jar " + GetFullAnalyticsQualifier( "D2C_v2-1.0") + " d2c.v2.Main " + parameters;
    
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

static bool getMeshVerticesFromJavaOutput( const char *filename, rvGetListOfVerticesFromMesh &return_data) {
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
  std::string yarn_output_folder = ToLower( "list_vertices_" + sessionID + "_" + cli_modelID);
  std::string local_tmp_folder = create_tmpdir();
  std::string local_output_folder = local_tmp_folder + "/" + yarn_output_folder;
  recursive_rmdir( yarn_output_folder.c_str());

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
    local_output_folder = yarn_output_folder;
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
    cmd_line = HADOOP_HDFS + " dfs -copyToLocal " + yarn_output_folder + " " + local_tmp_folder;
    ret_cmd = system( cmd_line.c_str());
    if ( ret_cmd == -1) {
      DEBUG( "Is 'yarn' and 'hdfs' in the path?\n");
    }
  }

  // ret_cmd is -1 on error

  // output in '../list_vertices_sessionID_modelID/part-r-00000'
  // error in '../list_vertices_sessionID_modelID/error.txt'
  char filename[ 8192];
  sprintf( filename, "%s/part-r-00000", local_output_folder.c_str());
  FILE *fi = fopen( filename, "rb");
 
  std::string return_error_str = "";
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
    std::string cmd_line = HADOOP_HDFS + " dfs -rm -r " + yarn_output_folder;
    ret_cmd = system( cmd_line.c_str());
    recursive_rmdir( local_tmp_folder.c_str());
  }
  
  // delete local tmp files ...
  recursive_rmdir( yarn_output_folder.c_str());

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

static bool getBoundaryTrianglesFromJavaOutput( const char *filename, 
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
				  const std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle> &lst_triangles) {
  for ( std::vector< VELaSSCo::BoundaryBinaryMesh::BoundaryTriangle>::const_iterator it = lst_triangles.begin();
	it != lst_triangles.end(); ++it) {
    lst_UsedNodeIDs.insert( it->_nodes[ 0]);
    lst_UsedNodeIDs.insert( it->_nodes[ 1]);
    lst_UsedNodeIDs.insert( it->_nodes[ 2]);
  }
  return lst_UsedNodeIDs.size() != 0;
}

void AnalyticsModule::calculateBoundaryOfAMesh( const std::string &sessionID,
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
  std::string yarn_output_folder = ToLower( "boundary_mesh_" + sessionID + "_" + cli_modelID);
  std::string local_tmp_folder = create_tmpdir();
  std::string local_output_folder = local_tmp_folder + "/" + yarn_output_folder;
  recursive_rmdir( yarn_output_folder.c_str());

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
  int coordsMeshId = meshID;
  {
    // may be the mesh number for the coordinates is not the same as the current MeshID
    rvGetListOfMeshes returnedListOfMeshes;
    DataLayerAccess::Instance()->getListOfMeshes( returnedListOfMeshes, sessionID, modelID, analysisID, stepValue);
    const std::vector< MeshInfo> &listOfMeshes = returnedListOfMeshes.meshInfos;
    std::string coordsName = "";
    for ( std::vector< MeshInfo>::const_iterator mi = listOfMeshes.begin(); mi < listOfMeshes.end(); mi++) {
      if ( meshID == mi->meshNumber) {
	coordsName = mi->coordsName;
      }
    }
    if ( coordsName.size() > 0) {
      for ( std::vector< MeshInfo>::const_iterator mi = listOfMeshes.begin(); mi < listOfMeshes.end(); mi++) {
	if ( coordsName == mi->coordsName) {
	  coordsMeshId = mi->meshNumber;
	}
      }
    }
    DEBUG( "Mesh number " << meshID << " has coords in mesh '" << coordsName << "' with id = " << coordsMeshId);
  }
  rvGetListOfVerticesFromMesh return_data;
  if ( ok) {
    // bool use_data_layer = true;
    // if ( use_data_layer) {
      std::cout << "doing DataLayer::getListOfSelectedVerticesFromMesh" << std::endl;
      std::vector< int64_t> lstVertexIds;
      for ( std::unordered_set< int64_t>::const_iterator itr = lst_UsedNodeIDs.begin(); itr != lst_UsedNodeIDs.end(); itr++) {
	lstVertexIds.push_back( *itr);
      }
      DataLayerAccess::Instance()->getListOfSelectedVerticesFromMesh( return_data,
								      sessionID,
								      modelID, analysisID, stepValue,
								      coordsMeshId, lstVertexIds);
    // } else {
    if ( return_data.status == "Error") {
	std::cout << "ERROR using DataLayer getListOfSelectedVerticesFromMesh, using the MapReduce version !!!" << std::endl;
      // the MapReduce version
      std::cout << "doing MapReduce::getListOfVerticesFromMesh" << std::endl;
      std::string error_str = MRgetListOfVerticesFromMesh( return_data,
							   sessionID, modelID, 
							   dataTableName,
							   analysisID, stepValue, 
							   coordsMeshId);
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


void AnalyticsModule::createVolumeLRSplineFromBoundingBox(const std::string& sessionID,
							  const std::string& modelID,
							  const std::string &dataTableName,
							  const std::string& resultID,
							  const double stepValue,
							  const std::string& analysisID,
							  const double* bBox, // ix doubles: min(x,y,z)-max(x,y,z)
							  const double tolerance, // Use ptr to allow NULL?
							  const int numSteps, // Use ptr to allow NULL?
							  /* out */
							  std::string *return_binary_volume_lrspline,
							  std::string *result_statistics, // JSON format?
							  std::string *return_error_str) {

  if (tolerance < 0.0) {
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": The tolerance is negative, the lrspline method will set a suitable tolerance!");
  }

  double local_num_steps = numSteps;
  if (local_num_steps > 8) {
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": Number of steps requested: " << local_num_steps << ", setting it to the maximum value 8!");
    local_num_steps = 8;
  }

  // needs to get the vertices from the DataLayer ...
  rvGetListOfVerticesFromMesh return_data;
  const int meshID = 1;
  std::string analysis_id_static_mesh("");
  double step_value_static_mesh = 0.0;

  // DEBUG("SINTEF: ERROR using DataLayer getListOfVerticesFromMesh, using the MapReduce version !!!");
  // the MapReduce version
  std::cout << "doing MapReduce::getListOfVerticesFromMesh" << std::endl;
  //    HBase::TableModelEntry table_name_set;
  //  std::string data_table_name("Simulations_Data_V4CIMNE");
  std::string error_str = MRgetListOfVerticesFromMesh( return_data,
						       sessionID, modelID, 
						       dataTableName,
						       analysis_id_static_mesh, step_value_static_mesh, 
						       meshID);

  std::vector< Vertex> &lst_vertices = return_data.vertex_list;
  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	": Map Reduce version, return_status: " << return_data.status << ", # vertices:" << lst_vertices.size());  
  //    ok = ( error_str.length() == 0);

#if 0
  try {
    DataLayerAccess::Instance()->getListOfVerticesFromMesh( return_data,
							    sessionID,
							    modelID, analysis_id_static_mesh, step_value_static_mesh,
							    meshID);
    lst_vertices = return_data.vertex_list;
  } catch (...) {
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": ERROR calling getListOfVerticesFromMesh, exception was thrown!");
    return_data.status = "Error";
  }
#endif

  // mesh_vertex.__set_id( node_id);
  // mesh_vertex.__set_x( node_x);
  // mesh_vertex.__set_y( node_y);
  // mesh_vertex.__set_z( node_z);

  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	": Read the vertices, return_status: " << return_data.status << ", # vertices:" << lst_vertices.size());  

  // We then fetch the corresponding results.
#if 1
  const size_t divide_factor = 10;//1; // Using '1' means we fetch all data at once.
  const size_t batch_size = ceil(lst_vertices.size()/divide_factor);
  std::vector<int64_t> local_list_of_vert_id(lst_vertices.size()/divide_factor);
  std::cout << "HACK: Only fetching a subpart of the vertices!" << std::endl;
  std::vector<ResultOnVertex>  result_list;
  for (size_t kj = 0; kj < divide_factor; ++kj) {
    std::vector<int64_t> local_list_of_vert_id;
    local_list_of_vert_id.reserve(batch_size);
    for (size_t ki = 0; ki < batch_size; ++ki) {
      size_t ind = kj*batch_size + ki;
      if (ind > lst_vertices.size() - 1) {
	break;
      }
      local_list_of_vert_id.push_back(ind);
    }
    rvGetResultFromVerticesID return_data_res;
    if (local_list_of_vert_id.size() > 0) {
      DataLayerAccess::Instance()->getResultFromVerticesID( return_data_res, 
							    sessionID, modelID, 
							    analysisID, stepValue,
							    resultID, local_list_of_vert_id); 
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": Fetching result, iter: " << kj << ", return_status: " <<
	    return_data_res.status << ", # vertices:" << result_list.size());  
      std::vector<ResultOnVertex> local_result_list = return_data_res.result_list;
      result_list.insert(result_list.end(), local_result_list.begin(), local_result_list.end());
    } else {
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": ERROR: The list of vertices is empty.");  
    }
  }

  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	": Fetched result, # vertices:" << result_list.size());  

#endif

#if 0
  if (lst_vertices.size() == 0) {
      vector<int64_t> lstVertexIds(2);
      lstVertexIds[0] = 0;
      lstVertexIds[1] = 1;
      DataLayerAccess::Instance()->getListOfSelectedVerticesFromMesh( return_data,
								      sessionID,
								      modelID, analysisID, stepValue,
								      meshID, lstVertexIds);
      lst_vertices = return_data.vertex_list;
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": Fetching selected vertices, return_status: " << return_data.status << ", # vertices:" << lst_vertices.size());  
  }
#endif


  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	": MISSING: Parse data to Spline friendly!");

  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	": MISSING: Perform the actual Volume LRSpline Approximation!");

  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	": MISSING: Convert output data to binary format!");

  bool use_precomputed_result = true;
  if (use_precomputed_result) {
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": Returning a pre-computed binary file!");
    // We see if the we are on eddie.
    std::string precomputed_result_eddie("/localfs/home/velassco/SINTEF_test/telescope_speed_dump_31_eddie.bin");
    std::ifstream is_eddie(precomputed_result_eddie);
    bool eddie = is_eddie.good();
    // We see if the we are on acuario.
    std::string precomputed_result_acuario("/localfs/home/velassco/SINTEF_test/telescope_speed_dump_31_acuario.bin");
    std::ifstream is_acuario(precomputed_result_acuario);
    bool acuario = is_acuario.good();
    if (eddie && acuario) {
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": Unable to decide if the platform is Acuario or Eddie, on both it seems ... Picking Eddie.");
    }
    if (eddie) {
      std::ifstream filename_binary_blob(precomputed_result_eddie,
					 std::ios::binary);
      std::stringstream buffer;
      buffer << filename_binary_blob.rdbuf();
      *return_binary_volume_lrspline = buffer.str();
    } else if (acuario) {
      std::ifstream filename_binary_blob(precomputed_result_acuario,
					 std::ios::binary);
      std::stringstream buffer;
      buffer << filename_binary_blob.rdbuf();
      *return_binary_volume_lrspline = buffer.str();
    } else { // Assuming a local desktop.
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": Not on eddie or acuario, using a pre-computed binary file is not supported on local desktop!");
    }
  }

  //*return_error_str = __FUNCTION__ + std::string(": Missing the binary result.");

  return;

  // at the moment only CLI interface:
  // modelID, if it's binary, convert it to 32-digit hexastring:
  char model_ID_hex_string[ 1024];
  std::string cli_modelID = ModelID_DoHexStringConversionIfNecesary( modelID, model_ID_hex_string, 1024);

  std::string spark_output_folder = ToLower( "volume_lrspline_" + sessionID + "_" + cli_modelID);
  std::string local_tmp_folder = create_tmpdir();
  std::string local_output_folder = local_tmp_folder + "/" + spark_output_folder;
  recursive_rmdir( spark_output_folder.c_str());

}
