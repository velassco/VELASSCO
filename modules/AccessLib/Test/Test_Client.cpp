
// CLib
#include <cstdio>
#include <cstdlib>
#include <cstring>

// STL
#include <iostream>
#include <string>

// VELaSSCo
#include "AccessLib.h"

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

void CheckVALResult(VAL_Result result, const std::string &error_message = "")
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

int doTestMorteza() {
  VAL_Result    result;
  VAL_SessionID sessionID;
  const char *status = NULL;
  char hex_string[ 1024];
  
  //
  // Test GetMeshDrawData() 
  //
  const char* fem_name 		= "VELaSSCo_HbaseBasicTest-part-" ; // "fine_mesh"; //"VELaSSCo_HbaseBasicTest";
  const char* fem_fullpath 	= "/home/jsperez/Sources/CIMNE/VELASSCO-Data/VELaSSCo_HbaseBasicTest_FEM";//"/localfs/home/velassco/common/simulation_files/Fem_small_examples/Telescope_128subdomains_ascii/";// "/localfs/home/velassco/common/simulation_files/VELaSSCo_HbaseBasicTest_FEM/";
  const char* fem_tablename = "VELaSSCo_Models_V4CIMNE";// "VELaSSCo_Models"; //
  
  //const char* fem_name 			=	"fine_mesh-ascii_";
  //const char* fem_fullpath    	=	"/home/jsperez/Sources/CIMNE/VELASSCO-Data/Telescope_128subdomains_ascii";
  //const char* fem_tablename  	= 	"VELaSSCo_Models_V4CIMNE";
  
  std::string fem_unique_name = fem_tablename;
  fem_unique_name += ":";
  fem_unique_name += fem_fullpath;
  fem_unique_name += ":";
  fem_unique_name += fem_name;
  
  const char *fem_access = "";
  const char *fem_return_modelID = NULL;
  result = valOpenModel( sessionID, fem_unique_name.c_str(), fem_access, &status, &fem_return_modelID);
  std::cout << "OpenModel: " << std::endl;
  std::cout << "   status = " << status << std::endl;
  char fem_hex_string[ 1024];
  if ( fem_return_modelID) {
    std::cout << "   FEM model_modelID = " << ModelID_DoHexStringConversionIfNecesary( fem_return_modelID, fem_hex_string, 1024) << std::endl;
  } else {
    // logout as it is not valid ...
    std::cout << "   ERROR FEM model could not be opened, login out ..." << std::endl;
    result = valUserLogout(sessionID);
    CheckVALResult(result);
    
    return EXIT_SUCCESS;
  }
  
  std::string fem_opened_modelID( fem_return_modelID );

  bool get_fem_analysisIDs = true;
  if(get_fem_analysisIDs){
    const char *fem_return_list = NULL;
    const char *fem_return_error_str = NULL;
    std::cout << "doing valGetListOfAnalyses" << std::endl;
    result = valGetListOfAnalyses( sessionID, fem_opened_modelID.c_str(), &fem_return_error_str, &fem_return_list);
    CheckVALResult(result, getStringFromCharPointers( "valGetListOfAnalyses ", fem_return_error_str));
    ModelID_DoHexStringConversionIfNecesary( fem_opened_modelID, fem_hex_string, 1024);
    std::cout << "GetListOfAnalyses: " << fem_opened_modelID << 
    ( ModelID_IsBinary( fem_opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if (fem_return_list) {
      std::cout << "   Analyses_list = " << fem_return_list << std::endl;
    } else {
      std::cout << "Error: " << fem_return_error_str << std::endl;
    }
  }
  
  const char* fem_analysisID = "Kratos";
  
  bool get_fem_timesteps = false;
  if(get_fem_timesteps){
    const double *fem_steps_return_list = NULL;
    size_t        fem_steps_return_num_steps = 0;
    const char   *fem_steps_return_error_str = NULL;
    std::cout << "doing valGetListOfTimeSteps" << std::endl;
    result = valGetListOfTimeSteps( sessionID, fem_opened_modelID.c_str(),
		fem_analysisID,
		&fem_steps_return_error_str, 
		&fem_steps_return_num_steps, &fem_steps_return_list);
    CheckVALResult(result, getStringFromCharPointers( "valGetListOfTimeSteps ", fem_steps_return_error_str));
    ModelID_DoHexStringConversionIfNecesary( fem_opened_modelID, hex_string, 1024);
    std::cout << "GetListOfTimeSteps: " << fem_opened_modelID << 
    ( ModelID_IsBinary( fem_opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if ( fem_steps_return_list) {
        std::cout << "   List_size = " << fem_steps_return_num_steps << std::endl;
        std::cout << "   Step_list = " << Hexdump( std::string( ( char *)fem_steps_return_list, fem_steps_return_num_steps)) << std::endl;
        bool show_all_time_steps = true;
        size_t show_n_steps = fem_steps_return_num_steps;
        if(!show_all_time_steps) show_n_steps = fem_steps_return_num_steps > 3 ? 3 : fem_steps_return_num_steps;
        for(size_t i = 0; i < show_n_steps; i++){
	  	  std::cout << "      Step " << i << " = " << fem_steps_return_list[ i] << std::endl;
	    }
    } else {
      std::cout << "Error: " << fem_steps_return_error_str << std::endl;
    }
  }
  std::cout << fem_opened_modelID << std::endl;
  double fem_time_step = 21.0;
  
  const char* list_of_meshes = NULL;
  result = valGetListOfMeshes( sessionID, fem_opened_modelID.c_str(), "", 0.0, &status, &list_of_meshes);
  //CheckVALResult(result, getStringFromCharPointers( "valGetListOfMeshes ", fem_steps_return_error_str));
  std::cout << "result: " << result << std::endl;
  std::cout << "status: " << status << std::endl;
  std::cout << list_of_meshes << std::endl;
  
  const VELaSSCo::RTFormat::File* fem_mesh_draw_data = NULL;
  result = valGetMeshDrawData( sessionID, fem_opened_modelID.c_str(), "", 0.0, 1, &status, &fem_mesh_draw_data );
  if(fem_mesh_draw_data)
  {
  }
  else
  {
  }

  return EXIT_SUCCESS;
}

int doTestMiguel() {
  VAL_Result    result;
  VAL_SessionID sessionID;
  const char *status = NULL;
  char hex_string[ 1024];

  // 
  // Test OpenModel
  //
  // unique_name is of the form ModelName or /Full/Path:ModelName or TableName:/Full/Path:ModelName
  // ModelName is Properties:fp
  // /Full/Path is Properties:nm
  // const char *unique_name = ""; // can be empty to get the first one
  // const char *unique_name = "*:*"; // can be empty to get the first one
  // const char *unique_name = "Test_VELaSSCo_Models:*:*"; // or using only the table's name and get the first one
  // const char *unique_name = "VELaSSCo_Models_V4CIMNE:/home/jsperez/Sources/CIMNE/VELASSCO-Data/Telescope_128subdomains_ascii:fine_mesh-ascii_";
  // const char *unique_name = "Test_VELaSSCo_Models:/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Large/:FluidizedBed_large";
  const char *unique_name = "VELaSSCo_Models:/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Large/:FluidizedBed_large";
  const char *access = "";
  const char *return_modelID = NULL;
  result = valOpenModel( sessionID, unique_name, access, &status, &return_modelID);
  std::cout << "OpenModel: " << std::endl;
  std::cout << "   status = " << status << std::endl;
  if ( return_modelID) {
    std::cout << "   model_modelID = " << ModelID_DoHexStringConversionIfNecesary( return_modelID, hex_string, 1024) << std::endl;
  } else {
    // logout as it is not valid ...
    std::cout << "   ERROR model could not be opened, login out ..." << std::endl;
    result = valUserLogout(sessionID);
    CheckVALResult(result);
    
    return EXIT_SUCCESS;
  }

  // need to store as return_modelID points to a temporary storage that will be reused in next query
  std::string opened_modelID( return_modelID);

  //
  // Test GetResultFromVerticesID()
  //

  const char*   modelID     = "d94ca29be534ca1ed578e90123b7"; // current DEM_box example in VELaSSCo_Models as of 10.11.2015, two days ago there where two models !
  const char*   resultID    = "MASS";
  const char*   analysisID  = "DEM";
  const int64_t vertexIDs[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
  const double  timeStep    = 10000.0;
  
  const int64_t* resultVertexIDs;
  const double*  resultValues;
  size_t         resultNumVertices;
  
  // This call does not comply with the VQuery form, but in the meantime ...
  result = valGetResultFromVerticesID(sessionID, modelID,
                                      resultID,
                                      analysisID,
                                      vertexIDs,
                                      timeStep,
                                      &resultVertexIDs,
                                      &resultValues,
                                      &resultNumVertices);
  CheckVALResult(result, "valGetResultFromVerticesID");
  
  //
  // Print received data
  //
  
  for (size_t i=0; i<resultNumVertices; i++)
    {
      std::cout << "Vertex: " << i;
      std::cout << "  ID: " << resultVertexIDs[i];
      std::cout << "  Values: [";
      for (size_t j=0; j<3; j++)
	std::cout << " " << resultValues[3*i+j];
      std::cout << " ]" << std::endl;
    }

  //
  // Test GetListOfAnalyses
  //
  bool do_get_list_of_analyses = true;
  if ( do_get_list_of_analyses) {
    const char *return_list = NULL;
    const char *return_error_str = NULL;
    std::cout << "doing valGetListOfAnalyses" << std::endl;
    result = valGetListOfAnalyses( sessionID, opened_modelID.c_str(),
				   &return_error_str, &return_list);
    CheckVALResult(result, getStringFromCharPointers( "valGetListOfAnalyses ", return_error_str));
    ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
    std::cout << "GetListOfAnalyses: " << opened_modelID << 
      ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if ( return_list) {
      std::cout << "   Analyses_list = " << return_list << std::endl;

      /* list is of the form: "Analysis name 1\nAnalysis name 2\n...\nAnalysis name N" */
      // select first analysis:
      char *sel_an = strdup( return_list);
      char *end = strchr( sel_an, '\n');
      if ( end) {
	*end = '\0';
      }
      analysisID = sel_an;
    } else {
      std::cout << "Error: " << return_error_str << std::endl;
    }
  }

  //
  // Test GetListOfTimeSteps
  //
  bool do_get_list_of_steps = true;
  double step_value = -1.0;
  if ( do_get_list_of_steps) {
    const double *return_list = NULL;
    size_t        return_num_steps = 0;
    const char *return_error_str = NULL;
    std::cout << "doing valGetListOfTimeSteps for analysis = '" << analysisID << "'." << std::endl;
    result = valGetListOfTimeSteps( sessionID, opened_modelID.c_str(),
				    analysisID, 
				    &return_error_str, 
				    &return_num_steps, &return_list);
    CheckVALResult(result, getStringFromCharPointers( "valGetListOfTimeSteps ", return_error_str));
    ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
    std::cout << "GetListOfTimeSteps: " << opened_modelID << 
      ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if ( return_list) {
      std::cout << "   List_size = " << return_num_steps << std::endl;
      std::cout << "   Step_list = " << Hexdump( std::string( ( char *)return_list, return_num_steps)) << std::endl;
      if ( return_num_steps > 0)
	std::cout << "      Step 0 = " << return_list[ 0] << std::endl;
      if ( return_num_steps > 1)
	std::cout << "      Step 1 = " << return_list[ 1] << std::endl;
      if ( return_num_steps > 2)
	std::cout << "      Step 2 = " << return_list[ 2] << std::endl;
      if ( return_num_steps > 0)
	step_value = return_list[ 0];
    } else {
      std::cout << "Error: " << return_error_str << std::endl;
    }
  }

  //
  // Test GetListOfResults
  //
  bool do_get_list_of_results = true;
  if ( do_get_list_of_results) {
    const char *return_list = NULL;
    const char *return_error_str = NULL;
    std::cout << "doing valGetListOfResults for analysis = '" << analysisID << "' and step = '" << step_value << "'." << std::endl;
    result = valGetListOfResults( sessionID, opened_modelID.c_str(),
				  analysisID, step_value,
				  &return_error_str, 
				  &return_list);
    CheckVALResult(result, getStringFromCharPointers( "valGetListOfResults ", return_error_str));
    ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
    std::cout << "GetListOfResults: " << opened_modelID << 
      ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if ( return_list) {
      std::cout << "   result_list = " << return_list << std::endl;
    } else {
      std::cout << "Error: " << return_error_str << std::endl;
    }
  }


  //
  // Test GetListOfMeshes
  //
  // analysisID = "geometry"; // for static meshes
  bool do_get_list_of_meshes = true;
  if ( do_get_list_of_meshes) {
    const char *return_list = NULL;
    const char *return_error_str = NULL;
    std::cout << "doing valGetListOfMeshes" << std::endl;
    const char *my_analysisID = ""; // for static meshes
    double my_stepValue = 0.0; // for static meshes, or 
    const char *mesh_type = "STATIC";
    if ( !strcasecmp( analysisID, "DEM")) {
      my_analysisID = analysisID; // looking for dynamic meshes
      my_stepValue = step_value; // looking for meshes in first step
      mesh_type = "DYNAMIC";
    }
    // double stepValue = 0.0; // for dynamic meshes
    result = valGetListOfMeshes( sessionID, opened_modelID.c_str(),
				 my_analysisID, my_stepValue,
				 &return_error_str, &return_list);
    CheckVALResult(result, getStringFromCharPointers( "valGetListOfMeshes ", return_error_str));
    ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
    std::cout << "GetListOfMeshes: " << opened_modelID << 
      ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if ( return_list) {
      std::cout << "   " << mesh_type << " meshes_list = " << std::endl;
      std::cout << return_list << std::endl;
    } else {
      std::cout << "Error: " << return_error_str << std::endl;
    }
  }

  //
  // Test GetBoundingBox()
  //

  bool do_bbox = false;
  if ( do_bbox) {
    const double *return_bbox = NULL;
    const char *return_error_str = NULL;
    std::cout << "doing valGetBoundingBox" << std::endl;
    
    result = valGetBoundingBox( sessionID, opened_modelID.c_str(), // the already opened model
				NULL, 0, // use all vertices ID
				"", // don't care about analysisID
				"ALL", NULL, 0, // use all steps / or don't care
				&return_bbox, &return_error_str);
    CheckVALResult(result, getStringFromCharPointers( "valGetBoundingBox ", return_error_str));
    ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
    std::cout << "GetBoundingBox: " << opened_modelID << 
      ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if ( return_bbox) {
      std::cout << "         bbox = ( " ;
      std::cout << return_bbox[ 0] << ", " << return_bbox[ 1] << ", " << return_bbox[ 2] << ") - ("
		<< return_bbox[ 3] << ", " << return_bbox[ 4] << ", " << return_bbox[ 5] << ")." << std::endl;
    } else {
      std::cout << "Error: " << return_error_str << std::endl;
    }
  }

  //
  // Test CloseModel() & UserLogout()
  //

  result = valCloseModel( sessionID, modelID, &status);
  CheckVALResult(result);
  std::cout << "CloseModel: " << std::endl;
  std::cout << "   status = " << status << std::endl;
  
  result = valUserLogout(sessionID);
  CheckVALResult(result);  

  return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
  VAL_Result    result;
  VAL_SessionID sessionID;
  const char *status = NULL;

  //
  // Test UserLogin()
  //

  char hostname_port[ 1024];
  int remote_port = 9990; // default port
  int velassco_default_port  = 26267;
  if ( argc == 2) {
    if ( askForHelp( argv[ 1])) {
      printf( "Usage: %s [ hostname [ port (default %d, VELaSSCo EngineLayer/QueryManager default port = %d)]]\n", 
	      argv[ 0], remote_port, velassco_default_port);
      return EXIT_FAILURE;
    }
    sprintf( hostname_port, "%s:%d", argv[ 1], remote_port);
  } else if ( argc == 3) {
    sprintf( hostname_port, "%s:%s", argv[ 1], argv[ 2]);
  } else {
    strcpy( hostname_port, "localhost:9990");
  }

  printf( "Connecting to '%s' ...\n", hostname_port);

  result = valUserLogin( hostname_port, "andreas", "1234", &sessionID);
  CheckVALResult(result, "valUserLogin");

  //// Test StatusDB

  result = valGetStatusDB( sessionID, &status);
  CheckVALResult(result, getStringFromCharPointers( "valGetStatusDB ", status));
  std::cout << "status = " << status << std::endl;

  //
  // Test GetListOfModels()
  //
  const char *return_list = NULL;
  const char *group_qualifier = ""; // loop over all available 4 tables
  const char *name_pattern = "*";
  result = valGetListOfModels( sessionID, group_qualifier, name_pattern, &status, &return_list);
  CheckVALResult(result, getStringFromCharPointers( "valGetListOfModels ", status));
  std::cout << "in VELaSSCo_models:" << std::endl;
  std::cout << "   status = " << status << std::endl;
  std::cout << "   model_list = " << return_list << std::endl;
  // group_qualifier = "Test_VELaSSCo_Models";
  // result = valGetListOfModels( sessionID, group_qualifier, name_pattern, &status, &return_list);
  // CheckVALResult(result);
  // std::cout << "in VELaSSCo_Models_V4CIMNE:" << std::endl;
  // std::cout << "   status = " << status << std::endl;
  // std::cout << "   model_list = " << return_list << std::endl;

  int ret = 0;
  ret = doTestMorteza();
  // ret = doTestMiguel();  

  return ret;
}
