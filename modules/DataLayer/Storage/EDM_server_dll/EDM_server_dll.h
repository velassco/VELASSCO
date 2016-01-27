
/*!
   namespace EDM VELaSSCo Data - EDMVD
*/
namespace EDMVD {
   
   enum ElementShapeType {
      UnknownElement = 0,
      PointElement = 1,
      LineElement = 2,
      TriangleElement = 3,
      QuadrilateralElement = 4,
      TetrahedraElement = 5,
      HexahedraElement = 6,
      PrismElement = 7,
      PyramidElement = 8,
      SphereElement = 9,
      CircleElement = 10,
      ComplexParticleElement = 11
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
         componentNames = (Container<char*>*)mai->relocatePointer((char*)componentNames);
         if (componentNames) componentNames->relocatePointerContainer(mai);
      }
   } ResultInfo;


   typedef struct ElementType {
      ElementShapeType                    shape; // to avoid ambiguity meshinfo.type.type
      EDMULONG32                          num_nodes;
   } ElementType;


   typedef struct MeshInfo {
      char                                *name;
      ElementType                         elementType;
      EDMULONG                            nVertices;
      EDMULONG                            nElements;
      char                                *meshUnits;
      char                                *meshColor;
      EDMULONG32                          meshNumber;
      char                                *coordsName;

      void                                relocateThis(CMemoryAllocatorInfo *mai) {
         name = mai->relocatePointer(name);
         meshUnits = mai->relocatePointer(meshUnits);
         meshColor = mai->relocatePointer(meshColor);
         coordsName = mai->relocatePointer(coordsName);
      }
   } MeshInfo;


   typedef struct Element {
      EDMULONG                            id;
      Container<EDMULONG>                 *nodes_ids;
      void                                relocateThis(CMemoryAllocatorInfo *mai) {
         if (nodes_ids) nodes_ids->relocateStructContainer(mai);
      }
   } Element;


   typedef struct ElementAttrib {
      EDMULONG                            id;
      char                                *name;
      Container<double>                   *value;
      void                                relocateThis(CMemoryAllocatorInfo *mai) {
         name = mai->relocatePointer(name);
         if (value) value->relocateStructContainer(mai);
      }
   } ElementAttrib;


   typedef struct ElementGroup {
      EDMULONG                            id;
      EDMULONG                            group_id;
   } ElementGroup;


   typedef struct ResultOnVertex {
      EDMULONG                            id;
      Container<double>                   *value;
      void                                relocateThis(CMemoryAllocatorInfo *mai) {
         if (value) value->relocateStructContainer(mai);
      }
   } ResultOnVertex;



   enum ModelType { mtFEM, mtDEM, mtAP209, mtUnknown };
}

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
struct nodeRvGetCoordinatesAndElementsFromMesh : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[6];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *vertex_list;
   cppRemoteParameter                    *element_list;
   cppRemoteParameter                    *element_attrib_list;
   cppRemoteParameter                    *element_group_info_list;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetCoordinatesAndElementsFromMesh(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&vertex_list, rptContainer);
      addAddribute(&element_list, rptContainer);
      addAddribute(&element_attrib_list, rptContainer);
      addAddribute(&element_group_info_list, rptContainer);
   }
};
struct nodeInGetCoordinatesAndElementsFromMesh : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *analysisID;
   cppRemoteParameter                    *timeStep;
   cppRemoteParameter                    *meshInfo;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeInGetCoordinatesAndElementsFromMesh(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&analysisID, rptSTRING);
      addAddribute(&timeStep, rptREAL);
      addAddribute(&meshInfo, rptContainer);
   }
};


/*===================================================================================================================*/
struct nodeRvGetResultFromVerticesID : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *result_list;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetResultFromVerticesID(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&result_list, rptCMemoryAllocator);
   }
};
struct nodeInGetResultFromVerticesID : public CppParameterClass
{
   cppRemoteParameter                    *attrPointerArr[3];
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


struct relocateResultInfo : public RelocateInfo
{
   Container<EDMVD::ResultInfo> *sResults;
};


using namespace EDMVD;