
// CLib
#include <cmath>
#include <cstddef>

// STD
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
#include <curl/curl.h>
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
    DEBUG( "Connected to HBase on " << DB_hostname << ":" << DB_port << endl);
  } catch ( TException &tx) {
    DEBUG( "Opening HBase connection error: " << tx.what() << endl);
    connected = false;
  }
  return connected;
}

bool HBase::stopConnection() {
  bool disconnected = true;
  try {
    ( *_transport)->close();
    DEBUG( "Disconnected from HBase server" << endl);
  } catch ( TException &tx) {
    DEBUG( "Closing HBase connection error: " << tx.what() << endl);
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

std::string HBase::getResultOnVertices( const std::string &sessionID,  const std::string &modelID, 
					const std::string &analysisID, const double       timeStep,  
					const std::string &resultID,   const std::string &listOfVertices ) {
  return getResultOnVertices_curl( sessionID, modelID, analysisID, timeStep, resultID, listOfVertices );
  // return getResultOnVertices_thrift( sessionID, modelID, analysisID, timeStep, resultID, listOfVertices );
}

std::string HBase::getResultOnVertices_curl( const std::string &sessionID,
					     const std::string &modelID,
					     const std::string &analysisID,
					     const double       timeStep,
					     const std::string &resultID,
					     const std::string &listOfVertices )
{
  std::cout << "getResultOnVertices CURL: =====" << std::endl;
  std::cout << "S " << sessionID      << std::endl;
  std::cout << "M " << modelID        << std::endl;
  std::cout << "R " << resultID       << std::endl;
  std::cout << "A " << analysisID     << std::endl;
  std::cout << "V " << listOfVertices << std::endl;
  std::cout << "T " << timeStep       << std::endl;

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

	key << simulationID_hex
		<< analysisNameLength_hex
		<< analysisNameChars
		<< timeStep_hex
		<< partitionID_hex;

  cmd += key.str();
  cout << cmd << endl;

    
  CurlCommand do_curl;
  string buffer;
    
  bool ok = do_curl.Evaluate( buffer, cmd);

  std::cout << "**********\n";    
  std::cout << buffer << std::endl;
  std::cout << "**********\n";    
    
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

std::string HBase::getResultOnVertices_thrift( const std::string &sessionID,
					       const std::string &modelID,
					       const std::string &analysisID,
					       const double       timeStep,
					       const std::string &resultID,
					       const std::string &listOfVertices )
{
  std::cout << "getResultOnVertices THRIFT: =====" << std::endl;
  std::cout << "S " << sessionID      << std::endl;
  std::cout << "M " << modelID        << std::endl;
  std::cout << "R " << resultID       << std::endl;
  std::cout << "A " << analysisID     << std::endl;
  std::cout << "V " << listOfVertices << std::endl;
  std::cout << "T " << timeStep       << std::endl;

  string table_name = "Simulations_Data";
  string result;

  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  // TScan ts;
  // std::stringstream filter;
  // filter.str("");
  // ts.__set_filterString(filter.str());

  StrVec cols;
  cols.push_back( "M:"); // all qualifiers inside the M column family
  const char *ascii_model_id = "1dfa14ef887d15415d62d3489c4ce41f";
  char bin_row_key[ 20];
  assert( strlen( ascii_model_id) == 32);
  FromHexString( bin_row_key, 20, ascii_model_id, strlen( ascii_model_id));
  std::string base64_key = base64_encode( bin_row_key, 16);
  string start_row = base64_decode( base64_key); // decode base64 to binary string

  for ( int i = 0; i < 16; i++) {
    std::cout << ( int)bin_row_key[ i] << ", ";
  }
  std::cout << endl;
  for ( int i = 0; i < 16; i++) {
    std::cout << ( int)start_row.data()[ i] << ", ";
  }
  std::cout << endl;

  ScannerID scan_id = _hbase_client->scannerOpen( table_name, start_row, cols, m);
  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);
  bool scan_ok = true;
  try {
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
	break;
      // process rowsResult
      std::cout << "numberof rows = " << rowsResult.size() << endl;
      for ( size_t i = 0; i < rowsResult.size(); i++) {
	// convert to return type
	// FullyQualifiedModelName model_info;
	// bool ok = getModelInfoFromRow( model_info, rowsResult[ i]);
	// if ( ok) {
	//   listOfModelNames.push_back( model_info);
	// }
	printRow( rowsResult[ i]);
      }
    }
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    result = tmp.str();
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    result = tmp.str();
  } catch (...) {
    result = "other error";
  }
  _hbase_client->scannerClose( scan_id);

  std::cout << "**********\n";
  std::cout << result << endl;
  std::cout << "**********\n";

  return "Error";
  // cmd += "/";
  // std::stringstream key;
  // //key << "0x";
  // //key << modelID;
  // //key << analysisID;
  // //key << timeStep;
  // //key << resultID;
  // 
  // //key << "643934636132396265353334636131656435373865393031323362376338636544454d383030303031/M";
  // key << "&*"; // first row of Simulations_Data ingested by ATOS start with a 4, avoiding asking for ALL the table !
  // 
  // cmd += key.str();
  // cout << cmd << endl;
  // 
  //   
  // CurlCommand do_curl;
  // string buffer;
  //   
  // bool ok = do_curl.Evaluate( buffer, cmd);
  // 
  // std::cout << "**********\n";    
  // std::cout << buffer << std::endl;
  // std::cout << "**********\n";    
  //   
  // //
  // string result;
  // if(analysisID.find("DEM") >= 0)
  //   {
  //     result = parse1DEM(buffer, listOfVertices);
  //   }
  // else if(analysisID.find("FEM") >= 0)
  //   {
  //     cout << buffer << endl;
  //   }
  // 
  // return result;
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
       cJSON * elem  = cJSON_CreateObject();
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
		int64_t id = *((int64_t*)(&(tmp_cq[ 10 ])));

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
		
		int64_t id = *((int64_t*)(&(tmp_cq[ 10 ])));
		
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

std::string HBase::getCoordinatesAndElementsFromMesh(const std::string &sessionID, const std::string &modelID, const std::string &analysisID, const double timeStep, const int32_t partitionID) {
  return getCoordinatesAndElementsFromMesh_curl( sessionID, modelID, analysisID, timeStep, partitionID );		
  //return getCoordinatesAndElementsFromMesh_thrift( sessionID, modelID, analysisID, timeStep, resultID, coords, elements );		
}

std::string HBase::getCoordinatesAndElementsFromMesh_curl(const std::string &sessionID, const std::string &modelID, const std::string &analysisID, const double timeStep, const unsigned partitionID) {
  std::cout << "getCoordinatesAndElementsFromMesh CURL: =====" << std::endl;
  std::cout << "S " << sessionID      << std::endl;
  std::cout << "M " << modelID        << std::endl;
  //std::cout << "R " << resultID     << std::endl;
  std::cout << "A " << analysisID     << std::endl;
  std::cout << "T " << timeStep       << std::endl;
  std::cout << "P " << partitionID    << std::endl;

  TableModelEntry tableModel;
  if(getTableNames(sessionID, modelID, tableModel) == false)
    cout << "No table containing the model with " << modelID << "Model ID.\n";
  else
    cout << "Model Table: " << tableModel._list_models
         << "Meta Table: "  << tableModel._metadata
         << "Data Table: "  << tableModel._data 		<< endl; 

  string cmd = "http://" + _db_host + ":8880/";
  cmd += tableModel._data;
  cmd += "/";
  std::stringstream key;
	
  std::string simulationID_hex       = modelID; //"A0F5090200000000A8F5090200000000";//"f8514229cc2e34534b6e48e6d3e829ee";//"4d0e0c37be088e715de50d2230887579";
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
    
  bool ok = do_curl.Evaluate( buffer, cmd);

  std::cout << "**********\n";    
  // std::cout << buffer << std::endl;
  std::cout << "**********\n";    
    
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

std::string HBase::getCoordinatesAndElementsFromMesh_thrift(const std::string &sessionID, const std::string &modelID, const std::string &analysisID, const double timeStep, const unsigned partitionID) {
  std::cout << "getResultOnVertices THRIFT: =====" << std::endl;
  std::cout << "S " << sessionID      << std::endl;
  std::cout << "M " << modelID        << std::endl;
  //std::cout << "R " << resultID     << std::endl;
  std::cout << "A " << analysisID     << std::endl;
  std::cout << "T " << timeStep       << std::endl;
  std::cout << "T " << partitionID    << std::endl;

  string table_name = "Simulations_Data";
  string result;

  vector< TRowResult> rowsResult;
  std::map<std::string,std::string> m;
  // TScan ts;
  // std::stringstream filter;
  // filter.str("");
  // ts.__set_filterString(filter.str());

  StrVec cols;
  cols.push_back( "M:"); // all qualifiers inside the M column family
  const char *ascii_model_id = "1dfa14ef887d15415d62d3489c4ce41f";
  char bin_row_key[ 20];
  assert( strlen( ascii_model_id) == 32);
  FromHexString( bin_row_key, 20, ascii_model_id, strlen( ascii_model_id));
  std::string base64_key = base64_encode( bin_row_key, 16);
  string start_row = base64_decode( base64_key); // decode base64 to binary string

  for ( int i = 0; i < 16; i++) {
    std::cout << ( int)bin_row_key[ i] << ", ";
  }
  std::cout << endl;
  for ( int i = 0; i < 16; i++) {
    std::cout << ( int)start_row.data()[ i] << ", ";
  }
  std::cout << endl;

  ScannerID scan_id = _hbase_client->scannerOpen( table_name, start_row, cols, m);
  // ScannerID scan_id = _hbase_client.scannerOpenWithScan( table_name, ts, m);
  bool scan_ok = true;
  try {
    // or _hbase_client.scannerGetList( rowsResult, scan_id, 10);
    while ( true) {
      _hbase_client->scannerGet( rowsResult, scan_id);
      if ( rowsResult.size() == 0)
	break;
      // process rowsResult
      std::cout << "numberof rows = " << rowsResult.size() << endl;
      for ( size_t i = 0; i < rowsResult.size(); i++) {
	// convert to return type
	// FullyQualifiedModelName model_info;
	// bool ok = getModelInfoFromRow( model_info, rowsResult[ i]);
	// if ( ok) {
	//   listOfModelNames.push_back( model_info);
	// }
	printRow( rowsResult[ i]);
      }
    }
  } catch ( const IOError &ioe) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "IOError = " << ioe.what();
    result = tmp.str();
  } catch ( TException &tx) {
    scan_ok = false;
    std::stringstream tmp;
    tmp << "TException = " << tx.what();
    result = tmp.str();
  } catch (...) {
    result = "other error";
  }
  _hbase_client->scannerClose( scan_id);

  std::cout << "**********\n";
  std::cout << result << endl;
  std::cout << "**********\n";

  return "Error";
  // cmd += "/";
  // std::stringstream key;
  // //key << "0x";
  // //key << modelID;
  // //key << analysisID;
  // //key << timeStep;
  // //key << resultID;
  // 
  // //key << "643934636132396265353334636131656435373865393031323362376338636544454d383030303031/M";
  // key << "&*"; // first row of Simulations_Data ingested by ATOS start with a 4, avoiding asking for ALL the table !
  // 
  // cmd += key.str();
  // cout << cmd << endl;
  // 
  //   
  // CurlCommand do_curl;
  // string buffer;
  //   
  // bool ok = do_curl.Evaluate( buffer, cmd);
  // 
  // std::cout << "**********\n";    
  // std::cout << buffer << std::endl;
  // std::cout << "**********\n";    
  //   
  // //
  // string result;
  // if(analysisID.find("DEM") >= 0)
  //   {
  //     result = parse1DEM(buffer, listOfVertices);
  //   }
  // else if(analysisID.find("FEM") >= 0)
  //   {
  //     cout << buffer << endl;
  //   }
  // 
  // return result;
}
