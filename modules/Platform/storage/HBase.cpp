
// CLib
#include <cmath>
#include <cstddef>

// STD
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

// Curl
#include <curl/curl.h>

// VELaSSCo
#include "HBase.h"

#include "cJSON.h"
#include "base64.h"
#include "Helpers.h"

using namespace std;
using namespace VELaSSCo;

/////////////////////////////////
// Curl + Json
/////////////////////////////////

#include "cJSON.h"

#include "Curl_cmd.h"

using namespace VELaSSCo;

bool HBase::startConnection( const char *DB_hostname, const int DB_port) {
  _socket = new boost::shared_ptr<TTransport>( new TSocket( DB_hostname, DB_port));
  _transport = new boost::shared_ptr<TTransport>( new TBufferedTransport( *_socket));
  _protocol = new boost::shared_ptr<TProtocol>( new TBinaryProtocol( *_transport));
  _hbase_client = new HbaseClient( *_protocol);
  _db_host = DB_hostname;
  bool connected = true;
  try {
    ( *_transport)->open();
    LOGGER_SM << "Connected to HBase on " << DB_hostname << ":" << DB_port << endl;
  } catch ( TException &tx) {
    LOGGER_SM << "Opening HBase connection error: " << tx.what() << endl;
    connected = false;
  }
  return connected;
}

bool HBase::stopConnection() {
  bool disconnected = true;
  try {
    if ( _transport) { // needed when doing 'quit' on the QueryManager prompt
      ( *_transport)->close();
    }
    LOGGER_SM << "Disconnected from HBase server" << endl;
  } catch ( TException &tx) {
    LOGGER_SM << "Closing HBase connection error: " << tx.what() << endl;
    disconnected = false;
  }
  delete _hbase_client;
  delete _protocol;
  delete _transport;
  delete _socket;
  _hbase_client = NULL;
  _protocol = NULL;
  _transport = NULL;
  _socket = NULL;
  return disconnected;
}

std::string HBase::parse1DEM(string b, std::string LOVertices)
{
	std::stringstream result;
	int64_t           vertexID = 0;

    printf("Parsing JSON tree ...\n");
    cJSON *json = cJSON_Parse(b.c_str());
    if ( !json) {
      printf("not a JSON tree !\n");
      return "";
    }
	
    // cout<< "List Of vertices : "<< LOVertices <<endl<<endl;
    cJSON *sentListOfVertices = cJSON_GetObjectItem(cJSON_Parse(LOVertices.c_str()), "id");
    
    cJSON *_return = cJSON_CreateObject();
    cJSON *listOfVertices = cJSON_CreateArray();
    cJSON_AddItemToObject(_return, "vertices", listOfVertices);
    
    vector<cJSON*> velem;
    vector<bool> enableVElem;
    for (int i = 0; i < cJSON_GetArraySize(json); i ++ )
    {
        //Get each row element
        cJSON *row = cJSON_GetArrayItem (json, i);
        //Read all elements of each row
        
        for (int j = 0; j < cJSON_GetArraySize(row); j ++ )
        {
            //A Hbase key  is composed by two elements: key and cell (which contains data)
            cJSON *keyC = cJSON_GetArrayItem (row, j);
            
            
            cJSON *keyJ = cJSON_GetArrayItem (keyC, 0);
            //Row key
            string key = base64_decode(keyJ->valuestring);
			// cout << "key : "<< key << endl; 

            cJSON *cellsJ = cJSON_GetArrayItem (keyC, 1);
            // cJSON * elem  = cJSON_CreateObject();
            
            for (int k = 0; k < cJSON_GetArraySize(cellsJ); k++ )
            {
                cJSON *val = cJSON_GetArrayItem (cellsJ, k);
                
                cJSON *elem = cJSON_GetArrayItem (val, 0);
                string tmp = base64_decode(elem->valuestring);
                // cout << "\t" << tmp <<endl;
                if(tmp.find("M:cid") == 0)
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int id = 0;
                    for (size_t n = 0; n < tmp.size() ; n ++)
                    {
                        if ((int)tmp[n] == 44 || (int)tmp[n] == 125 )
                        {
                            //Create a new elem
                            velem.push_back(cJSON_CreateObject());
                            cJSON_AddItemToObject( velem.back() ,"id" ,cJSON_CreateNumber(id));
                            int o = 0;
                            
                            
                            while (o < cJSON_GetArraySize(sentListOfVertices) && cJSON_GetArrayItem(sentListOfVertices, o)->valueint != id)
                            {
                                o++;
                            }
                            if(o == cJSON_GetArraySize(sentListOfVertices))
                            {
                                enableVElem.push_back(false);
                            }
                            else
                            {
                                enableVElem.push_back(true);
                            }
                            id = 0;
                        }
                        else if ((int)tmp[n] >= 48 && (int)tmp[n] <= 57 )
                        {
                            id = (id * 10) + ((int)tmp[n] - 48);
                        }
                    }
                    //              cout << tmp<<" M:cid :" << base64_decode(elem->valuestring) <<" "<< endl;
                    
                    
                }
                else if(tmp.find("M:cxyz") == 0)
                {
                    
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int _id = 0;
                    size_t id = 0;
                    int N = 3;
                    double xyz[N];
                    for(int b = 0; b < N; b++)
                        xyz[b] = 0.0;
                    int coma = 0;
                    int sign = 1;
                    for (size_t n = 0; n < tmp.size() ; n ++)
                    {
                        if ((int)tmp[n] == 44 || (int)tmp[n] == 125 )
                        {
                            //Create a new elem
                            //
                            xyz[_id] = sign * xyz[_id] ;
                            coma = 0;
                            sign = 1;
                            _id++;
                            if(_id == N)
                            {
                                if (id < velem.size())
                                {
                                    cJSON_AddItemToObject( velem.at(id) ,"coord", cJSON_CreateDoubleArray(xyz, N));
                                }
                                                                cout << xyz[0] << " " <<  xyz[1] << " " <<  xyz[2] << " "<< endl;
                                id++;
                                _id= 0;
                                for(int b = 0; b < N; b++)
                                    xyz[b] = 0.0;
                            }
                        }
                        else if ((int)tmp[n] >= 48 && (int)tmp[n] <= 57 )
                        {
                            if(coma < 0)
                            {
                                double val = ((int)tmp[n] - 48) * pow(10, coma);
                                xyz[_id] = xyz[_id] + val;
                                coma --;
                            }
                            else
                            {
                                xyz[_id] = (xyz[_id] * 10) + ((int)tmp[n] - 48);
                            }
                        }
                        else if ((int)tmp[n] == 45)
                        {
                            sign = -1;
                        }
                        else if ((int)tmp[n] == 46)
                        {
                            coma = -1 ;
                        }
                    }
                    //  cout << tmp<<" M:cxyz :"<< base64_decode(elem->valuestring)<< endl;
                    
                }
                else if(tmp.find("M:mcn") == 0)
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int val = 0;
                    size_t id = 0;
                    for (size_t n = 0; n < tmp.size() ; n ++)
                    {
                        if ((int)tmp[n] == 44 || (int)tmp[n] == 125 )
                        {
                            if (id < velem.size())
                            {
                                cJSON_AddItemToObject( velem.at(id) ,"mcn" ,cJSON_CreateNumber(val));
                            }
                            id++;
                            val = 0;
                        }
                        else if ((int)tmp[n] >= 48 && (int)tmp[n] <= 57 )
                        {
                            val = (val * 10) + ((int)tmp[n] - 48);
                        }
                    }
                    //cout << tmp<<" M:mcn :"<< base64_decode(elem->valuestring)<< endl;
                    
                }
                else if(tmp.find("M:mgp") == 0)
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int val = 0;
                    size_t id = 0;
                    for (size_t n = 0; n < tmp.size() ; n ++)
                    {
                        if ((int)tmp[n] == 44 || (int)tmp[n] == 125 )
                        {
                            if (id < velem.size())
                            {
                                cJSON_AddItemToObject( velem.at(id) ,"mgp" ,cJSON_CreateNumber(val));
                            }
                            id++;
                            val = 0;
                        }
                        else if ((int)tmp[n] >= 48 && (int)tmp[n] <= 57 )
                        {
                            val = (val * 10) + ((int)tmp[n] - 48);
                        }
                    }
                    //                 cout << tmp<<" M:mgp :"<< base64_decode(elem->valuestring)<< endl;
                    
                }
                else if(tmp.find("M:mid") == 0)
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int val = 0;
                    size_t id = 0;
                    for (size_t n = 0; n < tmp.size() ; n ++)
                    {
                        if ((int)tmp[n] == 44 || (int)tmp[n] == 125 )
                        {
                            if (id < velem.size())
                            {
                                cJSON_AddItemToObject( velem.at(id) ,"mid" ,cJSON_CreateNumber(val));
                            }
                            id++;
                            val = 0;
                        }
                        else if ((int)tmp[n] >= 48 && (int)tmp[n] <= 57 )
                        {
                            val = (val * 10) + ((int)tmp[n] - 48);
                        }
                    }
                    
                    // cout << tmp<<" M:mid :"<< base64_decode(elem->valuestring)<< endl;
                    
                }
                else if(tmp.find("M:mrd") == 0)
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int _id = 0;
                    size_t id = 0;
                    int N = 1;
                    double xyz[N] ;
                    for(int b = 0; b < N; b++)
                        xyz[b] = 0.0;
                    int coma = 0;
                    int sign = 1;
                    for (size_t n = 0; n < tmp.size() ; n ++)
                    {
                        if ((int)tmp[n] == 44 || (int)tmp[n] == 125 )
                        {
                            //Create a new elem
                            //
                            xyz[_id] = sign * xyz[_id] ;
                            coma = 0;
                            sign = 1;
                            _id++;
                            if(_id == N)
                            {
                                if (id < velem.size())
                                {
                                    cJSON_AddItemToObject( velem.at(id) ,"mrd", cJSON_CreateDoubleArray(xyz, N));
                                }
                                //                                cout << xyz[0] << " " <<  xyz[1] << " " <<  xyz[2] << " "<< endl;
                                id++;
                                _id= 0;
                                for(int b = 0; b < N; b++)
                                    xyz[b] = 0.0;
                            }
                        }
                        else if ((int)tmp[n] >= 48 && (int)tmp[n] <= 57 )
                        {
                            if(coma < 0)
                            {
                                double val = ((int)tmp[n] - 48) * pow(10, coma);
                                xyz[_id] = xyz[_id] + val;
                                coma --;
                            }
                            else
                            {
                                xyz[_id] = (xyz[_id] * 10) + ((int)tmp[n] - 48);
                            }
                        }
                        else if ((int)tmp[n] == 45)
                        {
                            sign = -1;
                        }
                        else if ((int)tmp[n] == 46)
                        {
                            coma = -1 ;
                        }
                    }
                    
                    //cout << tmp<<" M:mrd :"<< base64_decode(elem->valuestring)<< endl;
                    
                }
                else if(tmp.find("R:rid") == 0)
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int val = 0;
                    size_t id = 0;
                    for (size_t n = 0; n < tmp.size() ; n ++)
                    {
                        if ((int)tmp[n] == 44 || (int)tmp[n] == 125 )
                        {
                            if (id < velem.size())
                            {
                                cJSON_AddItemToObject( velem.at(id) ,"rid" ,cJSON_CreateNumber(val));
                            }
                            id++;
                            val = 0;
                        }
                        else if ((int)tmp[n] >= 48 && (int)tmp[n] <= 57 )
                        {
                            val = (val * 10) + ((int)tmp[n] - 48);
                        }
                    }
                    // cout << tmp<<" R:rid :"<< base64_decode(elem->valuestring)<< endl;
                    
                }
                else if(tmp.find("R:rnr") == 0)
                {
                    //                 elem = cJSON_GetArrayItem (val, 2);
                    //               tmp = base64_decode(elem->valuestring);
                    // cout << tmp<<" R:rnr :"<< base64_decode(elem->valuestring)<< endl;
                    
                }
                else if(tmp.find("R:rvl") == 0)
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int val = 0;
                    size_t id = 0;
                    for (size_t n = 0; n < tmp.size() ; n ++)
                    {
                        if ((int)tmp[n] == 44 || (int)tmp[n] == 125 )
                        {
                            if (id < velem.size())
                            {
                                cJSON_AddItemToObject( velem.at(id) ,"rvl" ,cJSON_CreateNumber(val));
                            }
                            id++;
                            val = 0;
                        }
                        else if ((int)tmp[n] >= 48 && (int)tmp[n] <= 57 )
                        {
                            val = (val * 10) + ((int)tmp[n] - 48);
                        }
                    }
                    // cout << tmp <<" R:rvl :"<< base64_decode(elem->valuestring)<< endl;
                    
                }
                else
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    //tmp = base64_decode(elem->valuestring);
                    //cout << tmp <<" Error "<< base64_decode(elem->valuestring)<< endl;
                    
                    // cout << tmp << " " << base64_decode(elem->valuestring)<< endl;                    
                    
		    if (tmp.find("M:c") == 0) {
		      // result << vertexID++ << " " << base64_decode(elem->valuestring) << endl;
		      std::string value = base64_decode(elem->valuestring);
		      // cout << tmp << " " << value.size() << " " << value << " " << endl << Hexdump(value);
                      
		      uint64_t v[3];
		      double vertex[3];
                      
		      v[0] = *((uint64_t*)(&(value[ 0])));
		      v[0] = __builtin_bswap64(v[0]);
		      vertex[0] = *((double*)(&v[0]));
		      v[1] = *((uint64_t*)(&(value[ 8])));
		      v[1] = __builtin_bswap64(v[1]);
		      vertex[1] = *((double*)(&v[1]));
		      v[2] = *((uint64_t*)(&(value[16])));
		      v[2] = __builtin_bswap64(v[2]);
		      vertex[2] = *((double*)(&v[2]));
                       
		      if (vertexID >= 3000)
			{
			  result << vertexID << " " << vertex[0] << " " << vertex[1] << " " << vertex[2] << "  0" << endl;
			  printf("\rReading vertex : %lld ...", ( long long int)( vertexID-3000));
			}
		      if (vertexID >= 4000)
			{
			  printf("\n");
			  break;
			}
			
		      vertexID++;
		    }
                }
                
                //cout << cJSON_Print(value) << endl;
                //cout << base64_decode(cJSON_GetArrayItem(value, 0)->valuestring) << endl;
                //cout <<"------------"<<endl;
            }
        }
        // exit(0);
        break;
    }
    for (size_t k = 0; k < velem.size(); k++)
    {
        if (enableVElem.at(k))
            cJSON_AddItemToArray( listOfVertices, velem.at(k));
    }

    //return cJSON_Print(_return);
    return result.str();
}

std::string HBase::getResultFromVerticesID( std::string& report, std::vector<ResultOnVertex> &listOfResults,
					 const std::string &sessionID, const std::string &modelID,
    			     const std::string &analysisID, const double       timeStep,  
				     const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID )
{
  //return getResultOnVertices_curl( sessionID, modelID, analysisID, timeStep, resultID, listOfVertices );
  return getResultFromVerticesID_thrift( report, listOfResults, sessionID, modelID, analysisID, timeStep, resultID, listOfVerticesID );
  //return getResultFromVerticesID_thrift_filter( report, listOfResults, sessionID, modelID, analysisID, timeStep, resultID, listOfVerticesID );
}

std::string HBase::getResultFromVerticesID_curl( const std::string &sessionID,
					     const std::string &modelID,
					     const std::string &analysisID,
					     const double       timeStep,
					     const std::string &resultID,
					     const std::string &listOfVertices )
{
  LOGGER_SM << "getResultOnVertices CURL: =====" << std::endl;
  LOGGER_SM << "S " << sessionID      << std::endl;
  LOGGER_SM << "M " << modelID        << std::endl;
  LOGGER_SM << "R " << resultID       << std::endl;
  LOGGER_SM << "A " << analysisID     << std::endl;
  LOGGER_SM << "V " << listOfVertices << std::endl;
  LOGGER_SM << "T " << timeStep       << std::endl;

  string cmd = "http://" + _db_host + ":8880/";
  // cmd += "Simulations_Data";
  // cmd += "Test_VELaSSCo_Models";
  cmd += "T_Simulations_Data";
  cmd += "/";
  std::stringstream key;
  //key << "0x";
  //key << modelID;
  //key << analysisID;
  //key << timeStep;
  //key << resultID;

  //key << "643934636132396265353334636131656435373865393031323362376338636544454d383030303031/M";
  //key << "&*"; // first row of Simulations_Data ingested by ATOS start with a 4, avoiding asking for ALL the table !

  // key << "D*";
  // key << "scanner/14448375409011a4cbd8";
	
	unsigned partitionID = 1;
	
	std::string simulationID_hex       = "4d0e0c37be088e715de50d2230887579";
	std::string analysisNameLength_hex = toHexString(byteSwap((uint32_t)analysisID.size()));
	std::string analysisNameChars      = analysisID;
	std::string timeStep_hex           = toHexString(byteSwap((double)timeStep));
	std::string partitionID_hex        = toHexString(byteSwap((uint32_t)partitionID));

	//key << simulationID_hex
		//<< analysisNameLength_hex
		//<< analysisNameChars
		//<< timeStep_hex
		//<< partitionID_hex;

  key << createDataRowKey(modelID, analysisID, timeStep, partitionID);

  cmd += key.str();
  cout << cmd << endl;

    
  CurlCommand do_curl;
  string buffer;
    
  bool ok = do_curl.Evaluate( buffer, cmd);

  LOGGER_SM << "**********\n";    
  LOGGER_SM << buffer << std::endl;
  LOGGER_SM << "**********\n";    
    
  //
  string result( "");
  if ( ok) {
    if(analysisID.find("DEM") >= 0)
      {
	// buffer should be checked against "Not found"
	std::size_t found = buffer.find( "Not found");
	if ( ( found == string::npos) || ( found > 10)) {
	  // assume that the error message "Not found" should be at the begining of the returned buffer
	  result = parse1DEM(buffer, listOfVertices);
	}
	if ( result == "") { // there has been some errors parsing the JSON tree or is "Not found"
	  result = buffer;
	}
      }
    else if(analysisID.find("FEM") >= 0)
      {
	cout << buffer << endl;
      }
  }  else {
    // !ok
    result = buffer;
  }
  return result;
}

// ==============================================
// ==== Thrift + Filters
// ==============================================
//#define READ_GROUPED_ROWS_FILTERS

// #define USE_HASHING_FILTER

/* GetResultsFromVerticesID */
static bool getResultsFromRow_filter( std::vector< ResultOnVertex > &listOfResults, const TRowResult &rowResult, const ResultInfo &resultInfo, const std::vector<int64_t> &listOfVerticesID, const int64_t &minVertexID, const int64_t &maxVertexID ) {
  int num_results = 0;
  unordered_map< int64_t, std::vector< double > > resultOnVertexListMap;
  for ( CellMap::const_iterator it = rowResult.columns.begin(); 
	it != rowResult.columns.end(); ++it) {
    const char *cq_str = it->first.c_str();
    //LOGGER_SM << cq_str << it->second.value.data() << std::endl;
    const char CF = cq_str[ 0];
    // cq_str[ 1] should be ':'
    const char subC = cq_str[ 2];
    if ( CF == 'R') {
      if ( it->first.length() > 3) { // for c1xxxx
      	int result_number = 0;
      	int n = sscanf( &cq_str[ 3], "%d", &result_number);
      	if ( n == 1 && resultInfo.resultNumber == result_number ) {
      	  // in fact not needed as it should always be meshID ( == coordsID)
      	  const char *pinfo = &cq_str[ 3];
      	  while ( *pinfo && isdigit( *pinfo))
      	    pinfo++;
      	  if ( subC == 'r') { // right qualifier, coordinates
      	    // coordinates are of the form rXXXXXXvl_ID, where ID is binary indes ...
      	    int64_t node_id;
      	    std::vector<double> result_values;
      	    if ( *pinfo == '_') pinfo++; // some data only is r000001_nodeId // to be removed soon
			else pinfo+= 3; // the 'vl_'
      	    node_id = byteSwap< int64_t>( *( int64_t *)pinfo );
      	    double *coords = ( double *)it->second.value.data();
            for(int i = 0; i < resultInfo.numberOfComponents; i++)
      	      result_values.push_back( byteSwap< double>( coords[ i ] ) );
      	      
      	    //LOGGER_SM << node_id << " " << coords[0] << " " << coords[1] << " " << coords[2] << std::endl;
#ifdef USE_HASHING_FILTER
#if __cplusplus < 201103L
	    // c++ < c++11
      	    resultOnVertexListMap.insert (std::make_pair< int64_t, std::vector< double > >(node_id, result_values));  
#else
	    // c++ <= c++98
      	    resultOnVertexListMap.insert (std::pair< int64_t, std::vector< double > >(node_id, result_values));  
#endif
#else
			ResultOnVertex result;
			result.__set_id( node_id );
			result.__set_value( result_values );
			listOfResults.push_back( result );
			num_results++;	  
#endif
      	  }
      	}
      }
    }
  }
 
#ifdef USE_HASHING_FILTER
  //LOGGER_SM << "====> before unordered" << std::endl; 
  
      for(size_t i = 0; i < listOfVerticesID.size(); i++){
		int64_t vertexID = listOfVerticesID[i];
		std::unordered_map< int64_t,std::vector< double > >::const_iterator resultOnVertex = resultOnVertexListMap.find (vertexID);

		if ( resultOnVertex == resultOnVertexListMap.end() ){
			//LOGGER_SM << "not found";
			//continue;
		} else {
		    //LOGGER_SM << "found";
			ResultOnVertex result;
			result.__set_id( resultOnVertex->first );
			result.__set_value( resultOnVertex->second );
			listOfResults.push_back( result );
			num_results++;	  
		}
		
		LOGGER_SM << num_results << std::endl;
	}
#endif
  
  LOGGER_SM << "Number of Results = " << num_results << std::endl; 
  
  return num_results;
}

bool HBase::getResultFromVerticesIDFromTables_filter( std::string& report, std::vector<ResultOnVertex> &listOfResults, const std::string& data_table,
           const std::string &sessionID,  const std::string &modelID,
           const std::string &analysisID, const double       timeStep,  
           const ResultInfo &resultInfo, const std::vector<int64_t> &listOfVerticesID, const int64_t &minVertexID, const int64_t &maxVertexID, const char *format ){
  bool scan_ok = true;  
#ifdef READ_GROUPED_ROWS_FILTERS
  int chunk_size = 30;
  for(int rowIdx = 0; rowIdx <= 128; rowIdx+=chunk_size){
#endif
  
  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  
  char resultNum_str[10];
  sprintf(resultNum_str, "%06d", resultInfo.resultNumber);
  
  int64_t minNodeNum = minVertexID;
  int64_t maxNodeNum = maxVertexID;
  LOGGER_SM << "filter = [" << minNodeNum << " , " << maxNodeNum << "]\n";
  minNodeNum = byteSwap(minNodeNum);  
  maxNodeNum = byteSwap(maxNodeNum);  
  
  double my_stepValue = ( analysisID == "") ? 0.0 : timeStep; // rowkeys for static meshes have stepValue == 0.0
  std::string prefixRowKey = createMetaRowKey( modelID, analysisID, my_stepValue, format);
#ifdef READ_GROUPED_ROWS_FILTERS
  std::string startRowKey = createDataRowKey( modelID, analysisID, my_stepValue, rowIdx, format); // partitionID = 0
  std::string stopRowKey = createDataRowKey( modelID, analysisID, my_stepValue, rowIdx+chunk_size-1, format); // partitionID = biggest ever
#else
  std::string startRowKey = createDataRowKey( modelID, analysisID, my_stepValue, 0, format); // partitionID = 0
  std::string stopRowKey = createDataRowKey( modelID, analysisID, my_stepValue,( int)0x7fffffff, format); // partitionID = biggest ever
#endif
  const size_t len_prefix_rowkey = prefixRowKey.length();
  
  TScan ts;
  //LOGGER_SM << "Starting scanner with scan..." << std::endl;
  std::stringstream filter;
  filter << "(";
  filter << "(QualifierFilter (=, 'regexstring:R*')) AND ";
  filter << "(RowFilter (>=, 'binary:";
  filter << startRowKey;
  filter << "')) AND (RowFilter (<=, 'binary:";
  filter << stopRowKey;
  filter << "')) AND ";
  filter << "ColumnRangeFilter ('r" << resultNum_str << "vl_";
  filter.write((const char*)&minNodeNum, 8);
  filter << "', true, 'r" << resultNum_str << "vl_";
  filter.write((const char*)&maxNodeNum, 8);
  filter << "', true)";
  filter << ")";
  /*filter << "(";
  filter << "(RowFilter (>=, 'binary:";
  filter << startRowKey;
  filter << "')) AND (RowFilter (<=, 'binary:";
  filter << stopRowKey;
  filter << "')) AND ";
  
  filter << "(";
  filter << "((QualifierFilter (=, 'binaryprefix:r" << resultNum_str << "vl_')) AND ";
  filter << " (QualifierFilter (>=, 'binary:r" << resultNum_str << "vl_";
  filter.write((const char*)&minNodeNum, 8);
  filter << "')) AND ";
  filter << "(QualifierFilter (<=, 'binary:r" << resultNum_str << "vl_";
  filter.write((const char*)&maxNodeNum, 8);
  filter << "')))";
  filter << " OR ";
  filter << "((QualifierFilter (=, 'binaryprefix:r" << resultNum_str << "_')) AND ";
  filter << "(QualifierFilter (>=, 'binary:r" << resultNum_str << "_";
  filter.write((const char*)&minNodeNum, 8);
  filter << "')) AND ";
  filter << "(QualifierFilter (<=, 'binary:r" << resultNum_str << "_";
  filter.write((const char*)&maxNodeNum, 8);
  filter << "')))";
  filter << ")";*/
  
  //filter << "SingleColumnValue(=, 'regexstring:*', 'substring:R', 'substring:r"
  //       << resultNum_str
  //       << "vl_*";
  //filter.write((const char*)&nodeNum, 8);
  //filter << "')";
  //filter << ")";

  LOGGER_SM << filter.str() << std::endl;
  ts.__set_filterString(filter.str());
  ScannerID scan_id = _hbase_client->scannerOpenWithScan(data_table, ts, m); 
  //LOGGER_SM << "The scanner id is " << scan_id << std::endl;
  
  LOGGER_SM << "\tAccessing table '" << data_table << "' with" << std::endl;
  LOGGER_SM << "\t startRowKey = " << startRowKey << std::endl;
  LOGGER_SM << "\t  stopRowKey = " << stopRowKey << std::endl;

  try {
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
		rowsResult.clear();
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0){
		  // LOGGER_SM << "No Row Results\n";
		  break;
	  }
	  
	  if(rowsResult.size() <= 1){
		// process rowsResult
		  for ( size_t i = 0; i < rowsResult.size(); i++) {
		// convert to return type
		// check if the rowkey is our's ... should be ....
		if ( rowsResult[ i].row.compare( 0, len_prefix_rowkey, prefixRowKey ) != 0)
		  continue; // break;
		bool ok = getResultsFromRow_filter( listOfResults, rowsResult[ i ], resultInfo, listOfVerticesID, minVertexID, maxVertexID );
		if ( ok) {
		
		  // getMeshInfoFromRow.push_back( model_info);
		  // getMeshInfoFromRow( tmp_lst_meshes, rowsResult[ i]);
		} else {
		  // nothing read? 
		  // eventually provide an error
		}
		// several rows = partitions so don't quit yet,
		  
		  }
	  }
      //break;
    } // while ( true)
    LOGGER_SM << "finish scannetGet\n";
    if ( listOfResults.size() == 0) {
      // nothing found
      // scan_ok = false; // scan was ok but nothing found ...
      std::stringstream tmp;
      if ( analysisID != "") {
	tmp << "Not found: Result for Dynamic Mesh" 
	    << " for Analysis = '" << analysisID << "'"
	    << " and stepValue = " << timeStep;
      } else {
	tmp << "Not found: Results for Static Mesh" ;
      }
      report = tmp.str();
    }
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << report << std::endl;
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    report = tmp.str();
    LOGGER_SM << "EXCEPTION: " << report << std::endl;
  }
  
  _hbase_client->scannerClose( scan_id);
    LOGGER_SM << "clossing scaner scannetGet\n";  
#ifdef READ_GROUPED_ROWS_FILTERS
  }
#endif

			
//#define DEBUG_RESULTS
#ifdef  DEBUG_RESULTS
	size_t n_debug_results = listOfResults.size() < 10 ? listOfResults.size() : 10;
		for(size_t i = 0; i < n_debug_results; i++){
			std::cout << listOfResults[i].id << "\t";
			for(size_t j = 0; j < listOfResults[i].value.size(); j++)
			  std::cout << listOfResults[i].value[j] << " ";
			std::cout << std::endl;
		}
#endif
  
  return scan_ok;
}

bool HBase::getResultInfoFromResultName( const std::string &sessionID, const std::string &modelID, 
					 const std::string &analysisID, const double stepValue, 
					 const std::string &resultName, ResultInfo &outResultInfo) {
  // look into the modelInfo table to get the correct table name
  TableModelEntry table_set;
  bool found = getVELaSSCoTableNames( sessionID, modelID, table_set);
  if ( found) {
    found = false;
    std::string report;
    std::vector<ResultInfo> listOfResultInfos;
    getListOfResults( report, listOfResultInfos, sessionID, modelID, analysisID, stepValue );
    size_t i = 0;
    for( i = 0; i < listOfResultInfos.size(); i++) {
      if(listOfResultInfos[i].name == resultName){
	outResultInfo = listOfResultInfos[i];
	found = true;
	break;
      }
    }
    if( i == listOfResultInfos.size() ){
      // LOGGER_SM << resultName << " is not found." << std::endl;
      // return "Error";
      found = false;
    }
  }
  return found;
}

std::string HBase::getResultFromVerticesID_thrift_filter( std::string& report, std::vector<ResultOnVertex> &listOfResults,
					 const std::string &sessionID, const std::string &modelID,
    			     const std::string &analysisID, const double       timeStep,  
				     const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID )
{
  LOGGER_SM << "getResultOnVertices THRIFT filter: =====" << std::endl;
  LOGGER_SM << "S " << sessionID      << std::endl;
  LOGGER_SM << "M " << modelID        << std::endl;
  LOGGER_SM << "R " << resultID       << std::endl;
  LOGGER_SM << "A " << analysisID     << std::endl;
  LOGGER_SM << "V " << "... ( " << listOfVerticesID.size() << " vertices)" << std::endl;
  LOGGER_SM << "T " << timeStep       << std::endl;

  string table_name;
  bool scan_ok = true;

  // look into the modelInfo table to get the correct table name
  TableModelEntry table_set;
  bool found = getVELaSSCoTableNames( sessionID, modelID, table_set);
  if ( found) {
    ResultInfo resultInfo;
    found = getResultInfoFromResultName( sessionID, modelID, analysisID, timeStep, resultID, resultInfo);
    if ( !found ){
      LOGGER_SM << resultID << " is not found." << std::endl;
      return "Error";
    }
    
    // by default hexstrings are lower case but some data has been injected as upper case !!!
    std::vector<int64_t> sorted_listOfVerticesID(listOfVerticesID.begin(), listOfVerticesID.end());
    std::sort(sorted_listOfVerticesID.begin(), sorted_listOfVerticesID.end());
    
    size_t i = 0;
    int64_t threshold = 1;
    while(i < sorted_listOfVerticesID.size()){
      // group the successive vertex ids
      size_t j = i + 1;
      for(; j < sorted_listOfVerticesID.size(); j++)
	if(!(
	     sorted_listOfVerticesID[j] >= (sorted_listOfVerticesID[j-1]-threshold) &&
	     sorted_listOfVerticesID[j] <= (sorted_listOfVerticesID[j-1]+threshold)
	     )) break;
      LOGGER_SM << "[" << i << ", " << j << "]\n";
		
      std::vector<int64_t> request_vertex_ids;
      for(size_t idx = i; idx <= j; idx++) request_vertex_ids.push_back(sorted_listOfVerticesID[idx]);
		
      scan_ok = getResultFromVerticesIDFromTables_filter( report, listOfResults, table_set._data, sessionID, modelID, analysisID, timeStep, resultInfo, request_vertex_ids, sorted_listOfVerticesID[i], sorted_listOfVerticesID[j-1]);
      if ( !scan_ok) {
	LOGGER_SM << "Failed scan with lowercase, listOfResults.size() = " << listOfResults.size() << std::endl;
      }
      if ( scan_ok && ( listOfResults.size() == 0)) {
	// try with uppercase
	scan_ok = getResultFromVerticesIDFromTables_filter( report, listOfResults, table_set._data, sessionID, modelID, analysisID, timeStep, resultInfo, request_vertex_ids, sorted_listOfVerticesID[i], sorted_listOfVerticesID[j-1], "%02X");
	if ( !scan_ok) {
	  LOGGER_SM << "Failed scan with uppercase, listOfResults.size() = " << listOfResults.size() << std::endl;
	}
      }
		
      i = j;
    }
  } else {
    LOGGER_SM << "Table names not found for this:" << std::endl;
    LOGGER_SM << "     sessionID = " << sessionID << std::endl;
    LOGGER_SM << "       modelID = " << modelID << std::endl;
    scan_ok = false;
  }
  string result;
  if ( scan_ok) {
    LOGGER_SM << "**********\n";
    bool there_are_results = listOfResults.size();
    if ( there_are_results) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    LOGGER_SM << "ERROR**********\n";
    result = "Error";
    report = "HBase::getResultFromVerticesID_thrift_filter THRIFT could not scan.";
  }
  return result;
}


// ==============================================
// ==== Thrift
// ==============================================
//#define READ_GROUPED_ROWS

/* GetResultsFromVerticesID */
static bool getResultsFromRow( std::unordered_map< int64_t, std::vector< double > > &resultOnVertexListMap,
			       // std::vector< ResultOnVertex > &listOfResults, 
			       const TRowResult &rowResult, const ResultInfo &resultInfo) {
  int num_results = 0;
  for ( CellMap::const_iterator it = rowResult.columns.begin(); 
	it != rowResult.columns.end(); ++it) {
    const char *cq_str = it->first.c_str();
    const char CF = cq_str[ 0];
    // cq_str[ 1] should be ':'
    const char subC = cq_str[ 2];
    if ( CF == 'R') {
      if ( it->first.length() > 3) { // for c1xxxx
      	int result_number = 0;
      	int n = sscanf( &cq_str[ 3], "%d", &result_number);
      	if ( n == 1 && resultInfo.resultNumber == result_number ) {
      	  // in fact not needed as it should always be meshID ( == coordsID)
      	  const char *pinfo = &cq_str[ 3];
      	  while ( *pinfo && isdigit( *pinfo))
      	    pinfo++;
      	  if ( subC == 'r') { // right qualifier, coordinates
      	    // coordinates are of the form rXXXXXXvl_ID, where ID is binary indes ...
      	    int64_t node_id;
      	    std::vector<double> result_values;
	    if ( *pinfo == '_') pinfo++; // some data only is r000001_nodeId // to be removed soon
	    else pinfo+= 3; // the 'vl_'
      	    node_id = byteSwap< int64_t>( *( int64_t *)pinfo );
      	    double *coords = ( double *)it->second.value.data();
            for(int i = 0; i < resultInfo.numberOfComponents; i++)
      	      result_values.push_back( byteSwap< double>( coords[ i ] ) );
      	    // ResultOnVertex result;
	    // result.__set_id( node_id );
	    // result.__set_value( result_values );
	    // listOfResults.push_back( result );
	    resultOnVertexListMap[ node_id] = result_values;
	    num_results++;
      	  }
      	}
      }
    }
  }
  return num_results;
}

bool HBase::getResultFromVerticesIDFromTables( std::string& report, std::vector<ResultOnVertex> &listOfResults, const std::string& data_table,
           const std::string &sessionID,  const std::string &modelID,
           const std::string &analysisID, const double       timeStep,  
           const ResultInfo &resultInfo,  const std::vector<int64_t> &listOfVerticesID, const char *format ){
  bool scan_ok = true;
  
  // listOfResults.reserve( listOfVerticesID.size() );

  std::unordered_map< int64_t, std::vector< double > > resultOnVertexListMap;

#ifdef READ_GROUPED_ROWS
  int chunk_size = 4;
  for(int rowIdx = 0; rowIdx <= 1000; rowIdx+=chunk_size){
#endif
    vector< TRowResult> rowsResult;
    std::map<std::string,std::string> m;

    StrVec cols;
    char buf[ 100];
    std::cout << "Making list of vertices column names...\n";
    for(size_t i = 0; i < listOfVerticesID.size(); i++){
      {
	std::ostringstream oss;
	sprintf( buf, "R:r%06dvl_", resultInfo.resultNumber );
	oss << buf;
	int64_t vertexID = byteSwap<int64_t>( listOfVerticesID[i] );
	oss.write( (char*)&vertexID, sizeof(int64_t));
	//LOGGER_SM << "=====================> " << oss.str() << std::endl;
	cols.push_back( oss.str() );

	// // to be removed soon
	// // some data only is r000001_nodeId
	// std::ostringstream oss2;
	// sprintf( buf, "R:r%06d_", resultInfo.resultNumber );
	// oss2 << buf;
	// oss2.write( (char*)&vertexID, sizeof(int64_t));
	// //LOGGER_SM << "=====================> " << oss.str() << std::endl;
	// cols.push_back( oss2.str() );
      }
    }
    std::cout << "Making list of vertices column names (Done)...\n";
    // M:un for units
    // M:c* prefix for c123456nm c123456nc for name of coordinates set and number of coordinates = vertices = nodes
    // M:m* prefix fir m123456nm (name) m123456cn (coord.set.name) m123456et (elemType)
    //                 m123456ne (numberOfElements) m123456nn (numNodesPerElem) m123456cl (color)
    // ScannerID scannerOpen( Test tableName, Test startRow, list< Text> columns, map< Text, Text> attributes)
    // has to build the rowkey.... 
    // Metadata rowkeys = modelId + AnalysisID + StepNumber
    double my_stepValue = ( analysisID == "") ? 0.0 : timeStep; // rowkeys for static meshes have stepValue == 0.0
    std::string prefixRowKey = createMetaRowKey( modelID, analysisID, my_stepValue, format);
#ifdef READ_GROUPED_ROWS
    std::string startRowKey = createDataRowKey( modelID, analysisID, my_stepValue, rowIdx/*0*/, format); // partitionID = 0
    std::string stopRowKey = createDataRowKey( modelID, analysisID, my_stepValue, rowIdx+chunk_size/*( int)0x7fffffff*/, format); // partitionID = biggest ever
#else
    std::string startRowKey = createDataRowKey( modelID, analysisID, my_stepValue, 0, format); // partitionID = 0
    std::string stopRowKey = createDataRowKey( modelID, analysisID, my_stepValue, ( int)0x7fffffff, format); // partitionID = biggest ever
#endif
    const size_t len_prefix_rowkey = prefixRowKey.length();
    ScannerID scan_id = _hbase_client->scannerOpenWithStop( data_table, startRowKey, stopRowKey, cols, m);
    // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);

    //LOGGER_SM << "\tAccessing table '" << data_table << "' with" << std::endl;
    //LOGGER_SM << "\t startRowKey = " << startRowKey << std::endl;
    //LOGGER_SM << "\t  stopRowKey = " << stopRowKey << std::endl;

    LOGGER_SM << "Looking for results of " << listOfVerticesID.size() << " vertices" << std::endl; 
    try {
      // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
      while ( true ) {
	_hbase_client->scannerGet( rowsResult, scan_id);
	if ( rowsResult.size() == 0)
	  break;
	// process rowsResult
	for ( size_t i = 0; i < rowsResult.size(); i++) {
	  // convert to return type
	  // check if the rowkey is our's ... should be ....
	  if ( rowsResult[ i].row.compare( 0, len_prefix_rowkey, prefixRowKey ) != 0)
	    continue; // break;
	  bool ok = getResultsFromRow( resultOnVertexListMap, /*listOfResults,*/ rowsResult[ i ], resultInfo);
	  if ( ok) {
			
	    //size_t i = 0;
	    //for(; i < foundFlags.size(); i++)
	    //  if(!foundFlags[i]) break;
	    //if(i == foundFlags.size()) done = true;

	    // getMeshInfoFromRow.push_back( model_info);
	    // getMeshInfoFromRow( tmp_lst_meshes, rowsResult[ i]);
	  } else {
	    // nothing read? 
	    // eventually provide an error
	  }
	  // several rows = partitions so don't quit yet,
	  // break;
	}
      } // while ( true)

      if ( listOfResults.size() == 0) {
	// nothing found
	// scan_ok = false; // scan was ok but nothing found ...
	std::stringstream tmp;
	if ( analysisID != "") {
	  tmp << "Not found: Result for Dynamic Mesh" 
	      << " for Analysis = '" << analysisID << "'"
	      << " and stepValue = " << timeStep;
	} else {
	  tmp << "Not found: Results for Static Mesh" ;
	}
	report = tmp.str();
      }
    } catch ( const IOError &ioe) {
      scan_ok = false;
      std::stringstream tmp;
      tmp << "IOError = " << ioe.what();
      report = tmp.str();
      LOGGER_SM << "EXCEPTION: " << report << std::endl;
    } catch ( TException &tx) {
      scan_ok = false;
      std::stringstream tmp;
      tmp << "TException = " << tx.what();
      report = tmp.str();
      LOGGER_SM << "EXCEPTION: " << report << std::endl;
    }
    _hbase_client->scannerClose( scan_id);
#ifdef READ_GROUPED_ROWS
  }
#endif

  LOGGER_SM << "Processing " << resultOnVertexListMap.size() << " retrieved results." << std::endl;
  size_t orig_num_vertices = resultOnVertexListMap.size();

  // get only asked vertices and 
  int64_t numVerticesNotFound = 0;
  listOfResults.clear();
  for ( size_t i = 0; i < listOfVerticesID.size(); i++) {
    int64_t vertexId = listOfVerticesID[ i];
    std::unordered_map< int64_t, std::vector< double > >::const_iterator it_vertexAndResult = resultOnVertexListMap.find( vertexId);
    if ( it_vertexAndResult == resultOnVertexListMap.end()) {
      // LOGGER_SM << "not found";
      numVerticesNotFound++;
    } else {
      // LOGGER_SM << " found";
      ResultOnVertex result;
      result.__set_id( it_vertexAndResult->first );
      result.__set_value( it_vertexAndResult->second );
      listOfResults.push_back( result );
    }
  }
  
  LOGGER_SM << "Used " << listOfResults.size() << " verticesID from the retrieved " << orig_num_vertices << " ones." << std::endl; 
  if ( numVerticesNotFound) {
    LOGGER_SM << numVerticesNotFound << " out of " << listOfVerticesID.size() << " vertices NOT FOUND" << std::endl;
  }

  // 	#define DEBUG_RESULTS
#ifdef  DEBUG_RESULTS
  size_t n_debug_results = listOfResults.size() < 10 ? listOfResults.size() : 10;
  for(size_t i = 0; i < n_debug_results; i++){
    std::cout << listOfResults[i].id << "\t";
    for(size_t j = 0; j < listOfResults[i].value.size(); j++)
      std::cout << listOfResults[i].value[j] << " ";
    std::cout << std::endl;
  }
#endif
  
  return scan_ok;
}

std::string HBase::getResultFromVerticesID_thrift( std::string& report, std::vector<ResultOnVertex> &listOfResults,
					 const std::string &sessionID, const std::string &modelID,
    			     const std::string &analysisID, const double       timeStep,  
				     const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID )
{
  LOGGER_SM << "getResultOnVertices THRIFT: =====" << std::endl;
  LOGGER_SM << "S " << sessionID      << std::endl;
  LOGGER_SM << "M " << modelID        << std::endl;
  LOGGER_SM << "R " << resultID       << std::endl;
  LOGGER_SM << "A " << analysisID     << std::endl;
  LOGGER_SM << "V " << "... ( " << listOfVerticesID.size() << " vertices)" << std::endl;
  LOGGER_SM << "T " << timeStep       << std::endl;

  string table_name;
  bool scan_ok = true;

  // look into the modelInfo table to get the correct table name
  TableModelEntry table_set;
  bool found = getVELaSSCoTableNames( sessionID, modelID, table_set);
  if ( found) {
    ResultInfo resultInfo;
    found = getResultInfoFromResultName( sessionID, modelID, analysisID, timeStep, resultID, resultInfo);
    if ( !found ){
      LOGGER_SM << resultID << " is not found." << std::endl;
      return "Error";
    }
	  
    // by default hexstrings are lower case but some data has been injected as upper case !!!
    scan_ok = getResultFromVerticesIDFromTables( report, listOfResults, table_set._data, sessionID, modelID, analysisID, timeStep, resultInfo, listOfVerticesID);
    if ( !scan_ok) {
      LOGGER_SM << "Failed scan with lowercase, listOfResults.size() = " << listOfResults.size() << std::endl;
    }
    if ( scan_ok && ( listOfResults.size() == 0)) { // i.e. scan was performed but nothing found
      // try with uppercase
      scan_ok = getResultFromVerticesIDFromTables( report, listOfResults, table_set._data, sessionID, modelID, analysisID, timeStep, resultInfo, listOfVerticesID, "%02X");
      if ( !scan_ok) {
	LOGGER_SM << "Failed scan with uppercase, listOfResults.size() = " << listOfResults.size() << std::endl;
      }
    }
  } else {
    LOGGER_SM << "Table names not found for this:" << std::endl;
    LOGGER_SM << "     sessionID = " << sessionID << std::endl;
    LOGGER_SM << "       modelID = " << modelID << std::endl;
    scan_ok = false;
  }
  string result;
  if ( scan_ok) {
    LOGGER_SM << "**********\n";
    bool there_are_results = listOfResults.size();
    if ( there_are_results) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    LOGGER_SM << "ERROR**********\n";
    result = "Error";
    report = "HBase::getResultFromVerticesID THRIFT could not scan.";
  }

  return result;
}

// ==============================================================
// ==============================================================
// GetCoordinatesAndElementsFromMesh
// ==============================================================
// ==============================================================

std::string HBase::parse1FEM(std::string b)
{
	std::stringstream result;

    printf("Parsing JSON tree ...\n");
    cJSON *json = cJSON_Parse(b.c_str());
    if ( !json) {
      printf("not a JSON tree !\n");
      return "";
    }
	
	std::vector<int64_t> indices;
	
    cJSON *_return = cJSON_CreateObject();
    cJSON *listOfVertices = cJSON_CreateArray();
    cJSON *listOfIndices  = cJSON_CreateArray();
    cJSON_AddItemToObject(_return, "vertices", listOfVertices);
    cJSON_AddItemToObject(_return, "indices", listOfIndices);
    
    // setbuf( stdout, NULL);
  // printf( "Number of elements = %d\n", cJSON_GetArraySize(json));
  for (int i = 0; i < cJSON_GetArraySize(json); i ++ ) {
    // Get each row element
    cJSON *row = cJSON_GetArrayItem ( json, i);
    // Read all elements of each row
    printf( "Number of rows = %d\n", cJSON_GetArraySize(row));
    int num_models = cJSON_GetArraySize(row);
    //there_are_models = ( num_models > 0);
    for (int j = 0; j < num_models; j ++ ) {
      printf( "row # %d\n", j);
      //A Hbase key  is composed by two elements: key and cell (which contains data)
      cJSON *keyC = cJSON_GetArrayItem (row, j);
      cJSON *keyJ = cJSON_GetArrayItem (keyC, 0);
      // Row key:
      string key = base64_decode(keyJ->valuestring);
      cJSON *cellsJ = cJSON_GetArrayItem (keyC, 1);
      // cJSON * elem  = cJSON_CreateObject();
       printf( "keyC = %s\n", keyC->valuestring);
       printf( "keyJ(decod) = %s", Hexdump( key).c_str());

      for (int k = 0; k < cJSON_GetArraySize(cellsJ); k++) {
	//    printf( "   cell # %d\n", k);
	cJSON *contents = cJSON_GetArrayItem (cellsJ, k);
	// contents is an array with 3 items: ( value, timestap, and "$")
	// printf( "   contents size %d\n", cJSON_GetArraySize( contents));
	// Colum Qualifier of the form: ColumFamily:qualifier      for instance Properties:nm
	cJSON *cq = cJSON_GetArrayItem (contents, 0);
	string tmp_cq = base64_decode(cq->valuestring);
	//printf( "      elem(decod) = %s\n", tmp_cq.c_str());
	cJSON *value = cJSON_GetArrayItem (contents, 2);
	string tmp_value = base64_decode( value->valuestring);
	//printf( "      elem(decod) = %s\n", tmp_value.c_str());
	if ( tmp_cq.find("M:c") == 0) {
		
		size_t  underline_pos = tmp_cq.find("_");
		int64_t id = *((int64_t*)(&(tmp_cq[ underline_pos ])));

        double vertex[3];
			  
	    vertex[0] = *((double*)(&tmp_value[0 ]));
		vertex[1] = *((double*)(&tmp_value[8 ]));
		vertex[2] = *((double*)(&tmp_value[16]));
		
		//vertices.push_back( vertex[0] );
		//vertices.push_back( vertex[1] );
		//vertices.push_back( vertex[2] );

		char identifier[] = { 'v' };
		result.write(identifier, 1);
		result.write((const char*)&id, 8);
		result.write((const char*)vertex, 24);

		//static int testi = 0; 
		//cout << testi++ << ". v [ " << id << " ] = " << vertex[0] << " " << vertex[1] << " " << vertex[2] << endl;
	} else if ( tmp_cq.find("M:m") == 0) {
		
		//int64_t id = *((int64_t*)(&(tmp_cq[ 10 ])));
		
		//std::cout << "M:m " << tmp_cq << "with id " << id << "found\n";
		// triangle
		if(tmp_cq.find("M:m000001_") == 0){
			int64_t indices[3];
			
			indices[0] = *((int64_t*)(&tmp_value[0 ]));			
			indices[1] = *((int64_t*)(&tmp_value[8 ]));
			indices[2] = *((int64_t*)(&tmp_value[16 ]));
			
			char identifier[] = { 't' };
			result.write(identifier, 1);
			result.write((const char*)indices, 24);
			
			//static size_t iiiii = 0;
			//cout << iiiii++ << "indices = ";
			//for(size_t iii = 0; iii < 3; iii++)  cout << indices[iii] << " ";
			//cout << endl;
			
		}
		// tetrahedrons
		else if(tmp_cq.find("M:m020406_") == 0){
			int64_t indices[4];
			
			indices[0] = *((int64_t*)(&tmp_value[0 ]));			
			indices[1] = *((int64_t*)(&tmp_value[8 ]));
			indices[2] = *((int64_t*)(&tmp_value[16]));
			indices[3] = *((int64_t*)(&tmp_value[24]));
			
			//cout << "indices = ";
			//for(size_t iii = 0; iii < 4; iii++)  cout << indices[iii] << " ";
			//cout << endl;
			
		}
		else if(tmp_cq.find("M:m844444_") == 0){
			int64_t indices[8];
			
			indices[0] = *((int64_t*)(&tmp_value[0 ]));			
			indices[1] = *((int64_t*)(&tmp_value[8 ]));
			indices[2] = *((int64_t*)(&tmp_value[16]));
			indices[3] = *((int64_t*)(&tmp_value[24]));
			indices[4] = *((int64_t*)(&tmp_value[32]));			
			indices[5] = *((int64_t*)(&tmp_value[40]));
			indices[6] = *((int64_t*)(&tmp_value[48]));
			indices[7] = *((int64_t*)(&tmp_value[56]));
			
			//cout << "indices = ";
			//for(size_t iii = 0; iii < 8; iii++)  cout << indices[iii] << " ";
			//cout << endl;
		}
		
		
	}
      }
    }
  }

    //return cJSON_Print(_return);
    return result.str();
}

std::string HBase::getCoordinatesAndElementsFromMesh( std::string& report,
					std::vector<Vertex>& vertices,
					std::vector< Element > &listOfElements, std::vector< ElementAttrib > &listOfElementAttribs, 
					std::vector< ElementGroup > &listOfElementInfoGroups,
					const std::string &sessionID, const std::string &modelID,
                    const std::string &analysisID,const double timeStep, const MeshInfo& meshInfo) {
  //return getCoordinatesAndElementsFromMesh_curl( sessionID, modelID, analysisID, timeStep, partitionID );		
  return getCoordinatesAndElementsFromMesh_thrift( report, vertices, listOfElements, listOfElementAttribs, listOfElementInfoGroups, sessionID, modelID, analysisID, timeStep, meshInfo );		
}

std::string HBase::getCoordinatesAndElementsFromMesh_curl(const std::string &sessionID, const std::string &modelID, const std::string &analysisID, const double timeStep, const unsigned partitionID) {
  LOGGER_SM << "getCoordinatesAndElementsFromMesh CURL: =====" << std::endl;
  LOGGER_SM << "S " << sessionID      << std::endl;
  LOGGER_SM << "M " << modelID        << std::endl;
  //LOGGER_SM << "R " << resultID     << std::endl;
  LOGGER_SM << "A " << analysisID     << std::endl;
  LOGGER_SM << "T " << timeStep       << std::endl;
  LOGGER_SM << "P " << partitionID    << std::endl;

  TableModelEntry tableModel;
  if(getVELaSSCoTableNames(sessionID, modelID, tableModel) == false)
    cout << "No table containing the model with " << modelID << "Model ID.\n";
  else
    cout << "Model Table: " << tableModel._list_models
         << "Meta Table: "  << tableModel._metadata
         << "Data Table: "  << tableModel._data 		<< endl; 

  string cmd = "http://" + _db_host + ":8880/";
  cmd += tableModel._data;
  cmd += "/";
  std::stringstream key;
	
  std::string simulationID_hex       = modelID; 
  std::string analysisNameLength_hex = toHexString(byteSwap((uint32_t)analysisID.size()));
  std::string analysisNameChars      = analysisID;
  std::string timeStep_hex           = toHexString(byteSwap((double)timeStep));
  std::string partitionID_hex        = toHexString(byteSwap((uint32_t)partitionID));

	key << simulationID_hex
		<< analysisNameLength_hex
		<< analysisNameChars
		<< timeStep_hex
		<< partitionID_hex;

  cmd += key.str();
  cout << cmd << endl;

    
  CurlCommand do_curl;
  string buffer;
    
  //bool ok = do_curl.Evaluate( buffer, cmd);

  LOGGER_SM << "**********\n";    
  // LOGGER_SM << buffer << std::endl;
  LOGGER_SM << "**********\n";    
    
  //
  string result( "");

  std::size_t found = buffer.find( "Not found");
  if ( ( found == string::npos) || ( found > 10)) {
	  cout << cmd << endl;
    // assume that the error message "Not found" should be at the begining of the returned buffer
    cout << "Looking for mesh...\n";
    result = parse1FEM(buffer);
  } else {
	cout << "Model not found\n";
  }
  
  cout << cmd << endl;
  
  return result;
}



// ==============================================
// ==== Thrift
// ==============================================
//#define READ_MESH_GROUPED_ROWS

/* GetMeshDrawData */
static bool getIndicesFromRow( std::vector< Element > &listOfElements, std::vector< ElementAttrib > &listOfElementAttribs, std::vector< ElementGroup > &listOfElementInfoGroups, const TRowResult &rowResult, const MeshInfo &meshInfo) {
  int num_elements = 0;
  
  //  UnknownElement = 0,
  //  PointElement = 1,
  //  LineElement = 2,
  //  TriangleElement = 3,
  //  QuadrilateralElement = 4,
  //  TetrahedraElement = 5,
  //  HexahedraElement = 6,
  //  PrismElement = 7,
  //  PyramidElement = 8,
  //  SphereElement = 9,
  //  CircleElement = 10,
  //  ComplexParticleElement = 11
  
  for ( CellMap::const_iterator it = rowResult.columns.begin(); 
	it != rowResult.columns.end(); ++it) {
    const char *cq_str = it->first.c_str();
    //std::cout << cq_str << *(( int64_t *)it->second.value.data()) << std::endl;
    const char CF = cq_str[ 0];
    // cq_str[ 1] should be ':'
    const char subC = cq_str[ 2];
    if ( CF == 'M') {
      if ( it->first.length() > 3) { // for c1xxxx
      	int mesh_number = 0;
      	int n = sscanf( &cq_str[ 3], "%d", &mesh_number);
      	//std::cout << "=======> " << meshInfo.name << " " << meshInfo.meshNumber << " " << mesh_number << std::endl;
      	if ( n == 1 && mesh_number == meshInfo.meshNumber ) {
      	  // in fact not needed as it should always be meshID ( == coordsID)
      	  const char *pinfo = &cq_str[ 3];
      	  while ( *pinfo && isdigit( *pinfo))
      	    pinfo++;
      	  if ( subC == 'm') { // right qualifier, coordinates
      	    // coordinates are of the form mMMMMMM##_ID, where ID is binary indes ...
      	    const char column_qualifier[2] = { pinfo[0], pinfo[1] };
      	    pinfo += 3; // ##_
      	    
      	    int64_t element_id = byteSwap< int64_t>( *( int64_t *)pinfo );
      	    //std::cout << "elementID " << element_id << std::endl;
      	    if(       meshInfo.elementType.shape == ElementShapeType::type::UnknownElement){
				LOGGER_SM << "  Unknown Element.\n";
		    } else if(meshInfo.elementType.shape == ElementShapeType::type::PointElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
                node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);        
			  }
		    } else if(meshInfo.elementType.shape == ElementShapeType::type::LineElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
			    if(meshInfo.name == "p2p contacts"){
					node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
					node_ids.push_back( byteSwap< int64_t>( indices[ 1 ] ) );
					node_ids.push_back( byteSwap< int64_t>( indices[ 2 ] ) );
				} else if(meshInfo.name == "p2w contacts"){
					node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
					node_ids.push_back( byteSwap< int64_t>( indices[ 1 ] ) );
				} else {
					node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
					node_ids.push_back( byteSwap< int64_t>( indices[ 1 ] ) );
				}
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);        
			  }
		    } else if(meshInfo.elementType.shape == ElementShapeType::type::TriangleElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
                node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 1 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 2 ] ) );
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);        
			  }
			} else if(meshInfo.elementType.shape == ElementShapeType::type::TetrahedraElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
                node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 1 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 2 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 3 ] ) );
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);        
			  }
			} else if(meshInfo.elementType.shape == ElementShapeType::type::QuadrilateralElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
                node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 1 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 2 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 3 ] ) );
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);        
			  }
			} else if(meshInfo.elementType.shape == ElementShapeType::type::HexahedraElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
                node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 1 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 2 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 3 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 4 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 5 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 6 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 7 ] ) );
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);        
			  }
			} else if(meshInfo.elementType.shape == ElementShapeType::type::PrismElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
                node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 1 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 2 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 3 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 4 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 5 ] ) );
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);        
			  }
			} else if(meshInfo.elementType.shape == ElementShapeType::type::PyramidElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
                node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 1 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 2 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 3 ] ) );
                node_ids.push_back( byteSwap< int64_t>( indices[ 4 ] ) );
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);        
			  }
			} else if(meshInfo.elementType.shape == ElementShapeType::type::SphereElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
                node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);   
      	        //std::cout << "Index = " << byteSwap< int64_t>( indices[ 0 ] ) << std::endl;   
			  }
			  else if(column_qualifier[0] == 'r' && column_qualifier[1] == 'd'){
				ElementAttrib element_attrib;
				element_attrib.id   = element_id;
				element_attrib.name = "Radius";
			    double radius = *(( double *)it->second.value.data());
			    std::vector<double> element_attrib_value;
			    element_attrib_value.push_back( byteSwap< double >(radius) );
			    element_attrib.value = element_attrib_value;
			    listOfElementAttribs.push_back(element_attrib);     	        
			    //std::cout << "Radius = " << byteSwap< double >(radius) << std::endl;
			  }
			} else if(meshInfo.elementType.shape == ElementShapeType::type::CircleElement){
			  if(column_qualifier[0] == 'c' && column_qualifier[1] == 'n'){
				Element element;
			    element.id = element_id;
				std::vector<NodeID> node_ids;
			    int64_t *indices = ( int64_t *)it->second.value.data();
                node_ids.push_back( byteSwap< int64_t>( indices[ 0 ] ) );
      	        element.nodes_ids = node_ids;
      	        listOfElements.push_back(element);        
			  }
			  else if(column_qualifier[0] == 'r' && column_qualifier[1] == 'd'){
				ElementAttrib element_attrib;
				element_attrib.id   = element_id;
				element_attrib.name = "Radius";
			    double radius = *(( double *)it->second.value.data());
			    std::vector<double> element_attrib_value;
			    element_attrib_value.push_back( byteSwap< int64_t>(radius) );
			    element_attrib.value = element_attrib_value;
			    listOfElementAttribs.push_back(element_attrib);     	        
			  }
			  else if(column_qualifier[0] == 'r' && column_qualifier[1] == 'd'){
				ElementAttrib element_attrib;
				element_attrib.id   = element_id;
				element_attrib.name = "Radius";
			    double radius = *(( double *)it->second.value.data());
			    std::vector<double> element_attrib_value;
			    element_attrib_value.push_back( byteSwap< int64_t>(radius) );
			    element_attrib.value = element_attrib_value;
			    listOfElementAttribs.push_back(element_attrib);     	        
			  }
			  else if(column_qualifier[0] == 'n' && column_qualifier[1] == 'o'){
				ElementAttrib element_attrib;
				element_attrib.id   = element_id;
				element_attrib.name = "Normal";
			    double* normal = (( double *)it->second.value.data());
			    std::vector<double> element_attrib_value;
			    element_attrib_value.push_back( byteSwap< int64_t>(normal[0]) );
			    element_attrib_value.push_back( byteSwap< int64_t>(normal[1]) );
			    element_attrib_value.push_back( byteSwap< int64_t>(normal[2]) );
			    element_attrib.value = element_attrib_value;
			    listOfElementAttribs.push_back(element_attrib);     	        
			  }
			} else if(meshInfo.elementType.shape == ElementShapeType::type::ComplexParticleElement){
				LOGGER_SM << "  ComplexParticleElement is not supported.\n";
			}
			
      	    
      	    if(column_qualifier[0] == 'g' && column_qualifier[1] == 'r'){
				ElementGroup element_group;
				element_group.id       = element_id;
				int64_t group_id = *(( int64_t *)it->second.value.data());
				element_group.group_id = group_id;
				listOfElementInfoGroups.push_back(element_group);
			}
      	  }
      	}
      }
    }
  }
  
  return num_elements;
}

bool HBase::getMeshElementsFromTable(std::string& report,
                    std::vector< Element > &listOfElements, std::vector< ElementAttrib > &listOfElementAttribs, std::vector< ElementGroup > &listOfElementInfoGroups,
                    const std::string& table_name,
					const std::string &sessionID, const std::string &modelID,
                    const std::string &analysisID,const double timeStep, const MeshInfo& meshInfo,  const char *format) {
  bool scan_ok = true;
#ifdef READ_MESH_GROUPED_ROWS
  int chunk_size = 4;
  for(int rowIdx = 0; rowIdx <= 1000; rowIdx+=chunk_size){
#endif
	  vector< TRowResult> rowsResult;
	  std::map<std::string,std::string> m;
	  // TScan ts;
	  // std::stringstream filter;
	  // filter.str("");
	  // ts.__set_filterString(filter.str());
	  StrVec cols;
	  
	  cols.push_back( "M");
	  
	  double my_stepValue = ( analysisID == "") ? 0.0 : timeStep; // rowkeys for static meshes have stepValue == 0.0
	  std::string prefixRowKey = createMetaRowKey( modelID, analysisID, my_stepValue, format);
#ifdef READ_MESH_GROUPED_ROWS
	  std::string startRowKey = createDataRowKey( modelID, analysisID, my_stepValue, rowIdx/*0*/, format); // partitionID = 0
	  std::string stopRowKey = createDataRowKey( modelID, analysisID, my_stepValue, rowIdx+chunk_size/*( int)0x7fffffff*/, format); // partitionID = biggest ever
#else
	  std::string startRowKey = createDataRowKey( modelID, analysisID, my_stepValue, 0, format); // partitionID = 0
	  std::string stopRowKey = createDataRowKey( modelID, analysisID, my_stepValue, ( int)0x7fffffff, format); // partitionID = biggest ever
#endif
	  const size_t len_prefix_rowkey = prefixRowKey.length();
	  ScannerID scan_id = _hbase_client->scannerOpenWithStop( table_name, startRowKey, stopRowKey, cols, m);
	  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);

	  //LOGGER_SM << "\tAccessing table '" << data_table << "' with" << std::endl;
	  //LOGGER_SM << "\t startRowKey = " << startRowKey << std::endl;
	  //LOGGER_SM << "\t  stopRowKey = " << stopRowKey << std::endl;
	  
	  try {
		// or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
		while ( true) {
		  _hbase_client->scannerGet( rowsResult, scan_id);
		  if ( rowsResult.size() == 0)
		break;
		  // process rowsResult
		  for ( size_t i = 0; i < rowsResult.size(); i++) {
			// convert to return type
			// check if the rowkey is our's ... should be ....
			if ( rowsResult[ i].row.compare( 0, len_prefix_rowkey, prefixRowKey ) != 0)
			  continue; // break;
			bool ok = getIndicesFromRow( listOfElements, listOfElementAttribs, listOfElementInfoGroups, rowsResult[ i ], meshInfo );
			if ( ok) {
				

			  // getMeshInfoFromRow.push_back( model_info);
			  // getMeshInfoFromRow( tmp_lst_meshes, rowsResult[ i]);
			} else {
			  // nothing read? 
			  // eventually provide an error
			}
			// several rows = partitions so don't quit yet,
			// break;
		  }
		} // while ( true)

		if ( listOfElements.size() == 0) {
		  // nothing found
		  // scan_ok = false; // scan was ok but nothing found ...
		  std::stringstream tmp;
		  if ( analysisID != "") {
		tmp << "Not found: Result for Dynamic Mesh" 
			<< " for Analysis = '" << analysisID << "'"
			<< " and stepValue = " << timeStep;
		  } else {
		tmp << "Not found: Results for Static Mesh" ;
		  }
		  report = tmp.str();
		}
	  } catch ( const IOError &ioe) {
		scan_ok = false;
		std::stringstream tmp;
		tmp << "IOError = " << ioe.what();
		report = tmp.str();
		LOGGER_SM << "EXCEPTION: " << report << std::endl;
	  } catch ( TException &tx) {
		scan_ok = false;
		std::stringstream tmp;
		tmp << "TException = " << tx.what();
		report = tmp.str();
		LOGGER_SM << "EXCEPTION: " << report << std::endl;
	  }
	  _hbase_client->scannerClose( scan_id);
#ifdef READ_MESH_GROUPED_ROWS
  }
#endif
  
  	#define DEBUG_RESULTS
	#ifdef  DEBUG_RESULTS
	size_t n_debug_elements = listOfElements.size() < 10 ? listOfElements.size() : 10;
			for(size_t i = 0; i < n_debug_elements; i++){
				std::cout << listOfElements[i].id << "\t";
				for(size_t j = 0; j < listOfElements[i].nodes_ids.size(); j++)
				  std::cout << listOfElements[i].nodes_ids[j] << " ";
				std::cout << std::endl;
			}
			
			n_debug_elements = listOfElementAttribs.size() < 10 ? listOfElementAttribs.size() : 10;
			for(size_t i = 0; i < n_debug_elements; i++){
				std::cout << listOfElementAttribs[i].id << "\t" << listOfElementAttribs[i].name << "\t";
				for(size_t j = 0; j < listOfElementAttribs[i].value.size(); j++)
				  std::cout << listOfElementAttribs[i].value[j] << " ";
				std::cout << std::endl;
			}
	#endif
  
  return scan_ok;
}

std::string HBase::getCoordinatesAndElementsFromMesh_thrift( std::string& report,
					std::vector<Vertex>& vertices, 
					std::vector< Element > &listOfElements, std::vector< ElementAttrib > &listOfElementAttribs, 
					std::vector< ElementGroup > &listOfElementInfoGroups,
					const std::string &sessionID, const std::string &modelID,
                    const std::string &analysisID,const double timeStep, const MeshInfo& meshInfo )
{
  LOGGER_SM << "getCoordinatesAndElementsFromMesh THRIFT: =====" << std::endl;
  LOGGER_SM << "S " << sessionID          << std::endl;
  LOGGER_SM << "M " << modelID            << std::endl;
  LOGGER_SM << "A " << analysisID         << std::endl;
  LOGGER_SM << "T " << timeStep           << std::endl;
  LOGGER_SM << "M " << meshInfo.name  << std::endl;

  bool scan_ok = true;
  TableModelEntry table_set;
  bool found = getVELaSSCoTableNames( sessionID, modelID, table_set);
  if ( found) {
	
	  /*std::string list_of_vertices_report;
	  std::string list_of_vertices_result;
	  
	  if(meshInfo.name == "p2p contacts"){
		list_of_vertices_result += getListOfVerticesFromMesh(list_of_vertices_report, vertices, sessionID, modelID, analysisID, timeStep, 1);
	  } else if(meshInfo.name == "p2w contacts"){
	  } else {
		list_of_vertices_result += getListOfVerticesFromMesh(list_of_vertices_report, vertices, sessionID, modelID, analysisID, timeStep, meshInfo.meshNumber);
	  }*/
		  
	  // if no errors
	  //if(list_of_vertices_result == "Ok"){

		//LOGGER_SM << "Number of vertices = " << vertices.size() << std::endl;  

		// by default hexstrings are lower case but some data has been injected as upper case !!!
		scan_ok = getMeshElementsFromTable( report, listOfElements, listOfElementAttribs, listOfElementInfoGroups, table_set._data, sessionID, modelID, analysisID, timeStep, meshInfo );
		if ( scan_ok && ( listOfElements.size() == 0)) {
		  // try with uppercase
		  scan_ok = getMeshElementsFromTable( report, listOfElements, listOfElementAttribs, listOfElementInfoGroups, table_set._data, sessionID, modelID, analysisID, timeStep, meshInfo, "%02X" );
		}
			  
		LOGGER_SM << "Number of elements = " << listOfElements.size() << std::endl;  
		LOGGER_SM << "Number of element attributes = " << listOfElementAttribs.size() << std::endl;        
		LOGGER_SM << "Number of element groups = " << listOfElementInfoGroups.size() << std::endl;
			  
	  //} else {
			  
	 //LOGGER_SM << "ERROR: getCoordinatesAndElementsFromMesh_thrift Report:" << list_of_vertices_report << std::endl;
	 //return "Error";
		
	  //}
  } else {
	scan_ok = false;
  }
    
  string result;
  if ( scan_ok) {
    LOGGER_SM << "**********\n";
    bool there_are_results = (listOfElements.size() > 0);
    if ( there_are_results) {
      result = "Ok";
    } else {
      result = "Error";
    }
  } else {
    LOGGER_SM << "ERROR**********\n";
    result = "Error";
    report = "HBase::getCoordinatesAndElementsFromMesh THRIFT could not scan.";
  }

  return result;
}
