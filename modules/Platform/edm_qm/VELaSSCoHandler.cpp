
#include "stdafx.h"

#ifdef _WINDOWS
#include <direct.h> 
#endif

#include "VELaSSCoMethods.h"

//using namespace  ::dli;

#include "EDM_interface.h"
#include "CLogger.h"
#include "VELaSSCo_Operations.h"
#include "VELaSSCoHandler.h"
#include <time.h>

#ifndef _WINDOWS
#include "../edm_qm/WindowsFunctionsForLinux.h"
#endif

/* ================================================================================================
string	statusDL(),
rvGetListOfModels GetListOfModelNames(1: string sessionID,
rvOpenModel FindModel(1: string sessionID,
-->   string	GetResultFromVerticesID(1: string sessionID,
-->   string 	GetCoordinatesAndElementsFromMesh(
-->   void stopAll()
rvGetListOfMeshes GetListOfMeshes(
rvGetListOfAnalyses GetListOfAnalyses(
rvGetListOfTimeSteps GetListOfTimeSteps(
rvGetListOfResults GetListOfResultsFromTimeStepAndAnalysis(
string	GetResultFormVerticesID(1: string sessionID,
string UserLogin(
string UserLogout(
string CloseModel(
string SetThumbnailOfAModel(
rvGetThumbnailOfAModel GetThumbnailOfAModel(
rvGetElementOfPointsInSpace GetElementOfPointsInSpace(
rvGetBoundaryOfLocalMesh GetBoundaryOfLocalMesh(
================================================================================================ */

/**
* VELaSSCoHandler constructor
*
*/
VELaSSCoHandler::VELaSSCoHandler() : VELaSSCo_Operations()
{
   int rstat;
   thelog = NULL;

   char *env = getenv("QUERY_RESULT_FOLDER");
   if (env) {
      QUERY_RESULT_FOLDER = (char*)malloc(strlen(env) + 2); strcpy(QUERY_RESULT_FOLDER, env);
      char *ec = QUERY_RESULT_FOLDER + strlen(env) - 1;
      if (*ec == '\\') {
         *ec = '/';
      } else if (*ec != '/') {
         strcat(QUERY_RESULT_FOLDER, "/");
      }
      rstat = _mkdir(QUERY_RESULT_FOLDER);
   }
}


void gen_random(char *s, const int len) {
   static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
   for (int i = 0; i < len - 1; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum)-1)];
   }
   s[len - 1] = 0;
}

/**
* Return the status of the different services
* which run on the Data Layer.
*/
void VELaSSCoHandler::statusDL(std::string& _return)
{
   thelog->logg(0, "-->statusDL\n\n");
   _return = "OK";
   thelog->logg(0, "status=OK\n\n");
}

/**
* returns a session if if the user exists with the specified password and the specified role or an empty role.
*/
void VELaSSCoHandler::userLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password)
{
   char sessionID[64];
   thelog->logg(3, "-->UserLogin\nuser_name=%s\nrole=%s\npassword=%s\n\n", user_name.data(), role.data(), password.data());
   gen_random(sessionID, sizeof(sessionID)); _return = sessionID;
}

/**
* Stop access to the system by a given session id and release all resources held by that session
*
* @param sessionID
*/
void VELaSSCoHandler::userLogout(std::string& _return, const std::string& sessionID)
{
   thelog->logg(1, "-->UserLogout\nsessionID=%s\n", sessionID.data());
}

/**
* Return the status of the different services
* which run on the Data Layer.
* @return string - returns a structured list of avialbe vertices,
* with the attached list of double
* if errors occur the contect is also returned here?
*/
void VELaSSCoHandler::getResultFromVerticesID(rvGetResultFromVerticesID& rv, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::vector<int64_t> & listOfVertices)
{
   int startTime = GetTickCount();
   VELaSSCoMethods theQuery(theCluster, thelog);
   try {
      thelog->logg(2, "-->GetResultFromVerticesID\nmodelID=%s\n", modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         theQuery.GetResultFromVerticesID(rv, analysisID, timeStep, resultID, listOfVertices);
      } else {
         char *emsg = "Model does not exist.";
         rv.__set_status("Error"); rv.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      rv.__set_status("Error"); rv.__set_report(errMsg);
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}


//struct rvGetCoordinatesAndElementsFromMesh {
//   1: string status
//   2 : string report
//   3 : list<Vertex>        vertex_list
//   4 : list<Element>       element_list
//   5 : list<ElementAttrib> element_attrib_list
//   6 : list<ElementGroup>  element_group_info_list
//}
///**
//Return the coordinates and elements of a model's mesh.
//@return string - returns a structured list of vertices and elements of a model's mesh.
//if errors occur the contect is also returned here?
//*/
//rvGetCoordinatesAndElementsFromMesh 	GetCoordinatesAndElementsFromMesh(
//   1: string   sessionID,
//   2 : string   modelID,
//   3 : string   analysisID,
//   4 : double   timeStep,
//   5 : MeshInfo meshInfo),

void VELaSSCoHandler::getCoordinatesAndElementsFromMesh(rvGetCoordinatesAndElementsFromMesh& rv, const std::string& sessionID, const std::string& modelID,
   const std::string& analysisID, const double timeStep, const MeshInfo& meshInfo)
{
   int startTime = GetTickCount();
   VELaSSCoMethods theQuery(theCluster, thelog);
   try {
      thelog->logg(2, "-->GetCoordinatesAndElementsFromMesh\nmodelID=%s\n", modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         theQuery.GetCoordinatesAndElementsFromMesh(rv, analysisID, timeStep);
      } else {
         char *emsg = "Model does not exist.";
         rv.__set_status("Error"); rv.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      rv.__set_status("Error"); rv.__set_report(errMsg);
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}

/**
* Stop Data Layer
*/
void VELaSSCoHandler::stopAll()
{
   //--------------------->
}



char *VELaSSCoHandler::getErrorMsg(CedmError *e)
{
   if (e->message) {
      strcpy(errmsg, e->message);
   } else {
      strcpy(errmsg, edmiGetErrorText(e->rstat));
   }
   delete e;
   ReportError(errmsg);
   return errmsg;
}
char *VELaSSCoHandler::getErrorMsg(int rstat)
{
   strcpy(errmsg, edmiGetErrorText(rstat));
   ReportError(errmsg);
   return errmsg;
}

/*=============================================================================================================================*/
void VELaSSCoHandler::calculateBoundaryOfAMesh(const std::string &sessionID,
      const std::string &modelID,
      const int meshID, const std::string &elementType,
      const std::string &analysisID, const double stepValue,
      std::string *return_binary_mesh, std::string *return_error_str)
/*=============================================================================================================================*/
{
   int startTime = GetTickCount();
   VELaSSCoMethods theQuery(theCluster, thelog);
   try {
      thelog->logg(2, "-->calculateBoundaryOfAMesh\nmodelID=%s\n", modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         bool dataOnFile = false;
         char *bomFileName = getResultFileName("boundary_of_mesh_", modelID.data());
         FILE *bomfp = fopen(bomFileName, "r");
         if (bomfp) {
            fseek(bomfp, 0L, SEEK_END);
            EDMULONG filesz = ftell(bomfp); fclose(bomfp);
            bomfp = fopen(bomFileName, "rb");
            if (bomfp) {
               return_binary_mesh->resize(filesz);
               size_t nDoubleRead = fread((void*)return_binary_mesh->data(), 1, filesz, bomfp);
               fclose(bomfp); dataOnFile = true;
            }
         }
         if (! dataOnFile) {
            theQuery.calculateBoundaryOfLocalMesh(meshID, elementType, analysisID, stepValue, getResultFileName("getCoordinate", modelID.data()),
               return_binary_mesh, return_error_str);
            if (return_error_str->size() == 0) {
               bomFileName = getResultFileName("boundary_of_mesh_", modelID.data());
               bomfp = fopen(bomFileName, "wb");
               if (bomfp) {
                  size_t nDoubleWritten = fwrite(return_binary_mesh->data(), 1, return_binary_mesh->size(), bomfp);
                  fclose(bomfp);
               }
            }
         }
      } else {
         *return_error_str = "Model does not exist.";
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      *return_error_str = errMsg;
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}
///*=============================================================================================================================*/
//void VELaSSCoHandler::getBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& rv, const std::string& sessionID,
//   const std::string& modelID, const std::string& meshID, const std::string& analysisID, const double time_step)
///*=============================================================================================================================*/
//{
//   VELaSSCoMethods theQuery(theCluster, thelog);
//   try {
//      thelog->logg(2, "-->GetBoundaryOfLocalMesh\nmodelID=%s\n", modelID.data());
//      setCurrentSession(sessionID.data());
//      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
//         theQuery.GetBoundaryOfLocalMesh(rv, analysisID, time_step, meshID);
//      } else {
//         char *emsg = "Model does not exist.";
//         rv.__set_status("Error"); rv.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
//      }
//   } catch (CedmError *e) {
//      string errMsg;
//      handleError(errMsg, e);
//      rv.__set_status("Error"); rv.__set_report(errMsg);
//   }
//}




/**
* Returns a list of names of data sets that are available from the VELaSSCo platform
* and - optionally - their properties.
*
* @param sessionID
* @param groupQualifier
* @param modelNamePattern
* @param options
*/
//void GetListOfModelNames             (rvGetListOfModels& _return, const std::string& sessionID, const std::string& model_group_qualifier, const std::string& model_name_pattern);
#define nOfResults 1
void VELaSSCoHandler::getListOfModels(rvGetListOfModels& _return, const std::string& sessionID, const std::string& groupQualifier, const std::string& modelNamePattern)
{
   int startTime = GetTickCount();
   char *modelName, *repositoryName = "", *schemaName;
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance), index = 0, numberOfHits = 1, nModels = 0;
   SdaiInstance resultBuffer[nOfResults];
   VELaSSCoSM::FullyQualifiedModelName fqmn;
   std::vector<VELaSSCoSM::FullyQualifiedModelName>  infoList;
   char nameBuf[2048], condition[1024];
   EdmiError  rstat;

   try {
      thelog->logg(3, "-->GetListOfModelNames\ngroupQualifier=%s\nmodelNamePattern=%s\n\n", groupQualifier.data(), modelNamePattern.data());

      sprintf(condition, "(name like '%s*') and (belongs_to.name like '%s*')", modelNamePattern.data(), groupQualifier.data());
      do {
         if (rstat = edmiSelectInstancesBN(theCluster->clusterModel->modelId, "ClusterModel", condition, 0,
            maxBufferSize, &index, &numberOfHits, resultBuffer)) {
            _return.__set_status("Error"); _return.__set_report(getErrorMsg(rstat));
            break;
         } else if (numberOfHits > 0) {
            tEdmiInstData cmd;
            ecl::ClusterModel cm(theCluster->clusterModel, theCluster->clusterModel->initInstData(ecl::et_ClusterModel, &cmd));
            for (int i = 0; i < numberOfHits; i++) {
               cm.setInstanceId(resultBuffer[i]);
               modelName = cm.get_name();
               if (modelName) {
                  ecl::ClusterRepository *cr = cm.get_belongs_to();
                  if (cr) repositoryName = cr->get_name();
                  fqmn.__set_name(modelName);
                  sprintf(nameBuf, "%s.%s", repositoryName ? repositoryName : "", modelName); fqmn.__set_full_path(nameBuf);
                  sprintf(nameBuf, "%llu", resultBuffer[i]); fqmn.__set_modelID(nameBuf);
                  infoList.push_back(fqmn); nModels++;
               }
            }
         }
         index++;
      } while (numberOfHits == nOfResults);
      _return.__set_status("OK");
      thelog->logg(1, "status=OK\nNumber of models=%llu\n\n\n", nModels);
      _return.__set_models(infoList);
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return.__set_status("Error"); _return.__set_report(errMsg);
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}


SdaiInstance VELaSSCoHandler::getClusterModelID(const char *repName, const char *modelName, EDMLONG *rstatp, SdaiInteger *nOfNameMatches)
{
   char condition[1024];
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance)* 2, index = 0;
   SdaiInstance resultBuffer[2];

   *nOfNameMatches = 2;
   sprintf(condition, "(name like '%s*') and (belongs_to.name = '%s')", modelName, repName);
   if ((*rstatp = edmiSelectInstancesBN(theCluster->clusterModel->modelId, "ClusterModel", condition, 0,
      maxBufferSize, &index, nOfNameMatches, resultBuffer)) == OK && *nOfNameMatches == 1) {
      return resultBuffer[0];
   } else {
      if (*rstatp == 0 && *nOfNameMatches == 2) {
         index = 0;
         sprintf(condition, "(name = '%s') and (belongs_to.name = '%s')", modelName, repName);
         if ((*rstatp = edmiSelectInstancesBN(theCluster->clusterModel->modelId, "ClusterModel", condition, 0,
            maxBufferSize, &index, nOfNameMatches, resultBuffer)) == OK && *nOfNameMatches == 1) {
            return resultBuffer[0];
         }
      }
      return 0;
   }
}

/**
* Returns a model GUID (from now on ModelID). The model host may do housekeeping actions,
* such as caching, and update its session model accordingly..
*
* @param sessionID
* @param modelName
* @param requestedAccess
*/
void VELaSSCoHandler::openModel(rvOpenModel& _return, const std::string& sessionID, const std::string& modelName, const std::string& requestedAccess)
{
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance) * 2, index = 0, numberOfHits = 2;
   SdaiInstance sdaiModelID;
   bool notFound = true;
   EDMLONG rstat;
   int startTime = GetTickCount();

   try {
      thelog->logg(3, "-->openModel\nmodelName=%s\nrequestedAccess=%s\n\n", modelName.data(), requestedAccess.data());
      sdaiModelID = getClusterModelID("DataRepository", modelName.data(), &rstat, &numberOfHits);
      if (rstat) {
         _return.__set_modelID("0"); _return.__set_report(getErrorMsg(rstat));
      } else if (numberOfHits == 0) {
         _return.__set_modelID("0"); _return.__set_report("No model match the given model name pattern");
      } else if (numberOfHits == 1) {
         char smodelID[512];
         sprintf(smodelID, "%llu", sdaiModelID);
         _return.__set_modelID(smodelID); _return.__set_report("");
      }
      if (strEQL(_return.modelID.data(), "0")) {
         thelog->logg(1, "status=Error\nerror report=%s\n\n", _return.report.data());
      } else {
         thelog->logg(1, "status=OK\nmodelID=%s\n\n", _return.modelID.data());
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return.__set_status("Error"); _return.__set_report(errMsg);
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}

/**
* Description: Removes the possibility to access a model via a previously assigned
* GUID (OpenModel). Corresponding housekeeping is wrapped up.
*
* @param sessionID
* @param modelName
*/
void VELaSSCoHandler::closeModel(std::string& _return, const std::string& sessionID, const std::string& modelName)
{

   try {


      _return = "CloseModel" + string(" is not implemented");
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return = errMsg;
   }
}


/**
* Description: Store a new thumbnail of a model
*
* @param sessionID
* @param modelID
* @param imageFile
*/
void VELaSSCoHandler::setThumbnailOfAModel(std::string& _return, const std::string& sessionID, const std::string& modelID, const std::string& imageFile)
{
   try {
      _return = "setThumbnailOfAModel" + string(" is not implemented");
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return = errMsg;
   }

}

/**
* Description: Return thumbnail of a model.
*
* @param sessionID
* @param modelID
*/
void VELaSSCoHandler::getThumbnailOfAModel(rvGetThumbnailOfAModel& _return, const std::string& sessionID, const std::string& modelID)
{
   try {
      _return.__set_status("Error"); _return.__set_report("getThumbnailOfAModel" + string(" is not implemented"));
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return.__set_status("Error"); _return.__set_report(errMsg);
   }
}

/**
* Retrieves the list of time steps for a given model and analysis.
*
* @param sessionID
* @param modelID
*/
void VELaSSCoHandler::getListOfAnalyses(rvGetListOfAnalyses& _return, const std::string& sessionID, const std::string& modelID)
{
   int startTime = GetTickCount();
   VELaSSCoMethods theQuery(theCluster, thelog);
   try {
      thelog->logg(2, "-->getListOfAnalyses\nmodelID=%s\n", modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         bool dataInFile = false;
         char *analysisesFileName = getResultFileName("analysis_names_", modelID.data());
         FILE *analysisesFp = fopen(analysisesFileName, "r");
         if (analysisesFp) {
            vector<string> names;
            char name[2048];
            while (fgets(name, sizeof(name), analysisesFp)) {
               names.push_back(name);
            }
            fclose(analysisesFp);
            _return.__set_analyses(names);
            _return.__set_report(""); _return.__set_status("OK");
         }
         if (! dataInFile) {
            theQuery.GetListOfAnalyses(_return);
            if (strEQL(_return.status.data(), "OK")) {
               analysisesFp = fopen(analysisesFileName, "w");
               if (analysisesFp) {
                  for (vector<string>::iterator nameIter = _return.analyses.begin(); nameIter != _return.analyses.end(); nameIter++) {
                     fprintf(analysisesFp, "%s\n", (char*)nameIter->data());
                  }
                  fclose(analysisesFp);
               }
            }
         }
      } else {
         char *emsg = "Model does not exist.";
         _return.__set_status("Error"); _return.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return.__set_status("Error"); _return.__set_report(errMsg);
   }
}

/**
* Description: Given a session id, model id and a list of vertices ID, compute the axis
* aligned Bounding Box (BBox) of the model. For dynamic meshes, a time-step option and
* a list of time-steps need also to be provided as input parameters. Based on the values
* of these two input parameters, the BBox is calculated considering a single time-step,
* all time-steps, an interval of time-step or a set of time-steps. If list of vertices
* I'ds are empty, then all vertices from the model are considered.
*
* @param sessionID
* @param modelID
* @param verticesID
* @param analysisID
* @param timeStepOption
* @param timeSteps
*/
void VELaSSCoHandler::calculateBoundingBox(const std::string &sessionID, const std::string &modelID, //const std::string &dataTableName,
   const std::string &analysisID, const int numSteps, const double *lstSteps,
   const int64_t numVertexIDs, const int64_t *lstVertexIDs,
   double *return_bbox, std::string *return_error_str)
{
   int startTime = GetTickCount();
   VELaSSCoMethods theQuery(theCluster, thelog);
   try {
      char *emsg = NULL;
      thelog->logg(2, "-->calculateBoundingBox\nmodelID=%s\n", modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         char *bbFileName;
         bool dataOnFile = false;
         if (numVertexIDs == 0 ) {
            bbFileName = getResultFileName("bounding_box_", modelID.data());
            FILE *bbfp = fopen(bbFileName, "rb");
            if (bbfp) {
               size_t nDoubleRead = fread(return_bbox, sizeof(double), 6, bbfp);
               if (nDoubleRead == 6) dataOnFile = true;
               fclose(bbfp);
            }
         }
         if (! dataOnFile) {
            theQuery.calculateBoundingBox(analysisID, numSteps, lstSteps, numVertexIDs, lstVertexIDs, return_bbox, return_error_str);
            if (numVertexIDs == 0 ) {
               bbFileName = getResultFileName("bounding_box_", modelID.data());
               FILE *bbfp = fopen(bbFileName, "wb");
               if (bbfp) {
                  size_t nDoubleWritten = fwrite(return_bbox, sizeof(double), 6, bbfp);
                  fclose(bbfp);
               }
            }
         }
      } else {
         char *emsg = "Error - Model does not exist.";
         *return_error_str = emsg;
         thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      delete e;
      *return_error_str = errMsg;
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}


FILE *VELaSSCoHandler::getStoredQueryResult(const char *p1, const char *p2, const char *p3)
{
   return NULL;
}


/**
* Retrieves the list of time steps for a given model and analysis.
*
* @param sessionID
* @param modelID
* @param analysisID
*/
void VELaSSCoHandler::getListOfTimeSteps(rvGetListOfTimeSteps& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const std::string &stepOptions, const int numSteps, const double *lstSteps)
{
   int startTime = GetTickCount();
   VELaSSCoMethods theQuery(theCluster, thelog);
   try {
      thelog->logg(2, "-->GetListOfAnalyses\nmodelID=%s\n", modelID.data());
      setCurrentSession(sessionID.data());
      FILE *storedQueryResult = getStoredQueryResult("getListOfTimeSteps", analysisID.data(), NULL);
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         theQuery.GetListOfTimeSteps(_return, analysisID);
      } else {
         char *emsg = "Model does not exist.";
         _return.__set_status("Error"); _return.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return.__set_status("Error"); _return.__set_report(errMsg);
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}

/**
* Retrieves the list of results for a given model, analysis and step-value
* as of OP-22.115
*
* @param sessionIDGetListOfResultsFromTimeStepAndAnalysis
* @param modelID
* @param analysisID
* @param stepValue
*/
void VELaSSCoHandler::getListOfResultsFromTimeStepAndAnalysis(rvGetListOfResults& rv, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double stepValue)
{
   int startTime = GetTickCount();
   VELaSSCoMethods theQuery(theCluster, thelog);
   try {
      thelog->logg(2, "-->GetListOfResultsFromTimeStepAndAnalysis\nmodelID=%s\n", modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         theQuery.GetListOfResultsFromTimeStepAndAnalysis(rv, analysisID, stepValue);
      } else {
         char *emsg = "Model does not exist.";
         rv.__set_status("Error"); rv.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      rv.__set_status("Error"); rv.__set_report(errMsg);
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}

/**
 * Extract a list of vertices from the open model and the selected meshID
 * as of OP-22.116
 *
 * @param sessionID
 * @param modelID
 * @param analysisID
 * @param stepValue
 * @param meshID
 */
void VELaSSCoHandler::getListOfVerticesFromMesh(rvGetListOfVerticesFromMesh& rv, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double stepValue, const int32_t meshID)
{
   int startTime = GetTickCount();
   VELaSSCoMethods theQuery(theCluster, thelog);
   try {
      thelog->logg(2, "-->getListOfVerticesFromMesh\nmodelID=%s\n", modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         theQuery.GetListOfVerticesFromMesh(rv, analysisID, stepValue,meshID);
      } else {
         char *emsg = "Model does not exist.";
         rv.__set_status("Error"); rv.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      rv.__set_status("Error"); rv.__set_report(errMsg);
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}

/**
 * Extract a list of selected vertices from the open model and the selected meshID
 *
 * @param sessionID
 * @param modelID
 * @param analysisID
 * @param stepValue
 * @param meshID
 * @param listOfVerticesID
 */
void VELaSSCoHandler::getListOfSelectedVerticesFromMesh(rvGetListOfVerticesFromMesh& rv, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double stepValue, const int32_t meshID, const std::vector<int64_t> &listOfVerticesID)
{
   try {
      rv.status = "Error"; rv.report = "getListOfSelectedVerticesFromMesh" + string(" is not implemented");
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      rv.status = "Error"; rv.report = errMsg;
   }
}

/**
* Returns a list of meshes present for the given time-step of that analysis.
* If analysis == "" and step-value == -1 then the list will be of the 'static' meshes.
* If analysis != "" and step-value != -1 then the list will be of the 'dynamic' meshes
* that are present on that step-values of that analysis.
*
* @param sessionID
* @param modelID
* @param analysisID
* @param timeStep
*/


void VELaSSCoHandler::getListOfMeshes(rvGetListOfMeshes& rv, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep)
{
   int startTime = GetTickCount();
   VELaSSCoMethods theQuery(theCluster, thelog);
   try {
      thelog->logg(2, "-->GetListOfMeshes\nmodelID=%s\n", modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         theQuery.GetListOfMeshes(rv, analysisID, timeStep);
      } else {
         char *emsg = "Model does not exist.";
         rv.__set_status("Error"); rv.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      rv.__set_status("Error"); rv.__set_report(errMsg);
   }
   thelog->logg(1, "Execution time=%d millisec.\n\n", GetTickCount() - startTime);
}

/*=============================================================================================================================*/
void VELaSSCoHandler::handleError(string &errMsg, CedmError *e)
/*=============================================================================================================================*/
{
   int rstat = e->rstat;
   if (e->message) {
      errMsg = e->message;
   } else {
      errMsg = edmiGetErrorText(rstat);
   }
   delete e;
   thelog->logg(1, "status=Error\nError message=%s\n\n", errMsg.data());
}

/*=============================================================================================================================*/
void VELaSSCoHandler::ReportError(char *f)
/*=============================================================================================================================*/
{
   if (thelog) {
      thelog->logg(0, f);
   } else {
      printf("%s\n", f);
   }
}

/*=============================================================================================================================*/
void VELaSSCoHandler::printLogHeader()
/*=============================================================================================================================*/
{
   if (thelog) {
      time_t now = time(NULL);
      struct tm *t = localtime(&now);
      thelog->logg(5, "EDNqueryManager started %02d.%02d.%04d %02d:%02d\n\n", t->tm_mday, t->tm_mon+1, t->tm_year + 1900, t->tm_hour,t->tm_min);
      theCluster->printClusterInfo(thelog);
      thelog->logg(0, "\n\n\n");
   }
}

/*=============================================================================================================================*/
char *VELaSSCoHandler::getResultFileName(char *fileName, const char *modelId)
/*
   QUERY_RESULT_FOLDER must end with '/' 
===============================================================================================================================*/
{
   if (QUERY_RESULT_FOLDER && *QUERY_RESULT_FOLDER) {
      sprintf(resultFolderBuffer, "%s%s_%s.dat", QUERY_RESULT_FOLDER, modelId, fileName);
      return resultFolderBuffer;
   }
   THROW("Path not found in VELaSSCoHandler::getResultFileName.");
   return NULL;
}
