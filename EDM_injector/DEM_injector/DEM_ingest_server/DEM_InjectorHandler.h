
#include "..\..\..\EDM_Interface\EDM_interface.h"

class DEM_InjectorHandler : public DEM_InjectorIf, public EDM_interface
{
   map<int, dem::Particle*>               particles;
   map<int, dem::FEM_mesh*>               meshes;
   int                                    cContactID;

   // Data when injecting from files
   map<double, dem::Timestep*>            timesteps;
   char                                   line[10240];
   size_t                                 nRead;
   FILE                                   *fp;
   dem::Simulation                        *theSimulation;
   dem::Particle                          *p;
   dem::Template_nn                       *tnn;
   dem::Timestep                          *ts;
   dem::Particle_result                   *pr;

   bool                                   newTimestep = false;
   double                                 timeStep;
   int                                    ID, GROUP, TYPE;
   double                                 VOLUME, MASS, PX, PY, PZ, VX, VY, VZ, Orientation_XX, Orientation_XY, Orientation_XZ;
   double                                 Orientation_YX, Orientation_YY, Orientation_YZ, Orientation_ZX, Orientation_ZY, Orientation_ZZ;
   double                                 Angular_Velocity_X, Angular_Velocity_Y, Angular_Velocity_Z, Kinetic_Energy;

   void                                   defineParticle();
   void                                   addCoordinates();
   void                                   addParticleResult();
   void                                   addVelocityMassVolume();
   char                                   *injectorFileName;
public:
   DEM_InjectorHandler(dbSchema *s)  {
      currentSchema = s; cContactID = 0;
   }
   void UserLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password);
   void UserLogout(std::string& _return, const std::string& sessionID);
   void StoreDEM_Simulation(std::string& _return, const std::string& sessionID, const std::string& model_name, const DEM_Inject::Simulation& theSimulation);
   void DeleteModelContent(std::string& _return, const std::string& sessionID, const std::string& model_name);
   
   void InjectFile(char *file_name);
   void InjectMesh(char *MeshFileFolder, char *MeshName, char *MeshVersion);
   char *readNextLine();
   void store_TIMESTEP_PARTICLES();
   void store_TIMESTEP_CONTACTS();
   void flushObjectsAndClose();
   void DeleteCurrentModelContent();
   void InitiateFileInjection();
   dem::Particle *findParticle(int particleID);
   dem::Custom_property_vector *newCustomPropertyVector(char *name, double x, double y, double z);
   dem::Custom_property_scalar *newCustomPropertyScalar(char *name, double x);
};
