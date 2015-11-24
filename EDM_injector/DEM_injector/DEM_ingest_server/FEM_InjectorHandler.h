
#include "..\..\..\EDM_Interface\EDM_interface.h"

class FEM_InjectorHandler : public EDM_interface
{
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
   double                                 NX, NY, NZ;
   double                                 Orientation_YX, Orientation_YY, Orientation_YZ, Orientation_ZX, Orientation_ZY, Orientation_ZZ;
   double                                 Angular_Velocity_X, Angular_Velocity_Y, Angular_Velocity_Z, Kinetic_Energy;
   int                                    P1, P2, WALL;
   double                                 CX, CY, CZ, FX, FY, FZ;

   void                                   defineParticle();
   void                                   addCoordinates();
   void                                   addParticleResult();
   void                                   addVelocityMassVolume();
   char                                   *injectorFileName;
public:
   FEM_InjectorHandler(dbSchema *s)  {
      currentSchema = s; cContactID = 0;
   }
   
   void InjectFile(char *file_name);
   void InjectMesh(char *MeshFileFolder, char *MeshName, char *MeshVersion);
   char *readNextLine();
   void flushObjectsAndClose();
   void DeleteCurrentModelContent();
   void InitiateFileInjection();
};
