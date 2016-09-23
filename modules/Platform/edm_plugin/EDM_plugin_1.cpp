// EDM_server_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#ifdef _WINDOWS
#include <direct.h> 
#endif
#include <iostream>
#include <fstream>
#include <string>
#include <float.h>
#include "EDM_plugin_1.h"
#include "VELaSSCoEDMplugin.h"
#include "FEM_InjectorHandler.h"
#include "../../AccessLib/AccessLib/BoundaryBinaryMesh.h"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#ifndef _WINDOWS
#include "../edm_qm/WindowsFunctionsForLinux.h"
#endif

#define PRINT_EXECTIMES 1


using namespace VELaSSCoSM;

static char *QUERY_RESULT_FOLDER = NULL;

static char *logFileName = NULL;

#ifndef TRACE
#define START_TRACE { FILE *fp = fopen(logFileName, "a"); fprintf
#define END_TRACE fclose(fp); }
#endif 

/*=============================================================================================================================*/
char *VELaSSCoEDMplugin::handleError(CedmError *e)
/*=============================================================================================================================*/
{
   char *errMsg;
   if (e->message) {
      errMsg = e->message;
   } else {
      errMsg = edmiGetErrorText(e->rstat);
   }
   char buf[20480];
   sprintf(buf, "Error: %s - file: %s - line %lld", errMsg, e->fileName, e->lineNo);
   printf("%s\n\n", buf);
   return dllMa->allocString(buf);
}
/*=============================================================================================================================*/
void VELaSSCoEDMplugin::setNodeId(fem::Node *n, EDMLONG nodeId)
/*=============================================================================================================================*/
{
   if (nodeIdArray == NULL) {
      THROW("VELaSSCoEDMplugin::setNodeId - initNodeIdMapping is required")
   }
}
/*=============================================================================================================================*/
void VELaSSCoEDMplugin::initNodeIdMapping(char *fileName)
/*=============================================================================================================================*/
{
   ////Open the file mapping and map it as read-only
   //file_mapping node_file(fileName, read_only);
   //nodeIdRegion = new mapped_region(node_file, read_only);
   //nodeIdArray = (EDMLONG*)nodeIdRegion->get_address();
   //nodeIdArraySize = nodeIdRegion->get_size();
}

/*=============================================================================================================================*/
char *VELaSSCoEDMplugin::getResultFileName(char *fileName, SdaiModel modelId)
/*=============================================================================================================================*/
{
   if ((QUERY_RESULT_FOLDER && *QUERY_RESULT_FOLDER == 0) || (_mkdir(QUERY_RESULT_FOLDER) == 0 || errno == EEXIST)) {
      sprintf(resultFolderBuffer, "%s%s", QUERY_RESULT_FOLDER, repositoryName);
      if (_mkdir(resultFolderBuffer) == 0 || errno == EEXIST) {
         strcat(resultFolderBuffer, "/"); strcat(resultFolderBuffer, modelName);
         if (_mkdir(resultFolderBuffer) == 0 || errno == EEXIST) {
            strcat(resultFolderBuffer, "/");
            sprintf(resultFolderBuffer + strlen(resultFolderBuffer), fileName, modelId);
            return resultFolderBuffer;
         }
      }
   }
   THROW("Path not found in VELaSSCoEDMplugin::getResultFileName.");
   return NULL;
}

/*===============================================================================================*/
EDMVD::ModelType VELaSSCoEDMplugin::getModelType(SdaiModel sdaiModelID)
/*===============================================================================================*/
{
   ModelType mType = mtUnknown;

   char *schemaName = NULL, *modelName = NULL;
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance), index = 0, numberOfHits = 1;
   SdaiInstance schemaID, edmModelID;

   void *sp = sdaiGetAttrBN(sdaiModelID, "edm_model", sdaiINSTANCE, &edmModelID);
   if (sp) {
      sp = sdaiGetAttrBN(edmModelID, "schema", sdaiINSTANCE, &schemaID);
      if (sp) {
         sp = sdaiGetAttrBN(sdaiModelID, "name", sdaiSTRING, &modelName);
         if (sp) {
            sp = sdaiGetAttrBN(schemaID, "name", sdaiSTRING, &schemaName);
            SdaiErrorCode rk = sdaiErrorQuery();
            if (sp && strEQL(schemaName, "DEM_SCHEMA_VELASSCO")) {
               mType = mtDEM;
            } else if (sp && strEQL(schemaName, "FEM_SCHEMA_VELASSCO")) {
               mType = mtFEM;
            }
         } 
         if (modelName) edmiFree(modelName);
         if (schemaName) edmiFree(schemaName);
      }
   }
   return mType;
}




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
* For each point in the input parameter points, the method returns data about the element that contains the point.
* The number of elements in the returned list of elements shall be the same as the number of points in the input parameter.
* If the method does not find an element for a point it shall return a dummy element with id equal to -1.
*
* @param sessionID
* @param modelName
* @param points
*/
//

EDMLONG VELaSSCoEDMplugin::GetResultAtPoints(Model *theModel, ModelType mt, nodeInGetResultAtPoints *inParam, nodeRvGetResultAtPoints *retVal)
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;
#ifdef afadf
   try {
      startTime = GetTickCount();
      if (mt == mtDEM) {
         emsg = "GetListOfResultsFromTimeStepAndAnalysis is not implemented for DEM models.";
      } else {

         Iterator<fem::Element*, fem::entityType> elemIter(theModel->getObjectSet(fem::et_Element), theModel);
         Iterator<fem::Node*, fem::entityType> nodeIter(theModel->getObjectSet(fem::et_Node), theModel);


         bool sequentialSearch = false;
         bool boxSearch = true;
         int n_IsPointInsideTetrahedron_1 = 0;
         int n_IsPointInsideTetrahedron_2 = 0;
         int n_Points = 0;
         std::vector<VELaSSCoSM::Element>  returnedElements_1;
         std::vector<VELaSSCoSM::Element>  returnedElements_2;
         tEdmiInstData elementData;
         elementData.cppObject = NULL; elementData.entityData = &theModel->theEntities[fem::et_Element];
         fem::Element cElement(theModel, &elementData);

         int startTime = GetTickCount();
         if (boxSearch) {
            Matrix<Container<InstanceId>*> *elemBoxMatrix = (Matrix<Container<InstanceId>*> *)theModel->voidElemBoxMatrix;
            int nInside = 0, nPointsInsideSeveralElements = 0;
            std::vector<VELaSSCoSM::Point> myPoints = points;
            for (std::vector<VELaSSCoSM::Point>::iterator p = myPoints.begin(); p != myPoints.end(); p++) {

               n_Points++;
               double p_in_x = p->x, p_in_y = p->y, p_in_z = p->z;
               int bx = p_in_x / fmc->dx, by = p_in_y / fmc->dy, bz = p_in_z / fmc->dz;
               bool outsideAll = true;
               if (bx < fmc->nOf_x && by < fmc->nOf_y && bz < fmc->nOf_z) {
                  Container<InstanceId> *elemBox = elemBoxMatrix->getElement(bx, by, bz);

                  for (InstanceId elementId = elemBox->first(); elementId; elementId = elemBox->next()) {
                     cElement.setInstanceId(elementId);
                     int elemId = cElement.get_id();

                     Iterator<fem::Node*, fem::entityType> nodeOfElemIter(cElement.get_nodes());
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
                           int elemId = cElement.get_id();
                           nInside++;
                           if (outsideAll) {
                              VELaSSCoSM::Element de;
                              de.__set_id(elemId); returnedElements_1.push_back(de);
                           } else {
                              nPointsInsideSeveralElements++;
                           }
                           outsideAll = false;
                        }
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
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   //if (emsg) {
   //   retVal->status->putString("Error"); retVal->report->putString(emsg);
   //   retVal->vertices->type = rptINTEGER;
   //} else {
   //   retVal->status->putString("OK");
   //   retVal->report->putString("");
   //}
#endif
   return rstat;
}



/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetListOfAnalyses(Model *theModel, ModelType mt, nodervGetListOfAnalyses *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;


   try {
      Container<char*> *nameList = new(dllMa)Container<char*>(dllMa);
      if (mt == mtDEM) {
         Iterator<dem::Simulation*, dem::entityType> simIter(theModel->getObjectSet(dem::et_Simulation), theModel);
         for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
            nameList->add(dllMa->allocString(s->get_name()));
         }
         retVal->analysis_name_list->putStringContainer(nameList);
      } else if (mt == mtFEM) {
         Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
         for (fem::Analysis *an = analysisIter.first(); an; an = analysisIter.next()) {
            if (an->exists_name()) {
               nameList->add(dllMa->allocString(an->get_name()));
            }
         }
         retVal->analysis_name_list->putStringContainer(nameList);
      } else {
         emsg = "Model does not exist.";
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
      retVal->analysis_name_list->type = rptINTEGER;
   } else {
      retVal->status->putString("OK");
      retVal->report->type = rptINTEGER;
   }
   return rstat;
}

/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetListOfTimeSteps(Model *theModel, ModelType mt, nodeInGetListOfTimeSteps *inParam, nodeRvGetListOfTimeSteps *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   char fnb[64];

   try {
      char *anid = inParam->analysisID->value.stringVal;
      Container<double> *timeSteps = NULL;
      
      sprintf(fnb, "timesteps_%s_%%llu.dat", mt == mtDEM ? "DEM" : "FEM");
      char *timeStepFileName = getResultFileName(fnb, theModel->modelId);
      FILE *timeStepFile = fopen(timeStepFileName, "rb");
      if (timeStepFile) {
         timeSteps = new(dllMa)Container<double>(dllMa, timeStepFile);
      } else {
         timeSteps = new(dllMa)Container<double>(dllMa);
         if (mt == mtDEM) {
            Iterator<dem::Simulation*, dem::entityType> simIter(theModel->getObjectSet(dem::et_Simulation), theModel);
            for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
               if (strEQL(s->get_name(), anid)) {
                  Iterator<dem::Timestep*, dem::entityType> tsIter(s->get_consists_of());
                  for (dem::Timestep *ts = tsIter.first(); ts; ts = tsIter.next()) {
                     timeSteps->add(ts->get_time_value());
                  }
               }
            }
         } else {
            Iterator<fem::ResultHeader*, fem::entityType> rhIter(theModel->getObjectSet(fem::et_ResultHeader), theModel);
            for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
               if (strEQL(rh->get_analysis(), anid)) {
                  timeSteps->add(rh->get_step());
               }
            }
         }
         timeSteps->writeToBinaryFile(timeStepFileName);
      }
      if (timeSteps) retVal->ListOfTimeSteps->putContainer(timeSteps);
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
      retVal->ListOfTimeSteps->type = rptINTEGER;
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}

/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetListOfVerticesFromMesh(Model *theModel, ModelType mt, nodeInGetListOfVerticesFromMesh *inParam, nodeRvGetListOfVerticesFromMesh *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;

   try {
      startTime = GetTickCount();
      double xx = 1.;
      //for (EDMLONG i = 0; i < 0x100000000; i++) {
      //   xx = xx * xx;
      //}
      endTime = GetTickCount();
      printf("Elapsed time for parallel execution is %d milliseconds\n\n", endTime - startTime);
      char *anid = inParam->analysisID->value.stringVal;
      Container<EDMVD::Vertex> vertices(dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;

      if (mt == mtDEM) {
         //Iterator<dem::Simulation*, dem::entityType> simIter(theModel->getObjectSet(dem::et_Simulation), theModel);
         //for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
         //   if (anid && strEQL(s->get_name(), anid)) {
         //      Iterator<dem::Timestep*, dem::entityType> tsIter(s->get_consists_of(), theModel);
         //      for (dem::Timestep *ts = tsIter.first(); ts; ts = tsIter.next()) {
         //         timeSteps.add(ts->get_time_value());
         //      }
         //   }
         //}
         //retVal->ListOfTimeSteps->putContainer(&timeSteps);
      } else {
         Iterator<fem::Node*, fem::entityType> nodeIter(theModel->getObjectSet(fem::et_Node), theModel);
         for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
            EDMVD::Vertex *v = vertices.createNext();
            v->id = n->get_id(); v->x = n->get_x(); v->y = n->get_y(); v->z = n->get_z();
            if (v->id > maxID) maxID = v->id;
            if (v->id < minID) minID = v->id;
         }
         retVal->vertices->putContainer(&vertices);
         retVal->maxID->putInteger(maxID); retVal->minID->putInteger(minID);
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
      retVal->vertices->type = rptINTEGER;
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}
/*===================================================================================================================*/
fem::Analysis *getFEManalysis(char *anid, Iterator<fem::Analysis*, fem::entityType> *iterp)
/*===================================================================================================================*/
{
   fem::Analysis *cAnalysis;
   for (cAnalysis = iterp->first(); cAnalysis; cAnalysis = iterp->next()) {
      char *an = cAnalysis->get_name();
      if (an && strEQL(an, anid)) {
         return cAnalysis;
      }
   }
   return NULL;
}
static char *LocationTypes[] = { "OnNodes", "OnGaussPoints" };
static char *ValueTypes[] = { "Scalar", "Vector", "Matrix", "PlainDeformationMatrix", "MainMatrix", "LocalAxes", "ComplexScalar", "ComplexVector" };
/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetListOfResultsFromTimeStepAndAnalysis(Model *theModel, ModelType mt, nodeInGetListOfResultsFromTimeStepAndAnalysis *inParam,
   nodeRvGetListOfResultsFromTimeStepAndAnalysis *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;

   try {


      char *anid = inParam->analysisID->value.stringVal;
      Container<EDMVD::Vertex> vertices(dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;

      if (!resultInfoMemory) resultInfoMemory = new CMemoryAllocator(0x100000);
      if (mt == mtDEM) {
         emsg = "GetListOfResultsFromTimeStepAndAnalysis is not implemented for DEM models.";
      } else {
         // The iterator must declared here because the object found by getFEManalysis is allocated in memory of the iterator
         Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
         fem::Analysis *cAnalysis = getFEManalysis(anid, &analysisIter);
         if (cAnalysis) {
            resultInfoMemory->freeAllMemory();
            relocateResultInfo *relocateInfo = (relocateResultInfo *)resultInfoMemory->createRelocateInfo(sizeof(relocateResultInfo));
            Container<EDMVD::ResultInfo> *resInfo = new(resultInfoMemory)Container<EDMVD::ResultInfo>(resultInfoMemory, 16);
            relocateInfo->sResults = resInfo;

            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps());
            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->stepValue->value.realVal) {
                  Iterator<fem::ResultHeader*, fem::entityType> resultIter(ts->get_results());
                  for (fem::ResultHeader *rh = resultIter.first(); rh; rh = resultIter.next()) {
                     EDMVD::ResultInfo *ri = resInfo->createNext();
                     static char *empty = (char*)"";
                     ri->name = resultInfoMemory->allocString(rh->get_name());
                     ri->type = resultInfoMemory->allocString(rh->exists_rType() ? ValueTypes[rh->get_rType()] : empty);
                     ri->location = resultInfoMemory->allocString(rh->exists_location() ? LocationTypes[rh->get_location()] : empty);
                     ri->gaussPointName = resultInfoMemory->allocString(rh->exists_gpName() ? rh->get_gpName() : empty);
                     ri->coordinatesName = resultInfoMemory->allocString(empty);
                     ri->units = resultInfoMemory->allocString(empty);
                     ri->resultNumber = 0;
                     sdaiErrorQuery();
                     ri->componentNames = new(resultInfoMemory)Container<char*>(resultInfoMemory);
                     SdaiAggr cnAID = rh->get_compName_aggrId();
                     ri->nOfComponents = (int)sdaiGetMemberCount(cnAID);
                     for (int ci = 0; ci < ri->nOfComponents; ci++) {
                        char *cn;
                        if (!edmiGetAggrElement(cnAID, ci, sdaiSTRING, &cn)) CHECK(sdaiErrorQuery());
                        ri->componentNames->add(resultInfoMemory->allocString(cn));
                        if (cn) edmiFree(cn);
                     }
                  }
               }
            }
            // Add info about the memory blocks within one memory block
            resultInfoMemory->prepareForRelocationBeforeTransfer();
            // Link the memory allocator to the return value.
            retVal->ResultInfoList->putCMemoryAllocator(resultInfoMemory);
         } else {
            emsg = "Analysis with specified name does not exist.";
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error3"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}

using namespace boost::interprocess;

/*===================================================================================================================*/
void VELaSSCoEDMplugin::addNewResult(Container<EDMVD::ResultOnVertex> *resultsOnVertices, fem::entityType resType, void *vres,
                                 int nOfValuesPrVertex, EDMULONG *maxID, EDMULONG *minID, EDMLONG nodeID)
/*===================================================================================================================*/
{
   fem::Result *res;
   fem::ScalarResult *scalar;
   fem::VectorResult *vector;
   if (resType == fem::et_ScalarResult) {
      scalar = (ScalarResult *)vres; res = scalar;
   } else if (resType == fem::et_VectorResult) {
      vector = (VectorResult *)vres; res = vector;
   }
   if (nodeID == 0) {
      fem::Node *node = res->get_result_for();
      nodeID = node ? node->get_id() : 0;
   }
   EDMVD::ResultOnVertex *rov = resultsOnVertices->createNext();
   if (resType == fem::et_ScalarResult) {
      rov->value[0] = scalar->get_val();
   } else if (resType == fem::et_VectorResult) {
      SdaiAggr cnAID = vector->get_values_aggrId();
      for (int vi = 0; vi < nOfValuesPrVertex; vi++) {
         double d;
         if (!edmiGetAggrElement(cnAID, vi, sdaiREAL, &d)) CHECK(sdaiErrorQuery());
         rov->value[vi] = d;
      }
   } else {
      THROW("Resulttype not supported in GetResultFromVerticesID.");
   }
   rov->id = nodeID;
   if (rov->id > *maxID) *maxID = rov->id;
   if (rov->id < *minID) *minID = rov->id;
}

/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetResultFromVerticesID(Model *theModel, ModelType mt, nodeInGetResultFromVerticesID *inParam,
   nodeRvGetResultFromVerticesID *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;

   try {
      startTime = GetTickCount();
      bool timeStepFound = false, resultIdFound = false;

      char *analysisID = inParam->analysisID->value.stringVal;
      char *resultID = inParam->resultID->value.stringVal;
      //listOfVertices
      Container<EDMVD::Vertex> vertices(dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;

      if (!resultInfoMemory) resultInfoMemory = new CMemoryAllocator(0x100000);
      if (mt == mtDEM) {
         emsg = "GetListOfResultsFromTimeStepAndAnalysis is not implemented for DEM models.";
      } else {
         Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
         fem::Analysis *cAnalysis = getFEManalysis(analysisID, &analysisIter);
         if (cAnalysis) {
            resultInfoMemory->freeAllMemory();
            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps());
            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->timeStep->value.realVal) {
                  timeStepFound = true;
                  Iterator<fem::ResultHeader*, fem::entityType> resultHeaderIter(ts->get_results());
                  for (fem::ResultHeader *rh = resultHeaderIter.first(); rh; rh = resultHeaderIter.next()) {
                     char *rhname = rh->get_name();
                     if (rhname && strEQL(rhname, resultID)) {
                        if (!resultIdFound) {
                           fem::Mesh *mesh = ts->get_mesh();
                           InstanceId resultId = rh->getInstanceId();
                           char *nodeIdFileName = getResultFileName("nodeId_%llu.dat", mesh->getInstanceId());
                           int nOfValuesPrVertex;

                           initNodeIdMapping(nodeIdFileName);

                           Iterator<fem::Result*, fem::entityType> valuestIter(rh->get_values());
                           fem::entityType resType;
                           
                           void *vres = valuestIter.first(&resType);
                           fem::Result *res;
                           fem::ScalarResult *scalar;
                           fem::VectorResult *vector;
                           if (resType == fem::et_ScalarResult) {
                              scalar = (ScalarResult *)vres; res = scalar; nOfValuesPrVertex = 1;
                           } else if (resType == fem::et_VectorResult) {
                              vector = (VectorResult *)vres; res = vector;
                              SdaiAggr cnAID = vector->get_values_aggrId();
                              nOfValuesPrVertex = (int)sdaiGetMemberCount(cnAID);
                           }

                           relocateResultOnVertex *relocateInfo = (relocateResultOnVertex *)resultInfoMemory->createRelocateInfo(sizeof(relocateResultOnVertex));
                           Container<EDMVD::ResultOnVertex> *resultsOnVertices = new(resultInfoMemory)Container<EDMVD::ResultOnVertex>(resultInfoMemory, 1024, (sizeof(double) * (nOfValuesPrVertex - 1)));
                           relocateInfo->vertexResults = resultsOnVertices;

                           EDMLONG *nodeIdArray = NULL;
                           EDMLONG bufferSize;

                           EDMULONG nOfInputVertices = inParam->listOfVertices->getAggrSize();
                           if (nOfInputVertices > 0) {
                              bool haveTriedToOpen = false;
getResultValues:
                              try {
                                 //Open the file mapping and map it as read-only
                                 file_mapping node_file(nodeIdFileName, read_only);
                                 mapped_region node_region(node_file, read_only);
                                 nodeIdArray = (EDMLONG*)node_region.get_address();
                                 bufferSize = node_region.get_size();
                                 
                                 maxID = nodeIdArray[1]; minID = nodeIdArray[0];
                                 EDMLONG *vertices = inParam->listOfVertices->value.intAggrVal;
                                 for (int i = 0; i < nOfInputVertices; i++) {
                                    if (vertices[i] >= minID && vertices[i] <= maxID) {
                                       SdaiInstance nodeId = nodeIdArray[vertices[i] - minID + 2];
                                       if (nodeId) {
                                          tEdmiInstData nd;
                                          nd.instanceId = nodeId; nd.entityData = &theModel->theEntities[fem::et_Node];
                                          fem::Node thisNode(theModel, &nd);
                                          Iterator<fem::Result*, fem::entityType> valuesOfNodeIter(thisNode.get_results());
                                          fem::entityType resType;

                                          for (void *vres = valuesOfNodeIter.first(&resType); vres; vres = valuesOfNodeIter.next(&resType)) {
                                             fem::Result *res;
                                             fem::ScalarResult *scalar;
                                             fem::VectorResult *vector;
                                             if (resType == fem::et_ScalarResult) {
                                                scalar = (ScalarResult *)vres; res = scalar;
                                             } else if (resType == fem::et_VectorResult) {
                                                vector = (VectorResult *)vres; res = vector;
                                             }
                                             fem::ResultHeader *thisRes = res->get_result_header();
                                             if (thisRes && thisRes->getInstanceId() == resultId) {
                                                addNewResult(resultsOnVertices, resType, vres, nOfValuesPrVertex, &maxID, &minID, thisNode.get_id());
                                             }
                                          }
                                       }
                                    }
                                 }
                              } catch (boost::interprocess::interprocess_exception exep) {
                                 if (haveTriedToOpen) {
                                    THROW("VELaSSCoEDMplugin::GetResultFromVerticesID - Internal error.");
                                 }
                                 haveTriedToOpen = true;
                                 Iterator<fem::Node *, fem::entityType> nodeIter(mesh->get_nodes());
                                 for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
                                    EDMLONG id = n->get_id();
                                    if (id > maxID) maxID = id; if (id < minID) minID = id;
                                 }
                                 const std::size_t fileSize = (maxID - minID + 3) * sizeof(EDMLONG);

                                 nodeIdArray = (EDMLONG *)dllMa->alloc(fileSize);
                                 memset(nodeIdArray, 0, fileSize);
                                 nodeIdArray[0] = minID;
                                 nodeIdArray[1] = maxID;
                                 for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
                                    EDMLONG id = n->get_id();
                                    nodeIdArray[id - minID + 2] = n->getInstanceId();
                                 }
                                 FILE *bfp = fopen(nodeIdFileName, "wb");
                                 if (bfp) {
                                    size_t nWritten = fwrite(nodeIdArray, 1, fileSize, bfp);
                                    if (nWritten != fileSize) THROW("Error when writing to binary file in VELaSSCoEDMplugin::GetResultFromVerticesID.");
                                    fclose(bfp);
                                 } else {
                                    THROW("Cannot open binary file in VELaSSCoEDMplugin::GetResultFromVerticesID.");
                                 }
                                 goto getResultValues;
                              }
                           } else {
                              // get result for all nodes in the mesh!!
                              for (void *vres = valuestIter.first(&resType); vres; vres = valuestIter.next(&resType)) {

                                 addNewResult(resultsOnVertices, resType, vres, nOfValuesPrVertex, &maxID, &minID, 0);

                              }
                           }
                           retVal->nOfValuesPrVertex->putInteger(nOfValuesPrVertex);
                           // Add info about the memory blocks within one memory block
                           resultInfoMemory->prepareForRelocationBeforeTransfer();
                           // Link the memory allocator to the return value.
                           retVal->result_list->putCMemoryAllocator(resultInfoMemory);
                        } else {
                           THROW("More than one result header satisfy the search criterias.")
                        }
                     }
                  }
               }
            }
            retVal->maxID->putInteger(maxID); retVal->minID->putInteger(minID);
         } else {
            emsg = "Analysis with specified name does not exist.";
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   endTime = GetTickCount();
   int execTime = endTime - startTime;
   if (emsg) {
      retVal->status->putString("Error4"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}
//typedef enum { ElementType_POINT, ElementType_LINE, ElementType_TRIANGLE, ElementType_QUADRILATERAL, ElementType_TETRAHEDRA, ElementType_HEXAHEDRA, ElementType_PRISM, ElementType_PYRAMID, ElementType_SPHERE, ElementType_CIRCLE } ElementType;

static ElementShapeType::type elementTypeConvert[] = {
   ElementShapeType::type::PointElement, ElementShapeType::type::LineElement,  ElementShapeType::type::TriangleElement,
   ElementShapeType::type::QuadrilateralElement, ElementShapeType::type::TetrahedraElement, ElementShapeType::type::HexahedraElement,
   ElementShapeType::type::PrismElement, ElementShapeType::type::PyramidElement, ElementShapeType::type::SphereElement, ElementShapeType::type::CircleElement,
   ElementShapeType::type::ComplexParticleElement
};
/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetCoordinatesAndElementsFromMesh(Model *theModel, ModelType mt, nodeInGetCoordinatesAndElementsFromMesh *inParam,
   nodeRvGetCoordinatesAndElementsFromMesh *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime;

   try {
      bool timeStepFound = false, resultIdFound = false;
      char *analysisID = inParam->analysisID->value.stringVal;
      char *meshName = inParam->meshName->value.stringVal;
      Container<EDMVD::Vertex> vertices(dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      ReturnedMeshInfo *metaData = (ReturnedMeshInfo*)dllMa->alloc(sizeof(metaData));

      startTime = GetTickCount();
      if (mt == mtDEM) {
         emsg = "GetCoordinatesAndElementsFromMesh is not implemented for DEM models.";
      } else {
         Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
         fem::Analysis *cAnalysis = getFEManalysis(analysisID, &analysisIter);
         if (cAnalysis) {
            emsg = "Specified time step not found.";
            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps());
            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->timeStep->value.realVal) {
                  timeStepFound = true;
                  fem::Mesh *mesh = ts->get_mesh();


                  char *vertexFileName = getResultFileName("vertices_%llu.dat", mesh->getInstanceId());
                  FILE *vertexFile = fopen(vertexFileName, "rb");
                  Container<EDMVD::Vertex> *vertices;
                  maxID = 0, minID = 0xfffffffffffffff;

                  if (vertexFile) {
                     vertices = new(dllMa)Container<EDMVD::Vertex>(dllMa, vertexFile);
                     for (EDMVD::Vertex *v = vertices->firstp(); v; v = vertices->nextp()) {
                        if (v->id > maxID) maxID = v->id; if (v->id < minID) minID = v->id;
                     }
                  } else {
                     vertices = new(dllMa)Container<EDMVD::Vertex>(dllMa, 0x10000);
                     Iterator<fem::Node*, fem::entityType> nodeIter(mesh->get_nodes());
                     for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
                        EDMVD::Vertex *v = vertices->createNext();
                        v->id = n->get_id();
                        v->x = n->get_x(); v->y = n->get_y(); v->z = n->get_z();
                        if (v->id > maxID) maxID = v->id; if (v->id < minID) minID = v->id;
                     }
                     vertices->writeToBinaryFile(vertexFileName);
                  }
                  metaData->minNodeID = minID; metaData->maxNodeID = maxID;
                  metaData->n_vertices = vertices->size();
                  metaData->vertex_record_size = sizeof(EDMVD::Vertex);
                  retVal->node_array->putContainer(vertices);



                  char *elementFileName = getResultFileName("elements_%llu.dat", mesh->getInstanceId());
                  FILE *elementFile = fopen(elementFileName, "rb");
                  EDMLONG n_vertices_pr_element = mesh->get_numberOfNodes();
                  Container<EDMVD::FEMelement> *elements;
                  EDMLONG extraVertexSize = sizeof(EDMULONG)* (n_vertices_pr_element - 1);
                  maxID = 0, minID = 0xfffffffffffffff;

                  if (elementFile) {
                     elements = new(dllMa)Container<EDMVD::FEMelement>(dllMa, elementFile, extraVertexSize);
                     for (EDMVD::FEMelement *e = elements->firstp(); e; e = elements->nextp()) {
                        if (e->id > maxID) maxID = e->id; if (e->id < minID) minID = e->id;
                     }
                  } else {
                     EDMLONG n_vertices_pr_element = mesh->get_numberOfNodes();
                     elements = new(dllMa)Container<EDMVD::FEMelement>(dllMa, 0x10000, extraVertexSize);
                     Iterator<fem::Element*, fem::entityType> elemIter(mesh->get_elements());
                     for (fem::Element *e = elemIter.first(); e; e = elemIter.next()) {
                        EDMVD::FEMelement *element = elements->createNext();
                        element->id = e->get_id();
                        if (element->id > maxID) maxID = element->id; if (element->id < minID) minID = element->id;
                        Iterator<fem::Node*, fem::entityType> nodeIter(e->get_nodes());
                        int i = 0;
                        for (fem::Node *n = nodeIter.first(); n && i < n_vertices_pr_element; n = nodeIter.next()) {
                           element->nodes_ids[i++] = n->get_id();
                        }
                     }
                     elements->writeToBinaryFile(elementFileName);
                  }
                  metaData->n_elements = elements->size();

                  //EDMLONG n_vertices_pr_element = mesh->get_numberOfNodes();
                  //Container<EDMVD::FEMelement> *elements = new(dllMa) Container<EDMVD::FEMelement>(dllMa, 0x10000, sizeof(EDMULONG) * (n_vertices_pr_element - 1));
                  //Iterator<fem::Element*, fem::entityType> elemIter(mesh->get_elements());
                  //for (fem::Element *e = elemIter.first(); e; e = elemIter.next()) {
                  //   EDMVD::FEMelement *element = elements->createNext();
                  //   element->id = e->get_id();
                  //   if (element->id > maxID) maxID = element->id; if (element->id < minID) minID = element->id;
                  //   Iterator<fem::Node*, fem::entityType> nodeIter(e->get_nodes());
                  //   int i = 0;
                  //   for (fem::Node *n = nodeIter.first(); n && i < n_vertices_pr_element; n = nodeIter.next()) {
                  //      element->nodes_ids[i++] = n->get_id();
                  //   }
                  //}
                  //metaData->element_type = mesh->exists_elementType() ? elementTypeConvert[mesh->get_elementType()] : UnknownElement;
                  metaData->element_record_size = sizeof(EDMVD::FEMelement) + sizeof(EDMULONG) * (n_vertices_pr_element - 1);
                  metaData->n_vertices_pr_element = n_vertices_pr_element;
                  metaData->minElementID = minID; metaData->maxElementID = maxID;
                  retVal->elemnt_array->putContainer(elements);
                  retVal->returned_mesh_info->putBlob(metaData, sizeof(ReturnedMeshInfo));
                  emsg = NULL;
               }
            }
         } else {
            emsg = "Analysis with specified name not found.";
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}



#define MaxElemRef 1000
typedef struct NodeInfo {
   int elemRefs[MaxElemRef];
   int nElemRefs;
   //fem::Node*theNode;
   InstanceId nodeID;

   void findNodeInfo(fem::Node* np, Model *theModel)
   {
      nodeID = np->get_id(); nElemRefs = 0;
      Iterator<fem::Element*, fem::entityType> elemIter(np->get_elements());
      for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
         if (nElemRefs < MaxElemRef) {
            //elemRefs[nElemRefs++] = ep->get_id();
            int i, id = ep->get_id();
            for (i = 0; i < nElemRefs && id > elemRefs[i]; i++);
            if (i < nElemRefs) {
               for (int j = nElemRefs; j > i; j--) elemRefs[j] = elemRefs[j - 1];
            }
            elemRefs[i] = id; nElemRefs++;
         } else {
            //vh->ReportError("nElemRefs > MaxElemRef in CalculateBoundaryOfMesh\n");
         }
      }
   }
} NodeInfo;

/*-------------------------------------------------------------------------------------------------------------------*/
int findElementsOfTriangle(NodeInfo &n1, NodeInfo &n2, NodeInfo &n3)
/*-------------------------------------------------------------------------------------------------------------------*/
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
#define swap(a, b) {EDMULONG t = *a; *a=*b; *b = t; }
/*-------------------------------------------------------------------------------------------------------------------*/
void CalculateBoundaryOfMesh(List<fem::Element*> *elementAggr, Container<EDMVD::Triangle>  *triangles, Model *theModel)
/*-------------------------------------------------------------------------------------------------------------------*/
{

   Iterator<fem::Element*, fem::entityType> elemIter(elementAggr);

   for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
      Iterator<fem::Node*, fem::entityType> nodeIter(ep->get_nodes());
      if (nodeIter.size() == 4) {
         NodeInfo nodes[4];
         int ix = 0;
         for (fem::Node* np = nodeIter.first(); np; np = nodeIter.next()) {
            nodes[ix++].findNodeInfo(np, theModel);
         }
         static int nodesInTriangles[4][3] = {
            { 0, 1, 2 },
            { 0, 1, 3 },
            { 0, 2, 3 },
            { 1, 2, 3 },
         };
         for (int i = 0; i < 4; i++) {
            NodeInfo *ni0 = &nodes[nodesInTriangles[i][0]], *ni1 = &nodes[nodesInTriangles[i][1]], *ni2 = &nodes[nodesInTriangles[i][2]];
            if (findElementsOfTriangle(*ni0, *ni1, *ni2) == 1) {
               EDMVD::Triangle *t = triangles->createNext();
               t->node_ids[0] = ni0->nodeID;
               t->node_ids[1] = ni1->nodeID;
               t->node_ids[2] = ni2->nodeID;
               EDMULONG *idp0 = &t->node_ids[0], *idp1 = &t->node_ids[1], *idp2 = &t->node_ids[2];
               if (*idp0 > *idp1) swap(idp0, idp1);
               if (*idp1 > *idp2) {
                  swap(idp1, idp2);
                  if (*idp0 > *idp1) swap(idp0, idp1);
               }
               if (t->node_ids[0] > t->node_ids[1] || t->node_ids[1] > t->node_ids[2]) {
                  int asdfasdf = 999;
               }
            }
         }
      } else {
         THROW("Not tetrahedral elements in CalculateBoundaryOfMesh\n");
      }
   }
}
/*-------------------------------------------------------------------------------------------------------------------*/
void GetAllTriangles(List<fem::Element*> *elementAggr, Container<EDMVD::Triangle>  *triangles, Model *theModel)
/*-------------------------------------------------------------------------------------------------------------------*/
{

   Iterator<fem::Element*, fem::entityType> elemIter(elementAggr);

   for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
      Iterator<fem::Node*, fem::entityType> nodeIter(ep->get_nodes());
      if (nodeIter.size() == 4) {
         InstanceId nodeIds[4];
         int ix = 0;
         for (fem::Node* np = nodeIter.first(); np; np = nodeIter.next()) {
            nodeIds[ix++] = np->get_id();
         }
         static int nodesInTriangles[4][3] = {
            { 0, 1, 2 },
            { 0, 1, 3 },
            { 0, 2, 3 },
            { 1, 2, 3 },
         };
         for (int i = 0; i < 4; i++) {
            InstanceId id0 = nodeIds[nodesInTriangles[i][0]], id1 = nodeIds[nodesInTriangles[i][1]], id2 = nodeIds[nodesInTriangles[i][2]];
            EDMVD::Triangle *t = triangles->createNext();
            t->node_ids_orig[0] = t->node_ids[0] = id0;
            t->node_ids_orig[1] = t->node_ids[1] = id1;
            t->node_ids_orig[2] = t->node_ids[2] = id2;
            EDMULONG *idp0 = &t->node_ids[0], *idp1 = &t->node_ids[1], *idp2 = &t->node_ids[2];
            if (*idp0 > *idp1) swap(idp0, idp1);
            if (*idp1 > *idp2) {
               swap(idp1, idp2);
               if (*idp0 > *idp1) swap(idp0, idp1);
            }
            if (t->node_ids[0] > t->node_ids[1] || t->node_ids[1] > t->node_ids[2]) {
               int asdfasdf = 999;
            }
         }
      } else {
         THROW("Not tetrahedral elements in CalculateBoundaryOfMesh\n");
      }
   }
}
static int compareTriangle(const void *a, const void *b)
{
   EDMVD::Triangle *at = (EDMVD::Triangle*)a, *bt = (EDMVD::Triangle*)b;
   if (at->node_ids[0] > bt->node_ids[0]) {
      return 1;
   } else if (at->node_ids[0] == bt->node_ids[0]) {
      if (at->node_ids[1] > bt->node_ids[1]) {
         return 1;
      } else if (at->node_ids[1] == bt->node_ids[1]) {
         if (at->node_ids[2] > bt->node_ids[2]) {
            return 1;
         } else if (at->node_ids[2] == bt->node_ids[2]) {
            return 0;
         } else {
            return -1;
         }
      } else {
         return -1;
      }
   } else {
      return -1;
   }
}

void writeTrianglesToFile(char *fileName, Container<EDMVD::Triangle> *cont)
{
   int nError = 0;
   FILE *triangleFile = fopen(fileName, "w");
   if (triangleFile) {
      EDMVD::Triangle *p = NULL;
      for (EDMVD::Triangle *t = cont->firstp(); t; t = cont->nextp()) {
         if (p) {
            if (p->conpare(t) >= 0) {
               //THROW("Merge error in VELaSSCoMethods::GetBoundaryOfLocalMesh.")
               if (++nError < 100)
                  fprintf(triangleFile, "Merge error in VELaSSCoMethods::GetBoundaryOfLocalMesh.");
            }
         }
         p = t;
         fprintf(triangleFile, "%10llu%10llu%10llu%10llu%10llu%10llu\n", t->node_ids[0], t->node_ids[1], t->node_ids[2], t->node_ids_orig[0], t->node_ids_orig[1], t->node_ids_orig[2]);
      }
      fclose(triangleFile);
   }
}
/*===================================================================================================================*/
void VELaSSCoEDMplugin::findTrianglesOfMesh(Model *theModel, fem::Mesh *mesh, nodeRvGetBoundaryOfLocalMesh *retVal)
/*===================================================================================================================*/
{
   char *triangelFileName = getResultFileName("triangles_%llu.dat", theModel->modelId);
   FILE *triangelFile = fopen(triangelFileName, "rb");
   EDMULONG nTrianglesPrContainerBuffer = 0x10000;
   Container<EDMVD::Triangle>  *triangles;

   if (triangelFile) {
      triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, triangelFile);
   } else {
      Container<EDMVD::Triangle>  *initial_triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, nTrianglesPrContainerBuffer);

      GetAllTriangles(mesh->get_elements(), initial_triangles, theModel);

      if (initial_triangles->size() > nTrianglesPrContainerBuffer) {
         Container<EDMVD::Triangle>  *sortable_triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, initial_triangles);
         initial_triangles = sortable_triangles;
      }
      initial_triangles->sort(compareTriangle);
                     
      EDMVD::Triangle *prev = initial_triangles->firstp();
      triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, nTrianglesPrContainerBuffer);
      for (EDMVD::Triangle *t = initial_triangles->nextp(); t; t = initial_triangles->nextp()) {
         if (prev->conpare(t) == 0) {
            prev = initial_triangles->nextp();
         } else {
            EDMVD::Triangle *newt = triangles->createNext();
            *newt = *prev; prev = t;
         }
      }
      if (prev) *triangles->createNext() = *prev;

      triangles->writeToBinaryFile(triangelFileName);
   }
   retVal->triangle_record_size->putInteger(sizeof(EDMVD::Triangle));
   retVal->n_triangles->putInteger(triangles->size());
   retVal->triangle_array->putContainer(triangles);
}
/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetBoundaryOfLocalMesh(Model *theModel, ModelType mt, nodeInGetBoundaryOfLocalMesh *inParam,
   nodeRvGetBoundaryOfLocalMesh *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime = GetTickCount();

   try {
      bool timeStepFound = false, resultIdFound = false;
      char *analysisID = inParam->analysisID->value.stringVal;
      char *meshID = inParam->meshID->value.stringVal;
      Container<EDMVD::Vertex> vertices(dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      ReturnedMeshInfo *metaData = (ReturnedMeshInfo*)dllMa->alloc(sizeof(metaData));

      if (mt == mtDEM) {
         emsg = "GetCoordinatesAndElementsFromMesh is not implemented for DEM models.";
      } else {
         if (analysisID == NULL || *analysisID == 0) {
            Iterator<fem::Mesh*, fem::entityType> meshIter(theModel->getObjectSet(fem::et_Mesh), theModel);
            fem::Mesh* mesh = meshIter.first();
            if (mesh) {
               findTrianglesOfMesh(theModel, mesh, retVal);
            }
         } else {
            Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
            fem::Analysis *cAnalysis = getFEManalysis(analysisID, &analysisIter);
            if (cAnalysis) {
               emsg = "Specified time step not found.";
               Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps());
               for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
                  if (ts->get_time_value() == inParam->timeStep->value.realVal) {
                     timeStepFound = true;
                     fem::Mesh *mesh = ts->get_mesh();
                  
                     //char *triangelFileName = getResultFileName("triangles_%llu.dat", theModel->modelId);
                     //FILE *triangelFile = fopen(triangelFileName, "rb");
                     //EDMULONG nTrianglesPrContainerBuffer = 0x10000;
                     //Container<EDMVD::Triangle>  *triangles;

                     //int nmilliSec = GetTickCount() - startTime;
                     //printf("Finding mesh time=%d\n", nmilliSec);
                     //if (triangelFile) {
                     //   triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, triangelFile);
                     //} else {
                     //   Container<EDMVD::Triangle>  *initial_triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, nTrianglesPrContainerBuffer);

                     //   GetAllTriangles(mesh->get_elements(), initial_triangles, theModel);

                     //   if (initial_triangles->size() > nTrianglesPrContainerBuffer) {
                     //      Container<EDMVD::Triangle>  *sortable_triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, initial_triangles);
                     //      initial_triangles = sortable_triangles;
                     //   }
                     //   initial_triangles->sort(compareTriangle);
                     //   
                     //   EDMVD::Triangle *prev = initial_triangles->firstp();
                     //   triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, nTrianglesPrContainerBuffer);
                     //   for (EDMVD::Triangle *t = initial_triangles->nextp(); t; t = initial_triangles->nextp()) {
                     //      if (prev->conpare(t) == 0) {
                     //         prev = initial_triangles->nextp();
                     //      } else {
                     //         EDMVD::Triangle *newt = triangles->createNext();
                     //         *newt = *prev; prev = t;
                     //      }
                     //   }
                     //   if (prev) *triangles->createNext() = *prev;

                     //   triangles->writeToBinaryFile(triangelFileName);
                     //}
                     //retVal->triangle_record_size->putInteger(sizeof(EDMVD::Triangle));
                     //retVal->n_triangles->putInteger(triangles->size());
                     //retVal->triangle_array->putContainer(triangles);

                     findTrianglesOfMesh(theModel, mesh, retVal);
                     emsg = NULL;
                  }
               }
            } else {
               emsg = "Analysis with specified name not found.";
            }
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}






void VELaSSCoEDMplugin::addMeshInfo(Container<EDMVD::MeshInfoEDM> *meshContainer, fem::Mesh *mesh)
{
   if (mesh) {
      EDMVD::MeshInfoEDM *mi = meshContainer->createNext();
      mi->name = resultInfoMemory->allocString(mesh->exists_name() ? mesh->get_name() : (char*)"");
      //mi->elementType.shape.type = VELaSSCoSM::ElementShapeType::UnknownElement;
      //mi->elementType.shape = mesh->exists_elementType() ? elementTypeConvert[mesh->get_elementType()] : VELaSSCoSM::ElementShapeType::UnknownElement;
      mi->elemType = mesh->exists_elementType() ? elementTypeConvert[mesh->get_elementType()] : VELaSSCoSM::ElementShapeType::UnknownElement;
      List<fem::Element*>* elems = mesh->get_elements();
      List<fem::Node*>* nodes = mesh->get_nodes();
      mi->nElements = elems ? elems->size() : 0;
      mi->nVertices = nodes ? nodes->size() : 0;
      mi->num_nodes = (EDMULONG32)mi->nVertices;
      mi->meshUnits = mi->meshColor = mi->coordsName = NULL;
   }
}
/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetListOfMeshes(Model *theModel, ModelType mt, nodeInGetListOfMeshes *inParam,
   nodeRvGetListOfMeshes *retVal)
/*
	Description: returns a list of meshes present for the given time-step of that analysis.
   If analysis == "" and step-value == -1 then the list will be of the 'static' meshes.
   If analysis != "" and step-value != -1 then the list will be of the 'dynamic' meshes that
   are present on that step-values of that analysis.
===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;

   try {
      bool timeStepFound = false, resultIdFound = false;
      char *analysisID = inParam->analysisID->value.stringVal;
      Container<EDMVD::Vertex> vertices(dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      ReturnedMeshInfo *metaData = (ReturnedMeshInfo*)dllMa->alloc(sizeof(metaData));

      if (!resultInfoMemory) resultInfoMemory = new CMemoryAllocator(0x100000);
      if (mt == mtDEM) {
         emsg = "GetListOfMeshes is not implemented for DEM models.";
      } else {
         emsg = "Analysis with specified name not found.";
         resultInfoMemory->freeAllMemory();
         relocateMeshInfo *relocateInfo = (relocateMeshInfo *)resultInfoMemory->createRelocateInfo(sizeof(relocateMeshInfo));
         Container<EDMVD::MeshInfoEDM> *meshContainer = new(resultInfoMemory)Container<EDMVD::MeshInfoEDM>(resultInfoMemory, 16);
         relocateInfo->meshes = meshContainer;

         if (analysisID == NULL || *analysisID == 0) {
            Iterator<fem::Mesh*, fem::entityType> meshIter(theModel->getObjectSet(fem::et_Mesh), theModel);
            for (fem::Mesh *mesh = meshIter.first(); mesh; mesh = meshIter.next()) {
               addMeshInfo(meshContainer, mesh);
            }
         } else {
            Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
            for (fem::Analysis *cAnalysis = analysisIter.first(); cAnalysis; cAnalysis = analysisIter.next()) {
               char *anid = cAnalysis->get_name();
               if (anid && strEQL(analysisID, anid)) {
                  emsg = "Specified time step not found.";
                  Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps());

                  for (fem::TimeStep *ts = tsIter.first(); ts && !timeStepFound; ts = tsIter.next()) {
                     if (ts->get_time_value() == inParam->timeStep->value.realVal || inParam->timeStep->value.realVal == -1.0) {
                        timeStepFound = true;

                        addMeshInfo(meshContainer, ts->get_mesh());

                     }
                  }
               }
            }
         }
         if (meshContainer->size() > 0) {
            // Add info about the memory blocks within one memory block
            resultInfoMemory->prepareForRelocationBeforeTransfer();
            // Link the memory allocator to the return value.
            retVal->mesh_info_list->putCMemoryAllocator(resultInfoMemory);
            emsg = NULL;
         } else {
            emsg = "No mesh satisfying the search criteria found.";
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}

double maxZ = 0.0, maxX = 0.0;

void calculateTheBox(fem::Mesh*mesh, double *return_bbox, int nOfVertices, EDMLONG *vertices, char **errmsgp)
{
   if (mesh) {
      if (nOfVertices == 0) {
         Iterator<fem::Node*, fem::entityType> nodeIter(mesh->get_nodes());
         for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
            double x = n->get_x(), y = n->get_y(), z = n->get_z();
            if (x > return_bbox[max_X]) return_bbox[max_X] = x;
            if (x < return_bbox[min_X]) return_bbox[min_X] = x;
            if (y > return_bbox[max_Y]) return_bbox[max_Y] = y;
            if (y < return_bbox[min_Y]) return_bbox[min_Y] = y;
            if (z > return_bbox[max_Z]) return_bbox[max_Z] = z;
            if (z < return_bbox[min_Z]) return_bbox[min_Z] = z;
            if (z > maxZ)
               maxZ = z;
            if (x > maxX)
               maxX = x;
         }
      }
      *errmsgp = NULL;
   } else {
      *errmsgp = "VELaSSCoEDMplugin::CalculateBoundingBox error: Mesh not found.";
   }
}


/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::CalculateBoundingBox(Model *theModel, ModelType mt,
   nodeInCalculateBoundingBox *inParam, nodeRvCalculateBoundingBox *retVal)
/*
   The VQ calculates the axis aligned Bounding Box of a model id for a static (FEM) or a dynamic (DEM) mesh taking into
   account an input list of Vertices ID. If the list of Vertices ID is empty, then all vertices are considered.
   For dynamic mesh, the Bounding Box is calculated considering one of the following time_step_options:
   •	ALL: the meshes of all time-steps are considered.
   •	SINGLE: only the mesh of the specified time-step is considered.
   •	INTERVAL: the meshes of the time-steps within an interval defined in the input list of time-steps are considered.
      The input list of time-steps must contain two values. The first value of input list (time_steps[0]) corresponds
      to the first time-step of the interval and the second value (time_steps[1]) to the last time-step of the interval.
      Time_steps[0] must be lower or equal to time_steps[1]. 
   •	SET: the meshes of the time-steps included in the input list of time-steps are considered. The time-steps in
      the list do not need to be correlative.

   The VQ returns the coordinates (x,y,z) of the vertices that define the Bounding Box (6 doubles) 
===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;
   double return_bbox[6];

   try {
      return_bbox[min_X] = return_bbox[min_Y] = return_bbox[min_Z] = DBL_MAX;
      return_bbox[max_X] = return_bbox[max_Y] = return_bbox[max_Z] = DBL_MIN;

      bool timeStepFound = false, resultIdFound = false;
      char *analysisID = inParam->analysisID->value.stringVal;
      EDMLONG nOfTimeSteps = inParam->timeSteps->getAggrSize();
      EDMLONG nOfVertices = inParam->vertexIDs->getAggrSize();
      double *timeSteps = inParam->timeSteps->value.realAggrVal;
      EDMLONG *vertices = inParam->vertexIDs->value.intAggrVal;

      startTime = GetTickCount();
      if (mt == mtDEM) {
         emsg = "CalculateBoundingBox is not implemented for DEM models.";
      } else {
         if (analysisID == NULL || *analysisID == 0) {
            Iterator<fem::Mesh*, fem::entityType> meshIter(theModel->getObjectSet(fem::et_Mesh), theModel);
            if (meshIter.size() > 1) {
               emsg = "VELaSSCoEDMplugin::CalculateBoundingBox error: More than one mesh in the specified model.";
            } if (meshIter.size() == 0) {
               emsg = "VELaSSCoEDMplugin::CalculateBoundingBox error: No mesh in the specified model.";
            } else {
               for (fem::Mesh *mesh = meshIter.first(); mesh; mesh = meshIter.next()) {
                  calculateTheBox(mesh, return_bbox, nOfVertices, vertices, &emsg);
               }
            }
         } else {
            emsg = "Analysis with specified name not found.";
            Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
            fem::Analysis *cAnalysis = getFEManalysis(analysisID, &analysisIter);
            if (cAnalysis) {
               Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps());
               for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
                  double cts = ts->get_time_value();
                  int i;
                  for (i=0; i < nOfTimeSteps && timeSteps[i] != cts; i++) ;
                  if (i < nOfTimeSteps) {
                     timeStepFound = true;

                     calculateTheBox(ts->get_mesh(), return_bbox, nOfVertices, vertices, &emsg);
                     //if (mesh) {
                     //   if (nOfVertices == 0) {
                     //      Iterator<fem::Node*, fem::entityType> nodeIter(mesh->get_nodes());
                     //      for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
                     //         double x = n->get_x(), y = n->get_y(), z = n->get_z();
                     //         if (x > return_bbox[max_X]) return_bbox[max_X] = x;
                     //         if (x < return_bbox[min_X]) return_bbox[min_X] = x;
                     //         if (y > return_bbox[max_Y]) return_bbox[max_Y] = y;
                     //         if (y < return_bbox[min_Y]) return_bbox[min_Y] = y;
                     //         if (z > return_bbox[max_Z]) return_bbox[max_Z] = z;
                     //         if (z < return_bbox[min_Z]) return_bbox[min_Z] = z;
                     //      }
                     //   }
                     //   retVal->return_bbox->putRealAggr(return_bbox, 6);
                     //   emsg = NULL;
                     //} else {
                     //   emsg = "VELaSSCoEDMplugin::CalculateBoundingBox error: Mesh not found.";
                     //}
                  }
               }
            }
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
      delete e;
   }
   endTime = GetTickCount();
   int execTime = endTime - startTime;
   if (emsg) {
      retVal->return_error_str->putString(emsg);
   } else {
      retVal->return_bbox->putRealAggr(return_bbox, 6);
      retVal->return_error_str->putString("OK");
   }
   return rstat;
}

/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::InjectFiles(Model *theModel, ModelType mt, nodeInInjectFiles *inParam, nodeRvInjectFiles *retVal)
/*
===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;
   char *test;

   try {
      startTime = GetTickCount();
      if (mt == mtDEM) {
         emsg = "Injectfiles is not implemented for DEM models.";
      } else {
         int sz = 0;
         for (int i = 0; i < inParam->nOfAttributes; i++) {
            cppRemoteParameter *rp = inParam->attrPointerArr[i];
            if (rp->type = rptSTRING) {
               // it is a file name
               sz += (strlen(rp->value.stringVal) + 2);
            }
         }
         test = (char*)dllMa->alloc(sz);
         char *bp = test;
         for (int i = 0; i < inParam->nOfAttributes; i++) {
            cppRemoteParameter *rp = inParam->attrPointerArr[i];
            if (rp->type = rptSTRING) {
               strcpy(bp, rp->value.stringVal); bp += (strlen(rp->value.stringVal) + 1); *bp = '\n'; bp++;
            }
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   endTime = GetTickCount();
   int execTime = endTime - startTime;
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString(test);
   }
   return rstat;
}


static CLoggWriter    *ourLogger = NULL;

void logError(char *repositoryName, char *modelName, char *methodName, char *msg, int lineNo)
{
   char msgBuf[10000];
   time_t t = time(NULL);

   sprintf(msgBuf, "\n\n%s\nError in EDMserver_dll detected, repositoryName=%s, modelName=%s, methodName=%s\nError=%s, at line %d\n",
      asctime(localtime(&t)), repositoryName, modelName, methodName, msg, lineNo);
   if (ourLogger) {
      ourLogger->logg(0, msgBuf);
   } else {
      printf(msgBuf);
   }
   printf(msgBuf);
}

void logError(char *repositoryName, char *modelName, char *methodName, int rstat, int lineNo)
{
   logError(repositoryName, modelName, methodName, edmiGetErrorText(rstat), lineNo);
}

void setWrongNumberErrorMsg(cppRemoteParameter *status, cppRemoteParameter *report, cppRemoteParameter *returValue)
{
   status->putString("Error");
   report->putString("Wrong number of input parameters or result values.");
   if (returValue) returValue->type = rptUndefined;
}
extern "C" EDMLONG DLL_EXPORT dll_main(char *repositoryName, char *modelName, char *methodName,
   EDMLONG nOfParameters, cppRemoteParameter *parameters, EDMLONG nOfReturnValues, cppRemoteParameter *returnValues, void **threadObject)
{
   EdmiError rstat = OK;
   int lineNo = __LINE__;
#define tr lineNo = __LINE__;

   try {
      //EdmiError trrstat = edmiTrace(DEFINE_TRACE, TRACE_CALLS | TRACE_ARGS | TRACE_RETURNS | TRACE_ERRORS | TRACE_TO_STDOUT, 0, "");
      //trrstat = edmiTrace(START_TRACE, 0, 0, "");
      int startTime, endTime;
      startTime = GetTickCount();

      if (ourLogger == NULL) {
         char fn[2048], hn[512];
         char *lff = getenv("LOG_FILE_FOLDER");

         if (gethostname(hn, sizeof(hn)) || lff == NULL) {
            sprintf(fn, "plugin_1_%d_%d.log", rand(), getpid());
         } else {
            sprintf(fn, "%s/plugin_1_%s_%d.log", lff, hn, getpid());
         }
         logFileName = (char*)malloc(strlen(fn) + 1); strcpy(logFileName, fn);
         FILE *logFilep = fopen(fn , "w");
         ourLogger = new CLoggWriter(logFilep, true, true);
      }

START_TRACE (fp, "plugin - 1, %s.%s - %s\n", repositoryName, modelName, methodName); END_TRACE
      if (QUERY_RESULT_FOLDER == NULL) {
         char *env = getenv("QUERY_RESULT_FOLDER");
         if (env) {
            QUERY_RESULT_FOLDER = (char*)malloc(strlen(env) + 2); strcpy(QUERY_RESULT_FOLDER, env);
            char *ec = QUERY_RESULT_FOLDER + strlen(env) - 1;
            if (*ec == '\\') {
               *ec = '/';
            } else if (*ec != '/') {
               strcat(QUERY_RESULT_FOLDER, "/");
            }
         } else {
            QUERY_RESULT_FOLDER = "";
         }
      }
START_TRACE(fp, "plugin - 2\n");END_TRACE
      VELaSSCoEDMplugin *plugin = new VELaSSCoEDMplugin(QUERY_RESULT_FOLDER, repositoryName, modelName); tr;
      CMemoryAllocator *theMA = plugin->getMemoryAllocator(); tr;
      *threadObject = (void*)plugin;
      
      Database VELaSSCo_db("", "", ""); tr;
      Repository VELaSSCo_Repository(&VELaSSCo_db, repositoryName); tr;
      
      if (strEQL(methodName, "InjectFEMfiles") || strEQL(methodName, "InjectDEMfiles") || strEQL(methodName, "AnalyzeFEMfiles")) {
         tr;
         bool FEM = strEQL(methodName, "InjectFEMfiles") || strEQL(methodName, "AnalyzeFEMfiles");
         bool analyze = strEQL(methodName, "AnalyzeFEMfiles");
         const char *schemaName = FEM ? "fem_schema_velassco" : "dem_schema_velassco";tr;
         
         nodeRvInjectFiles *results = new(theMA)nodeRvInjectFiles(NULL, returnValues);tr;
         nodeInInjectFiles *inParams = new(theMA)nodeInInjectFiles(NULL, parameters);tr;
         
         FEM_InjectorHandler femInjector(&fem_schema_velassco_SchemaObject);tr;
         if (analyze) {
            femInjector.initAnalyze(theMA);tr;
         } else {
            VELaSSCo_Repository.open(sdaiRW);tr;
            femInjector.setCurrentRepository(&VELaSSCo_Repository);tr;
            femInjector.setCurrentSchemaName(schemaName);tr;
            femInjector.setDatabase(&VELaSSCo_db);tr;
            femInjector.setCurrentModel(modelName);tr;
            femInjector.DeleteCurrentModelContent();tr;
         }
START_TRACE(fp, "plugin - 5\n");END_TRACE
         for (int i = 0; i < MAX_INJECT_FILES; i++) {
            if (inParams->attrPointerArr[i]->type == rptSTRING) {
               char *file_name = inParams->attrPointerArr[i]->value.stringVal;tr;
               femInjector.injectorFileName = file_name;tr;
               femInjector.fp = fopen(file_name, "r"); femInjector.cLineno = 0;tr;
START_TRACE(fp, "plugin - inject file %s\n", file_name);END_TRACE
               if (femInjector.fp) {
                  int extPos = strlen(file_name) - 9;
                  if (extPos > 0 && strnEQL(file_name + extPos, ".post.msh", 9)) {
                     if (analyze) {
                        femInjector.AnalyzeMeshFile();tr;
                     } else {
                        femInjector.InjectMeshFile();tr;
                     }
                  } else if (extPos > 0 && strnEQL(file_name + extPos, ".post.res", 9)) {
                     if (analyze) {
                        femInjector.AnalyzeResultFile();tr;
                     } else {
                        femInjector.InjectResultFile();tr;
                     }
                  }
                  fclose(femInjector.fp);tr;
               } else {
START_TRACE(fp, "plugin - Illegal FEM file name %s\n", file_name);END_TRACE
                   femInjector.printError("Illegal FEM file name");tr;
               }
            }
         }
START_TRACE(fp, "plugin - 6\n");END_TRACE
         endTime = GetTickCount();
         char msg[2048];
         if (analyze) {
            results->status->putString("OK");tr;
            results->maxNodeId->putInteger(femInjector.maxNodeId);tr;
            results->maxElementId->putInteger(femInjector.maxElementId);tr;
            results->resultNames->putStringContainer(femInjector.resultNames);tr;
            results->timesteps->putContainer(femInjector.temesteps);tr;
            for (double *dp = femInjector.temesteps->firstp(); dp;  dp = femInjector.temesteps->nextp()) {
               dp = 0;
            }
         } else {
            CHECK(edmiCommitTransaction());tr;
            femInjector.flushObjectsAndClose();tr;
            sprintf(msg, "Injection to %s.%s finished. Time used=%d milliseconds.", repositoryName, modelName, endTime - startTime);
            START_TRACE(fp, "%s\n", msg);END_TRACE
            results->status->putString("OK");tr;
            results->report->putString(msg);tr;
         }
START_TRACE(fp, "plugin - 7\n");END_TRACE
      } else {
         Model VELaSSCo_model(&VELaSSCo_Repository, theMA, NULL); tr;
         VELaSSCo_model.open(modelName, sdaiRO); tr;
         ModelType vmt = plugin->getModelType(VELaSSCo_model.modelId); tr;

         VELaSSCo_model.defineSchema(vmt == mtFEM ? (dbSchema*)&fem_schema_velassco_SchemaObject : (dbSchema*)&dem_schema_velassco_SchemaObject); tr;

         if (strEQL(methodName, "GetListOfAnalyses")) {
            nodervGetListOfAnalyses *results = new(theMA)nodervGetListOfAnalyses(NULL, returnValues); tr;
            if (nOfParameters != 0 || nOfReturnValues != 3) {
               setWrongNumberErrorMsg(results->status, results->report, results->analysis_name_list);
            } else {
               rstat = plugin->GetListOfAnalyses(&VELaSSCo_model, vmt, results); tr;
            }
         } else if (strEQL(methodName, "GetListOfTimeSteps")) {
            nodeRvGetListOfTimeSteps *results = new(theMA)nodeRvGetListOfTimeSteps(NULL, returnValues); tr;
            nodeInGetListOfTimeSteps *inParams = new(theMA)nodeInGetListOfTimeSteps(NULL, parameters);
            if (nOfParameters != 1 || nOfReturnValues != 3) {
               setWrongNumberErrorMsg(results->status, results->report, results->ListOfTimeSteps);
            } else {
               rstat = plugin->GetListOfTimeSteps(&VELaSSCo_model, vmt, inParams, results); tr;
            }
         } else if (strEQL(methodName, "GetListOfVerticesFromMesh")) {
            nodeRvGetListOfVerticesFromMesh *results = new(theMA)nodeRvGetListOfVerticesFromMesh(NULL, returnValues); tr;
            nodeInGetListOfVerticesFromMesh *inParams = new(theMA)nodeInGetListOfVerticesFromMesh(NULL, parameters);
            if (nOfParameters != 3 || nOfReturnValues != 5) {
               setWrongNumberErrorMsg(results->status, results->report, results->vertices);
            } else {
               rstat = plugin->GetListOfVerticesFromMesh(&VELaSSCo_model, vmt, inParams, results); tr;
            }
         } else if (strEQL(methodName, "GetListOfResultsFromTimeStepAndAnalysis")) {
            nodeRvGetListOfResultsFromTimeStepAndAnalysis *results = new(theMA)nodeRvGetListOfResultsFromTimeStepAndAnalysis(NULL, returnValues); tr;
            nodeInGetListOfResultsFromTimeStepAndAnalysis *inParams = new(theMA)nodeInGetListOfResultsFromTimeStepAndAnalysis(NULL, parameters);
            if (nOfParameters != 2 || nOfReturnValues != 3) {
               setWrongNumberErrorMsg(results->status, results->report, results->ResultInfoList);
            } else {
               rstat = plugin->GetListOfResultsFromTimeStepAndAnalysis(&VELaSSCo_model, vmt, inParams, results);
            }
         } else if (strEQL(methodName, "GetResultFromVerticesID")) { tr;
            nodeRvGetResultFromVerticesID *results = new(theMA)nodeRvGetResultFromVerticesID(NULL, returnValues);
            nodeInGetResultFromVerticesID *inParams = new(theMA)nodeInGetResultFromVerticesID(NULL, parameters);
            if (nOfParameters != 4 || nOfReturnValues != 6) {
               setWrongNumberErrorMsg(results->status, results->report, results->result_list);
            } else {
               rstat = plugin->GetResultFromVerticesID(&VELaSSCo_model, vmt, inParams, results); tr;
            }
         } else if (strEQL(methodName, "GetCoordinatesAndElementsFromMesh")) { tr;
            nodeRvGetCoordinatesAndElementsFromMesh *results = new(theMA)nodeRvGetCoordinatesAndElementsFromMesh(NULL, returnValues);
            nodeInGetCoordinatesAndElementsFromMesh *inParams = new(theMA)nodeInGetCoordinatesAndElementsFromMesh(NULL, parameters);
            if (nOfParameters != 3 || nOfReturnValues != 5) {
               results->elemnt_array->type = results->returned_mesh_info->type = rptUndefined;
               setWrongNumberErrorMsg(results->status, results->report, results->node_array);
            } else {
               rstat = plugin->GetCoordinatesAndElementsFromMesh(&VELaSSCo_model, vmt, inParams, results); tr;
            }
         } else if (strEQL(methodName, "GetBoundaryOfLocalMesh")) {
            nodeRvGetBoundaryOfLocalMesh *results = new(theMA)nodeRvGetBoundaryOfLocalMesh(NULL, returnValues);
            nodeInGetBoundaryOfLocalMesh *inParams = new(theMA)nodeInGetBoundaryOfLocalMesh(NULL, parameters);
            if (nOfParameters != 3 || nOfReturnValues != 5) {
               setWrongNumberErrorMsg(results->status, results->report, results->triangle_array);
            } else {
               rstat = plugin->GetBoundaryOfLocalMesh(&VELaSSCo_model, vmt, inParams, results); tr;
            }
         } else if (strEQL(methodName, "GetListOfMeshes")) { tr;
            nodeRvGetListOfMeshes *results = new(theMA)nodeRvGetListOfMeshes(NULL, returnValues);
            nodeInGetListOfMeshes *inParams = new(theMA)nodeInGetListOfMeshes(NULL, parameters);
            if (nOfParameters != 2 || nOfReturnValues != 3) {
               setWrongNumberErrorMsg(results->status, results->report, results->mesh_info_list);
            } else {
               rstat = plugin->GetListOfMeshes(&VELaSSCo_model, vmt, inParams, results); tr;
            }
         } else if (strEQL(methodName, "CalculateBoundingBox")) { tr;
            nodeInCalculateBoundingBox *inParams = new(theMA)nodeInCalculateBoundingBox(NULL, parameters);
            nodeRvCalculateBoundingBox *results = new(theMA)nodeRvCalculateBoundingBox(NULL, returnValues);
            if (nOfParameters != 3 || nOfReturnValues != 2) {
               results->return_error_str->putString("Wrong number of input parameters or result values.");
            } else {
               rstat = plugin->CalculateBoundingBox(&VELaSSCo_model, vmt, inParams, results); tr;
            }
         } else if (strEQL(methodName, "GetBoundaryOfSubMesh")) {
            nodeRvGetBoundaryOfLocalMesh *results = new(theMA)nodeRvGetBoundaryOfLocalMesh(NULL, returnValues);
            nodeInGetBoundaryOfLocalMesh *inParams = new(theMA)nodeInGetBoundaryOfLocalMesh(NULL, parameters);
            if (nOfParameters != 3 || nOfReturnValues != 5) {
               setWrongNumberErrorMsg(results->status, results->report, results->triangle_array);
            } else {
               rstat = plugin->GetBoundaryOfSubMesh(&VELaSSCo_model, vmt, inParams, results); tr;
            }
         } else if (strEQL(methodName, "GetNodeCoordinates")) {
            nodeRvGetNodeCoordinates *results = new(theMA)nodeRvGetNodeCoordinates(NULL, returnValues);
            nodeInGetNodeCoordinates *inParams = new(theMA)nodeInGetNodeCoordinates(NULL, parameters);
            if (nOfParameters != 2 || nOfReturnValues != 3) {
               results->return_error_str->putString("Wrong number of input parameters or result values.");
            } else {
               rstat = plugin->GetNodeCoordinates(&VELaSSCo_model, vmt, inParams, results); tr;
            }
         }
      }
START_TRACE(fp, "plugin - 10\n");END_TRACE
      endTime = GetTickCount();
      //ourLogger->logg(4, "Query %s on %s.%s finished.\nExection time: %d millisec.\n\n", methodName, repositoryName, modelName,  endTime - startTime);

   } catch (CedmError *e) {
      rstat = e->rstat; delete e;
START_TRACE(fp, "plugin - 11  %s  %s  %s  %d  %d\n", repositoryName, modelName, methodName, rstat, lineNo);END_TRACE
//      edmiAbortTransaction();
      //logError(repositoryName, modelName, methodName, rstat, lineNo);
   } catch (...) {
START_TRACE(fp, "plugin - 12\n");END_TRACE
  //    edmiAbortTransaction();
      //logError(repositoryName, modelName, methodName, "GPF exception", lineNo);
      rstat = sdaiESYSTEM;
   }
   return rstat;
}

extern "C" EDMLONG DLL_EXPORT dll_version()
{
   return 1;
}

extern "C" void  DLL_EXPORT *dll_malloc(void *threadObject, EDMLONG bufSize)
{
   VELaSSCoEDMplugin *plugin = (VELaSSCoEDMplugin*)threadObject;
   if (plugin) {
      return plugin->alloc(bufSize);
   } else {
      return NULL;
   }
}

extern "C" EDMLONG  DLL_EXPORT dll_freeAll(void *threadObject)
{
   VELaSSCoEDMplugin *plugin = (VELaSSCoEDMplugin*)threadObject;
   delete plugin;
   return OK;
}

VELaSSCoEDMplugin::~VELaSSCoEDMplugin()
{
   delete dllMa;
   if (resultInfoMemory) {
      delete resultInfoMemory;

   }
   //if (nodeIdRegion) {
   //   delete nodeIdRegion;
   //}
}


CMemoryAllocator ma(0x100000);

/*================================================================================================*/
void* ourMemoryAllocator(SdaiVoid _ma, EDMLONG size)
/*================================================================================================*/
{
   CMemoryAllocator* ma = (CMemoryAllocator*)_ma;
   return ma->alloc(size);
}
/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetBoundaryOfSubMesh(Model *theModel, ModelType mt, nodeInGetBoundaryOfLocalMesh *inParam,
   nodeRvGetBoundaryOfLocalMesh *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
#define nOfParameters 4

   try {
      SdaiServerContext          serverContextId;
      cppRemoteParameter         stringPar, intPar, realPar, intArrayPar;
      cppRemoteParameter         *parameters[nOfParameters];
      cppRemoteParameter         retInt, retIntArr, retLongIntArr, retIntContainer;
      cppRemoteParameter         *retVals[4];
      EDMLONG                    intArr[100];

      EdmiError rstat = edmiDefineServerContext("sc", "superuser", "", "v", "TCP", "9090", "localhost", NULL, NULL, NULL, NULL, NULL, &serverContextId);

      for (int i = 0; i < 100; i++) intArr[i] = i + 1000;
      stringPar.type = rptSTRING; stringPar.value.stringVal = "Ja vi elsker dette landet";
      //intPar.type = rptINTEGER; intPar.value.intVal = 10660000;
      intPar.type = rptINTEGER; intPar.value.intVal = 1066;
      realPar.type = rptREAL; realPar.value.realVal = 3.14;
      intArrayPar.type = rptAggrINTEGER; intArrayPar.value.intAggrVal = intArr; intArrayPar.bufferSize = 100 * sizeof(EDMLONG);

      parameters[0] = &stringPar;  parameters[1] = &intPar; parameters[2] = &realPar; parameters[3] = &intArrayPar;
      retVals[0] = &retInt; retVals[1] = &retIntArr; retVals[2] = &retLongIntArr; retVals[3] = &retIntContainer;
      rstat = edmiRemoteExecuteCppMethod(serverContextId, "FEM_models", "VELaSSCo_HbaseBasicTest_part_1", "cpp_plugins", "VELaSSCo", "TestInputParameters", 0,
         nOfParameters, (RemoteParameter*)parameters, NULL, 4, (RemoteParameter*)retVals, &ourMemoryAllocator, &ma, NULL);

      if (rstat) {
         printf("Error encounterd, rstat = %llu - %s\n", rstat, edmiGetErrorText(rstat));
      } else {
         EDMLONG nElem = retIntArr.bufferSize / sizeof(EDMLONG);
         for (int i = 0; i < nElem; i++) {
            if (retIntArr.value.intAggrVal[i] != i * i) {
               printf("error in returned array\n"); break;
            }
         }
         if (retInt.value.intVal != intPar.value.intVal) {
            printf("error in returned int\n");
         }
         nElem = retLongIntArr.bufferSize / sizeof(EDMLONG);
         for (int i = 0; i < nElem; i++) {
            if (retLongIntArr.value.intAggrVal[i] != i * 123) {
               printf("error in returned long array\n"); break;
            }
         }
         Container<EDMVD::Vertex> *vertexes = new(&ma)Container<EDMVD::Vertex>(&ma, &retIntContainer);
         EDMLONG i = 0;
         double x = 1.0;
         for (EDMVD::Vertex *v = vertexes->firstp(); v; v = vertexes->nextp()) {
            if (v->id != i || v->x != x || v->y != (x + 1.0) || v->z != (x + 2.0)) {
               printf("Error in returned Container\n"); break;
            }
            x += 1.5; i++;
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}

/*===================================================================================================================*/
EDMLONG * VELaSSCoEDMplugin::createNodeId_to_object_id_file(fem::Mesh *mesh, char *nodeIdFileName)
/*===================================================================================================================*/
{
   EDMULONG maxID = 0, minID = 0xfffffffffffffff;

   Iterator<fem::Node *, fem::entityType> nodeIter(mesh->get_nodes());
   for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
      EDMLONG id = n->get_id();
      if (id > maxID) maxID = id; if (id < minID) minID = id;
   }
   const std::size_t fileSize = (maxID - minID + 3) * sizeof(EDMLONG);

   EDMLONG *nodeIdArray = (EDMLONG *)dllMa->alloc(fileSize);
   memset(nodeIdArray, 0, fileSize);
   nodeIdArray[0] = minID;
   nodeIdArray[1] = maxID;
   for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
      EDMLONG id = n->get_id();
      nodeIdArray[id - minID + 2] = n->getInstanceId();
            if (id == 2942089) {
               int asdfasdf = 999;
            } else if (id == 2921129) {
               int asdfasdf = 999;
            } else if (id == 0) {
               int asdfasdf = 999;
            }
   }
   FILE *bfp = fopen(nodeIdFileName, "wb");
   if (bfp) {
      size_t nWritten = fwrite(nodeIdArray, 1, fileSize, bfp);
      if (nWritten != fileSize) THROW("Error when writing to binary file in VELaSSCoEDMplugin::GetResultFromVerticesID.");
      fclose(bfp);
   } else {
      THROW("Cannot open binary file in VELaSSCoEDMplugin::createNodeId_to_object_id_file.");
   }
   return nodeIdArray;
}
/*===================================================================================================================*/
void VELaSSCoEDMplugin::getNodeCoordinates(Model *theModel, EDMULONG nOfNodes, EDMULONG *nodeIds, Container<EDMVD::Vertex> *vertices)
/*===================================================================================================================*/
{
   EDMULONG maxID = nodeIdArray[1], minID = nodeIdArray[0];
   for (int i = 0; i < nOfNodes; i++) {
            if (nodeIds[i] == 2942089) {
               int asdfasdf = 999;
            } else if (nodeIds[i] == 2921129) {
               int asdfasdf = 999;
            } else if (nodeIds[i] == 0) {
               int asdfasdf = 999;
            }

      if (nodeIds[i] >= minID && nodeIds[i] <= maxID) {
         SdaiInstance nodeId = nodeIdArray[nodeIds[i] - minID + 2];
         if (nodeId) {
            tEdmiInstData nd;
            nd.instanceId = nodeId; nd.entityData = &theModel->theEntities[fem::et_Node];
            fem::Node thisNode(theModel, &nd);
            EDMVD::Vertex *v = vertices->createNext();
            v->id = nodeIds[i];
            v->x = thisNode.get_x(); v->y = thisNode.get_y(); v->z = thisNode.get_z(); 
            if (v->id != thisNode.get_id()) {
               THROW("Illegal node Id found by VELaSSCoEDMplugin::GetNodeCoordinates.");
            }
         }
      }
   }
}
/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetNodeCoordinates(Model *theModel, ModelType mt, nodeInGetNodeCoordinates *inParam,
   nodeRvGetNodeCoordinates *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
#define nOfParameters 4

   try {
      FILE *nodeIdsfp = fopen(inParam->nodeIdsFileName->value.stringVal, "rb");
      if (nodeIdsfp) {
         EDMULONG nOfNodes = inParam->nOfNodeIds->value.intVal;
         EDMULONG *nodeIds = (EDMULONG*)dllMa->alloc(nOfNodes * sizeof(EDMULONG)); 
         size_t nRead = fread(nodeIds, sizeof(EDMULONG), nOfNodes, nodeIdsfp);
         fclose(nodeIdsfp);

         Container<EDMVD::Vertex> *vertices = new(dllMa)Container<EDMVD::Vertex>(dllMa);

         Iterator<fem::Mesh*, fem::entityType> meshIter(theModel->getObjectSet(fem::et_Mesh), theModel);
         for (fem::Mesh *mesh = meshIter.first(); mesh; mesh = meshIter.next()) {
            char *nodeIdFileName = getResultFileName("nodeId_%llu.dat", mesh->getInstanceId());
            int nOfValuesPrVertex;

            initNodeIdMapping(nodeIdFileName); nodeIdArray = NULL;
            
            try {
               //Open the file mapping and map it as read-only
               file_mapping node_file(nodeIdFileName, read_only);
               mapped_region node_region(node_file, read_only);
               nodeIdArray = (EDMLONG*)node_region.get_address();
               EDMULONG bufferSize = node_region.get_size();

               getNodeCoordinates(theModel, nOfNodes, nodeIds, vertices);

               //EDMULONG maxID = nodeIdArray[1], minID = nodeIdArray[0];
               //for (int i = 0; i < nOfNodes; i++) {
               //   if (nodeIds[i] >= minID && nodeIds[i] <= maxID) {
               //      SdaiInstance nodeId = nodeIdArray[nodeIds[i] - minID + 2];
               //      if (nodeId) {
               //         tEdmiInstData nd;
               //         nd.instanceId = nodeId; nd.entityData = &theModel->theEntities[fem::et_Node];
               //         fem::Node thisNode(theModel, &nd);
               //         EDMVD::Vertex *v = vertices->createNext();
               //         v->id = nodeIds[i];
               //         v->x = thisNode.get_x(); v->y = thisNode.get_y(); v->z = thisNode.get_z(); 
               //         if (v->id != thisNode.get_id()) {
               //            retVal->return_error_str->putString("Illegal node Id found by VELaSSCoEDMplugin::GetNodeCoordinates.");
               //            return rstat;
               //         }
               //      }
               //   }
               //}
            } catch (boost::interprocess::interprocess_exception exep) {
               try {
                  nodeIdArray = createNodeId_to_object_id_file(mesh, getResultFileName("nodeId_%llu.dat", mesh->getInstanceId()));
                  getNodeCoordinates(theModel, nOfNodes, nodeIds, vertices);
               } catch (CedmError *e) {
                  char *emsg = handleError(e);
                  retVal->return_error_str->putString(emsg);
                  return rstat;
               }
            }
            retVal->vertices->putContainer(vertices); retVal->nOfNodesFound->putInteger(vertices->size());
         }
      }
   } catch (CedmError *e) {
      emsg = handleError(e);
   }
   if (emsg) {
      retVal->return_error_str->putString(emsg);
   }
   return rstat;
}
