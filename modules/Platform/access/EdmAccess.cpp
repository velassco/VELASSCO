#include <sstream>
#include <stddef.h>  // defines NULL

#include "EdmAccess.h"
#include "cJSON.h"
#include "Helpers.h"

// Global static pointer used to ensure a single instance of the class.
EdmAccess* EdmAccess::m_pInstance = NULL;


EdmAccess* EdmAccess::Instance()
{
    // Only allow one instance of class to be generated.
    if (!m_pInstance)
    {
        m_pInstance = new EdmAccess;
    }
    return m_pInstance;
}


bool EdmAccess::startConnection( const char *data_layer_hostname, const int data_layer_port)
{
	DEBUG( "Connecting to EDM SM at " << data_layer_hostname << ":" << data_layer_port);
	
	socket= boost::shared_ptr<TTransport>(new TSocket( data_layer_hostname, data_layer_port));
    transport = boost::shared_ptr<TTransport>(new TBufferedTransport(socket));
	protocol = boost::shared_ptr<TProtocol>(new TBinaryProtocol(transport));
	
	cli = new VELaSSCoSMClient(protocol);
	
	try
	{
		transport->open();
		DEBUG( "Connection established.\n");
	}
    catch (TException& tx)
    {
        cout << "ERROR: " << tx.what() << endl;
    }
    
    return true;		
	
}


bool EdmAccess::stopConnection()
{
    try
    {
        transport->close();
        DEBUG( "Connection to EDM closed");      
    } catch (TException& tx) {
        cout << "ERROR: " << tx.what() << endl;
    }
    return true;
}


void EdmAccess::getStatusDB( std::string& _return) {
    
    DEBUG( " Function EdmAccess::getStatusDB not implemented (QueryManager/EdmAccess.cpp).");
	return;
}


void EdmAccess::userLogin( std::string &_return, 
				 const std::string &name, const std::string &role, const std::string &password) {
				 
		DEBUG( "UserLogin with params: olav, myRole, myPassword");
		cli->UserLogin(_return, "olav", "myRole", "myPassword");
		DEBUG( "User logged on with session ID: " +  _return + "\n");	
}


void EdmAccess::userLogout( std::string &_return, const std::string &sessionID) {

    DEBUG( " Function EdmAccess::userLogout not implemented (QueryManager/EdmAccess.cpp).");
	return;
}


void EdmAccess::getListOfModels( rvGetListOfModels &_return,
				       const std::string &sessionID, 
				       const std::string &model_group_qualifier, const std::string &model_name_pattern) {
				       
    DEBUG( " Function EdmAccess::getListOfModels not implemented (QueryManager/EdmAccess.cpp).");
	return;
}


void EdmAccess::openModel( rvOpenModel &_return,
				    const std::string &sessionID, 
				    const std::string &unique_name, const std::string &requested_access) {

	cli->FindModel(_return, sessionID, unique_name, requested_access);
	DEBUG ("Openning modelID: " + _return.modelID);
	return;
}


void EdmAccess::closeModel( std::string &_return,
				  const std::string &sessionID, 
				  const std::string &modelID) {
				  
    DEBUG( " Function EdmAccess::closeModel not implemented (QueryManager/EdmAccess.cpp).");
	return;
}


void EdmAccess::getResultFromVerticesID( rvGetResultFromVerticesID &_return,
				  const std::string &sessionID, const std::string &modelID, 
				  const std::string &analysisID, const double timeStep,
				  const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID ){
				  
   //Code added by Ivan Cores from StorageModuleTest.cpp
   	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int startTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds

	//   rvGetResultFromVerticesID verticesResultRV;
	//   vector<int64_t> listOfVertices;

	//   listOfVertices.push_back(63327);
	//   listOfVertices.push_back(63699);
	//   listOfVertices.push_back(63707);
	//   listOfVertices.push_back(64285);
	//   listOfVertices.push_back(123400);

   DEBUG("GetResultFromVerticesID - " + modelID);
   cli->GetResultFromVerticesID(_return, sessionID, modelID, analysisID, timeStep, resultID, listOfVerticesID );
   	gettimeofday(&tp, NULL);
   	long int endTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
   printf("Elapsed time for GetResultFromVerticesID is %0.3f seconds\n", (endTime - startTime)/1000.0 );
   DEBUG("Return status: " + _return.status);
   if (strncmp(_return.status.data(), "Error", 5) == 0) {
      printf("Error message: \"%s\"\n", _return.report.data());
   } else {
	//Only for testing
      int nRes = 0;
      //printf("%s has %d results.\n", modelName, _return.n_result_records);

      for (vector<VELaSSCoSM::ResultOnVertex>::iterator resIter = _return.result_list.begin(); resIter != _return.result_list.end(); resIter++) {
         vector<double>::iterator valuesIter = resIter->value.begin();
         if (nRes++ > 16) break;
         printf("%10ld %0.3f\n", resIter->id, *valuesIter);
      }

      printf(_return.report.data());
	}
   
	return;
}


void EdmAccess::getCoordinatesAndElementsFromMesh( 
	rvGetCoordinatesAndElementsFromMesh& _return,
	const std::string& sessionID, const std::string& modelID, const std::string& analysisID, 
	const double timeStep, const MeshInfo& meshInfo )
{
  //Code added by Ivan Cores from StorageModuleTest.cpp
  
  DEBUG("GetCoordinatesAndElementsFromMesh - " + modelID);
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long int startTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
  cli->GetCoordinatesAndElementsFromMesh(_return, sessionID, modelID, analysisID, timeStep, meshInfo);
  gettimeofday(&tp, NULL);
  long int endTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
  printf("Elapsed time for GetCoordinatesAndElementsFromMesh is %0.3f seconds\n", (endTime - startTime)/1000.0 );
  if (strncmp(_return.status.data(), "Error", 5) == 0) {
    printf("Error message: \"%s\"\n", _return.report.data());
  } else {
    printf("Query report:\n%s\n\n", _return.report.data());
    //OLI
    printf("Number of vertices transferred: %12lu\n", _return.vertex_list.size());
    for (size_t i = 0; i < 20 && i < _return.vertex_list.size(); i++) {
      VELaSSCoSM::Vertex v = _return.vertex_list[i];
      printf("%12ld   %0.3f  %0.3f  %0.3f\n", v.id, v.x, v.y, v.z);
    }
    printf("Number of elements transferred: %12ld\n", _return.element_list.size());
    for (size_t i = 0; i < 20 && i < _return.element_list.size(); i++) {
      VELaSSCoSM::Element e = _return.element_list[i];
      printf("%12ld   ", e.id);
      for (size_t j = 0; j < _return.element_list[i].nodes_ids.size(); j++) printf("  %10ld", _return.element_list[i].nodes_ids[j]);
      printf("\n");
    }
  }
  return;
  
}
                  
void EdmAccess::getListOfMeshes( rvGetListOfMeshes &_return,
				       const std::string &sessionID,
				       const std::string &modelID,
				       const std::string &analysisID,
				       const double stepValue) {
				       
	//Code added by Ivan Cores from StorageModuleTest.cpp

   DEBUG("GetListOfMeshes - " + modelID);
  	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int startTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
   cli->GetListOfMeshes(_return, sessionID, modelID, analysisID, stepValue);
   	gettimeofday(&tp, NULL);
   	long int endTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
   printf("Elapsed time for GetListOfMeshes is %0.3f seconds\n", (endTime - startTime)/1000.0 );
   DEBUG("Return status: " + _return.status);
   //printf("%s has %d meshes.\n", modelID, _return.meshInfos.size());
   for (vector<VELaSSCoSM::MeshInfo>::iterator meshIter = _return.meshInfos.begin(); meshIter != _return.meshInfos.end(); meshIter++) {
      printf("   %20s  %10ld  %10ld\n", meshIter->name.data(), meshIter->nElements, meshIter->nVertices);
   }
   return;
}

void EdmAccess::getListOfAnalyses( rvGetListOfAnalyses &_return,
					 const std::string &sessionID,
					 const std::string &modelID) 
{
	//Code added by Ivan Cores from StorageModuleTest.cpp

   rvGetListOfTimeSteps rvTimesteps;

   DEBUG("GetListOfAnalyses - " + modelID);
  	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int startTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
   cli->GetListOfAnalyses(_return, sessionID, modelID);
   	gettimeofday(&tp, NULL);
   	long int endTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
   printf("Elapsed time for GetListOfAnalyses is %0.3f seconds\n", (endTime - startTime)/1000.0 );
   DEBUG("Return status: " + _return.status);
   //printf("%s has %d analyses.\n", modelID, _return.analyses.size());
   if (strncmp(_return.status.data(), "Error", 5) == 0) {
      printf("Error message: \"%s\"\n", _return.report.data());
   } else {
      printf("Execution report:\n%s\n\n\n", _return.report.data());
/*      for (vector<string>::iterator nameIter = _return.analyses.begin(); nameIter != _return.analyses.end(); nameIter++) {
         printf("Analysis name : %s\n", (char*)nameIter->data());
         cli->GetListOfTimeSteps(rvTimesteps, sessionID, modelID, *nameIter);
         printf("   %s has %d time steps:\n", (char*)nameIter->data(), rvTimesteps.time_steps.size());
         printf("   %s has the following time steps:\n", (char*)nameIter->data());
         int i = 0;
         for (vector<double>::iterator tsIter = rvTimesteps.time_steps.begin(); tsIter != rvTimesteps.time_steps.end(); tsIter++) {
            printf("%Lf\n", *tsIter);
         }
      }*/
   }
   return;
}


void EdmAccess::getListOfTimeSteps( rvGetListOfTimeSteps &_return,
					  const std::string &sessionID,
					  const std::string &modelID,
					  const std::string &analysisID,
					  const std::string &stepOptions, const int numSteps, const double *lstSteps) {
					  
    DEBUG( " Function EdmAccess::getListOfTimeSteps not implemented (QueryManager/EdmAccess.cpp).");
	return;
}


void EdmAccess::getListOfResultsFromTimeStepAndAnalysis( rvGetListOfResults &_return,
							       const std::string &sessionID,
							       const std::string &modelID,
							       const std::string &analysisID,
							       const double stepValue) {
	//Code added by Ivan Cores from StorageModuleTest.cpp
   
   DEBUG("GetListOfResultsFromTimeStepAndAnalysis - " + modelID);
  	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int startTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
   cli->GetListOfResultsFromTimeStepAndAnalysis(_return, sessionID, modelID, analysisID, stepValue);
   	gettimeofday(&tp, NULL);
   	long int endTime = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
   printf("Elapsed time for GetListOfResultsFromTimeStepAndAnalysis is %0.3f seconds\n", (endTime - startTime)/1000.0 );
   DEBUG("Return status: " + _return.status);
   //printf("%s has %d result headers.\n", modelID, _return.result_list.size());
   int nRes = 0;
   if (strncmp(_return.status.data(), "Error", 5) == 0) {
      printf("Error message: \"%s\"\n", _return.report.data());
   } else {
      for (vector<VELaSSCoSM::ResultInfo>::iterator resIter = _return.result_list.begin(); resIter != _return.result_list.end(); resIter++) {
         if (nRes++ > 16) break;
         printf("%20s %15s %15s %10d\n", (char*)resIter->name.data(), (char*)resIter->type.data(), (char*)resIter->location.data(), resIter->numberOfComponents);
         for (vector<string>::iterator compName = resIter->componentNames.begin(); compName != resIter->componentNames.end(); compName++) {
            printf("                                     %s\n", compName->data());
         }
      }
   }	
   return;					       

}


void EdmAccess::getListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
						 const std::string &sessionID, const std::string &modelID, 
						 const std::string &analysisID, const double stepValue, 
						 const int32_t meshID) {
    DEBUG( " Function EdmAccess::getListOfVerticesFromMesh not implemented (QueryManager/EdmAccess.cpp).");
	return;
}


/*void EdmAccess::stopAll()
{
    DEBUG( " Function EdmAccess::stopAll not implemented (QueryManager/EdmAccess.cpp).");
	return;
}*/




void EdmAccess::calculateBoundingBox( const std::string &sessionID, const std::string &modelID, const std::string &dataTableName,
			     const std::string &analysisID, const int numSteps, const double *lstSteps,
			     const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			     double *return_bbox, std::string *return_error_str) {
			     
	return;
}


void EdmAccess::calculateDiscrete2Continuum( const std::string &sessionID, const std::string &modelID,
				    const std::string &analysisName, const std::string &staticMeshID, const std::string &tSOptions, 
				    const int numSteps, const double *lstSteps, const std::string &coarseGrainingMethod, 
				    const double width, const double cutoffFactor,
				    const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGoptions, 
				    const double deltaT,
				    std::string *returnQueryOutcome, std::string *return_error_str) {
			     
	return;
}


// std::string EdmAccess::MRgetListOfVerticesFromMesh( rvGetListOfVerticesFromMesh &_return, 
// 				    const std::string &sessionID, const std::string &modelID, 
// 				    const std::string &dataTableName,
// 				    const std::string &analysisID, const double stepValue, 
// 				    const int32_t meshID) {
// 			     
// 	return NULL;
// }


void EdmAccess::calculateBoundaryOfAMesh( const std::string &sessionID, const std::string &DataLayer_sessionID,
				 const std::string &modelID, const std::string &dataTableName,
				 const int meshID, const std::string &elementType,
				 const std::string &analysisID, const double stepValue,
				 std::string *return_binary_mesh, std::string *return_error_str) {
			     
	return;
}
    



