#include <sstream>
#include <stddef.h>  // defines NULL


#include "storageModule.h"
#include "cJSON.h"

// Global static pointer used to ensure a single instance of the class.
storageModule* storageModule::m_pInstance = NULL;


storageModule* storageModule::Instance()
{
    // Only allow one instance of class to be generated.
    if (!m_pInstance)
    {
        m_pInstance = new storageModule;
    }
    return m_pInstance;
}

double storageModule::fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

string storageModule::parse1DEM(string b,   std::string LOVertices)
{
    cout<< "List Of vertices : "<< LOVertices <<endl<<endl;
    cJSON *sentListOfVertices = cJSON_GetObjectItem(cJSON_Parse(LOVertices.c_str()), "id");
    
    cJSON *_return = cJSON_CreateObject();
    cJSON *listOfVertices = cJSON_CreateArray();
    cJSON_AddItemToObject(_return, "vertices", listOfVertices);
    
    cJSON *json = cJSON_Parse(b.c_str());
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
            cout<< "key : "<< key <<endl;
            cJSON *cellsJ = cJSON_GetArrayItem (keyC, 1);
            
            
            cJSON * elem  = cJSON_CreateObject();
            
            for (int k = 0; k < cJSON_GetArraySize(cellsJ); k++ )
            {
                cJSON *val = cJSON_GetArrayItem (cellsJ, k);
                
                cJSON *elem = cJSON_GetArrayItem (val, 0);
                string tmp = base64_decode(elem->valuestring);
                cout << "\t"<<tmp <<endl;
                if(tmp.find("M:cid") == 0)
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int id = 0;
                    for (int n = 0; n < tmp.size() ; n ++)
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
                    int id = 0;
                    int N = 3;
                    double xyz[N];
                    for(int b = 0; b < N; b++)
                        xyz[b] = 0.0;
                    int coma = 0;
                    int sign = 1;
                    for (int n = 0; n < tmp.size() ; n ++)
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
                    
                    //  cout << tmp<<" M:cxyz :"<< base64_decode(elem->valuestring)<< endl;
                    
                }
                else if(tmp.find("M:mcn") == 0)
                {
                    elem = cJSON_GetArrayItem (val, 2);
                    tmp = base64_decode(elem->valuestring);
                    cout << "\t\t"<<tmp <<endl;
                    int val = 0;
                    int id = 0;
                    for (int n = 0; n < tmp.size() ; n ++)
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
                    int id = 0;
                    for (int n = 0; n < tmp.size() ; n ++)
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
                    int id = 0;
                    for (int n = 0; n < tmp.size() ; n ++)
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
                    int id = 0;
                    int N = 1;
                    double xyz[N] ;
                    for(int b = 0; b < N; b++)
                        xyz[b] = 0.0;
                    int coma = 0;
                    int sign = 1;
                    for (int n = 0; n < tmp.size() ; n ++)
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
                    int id = 0;
                    for (int n = 0; n < tmp.size() ; n ++)
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
                    int id = 0;
                    for (int n = 0; n < tmp.size() ; n ++)
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
                    cout << tmp <<" Error "<< base64_decode(elem->valuestring)<< endl;
                    
                }
                
                //cout << cJSON_Print(value) << endl;
                //                cout << base64_decode(cJSON_GetArrayItem(value, 0)->valuestring) << endl;
                //cout <<"------------"<<endl;
                
                
            }
        }
        exit(0);
    }
    for (int k = 0; k < velem.size(); k++)
    {
        if (enableVElem.at(k))
            cJSON_AddItemToArray( listOfVertices, velem.at(k));
    }
    return cJSON_Print(_return);
}

string storageModule::getResultOnVertices( std::string sessionID,  std::string modelID,  std::string analysisID,  double timeStep,  std::string resultID,  std::string listOfVertices)
{
    
    string cmd = "http://pez001:8880/";
    cmd += "Simulations_Data";
    cmd += "/";
    std::stringstream key;
    key << "0x";
    key << modelID;
    key << analysisID;
    key << timeStep;
    key << resultID;
    key << "*";
    
    cmd += key.str();
    //        cout << cmd << endl;
    CURL *curl = NULL;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl)
    {
        res = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        if(res != CURLE_OK)
        {
            fprintf(stderr, "CURLOPT_ERRORBUFFER failed: %s\n", curl_easy_strerror(res));
        }
        
        res = curl_easy_setopt(curl, CURLOPT_URL, cmd.c_str());
        if(res != CURLE_OK)
        {
            fprintf(stderr, "CURLOPT_URL failed: %s\n", curl_easy_strerror(res));
        }
        
        res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        if(res != CURLE_OK)
        {
            fprintf(stderr, "CURLOPT_FOLLOWLOCATION failed: %s\n", curl_easy_strerror(res));
        }
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        if(res != CURLE_OK)
        {
            fprintf(stderr, "CURLOPT_WRITEFUNCTION failed: %s\n", curl_easy_strerror(res));
        }
        
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        if(res != CURLE_OK)
        {
            fprintf(stderr, "CURLOPT_WRITEDATA failed: %s\n", curl_easy_strerror(res));
        }
        
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Accept: application/json;");
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        if(res != CURLE_OK)
        {
            fprintf(stderr, "CURLOPT_HTTPHEADER failed: %s\n", curl_easy_strerror(res));
        }
        
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        
        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);
        
        
    }
    
    
    //
    stringstream  result;
    if(analysisID.find("DEM") >= 0)
    {
        result << parse1DEM(buffer, listOfVertices);
    }
    else if(analysisID.find("FEM") >= 0)
    {
        cout << buffer <<endl;
    }
    return result.str();
}

string storageModule::checkIfAllVerticesArePresent(string listOfVertices, string out)
{
    
    /* To debug
     cout<< "-------------------------" << endl;
     cout << listOfVertices <<endl;
     cout<< "=========================" << endl;
     cout << out <<endl;
     cout<< "-------------------------" << endl;
     */
    cJSON *jsonListOfVertices = cJSON_Parse(listOfVertices.c_str());
    jsonListOfVertices = cJSON_GetArrayItem(jsonListOfVertices, 0);
    
    cJSON *jsonOut = cJSON_Parse(out.c_str());
    
    stringstream error;
    bool firstMissingElement = false;
    
    
    jsonOut = cJSON_GetArrayItem(jsonOut, 0);
    int elementIdi;
    
    cJSON *elementJ;
    
    error <<"{";
    error << "\"Message\": \"Missing elements\",";
    error << "\"value\": [";
    int j = 0;
    for (int i = 0; i < cJSON_GetArraySize(jsonListOfVertices) ; i++)
    {
        
        j = 0;
        elementIdi = cJSON_GetArrayItem(jsonListOfVertices, i)->valueint;
        
        bool test = false;
        while (j < cJSON_GetArraySize(jsonOut) && !test)
        {
            elementJ = cJSON_GetArrayItem(jsonOut, j);
            // cout << "\t" << cJSON_GetObjectItem(elementJ,"id")->valueint << " "  <<elementIdi <<endl;
            if(cJSON_GetObjectItem(elementJ,"id")->valueint == elementIdi)
            {
                test = true;
            }
            else
            {
                j++;
            }
            
        }
        
        if (!test )
        {
            if(firstMissingElement)
            {
                error << ", ";
            }
            else
            {
                firstMissingElement = true;
            }
            error << elementIdi;
        }
    }
    
    error <<"]}";
    jsonOut = cJSON_Parse(out.c_str());
    cJSON_AddItemReferenceToObject (
                                    jsonOut,
                                    "Error",
                                    cJSON_Parse(error.str().c_str())
                                    );
    return cJSON_Print(jsonOut);
}
