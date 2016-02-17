


class VELaSSCoMethods : public EDMclusterExecution
{
   char                                         *getPluginPath();
   char                                         *getPluginName();
public:
   VELaSSCoMethods(EDMclusterServices *cs) : EDMclusterExecution(cs) { }
   ~VELaSSCoMethods();
   void                 GetListOfAnalyses(rvGetListOfAnalyses& _return);
   void                 ListModels();
   void                 ValidateModels();
   void                 getBoundingBox();
   void                 GetListOfResults(char *modelId, char *analysisID, double timeStep);
   void                 GetListOfTimeSteps(rvGetListOfTimeSteps& rv, const std::string& analysisID);
   void                 GetListOfVerticesFromMesh(rvGetListOfVerticesFromMesh& rv, const std::string& analysisID, const double stepValue, const int32_t meshID);
   void                 GetListOfResultsFromTimeStepAndAnalysis(rvGetListOfResults& rv, const std::string& analysisID, const double stepValue);
   void                 GetCoordinatesAndElementsFromMesh(rvGetCoordinatesAndElementsFromMesh& rv, const std::string& analysisID, const double stepValue);
   void                 GetResultFromVerticesID(rvGetResultFromVerticesID& rv, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::vector<int64_t> & listOfVertices);
   void                 GetBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& rv, const std::string& analysisID, const double stepValue, const std::string& meshID);
   void                 GetListOfMeshes(rvGetListOfMeshes& rv, const std::string& analysisID, const double stepValue);
};


class VELaSSCoCluster : public EDMclusterServices
{
public:
   VELaSSCoCluster(Model *m) : EDMclusterServices(m) { }
};