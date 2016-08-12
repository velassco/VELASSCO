#pragma once


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

inline int unlink_cb( const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
  int rv = remove( fpath);
  // if ( rv)
  //   perror( fpath);
  return rv;
}

inline int recursive_rmdir( const char *path) {
  return nftw( path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

#include <sys/stat.h>
#include <sys/types.h>

inline std::string create_tmpdir() {
  char *tmp_dir = tempnam( NULL, "VQuery");
  int ret = mkdir( tmp_dir, 0755);
  assert( ret == 0); // returns -1 if error
  if ( ret != -1) {
    std::string ret_str( tmp_dir);
    free( tmp_dir);
    return ret_str;
  } else {
    return std::string( "");
  }
}

inline std::string GetFullAnalyticsQualifier( const std::string &jar_name_no_extension) {
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

inline std::string GetFullHBaseConfigurationFilename() {
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
