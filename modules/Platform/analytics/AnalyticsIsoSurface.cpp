#include "DataLayerAccess.h" // must be included before Analytics
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
					  const std::string &modelID, const std::string &dataTableName,
					  const int meshID, const std::string &analysisID, const double stepValue,
					  const std::string &resultName, const int resultComp, const double isoValue,
					  std::string *return_binary_mesh, std::string *return_error_str)
{
  *return_error_str = "[calculateIsoSurface] -- unimplemented";
}

