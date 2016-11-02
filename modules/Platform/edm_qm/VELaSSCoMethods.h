


class VELaSSCoMethods : public EDMclusterExecution
{
   char                                         *getPluginPath();
   char                                         *getPluginName();
   CLoggWriter                                  *thelog;
public:
   VELaSSCoMethods(EDMclusterServices *cs, CLoggWriter *tl) : EDMclusterExecution(cs) { thelog = tl; }
   ~VELaSSCoMethods();
   void                 GetListOfAnalyses(rvGetListOfAnalyses& _return);
   void                 ListModels();
   void                 ValidateModels();
   void                 getBoundingBox();
   void                 GetListOfResults(char *modelId, char *analysisID, double timeStep);
   Container<double>    *GetListOfTimeSteps(rvGetListOfTimeSteps& rv, const std::string& analysisID);
   void                 setResults(rvGetListOfTimeSteps& rv, Container<double> *allTimeSteps);
   void                 GetListOfVerticesFromMesh(rvGetListOfVerticesFromMesh& rv, const std::string& analysisID, const double stepValue, const int32_t meshID);
   void                 GetListOfResultsFromTimeStepAndAnalysis(rvGetListOfResults& rv, const std::string& analysisID, const double stepValue);
   void                 GetCoordinatesAndElementsFromMesh(rvGetCoordinatesAndElementsFromMesh& rv, const std::string& analysisID, const double stepValue);
   void                 GetResultFromVerticesID(rvGetResultFromVerticesID& rv, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::vector<int64_t> & listOfVertices);
//   bool                 GetBoundaryOfLocalMesh(const std::string& analysisID, const double stepValue, const int meshID, MergeJob **lastMergep, std::string *return_error_str);
   void                 GetListOfMeshes(rvGetListOfMeshes& rv, const std::string& analysisID, const double stepValue);
   void                 InjectFileSequence(Container<char*> *FileNameFormats, int FirstModelNo, int LastModelNo, char *EDMmodelNameFormat, Container<char*> *msgs);
   void                 CreateCubeFromFileSequence(Container<char*> *FileNameFormats, int FirstModelNo, int LastModelNo, char *EDMmodelNameFormat, Container<char*> *msgs);
   void                 calculateBoundaryOfLocalMesh(const int meshID, const std::string &elementType, const std::string &analysisID, const double stepValue, char *tempFileForNodeIds, std::string *return_binary_mesh, std::string *return_error_str);
   void                 calculateBoundingBox(const std::string &analysisID, const int numSteps, const double *lstSteps,
                           const int64_t numVertexIDs, const int64_t *lstVertexIDs,
                           double *return_bbox, std::string *return_error_str);
};


class VELaSSCoCluster : public EDMclusterServices
{
public:
   VELaSSCoCluster(Model *m) : EDMclusterServices(m) { }
};
