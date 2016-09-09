/*!
   namespace EDM VELaSSCo Data - EDMVD
*/
namespace EDMVD {

class VELaSSCoEDMplugin
{
   CMemoryAllocator     *dllMa;
   CMemoryAllocator     *resultInfoMemory;

   char                 *QUERY_RESULT_FOLDER;
   char                 *repositoryName;
   char                 *modelName;
   char                 resultFolderBuffer[2048];
   char                 *getResultFileName(char *fileName, SdaiModel modelId);
  // mapped_region        nodeIdRegion();
   EDMLONG              *nodeIdArray;
   EDMLONG              nodeIdArraySize;
   //void                 setObjectId
   void                 setNodeId(fem::Node *n, EDMLONG nodeId);
   void                 initNodeIdMapping(char *fileName);
   void                 addMeshInfo(Container<EDMVD::MeshInfoEDM> *meshContainer, fem::Mesh *mesh);
public:
   VELaSSCoEDMplugin(char *crf, char *rn, char *mn) {
      dllMa = new CMemoryAllocator(0x100000); resultInfoMemory = NULL; QUERY_RESULT_FOLDER = crf; repositoryName = rn; modelName = mn;
      nodeIdArray = NULL; //nodeIdRegion = NULL; 
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
   EDMLONG              GetBoundaryOfSubMesh(Model *theModel, ModelType mt, nodeInGetBoundaryOfLocalMesh *inParam, nodeRvGetBoundaryOfLocalMesh *retVal);
   EDMLONG              GetListOfMeshes(Model *theModel, ModelType mt, nodeInGetListOfMeshes *inParam, nodeRvGetListOfMeshes *retVal);
   EDMLONG              InjectFiles(Model *theModel, ModelType mt, nodeInInjectFiles *inParam, nodeRvInjectFiles *retVal);
   void                 addNewResult(Container<EDMVD::ResultOnVertex> *resultsOnVertices, fem::entityType resType, void *vres, int nOfValuesPrVertex, EDMULONG *maxID, EDMULONG *minID, EDMLONG nodeID);
   EDMLONG              CalculateBoundingBox(Model *theModel, ModelType mt, nodeInCalculateBoundingBox *inParam, nodeRvCalculateBoundingBox *retVal);
   EDMLONG              GetResultAtPoints(Model *theModel, ModelType mt, nodeInGetResultAtPoints *inParam, nodeRvGetResultAtPoints *retVal);
   //EDMLONG              GetElementOfPointsInSpace(rvGetElementOfPointsInSpace& _return, const std::string& sessionID, const std::string& modelName, const std::vector<Point> & points);
};


} /* end namespace EDMVD */
