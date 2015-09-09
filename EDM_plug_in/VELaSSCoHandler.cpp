
#include "stdafx.h"


using namespace  ::dli;

#include "..\EDM_Interface\EDM_interface.h"
#include "EDMmodelCache.h"
#include "VELaSSCoHandler.h"
#include "Matrix.h"
#include < time.h >


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
VELaSSCoHandler::VELaSSCoHandler() {
   // Your initialization goes here
}


void gen_random(char *s, const int len) {
   static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
   for (int i = 0; i < len; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum)-1)];
   }
   s[len] = 0;
}

/**
* returns a session if if the user exists with the specified password and the specified role or an empty role.
*
* @param user_name
* @param role
* @param password
*/
void VELaSSCoHandler::UserLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password)
{
   char sessionID[64];
   gen_random(sessionID, sizeof(sessionID)); _return = sessionID;
}

/**
* Stop access to the system by a given session id and release all resources held by that session
*
* @param sessionID
*/
void VELaSSCoHandler::UserLogout(std::string& _return, const std::string& sessionID)
{

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

void VELaSSCoHandler::GetElementOfPointsInSpace(rvGetElementOfPointsInSpace& _return, const std::string& sessionID, const std::string& modelName, const std::vector<Point> & points)
{
   setCurrentSession(sessionID.data());
   EDMmodelCache *emc = setCurrentModelCache(atol(modelName.data()));
   FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);

   Iterator<fem::Element*, fem::entityType> elemIter(fmc->getObjectSet(fem::et_Element));
   Iterator<fem::Node*, fem::entityType> nodeIter(fmc->getObjectSet(fem::et_Node));
  

   bool sequentialSearch = false;
   bool boxSearch = true;
   int n_IsPointInsideTetrahedron_1 = 0;
   int n_IsPointInsideTetrahedron_2 = 0;
   int n_Points = 0;
   std::vector<dli::Element>  returnedElements_1;
   std::vector<dli::Element>  returnedElements_2;


   int startTime = GetTickCount();
   if (boxSearch) {
      Matrix<Set<fem::Element*>*> *elemBoxMatrix = (Matrix<Set<fem::Element*>*> *)fmc->voidElemBoxMatrix;
      int nInside = 0, nPointsInsideSeveralElements = 0;
      std::vector<dli::Point> myPoints = points;
      for (std::vector<dli::Point>::iterator p = myPoints.begin(); p != myPoints.end(); p++) {

         n_Points++;
         double p_in_x = p->x, p_in_y = p->y, p_in_z = p->z;
         int bx = p_in_x / fmc->dx, by = p_in_y / fmc->dy, bz = p_in_z / fmc->dz;
         Set<fem::Element*> *elemBox = elemBoxMatrix->getElement(bx, by, bz);
         bool outsideAll = true;
         Iterator<fem::Element*, fem::entityType> boxElemIter(elemBox);

         for (fem::Element *ep = boxElemIter.first(); ep; ep = boxElemIter.next()) {
            int elemId = ep->get_id();

            Iterator<fem::Node*, fem::entityType> nodeOfElemIter(ep->get_nodes());
            if (nodeOfElemIter.size() == 4) {
               fem::Node *np = nodeOfElemIter.first();
               double a_x = np->get_x(), a_y = np->get_y(), a_z = np->get_z(); np = nodeOfElemIter.next();
               double b_x = np->get_x(), b_y = np->get_y(), b_z = np->get_z(); np = nodeOfElemIter.next();
               double c_x = np->get_x(), c_y = np->get_y(), c_z = np->get_z(); np = nodeOfElemIter.next();
               double d_x = np->get_x(), d_y = np->get_y(), d_z = np->get_z(); np = nodeOfElemIter.next();
               double dst, epsilon = 0.000;
               double r_a = 0.0, r_b = 0.0, r_c = 0.0, r_d = 0.0;
               n_IsPointInsideTetrahedron_1++;
               if (IsPointInsideTetrahedron(p_in_x, p_in_y, p_in_z, a_x, a_y, a_z, b_x, b_y, b_z, c_x, c_y, c_z, d_x, d_y, d_z,
                  epsilon, r_a, r_b, r_c, r_d, &dst)) {
                  int elemId = ep->get_id();
                  nInside++;
                  if (outsideAll) {
                     dli::Element de;
                     de.__set_id(ep->get_id()); returnedElements_1.push_back(de);
                  } else {
                     nPointsInsideSeveralElements++;
                  }
                  outsideAll = false;
               }
            }
         }
         if (outsideAll) {
            dli::Element de;
            de.__set_id(-1); returnedElements_1.push_back(de);
         }
      }
   }
   int endTime = GetTickCount();

   if (sequentialSearch) {
      int nInside = 0, nPointsInsideSeveralElements = 0;
      std::vector<dli::Point> myPoints = points;
      for (std::vector<dli::Point>::iterator p = myPoints.begin(); p != myPoints.end(); p++) {

         double p_in_x = p->x, p_in_y = p->y, p_in_z = p->z;
         bool outsideAll = true;
         for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
            int elemId = ep->get_id();

            Iterator<fem::Node*, fem::entityType> nodeOfElemIter(ep->get_nodes());
            if (nodeOfElemIter.size() == 4) {
               fem::Node *np = nodeOfElemIter.first();
               double a_x = np->get_x(), a_y = np->get_y(), a_z = np->get_z(); np = nodeOfElemIter.next();
               double b_x = np->get_x(), b_y = np->get_y(), b_z = np->get_z(); np = nodeOfElemIter.next();
               double c_x = np->get_x(), c_y = np->get_y(), c_z = np->get_z(); np = nodeOfElemIter.next();
               double d_x = np->get_x(), d_y = np->get_y(), d_z = np->get_z(); np = nodeOfElemIter.next();
               double dst, epsilon = 0.000;
               double r_a = 0.0, r_b = 0.0, r_c = 0.0, r_d = 0.0;
               n_IsPointInsideTetrahedron_2++;
               if (IsPointInsideTetrahedron(p_in_x, p_in_y, p_in_z, a_x, a_y, a_z, b_x, b_y, b_z, c_x, c_y, c_z, d_x, d_y, d_z,
                  epsilon, r_a, r_b, r_c, r_d, &dst)) {
                  int elemId = ep->get_id();
                  nInside++;
                  if (outsideAll) {
                     dli::Element de;
                     de.__set_id(ep->get_id()); returnedElements_2.push_back(de);
                  } else {
                     nPointsInsideSeveralElements++;
                  }
                  outsideAll = false;
               }
            }
         }
         if (outsideAll) {
            dli::Element de;
            de.__set_id(-1); returnedElements_2.push_back(de);
         }
      }
   }
   if (sequentialSearch && boxSearch) {
      std::vector<dli::Element>::iterator e1 = returnedElements_1.begin();
      for (std::vector<dli::Element>::iterator e2 = returnedElements_2.begin(); e2 != returnedElements_2.end(); e2++) {
         if (e1 != returnedElements_1.end()) {
            if (e1->id != e2->id) {
               printf("Difference between box search and sequential search!!!");
            }
         } else {
            printf("Different length of box search and sequential search!!!");
         }
         e1++;
      }
   }
   if (boxSearch) {
      char msg[10000];
      sprintf(msg, "Containing element found for %d points.\nIsPointInsideTetrahedron is executed %d times.\nElapsed time for the search is %d millisecponds\n",
         n_Points, n_IsPointInsideTetrahedron_1, endTime - startTime);
      _return.__set_report(msg);
   }
   _return.__set_status("OK");
   _return.__set_elements(boxSearch ? returnedElements_1 : returnedElements_2);
}

#define MaxElemRef 100
typedef struct NodeInfo {
   int elemRefs[MaxElemRef];
   int nElemRefs;
   fem::Node*theNode;

   void findNodeInfo(fem::Node* np, VELaSSCoHandler *vh)
   {
      theNode = np; nElemRefs = 0;
      ReferencesIterator<fem::Element*, fem::entityType> elemIter(np, fem::et_Element);
      for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
         if (nElemRefs < MaxElemRef) {
            //elemRefs[nElemRefs++] = ep->get_id();
            int i, id = ep->get_id();
            for (i = 0; i < nElemRefs && id > elemRefs[i]; i++) ;
            if (i < nElemRefs) {
               for (int j = nElemRefs; j > i; j--) elemRefs[j] = elemRefs[j - 1];
            }
            elemRefs[i] = id; nElemRefs++;
         } else {
            vh->ReportError("nElemRefs > MaxElemRef in CalculateBoundaryOfMesh\n");
         }
      }
   }
} NodeInfo;


int findElementsOfTriangle(NodeInfo &n1, NodeInfo &n2, NodeInfo &n3)
{
   int nElems = 0, i1 = 0, i2 = 0, i3 = 0;

   while (i1 < n1.nElemRefs && i2 < n2.nElemRefs && i3 < n3.nElemRefs) {
      while (n1.elemRefs[i1] < n2.elemRefs[i2] && i1 < n1.nElemRefs)
         i1++;
      if (i1 < n1.nElemRefs) {
         while (n3.elemRefs[i3] < n2.elemRefs[i2] && i3 < n3.nElemRefs)
            i3++;
         if (i1 < n1.nElemRefs && i2 < n2.nElemRefs && i3 < n3.nElemRefs && n3.elemRefs[i3] == n2.elemRefs[i2] && n1.elemRefs[i1] == n2.elemRefs[i2]) {
            nElems++;
         }
      }
      i2++;
   }
   return nElems;
}

void copyNode(dli::Node &dn, fem::Node *sn)
{
   dn.__set_id(sn->get_id());
   dn.__set_x(sn->get_x()); dn.__set_y(sn->get_y()); dn.__set_z(sn->get_z());
}

void VELaSSCoHandler::CalculateBoundaryOfMesh(FEMmodelCache *fmc, std::vector<Triangle>  &elements)
{

   Iterator<fem::Element*, fem::entityType> elemIter(fmc->getObjectSet(fem::et_Element));

   for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
      Iterator<fem::Node*, fem::entityType> nodeIter(ep->get_nodes());
      if (nodeIter.size() == 4) {
         NodeInfo nodes[4];
         int ix = 0;
         for (fem::Node* np = nodeIter.first(); np; np = nodeIter.next()) {
            nodes[ix++].findNodeInfo(np, this);
         }
         static int nodesInTriangles[4][3] = {
            { 0, 1, 2 },
            { 0, 1, 3 },
            { 0, 2, 3 },
            { 1, 2, 3 },
         };
         //fem::Node* np0 = nodeIter.first();
         //fem::Node* np1 = nodeIter.next();
         //fem::Node* np2 = nodeIter.next();
         //fem::Node* np3 = nodeIter.next();
         //
         //nodes[0].findNodeInfo(np0, this);
         //nodes[1].findNodeInfo(np1, this);
         //nodes[2].findNodeInfo(np2, this);
         //nodes[3].findNodeInfo(np3, this);

         //int n_elementsOfTriangle1 = findElementsOfTriangle(nodes[0], nodes[1], nodes[2]);
         //int n_elementsOfTriangle2 = findElementsOfTriangle(nodes[0], nodes[2], nodes[3]);
         //int n_elementsOfTriangle3 = findElementsOfTriangle(nodes[0], nodes[1], nodes[3]);
         //int n_elementsOfTriangle4 = findElementsOfTriangle(nodes[1], nodes[2], nodes[3]);
         for (int i = 0; i < 4; i++) {
            NodeInfo *ni1 = &nodes[nodesInTriangles[i][0]], *ni2 = &nodes[nodesInTriangles[i][1]], *ni3 = &nodes[nodesInTriangles[i][2]];
            if (findElementsOfTriangle (*ni1, *ni2, *ni3) == 1) {
               dli::Node dn;
               std::vector <dli::Node> dnodes;
               copyNode(dn, ni1->theNode); dnodes.push_back(dn);
               copyNode(dn, ni2->theNode); dnodes.push_back(dn);
               copyNode(dn, ni3->theNode); dnodes.push_back(dn);
               dli::Triangle t;
               t.__set_nodes(dnodes);
               elements.push_back(t);
            }
         }

         int asdf = 9999;

      } else {
         ReportError("not tetrahdere elements in CalculateBoundaryOfMesh\n");
      }
   }
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

void VELaSSCoHandler::GetBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& _return, const std::string& sessionID, const std::string& modelName, const std::string& meshID, const std::string& analysisID, const double time_step)
{
   try {
      setCurrentSession(sessionID.data());
      EDMmodelCache *emc = setCurrentModelCache(atol(modelName.data()));
      if (emc) {
         FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
         std::vector<Triangle>  elements;

         CalculateBoundaryOfMesh(fmc, elements);
         _return.__set_status("OK");
         _return.__set_elements(elements);
      } else {
      _return.__set_status("Error"); _return.__set_report("Model does not exist.");
      }
   } catch (CedmError *e) {
      _return.__set_status("Error"); _return.__set_report(getErrorMsg(e));
   } catch (int thrownRstat) {
      _return.__set_status("Error"); _return.__set_report(getErrorMsg(thrownRstat));
   }

}


/**
* Given a list of vertices id's from the model, vertexIDs, GetResultFromVerticesID will get
* the result value of a given type (resultID) for each vertex id of the list.
*
* @param sessionID
* @param modelID
* @param vertexIDs
* @param resultID
* @param time_step
* @param analysisID
*/
void VELaSSCoHandler::GetResultFromVerticesID(rvGetResultFromVerticesID_B& _return, const std::string& sessionID, const std::string& modelID, const std::vector<int64_t> & vertexIDs, const std::string& resultID, const double time_step, const std::string& analysisID)
{
   try {
      setCurrentSession(sessionID.data());
      EDMmodelCache *emc = setCurrentModelCache(atol(modelID.data()));
      if (emc) {
         if (emc->type == mtDEM) {
            DEMmodelCache *dmc = dynamic_cast<DEMmodelCache*>(emc);
         } else {
            int nResultHeaderMatches = 0;
            std::vector<VertexResult> vResults;

            FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
            Iterator<fem::ResultHeader*, fem::entityType> rhIter(fmc->getObjectSet(fem::et_ResultHeader));
            for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
               if (strEQL(analysisID.data(), rh->get_analysis()) && time_step == rh->get_step() && strEQL(resultID.data(), rh->get_name())) {

                  fem::ResultBlock *rb = (fem::ResultBlock *)rh->getFirstReferencing(fem::et_ResultBlock);
                  if (rb) {
                     if (nResultHeaderMatches == 0) {
                        Iterator<fem::Result*, fem::entityType> valueIter(rb->get_values());
                        fem::entityType resultType;
                        for (fem::Result *r = valueIter.first(&resultType); r; r = valueIter.next(&resultType)) {
                           VertexResult vr;
                           fem::Node *n = r->get_result_for();
                           vector<double> values;

                           vr.__set_vertexID(n->get_id());
                           int size = sizeof(_return);
                           if (resultType == fem::et_ScalarResult) {
                              fem::ScalarResult *sr = static_cast<ScalarResult*>(r);
                              values.push_back(sr->get_val());
                           } else {
                              fem::VectorResult *vr = static_cast<VectorResult*>(r);
                              Iterator<double, fem::entityType> resultIter(vr->get_values());
                              for (double val = resultIter.first(); resultIter.moreElems(); val = resultIter.next()) {
                                 values.push_back(val);
                              }
                           }
                           vr.__set_resuls(values);
                           vResults.push_back(vr);
                        }
                     }
                     nResultHeaderMatches++;
                  }
               }
            }
            if (nResultHeaderMatches == 0) {
               _return.__set_status("Error"); _return.__set_report("No set of results satisfy search criteria.");
            //} else if (nResultHeaderMatches > 1) {
            //   _return.__set_status("Error"); _return.__set_report("More than one set of results satisfy search criteria.");
            } else {
                  _return.__set_status("OK"); _return.__set_vertexResults(vResults);
            }
         }
      } else {
         _return.__set_status("Error"); _return.__set_report("Model does not exist.");
      }
   } catch (CedmError *e) {
      _return.__set_status("Error"); _return.__set_report(getErrorMsg(e));
   } catch (int thrownRstat) {
      _return.__set_status("Error"); _return.__set_report(getErrorMsg(thrownRstat));
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
void VELaSSCoHandler::GetListOfModels(rvGetListOfModels& _return, const std::string& sessionID, const std::string& groupQualifier, const std::string& modelNamePattern, const std::string& options)
{
   char *modelName, *repositoryName;
   SdaiBoolean userDefined;
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance), index = 10, numberOfHits = 1;
   SdaiInstance resultBuffer[1], repositoryID;
   dli::ModelInfo mi;
   std::vector<dli::ModelInfo>  infoList;

   do {
      edmiSelectInstancesBN(edmiGetModelBN("SystemRepository", "ExpressDataManager"), "edm_model", NULL, 0,
         maxBufferSize, &index, &numberOfHits, resultBuffer);
      index++;
      if (numberOfHits > 0) {
         void *sp = sdaiGetAttrBN(resultBuffer[0], "Repository", sdaiINSTANCE, &repositoryID);
         if (sp) {
            sp = sdaiGetAttrBN(repositoryID, "USER_DEFINED", sdaiBOOLEAN, &userDefined);
            sp = sdaiGetAttrBN(repositoryID, "Name", sdaiSTRING, &repositoryName);
            if (sp) {
               if (strEQL(repositoryName, "DataRepository") || userDefined) {
                  sp = sdaiGetAttrBN(resultBuffer[0], "Name", sdaiSTRING, &modelName);
                  if (sp) {
                     mi.__set_name(modelName);
                     infoList.push_back(mi);
                  }
               }
            }
         }
      }
   } while (numberOfHits > 0);
   _return.__set_status("OK");
   _return.__set_models(infoList);
}
