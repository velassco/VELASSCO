
#include "..\..\..\EDM_Interface\EDM_interface.h"

class DEM_InjectorHandler : public DEM_InjectorIf, public EDM_interface
{
   map<int, dem::Particle*>         particles;
public:
   DEM_InjectorHandler(dbSchema *s)  {
      currentSchema = s;
   }
   void UserLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password);
   void UserLogout(std::string& _return, const std::string& sessionID);
   void StoreDEM_Simulation(std::string& _return, const std::string& sessionID, const std::string& model_name, const DEM_Inject::Simulation& theSimulation);
   void DeleteModelContent(std::string& _return, const std::string& sessionID, const std::string& model_name);
};
