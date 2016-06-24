

class VELaSSCoHandler : public VELaSSCo_VQueries, public EDM_interface
{
   char                                         errmsg[2048];
   //EDMmodelCache                                *setCurrentModelCache(SdaiModel modelID);
   //std::map<SdaiModel, EDMmodelCache*>          caches;
   CLoggWriter                                  *thelog;
   EDMclusterServices                           *theCluster;

   SdaiInstance                                 getClusterModelID(const char *repName, const char *modelName, EDMLONG *rstatp, SdaiInteger *nOfNameMatches);
public:
   Repository                                   *cFEMrep;
   Repository                                   *cDEMrep;

   VELaSSCoHandler();
   char *getErrorMsg(CedmError *e);
   char *getErrorMsg(int rstat);
   void defineLogger(CLoggWriter *lw) { thelog = lw; }
   void handleError(string &errMsg, CedmError *e);

   /**
   * Return the status of the different services
   * which run on the Data Layer.
   */
   void statusDL(std::string& _return);

   /**
   * Data Query operations
   *
   * @param sessionID
   * @param model_group_qualifier
   * @param model_name_pattern
   */
   void getListOfModels(rvGetListOfModels& _return, const std::string& sessionID, const std::string& model_group_qualifier, const std::string& model_name_pattern);
   void openModel(rvOpenModel& _return, const std::string& sessionID, const std::string& unique_model_name_pattern, const std::string& requested_access);

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
   void getResultFromVerticesID(rvGetResultFromVerticesID& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::vector<int64_t> & listOfVertices);

   /**
   * Return the coordinates and elements of a model's mesh.
   * @return string - returns a structured list of vertices and elements of a model's mesh.
   * if errors occur the contect is also returned here?
   *
   * @param sessionID
   * @param modelID
   * @param analysisID
   * @param timeStep
   * @param meshInfo
   */
   void getCoordinatesAndElementsFromMesh(rvGetCoordinatesAndElementsFromMesh& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const MeshInfo& meshInfo);

   /**
   * Stop Data Layer
   */
   void stopAll();

   /**
   * returns a session if if the user exists with the specified password and the specified role or an empty role.
   *
   * @param user_name
   * @param role
   * @param password
   */
   void userLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password);

   /**
   * Stop access to the system by a given session id and release all resources held by that session
   *
   * @param sessionID
   */
   void userLogout(std::string& _return, const std::string& sessionID);

   /**
   * Description: Removes the possibility to access a model via a previously assigned
   * GUID (OpenModel). Corresponding housekeeping is wrapped up.
   *
   * @param sessionID
   * @param modelName
   */
   void closeModel(std::string& _return, const std::string& sessionID, const std::string& modelName);

   /**
   * Description: Store a new thumbnail of a model
   *
   * @param sessionID
   * @param modelID
   * @param imageFile
   */
   void setThumbnailOfAModel(std::string& _return, const std::string& sessionID, const std::string& modelID, const std::string& imageFile);

   /**
   * Description: Return thumbnail of a model.
   *
   * @param sessionID
   * @param modelID
   */
   void getThumbnailOfAModel(rvGetThumbnailOfAModel& _return, const std::string& sessionID, const std::string& modelID);

   /**
   * For each point in the input parameter points, the method returns data about the element that contains the point.
   * The number of elements in the returned list of elements shall be the same as the number of points in the input parameter.
   * If the method does not find an element for a point it shall return a dummy element with id equal to -1.
   *
   * @param sessionID
   * @param modelName
   * @param points
   */
   void getElementOfPointsInSpace(rvGetElementOfPointsInSpace& _return, const std::string& sessionID, const std::string& modelName, const std::vector<Point> & points);

   /**
   * GetBoundaryOfLocalMesh will get the partial tetrahedral elements, calculate and output the unique triangles,
   * i.e. triangles that are not shared between two or more elements.
   *
   * @param sessionID
   * @param modelID
   * @param meshID
   * @param analysisID
   * @param time_step
   */
   void getBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& _return, const std::string& sessionID, const std::string& modelID, const std::string& meshID, const std::string& analysisID, const double time_step);

   /**
   * Retrieves the list of time steps for a given model and analysis.
   *
   * @param sessionID
   * @param modelID
   */
   void getListOfAnalyses(rvGetListOfAnalyses& _return, const std::string& sessionID, const std::string& modelID);

   void calculateBoundingBox(const std::string &sessionID, const std::string &modelID, const std::string &dataTableName,
      const std::string &analysisID, const int numSteps, const double *lstSteps,
      const int64_t numVertexIDs, const int64_t *lstVertexIDs,
      double *return_bbox, std::string *return_error_str) ;
   /**
   * Retrieves the list of time steps for a given model and analysis.
   *
   * @param sessionID
   * @param modelID
   * @param analysisID
   */
   void getListOfTimeSteps(rvGetListOfTimeSteps& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const std::string &stepOptions, const int numSteps, const double *lstSteps);

   /**
   * Retrieves the list of results for a given model, analysis and step-value
   * as of OP-22.115
   *
   * @param sessionID
   * @param modelID
   * @param analysisID
   * @param stepValue
   */
   void getListOfResultsFromTimeStepAndAnalysis(rvGetListOfResults& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double stepValue);

   /**
   * Extract a list of vertices from the open model and the selected meshID
   * as of OP-22.116
   *
   * @param sessionID
   * @param modelID
   * @param analysisID
   * @param stepValue
   * @param meshID
   */
   void getListOfVerticesFromMesh(rvGetListOfVerticesFromMesh& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double stepValue, const int32_t meshID);

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
   void getListOfMeshes(rvGetListOfMeshes& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep);
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
//   void GetBoundingBox(rvGetBoundingBox& _return, const std::string& sessionID, const std::string& modelID, const std::vector<int64_t> & verticesID, const std::string& analysisID, const std::string& timeStepOption, const std::vector<double> & timeSteps);
   /**
   * Create cluster model creates, as the name says, a cluster model. The specified schema must exist
   * in all the EDM databases in the clulster.
   * The physical models are created in the databases in the cluster in a repository named
   * clusterRepositoryName. If number of physical models are equal to the number of databases in the
   * cluster, there will be created one model in each database.
   * If the physical models already exist, this method will only create metadata about the cluster model
   * in the cluster database.
   *
   * @param sessionID
   * @param clusterRepositoryName
   * @param clusterModelName
   * @param schemaName
   * @param physicalModelNames
   */
//   void CreateClusterModel(rvCreateClusterModel& _return, const std::string& sessionID, const std::string& clusterRepositoryName, const std::string& clusterModelName, const std::string& schemaName, const std::vector<std::string> & physicalModelNames);

//   void CalculateBoundaryOfMesh(FEMmodelCache *fmc, std::vector<Triangle> &elements);
   void ReportError(char *f);
   void InitQueryCaches();
   //void GetFEMresultFromVerticesID(rvGetResultFromVerticesID_B& _return, bool allNodes, std::map<int, int> & nodesInParameter, const char *resultID, const double time_step, const char *analysisID, FEMmodelCache *fmc);
   //void GetDEMresultFromVerticesID(rvGetResultFromVerticesID_B& _return, bool allNodes, std::map<int, int> & nodesInParameter, const char *resultID, const double time_step, const char *analysisID, DEMmodelCache *dmc);
   void defineCluster(EDMclusterServices *cs) { theCluster = cs; }
   void InjectData(char *commandFileName);
   bool stopConnection() { return true;  }
   void getStatusDB(std::string &s)
   {
      s = "OK";
   }

   void calculateDiscrete2Continuum(const std::string &sessionID, const std::string &modelID,
      const std::string &analysisName, const std::string &staticMeshID, const std::string &tSOptions,
      const int numSteps, const double *lstSteps, const std::string &coarseGrainingMethod,
      const double width, const double cutoffFactor,
      const bool processContacts, const bool doTemporalAVG, const std::string &temporalAVGoptions,
      const double deltaT,
      std::string *returnQueryOutcome, std::string *return_error_str)
   {
      return_error_str->append("calculateDiscrete2Continuum is not implemented");
   }

   std::string MRgetListOfVerticesFromMesh(rvGetListOfVerticesFromMesh &_return,
      const std::string &sessionID, const std::string &modelID,
      const std::string &dataTableName,
      const std::string &analysisID, const double stepValue,
      const int32_t meshID)  { return ""; }

   void calculateBoundaryOfAMesh(const std::string &sessionID, const std::string &DataLayer_sessionID,
      const std::string &modelID, const std::string &dataTableName,
      const int meshID, const std::string &elementType,
      const std::string &analysisID, const double stepValue,
      std::string *return_binary_mesh, std::string *return_error_str)  {}
   
};
