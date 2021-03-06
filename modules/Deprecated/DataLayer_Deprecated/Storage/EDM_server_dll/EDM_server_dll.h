
/*!
   namespace EDM VELaSSCo Data - EDMVD
*/
namespace EDMVD {
   

  struct Triangle {
      EDMULONG                            node_ids[3];
      
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
//      ElementShapeType                    shape; // to avoid ambiguity meshinfo.type.type
      EDMULONG32                          num_nodes;
   } ElementType;


   typedef struct MeshInfo {
      char                                *name;        // will return mesh info about the mesh with the specified name - error message if specified mesh does not exist.
      ElementType                         elementType;  // used to calculate number of nodes to be returned for each element
      EDMULONG                            nVertices;    // used to calculate buffer sizes
      EDMULONG                            nElements;    // used to calculate buffer sizes
      char                                *meshUnits;   // not used yet
      char                                *meshColor;   // not used yet
      EDMULONG32                          meshNumber;   // not relevant for EDM
      char                                *coordsName;  // not used yet

      void                                relocateThis(CMemoryAllocatorInfo *mai) {
         name = mai->relocatePointer(name);
         meshUnits = mai->relocatePointer(meshUnits);
         meshColor = mai->relocatePointer(meshColor);
         coordsName = mai->relocatePointer(coordsName);
      }
   } MeshInfo;


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
      EDMULONG                            id;
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
   cppRemoteParameter                    *attrPointerArr[2];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvInjectFiles(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
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
   Container<EDMVD::MeshInfo> *meshes;
};

class VELaSSCoEDMplugin
{
   CMemoryAllocator     *dllMa;
   CMemoryAllocator     *resultInfoMemory;

   char                 *QUERY_RESULT_FOLDER;
   char                 *repositoryName;
   char                 *modelName;
   char                 resultFolderBuffer[2048];
   char                 *getResultFileName(char *fileName, SdaiModel modelId);
public:
   VELaSSCoEDMplugin(char *crf, char *rn, char *mn) {
      dllMa = new CMemoryAllocator(0x100000); resultInfoMemory = NULL; QUERY_RESULT_FOLDER = crf; repositoryName = rn; modelName = mn;
   }
   ~VELaSSCoEDMplugin();

   char                 *handleError(CedmError *e);
   EDMVD::ModelType     getModelType(SdaiModel sdaiModelID);
   CMemoryAllocator     *getMemoryAllocator() { return dllMa; }
   void                 *alloc(EDMLONG size) { return dllMa->alloc(size); }

   EDMLONG              GetListOfAnalyses(Model *theModel, EDMVD::ModelType mt, nodervGetListOfAnalyses *retVal);
   EDMLONG              GetListOfTimeSteps(Model *theModel, ModelType mt, nodeInGetListOfTimeSteps *inParam, nodeRvGetListOfTimeSteps *retVal);
   EDMLONG              GetListOfVerticesFromMesh(Model *theModel, ModelType mt, nodeInGetListOfVerticesFromMesh *inParam, nodeRvGetListOfVerticesFromMesh *retVal);
   EDMLONG              GetListOfResultsFromTimeStepAndAnalysis(Model *theModel, ModelType mt, nodeInGetListOfResultsFromTimeStepAndAnalysis *inParam, nodeRvGetListOfResultsFromTimeStepAndAnalysis *retVal);
   EDMLONG              GetResultFromVerticesID(Model *theModel, ModelType mt, nodeInGetResultFromVerticesID *inParam, nodeRvGetResultFromVerticesID *retVal);
   EDMLONG              GetCoordinatesAndElementsFromMesh(Model *theModel, ModelType mt, nodeInGetCoordinatesAndElementsFromMesh *inParam, nodeRvGetCoordinatesAndElementsFromMesh *retVal);
   EDMLONG              GetBoundaryOfLocalMesh(Model *theModel, ModelType mt, nodeInGetBoundaryOfLocalMesh *inParam, nodeRvGetBoundaryOfLocalMesh *retVal);
   EDMLONG              GetListOfMeshes(Model *theModel, ModelType mt, nodeInGetListOfMeshes *inParam, nodeRvGetListOfMeshes *retVal);
   EDMLONG              InjectFiles(Model *theModel, ModelType mt, nodeInInjectFiles *inParam, nodeRvInjectFiles *retVal);
};



} /* end namespace EDMVD */

using namespace EDMVD;