
#include "stdafx.h"
#include "VELaSSCoMethods.h"


//using namespace  ::dli;

#include "EDM_interface.h"
#include "CLogger.h"
#include "EdmAccess.h"
#include "VELaSSCoHandler.h"
//#include "Matrix.h"
#ifdef _WINDOWS
#include <time.h>
#else
#include <sys/time.h>
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


#define newObject(className) new(m)className(m)

// point inside first tetrahedron of the VELaSSCo_HbaseBasicTest example
// is 0.034967938,0.689002752,0.025866667
// it is the centre of the first tetrahedron
// assuming numbering starts with 1, then:
// first tetrahedron ( id = 1) has nodes = 1794 1660 1802 1768
// in c/c++, numberations starts with 0, so nodes = 1793 1659 1801 1767
// coordinates of these nodes are:
//    1660 0.07139438390731812 0.7269042730331421 0.0
//    1768 0.0 0.683978796005249 0.10346666723489761
//    1794 0.06847736984491348 0.6451279520988464 0.0
//    1802 0.0 0.699999988079071 0.0

bool IsPointInsideTetrahedron(double p_in_x, double p_in_y, double p_in_z,
   double a_x, double a_y, double a_z,
   double b_x, double b_y, double b_z,
   double c_x, double c_y, double c_z,
   double d_x, double d_y, double d_z,
   double epsilon,
   double r_a, double r_b, double r_c, double r_d, double *dst)
{
   double r, s, t;
   r = s = t = 0;

   double da_x = a_x - d_x;
   double da_y = a_y - d_y;
   double da_z = a_z - d_z;
   double db_x = b_x - d_x;
   double db_y = b_y - d_y;
   double db_z = b_z - d_z;
   double dc_x = c_x - d_x;
   double dc_y = c_y - d_y;
   double dc_z = c_z - d_z;
   double dp_x = p_in_x - d_x;
   double dp_y = p_in_y - d_y;
   double dp_z = p_in_z - d_z;
   double r_denominator = da_x * (db_y * dc_z - dc_y * db_z) + db_x * (dc_y * da_z - da_y * dc_z) + dc_x * (da_y * db_z - db_y * da_z);
   double r_numerator = dp_x * (db_y * dc_z - dc_y * db_z) + db_x * (dc_y * dp_z - dp_y * dc_z) + dc_x * (dp_y * db_z - db_y * dp_z);

   if (fabs(r_numerator) < epsilon)
      r_numerator = 0.0f;
   if (fabs(r_denominator) > epsilon)
      r = r_numerator / r_denominator;
   else
      r = 0.0f;
   if ((r < 0.0f) || (r > 1.0f))
      return false;

   double s_denominator = db_y * dc_z - dc_y * db_z;
   double s_numerator = dp_y * dc_z - dc_y * dp_z + r * (dc_y * da_z - da_y * dc_z);
   if (fabs(s_numerator) < epsilon)
      s_numerator = 0.0f;
   if (fabs(s_denominator) > epsilon)
      s = s_numerator / s_denominator;
   else {
      s_denominator = db_y * dc_x - dc_y * db_x;
      s_numerator = dp_y * dc_x - dc_y * dp_x + r * (dc_y * da_x - da_y * dc_x);
      if (fabs(s_denominator) > epsilon) {
         s = s_numerator / s_denominator;
      } else {
         s_denominator = db_x * dc_z - dc_x * db_z;
         s_numerator = dp_x * dc_z - dc_x * dp_z + r * (dc_x * da_z - da_x * dc_z);
         if (fabs(s_denominator) > epsilon) {
            s = s_numerator / s_denominator;
         } else {
            s = 0.0f;
         }
      }
   }
   if ((s < 0.0f) || (s > 1.0f))
      return false;

   double t_denominator = dc_z;
   double t_numerator = dp_z - r * da_z - s * db_z;
   if (fabs(t_numerator) < epsilon)
      t_numerator = 0.0f;
   if (fabs(t_denominator) > epsilon)
      t = t_numerator / t_denominator;
   else {
      t_denominator = dc_x;
      t_numerator = dp_x - r * da_x - s * db_x;
      if (fabs(t_denominator) > epsilon)
         t = t_numerator / t_denominator;
      else {
         t_denominator = dc_y;
         t_numerator = dp_y - r * da_y - s * db_y;
         if (fabs(t_denominator) > epsilon)
            t = t_numerator / t_denominator;
         else {
            t = 0.0f;
         }
      }
   }
   if ((t < 0.0f) || (t > 1.0f))
      return false;

   if (((r + s + t) >= -0.0) && ((r + s + t) <= 1.0f)) {
      double v = 1.0 - r - s - t;
      *dst = r_a * r + r_b * s + r_c * t + r_d * v;
      return true;
   } else {
      return false;
   }
}

/**
* VELaSSCoHandler constructor
*
*/
VELaSSCoHandler::VELaSSCoHandler() : VELaSSCo_VQueries() {
   thelog = NULL;
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
   thelog->logg(1, "-->UserLogout\nsessionID=%s\n\n", sessionID.data());
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
   VELaSSCoMethods theQuery(theCluster);
   try {
      thelog->logg(2, "-->GetResultFromVerticesID\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
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
   VELaSSCoMethods theQuery(theCluster);
   try {
      thelog->logg(2, "-->GetCoordinatesAndElementsFromMesh\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
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
}

/**
* Stop Data Layer
*/
void VELaSSCoHandler::stopAll()
{
   //--------------------->
}
/**
* For each point in the input parameter points, the method returns data about the element that contains the point.
* The number of elements in the returned list of elements shall be the same as the number of points in the input parameter.
* If the method does not find an element for a point it shall return a dummy element with id equal to -1.
*
* @param sessionID
* @param modelName
* @param points
*/
//
//void VELaSSCoHandler::GetElementOfPointsInSpace(rvGetElementOfPointsInSpace& _return, const std::string& sessionID, const std::string& modelName, const std::vector<Point> & points)
//{
//   try {
//      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelName.data()));
//      if (emc) {
//         FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
//
//         Iterator<fem::Element*, fem::entityType> elemIter(fmc->getObjectSet(fem::et_Element), fmc);
//         Iterator<fem::Node*, fem::entityType> nodeIter(fmc->getObjectSet(fem::et_Node), fmc);
//
//
//         bool sequentialSearch = false;
//         bool boxSearch = true;
//         int n_IsPointInsideTetrahedron_1 = 0;
//         int n_IsPointInsideTetrahedron_2 = 0;
//         int n_Points = 0;
//         std::vector<VELaSSCoSM::Element>  returnedElements_1;
//         std::vector<VELaSSCoSM::Element>  returnedElements_2;
//         tEdmiInstData elementData;
//         elementData.cppObject = NULL; elementData.entityData = &fmc->theEntities[fem::et_Element];
//         fem::Element cElement(fmc, &elementData);
//
//         int startTime = GetTickCount();
//         if (boxSearch) {
//            Matrix<Container<InstanceId>*> *elemBoxMatrix = (Matrix<Container<InstanceId>*> *)fmc->voidElemBoxMatrix;
//            int nInside = 0, nPointsInsideSeveralElements = 0;
//            std::vector<VELaSSCoSM::Point> myPoints = points;
//            for (std::vector<VELaSSCoSM::Point>::iterator p = myPoints.begin(); p != myPoints.end(); p++) {
//
//               n_Points++;
//               double p_in_x = p->x, p_in_y = p->y, p_in_z = p->z;
//               int bx = p_in_x / fmc->dx, by = p_in_y / fmc->dy, bz = p_in_z / fmc->dz;
//               bool outsideAll = true;
//               if (bx < fmc->nOf_x && by < fmc->nOf_y && bz < fmc->nOf_z) {
//                  Container<InstanceId> *elemBox = elemBoxMatrix->getElement(bx, by, bz);
//
//                  for (InstanceId elementId = elemBox->first(); elementId; elementId = elemBox->next()) {
//                     cElement.setInstanceId(elementId);
//                     int elemId = cElement.get_id();
//
//                     Iterator<fem::Node*, fem::entityType> nodeOfElemIter(cElement.get_nodes());
//                     if (nodeOfElemIter.size() == 4) {
//                        fem::Node *np = nodeOfElemIter.first();
//                        double a_x = np->get_x(), a_y = np->get_y(), a_z = np->get_z(); np = nodeOfElemIter.next();
//                        double b_x = np->get_x(), b_y = np->get_y(), b_z = np->get_z(); np = nodeOfElemIter.next();
//                        double c_x = np->get_x(), c_y = np->get_y(), c_z = np->get_z(); np = nodeOfElemIter.next();
//                        double d_x = np->get_x(), d_y = np->get_y(), d_z = np->get_z(); np = nodeOfElemIter.next();
//                        double dst, epsilon = 0.000;
//                        double r_a = 0.0, r_b = 0.0, r_c = 0.0, r_d = 0.0;
//                        n_IsPointInsideTetrahedron_1++;
//                        if (IsPointInsideTetrahedron(p_in_x, p_in_y, p_in_z, a_x, a_y, a_z, b_x, b_y, b_z, c_x, c_y, c_z, d_x, d_y, d_z,
//                           epsilon, r_a, r_b, r_c, r_d, &dst)) {
//                           int elemId = cElement.get_id();
//                           nInside++;
//                           if (outsideAll) {
//                              VELaSSCoSM::Element de;
//                              de.__set_id(elemId); returnedElements_1.push_back(de);
//                           } else {
//                              nPointsInsideSeveralElements++;
//                           }
//                           outsideAll = false;
//                        }
//                     }
//                  }
//               }
//               if (outsideAll) {
//                  VELaSSCoSM::Element de;
//                  de.__set_id(-1); returnedElements_1.push_back(de);
//               }
//            }
//         }
//         int endTime = GetTickCount();
//
//         if (sequentialSearch) {
//            int nInside = 0, nPointsInsideSeveralElements = 0;
//            std::vector<VELaSSCoSM::Point> myPoints = points;
//            for (std::vector<VELaSSCoSM::Point>::iterator p = myPoints.begin(); p != myPoints.end(); p++) {
//
//               double p_in_x = p->x, p_in_y = p->y, p_in_z = p->z;
//               bool outsideAll = true;
//               for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
//                  int elemId = ep->get_id();
//
//                  Iterator<fem::Node*, fem::entityType> nodeOfElemIter(ep->get_nodes());
//                  if (nodeOfElemIter.size() == 4) {
//                     fem::Node *np = nodeOfElemIter.first();
//                     double a_x = np->get_x(), a_y = np->get_y(), a_z = np->get_z(); np = nodeOfElemIter.next();
//                     double b_x = np->get_x(), b_y = np->get_y(), b_z = np->get_z(); np = nodeOfElemIter.next();
//                     double c_x = np->get_x(), c_y = np->get_y(), c_z = np->get_z(); np = nodeOfElemIter.next();
//                     double d_x = np->get_x(), d_y = np->get_y(), d_z = np->get_z(); np = nodeOfElemIter.next();
//                     double dst, epsilon = 0.000;
//                     double r_a = 0.0, r_b = 0.0, r_c = 0.0, r_d = 0.0;
//                     n_IsPointInsideTetrahedron_2++;
//                     if (IsPointInsideTetrahedron(p_in_x, p_in_y, p_in_z, a_x, a_y, a_z, b_x, b_y, b_z, c_x, c_y, c_z, d_x, d_y, d_z,
//                        epsilon, r_a, r_b, r_c, r_d, &dst)) {
//                        int elemId = ep->get_id();
//                        nInside++;
//                        if (outsideAll) {
//                           VELaSSCoSM::Element de;
//                           de.__set_id(ep->get_id()); returnedElements_2.push_back(de);
//                        } else {
//                           nPointsInsideSeveralElements++;
//                        }
//                        outsideAll = false;
//                     }
//                  }
//               }
//               if (outsideAll) {
//                  VELaSSCoSM::Element de;
//                  de.__set_id(-1); returnedElements_2.push_back(de);
//               }
//            }
//         }
//         if (sequentialSearch && boxSearch) {
//            std::vector<VELaSSCoSM::Element>::iterator e1 = returnedElements_1.begin();
//            for (std::vector<VELaSSCoSM::Element>::iterator e2 = returnedElements_2.begin(); e2 != returnedElements_2.end(); e2++) {
//               if (e1 != returnedElements_1.end()) {
//                  if (e1->id != e2->id) {
//                     printf("Difference between box search and sequential search!!!");
//                  }
//               } else {
//                  printf("Different length of box search and sequential search!!!");
//               }
//               e1++;
//            }
//         }
//         if (boxSearch) {
//            char msg[10000];
//            sprintf(msg, "Containing element found for %d points.\nIsPointInsideTetrahedron is executed %d times.\nElapsed time for the search is %d millisecponds\n",
//               n_Points, n_IsPointInsideTetrahedron_1, endTime - startTime);
//            _return.__set_report(msg);
//         }
//         _return.__set_status("OK");
//         _return.__set_elements(boxSearch ? returnedElements_1 : returnedElements_2);
//      } else {
//         _return.__set_status("Error"); _return.__set_report("Model does not exist.");
//      }
//   } catch (CedmError *e) {
//      string errMsg;
//      handleError(errMsg, e);
//      _return.__set_status("Error"); _return.__set_report(errMsg);
//   }
//}
//


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
void VELaSSCoHandler::getBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& rv, const std::string& sessionID,
   const std::string& modelID, const std::string& meshID, const std::string& analysisID, const double time_step)
/*=============================================================================================================================*/
{
   VELaSSCoMethods theQuery(theCluster);
   try {
      thelog->logg(2, "-->GetBoundaryOfLocalMesh\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         theQuery.GetBoundaryOfLocalMesh(rv, analysisID, time_step, meshID);
      } else {
         char *emsg = "Model does not exist.";
         rv.__set_status("Error"); rv.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      rv.__set_status("Error"); rv.__set_report(errMsg);
   }
}




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
   char *modelName, *repositoryName = "", *schemaName;
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance), index = 0, numberOfHits = 1, nModels = 0;
   SdaiInstance resultBuffer[nOfResults];
   VELaSSCoSM::FullyQualifiedModelName fqmn;
   std::vector<VELaSSCoSM::FullyQualifiedModelName>  infoList;
   char nameBuf[2048], condition[1024];
   EdmiError  rstat;

   try {
      thelog->logg(3, "-->GetListOfModelNames\nsessionID=%s\ngroupQualifier=%s\nmodelNamePattern=%s\n\n", sessionID.data(), groupQualifier.data(), modelNamePattern.data());

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

   try {
      thelog->logg(3, "-->openModel\nsessionID=%s\nmodelName=%s\nrequestedAccess=%s\n\n", sessionID.data(), modelName.data(), requestedAccess.data());
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
   VELaSSCoMethods theQuery(theCluster);
   try {
      thelog->logg(2, "-->getListOfAnalyses\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
      setCurrentSession(sessionID.data());
      if (theQuery.OpenClusterModelAndPrepareExecution(modelID)) {
         theQuery.GetListOfAnalyses(_return);
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
* Retrieves the list of time steps for a given model and analysis.
*
* @param sessionID
* @param modelID
*/
void VELaSSCoHandler::calculateBoundingBox(const std::string &sessionID, const std::string &modelID, const std::string &dataTableName,
   const std::string &analysisID, const int numSteps, const double *lstSteps,
   const int64_t numVertexIDs, const int64_t *lstVertexIDs,
   double *return_bbox, std::string *return_error_str)
{
   *return_error_str = "calculateBoundingBox is not implemented";
}
//void VELaSSCoHandler::CalculateBoundingBox(rvGetListOfAnalyses& _return, const std::string& sessionID, const std::string& modelID)
//{
//   try {
//      thelog->logg(2, "-->CalculateBoundingBox\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
//      setCurrentSession(sessionID.data());
//      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelID.data()));
//      if (emc) {
//         vector<string> analysisNames;
//        if (emc->type == mtDEM) {
//            DEMmodelCache *dmc = dynamic_cast<DEMmodelCache*>(emc);
//            Iterator<dem::Simulation*, dem::entityType> simIter(dmc->getObjectSet(dem::et_Simulation), dmc);
//
//
//            _return.__set_status("OK"); _return.__set_analyses(analysisNames);
//         } else if (emc->type == mtFEM) {
//            FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
//            Iterator<fem::Mesh*, fem::entityType> meshIter(fmc->getObjectSet(fem ::et_Mesh), fmc);
//            fem::Mesh *mesh = meshIter.first();
//            //while (mesh) {
//            //   Iterator<fem::Node*, fem::entityType> nodeIter(mesh->get_nodes());
//            //   for (fem::Node *n = nodeIter.first(); n; )
//            //}
//
//
//
//
//            _return.__set_status("OK"); _return.__set_analyses(analysisNames);
//            thelog->logg(0, "status=OK\n\n");
//        }
//      } else {
//         char *emsg = "Model does not exist.";
//         _return.__set_status("Error"); _return.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
//      }
//   } catch (CedmError *e) {
//      string errMsg;
//      handleError(errMsg, e);
//      _return.__set_status("Error"); _return.__set_report(errMsg);
//   }
//}

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
//void VELaSSCoHandler::GetBoundingBox(rvGetBoundingBox& _return, const std::string& sessionID, const std::string& modelID, const std::vector<int64_t> & verticesID, const std::string& analysisID, const std::string& timeStepOption, const std::vector<double> & timeSteps)
//{
//   try {
//      thelog->logg(2, "-->GetBoundingBox\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
//      setCurrentSession(sessionID.data());
//      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelID.data()));
//      if (emc) {
//         vector<string> analysisNames;
//         if (emc->type == mtDEM) {
//            DEMmodelCache *dmc = dynamic_cast<DEMmodelCache*>(emc);
//            Iterator<dem::Simulation*, dem::entityType> simIter(dmc->getObjectSet(dem::et_Simulation), dmc);
//
//
//            _return.__set_status("OK"); 
//         } else if (emc->type == mtFEM) {
//            FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
//            Iterator<fem::Mesh*, fem::entityType> meshIter(fmc->getObjectSet(fem::et_Mesh), fmc);
//            fem::Mesh *mesh = meshIter.first();
//            //while (mesh) {
//            //   Iterator<fem::Node*, fem::entityType> nodeIter(mesh->get_nodes());
//            //   for (fem::Node *n = nodeIter.first(); n; )
//            //}
//
//
//
//
//            _return.__set_status("OK"); 
//            thelog->logg(0, "status=OK\n\n");
//         }
//      } else {
//         char *emsg = "Model does not exist.";
//         _return.__set_status("Error"); _return.__set_report(emsg); thelog->logg(1, "status=Error\nerror report=%s\n\n", emsg);
//      }
//   } catch (CedmError *e) {
//      string errMsg;
//      handleError(errMsg, e);
//      _return.__set_status("Error"); _return.__set_report(errMsg);
//   }
//}



/**
* Retrieves the list of time steps for a given model and analysis.
*
* @param sessionID
* @param modelID
* @param analysisID
*/
void VELaSSCoHandler::getListOfTimeSteps(rvGetListOfTimeSteps& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const std::string &stepOptions, const int numSteps, const double *lstSteps)
{
   VELaSSCoMethods theQuery(theCluster);
   try {
      thelog->logg(2, "-->GetListOfAnalyses\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
      setCurrentSession(sessionID.data());
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
   VELaSSCoMethods theQuery(theCluster);
   try {
      thelog->logg(2, "-->GetListOfResultsFromTimeStepAndAnalysis\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
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
}


void VELaSSCoHandler::getListOfVerticesFromMesh(rvGetListOfVerticesFromMesh& rv, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double stepValue, const int32_t meshID)
{
   VELaSSCoMethods theQuery(theCluster);
   try {
      thelog->logg(2, "-->getListOfVerticesFromMesh\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
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
   VELaSSCoMethods theQuery(theCluster);
   try {
      thelog->logg(2, "-->GetListOfMeshes\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
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

