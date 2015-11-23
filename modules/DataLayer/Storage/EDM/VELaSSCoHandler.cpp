
#include "stdafx.h"


//using namespace  ::dli;

#include "..\..\..\..\EDM_Interface\EDM_interface.h"
#include "CLogger.h"
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
void  VELaSSCoHandler::statusDL(std::string& _return)
{
   thelog->logg(0, "-->statusDL\n\n");
   _return = "OK";
   thelog->logg(0, "status=OK\n\n");
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
   thelog->logg(3, "-->UserLogin\nuser_name=%s\nrole=%s\npassword=%s\n\n", user_name.data(), role.data(), password.data());
   gen_random(sessionID, sizeof(sessionID)); _return = sessionID;
}

/**
* Stop access to the system by a given session id and release all resources held by that session
*
* @param sessionID
*/
void VELaSSCoHandler::UserLogout(std::string& _return, const std::string& sessionID)
{
   thelog->logg(1, "-->UserLogout\nsessionID=%s\n\n", sessionID.data());

}

/**
* Return the status of the different services
* which run on the Data Layer.
* @return string - returns a structured list of avialbe vertices,
* with the attached list of double
* if errors occur the contect is also returned here?
*
* @param sessionID
* @param modelID
* @param analysisID
* @param timeStep
* @param resultID
* @param listOfVertices
*/
void VELaSSCoHandler::GetResultFromVerticesID(std::string& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::string& listOfVertices)
{

}

/**
* Stop Data Layer
*/
void VELaSSCoHandler::stopAll()
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
   EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelName.data()));
   if (emc) {
      FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);

      Iterator<fem::Element*, fem::entityType> elemIter(fmc->getObjectSet(fem::et_Element));
      Iterator<fem::Node*, fem::entityType> nodeIter(fmc->getObjectSet(fem::et_Node));


      bool sequentialSearch = false;
      bool boxSearch = true;
      int n_IsPointInsideTetrahedron_1 = 0;
      int n_IsPointInsideTetrahedron_2 = 0;
      int n_Points = 0;
      std::vector<VELaSSCoSM::Element>  returnedElements_1;
      std::vector<VELaSSCoSM::Element>  returnedElements_2;


      int startTime = GetTickCount();
      if (boxSearch) {
         Matrix<Set<fem::Element*>*> *elemBoxMatrix = (Matrix<Set<fem::Element*>*> *)fmc->voidElemBoxMatrix;
         int nInside = 0, nPointsInsideSeveralElements = 0;
         std::vector<VELaSSCoSM::Point> myPoints = points;
         for (std::vector<VELaSSCoSM::Point>::iterator p = myPoints.begin(); p != myPoints.end(); p++) {

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
                        VELaSSCoSM::Element de;
                        de.__set_id(ep->get_id()); returnedElements_1.push_back(de);
                     } else {
                        nPointsInsideSeveralElements++;
                     }
                     outsideAll = false;
                  }
               }
            }
            if (outsideAll) {
               VELaSSCoSM::Element de;
               de.__set_id(-1); returnedElements_1.push_back(de);
            }
         }
      }
      int endTime = GetTickCount();

      if (sequentialSearch) {
         int nInside = 0, nPointsInsideSeveralElements = 0;
         std::vector<VELaSSCoSM::Point> myPoints = points;
         for (std::vector<VELaSSCoSM::Point>::iterator p = myPoints.begin(); p != myPoints.end(); p++) {

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
                        VELaSSCoSM::Element de;
                        de.__set_id(ep->get_id()); returnedElements_2.push_back(de);
                     } else {
                        nPointsInsideSeveralElements++;
                     }
                     outsideAll = false;
                  }
               }
            }
            if (outsideAll) {
               VELaSSCoSM::Element de;
               de.__set_id(-1); returnedElements_2.push_back(de);
            }
         }
      }
      if (sequentialSearch && boxSearch) {
         std::vector<VELaSSCoSM::Element>::iterator e1 = returnedElements_1.begin();
         for (std::vector<VELaSSCoSM::Element>::iterator e2 = returnedElements_2.begin(); e2 != returnedElements_2.end(); e2++) {
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
   } else {
      _return.__set_status("Error"); _return.__set_report("Model does not exist.");
   }
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

void copyNode(VELaSSCoSM::Node &dn, fem::Node *sn)
{
   //dn.__set_id(sn->get_id());
   //dn.__set_x(sn->get_x()); dn.__set_y(sn->get_y()); dn.__set_z(sn->get_z());
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
               VELaSSCoSM::Node dn;
               std::vector <VELaSSCoSM::Node> dnodes;
               copyNode(dn, ni1->theNode); dnodes.push_back(dn);
               copyNode(dn, ni2->theNode); dnodes.push_back(dn);
               copyNode(dn, ni3->theNode); dnodes.push_back(dn);
               VELaSSCoSM::Triangle t;
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
/*=============================================================================================================================*/
void VELaSSCoHandler::GetBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& _return, const std::string& sessionID,
   const std::string& modelID, const std::string& meshID, const std::string& analysisID, const double time_step)
/*=============================================================================================================================*/
{
   try {
      thelog->logg(5, "-->GetBoundaryOfLocalMesh\nsessionID=%s\nmodelID=%s\nmeshID=%s\nanalysisID=%s\ntime_step=%f\n\n",
         sessionID.data(), modelID.data(), meshID.data(), analysisID.data(), time_step);
      setCurrentSession(sessionID.data());
      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelID.data()));
      if (emc) {
         FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
         std::vector<Triangle>  elements;

         CalculateBoundaryOfMesh(fmc, elements);
         _return.__set_status("OK"); _return.__set_elements(elements); thelog->logg(0, "status=OK\n\n");
      } else {
         char *em = "Model does not exist.";
         _return.__set_status("Error"); _return.__set_report(em); thelog->logg(1, "status=Error\nerror report=%s\n\n", em);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return.__set_status("Error"); _return.__set_report(errMsg);
   }
}

/*=============================================================================================================================*/
void VELaSSCoHandler::GetFEMresultFromVerticesID(rvGetResultFromVerticesID_B& _return, bool allNodes,
   std::map<int, int> & nodesInParameter, const char *resultID, const double time_step, const char *analysisID, FEMmodelCache *fmc)
/*=============================================================================================================================*/
{
   int nResultHeaderMatches = 0;
   std::vector<VertexResult> vResults;

   try {
      Iterator<fem::ResultHeader*, fem::entityType> rhIter(fmc->getObjectSet(fem::et_ResultHeader));
      for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
         if (strEQL(analysisID, rh->get_analysis()) && time_step == rh->get_step() && strEQL(resultID, rh->get_name())) {
            fem::ResultBlock *rb = (fem::ResultBlock *)rh->getFirstReferencing(fem::et_ResultBlock);
            if (rb) {
               nResultHeaderMatches++;
               Iterator<fem::Result*, fem::entityType> valueIter(rb->get_values());
               fem::entityType resultType;
               for (fem::Result *r = valueIter.first(&resultType); r; r = valueIter.next(&resultType)) {
                  VertexResult vr;
                  fem::Node *n = r->get_result_for();
                  if (allNodes || nodesInParameter[n->get_id()] == 1) {
                     vector<double> values;
                     vr.__set_vertexID(n->get_id());
                     int size = sizeof(_return);
                     if (resultType == fem::et_ScalarResult) {
                        fem::ScalarResult *sr = static_cast<ScalarResult*>(r);
                        values.push_back(sr->get_val());
                     } else {
                        fem::VectorResult *vr = static_cast<VectorResult*>(r);
                        Iterator<double, fem::entityType> resultIter(vr->get_values());
                        for (double val = resultIter.firstReal(); resultIter.moreElems(); val = resultIter.nextReal()) {
                           values.push_back(val);
                        }
                     }
                     vr.__set_resuls(values);
                     vResults.push_back(vr);
                  }
               }
            }
         }
      }
      if (nResultHeaderMatches == 0) {
         _return.__set_status("Error"); _return.__set_report("No set of results satisfy search criteria.");
      } else {
         _return.__set_status("OK"); _return.__set_vertexResults(vResults);
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return.__set_status("Error"); _return.__set_report(errMsg);
   }
}

void VELaSSCoHandler::GetDEMresultFromVerticesID(rvGetResultFromVerticesID_B& _return, bool allNodes, std::map<int, int> & nodesInParameter, const char *resultID, const double time_step, const char *analysisID, DEMmodelCache *dmc)
{
   vector<VertexResult> vResults;
   //vector<int64_t> theVertexIDs = vertexIDs;

   //for (vector<int64_t>::iterator idIter = theVertexIDs.begin(); idIter != theVertexIDs.end(); idIter++) {
   //   ;
   //}
   //Iterator<fem::ResultHeader*, fem::entityType> rhIter(fmc->getObjectSet(fem::et_ResultHeader));
   //for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
   //   if (strEQL(analysisID, rh->get_analysis()) && time_step == rh->get_step() && strEQL(resultID, rh->get_name())) {
   //      fem::ResultBlock *rb = (fem::ResultBlock *)rh->getFirstReferencing(fem::et_ResultBlock);
   //      if (rb) {
   //         nResultHeaderMatches++;
   //         Iterator<fem::Result*, fem::entityType> valueIter(rb->get_values());
   //         fem::entityType resultType;
   //         for (fem::Result *r = valueIter.first(&resultType); r; r = valueIter.next(&resultType)) {
   //            VertexResult vr;
   //            fem::Node *n = r->get_result_for();
   //            if (allNodes || nodesInParameter[n->get_id()] == 1) {
   //               vector<double> values;
   //               vr.__set_vertexID(n->get_id());
   //               int size = sizeof(_return);
   //               if (resultType == fem::et_ScalarResult) {
   //                  fem::ScalarResult *sr = static_cast<ScalarResult*>(r);
   //                  values.push_back(sr->get_val());
   //               } else {
   //                  fem::VectorResult *vr = static_cast<VectorResult*>(r);
   //                  Iterator<double, fem::entityType> resultIter(vr->get_values());
   //                  for (double val = resultIter.firstReal(); resultIter.moreElems(); val = resultIter.nextReal()) {
   //                     values.push_back(val);
   //                  }
   //               }
   //               vr.__set_resuls(values);
   //               vResults.push_back(vr);
   //            }
   //         }
   //      }
   //   }
   //}
   //if (nResultHeaderMatches == 0) {
   //   _return.__set_status("Error"); _return.__set_report("No set of results satisfy search criteria.");
   //} else {
   //   _return.__set_status("OK"); _return.__set_vertexResults(vResults);
   //}
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
//void VELaSSCoHandler::GetResultFromVerticesID(rvGetResultFromVerticesID_B& _return, const std::string& sessionID, const std::string& modelID, const std::string& coordinatesSet, const std::vector<int64_t> & vertexIDs, const std::string& resultID, const double time_step, const std::string& analysisID)
//{
//   try {
//thelog->logg(3, "-->GetListOfAnalyses\nsessionID=%s\nmodelName=%s\nanalysisID=%s\n\n", sessionID.data(), modelID.data(), analysisID.data());
//      setCurrentSession(sessionID.data());
//      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelID.data()));
//      if (emc) {
//         std::map<int, int> nodesInParameter;
//         std::vector<int64_t> myVertexIDs = vertexIDs;
//         for (vector < int64_t>::iterator vIter = myVertexIDs.begin(); vIter != myVertexIDs.end(); vIter++) {
//            nodesInParameter[*vIter] = 1;
//         }
//         if (emc->type == mtDEM) {
//            DEMmodelCache *dmc = dynamic_cast<DEMmodelCache*>(emc);
//         } else {
//            GetFEMresultFromVerticesID(_return,myVertexIDs.size() == 0, nodesInParameter, resultID.data(), time_step, analysisID.data(), dynamic_cast<FEMmodelCache*>(emc));
//            //int nResultHeaderMatches = 0;
//            //std::vector<VertexResult> vResults;
//
//            //FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
//            //Iterator<fem::ResultHeader*, fem::entityType> rhIter(fmc->getObjectSet(fem::et_ResultHeader));
//            //for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
//            //   if (strEQL(analysisID.data(), rh->get_analysis()) && time_step == rh->get_step() && strEQL(resultID.data(), rh->get_name())) {
//            //      fem::ResultBlock *rb = (fem::ResultBlock *)rh->getFirstReferencing(fem::et_ResultBlock);
//            //      if (rb) {
//            //         nResultHeaderMatches++;
//            //         Iterator<fem::Result*, fem::entityType> valueIter(rb->get_values());
//            //         fem::entityType resultType;
//            //         for (fem::Result *r = valueIter.first(&resultType); r; r = valueIter.next(&resultType)) {
//            //            VertexResult vr;
//            //            fem::Node *n = r->get_result_for();
//            //            vector<double> values;
//
//            //            vr.__set_vertexID(n->get_id());
//            //            int size = sizeof(_return);
//            //            if (resultType == fem::et_ScalarResult) {
//            //               fem::ScalarResult *sr = static_cast<ScalarResult*>(r);
//            //               values.push_back(sr->get_val());
//            //            } else {
//            //               fem::VectorResult *vr = static_cast<VectorResult*>(r);
//            //               Iterator<double, fem::entityType> resultIter(vr->get_values());
//            //               for (double val = resultIter.first(); resultIter.moreElems(); val = resultIter.next()) {
//            //                  values.push_back(val);
//            //               }
//            //            }
//            //            vr.__set_resuls(values);
//            //            vResults.push_back(vr);
//            //         }
//            //      }
//            //   }
//            //}
//            //if (nResultHeaderMatches == 0) {
//            //   _return.__set_status("Error"); _return.__set_report("No set of results satisfy search criteria.");
//            //} else {
//            //   _return.__set_status("OK"); _return.__set_vertexResults(vResults);
//            //}
//         }
//      } else {
//         _return.__set_status("Error"); _return.__set_report("Model does not exist.");
//      }
//   } catch (CedmError *e) {
//      _return.__set_status("Error"); _return.__set_report(getErrorMsg(e));
//   } catch (int thrownRstat) {
//      _return.__set_status("Error"); _return.__set_report(getErrorMsg(thrownRstat));
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

void VELaSSCoHandler::GetListOfModelNames(rvGetListOfModels& _return, const std::string& sessionID, const std::string& groupQualifier, const std::string& modelNamePattern)
{
   char *modelName, *repositoryName;
   SdaiBoolean userDefined;
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance), index = 10, numberOfHits = 1, nModels = 0;
   SdaiInstance resultBuffer[1], repositoryID;
   VELaSSCoSM::FullyQualifiedModelName fqmn;
   std::vector<VELaSSCoSM::FullyQualifiedModelName>  infoList;
   char nameBuf[2048];

   try {
      thelog->logg(3, "-->GetListOfModelNames\nsessionID=%s\ngroupQualifier=%s\nmodelNamePattern=%s\n\n", sessionID.data(), groupQualifier.data(), modelNamePattern.data());
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
                        fqmn.__set_name(modelName); nModels++;
                        sprintf(nameBuf, "%s.%s", repositoryName, modelName); fqmn.__set_full_path(nameBuf);
                        sprintf(nameBuf, "%llu", resultBuffer[0]); fqmn.__set_modelID(nameBuf);
                        infoList.push_back(fqmn);
                     }
                  }
               }
            }
         }
      } while (numberOfHits > 0);
      _return.__set_status("OK");
      thelog->logg(1, "status=OK\nNumber of models=%llu\n\n\n", nModels);
      _return.__set_models(infoList);
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return.__set_status("Error"); _return.__set_report(errMsg);
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

void VELaSSCoHandler::FindModel(rvOpenModel& _return, const std::string& sessionID, const std::string& modelName, const std::string& requestedAccess)
{
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance) * 2, index = 10, numberOfHits = 2;
   SdaiInstance resultBuffer[2], sdaiModelID;
   bool notFound = true;
   int rstat;
   char condition[1024];

   try {
      thelog->logg(3, "-->FindModel\nsessionID=%s\nmodelName=%s\nrequestedAccess=%s\n\n", sessionID.data(), modelName.data(), requestedAccess.data());
      sprintf(condition, "name like '%s'", modelName.data());
      if (rstat = edmiSelectInstancesBN(edmiGetModelBN("SystemRepository", "ExpressDataManager"), "edm_model", condition, 0,
         maxBufferSize, &index, &numberOfHits, resultBuffer)) {
         _return.__set_modelID("0"); _return.__set_report(getErrorMsg(rstat));
      } else if (numberOfHits == 0) {
         _return.__set_modelID("0"); _return.__set_report("No model match the given model name pattern");
      } else if (numberOfHits == 1) {
         void *sp = sdaiGetAttrBN(resultBuffer[0], "Sdai_Model", sdaiINSTANCE, &sdaiModelID);
         if (sp) {
            if (sdaiOpenModel(sdaiModelID, strEQL(requestedAccess.data(), "read") ? sdaiRO : sdaiRW)) {
               char smodelID[512];
               sprintf(smodelID, "%llu", sdaiModelID);
               _return.__set_modelID(smodelID); _return.__set_report("");
            } else {
               _return.__set_modelID("0"); _return.__set_report(getErrorMsg(sdaiErrorQuery()));
            }
         } else {
            _return.__set_modelID("0"); _return.__set_report(getErrorMsg(rstat));
         }
      } else {
         _return.__set_modelID("0"); _return.__set_report("Several models match the given model name pattern.");
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
void VELaSSCoHandler::CloseModel(std::string& _return, const std::string& sessionID, const std::string& modelName)
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
void VELaSSCoHandler::SetThumbnailOfAModel(std::string& _return, const std::string& sessionID, const std::string& modelID, const std::string& imageFile)
{
   try {
      _return = "SetThumbnailOfAModel" + string(" is not implemented");
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
void VELaSSCoHandler::GetThumbnailOfAModel(rvGetThumbnailOfAModel& _return, const std::string& sessionID, const std::string& modelID)
{
   try {
      _return.__set_status("Error"); _return.__set_report("GetThumbnailOfAModel" + string(" is not implemented"));
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
void VELaSSCoHandler::GetListOfAnalyses(rvGetListOfAnalyses& _return, const std::string& sessionID, const std::string& modelID)
{
   try {
      thelog->logg(2, "-->GetListOfAnalyses\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
      setCurrentSession(sessionID.data());
      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelID.data()));
      if (emc) {
         vector<string> analysisNames;
        if (emc->type == mtDEM) {
            DEMmodelCache *dmc = dynamic_cast<DEMmodelCache*>(emc);
            Iterator<dem::Simulation*, dem::entityType> simIter(dmc->getObjectSet(dem::et_Simulation));
            for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
               analysisNames.push_back(s->get_name());
            }
            _return.__set_status("OK"); _return.__set_analyses(analysisNames);
         } else if (emc->type == mtFEM) {
            FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
            Iterator<fem::ResultHeader*, fem::entityType> rhIter(fmc->getObjectSet(fem::et_ResultHeader));
            map<string, int> names;
            for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
               char *name = rh->get_analysis();
               if (names[rh->get_analysis()] == NULL) {
                  names[rh->get_analysis()] = 1; analysisNames.push_back(rh->get_analysis());
               }
            }
            _return.__set_status("OK"); _return.__set_analyses(analysisNames);
            thelog->logg(0, "status=OK\n\n");
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
* Retrieves the list of time steps for a given model and analysis.
*
* @param sessionID
* @param modelID
*/
void VELaSSCoHandler::CalculateBoundingBox(rvGetListOfAnalyses& _return, const std::string& sessionID, const std::string& modelID)
{
   try {
      thelog->logg(2, "-->CalculateBoundingBox\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
      setCurrentSession(sessionID.data());
      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelID.data()));
      if (emc) {
         vector<string> analysisNames;
        if (emc->type == mtDEM) {
            DEMmodelCache *dmc = dynamic_cast<DEMmodelCache*>(emc);
            Iterator<dem::Simulation*, dem::entityType> simIter(dmc->getObjectSet(dem::et_Simulation));


            _return.__set_status("OK"); _return.__set_analyses(analysisNames);
         } else if (emc->type == mtFEM) {
            FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
            Iterator<fem::Mesh*, fem::entityType> meshIter(fmc->getObjectSet(fem::et_Mesh));
            fem::Mesh *mesh = meshIter.first();
            //while (mesh) {
            //   Iterator<fem::Node*, fem::entityType> nodeIter(mesh->get_nodes());
            //   for (fem::Node *n = nodeIter.first(); n; )
            //}




            _return.__set_status("OK"); _return.__set_analyses(analysisNames);
            thelog->logg(0, "status=OK\n\n");
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
void VELaSSCoHandler::GetBoundingBox(rvGetBoundingBox& _return, const std::string& sessionID, const std::string& modelID, const std::vector<int64_t> & verticesID, const std::string& analysisID, const std::string& timeStepOption, const std::vector<double> & timeSteps)
{
   try {
      thelog->logg(2, "-->GetBoundingBox\nsessionID=%s\nmodelID=%s\n\n", sessionID.data(), modelID.data());
      setCurrentSession(sessionID.data());
      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelID.data()));
      if (emc) {
         vector<string> analysisNames;
         if (emc->type == mtDEM) {
            DEMmodelCache *dmc = dynamic_cast<DEMmodelCache*>(emc);
            Iterator<dem::Simulation*, dem::entityType> simIter(dmc->getObjectSet(dem::et_Simulation));


            _return.__set_status("OK"); 
         } else if (emc->type == mtFEM) {
            FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
            Iterator<fem::Mesh*, fem::entityType> meshIter(fmc->getObjectSet(fem::et_Mesh));
            fem::Mesh *mesh = meshIter.first();
            //while (mesh) {
            //   Iterator<fem::Node*, fem::entityType> nodeIter(mesh->get_nodes());
            //   for (fem::Node *n = nodeIter.first(); n; )
            //}




            _return.__set_status("OK"); 
            thelog->logg(0, "status=OK\n\n");
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
* Retrieves the list of time steps for a given model and analysis.
*
* @param sessionID
* @param modelID
* @param analysisID
*/
void VELaSSCoHandler::GetListOfTimeSteps(rvGetListOfTimeSteps& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID)
{
   try {
      thelog->logg(3, "-->GetListOfTimeSteps\nsessionID=%s\nmodelID=%s\nanalysisID=%s\n\n", sessionID.data(), modelID.data(), analysisID.data());
      setCurrentSession(sessionID.data());
      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelID.data()));
      if (emc) {
         vector<double> timeSteps;
         if (emc->type == mtDEM) {
            DEMmodelCache *dmc = dynamic_cast<DEMmodelCache*>(emc);
            Iterator<dem::Simulation*, dem::entityType> simIter(dmc->getObjectSet(dem::et_Simulation));
            for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
               if (strEQL(s->get_name(), analysisID.data())) {
                  Iterator<dem::Timestep*, dem::entityType> tsIter(s->get_consists_of());
                  for (dem::Timestep *ts = tsIter.first(); ts; ts = tsIter.next()) {
                     timeSteps.push_back(ts->get_time_value());
                  }
               }
            }
            _return.__set_status("OK"); _return.__set_time_steps(timeSteps);
         } else {
            FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
            Iterator<fem::ResultHeader*, fem::entityType> rhIter(fmc->getObjectSet(fem::et_ResultHeader));
            for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
               if (strEQL(rh->get_analysis(), analysisID.data())) {
                  timeSteps.push_back(rh->get_step());
               }
            }
            _return.__set_status("OK"); _return.__set_time_steps(timeSteps);
            thelog->logg(0, "status=OK\n\n");
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
void VELaSSCoHandler::GetListOfMeshes(rvGetListOfMeshes& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep)
{
   try {
      setCurrentSession(sessionID.data());
      EDMmodelCache *emc = setCurrentModelCache(EDM_ATOI64(modelID.data()));
      if (emc) {
         if (emc->type == mtDEM) {
            DEMmodelCache *dmc = dynamic_cast<DEMmodelCache*>(emc);
         } else {
            vector<VELaSSCoSM::MeshInfo> meshInfos;
            FEMmodelCache *fmc = dynamic_cast<FEMmodelCache*>(emc);
            Iterator<fem::Mesh*, fem::entityType> meshIter(fmc->getObjectSet(fem::et_ResultHeader));
            for (fem::Mesh *m = meshIter.first(); m; m = meshIter.next()) {
               //if (strEQL(rh->get_analysis(), analysisID.data())) {
               //   timeSteps.push_back(rh->get_step());
               //}
            }
            _return.__set_status("OK"); _return.__set_meshInfos(meshInfos);
            thelog->logg(0, "status=OK\n\n");
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
/*=============================================================================================================================*/
void  VELaSSCoHandler::CreateClusterModel(rvCreateClusterModel& _return, const std::string& sessionID, const std::string& clusterRepositoryName,
   const std::string& clusterModelName, const std::string& schemaName, const std::vector<std::string> & physicalModelNames)
/*=============================================================================================================================*/
{
   SdaiInteger  index = 0, numberOfHits = 2;
   SdaiInstance resultBuffer[2];
   char condition[1024];

   try {
      sprintf(condition, "(name = '%s') and (belongs_to.name = '%s')", clusterModelName.data(), clusterRepositoryName.data());
      CHECK(edmiFindInstancesBN(theCluster->getClusterModelID(), "ClusterModel", condition, 2 * sizeof(SdaiInstance), &index, &numberOfHits, resultBuffer));
      if (numberOfHits == 0) {
      } else {
         _return.status = "Error"; _return.report = "Model already exist.";
      }
   } catch (CedmError *e) {
      string errMsg;
      handleError(errMsg, e);
      _return.__set_status("Error"); _return.__set_report(errMsg);
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

