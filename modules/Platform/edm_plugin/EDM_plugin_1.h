
/*!
   namespace EDM VELaSSCo Data - EDMVD
*/
namespace EDMVD {
   

  struct Triangle {
      EDMULONG                            node_ids[3];
      EDMULONG                            node_ids_orig[3];
      
      inline int                          conpare(const Triangle *x)
      {
         if (node_ids[0] > x->node_ids[0]) {
            return 1;
         } else if (node_ids[0] < x->node_ids[0]) {
            return -1;
         } else {
            if (node_ids[1] > x->node_ids[1]) {
               return 1;
            } else if (node_ids[1] < x->node_ids[1]) {
               return -1;
            } else {
               if (node_ids[2] > x->node_ids[2]) {
                  return 1;
               } else if (node_ids[2] < x->node_ids[2]) {
                  return -1;
               } else {
                  return 0;
               }
            }
         }
      }
   };


   typedef struct Vertex {
      EDMULONG                            id;
      double                              x;
      double                              y;
      double                              z;
   } Vertex;

   typedef struct ResultInfo {
      char                                *name;
      char                                *type;
      int                                 nOfComponents;
      Container<char*>                    *componentNames;
      char                                *location;
      char                                *gaussPointName;
      char                                *coordinatesName;
      char                                *units;
      int                                 *resultNumber;
      void                                relocateThis(CMemoryAllocatorInfo *mai) {
         name = mai->relocatePointer(name);
         type = mai->relocatePointer(type);
         location = mai->relocatePointer(location);
         gaussPointName = mai->relocatePointer(gaussPointName);
         coordinatesName = mai->relocatePointer(coordinatesName);
         units = mai->relocatePointer(units);
         if (componentNames) {
            componentNames = (Container<char*>*)mai->relocatePointer((char*)componentNames);
            componentNames->relocatePointerContainer(mai);
         }
      }
   } ResultInfo;


   typedef struct ElementType {
      VELaSSCoSM::ElementShapeType        shape; // to avoid ambiguity meshinfo.type.type
      EDMULONG                            num_nodes;
   } ElementType;


   typedef struct MeshInfoEDM {
      char                                *name;        // will return mesh info about the mesh with the specified name - error message if specified mesh does not exist.
      //ElementType                         elementType;  // used to calculate number of nodes to be returned for each element
      VELaSSCoSM::ElementShapeType::type  elemType;
      EDMULONG                            num_nodes;
      EDMULONG                            nVertices;    // used to calculate buffer sizes
      EDMULONG                            nElements;    // used to calculate buffer sizes
      char                                *meshUnits;   // not used yet
      char                                *meshColor;   // not used yet
      EDMULONG                            meshNumber;   // not relevant for EDM
      char                                *coordsName;  // not used yet

      void                                relocateThis(CMemoryAllocatorInfo *mai) {
         name = mai->relocatePointer(name);
         meshUnits = mai->relocatePointer(meshUnits);
         meshColor = mai->relocatePointer(meshColor);
         coordsName = mai->relocatePointer(coordsName);
      }
   } MeshInfoEDM;


   //typedef struct Element {
   //   EDMULONG                            id;
   //   Container<EDMULONG>                 *nodes_ids;
   //   void                                relocateThis(CMemoryAllocatorInfo *mai) {
   //      if (nodes_ids) {
   //         nodes_ids = (Container<EDMULONG>*)mai->relocatePointer((char*)nodes_ids);
   //         nodes_ids->relocateStructContainer(mai);
   //      }
   //   }
   //} Element;

   ///*DEM Particel is vertex. - possibly not used for FEM */
   //typedef struct ElementAttrib {
   //   EDMULONG                            id;      // DEM: particle id
   //   char                                *name;   // DEM: 
   //   Container<double>                   *value;  // DEM: f.ex. radius
   //   void                                relocateThis(CMemoryAllocatorInfo *mai) {
   //      name = mai->relocatePointer(name);
   //      if (value) {
   //         value = (Container<double>*)mai->relocatePointer((char*)value);
   //         value->relocateStructContainer(mai);
   //      }
   //   }
   //} ElementAttrib;

   //// DEM: link between particle id and group_id
   //typedef struct ElementGroup {
   //   EDMULONG                            id;
   //   EDMULONG                            group_id;
   //} ElementGroup;

   /*
   //struct rvGetCoordinatesAndElementsFromMesh {
   //   1: string status
   //   2 : string report
   //   3 : list<Vertex>        vertex_list
   //   4 : list<Element>       element_list
   //   5 : list<ElementAttrib> element_attrib_list
   //   6 : list<ElementGroup>  element_group_info_list
   //}

   struct rvGetCoordinatesAndElementsFromMesh {
      1  : string                      status
      2  : string                      report
      3  : i64                         model_type        // FEM = 1, DEM =2
      4  : ElementShapeType            element_type
      5  : i64                         n_vertices
      6  : i64                         vertex_record_size
      7  : i64                         n_elements
      8  : i64                         n_vertices_pr_element
      9  : i64                         element_record_size
      10 : binary                      vertex_array
      11 : binary                      element_array
   }
   */
   typedef struct ReturnedMeshInfo {
      EDMULONG                            model_type;
//      ElementShapeType                    element_type;
      EDMULONG                            n_vertices;
      EDMULONG                            vertex_record_size;
      EDMULONG                            n_elements;
      EDMULONG                            n_vertices_pr_element;
      EDMULONG                            element_record_size;
      EDMULONG                            minNodeID;
      EDMULONG                            maxNodeID;
      EDMULONG                            minElementID;
      EDMULONG                            maxElementID;
   } ReturnedMeshInfo;
  
   typedef struct FEMelement {
      EDMULONG                            id;
      EDMULONG                            nodes_ids[1];
   } FEMelement;
  
   typedef struct DEMelement {
      EDMULONG                            id;
      EDMULONG                            group_id;
      EDMULONG                            nodes_ids[1];
   } DEMelement;

   typedef struct ResultOnVertex {
      EDMULONG                            nodeId;
      EDMULONG                            sequenceNo;  // in input data
      double                              value[1];
   } ResultOnVertex;



   enum ModelType { mtFEM, mtDEM, mtAP209, mtUnknown };

struct nodervGetListOfAnalyses : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *analysis_name_list;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodervGetListOfAnalyses(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&analysis_name_list, rptStringContainer);
   }
};
/*===================================================================================================================*/
struct nodeRvGetListOfTimeSteps : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *ListOfTimeSteps;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetListOfTimeSteps(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&ListOfTimeSteps, rptContainer);
   }
};

struct nodeInGetListOfTimeSteps : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[1];
   cppRemoteParameter                    *analysisID;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetListOfTimeSteps(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
   }
};
/*===================================================================================================================*/
struct nodeRvGetListOfVerticesFromMesh : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[5];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *vertices;
   cppRemoteParameter                    *minID;
   cppRemoteParameter                    *maxID;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetListOfVerticesFromMesh(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&vertices, rptContainer);
      addAddribute(&minID, rptINTEGER);
      addAddribute(&maxID, rptINTEGER);
   }
};

struct nodeInGetListOfVerticesFromMesh : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *analysisID;
   cppRemoteParameter                    *stepValue;
   cppRemoteParameter                    *meshID;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetListOfVerticesFromMesh(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
      addAddribute(&stepValue, rptREAL);
      addAddribute(&meshID, rptINTEGER);
   }
};

/*===================================================================================================================*/
struct nodeRvGetListOfResultsFromTimeStepAndAnalysis : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *ResultInfoList;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetListOfResultsFromTimeStepAndAnalysis(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&ResultInfoList, rptContainer);
   }
};
struct nodeInGetListOfResultsFromTimeStepAndAnalysis : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[2];
   cppRemoteParameter                    *analysisID;
   cppRemoteParameter                    *stepValue;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetListOfResultsFromTimeStepAndAnalysis(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
      addAddribute(&stepValue, rptREAL);
   }
};

/*===================================================================================================================*/
struct nodeRvGetResultAtPoints : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[6];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *result_list;
   cppRemoteParameter                    *minID;
   cppRemoteParameter                    *maxID;
   cppRemoteParameter                    *nOfValuesPrVertex;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetResultAtPoints(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&result_list, rptCMemoryAllocator);
      addAddribute(&minID, rptINTEGER);
      addAddribute(&maxID, rptINTEGER);
      addAddribute(&nOfValuesPrVertex, rptINTEGER);
   }
};
struct nodeInGetResultAtPoints : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[4];
   cppRemoteParameter                    *analysisID;
   cppRemoteParameter                    *timeStep;
   cppRemoteParameter                    *resultID;
   cppRemoteParameter                    *listOfPoints;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetResultAtPoints(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
      addAddribute(&timeStep, rptREAL);
      addAddribute(&resultID, rptSTRING);
      addAddribute(&listOfPoints, rptContainer);
   }
};


/*===================================================================================================================*/
struct nodeRvGetResultFromVerticesID : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[6];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *result_list;
   cppRemoteParameter                    *minID;
   cppRemoteParameter                    *maxID;
   cppRemoteParameter                    *nOfValuesPrVertex;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetResultFromVerticesID(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&result_list, rptCMemoryAllocator);
      addAddribute(&minID, rptINTEGER);
      addAddribute(&maxID, rptINTEGER);
      addAddribute(&nOfValuesPrVertex, rptINTEGER);
   }
};
struct nodeInGetResultFromVerticesID : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[4];
   cppRemoteParameter                    *analysisID;
   cppRemoteParameter                    *timeStep;
   cppRemoteParameter                    *resultID;
   cppRemoteParameter                    *listOfVertices;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetResultFromVerticesID(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
      addAddribute(&timeStep, rptREAL);
      addAddribute(&resultID, rptSTRING);
      addAddribute(&listOfVertices, rptContainer);
   }
};


/*===================================================================================================================
typedef i64 NodeID;
struct Vertex {
   1: NodeID                              id
   2: double                              x
   3: double                              y
   4: double                              z
}

struct rvGetCoordinatesAndElementsFromMesh {
   1: string status
   2: string report
   3: list<Vertex>        vertex_list
   4: list<Element>       element_list
   5: list<ElementAttrib> element_attrib_list
   6: list<ElementGroup>  element_group_info_list
}
*/
struct nodeRvGetCoordinatesAndElementsFromMesh : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[5];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *node_array;
   cppRemoteParameter                    *elemnt_array;
   cppRemoteParameter                    *returned_mesh_info;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetCoordinatesAndElementsFromMesh(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&node_array, rptContainer);
      addAddribute(&elemnt_array, rptContainer);
      addAddribute(&returned_mesh_info, rptBLOB);
   }
};
struct nodeInGetCoordinatesAndElementsFromMesh : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *analysisID;
   cppRemoteParameter                    *timeStep;
   cppRemoteParameter                    *meshName;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetCoordinatesAndElementsFromMesh(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
      addAddribute(&timeStep, rptREAL);
      addAddribute(&meshName, rptSTRING);
   }
};


/*===================================================================================================================*/
struct nodeRvGetBoundaryOfLocalMesh : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[5];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *n_triangles;
   cppRemoteParameter                    *triangle_record_size;
   cppRemoteParameter                    *triangle_array;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetBoundaryOfLocalMesh(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&n_triangles, rptINTEGER);
      addAddribute(&triangle_record_size, rptINTEGER);
      addAddribute(&triangle_array, rptContainer);
   }
};
struct nodeInGetBoundaryOfLocalMesh : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *analysisID;
   cppRemoteParameter                    *timeStep;
   cppRemoteParameter                    *meshID;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetBoundaryOfLocalMesh(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
      addAddribute(&timeStep, rptREAL);
      addAddribute(&meshID, rptSTRING);
   }
};


/*===================================================================================================================*/
struct nodeRvGetNodeCoordinates : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *vertices;
   cppRemoteParameter                    *nOfNodesFound;
   cppRemoteParameter                    *return_error_str;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetNodeCoordinates(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&vertices, rptContainer);
      addAddribute(&nOfNodesFound, rptINTEGER);
      addAddribute(&return_error_str, rptSTRING);
   }
};
struct nodeInGetNodeCoordinates : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[2];
   cppRemoteParameter                    *nodeIdsFileName;
   cppRemoteParameter                    *nOfNodeIds;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetNodeCoordinates(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&nodeIdsFileName, rptSTRING);
      addAddribute(&nOfNodeIds, rptINTEGER);
   }
};


/*===================================================================================================================*/
struct nodeRvGetListOfMeshes : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *mesh_info_list;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetListOfMeshes(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&mesh_info_list, rptCMemoryAllocator);
   }
};
struct nodeInGetListOfMeshes : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[2];
   cppRemoteParameter                    *analysisID;
   cppRemoteParameter                    *timeStep;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetListOfMeshes(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
      addAddribute(&timeStep, rptREAL);
   }
};


/*===================================================================================================================*/
struct nodeRvInjectFiles : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[6];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *maxNodeId;
   cppRemoteParameter                    *maxElementId;
   cppRemoteParameter                    *timesteps;
   cppRemoteParameter                    *resultNames;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvInjectFiles(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&maxNodeId, rptINTEGER);
      addAddribute(&maxElementId, rptINTEGER);
      addAddribute(&timesteps, rptContainer);
      addAddribute(&resultNames, rptStringContainer);
   }
};
#define MAX_INJECT_FILES 6
struct nodeInInjectFiles : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[MAX_INJECT_FILES];
   cppRemoteParameter                    *fileName_0;
   cppRemoteParameter                    *fileName_1;
   cppRemoteParameter                    *fileName_2;
   cppRemoteParameter                    *fileName_3;
   cppRemoteParameter                    *fileName_4;
   cppRemoteParameter                    *fileName_5;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInInjectFiles(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&fileName_0, rptUndefined);
      addAddribute(&fileName_1, rptUndefined);
      addAddribute(&fileName_2, rptUndefined);
      addAddribute(&fileName_3, rptUndefined);
      addAddribute(&fileName_4, rptUndefined);
      addAddribute(&fileName_5, rptUndefined);
   }
};

#define min_X 0
#define min_Y 1
#define min_Z 2
#define max_X 3
#define max_Y 4
#define max_Z 5
/*===================================================================================================================*/
struct nodeRvCalculateBoundingBox : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[2];
   cppRemoteParameter                    *return_bbox;
   cppRemoteParameter                    *return_error_str;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvCalculateBoundingBox(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&return_bbox, rptSTRING);
      addAddribute(&return_error_str, rptSTRING);
   }
};
struct nodeInCalculateBoundingBox : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *analysisID;
   cppRemoteParameter                    *timeSteps;
   cppRemoteParameter                    *vertexIDs;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInCalculateBoundingBox(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
      addAddribute(&timeSteps, rptLongAggrREAL);
      addAddribute(&vertexIDs, rptLongAggrINTEGER);
   }
};


struct relocateResultOnVertex : public RelocateInfo
{
   Container<EDMVD::ResultOnVertex> *vertexResults;
};


struct relocateResultInfo : public RelocateInfo
{
   Container<EDMVD::ResultInfo> *sResults;
};


struct relocateMeshInfo : public RelocateInfo
{
   Container<EDMVD::MeshInfoEDM> *meshes;
};



} /* end namespace EDMVD */

using namespace EDMVD;