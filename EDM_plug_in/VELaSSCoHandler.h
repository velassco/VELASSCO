class VELaSSCoHandler : public VELaSSCoIf, public EDM_interface
{
   char                                         errmsg[2048];
   EDMmodelCache                                *setCurrentModelCache(const char *modelName);
   std::map<string, EDMmodelCache*>             caches;
public:
   Repository                                   *cFEMrep;
   Repository                                   *cDEMrep;

   VELaSSCoHandler();
   char *getErrorMsg(CedmError *e);
   char *getErrorMsg(int rstat);

   /**
   * Return the status of the different services
   * which run on the Data Layer.
   * @return string - returns a structured list of avialbe vertices,
   * 	with the attached list of double
   * 	if errors occur the contect is also returned here?
   *
   * @param sessionID
   * @param modelID
   * @param analysisID
   * @param timeStep
   * @param resultID
   * @param listOfVertices
   */
   void GetResultFormVerticesID(std::string& _return, const std::string& sessionID, const std::string& modelID, const std::string& analysisID, const double timeStep, const std::string& resultID, const std::string& listOfVertices);

   /**
   * returns a session if if the user exists with the specified password and the specified role or an empty role.
   *
   * @param user_name
   * @param role
   * @param password
   */
   void UserLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password);

   /**
   * Stop access to the system by a given session id and release all resources held by that session
   *
   * @param sessionID
   */
   void UserLogout(std::string& _return, const std::string& sessionID) ;
   
   /**
   * For each point in the input parameter points, the method returns data about the element that contains the point.
   * The number of elements in the returned list of elements shall be the same as the number of points in the input parameter.
   * If the method does not find an element for a point it shall return a dummy element with id equal to -1.
   *
   * @param sessionID
   * @param modelName
   * @param points
   */
   void GetElementOfPointsInSpace(rvGetElementOfPointsInSpace& _return, const std::string& sessionID, const std::string& modelName, const std::vector<Point> & points);
   
   /**
   * GetBoundaryOfLocalMesh will get the partial tetrahedral elements, calculate and output the unique triangles,
   * i.e. triangles that are not shared between two or more elements.
   *
   * @param sessionID
   * @param modelName
   * @param meshID
   * @param analysisID
   * @param time_step
   */
   void GetBoundaryOfLocalMesh(rvGetBoundaryOfLocalMesh& _return, const std::string& sessionID, const std::string& modelName, const std::string& meshID, const std::string& analysisID, const double time_step);

   
   void CalculateBoundaryOfMesh(FEMmodelCache *fmc, std::vector<Triangle> &elements);
   void ReportError(char *f) { printf(f); }
   void InitQueryCaches();
};


typedef struct BoundingBox {
   double min_x, min_y, min_z, max_x, max_y, max_z;
} BoundingBox;