// DEM_ingest_server.cpp : Defines the entry point for the console application.

#include "stdafx.h"


using namespace  ::DEM_Inject;

#include "DEM_InjectorHandler.h"


#define newObject(className) new(m)className(m)



void gen_random(char *s, const int len) {
   static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
   for (int i = 0; i < len; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum)-1)];
   }
   s[len] = 0;
}
/**
* returns a session if if the user exists with the specified password and the specified role or an empty role.
*
* @param user_name
* @param role
* @param password
*/
void DEM_InjectorHandler::UserLogin(std::string& _return, const std::string& user_name, const std::string& role, const std::string& password) {
   char sessionID[64];
   gen_random(sessionID, sizeof(sessionID)); _return = sessionID;
}

/**
* Stop access to the system by a given session id and release all resources held by that session
*
* @param sessionID
*/
void DEM_InjectorHandler::UserLogout(std::string& _return, const std::string& sessionID) {
   _return = "OK";
}

/**
* Stores a DEM_simulation with all timesteps, meshes, particles, results etc. in one operation in EDM
* If the model exists, the Simulation is added to the model. If the model does not exist, it is created.
*
* @param sessionID
* @param model_name
* @param theSimulation
*/

void DEM_InjectorHandler::StoreDEM_Simulation(std::string& _return, const std::string& sessionID, const std::string& model_name, const DEM_Inject::Simulation& theSimulation)
{
   try {
      setCurrentSession(sessionID.data());
      setCurrentModel(model_name.data());
      CMemoryAllocator *ma = m->getMemoryAllocator();
      m->reset();
      
      printf("StoreDEM_Simulation\n");
      dem::Simulation *sim = newObject(dem::Simulation);
      sim->put_name((char*)theSimulation.name.data());

      vector<DEM_Inject::Timestep> tsteps = theSimulation.consists_of;
      printf("There are %d Timesteps\nEach timestep has the following attributes:\n", tsteps.size());
      vector<DEM_Inject::Timestep>::iterator ts = tsteps.begin();
      for (ts; ts != tsteps.end(); ts++) {
         dem::Timestep *dts = newObject(dem::Timestep );
         sim->put_consists_of_element(dts); dts->put_time_value(ts->time_value);
         vector<DEM_Inject::Particle>::iterator pa = ts->consists_of.begin();
         for (pa; pa != ts->consists_of.end(); pa++) {
            dem::Particle *p;
            if (pa->type == Sphere_Particle) {
               dem::Sphere *sp = newObject(dem::Sphere); p = sp; sp->put_radius(pa->radius);
            } else {
               dem::Template_nn *tn = newObject(dem::Template_nn); p = tn;
            }
            p->put_id(pa->id); p->put_group(pa->group);
            if (particles[pa->id]) {
               _return = "particle already defined"; return;
            } else {
               particles[pa->id] = p;
            }
         }
         vector<DEM_Inject::Contact>::iterator contact = ts->has_contact.begin();
         for (contact; contact != ts->has_contact.end(); contact++) {
            dem::Contact *dcp;
            dem::Particle *p;
            if (contact->type == Geometry_Contact) {
               dem::Particle_Geometry_contact *dpgc = newObject(dem::Particle_Geometry_contact); dcp = dpgc;
            } else {
               dem::Particle_Particle_contact *dppc = newObject(dem::Particle_Particle_contact); dcp = dppc;
               if (contact->P1_id && (p = particles[contact->P1_id])) {
                  dppc->put_P1(p);
               }
               if (contact->P2_id && (p = particles[contact->P2_id])) {
                  dppc->put_P2(p);
               }
            }
            dcp->put_id(cContactID++);
            dts->put_has_contact_element(dcp);
            vector<double>::iterator coordIter = contact->contact_location.begin();
            for (int i = 1; i < 4; i++) {
               if (coordIter == contact->contact_location.end())
                  break;
               dcp->put_contact_location_element(i, *coordIter); coordIter++;
            }
         }
      }
     // m->writeAllObjectsToDatabase();

      _return = "OK";
   } catch (CedmError *e) {
      if (e->message) {
         _return = e->message;
      } else {
         _return = edmiGetErrorText(e->rstat);
      }
      delete e;
   }
}

/**
* Stores a DEM_simulation with all timesteps, meshes, particles, results etc. in one operation in EDM
* If the model exists, the Simulation is added to the model. If the model does not exist, it is created.
*
* @param sessionID
* @param model_name
*/
void DEM_InjectorHandler::DeleteModelContent(std::string& _return, const std::string& sessionID, const std::string& model_name)
{
   try {
      setCurrentSession(sessionID.data());
      setCurrentModel(model_name.data());
      CHECK(edmiDeleteModelContents(m->modelId));
   } catch (CedmError *e) {
      if (e->message) {
         _return = e->message;
      } else {
         _return = edmiGetErrorText(e->rstat);
      }
      delete e;
   }
}

void DEM_InjectorHandler::DeleteCurrentModelContent()
{
   CHECK(edmiDeleteModelContents(m->modelId));
}

void DEM_InjectorHandler::InitiateFileInjection()
{
   theSimulation = newObject(dem::Simulation);
   theSimulation->put_name("theSimulation");
}

void DEM_InjectorHandler::InjectFile(char *file_name)
{
   injectorFileName = file_name;
   fp = fopen(file_name, "r");
   if (fp) {
      while (readNextLine() > 0) {
         if (strEQL(line, "TIMESTEP PARTICLES\n")) {
            store_TIMESTEP_PARTICLES();
         } else if (strEQL(line, "TIMESTEP CONTACTS\n")) {
            store_TIMESTEP_CONTACTS();
         }
      }
      fclose(fp);
   } else {
      THROW("Unaple to open DEM data file.");
   }
}

void DEM_InjectorHandler::InjectMesh(char *MeshFileFolder, char *MeshName, char *MeshVersion)
{
   char fileName[1024], stlline[2048], command[512], x[512], y[512], z[512], p4[512], p5[512], MeshNameWithVersion[512];
   CMemoryAllocator* ma = m->getMemoryAllocator();

   newTimestep = false;
   for (std::map<double, dem::Timestep*>::iterator tsit = timesteps.begin(); tsit != timesteps.end(); ++tsit) {
      dem::Timestep* ts = tsit->second;
      dem::FEM_mesh *mesh = NULL;
      Set<dem::Vertex*> *facet;

      sprintf(fileName, "%s/%s_%s_%.0lf.stl", MeshFileFolder, MeshName, MeshVersion, tsit->first);
      FILE *stlFile = fopen(fileName, "r");
      if (stlFile) {
         int vertexNo = 0;
         while (fgets(stlline, sizeof(stlline), stlFile)) {
            int nColumns = sscanf(stlline, "%s %s %s %s %s %s", command, x, y, z, p4, p5);
            if (strEQL(command, "endsolid")) {
               break;
            } else if (strEQL(command, "vertex")) {
               if (vertexNo++ == 0) {
                  // new facet
                  if (mesh == NULL) {
                     mesh = newObject(dem::FEM_mesh); ts->put_boundary_element(mesh);
                     meshes[atol(MeshVersion)] = mesh;
                     sprintf(MeshNameWithVersion, "%s_%s", MeshName, MeshVersion); mesh->put_name(ma->allocString(MeshNameWithVersion));
                  }
                  facet = new(ma)Set<dem::Vertex*>(m);
                  mesh->put_facets_element(facet);
               }
               dem::Vertex *v = newObject(dem::Vertex);
               v->put_vertex_position_element(atof(x)); v->put_vertex_position_element(atof(y)); v->put_vertex_position_element(atof(z));
               facet->add(v);
               if (vertexNo >= 3) vertexNo = 0; // build another facet next time
            }
         }
         fclose(stlFile);
      } else {
         printf("Illegal Mesh file name: %s\n", fileName);
      }
   }
}



void DEM_InjectorHandler::store_TIMESTEP_CONTACTS()
{
   int               nOfContacts, nValues;
   
   newTimestep = false;
   readNextLine();
   sscanf(line, "%lf %d", &timeStep, &nOfContacts);
   dem::Timestep *ts = timesteps[timeStep];
   if (!ts) {
      ts = newObject(dem::Timestep); ts->put_time_value(timeStep); timesteps[timeStep] = ts;
      theSimulation->put_consists_of_element(ts); newTimestep = true;
   }

   readNextLine();
   bool particle_particle_contact = strEQL(line, " P1 P2 CX CY CZ FX FY FZ\n") || strEQL(line, "P1 P2 CX CY CZ FX FY FZ\n");

   if (particle_particle_contact || strEQL(line, " P1 CX CY CZ FX FY FZ\n")) {
      for (int i = 0; i < nOfContacts; i++) {
         dem::Particle *p1, *p2;
         dem::Contact *c = NULL;
         readNextLine();
         if (particle_particle_contact) {
            nValues = sscanf(line, "%d %d %lf %lf %lf %lf %lf %lf", &P1, &P2, &CX, &CY, &CZ, &FX, &FY, &FZ);
         } else {
            nValues = sscanf(line, "%d %lf %lf %lf %lf %lf %lf", &P1, &CX, &CY, &CZ, &FX, &FY, &FZ);
         }
         if (nValues != (particle_particle_contact ? 8 : 7)) {
            printf("Error - Number of values in particle to particle contact line is illegal.\n");
         } else {
            p1 = findParticle(P1);
            if (p1) {
               if (particle_particle_contact) {
                  p2 = findParticle(P2);
                  if (p2) {
                     dem::Particle_Particle_contact *ppc = newObject(dem::Particle_Particle_contact); c = ppc;
                     ppc->put_P1(p1); ppc->put_P2(p2);
                  }
               } else {
                  dem::Particle_Geometry_contact *pgc = newObject(dem::Particle_Geometry_contact); c = pgc;
                  pgc->put_P1(p1);
               }
               if (c) {
                  c->put_id(cContactID++);
                  c->put_contact_location_element(1, CX); c->put_contact_location_element(2, CY); c->put_contact_location_element(3, CZ);
                  dem::Contact_result *cr = newObject(dem::Contact_result);
                  dem::Contact_Force  *cf = newObject(dem::Contact_Force);
                  cf->put_Fx_Fy_Fz_element(FX); cf->put_Fx_Fy_Fz_element(FY); cf->put_Fx_Fy_Fz_element(FZ);
                  cr->put_contact_results_properties_element(cf);
                  cr->put_calculated_for(ts);
                  cr->put_valid_for(c);
                  ts->put_has_contact_element(c);
               }
            }
         }
      }
   } else if (strEQL(line, " P1 P2 FX FY FZ\n")) {
      for (int i = 0; i < nOfContacts; i++) {
         readNextLine();
         nValues = sscanf(line, "%d %d %lf %lf %lf", &P1, &P2, &FX, &FY, &FZ);
         if (nValues == 5) {
            dem::Particle *p1 = findParticle(P1);
            dem::Particle *p2 = findParticle(P2);
            if (p1 && p1) {
               dem::Particle_Particle_contact *ppc = newObject(dem::Particle_Particle_contact);
               dem::Contact *c = ppc;
               ppc->put_P1(p1); ppc->put_P2(p2);
               c->put_id(cContactID++);
               dem::Contact_result *cr = newObject(dem::Contact_result);
               dem::Contact_Force  *cf = newObject(dem::Contact_Force);
               cf->put_Fx_Fy_Fz_element(FX); cf->put_Fx_Fy_Fz_element(FY); cf->put_Fx_Fy_Fz_element(FZ);
               cr->put_contact_results_properties_element(cf);
               cr->put_calculated_for(ts);
               cr->put_valid_for(c);
               ts->put_has_contact_element(c);
            } else {
               printf("Illegal particle number in particle contact file \"%s\"\n", injectorFileName);
            }
         } else {
            printf("Illegal number of columns in particle contact file \"%s\"\n", injectorFileName);
         }
      }
   } else if (strEQL(line, "P1 WALL CX CY CZ FX FY FZ\n")) {
      for (int i = 0; i < nOfContacts; i++) {
         readNextLine();
         nValues = sscanf(line, "%d %d %lf %lf %lf %lf %lf %lf", &P1, &WALL, &CX, &CY, &CZ, &FX, &FY, &FZ);
         if (nValues == 8) {
            dem::Particle *p1 = findParticle(P1);
            dem::Particle_Geometry_contact *pgc = newObject(dem::Particle_Geometry_contact);
            dem::Contact *c = pgc;
            if (p1) {
               pgc->put_P1(p1);
               if (WALL) {
                  dem::FEM_mesh *mesh = meshes[WALL];
                  if (mesh) {
                     pgc->put_geometry(mesh); ts->put_has_contact_element(pgc);
                  } else {
                     printf("Illegal mesh ID \"%ld\" in file \"%s\"\n", WALL, injectorFileName);
                  }
               }
            } else {
               printf("Illegal particle number in particle contact file \"%s\"\n", injectorFileName);
            }
         } else {
            printf("Illegal number of columns in particle contact file \"%s\"\n", injectorFileName);
         }
      }
   } else if (strEQL(line, " P1 FX FY FZ NX NY NZ\n")) {
      for (int i = 0; i < nOfContacts; i++) {
         readNextLine();
         nValues = sscanf(line, "%d %lf %lf %lf %lf %lf %lf", &P1, &FX, &FY, &FZ, &NX, &NY, &NZ);
         if (nValues == 7) {
            dem::Particle *p1 = findParticle(P1);
            dem::Particle_Geometry_contact *pgc = newObject(dem::Particle_Geometry_contact);
            if (p1) {
               pgc->put_P1(p1);
               dem::Contact *c = pgc;
               c->put_id(cContactID++);
               dem::Contact_result *cr = newObject(dem::Contact_result);
               dem::Contact_Force  *cf = newObject(dem::Contact_Force);
               cf->put_Fx_Fy_Fz_element(FX); cf->put_Fx_Fy_Fz_element(FY); cf->put_Fx_Fy_Fz_element(FZ);
               cr->put_contact_results_properties_element(cf);
               cr->put_calculated_for(ts);
               cr->put_valid_for(c);
               ts->put_has_contact_element(c);
            } else {
               printf("Illegal particle number in particle contact file \"%s\"\n", injectorFileName);
            }
         } else {
            printf("Illegal number of columns in particle contact file \"%s\"\n", injectorFileName);
         }
      }
   } else {
      printf("Illegal column names \"%s\" in file \"%s\"\n", line, injectorFileName);
   }
}

dem::Particle *DEM_InjectorHandler::findParticle(int particleID)
{
   dem::Particle *p = particles[particleID];
   if (p) {
      return p;
   } else {
      printf("Error - Particle not defined.\n");
      return NULL;
   }
}

void DEM_InjectorHandler::defineParticle()
{
   p = particles[ID];
   if (p) {
      tnn = static_cast<Template_nn *>(p);
   } else {
      tnn = newObject(dem::Template_nn); p = tnn;  particles[ID] = p; p->put_id(ID);
   }
   if (newTimestep) ts->put_consists_of_element(p);
   p->put_group(GROUP);
}

void DEM_InjectorHandler::addCoordinates()
{
   if (!p->exists_coordinates()) {
      p->put_coordinates_element(1, PX); p->put_coordinates_element(2, PY); p->put_coordinates_element(3, PZ);
   }
}

void DEM_InjectorHandler::addParticleResult()
{
   pr = newObject(dem::Particle_result);
   pr->put_valid_for(p); pr->put_calculated_for(ts);
}

void DEM_InjectorHandler::addVelocityMassVolume()
{
   dem::Velocity *v = newObject(dem::Velocity);
   v->put_Vx_Vy_Vz_element(1, VX); v->put_Vx_Vy_Vz_element(2, VY); v->put_Vx_Vy_Vz_element(3, VZ);
   pr->put_result_properties_element(v);
   dem::Mass *mass = newObject(dem::Mass); mass->put_mass(MASS);
   pr->put_result_properties_element(mass);
   dem::Volume *vol = newObject(dem::Volume); vol->put_volume(VOLUME);
   pr->put_result_properties_element(vol);
}

void DEM_InjectorHandler::store_TIMESTEP_PARTICLES()
{
   int nOfParticles;

   newTimestep = false;
   readNextLine();
   sscanf(line, "%lf %d", &timeStep, &nOfParticles);
   ts = timesteps[timeStep];
   if (!ts) {
      ts = newObject(dem::Timestep); ts->put_time_value(timeStep); timesteps[timeStep] = ts;
      theSimulation->put_consists_of_element(ts); newTimestep = true;
   }

   readNextLine();
   if (strEQL(line, " ID GROUP VOLUME MASS PX PY PZ VX VY VZ Orientation_XX Orientation_XY Orientation_XZ Orientation_YX Orientation_YY Orientation_YZ Orientation_ZX Orientation_ZY Orientation_ZZ Angular_Velocity_X Angular_Velocity_Y Angular_Velocity_Z Kinetic_Energy\n")) {
      for (int i = 0; i < nOfParticles; i++) {
         readNextLine();
         int nValues = sscanf(line, "%d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &ID, &GROUP, &VOLUME, &MASS, &PX, &PY, &PZ, &VX, &VY, &VZ, &Orientation_XX, &Orientation_XY, &Orientation_XZ, &Orientation_YX, &Orientation_YY, &Orientation_YZ, &Orientation_ZX, &Orientation_ZY, &Orientation_ZZ, &Angular_Velocity_X, &Angular_Velocity_Y, &Angular_Velocity_Z, &Kinetic_Energy);

         defineParticle();
         addCoordinates();
         addParticleResult();
         addVelocityMassVolume();

         pr->put_result_properties_element(newCustomPropertyVector("Orientation_X", Orientation_XX, Orientation_XY, Orientation_XZ));
         pr->put_result_properties_element(newCustomPropertyVector("Orientation_Y", Orientation_XX, Orientation_XY, Orientation_XZ));
         pr->put_result_properties_element(newCustomPropertyVector("Orientation_Z", Orientation_XX, Orientation_XY, Orientation_XZ));
         pr->put_result_properties_element(newCustomPropertyVector("Angular_Velocity", Angular_Velocity_X, Angular_Velocity_Y, Angular_Velocity_Z));
         pr->put_result_properties_element(newCustomPropertyScalar("Kinetic_Energy", Kinetic_Energy));
      }
   } else if (strEQL(line, "ID GROUP TYPE VOLUME MASS PX PY PZ VX VY VZ\n")) {
      for (int i = 0; i < nOfParticles; i++) {
         readNextLine();
         int nValues = sscanf(line, "%d %d %d %lf %lf %lf %lf %lf %lf %lf %lf",
            &ID, &GROUP, &TYPE, &VOLUME, &MASS, &PX, &PY, &PZ, &VX, &VY, &VZ);
         if (nValues == 11) {
            defineParticle();
            addCoordinates();
            addParticleResult();
            addVelocityMassVolume();
         } else {
            printf("Illegal number of columns in particle contact file \"%s\"\n", injectorFileName);
         }
      }
   } else if (strEQL(line, " ID GROUP VOLUME MASS PX PY PZ VX VY VZ Angular_Velocity_X Angular_Velocity_Y Angular_Velocity_Z Kinetic_Energy\n")) {
      for (int i = 0; i < nOfParticles; i++) {
         readNextLine();
         int nValues = sscanf(line, "%d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &ID, &GROUP, &VOLUME, &MASS, &PX, &PY, &PZ, &VX, &VY, &VZ, &Angular_Velocity_X, &Angular_Velocity_Y, &Angular_Velocity_Z, &Kinetic_Energy);
         if (nValues == 14) {
            defineParticle();
            addCoordinates();
            addParticleResult();
            addVelocityMassVolume();
            pr->put_result_properties_element(newCustomPropertyVector("Angular_Velocity", Angular_Velocity_X, Angular_Velocity_Y, Angular_Velocity_Z));
            pr->put_result_properties_element(newCustomPropertyScalar("Kinetic_Energy", Kinetic_Energy));
         } else {
            printf("Illegal number of columns in particle contact file \"%s\"\n", injectorFileName);
         }
      }
   } else {
      printf("Illegal column names \"%s\" in file \"%s\"\n", line, injectorFileName);
   }
}


char *DEM_InjectorHandler::readNextLine()
{
   return fgets(line, sizeof(line), fp);
}

void DEM_InjectorHandler::flushObjectsAndClose()
{
   //m->writeAllObjectsToDatabase();
   m->close();
}

dem::Custom_property_vector *DEM_InjectorHandler::newCustomPropertyVector(char *name, double x, double y, double z)
{
   dem::Custom_property_vector *cpv = newObject(dem::Custom_property_vector);
   cpv->put_name(name);
   cpv->put_CPx_CPy_CPz_element(1, x); cpv->put_CPx_CPy_CPz_element(2, y); cpv->put_CPx_CPy_CPz_element(3, z);
   return cpv;
}

dem::Custom_property_scalar *DEM_InjectorHandler::newCustomPropertyScalar(char *name, double x)
{
   dem::Custom_property_scalar *cps = newObject(dem::Custom_property_scalar);
   cps->put_name(name); cps->put_custom_prop(x);
   return cps;
}
