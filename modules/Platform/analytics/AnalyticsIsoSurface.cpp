#include <fstream>
#include <iterator>
#include <algorithm>
#include <sstream>
#include "DataLayerAccess.h" // must be included before Analytics
#include "AnalyticsCommon.h"
#include "Analytics.h"
// the format is shared between the QueryManager, the AccessLibrary and th visualiztion client
// so it's located in the AccessLibrary folder
#include "../../AccessLib/AccessLib/BoundaryBinaryMesh.h"

using namespace VELaSSCo;

#pragma message( "WARNING: ")
#pragma message( "WARNING: yarn and hdfs should be in the $PATH and not hard coded in the source.")
#pragma message( "WARNING: look at GetFullHBaseConfigurationFilename() for a similar approach: PathToYarnAndHDFS()")
#pragma message( "WARNING: ")

void AnalyticsModule::calculateIsoSurface(const std::string &sessionID,
					  const std::string &modelID,
					  const std::string &dataTableName,
					  const int meshID,
					  const std::string &analysisID,
					  const double stepValue,
					  const std::string &resultName,
					  const int resultComp,
					  const double isoValue,
					  std::string *return_binary_mesh,
					  std::string *return_error_str)
{
  std::stringstream cmdline;
  std::string pathJar(getVELaSSCoBaseDir());
  pathJar += "/AnalyticsSparkJobs/VelasscoSpark-0.1-SNAPSHOT.jar";

  std::string outputHdfsFile("Mesh_IsoSurface_");
  outputHdfsFile += sessionID;
  outputHdfsFile += ".dat";

  std::string logFile("/tmp/Output_IsoSurface_");
  logFile += sessionID;
  logFile += ".log";

  
  char strMeshID[64];
  sprintf(strMeshID, "m%06d", meshID);
  cmdline << "spark-submit --master yarn --driver-memory 3g --executor-memory 3g --num-executors 40 --class com.cimne.velassco.ComputeIsoSurfaceApp "
	  << pathJar << " --model_id " << modelID << " --analysis \"" << analysisID << "\" --timestep " << stepValue
	  << " --mesh_id " << strMeshID
	  << " --result \"" << resultName << "\" --component " << resultComp << " --isovalue " <<  isoValue
	  << " --output_path " << outputHdfsFile;
  std::string suffixV4CIMNE("_V4CIMNE");
  if(dataTableName.rfind(suffixV4CIMNE) == (dataTableName.size()-suffixV4CIMNE.size())) {
    LOGGER << " requested non official table = " << suffixV4CIMNE << std::endl;
    cmdline << " --suffix _V4CIMNE";
  }
  cmdline << " > " << logFile;
  LOGGER << "[AnalyticsModule::calculateIsoSurface] -- invoking spark job as:\n";
  LOGGER << cmdline.str() << std::endl;
  int ret = system(cmdline.str().c_str());

  if (ret != 0)
    {
      LOGGER << "spark job return status is fail = " << ret << std::endl;
      std::stringstream ss;
      ss << "error executing system, ret = " << ret;
      ss << " ( is 'spark-submit' in the path?) ";
      // here we can read the output of the system and complete de
      // error message
      *return_error_str = ss.str();
      return;
    }
  std::stringstream cmd_dfs;
  std::string localFileName("/tmp/");
  localFileName += outputHdfsFile;
  // make sure local files does not exists
  unlink( localFileName.c_str());
  cmd_dfs << "hdfs dfs -copyToLocal " << outputHdfsFile << " " << localFileName;
  ret = system(cmd_dfs.str().c_str());
  if (ret != 0)
    {
      LOGGER << "hdfs dfs return status is fail = " << ret << std::endl;
      LOGGER << " doing " << cmd_dfs.str().c_str() << std::endl;
      std::stringstream ss;
      ss << "error executing hdfs dfs -copyToLocal, ret = " << ret;
      *return_error_str = ss.str();
      return;
    }

  return_error_str->clear();
  //return_binary_mesh->clear();
  std::ifstream ifsLocal(localFileName, std::ios::binary);
  return_binary_mesh->assign(std::istreambuf_iterator<char>(ifsLocal),
  std::istreambuf_iterator<char>());
  LOGGER << "return_binary_mesh.length() = " << return_binary_mesh->length() << std::endl;
  //*return_error_str = "[calculateIsoSurface] -- almost implemented";
}

