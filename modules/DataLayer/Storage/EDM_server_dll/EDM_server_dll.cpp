// EDM_server_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "EDM_server_dll.h"


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
   printf("\n\n%s\n\n", buf);
   return dllMa->allocString(buf);
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

/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetListOfAnalyses(Model *theModel, ModelType mt, nodervGetListOfAnalyses *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;

   try {
      char* np;

      Container<char*> *nameList = new(dllMa)Container<char*>(dllMa);
      if (mt == mtDEM) {
         Iterator<dem::Simulation*, dem::entityType> simIter(theModel->getObjectSet(dem::et_Simulation), theModel);
         for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
            nameList->add(dllMa->allocString(s->get_name()));
         }
         retVal->analysis_name_list->putStringContainer(nameList);
      } else if (mt == mtFEM) {
         Iterator<fem::ResultHeader*, fem::entityType> rhIter(theModel->getObjectSet(fem::et_ResultHeader), theModel);
         for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
            char* anName = rh->get_analysis();
            for (np = nameList->first(); np && strNEQ(anName, np); np = nameList->next());
            if (np == NULL) {
               nameList->add(dllMa->allocString(anName));
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

   try {
      char *anid = inParam->analysisID->value.stringVal;
      Container<double> timeSteps(dllMa);

      if (mt == mtDEM) {
         Iterator<dem::Simulation*, dem::entityType> simIter(theModel->getObjectSet(dem::et_Simulation), theModel);
         for (dem::Simulation *s = simIter.first(); s; s = simIter.next()) {
            if (strEQL(s->get_name(), anid)) {
               Iterator<dem::Timestep*, dem::entityType> tsIter(s->get_consists_of(), theModel);
               for (dem::Timestep *ts = tsIter.first(); ts; ts = tsIter.next()) {
                  timeSteps.add(ts->get_time_value());
               }
            }
         }
         retVal->ListOfTimeSteps->putContainer(&timeSteps);
      } else {
         Iterator<fem::ResultHeader*, fem::entityType> rhIter(theModel->getObjectSet(fem::et_ResultHeader), theModel);
         for (fem::ResultHeader *rh = rhIter.first(); rh; rh = rhIter.next()) {
            if (strEQL(rh->get_analysis(), anid)) {
               timeSteps.add(rh->get_step());
            }
         }
         retVal->ListOfTimeSteps->putContainer(&timeSteps);
      }
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
      printf("\n\nElapsed time for parallel execution is %d milliseconds\n\n", endTime - startTime);
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
   int startTime, endTime;

   try {
      startTime = GetTickCount();


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

            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps(), theModel);
            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->stepValue->value.realVal) {
                  Iterator<fem::ResultHeader*, fem::entityType> resultIter(ts->get_results(), theModel);
                  for (fem::ResultHeader *rh = resultIter.first(); rh; rh = resultIter.next()) {
                     EDMVD::ResultInfo *ri = resInfo->createNext();
                     ri->name = resultInfoMemory->allocString(rh->get_name());
                     ri->type = resultInfoMemory->allocString(rh->exists_rType() ? ValueTypes[rh->get_rType()] : "");
                     ri->location = resultInfoMemory->allocString(rh->exists_location() ? LocationTypes[rh->get_location()] : "");
                     ri->gaussPointName = resultInfoMemory->allocString(rh->exists_gpName() ? rh->get_gpName() : "");
                     ri->coordinatesName = resultInfoMemory->allocString("");
                     ri->units = resultInfoMemory->allocString("");
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
   endTime = GetTickCount();
   return rstat;
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
            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps(), theModel);
            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->timeStep->value.realVal) {
                  timeStepFound = true;
                  Iterator<fem::ResultHeader*, fem::entityType> resultHeaderIter(ts->get_results(), theModel);
                  for (fem::ResultHeader *rh = resultHeaderIter.first(); rh; rh = resultHeaderIter.next()) {
                     char *rhname = rh->get_name();
                     if (rhname && strEQL(rhname, resultID)) {
                        if (!resultIdFound) {
                           Iterator<fem::Result*, fem::entityType> resultIter(rh->get_values(), theModel);
                           fem::entityType resType;
                           resultIdFound = true;
                           int nOfValuesPrVertex;
                           
                           void *vres = resultIter.first(&resType);
                           if (resType == fem::et_ScalarResult) {
                              nOfValuesPrVertex = 1;
                           } else {
                              fem::VectorResult *vector = (VectorResult *)vres;
                              SdaiAggr cnAID = vector->get_values_aggrId();
                              nOfValuesPrVertex = (int)sdaiGetMemberCount(cnAID);
                           }
                           retVal->nOfValuesPrVertex->putInteger(nOfValuesPrVertex);
                           
                           relocateResultOnVertex *relocateInfo = (relocateResultOnVertex *)resultInfoMemory->createRelocateInfo(sizeof(relocateResultOnVertex));
                           Container<EDMVD::ResultOnVertex> *resultsOnVertices = new(resultInfoMemory)Container<EDMVD::ResultOnVertex>(resultInfoMemory, 1024, (sizeof(double) * (nOfValuesPrVertex - 1)));
                           relocateInfo->vertexResults = resultsOnVertices;
                           
                           EDMULONG maxInputID = 0, minInputID = 0xfffffffffffffff;
                           char *vertixSpecified = NULL;
                           EDMULONG nOfInputVertices = inParam->listOfVertices->getAggrSize();
                           if (nOfInputVertices > 0) {
                              EDMLONG *vertices = inParam->listOfVertices->value.intAggrVal;
                              for (int i = 0; i < nOfInputVertices; i++) {
                                 if (vertices[i] > maxInputID) maxInputID = vertices[i]; if (vertices[i] < minInputID) minInputID = vertices[i];
                              }
                              vertixSpecified = (char*)dllMa->allocZeroFilled((maxInputID - minInputID + 1) * sizeof(char));
                              for (int i = 0; i < nOfInputVertices; i++) vertixSpecified[vertices[i] - minInputID] = 1;
                           }
                           for (void *vres = resultIter.first(&resType); vres; vres = resultIter.next(&resType)) {
                              fem::Result *res;
                              fem::ScalarResult *scalar;
                              fem::VectorResult *vector;
                              if (resType == fem::et_ScalarResult) {
                                 scalar = (ScalarResult *)vres; res = scalar;
                              } else if (resType == fem::et_VectorResult) {
                                 vector = (VectorResult *)vres; res = vector;
                              }
                              fem::Node *node = res->get_result_for();
                              EDMLONG nodeID = node ? node->get_id() : 0;

                              if (vertixSpecified == NULL || (nodeID >= minInputID && nodeID <= maxInputID && vertixSpecified[nodeID - minInputID])) {
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
                                    emsg = "Resulttype not supported in GetResultFromVerticesID.";
                                 }
                                 rov->id = nodeID;
                                 if (rov->id > maxID) maxID = rov->id;
                                 if (rov->id < minID) minID = rov->id;
                              }
                           }
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

static VELaSSCoSM::ElementShapeType elementTypeConvert[] = {PointElement, LineElement,  TriangleElement, QuadrilateralElement, TetrahedraElement, HexahedraElement, PrismElement, PyramidElement, SphereElement, CircleElement, ComplexParticleElement};
/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetCoordinatesAndElementsFromMesh(Model *theModel, ModelType mt, nodeInGetCoordinatesAndElementsFromMesh *inParam,
   nodeRvGetCoordinatesAndElementsFromMesh *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;

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
            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps(), theModel);
            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->timeStep->value.realVal) {
                  timeStepFound = true;
                  fem::Mesh *mesh = ts->get_mesh();
                  
                  Container<EDMVD::Vertex> *vertices = new(dllMa) Container<EDMVD::Vertex>(dllMa, 0x10000);
                  Iterator<fem::Node*, fem::entityType> nodeIter(mesh->get_nodes(), theModel);
                  for (fem::Node *n = nodeIter.first(); n; n = nodeIter.next()) {
                     EDMVD::Vertex *v = vertices->createNext();
                     v->id = n->get_id();
                     v->x = n->get_x(); v->y = n->get_y(); v->z = n->get_z();
                     if (v->id > maxID) maxID = v->id; if (v->id < minID) minID = v->id;
                  }
                  metaData->minNodeID = minID; metaData->maxNodeID = maxID;
                  metaData->n_vertices = nodeIter.size();
                  metaData->vertex_record_size = sizeof(EDMVD::Vertex);
                  retVal->node_array->putContainer(vertices);

                  EDMLONG n_vertices_pr_element = mesh->get_numberOfNodes();
                  Container<EDMVD::FEMelement> *elements = new(dllMa) Container<EDMVD::FEMelement>(dllMa, 0x10000, sizeof(EDMULONG) * (n_vertices_pr_element - 1));
                  Iterator<fem::Element*, fem::entityType> elemIter(mesh->get_elements(), theModel);
                  for (fem::Element *e = elemIter.first(); e; e = elemIter.next()) {
                     EDMVD::FEMelement *element = elements->createNext();
                     element->id = e->get_id();
                     if (element->id > maxID) maxID = element->id; if (element->id < minID) minID = element->id;
                     Iterator<fem::Node*, fem::entityType> nodeIter(e->get_nodes(), theModel);
                     int i = 0;
                     for (fem::Node *n = nodeIter.first(); n && i < n_vertices_pr_element; n = nodeIter.next()) {
                        element->nodes_ids[i++] = n->get_id();
                     }
                  }
                  metaData->n_elements = elemIter.size();
                  metaData->element_type = mesh->exists_elementType() ? elementTypeConvert[mesh->get_elementType()] : UnknownElement;
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
   endTime = GetTickCount();
   int execTime = endTime - startTime;
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
      Iterator<fem::Element*, fem::entityType> elemIter(np->get_elements(), theModel);
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

   Iterator<fem::Element*, fem::entityType> elemIter(elementAggr, theModel);

   for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
      Iterator<fem::Node*, fem::entityType> nodeIter(ep->get_nodes(), theModel);
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
/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetBoundaryOfLocalMesh(Model *theModel, ModelType mt, nodeInGetBoundaryOfLocalMesh *inParam,
   nodeRvGetBoundaryOfLocalMesh *retVal)
/*===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;

   try {
      bool timeStepFound = false, resultIdFound = false;
      char *analysisID = inParam->analysisID->value.stringVal;
      char *meshID = inParam->meshID->value.stringVal;
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
            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps(), theModel);
            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->timeStep->value.realVal) {
                  timeStepFound = true;
                  fem::Mesh *mesh = ts->get_mesh();
                  EDMULONG nTrianglesPrContainerBuffer = 127; //0x10000;
                  Container<EDMVD::Triangle>  *triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, nTrianglesPrContainerBuffer);
                  
                  CalculateBoundaryOfMesh(mesh->get_elements(), triangles, theModel);

                  if (triangles->size() > nTrianglesPrContainerBuffer) {
                     Container<EDMVD::Triangle>  *sortable_triangles = new(dllMa)Container<EDMVD::Triangle>(dllMa, triangles);
                     triangles = sortable_triangles;
                  }
                  triangles->sort(compareTriangle);
                  EDMVD::Triangle *prev = NULL;
                  for (EDMVD::Triangle *t = triangles->firstp(); t; t = triangles->nextp()) {
                     if (t->node_ids[0] == 0 || t->node_ids[1] == 0 || t->node_ids[2] == 0) {
                        int nasdfasdf = 9999;
                     }
                     if (prev) {
                        if (prev->node_ids[0] == t->node_ids[0] && prev->node_ids[1] == t->node_ids[1] && prev->node_ids[2] == t->node_ids[2]) {
                           int nasdfasdf = 9999;
                        }
                     }
                     prev = t;
                  }

                  retVal->triangle_record_size->putInteger(sizeof(EDMVD::Triangle));
                  retVal->n_triangles->putInteger(triangles->size());
                  retVal->triangle_array->putContainer(triangles);
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
   endTime = GetTickCount();
   int execTime = endTime - startTime;
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}
/*===================================================================================================================*/
EDMLONG VELaSSCoEDMplugin::GetListOfMeshes(Model *theModel, ModelType mt, nodeInGetListOfMeshes *inParam,
   nodeRvGetListOfMeshes *retVal)
/*
===================================================================================================================*/
{
   EdmiError rstat = OK;
   char *emsg = NULL;
   int startTime, endTime;

   try {
      bool timeStepFound = false, resultIdFound = false;
      char *analysisID = inParam->analysisID->value.stringVal;
      Container<EDMVD::Vertex> vertices(dllMa, 1024);
      EDMULONG maxID = 0, minID = 0xfffffffffffffff;
      ReturnedMeshInfo *metaData = (ReturnedMeshInfo*)dllMa->alloc(sizeof(metaData));

      if (!resultInfoMemory) resultInfoMemory = new CMemoryAllocator(0x100000);
      startTime = GetTickCount();
      if (mt == mtDEM) {
         emsg = "GetListOfMeshes is not implemented for DEM models.";
      } else {
         Iterator<fem::Analysis*, fem::entityType> analysisIter(theModel->getObjectSet(fem::et_Analysis), theModel);
         fem::Analysis *cAnalysis = getFEManalysis(analysisID, &analysisIter);
         if (cAnalysis) {
            emsg = "Specified time step not found.";
            Iterator<fem::TimeStep*, fem::entityType> tsIter(cAnalysis->get_time_steps(), theModel);
            resultInfoMemory->freeAllMemory();
            relocateMeshInfo *relocateInfo = (relocateMeshInfo *)resultInfoMemory->createRelocateInfo(sizeof(relocateMeshInfo));
            Container<EDMVD::MeshInfo> *meshContainer = new(resultInfoMemory)Container<EDMVD::MeshInfo>(resultInfoMemory, 16);
            relocateInfo->meshes = meshContainer;

            for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
               if (ts->get_time_value() == inParam->timeStep->value.realVal) {
                  timeStepFound = true;
                  fem::Mesh *mesh = ts->get_mesh();

                  EDMVD::MeshInfo *mi = meshContainer->createNext();
                  mi->name = resultInfoMemory->allocString(mesh->exists_name() ? mesh->get_name() : "");
                  mi->elementType.shape = mesh->exists_elementType() ? elementTypeConvert[mesh->get_elementType()] : UnknownElement;
                  List<fem::Element*>* elems = mesh->get_elements();
                  List<fem::Node*>* nodes = mesh->get_nodes();
                  mi->nElements = elems ? elems->size() : 0;
                  mi->nVertices = nodes ? nodes->size() : 0;
                  mi->elementType.num_nodes = (EDMULONG32)mi->nVertices;
                  mi->meshUnits = mi->meshColor = mi->coordsName = NULL;

                  // Add info about the memory blocks within one memory block
                  resultInfoMemory->prepareForRelocationBeforeTransfer();
                  // Link the memory allocator to the return value.
                  retVal->mesh_info_list->putCMemoryAllocator(resultInfoMemory);
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
   endTime = GetTickCount();
   int execTime = endTime - startTime;
   if (emsg) {
      retVal->status->putString("Error"); retVal->report->putString(emsg);
   } else {
      retVal->status->putString("OK");
      retVal->report->putString("");
   }
   return rstat;
}

void logError(char *repositoryName, char *modelName, char *methodName, char *msg, int lineNo)
{
   char msgBuf[10000];
   time_t t = time(NULL);

   sprintf(msgBuf, "\n\n%s\nError in EDMserver_dll detected, repositoryName=%s, modelName=%s, methodName=%s\nError=%s, at line %d\n",
      asctime(localtime(&t)), repositoryName, modelName, methodName, msg, lineNo);
   FILE *logfp = fopen("VELaSSCO_EDMserver_dll_error.log", "a");
   if (logfp) {
      fprintf(logfp, msgBuf); fclose(logfp);
   }
   printf(msgBuf);
}

void logError(char *repositoryName, char *modelName, char *methodName, int rstat, int lineNo)
{
   logError(repositoryName, modelName, methodName, edmiGetErrorText(rstat), lineNo);
}
extern "C" EDMLONG __declspec(dllexport) dll_main(char *repositoryName, char *modelName, char *methodName,
   EDMLONG nOfParameters, cppRemoteParameter *parameters, EDMLONG nOfReturnValues, cppRemoteParameter *returnValues, void **threadObject)
{
   EdmiError rstat = OK;
   int lineNo = __LINE__;
#define tr lineNo = __LINE__;

   try {
      //EdmiError trrstat = edmiTrace(DEFINE_TRACE, TRACE_CALLS | TRACE_ARGS | TRACE_RETURNS | TRACE_ERRORS | TRACE_TO_STDOUT, 0, "");
      //trrstat = edmiTrace(START_TRACE, 0, 0, "");
      
      VELaSSCoEDMplugin *plugin = new VELaSSCoEDMplugin(); tr;
      CMemoryAllocator *theMA = plugin->getMemoryAllocator(); tr;
      *threadObject = (void*)plugin;
      
      Database VELaSSCo_db("", "", ""); tr;
      //SdaiSession sessionId = sdaiOpenSession();
      Repository VELaSSCo_Repository(&VELaSSCo_db, repositoryName); tr;
      Model VELaSSCo_model(&VELaSSCo_Repository, theMA, NULL); tr;
      VELaSSCo_model.open(modelName, sdaiRO); tr;

      ModelType vmt = plugin->getModelType(VELaSSCo_model.modelId); tr;
      VELaSSCo_model.defineSchema(vmt == mtFEM ? (dbSchema*)&fem_schema_velassco_SchemaObject : (dbSchema*)&dem_schema_velassco_SchemaObject); tr;

      if (strEQL(methodName, "GetListOfAnalyses")) {
         nodervGetListOfAnalyses *results = new(theMA)nodervGetListOfAnalyses(NULL, returnValues); tr;
         if (nOfParameters != 0 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->analysis_name_list->type = rptUndefined;
         } else {
            rstat = plugin->GetListOfAnalyses(&VELaSSCo_model, vmt, results); tr;
         }
      } else if (strEQL(methodName, "GetListOfTimeSteps")) {
         nodeRvGetListOfTimeSteps *results = new(theMA)nodeRvGetListOfTimeSteps(NULL, returnValues); tr;
         nodeInGetListOfTimeSteps *inParams = new(theMA)nodeInGetListOfTimeSteps(NULL, parameters);
         if (nOfParameters != 1 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->ListOfTimeSteps->type = rptUndefined;
         } else {
            rstat = plugin->GetListOfTimeSteps(&VELaSSCo_model, vmt, inParams, results); tr;
         }
      } else if (strEQL(methodName, "GetListOfVerticesFromMesh")) {
         nodeRvGetListOfVerticesFromMesh *results = new(theMA)nodeRvGetListOfVerticesFromMesh(NULL, returnValues); tr;
         nodeInGetListOfVerticesFromMesh *inParams = new(theMA)nodeInGetListOfVerticesFromMesh(NULL, parameters);
         if (nOfParameters != 3 || nOfReturnValues != 5) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->vertices->type = rptUndefined;
         } else {
            rstat = plugin->GetListOfVerticesFromMesh(&VELaSSCo_model, vmt, inParams, results); tr;
         }
      } else if (strEQL(methodName, "GetListOfResultsFromTimeStepAndAnalysis")) {
         nodeRvGetListOfResultsFromTimeStepAndAnalysis *results = new(theMA)nodeRvGetListOfResultsFromTimeStepAndAnalysis(NULL, returnValues); tr;
         nodeInGetListOfResultsFromTimeStepAndAnalysis *inParams = new(theMA)nodeInGetListOfResultsFromTimeStepAndAnalysis(NULL, parameters);
         if (nOfParameters != 2 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->ResultInfoList->type = rptUndefined;
         } else {
            rstat = plugin->GetListOfResultsFromTimeStepAndAnalysis(&VELaSSCo_model, vmt, inParams, results);
         }
      } else if (strEQL(methodName, "GetResultFromVerticesID")) { tr;
         nodeRvGetResultFromVerticesID *results = new(theMA)nodeRvGetResultFromVerticesID(NULL, returnValues);
         nodeInGetResultFromVerticesID *inParams = new(theMA)nodeInGetResultFromVerticesID(NULL, parameters);
         if (nOfParameters != 4 || nOfReturnValues != 6) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
            results->result_list->type = rptUndefined;
         } else {
            rstat = plugin->GetResultFromVerticesID(&VELaSSCo_model, vmt, inParams, results); tr;
         }
      } else if (strEQL(methodName, "GetCoordinatesAndElementsFromMesh")) { tr;
         nodeRvGetCoordinatesAndElementsFromMesh *results = new(theMA)nodeRvGetCoordinatesAndElementsFromMesh(NULL, returnValues);
         nodeInGetCoordinatesAndElementsFromMesh *inParams = new(theMA)nodeInGetCoordinatesAndElementsFromMesh(NULL, parameters);
         if (nOfParameters != 3 || nOfReturnValues != 5) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
         } else {
            rstat = plugin->GetCoordinatesAndElementsFromMesh(&VELaSSCo_model, vmt, inParams, results); tr;
         }
      } else if (strEQL(methodName, "GetBoundaryOfLocalMesh")) {
         nodeRvGetBoundaryOfLocalMesh *results = new(theMA)nodeRvGetBoundaryOfLocalMesh(NULL, returnValues);
         nodeInGetBoundaryOfLocalMesh *inParams = new(theMA)nodeInGetBoundaryOfLocalMesh(NULL, parameters);
         if (nOfParameters != 3 || nOfReturnValues != 5) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
         } else {
            rstat = plugin->GetBoundaryOfLocalMesh(&VELaSSCo_model, vmt, inParams, results); tr;
         }
      } else if (strEQL(methodName, "GetListOfMeshes")) { tr;
         nodeRvGetListOfMeshes *results = new(theMA)nodeRvGetListOfMeshes(NULL, returnValues);
         nodeInGetListOfMeshes *inParams = new(theMA)nodeInGetListOfMeshes(NULL, parameters);
         if (nOfParameters != 2 || nOfReturnValues != 3) {
            results->status->putString("Error");
            results->report->putString("Wrong number of input parameters or result values.");
         } else {
            rstat = plugin->GetListOfMeshes(&VELaSSCo_model, vmt, inParams, results); tr;
         }
      }
   } catch (CedmError *e) {
      rstat = e->rstat; delete e;
      logError(repositoryName, modelName, methodName, rstat, lineNo);
   } catch (...) {
      logError(repositoryName, modelName, methodName, "GPF exception", lineNo);
      rstat = sdaiESYSTEM;
   }
   return rstat;
}

extern "C" EDMLONG __declspec(dllexport) dll_version()
{
   return 1;
}

extern "C" void  __declspec(dllexport) *dll_malloc(void *threadObject, EDMLONG bufSize)
{
   VELaSSCoEDMplugin *plugin = (VELaSSCoEDMplugin*)threadObject;
   if (plugin) {
      return plugin->alloc(bufSize);
   } else {
      return NULL;
   }
}

extern "C" EDMLONG  __declspec(dllexport) dll_freeAll(void *threadObject)
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
}
