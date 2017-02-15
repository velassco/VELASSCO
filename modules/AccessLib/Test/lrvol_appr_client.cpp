
// CLib
#include <cstdio>
#include <cstdlib>
#include <cstring>

// STL
#include <iostream>
#include <string>
#include <fstream>

//#include <boost/filesystem.hpp>

// VELaSSCo
#include "AccessLib.h"
#include "BoundaryBinaryMesh.h"

// from Helpers.h
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

  static std::string Hexdump(const std::string input, const size_t max_len = 80)
  {
    std::stringstream out;

    size_t end = input.size();
    if ( max_len && ( end > max_len)) {
      end = max_len;
    }
    size_t i = 0;
    for (i=0; i<end; i+=16)
      {
	out << std::hex << std::setw(2*sizeof(size_t)) << std::setfill('0') << (size_t)i << ": ";
	for (size_t j=0; j<16; j++) 
	  if (i+j < input.size())
	    out << std::setw(2) << (unsigned)(unsigned char)(input[i+j]) << " ";
	  else
	    out << "   ";

	out << " ";
	for (size_t j=0; j<16; j++) {
	  if (i+j < input.size()) {
	    if (isprint((unsigned char)input[i+j])) {
	      out << input[i+j];
	    } else {
	      out << '.';
	    }
	  }
	}

	out << std::endl;
      }

    if ( input.size() > end) {
      out << std::hex << std::setw(2*sizeof(size_t)) << std::setfill('0') << (size_t)( i + 16) << ": ";
      out << " . . ." << std::endl;
    }

    return out.str();
  }

  // returns NULL if dst_len is too short, otherwise return dst
  static const char *ToHexString( char *dst, size_t dst_len, const char *src, const size_t src_len) {
    if ( !dst) return NULL;
    if ( dst_len <= 0) return NULL;
    size_t isrc = 0;
    for ( size_t idst = 0; 
	  ( isrc < src_len) && ( idst < dst_len - 1); 
	  isrc++, idst += 2) {
      sprintf( &dst[ idst], "%02x", ( unsigned char)src[ isrc]);
    }
    // if all chars converted, then return dst
    return ( isrc == src_len) ? dst : NULL;
  }
  static bool ModelID_IsBinary( const std::string &modelID) {
    return ( modelID.length() == 16);
  }
  static std::string ModelID_DoHexStringConversionIfNecesary( const std::string &modelID, char *tmp_buf, size_t size_tmp_buf) {
    if ( modelID.length() == 16) {
      return ( std::string) ToHexString( tmp_buf, size_tmp_buf, modelID.c_str(), modelID.size());
    } else {
      return modelID;
    }
  }

void CheckVALResult(VAL_Result result, const std::string &error_message = "", const bool do_exit = true)
{
  if (result != VAL_SUCCESS)
  {
    const char* message;
    valErrorMessage(result, &message);

    std::cout << "VELaSSCo ERROR: " << std::endl;
    std::cout << "  " << message    << std::endl;
    if ( error_message.length() > 0) {
      std::cout << "  Query message: " << error_message    << std::endl;
    }
    if ( do_exit)
      exit(EXIT_FAILURE);
  }
}


#ifdef _WIN32
#ifndef strcasecmp
#define strcasecmp  _stricmp
#endif
#endif // _WIN32

bool askForHelp( const char *txt) {
  return !strcasecmp( txt, "-h") || !strcasecmp( txt, "--h") || !strcasecmp( txt, "-help") || !strcasecmp( txt, "--help");
}

std::string getStringFromCharPointers( const char *str1, const char *str2) {
  return std::string( str1 ? str1 : "") + std::string( str2 ? str2 : "");
}








int getVolumeLRApproximation(const VAL_SessionID sessionID,
			     int field_dim,
			     int num_steps,
			     double tolerance,
			     std::string filename_result)
{
  VAL_Result    result;
  const char *status = NULL;
  char hex_string[ 1024];

  std::cout << "=======================>>> SINTEF <<<=====================\n";
  std::cout << "sessionID: " << sessionID << std::endl;

  const char* model_tablename = "VELaSSCo_Models";
  const char* model_name = "fine_mesh-ascii_";
  const char* model_fullpath =
    "/exports/eddie3_apps_local/apps/community/VELaSSCo/simulation_files/Telescope_128subdomains_ascii";
  std::string model_unique_name = model_tablename;
  model_unique_name += ":";
  model_unique_name += model_fullpath;
  model_unique_name += ":";
  model_unique_name += model_name;

#if 0
  //
  // Test GetListOfModels()
  //
  const char *return_list = NULL;
  const char *group_qualifier = model_tablename;
  const char *name_pattern = model_name;
  result = valGetListOfModels( sessionID, group_qualifier, name_pattern, &status, &return_list);
  CheckVALResult(result, getStringFromCharPointers( "valGetListOfModels ", status));
  std::cout << "in VELaSSCo_models:" << std::endl;
  std::cout << "   status = " << status << std::endl;
  std::cout << "   model_list = " << return_list << std::endl;
#endif

  const char *access = "";
  const char *return_modelID = NULL;
  std::cout << "getVolumeLRApproximation(): doing OpenModel of " << model_unique_name << std::endl;
  result = valOpenModel( sessionID, model_unique_name.c_str(), access, &status, &return_modelID);
  CheckVALResult(result, getStringFromCharPointers( "valOpenModel ", status));
  std::cout << "OpenModel: " << std::endl;
  std::cout << "   status = " << ( status ? status : "(null)") << std::endl;
  if ( return_modelID) {
    std::cout << "return_modelID: " << return_modelID << std::endl;
    std::cout << "   model_modelID = " <<
      ModelID_DoHexStringConversionIfNecesary( return_modelID, hex_string, 1024) << std::endl;
  } else {
    // logout as it is not valid ...
    std::cout << "   ERROR model could not be opened, login out ..." << std::endl;
    return EXIT_SUCCESS;
  }
  std::string opened_modelID(return_modelID);
  std::string analysisID("Kratos");

  //
  // Test GetListOfTimeSteps
  //
  bool do_get_list_of_steps = true;
  double step_value = 0.0;//-1.0;
  if ( do_get_list_of_steps) {
    const double *return_list = NULL;
    size_t        return_num_steps = 0;
    const char *return_error_str = NULL;
    std::cout << "INFO: Doing valGetListOfTimeSteps for analysis = '" << analysisID << "'." << std::endl;
    result = valGetListOfTimeSteps( sessionID, opened_modelID.c_str(),
  				    analysisID.c_str(), 
  				    &return_error_str, 
  				    &return_num_steps, &return_list);
    // std::cout << "doTestSINTEF(): Done with valGetListOfTimeSteps, return_num_steps: " <<
    //   return_num_steps << "'." << std::endl;
    bool exit_on_error = false; // We use a dummy value if the list of time steps is not available.
    CheckVALResult(result, getStringFromCharPointers( "valGetListOfTimeSteps ", return_error_str), exit_on_error);
    ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
    std::cout << "doTestSINTEF(): GetListOfTimeSteps: " << opened_modelID << 
      ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if ( return_list) {
      std::cout << "INFO:   List_size = " << return_num_steps << std::endl;
      std::cout << "INFO:   Step_list = " <<
	Hexdump( std::string( ( char *)return_list, return_num_steps)) << std::endl;
      // std::cout << "   # steps   = " << return_num_steps << std::endl;
      // if ( return_num_steps > 0)
      // 	std::cout << "      Step 0 = " << return_list[ 0] << std::endl;
      // if ( return_num_steps > 1)
      // 	std::cout << "      Step 1 = " << return_list[ 1] << std::endl;
      // if ( return_num_steps > 2)
      // 	std::cout << "      Step 2 = " << return_list[ 2] << std::endl;
      // if ( return_num_steps > 3)
      // 	std::cout << "      Step 3 = " << return_list[ 3] << std::endl;
      //size_t mid_step = (size_t)(return_num_steps/2);
      //      size_t last_step = (size_t)(return_num_steps/2);
      //      std::cout << "mid_step: " << mid_step << std::endl;
#if 1
      std::cout << "Picked the last time step!" << std::endl;
      step_value = return_list[return_num_steps-1];//mid_step];
#else
      std::cout << "Picked the first time step!" << std::endl;
      step_value = return_list[0];//mid_step];
#endif
      std::cout << "INFO: step_value: " << step_value << std::endl;
    } else {
      std::cout << "INFO: return_list is NULL. Error: " << return_error_str << std::endl;
    }
  }

  const char* resultID = (field_dim == 1) ? "PRESSURE" : "VELOCITY"; // For the Telescope example.
  const double step_value_static_mesh = 0.0;
  std::string analysis_id_static_mesh("");
 
#if 0
  // We fetch the list of results for the analysisID.
  const char *return_list_results = NULL;
  result = valGetListOfResults(sessionID,
			       opened_modelID.c_str(),
			       analysisID.c_str(),
			       step_value,
			       &status,
			       &return_list_results);
  std::string list_of_results(return_list_results);
  std::cout << "SINTEF: list_of_results for analysisID: " << analysisID << ": " << list_of_results << std::endl;
  // We fetch a list of all meshes for the model.
  const char *return_list_meshes = NULL;
  result = valGetListOfMeshes(sessionID,
			      opened_modelID.c_str(),
			      analysis_id_static_mesh.c_str(),
			      step_value_static_mesh,
			      &status,
			      &return_list_meshes);
#endif

#if 0
  double bBox[6];
  bBox[0] = 0.0; // We initialize it with something illegal.
  bBox[1] = 0.0;
  bBox[2] = 0.0; 
  bBox[3] = -2.0;
  bBox[4] = -2.0;
  bBox[5] = -2.0; 
#endif

  // bool do_bbox = true;
  // if ( do_bbox) {
  const double *return_bbox = NULL;
  const char *return_error_str = NULL;
  std::vector<double> steps(1);
  steps[0] = step_value;
  // We want the bounding box for the static mesh.
  result = valGetBoundingBox( sessionID, opened_modelID.c_str(), // the already opened model
			      NULL, 0, // use all vertices ID
			      analysis_id_static_mesh.c_str(),//analysisID.c_str(),
			      "SINGLE",
			      &step_value_static_mesh,//&steps[0],
			      1,
			      &return_bbox, &return_error_str);
  CheckVALResult(result, getStringFromCharPointers( "valGetBoundingBox ", return_error_str));
  ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
  std::cout << "doTestSINTEF(): GetBoundingBox: " << opened_modelID << 
    ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
  if ( !return_bbox) {
    //   std::cout << "         bbox = ( " ;
    //   std::cout << return_bbox[ 0] << ", " << return_bbox[ 1] << ", " << return_bbox[ 2] << ") - ("
    // 		<< return_bbox[ 3] << ", " << return_bbox[ 4] << ", " << return_bbox[ 5] << ")." << std::endl;
    //   bBox[0] = return_bbox[0];
    //   bBox[1] = return_bbox[1];
    //   bBox[2] = return_bbox[2];
    //   bBox[3] = return_bbox[3];
    //   bBox[4] = return_bbox[4];
    //   bBox[5] = return_bbox[5];
    // } else {
    std::cout << "Error: " << return_error_str << std::endl;
    std::cout << "Failed extracting the bbox from the model!" << std::endl;
    return EXIT_FAILURE;
  }
  //  }

  // To demonstrate the query running the approximation we use the lowest step value as test case.
  // Hence we make sure that there isn't a result in HBase when calling the getVolumeLRSpline query.
  const char* meshID = "Kratos Tetrahedra3D4 Mesh";
  const char*    resultErrorStr = NULL;
  bool delete_results = (num_steps == 1);
  if (delete_results) {
    std::cout << "doTestSINTEF(): Calling valDeleteVolumeLRSplineFromBoundingBox()." << std::endl;
    result = valDeleteVolumeLRSplineFromBoundingBox( sessionID,
						     opened_modelID.c_str(), // The already opened model.
						     meshID, // "Kratos Tetrahedra3D4 Mesh" for the Telescope example.
						     resultID, // "VELOCITY" for the Telescope example.
						     step_value, // Using the last timestep.
						     analysisID.c_str(), // "Kratos" for the Telescope example.
						     return_bbox, // Of the static mesh (possibly a subset).
						     tolerance, // Positive value w/conv '< 0.0' => method chooses.
						     num_steps, // Legal values: {0,1,2, ...,8}
						     &resultErrorStr);
    std::cout << "doTestSINTEF(): Done calling valDeleteVolumeLRSplineFromBoundingBox()." << std::endl;
    std::cout << "doTestSINTEF(): result: " << result << std::endl;
    CheckVALResult(result, getStringFromCharPointers( "valDeleteVolumeLRSplineFromBoundingBox ", resultErrorStr));
  }

  // The result arguments.
  const char*    resultBinaryLRSpline = NULL;
  size_t resultBinaryLRSplineSize;
  const char*    resultStatistics = NULL;
  size_t resultStatisticsSize;
  std::cout << "doTestSINTEF(): Calling valComputeVolumeLRSplineFromBoundingBox()." << std::endl;
  result = valGetVolumeLRSplineFromBoundingBox( sessionID,
						opened_modelID.c_str(), // The already opened model.
						meshID, // "Kratos Tetrahedra3D4 Mesh" for the Telescope example.
						resultID, // "VELOCITY" for the Telescope example.
						step_value, // Using the last timestep.
						analysisID.c_str(), // "Kratos" for the Telescope example.
						return_bbox, // Of the static mesh (possibly a subset).
						tolerance, // Positive value (< 0.0 => method chooses).
						num_steps, // Range: {1, 2, ..., 9} (< 0 => method chooses).
						&resultBinaryLRSpline,
						&resultBinaryLRSplineSize,
						&resultStatistics,
						&resultStatisticsSize,

						&resultErrorStr);
  std::cout << "doTestSINTEF(): Done calling valComputeVolumeLRSplineFromBoundingBox()." << std::endl;
  std::cout << "doTestSINTEF(): result: " << result << std::endl;
  CheckVALResult(result, getStringFromCharPointers( "valComputeVolumeLRSplineFromBoundingBox ", resultErrorStr));
  ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
  std::cout << "doTestSINTEF(): ComputeVolumeLRSplineFromBoundingBox: " << opened_modelID << 
    ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
  if ( resultBinaryLRSpline) {
    std::cout << "doTestSINTEF(): Result binary volume lrspline # bytes: " << resultBinaryLRSplineSize << std::endl;
#ifndef _WIN32
    system("mkdir -p \"tmp\"");
#endif // _WIN32
    // std::string result_name;
    // result_name = "lrspline_approx_" + std::string(resultID) + "_" + std::to_string(num_steps) + ".bin";
    // std::string result_name_fullpath;
    // result_name_fullpath = result_name;
    std::ofstream binary_blob(filename_result,
			      std::ofstream::binary);
    std::cout << "doTestSINTEF(): Writing result to " << filename_result << std::endl;
    binary_blob.write(resultBinaryLRSpline, resultBinaryLRSplineSize);
  } else {
    std::cout << "doTestSINTEF(): Error: " << resultErrorStr << std::endl;
  }

  return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{

  if ( argc != 7) {
    std::cout << "Usage: hostname port field_dim level tolerance filename_result" << std::endl;
    return EXIT_FAILURE;
  }

  VAL_Result    result;
  VAL_SessionID sessionID;
  const char *status = NULL;

  std::string server(argv[1]);
  int port = atoi(argv[2]);
  int field_dim = atoi(argv[3]);
  int num_steps = atoi(argv[4]);
  double tolerance = atof(argv[5]);
  std::string filename_result(argv[6]);

  std::cout << "Input parameters: " << server << " " << port << " " << field_dim <<
    " " << num_steps << " " << tolerance << " " << filename_result << std::endl;

  // int remote_port = 9990; // default port
  // int velassco_default_port  = 26268; // Well, currently 26666.

  std::string hostname_port = server;
  hostname_port.append(":");
  hostname_port.append(std::to_string(port).c_str());
  hostname_port.append("?multiuser");

  // char hostname_port[ 1024];
  // sprintf( hostname_port, "%s:%s?multiuser", server, port);

  printf( "Connecting to '%s' ...\n", hostname_port.c_str());

  //// Test UserLogin()
  result = valUserLogin( hostname_port.c_str(), "andreas", "1234", &sessionID);
  CheckVALResult(result, "valUserLogin");

  //// Test StatusDB
  result = valGetStatusDB( sessionID, &status);
  CheckVALResult(result, getStringFromCharPointers( "valGetStatusDB ", status));
  std::cout << "status db = " << status << std::endl;

  // enable compression between QM --> AL
  // by default it's disabled so that old AL & plug-in's also work
  result = valSetConfiguration( sessionID, "CompressionEnabled", "1", &status);
  CheckVALResult(result);
  std::cout << "SetConfiguration: " << std::endl;
  std::cout << "   status = " << ( status ? status : "(null)") << std::endl;  

  // The function call uses the telescope model.
  // For dim=1 PRESSURE is used as input data, for dim=3 VELOCITY is used.
  int ret = getVolumeLRApproximation(sessionID,
				     field_dim,
				     num_steps,
				     tolerance,
				     filename_result);

  result = valUserLogout(sessionID);
  CheckVALResult(result);  

  return ret;
}
