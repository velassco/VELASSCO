
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







int doTestiCores( const VAL_SessionID sessionID) {
  VAL_Result    result;
  const char *status = NULL;
  
  std::cout << "\n\n SLEEP \n\n" << std::endl;
  //sleep(4);
  std::cout << "=======================>>> Ivan Cores <<<=====================\n";


  
/*  const char* model_name 		= "FluidizedBed_large";//"FluidizedBed_small";//"FluidizedBed_large";
  const char* model_fullpath 	= "/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Large/";//"/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Small/";// "/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Small/";//"/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Large/";
  const char* model_tablename   = "VELaSSCo_Models";//"VELaSSCo_Models";//"Test_VELaSSCo_Models";
  */
  
    
  const char* model_name 		= "fine_mesh-ascii_";//"FluidizedBed_small";//"FluidizedBed_large";
  const char* model_fullpath 	= "/localfs/home/velassco/common/simulation_files/Fem_small_examples/Telescope_128subdomains_ascii";//"/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Small/";// "/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Small/";//"/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Large/";
  const char* model_tablename   = "VELaSSCo_Models_V4CIMNE";//"VELaSSCo_Models";//"Test_VELaSSCo_Models";
  
  
  
  std::string model_unique_name = model_tablename;
  model_unique_name += ":";
  model_unique_name += model_fullpath;
  model_unique_name += ":";
  model_unique_name += model_name;
  
  //model_unique_name.erase(remove(model_unique_name.begin(), model_unique_name.end(), ' '), model_unique_name.end());
  
  const char *access = "";
  const char *return_modelID = NULL;
  result = valOpenModel( sessionID, model_unique_name.c_str(), access, &status, &return_modelID);
  CheckVALResult(result, getStringFromCharPointers( "valOpenModel ", status));
  std::cout << "OpenModel: " << std::endl;
  std::cout << "   status = " << ( status ? status : "(null)") << std::endl;
  if ( return_modelID) {
    std::cout << "   model_modelID = " << return_modelID << std::endl;
  } else {
    // logout as it is not valid ...
    std::cout << "   ERROR FEM model could not be opened, login out ..." << std::endl;
    return EXIT_SUCCESS;
  }
  
  const char* listOfMeshes = 0;
  std::string modelID( return_modelID );


  
if (modelID.size() == 0)
{
	std::cout << "   model_modelID *********ERROR           ******** =00 " << return_modelID << std::endl;
 	return EXIT_SUCCESS;
} 
else
{
	std::cout << "   model_modelID *********OK              ******** =00 " << return_modelID << std::endl;
}

 
/* 
  const char* analysisID = "";
  double      timeStep = 0.0;
  const char* meshID = "Kratos Tetrahedra3D4 Mesh";

				   
	const char     *resultMesh;
	size_t         resultMeshByteSize;
	const char     *resultErrorStr;			   
				result = valGetBoundaryOfAMesh(sessionID, return_modelID, meshID, analysisID, timeStep, &resultMesh, &resultMeshByteSize, &resultErrorStr);
	std::cout << "result:  " << result << std::endl;		
	
*/	
	
			   
 /*
 
  const char* analysisID = "DEM";
  double      timeStep = 2939000.0;
  
  result = valGetListOfMeshes( sessionID, modelID.c_str(), analysisID, timeStep, &status, &listOfMeshes ); 
  CheckVALResult(result, getStringFromCharPointers( "valOpenModel ", status));
  std::cout  << "GetListOfMeshes: "    << std::endl
             << "   status = " << status << std::endl;
  if(listOfMeshes) std::cout << "List Of Mesehes = \n" << listOfMeshes << std::endl;
  
  const char* meshID = "particles";
   
  const VELaSSCo::RTFormat::File* mesh_draw_data = NULL;
  result = valGetMeshDrawData( sessionID, modelID.c_str(), analysisID, timeStep, meshID, &status, &mesh_draw_data );
  std::cout << "status:  " << status;
  
  //const int64_t vertexIDs[] = {2724, 10, 0};
  std::vector<int64_t> vertexIDs;
  for(int64_t idx = 1; idx <= 11000; idx+=1)
    vertexIDs.push_back(idx);
  vertexIDs.push_back(0);
	  
  const int64_t  *resultVertexIDs;
  const double   *resultValues;
  size_t          resultNumVertices;
  result = valGetResultFromVerticesID( sessionID, modelID.c_str(), "Velocity", "DEM", vertexIDs.data(), 2939000.0, &status, &resultVertexIDs, &resultValues, &resultNumVertices);
  std::cout  << "valGetResultFromVerticesID: "    << std::endl
             << "   status = " << status << std::endl;
//  for(size_t i = 0; i < resultNumVertices; i++){
//	std::cout << resultVertexIDs[i] << "\t" << resultValues[3*i+0] << " " << resultValues[3*i+1] << " " << resultValues[3*i+2] << std::endl;
//  }
  */
  
  std::cout << "=======================>>> Ivan Cores <<<=====================\n";
  
  return EXIT_SUCCESS;
}





int doTestMorteza( const VAL_SessionID sessionID) {
  VAL_Result    result;
  const char *status = NULL;
  
  std::cout << "=======================>>> Morteza <<<=====================\n";
  
  // Name: FluidizedBed_small
  // FullPath: /localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Small/
  // ModelID: d0279880beacc38c32c5ae849074c01e
  // Location: Hbase:VELaSSCo_Models
  
  //
  // Test GetMeshDrawData() 
  //
  
  //Name: FluidizedBed_large
  //FullPath: /localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Large/
  //ModelID: 35c14b308febbaedd343e077d5e2c3ae
  //Location: Hbase:Test_VELaSSCo_Models
  
  const char* model_name 		= "FluidizedBed_large";//"FluidizedBed_small";//"FluidizedBed_large";
  const char* model_fullpath 	= "/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Large/";//"/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Small/";// "/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Small/";//"/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Large/";
  const char* model_tablename   = "VELaSSCo_Models";//"VELaSSCo_Models";//"Test_VELaSSCo_Models";
  
  //const char* fem_name 			=	"fine_mesh-ascii_";
  //const char* fem_fullpath    	=	"/home/jsperez/Sources/CIMNE/VELASSCO-Data/Telescope_128subdomains_ascii";
  //const char* fem_tablename  	= 	"VELaSSCo_Models_V4CIMNE";
  
  std::string model_unique_name = model_tablename;
  model_unique_name += ":";
  model_unique_name += model_fullpath;
  model_unique_name += ":";
  model_unique_name += model_name;
  
  //model_unique_name.erase(remove(model_unique_name.begin(), model_unique_name.end(), ' '), model_unique_name.end());
  
  const char *access = "";
  const char *return_modelID = NULL;
  result = valOpenModel( sessionID, model_unique_name.c_str(), access, &status, &return_modelID);
  CheckVALResult(result, getStringFromCharPointers( "valOpenModel ", status));
  std::cout << "OpenModel: " << std::endl;
  std::cout << "   status = " << ( status ? status : "(null)") << std::endl;
  if ( return_modelID) {
    std::cout << "   model_modelID = " << return_modelID << std::endl;
  } else {
    // logout as it is not valid ...
    std::cout << "   ERROR FEM model could not be opened, login out ..." << std::endl;
    return EXIT_SUCCESS;
  }
  
  const char* listOfMeshes = 0;
  std::string modelID( return_modelID );
  
  const char* analysisID = "DEM";
  double      timeStep = 2939000.0;
  
  result = valGetListOfMeshes( sessionID, modelID.c_str(), analysisID, timeStep, &status, &listOfMeshes ); 
  CheckVALResult(result, getStringFromCharPointers( "valOpenModel ", status));
  std::cout  << "GetListOfMeshes: "    << std::endl
             << "   status = " << status << std::endl;
  if(listOfMeshes) std::cout << "List Of Mesehes = \n" << listOfMeshes << std::endl;
  
  const char* meshID = "particles";
   
  const VELaSSCo::RTFormat::File* mesh_draw_data = NULL;
  result = valGetMeshDrawData( sessionID, modelID.c_str(), analysisID, timeStep, meshID, &status, &mesh_draw_data );
  std::cout << "status:  " << status;
  
  //const int64_t vertexIDs[] = {2724, 10, 0};
  std::vector<int64_t> vertexIDs;
  for(int64_t idx = 1; idx <= 11000; idx+=1)
    vertexIDs.push_back(idx);
  vertexIDs.push_back(0);
	  
  const int64_t  *resultVertexIDs;
  const double   *resultValues;
  size_t          resultNumVertices;
  result = valGetResultFromVerticesID( sessionID, modelID.c_str(), "Velocity", "DEM", vertexIDs.data(), 2939000.0, &status, &resultVertexIDs, &resultValues, &resultNumVertices);
  std::cout  << "valGetResultFromVerticesID: "    << std::endl
             << "   status = " << status << std::endl;
  for(size_t i = 0; i < resultNumVertices; i++){
	std::cout << resultVertexIDs[i] << "\t" << resultValues[3*i+0] << " " << resultValues[3*i+1] << " " << resultValues[3*i+2] << std::endl;
  }
  
  std::cout << "=======================>>> Morteza <<<=====================\n";
  
  return EXIT_SUCCESS;
}

std::string gen_random(const int len) {
 std::string s=*(new std::string(len,'a'));

static const char alphanum[]="012345678ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return s;
}

int doTestDC( const VAL_SessionID sessionID){
  srand(time(0));
  VAL_Result    result;
  // const char *status = NULL;
  // char hex_string[ 1024];

  std::cout << "=======================>>> Giuseppe <<<=====================\n";
  
  //Test Discrete To Continuum
  std::cout<<"Performing Discrete2Continuum Test"<<std::endl;

  std::string modelID = "3885c75d0d5c360515a64c0ac117a459";
  std::string analisysName = "analysisName_10";
  //concat random 5charstring at the end of the analysis name

  analisysName=analisysName+(gen_random(5) );
  std::cout<<"HEREEEE\n\n\n"<<gen_random(5)<<std::endl<<std::endl;

  std::string staticMeshID = "708f23d401f17c34ac3df5bff499032b";

  std::string stepOption = "INTERVAL"; //try all possible values = ALL SINGLE INTERVAL
  const double lstSteps[] = {2799000,4000000};
  const int numStep = 2;
  std::string coarseGrainedMethod = "Gaussian" ;
  const double w =0.0024;
  const double cutF =3;
  const bool procCont = true;
  const bool doAVG = true;

  std::string tempAVGOpts = "ALL";
  const double deltaT = 30000;

  const char* queryOutCome = NULL;
  const char* resultError = NULL;

  result = valGetDiscrete2Continuum(       sessionID,
					   modelID.c_str(),
					   analisysName.c_str(),
					   staticMeshID.c_str(),
					   stepOption.c_str(),
					   &lstSteps[0],
					   numStep,
					   coarseGrainedMethod.c_str(),
					   w,  cutF , procCont,
					   doAVG, tempAVGOpts.c_str(),
					   deltaT,
					   //out 
					   &queryOutCome, &resultError
					   );


  if (result == VAL_SUCCESS) {
    std::cout<<"SUCCESS: "<<queryOutCome<<std::endl;
  } else {
    std::cout<<"FAILED: "<<resultError<<std::endl;
  }

  std::cout << "=======================>>> Giuseppe <<<=====================\n";

  return 0;

}



// eol = end of line
bool is_eol( const char c) {
  return ( c == '\f') || ( c == '\n') || ( c == '\r');
}
char *streol( const char *s) {
  if ( !s) return NULL;
  const char *r = s;
  while ( *r && !is_eol( *r)) r++;
  return *r ? ( char *)r : NULL;
}

int doTestMiguel( const VAL_SessionID sessionID) {
  VAL_Result    result;
  const char *status = NULL;
  char hex_string[ 1024];

  std::cout << "=======================>>> Miguel <<<=====================\n";

  // const char *value = NULL;
  // const char *lst[] = { "All", "CompressionList", "CompressionType", "CompressionLevel", "CompressionThreshold", NULL};
  // for ( int idx = 0; lst[ idx]; idx++) {
  //   result = valGetConfiguration( sessionID, lst[ idx], &status, &value);
  //   CheckVALResult(result);
  //   std::cout << "GetConfiguration: " << std::endl;
  //   std::cout << "   status = " << ( status ? status : "(null)") << std::endl;  
  //   std::cout << "   value = " << ( value ? value : "(null)") << std::endl;  
  // }
  // 
  // strcpy( hex_string, "9");
  // result = valSetConfiguration( sessionID, "CompressionLevel", hex_string, &status);
  // CheckVALResult(result);
  // std::cout << "SetConfiguration: " << std::endl;
  // std::cout << "   status = " << ( status ? status : "(null)") << std::endl;  

    // return EXIT_SUCCESS;
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
  // const char *unique_name = "VELaSSCo_Models:/localfs/home/velassco/common/simulation_files/DEM_examples/Fluidized_Bed_Large/:FluidizedBed_large";
  // const char *unique_name = "VELaSSCo_Models:/localfs/home/velassco/common/simulation_files/VELaSSCo_HbaseBasicTest_FEM/:VELaSSCo_HbaseBasicTest";
  const char *unique_name = "VELaSSCo_Models_V4CIMNE:/localfs/home/velassco/common/simulation_files/VELaSSCo_HbaseBasic_FEM:VELaSSCo_HbaseBasicTestGP-part_";

  // // EDM models:
  // const char *unique_name = "VELaSSCo_HbaseBasicTest_part_1";
  const char *access = "";
  const char *return_modelID = NULL;
  std::cout << "doing OpenModel of " << unique_name << std::endl;
  result = valOpenModel( sessionID, unique_name, access, &status, &return_modelID);
  CheckVALResult(result, getStringFromCharPointers( "valOpenModel ", status));
  std::cout << "OpenModel: " << std::endl;
  std::cout << "   status = " << ( status ? status : "(null)") << std::endl;
  if ( return_modelID) {
    std::cout << "   model_modelID = " << ModelID_DoHexStringConversionIfNecesary( return_modelID, hex_string, 1024) << std::endl;
  } else {
    // logout as it is not valid ...
    std::cout << "   ERROR model could not be opened, login out ..." << std::endl;
    return EXIT_SUCCESS;
  }

  // need to store as return_modelID points to a temporary storage that will be reused in next query
  std::string opened_modelID( return_modelID);
  const char*   analysisID  = "";

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
      char *end = streol( sel_an);
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
      std::cout << "   # steps   = " << return_num_steps << std::endl;
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

  const char *res_name = NULL;
  int num_comp = 0;

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
    const bool do_exit = false;
    CheckVALResult(result, getStringFromCharPointers( "valGetListOfResults ", return_error_str), do_exit);
    ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
    std::cout << "GetListOfResults: " << opened_modelID << 
      ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if ( return_list) {
      std::cout << "   result_list = " << return_list << std::endl;
      const char *keyword = "Name:";
      char *found = strdup( strstr( return_list, keyword));
      if ( found) {
	found += strlen( keyword) + 1; // the space inbetween
	res_name = found;
	char *end = streol( res_name);
	if ( end) {
	  *end = '\0';
	}
	end++;
	keyword = "NumberOfComponents:";
	found = strstr( end, keyword);
	found += strlen( keyword) + 1; // the space inbetween
	num_comp = 1;
	if( found) {
	  int n = sscanf( found, "%d", &num_comp);
	  if ( n != 1) // there was some error...
	    num_comp = 1;
	}
      }
    } else {
      std::cout << "Error: " << return_error_str << std::endl;
    }
  }
  
  //
  // Test GetResultFromVerticesID()
  //
  bool do_GetResultFromVerticesID = true;
  if ( do_GetResultFromVerticesID && res_name && *res_name) {
    // list needs to end with '0'
    const int64_t vertexIDs[] = { 1, 2, 3, 0 };
    
    const int64_t* resultVertexIDs;
    const double*  resultValues;
    size_t         resultNumVertices;
    const char*    status;

    std::cout << "doing valGetResultFromVerticesID for " << std::endl;
    std::cout << "\tanalysis = '" << analysisID << "', step = '" << step_value << "' and result = '" << res_name << "'." << std::endl;
    
    // This call does not comply with the VQuery form, but in the meantime ...
    result = valGetResultFromVerticesID(sessionID, opened_modelID.c_str(),
					res_name,
					analysisID,
					vertexIDs, 
					step_value,
          &status,
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
	for (int j=0; j< num_comp; j++)
	  std::cout << " " << resultValues[num_comp*i+j];
	std::cout << " ]" << std::endl;
      }
  } else {
    if ( !res_name || !*res_name) {
      std::cout << "There was no Result Name selected" << std::endl;
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
    const bool do_exit = false;
    CheckVALResult(result, getStringFromCharPointers( "valGetListOfMeshes ", return_error_str), do_exit);
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
  
  // //
  // // Test GetBoundingBox()
  // //
  // 
  // bool do_bbox = false;
  // if ( do_bbox) {
  //   const double *return_bbox = NULL;
  //   const char *return_error_str = NULL;
  //   std::cout << "doing valGetBoundingBox" << std::endl;
  //   
  //   result = valGetBoundingBox( sessionID, opened_modelID.c_str(), // the already opened model
  // 				NULL, 0, // use all vertices ID
  // 				"", // don't care about analysisID
  // 				"ALL", NULL, 0, // use all steps / or don't care
  // 				&return_bbox, &return_error_str);
  //   CheckVALResult(result, getStringFromCharPointers( "valGetBoundingBox ", return_error_str));
  //   ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
  //   std::cout << "GetBoundingBox: " << opened_modelID << 
  //     ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
  //   if ( return_bbox) {
  //     std::cout << "         bbox = ( " ;
  //     std::cout << return_bbox[ 0] << ", " << return_bbox[ 1] << ", " << return_bbox[ 2] << ") - ("
  // 		<< return_bbox[ 3] << ", " << return_bbox[ 4] << ", " << return_bbox[ 5] << ")." << std::endl;
  //   } else {
  //     std::cout << "Error: " << return_error_str << std::endl;
  //   }
  // }

  //
  // Test GetBoundaryOfAMesh()
  //
  
  bool do_boundary = true;
  if ( do_boundary) {
    const char *return_mesh = NULL;
    size_t return_mesh_size = 0;
    const char *return_error_str = NULL;
    std::cout << "doing valGetBoundaryOfAMesh" << std::endl;
    
    result = valGetBoundaryOfAMesh( sessionID, opened_modelID.c_str(), // the already opened model
				    "Part", // Mesh ID ( name)
				    "", 0.0, // don't care about analysisID or stepValue
				    &return_mesh, &return_mesh_size, &return_error_str);
    CheckVALResult(result, getStringFromCharPointers( "valGetBoundaryOfAMesh ", return_error_str));
    ModelID_DoHexStringConversionIfNecesary( opened_modelID, hex_string, 1024);
    std::cout << "GetBoundaryOfAMesh: " << opened_modelID << 
      ( ModelID_IsBinary( opened_modelID) ? " ( binary)" : " ( ascii)") << std::endl;
    if ( return_mesh) {
      std::cout << "   boundary_mesh = ( " << return_mesh_size << " bytes)" << std::endl;
      std::cout << Hexdump( std::string( return_mesh, return_mesh_size), 128) << std::endl;
    } else {
      std::cout << "Error: " << return_error_str << std::endl;
    }
  }

  //
  // Test CloseModel() & UserLogout()
  //

  result = valCloseModel( sessionID, opened_modelID.c_str(), &status);
  CheckVALResult(result);
  std::cout << "CloseModel: " << std::endl;
  std::cout << "   status = " << ( status ? status : "(null)") << std::endl;
  
  std::cout << "=======================>>> Miguel <<<=====================\n";

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
  int velassco_default_port  = 26268;
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
  result = valGetListOfModels( sessionID, group_qualifier, name_pattern, &status, &return_list);
  CheckVALResult(result);
  // std::cout << "in VELaSSCo_Models_V4CIMNE:" << std::endl;
  std::cout << "   status = " << status << std::endl;
  std::cout << "   model_list = " << return_list << std::endl;

  int ret = 0;

  // enable compression between QM --> AL
  // by default it's disabled so that old AL & plug-in's also work
  result = valSetConfiguration( sessionID, "CompressionEnabled", "1", &status);
  CheckVALResult(result);
  std::cout << "SetConfiguration: " << std::endl;
  std::cout << "   status = " << ( status ? status : "(null)") << std::endl;  

  ret = doTestiCores(sessionID);
  //ret = doTestMorteza( sessionID);
  //ret = doTestMiguel( sessionID); 
  //ret= doTestDC (sessionID);

  // result = valStopVELaSSCo( sessionID, &status);
  // CheckVALResult(result);  

  result = valUserLogout(sessionID);
  CheckVALResult(result);  

  return ret;
}
