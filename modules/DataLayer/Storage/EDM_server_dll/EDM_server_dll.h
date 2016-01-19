
/*!
   namespace EDM VELaSSCo Data - EDMVD
*/
namespace EDMVD {

   typedef struct Vertex {
      EDMULONG                            id;
      double                              x;
      double                              y;
      double                              z;
   } Vertex;

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
      addAddribute(&analysis_name_list, rptStringCollection);
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
      addAddribute(&ListOfTimeSteps, rptCollection);
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
   cppRemoteParameter                    *attrPointerArr[3];
   cppRemoteParameter                    *status;
   cppRemoteParameter                    *report;
   cppRemoteParameter                    *vertices;

   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }
   nodeRvGetListOfVerticesFromMesh(CMemoryAllocator *_ma, cppRemoteParameter *inAttrPointerArr)
      : CppParameterClass(attrPointerArr, sizeof(attrPointerArr), _ma, inAttrPointerArr) {
      addAddribute(&status, rptSTRING);
      addAddribute(&report, rptSTRING);
      addAddribute(&vertices, rptCollection);
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


using namespace EDMVD;