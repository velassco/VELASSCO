#include <sstream>
#include <stddef.h>  // defines NULL
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "Helpers.h"
#include "Analytics.h"

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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>

static int unlink_cb( const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
  int rv = remove( fpath);
  // if ( rv)
  //   perror( fpath);
  return rv;
}
static int recursive_rmdir( const char *path) {
  return nftw( path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
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

  bool use_yarn = true;
  // running java:
  int ret_cmd = 0;
  if ( !use_yarn) {
    std::string cmd_line = "java -jar ../Analytics/GetBoundingBoxOfAModel.jar " + 
      sessionID + " " + cli_modelID + " " + dataTableName;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
  } else { 
    // Using yarn:
    // execute and copy to localdir the result's files
    // running Yarn:
    std::string hadoop_home = "/localfs/home/velassco/common";
    std::string hadoop_bin = hadoop_home + "/hadoop/bin";
    std::string cmd_line = hadoop_bin + "/yarn jar ../Analytics/GetBoundingBoxOfAModel.jar " + 
      sessionID + " " + cli_modelID + " " + dataTableName;
    DEBUG( cmd_line);
    ret_cmd = system( cmd_line.c_str());
    // output in '../bbox_sessionID_modelID/part-r-00000' but in hdfs
    // error in '../bbox_sessionID_modelID/error.txt'
    // copy it to local:
    // cmd_line = hadoop_bin + "/hdfs dfs -copyToLocal bbox .";
    cmd_line = hadoop_bin + "/hdfs dfs -copyToLocal " + output_folder + " .";
    ret_cmd = system( cmd_line.c_str());
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
    fi = fopen( filename, "r");
    if (fi) {
      const size_t size_buffer = 1024 * 1024;
      char buffer[ size_buffer + 1];
      char *ok = fgets( buffer, size_buffer, fi);
      fclose( fi);
      if ( ok) {
	*return_error_str = std::string( buffer);
	errorfile_read = true;
      }
    }
    if ( !errorfile_read) {
      *return_error_str = std::string( "Problems executing ") + __FUNCTION__ + 
	( use_yarn ? " Yarn" : " Java") + std::string( " job.");
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
    fi = fopen( filename, "r");
    if ( fi) {
      const size_t size_buffer = 1024 * 1024;
      char buffer[ size_buffer + 1];
      char *ok = fgets( buffer, size_buffer, fi);
      fclose( fi);
      if ( ok) {
	*return_error_str = std::string( buffer);
	errorfile_read = true;
      }
    }
    if ( !errorfile_read) {
      *return_error_str = std::string( "Problems with ") + FUNCTION_NAME + 
	( use_yarn ? " Yarn" : " Java") + std::string( " results.");
    }
  }

  if ( use_yarn) {
    // cmd_line = hadoop_bin + "/hdfs dfs -rmdir --ignore-fail-on-non-empty bbox";
    // ret = system( cmd_line.c_str());
  }

  // delete local tmp files ...
  // recursive_rmdir( "bbox");
}


void AnalyticsModule::calculateDiscrete2Continuum(const std::string &sessionID, const std::string &modelID,
						  const std::string &analysisID, const std::string &staticMeshID, 
						  const std::string &stepOptions, const int numSteps, const double *lstSteps,
						  const std::string &cGMethod, const double width, const double cutoffFactor,
						  const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGOptions,
						  const double deltaT, std::string *returnQueryOutcome, std::string *return_error_str) {
  bool use_yarn = true;
  
  if (use_yarn) {
    std::string hadoop_home = "/localfs/home/velassco/common";
    std::string hadoop_bin = hadoop_home + "/hadoop/bin";
	
    std::string parameters = modelID + " " + analysisID + " " + staticMeshID + " " + stepOptions + " ";
    
    std::stringstream s;
    
    // numSteps can be 0 or n
    if ( numSteps == 1)
      s << " " << lstSteps[0];	
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
    
    std::string cmd_line = hadoop_bin + "/yarn jar ../Analytics/GetDiscrete2ContinuumOfAModel.jar " + parameters;
    
    int ret = 0;
    ret = system(cmd_line.c_str());
    
    //result stored in HBase and outcome in a file
    FILE *fi = fopen( "D2C/queryOutcome", "r");
  
    if (!fi) {
      // try reading error file
      bool errorfile_read = false;
      fi = fopen( "D2C/queryError", "r");
      if (fi) {
	const size_t size_buffer = 1024 * 1024;
	char buffer[ size_buffer + 1];
	char *ok = fgets( buffer, size_buffer, fi);
	fclose( fi);
	if ( ok) {
	  *return_error_str = std::string( buffer);
	  errorfile_read = true;
	}
      }
      if ( !errorfile_read) {
	*return_error_str = std::string( "Problems executing ") + __FUNCTION__ + 
	  ( use_yarn ? " Yarn" : " Java") + std::string( " job.");
      }
      return;
    }
    //reading query outcome from file
    //*return_error_str = std::string( "NO ERROR");
    else {
      //QUERY OK
      // not using yarn ...
      const size_t size_buffer = 1024;
      char buffer[ size_buffer + 1];
      char *ok = fgets( buffer, size_buffer, fi);
      fclose( fi);

      if ( ok)
	*returnQueryOutcome = std::string( buffer);
    }
   
    //remove recursevely folder D2C
   
  }

}


void AnalyticsModule::calculateBoundaryOfAMesh( const std::string &sessionID, const std::string &modelID, const std::string &dataTableName,
			       const int meshID, const std::string &analysisID, const double stepValue,
			       std::string *return_binary_mesh, std::string *return_error_str) {
  // This is code from the DemoServer :

  // if this is really needed, then may be it's better to change num_nodes to be int64_t ...
#pragma pack( push, 1)
  typedef struct {
    int64_t _id;
    double _coord[ 3];
  } MeshPoint;
  typedef struct {
    int _num_nodes;
    int64_t _nodes[ 3];
  } BoundaryTriangle;
#pragma pack( pop)
  MeshPoint lst_vertices[] = {
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
  BoundaryTriangle lst_triangles[] = {
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
  int64_t num_vertices = ( int64_t)( sizeof( lst_vertices) / sizeof( MeshPoint));
  int64_t num_triangles = ( int64_t)( sizeof( lst_triangles) / sizeof( BoundaryTriangle));
  std::ostringstream oss;
  oss << "NumberOfVertices: " << num_vertices << std::endl;
  oss << "NumberOfFaces: " << num_triangles << std::endl;
  oss.write( ( const char *)lst_vertices, sizeof( lst_vertices));
  oss.write( ( const char *)lst_triangles, sizeof( lst_triangles));

  *return_binary_mesh = oss.str();

  // end of code from DemoServer.cpp

////   // at the moment only CLI interface:
////   // modelID, if it's binary, convert it to 32-digit hexastring:
////   char model_ID_hex_string[ 1024];
////   std::string cli_modelID = ModelID_DoHexStringConversionIfNecesary( modelID, model_ID_hex_string, 1024);
//// 
////   // remove local
////   // MR output in '../bbox_sessionID_modelID/_SUCCESS'
////   // output in '../bbox_sessionID_modelID/part-r-00000'
////   // error in '../bbox_sessionID_modelID/error.txt'
////   // unlink( "bbox_sessionID_modelID/part-r-00000");
////   // unlink( "bbox_sessionID_modelID/error.txt");
////   // unlink( "bbox_sessionID_modelID/_SUCCESS");
////   // rmdir( "bbox_sessionID_modelID");
////   // delete local temporary files
////   std::string output_folder = ToLower( "bbox_" + sessionID + "_" + cli_modelID);
////   recursive_rmdir( output_folder.c_str());
//// 
////   bool use_yarn = true;
////   // running java:
////   int ret_cmd = 0;
////   if ( !use_yarn) {
////     std::string cmd_line = "java -jar ../Analytics/GetBoundingBoxOfAModel.jar " + 
////       sessionID + " " + cli_modelID + " " + dataTableName;
////     DEBUG( cmd_line);
////     ret_cmd = system( cmd_line.c_str());
////   } else { 
////     // Using yarn:
////     // execute and copy to localdir the result's files
////     // running Yarn:
////     std::string hadoop_home = "/localfs/home/velassco/common";
////     std::string hadoop_bin = hadoop_home + "/hadoop/bin";
////     std::string cmd_line = hadoop_bin + "/yarn jar ../Analytics/GetBoundingBoxOfAModel.jar " + 
////       sessionID + " " + cli_modelID + " " + dataTableName;
////     DEBUG( cmd_line);
////     ret_cmd = system( cmd_line.c_str());
////     // output in '../bbox_sessionID_modelID/part-r-00000' but in hdfs
////     // error in '../bbox_sessionID_modelID/error.txt'
////     // copy it to local:
////     // cmd_line = hadoop_bin + "/hdfs dfs -copyToLocal bbox .";
////     cmd_line = hadoop_bin + "/hdfs dfs -copyToLocal " + output_folder + " .";
////     ret_cmd = system( cmd_line.c_str());
////   }
//// 
////   // ret_cmd is -1 on error
//// 
////   // output in '../bbox_sessionID_modelID/part-r-00000'
////   // error in '../bbox_sessionID_modelID/error.txt'
////   char filename[ 8192];
////   sprintf( filename, "%s/part-r-00000", output_folder.c_str());
////   FILE *fi = fopen( filename, "r");
////   
////   if (!fi) {
////     // try reading error file
////     bool errorfile_read = false;
////     sprintf( filename, "%s/error.txt", output_folder.c_str());
////     fi = fopen( filename, "r");
////     if (fi) {
////       const size_t size_buffer = 1024 * 1024;
////       char buffer[ size_buffer + 1];
////       char *ok = fgets( buffer, size_buffer, fi);
////       fclose( fi);
////       if ( ok) {
//// 	*return_error_str = std::string( buffer);
//// 	errorfile_read = true;
////       }
////     }
////     if ( !errorfile_read) {
////       *return_error_str = std::string( "Problems executing ") + __FUNCTION__ + 
//// 	( use_yarn ? " Yarn" : " Java") + std::string( " job.");
////     }
////     return;
////   }
////   
////   // read output
////   char keyword[ 1024];
////   double value = 0.0;
////   int num_values = 0;
////   for ( int i = 0; i < 6; i++) {
////     int n = fscanf( fi, "%s %lf", keyword, &value);
////     if ( n == 2) {
////       if (      !strcasecmp( keyword, "min_x")) { return_bbox[ 0] = value; num_values++; }
////       else if ( !strcasecmp( keyword, "min_y")) { return_bbox[ 1] = value; num_values++; }
////       else if ( !strcasecmp( keyword, "min_z")) { return_bbox[ 2] = value; num_values++; }
////       else if ( !strcasecmp( keyword, "max_x")) { return_bbox[ 3] = value; num_values++; }
////       else if ( !strcasecmp( keyword, "max_y")) { return_bbox[ 4] = value; num_values++; }
////       else if ( !strcasecmp( keyword, "max_z")) { return_bbox[ 5] = value; num_values++; }
////     }
////   }
////   fclose( fi);
////   // verify output:
////   if ( num_values != 6) {
////     // try reading error file
////     bool errorfile_read = false;
////     sprintf( filename, "%s/error.txt", output_folder.c_str());
////     fi = fopen( filename, "r");
////     if ( fi) {
////       const size_t size_buffer = 1024 * 1024;
////       char buffer[ size_buffer + 1];
////       char *ok = fgets( buffer, size_buffer, fi);
////       fclose( fi);
////       if ( ok) {
//// 	*return_error_str = std::string( buffer);
//// 	errorfile_read = true;
////       }
////     }
////     if ( !errorfile_read) {
////       *return_error_str = std::string( "Problems with ") + FUNCTION_NAME + 
//// 	( use_yarn ? " Yarn" : " Java") + std::string( " results.");
////     }
////   }
//// 
////   if ( use_yarn) {
////     // cmd_line = hadoop_bin + "/hdfs dfs -rmdir --ignore-fail-on-non-empty bbox";
////     // ret = system( cmd_line.c_str());
////   }
//// 
////   // delete local tmp files ...
////   // recursive_rmdir( "bbox");
}
