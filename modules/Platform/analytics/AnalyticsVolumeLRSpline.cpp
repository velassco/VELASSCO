#include <fstream>
#include <iterator>
#include <algorithm>
#include <sstream>
#include "DataLayerAccess.h" // must be included before Analytics
#include "AnalyticsCommon.h"
#include "Analytics.h"

using namespace VELaSSCo;

#pragma message( "WARNING: ")
#pragma message( "WARNING: yarn and hdfs should be in the $PATH and not hard coded in the source.")
#pragma message( "WARNING: look at GetFullHBaseConfigurationFilename() for a similar approach: PathToYarnAndHDFS()")
#pragma message( "WARNING: ")



void AnalyticsModule::createVolumeLRSplineFromBoundingBox(const std::string& sessionID,
							  const std::string& modelID,
							  const std::string &dataTableName,
							  const int meshID,
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

  int num_ref_levels = numSteps*3; // Refinement levels is counted per direction.
  const int max_num_ref_levels = 24;
  if (num_ref_levels > max_num_ref_levels) {
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": Number of ref levels requested: " << num_ref_levels << ", setting it to the maximum value " <<
	  max_num_ref_levels << "!");
    num_ref_levels = max_num_ref_levels;
  }

  // at the moment only CLI interface:
  // modelID, if it's binary, convert it to 32-digit hexastring:
  char model_ID_hex_string[ 1024];
  std::string cli_modelID = ModelID_DoHexStringConversionIfNecesary( modelID, model_ID_hex_string, 1024);

  std::string spark_output_folder = ToLower( "volume_lrspline_" + sessionID + "_" + cli_modelID);
  std::string local_tmp_folder = create_tmpdir();
  std::string local_output_folder = local_tmp_folder + "/" + spark_output_folder;
  std::cout << "local_output_folder: " << local_output_folder << std::endl;
  recursive_rmdir( spark_output_folder.c_str());

  system(("mkdir -p "+local_output_folder).c_str());
 
  bool use_yarn = false;//true;

  std::string analysis_id_static_mesh("");
  double step_value_static_mesh = 0.0;
  // needs to get the vertices from the DataLayer ...
  rvGetListOfVerticesFromMesh return_data;
#if 1
  // std::string analysis_id_static_mesh("");
  // double step_value_static_mesh = 0.0;
  std::cout << "doing MapReduce::getListOfVerticesFromMesh" << std::endl;
  std::string error_str = MRgetListOfVerticesFromMesh( return_data,
						       sessionID, modelID, 
						       dataTableName,
						       analysis_id_static_mesh, step_value_static_mesh, 
						       meshID);

  std::vector< Vertex> &lst_vertices = return_data.vertex_list;
  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	": Map Reduce version, return_status: " << return_data.status << ", # vertices:" << lst_vertices.size());  
  //    ok = ( error_str.length() == 0);

#else
  // This version caused IO error on Acuario for the Telescope model (4.2 mill nodes).
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

  DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	": Read the vertices, return_status: " << return_data.status << ", # vertices:" << lst_vertices.size());  
#endif

  // We then fetch the corresponding results.
  // When trying to fetch missing data (due to corrupt table or invalid id) it helped to split up the fetching.
  const size_t divide_factor = 1;//10;//1; // Using '1' means we fetch all the result data at once.
  const size_t batch_size = ceil(lst_vertices.size()/divide_factor);
  std::vector<ResultOnVertex>  result_list;
  for (size_t kj = 0; kj < divide_factor; ++kj) {
    std::vector<int64_t> local_list_of_vert_id;
    local_list_of_vert_id.reserve(batch_size);
    for (size_t ki = 0; ki < batch_size; ++ki) {
      size_t ind = kj*batch_size + ki;
      if (ind > lst_vertices.size() - 1) {
	break;
      }
      local_list_of_vert_id.push_back(lst_vertices[ind].id);
    }
    rvGetResultFromVerticesID return_data_res;
    if (local_list_of_vert_id.size() > 0) {
      DataLayerAccess::Instance()->getResultFromVerticesID( return_data_res, 
							    sessionID, modelID, 
							    analysisID, stepValue,
							    resultID, local_list_of_vert_id); 
      std::vector<ResultOnVertex> local_result_list = return_data_res.result_list;
      result_list.insert(result_list.end(), local_result_list.begin(), local_result_list.end());
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": Fetched result, iter: " << kj << ", return_status: " <<
	    return_data_res.status << ", # vertices:" << result_list.size());  
    } else {
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": ERROR: The list of vertices is empty.");  
    }
  }

  // We write the data to local file.
  int num_nodes = lst_vertices.size();
  // std::vector<double> nodes;
  // std::vector<double> results;
  std::cout << "spark_output_folder: " << spark_output_folder << std::endl;
  std::string local_nodes_filename = local_output_folder + "/lr_vol_appr_input.txt";
  std::ofstream fileout(local_nodes_filename.c_str());
  fileout << num_nodes << std::endl;
  for (size_t ki = 0; ki < lst_vertices.size(); ++ki) {
    if (lst_vertices[ki].id != result_list[ki].id) {
      // This may happen when using map reduce I guess, but not sure, result could be sorted.
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": ERROR: The ordering of nodes and result is not the same!");
      return;
    }

    fileout << lst_vertices[ki].x << " " << lst_vertices[ki].y << " " << lst_vertices[ki].z;
    for (size_t kj = 0; kj < result_list[ki].value.size(); ++kj) {
      fileout << " " << result_list[ki].value[kj];
    }
    fileout << "\n";
  }

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

  bool use_precomputed_result = true;
  if (!use_precomputed_result) {
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": UNDER CONSTRUCTION: Perform the actual Volume LRSpline Approximation!");

    std::stringstream cmdline;
    std::string pathPython(getVELaSSCoBaseDir());
    pathPython += "/AnalyticsSparkJobs/spark_volume_lr_spline.py";

    std::string outputHdfsFile("Volume_LRSpline_");
    outputHdfsFile += sessionID;
    outputHdfsFile += ".bin";

    std::string logFile("/tmp/Output_Volume_LRSpline_");
    logFile += sessionID;
    logFile += ".log";

    const int binary = 1;
    const int field_comp_first = 4;
    const int field_comp_last = (resultID.compare("VELOCITY") == 0) ? 6 : 4;
    cmdline << "spark-submit --master yarn --driver-memory 3g --executor-memory 3g --num-executors 1 "
	    << pathPython << " " << local_nodes_filename << " " << num_ref_levels << " " <<
      field_comp_first << " " << field_comp_last << " " << binary << " " << outputHdfsFile << " > " << logFile;
    // ./app --infile ../data/telescope_lr_vol_appr_input.txt --max_levels 24 --tolerance 0.5 --field_components 4 4 --binary 1
    LOGGER << "[AnalyticsModule::createVolumeLRSplineFromBoundingBox] -- invoking spark job as:\n";
    LOGGER << cmdline.str() << std::endl;
    int ret = system(cmdline.str().c_str());

    if (ret != 0)
      {
	LOGGER << "spark job return status is fail = " << ret << std::endl;
	std::stringstream ss;
	ss << "error executing system, ret = " << ret;
	ss << " ( is 'spark-submit' in the path?) ";
	// here we can read the output of the system and complete the error message
	*return_error_str = ss.str();
	return;
      }
    std::stringstream cmd_dfs;
    std::string localFileName("/tmp/");
    localFileName += outputHdfsFile;
    cmd_dfs << "hdfs dfs -copyToLocal " << outputHdfsFile << " " << localFileName;
    ret = system(cmd_dfs.str().c_str());
    if (ret != 0)
      {
	LOGGER << "hdfs dfs return status is fail = " << ret << std::endl;
	std::stringstream ss;
	ss << "error executing hdfs dfs -copyToLocal, ret = " << ret;
	*return_error_str = ss.str();
	return;
      }

    return_error_str->clear();
    //return_binary_mesh->clear();
    std::ifstream ifsLocal(localFileName, std::ios::binary);
    return_binary_volume_lrspline->assign(std::istreambuf_iterator<char>(ifsLocal),
					  std::istreambuf_iterator<char>());
    LOGGER << "return_binary_volume_lrspline.length() = " << return_binary_volume_lrspline->length() << std::endl;



  } else {
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": MISSING: We should perform the actual Volume LRSpline Approximation!");
    DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	  ": Returning a pre-computed binary file!");
    // We see if the we are on eddie.
    std::string precomputed_result_eddie("/local/VELaSSCo/SINTEF_test/telescope_speed_dump_31.bin");
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

  DEBUG( "Deleting output files ...");
  if ( use_yarn) {
    std::string cmd_line = HADOOP_HDFS + " dfs -rm -r " + spark_output_folder;
    DEBUG( cmd_line);
    int ret_cmd = system( cmd_line.c_str());
    if (ret_cmd == -1) {
      DEBUG("SINTEF: " << __FILE__ << ", line: " << __LINE__ <<
	    ": HADOOP_HDFS command failed!");
    }
    recursive_rmdir( local_tmp_folder.c_str());
  }

#if 0
  recursive_rmdir( spark_output_folder.c_str());
#endif

  return;
}
