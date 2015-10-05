

// STD
#include <sstream>

// VELaSSCo
#include "EDM.h"
#include "HBase.h"
#include "storageModule.h"

#include "cJSON.h"
#include "base64.h"


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

string storageModule::getStatusDB() {
	// HBase
	VELaSSCo::HBase hbasedb;

	// EDM
	VELaSSCo::EDM edmdb;


	VELaSSCo::AbstractDB &db = hbasedb;
	
	return db.getStatusDB();
}

string storageModule::getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					   const std::string &sessionID, const std::string &model_group_qualifier, 
					   const std::string &model_name_pattern) { 
  VELaSSCo::HBase hbasedb;   // HBase
  VELaSSCo::EDM edmdb;       // EDM
  VELaSSCo::AbstractDB &db = hbasedb;
  return db.getListOfModelNames( report, listOfModelNames, 
				 sessionID, model_group_qualifier, model_name_pattern);
}

string storageModule::getResultOnVertices( std::string sessionID,  std::string modelID,  std::string analysisID,  double timeStep,  std::string resultID,  std::string listOfVertices)
{ 
	// HBase
	VELaSSCo::HBase hbasedb;

	// EDM
	VELaSSCo::EDM edmdb;


	VELaSSCo::AbstractDB &db = hbasedb;
	
	return db.getResultOnVertices(sessionID, modelID, analysisID, timeStep, resultID, listOfVertices);
}

string storageModule::checkIfAllVerticesArePresent(string listOfVertices, string out)
{
    
    ///* To debug
     cout<< "-------------------------" << endl;
     cout << listOfVertices <<endl;
     cout<< "=========================" << endl;
     cout << out <<endl;
     cout<< "-------------------------" << endl;
     //*/
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
