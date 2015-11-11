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
  // MR output in '../bbox/_SUCCESS'
  // output in '../bbox/part-r-00000'
  // error in '../bbox/error.txt'
  // unlink( "bbox/part-r-00000");
  // unlink( "bbox/error.txt");
  // unlink( "bbox/_SUCCESS");
  // rmdir( "bbox");
  // delete local temporary files
  recursive_rmdir( "bbox");

  bool use_yarn = true;
  // running java:
  if ( !use_yarn) {
    std::string cmd_line = "java -jar ../Analytics/GetBoundingBoxOfAModel.jar " + cli_modelID + " " + dataTableName;
    DEBUG( cmd_line);
    int ret = system( cmd_line.c_str());
  } else { 
    // Using yarn:
    // execute and copy to localdir the result's files
    // running Yarn:
    std::string hadoop_home = "/localfs/home/velassco/common";
    std::string hadoop_bin = hadoop_home + "/hadoop/bin";
    std::string cmd_line = hadoop_bin + "/yarn jar ../Analytics/GetBoundingBoxOfAModel.jar " + cli_modelID + " " + dataTableName;
    int ret = 0;
    DEBUG( cmd_line);
    ret = system( cmd_line.c_str());
    // output in '../bbox/part-r-00000' but in hdfs
    // error in '../bbox/error.txt'
    // copy it to local:
    cmd_line = hadoop_bin + "/hdfs dfs -copyToLocal bbox .";
    ret = system( cmd_line.c_str());
  }

  // output in '../bbox/part-r-00000'
  // error in '../bbox/error.txt'
  FILE *fi = fopen( "bbox/part-r-00000", "r");
  if ( !fi) {
    // try reading error file
    bool errorfile_read = false;
    fi = fopen( "bbox/error.txt", "r");
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
    fi = fopen( "bbox/error.txt", "r");
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


/*
void calculateDiscrete2Continuum(const std::string &sessionID, const std::string &modelID,
								const std::string &analysisName, const std::string &staticMeshID, 
								const std::string &timestepOptions, const double *timesteps,
								const std::string &CGMethod, const double width, const double cutoffFactor,
								const bool processContacts, const bool doTemporalAverage, const std::string &TAOptions,
								const std::string &prefixHBaseTableToUse, std::string *returnQueryOutcome, std::string *return_error_str){

 bool use_yarn = true;
 
 if (use_yarn) {
	std::string hadoop_home = "/localfs/home/velassco/common";
	std::string hadoop_bin = hadoop_home + "/hadoop/bin";
	std::string parameters = modelID + " " + analysisName + " " + staticMeshID + " " + timestepOptions + " ";

	std::stringstream s;

	size_timesteps = 1;

	if (size_timesteps == 1)
		s << " " << timesteps[0];	
	else
		s  << timesteps[0] << "," << timesteps[1] << " ";

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

	if (doTemporalAverage)
		do_temp_avg = "true";
	else
		do_temp_avg = "false";

	std::stringstream s_deltaT;
	s_deltaT << deltaT;

	parameters+= CGMethod +  " " + s_width.str() + " " + s_cutoff.str() + " " + proc_cont + " " + do_temp_avg + " " + TAOptions + " " + s_deltaT.str();

	std::string cmd_line = hadoop_bin + "/yarn jar ../Analytics/GetDiscrete2ContinuumOfAModel.jar " + parameters;
  
	int ret = 0;
	ret = system(cmd_line.c_str());
	
  //result stored in HBase and outcome in a file
  //reading query outcome from file
  *return_error_str = std::string( "NO ERROR")
  *returnQueryOutcome = std::string( "NO ERROR")
 }
}*/