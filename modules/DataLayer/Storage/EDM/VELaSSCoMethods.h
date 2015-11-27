


class VELaSSCoMethods : public EDMclusterExecution
{
public:
   VELaSSCoMethods(EDMclusterServices *cs) : EDMclusterExecution(cs) { }
   void                 GetListOfAnalyses(rvGetListOfAnalyses& _return, const std::string& modelID);
   void                 ListModels();
   void                 ValidateModels();
   void                 getBoundingBox();
   void                 GetListOfResults(char *modelId, char *analysisID, double timeStep);
   void                 GetListOfTimeSteps(rvGetListOfTimeSteps& rv, char *modelId, char *analysisID);
};


class VELaSSCoCluster : public EDMclusterServices
{
public:
   VELaSSCoCluster(Model *m) : EDMclusterServices(m) { }
};