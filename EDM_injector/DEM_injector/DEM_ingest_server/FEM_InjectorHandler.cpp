// DEM_ingest_server.cpp : Defines the entry point for the console application.

#include "stdafx.h"

/*===================================================================================================================================================

GiD ascii format for mesh and results files is explained at
http ://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files

In Ascii format there are two files :
one for the mesh(es) http ://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files/mesh_format-_modelnamepostmsh
and one for the results http ://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files/results_format-_modelnamepostres

The VELaSSCo_HbaseBasicTest is an artificial tetrahedral mesh of a unit cube, between(0, 0, 0) and(1, 1, 1) with some artificial created results.
This artifical dataset is subdivided into4 partitions :

VELaSSCo_HbaseBasicTest - part - 1.post.msh and VELaSSCo_HbaseBasicTest - part - 1.post.res
VELaSSCo_HbaseBasicTest - part - 2.post.msh and VELaSSCo_HbaseBasicTest - part - 2.post.res
VELaSSCo_HbaseBasicTest - part - 3.post.msh and VELaSSCo_HbaseBasicTest - part - 3.post.res
VELaSSCo_HbaseBasicTest - part - 4.post.msh and VELaSSCo_HbaseBasicTest - part - 4.post.res

The mesh is a single volume of around 10, 000 tetrahedrons and 10 time - steps with 2 vector results.

Basically, to read the VELaSSCo_HbaseBasicTest - part - X.post files :

Meshes
== == ==
Meshes start with following header :

MESH "Mesh name" dimension 3 ElemType Tetrahedra Nnode 4

dimension--> 2 or 3, dimension of the mesh : 2d or 3d
ElemType--> one of Point, Line, Triangle, Quadrilateral, Tetrahedra, Hexahedra, Prism, Pyramid, Sphere, Circle
Nnode--> number of nodes of the elements, for instance 4 or 10 for Tetrahedra, or 3 or 6 for Triangle
(check http ://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files/mesh_format-_modelnamepostmsh)
followed by the coordinates / end coordinates section :
node_id coordinates_x coordinates_y coordinates_z_optional
node_id is a global id of the node
coordinates can be specified in the first coordinates block for all meshes or each mesh defines their own
followed by the elements / end elements section :
element_id node_1 ... node_Nnode group_id_optional
element_id is a global id of the element
node_1 ... node_Nnode depends on the element connectivity
for order of nodes check http ://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files/mesh_format-_modelnamepostmsh

any comment line begins with the character '#'.
Optionally, between the the MESH header and the coordinates section, the color of the mesh can be specified with :
# Color red green blue
color components are between 0.0 and 1.0

the first line with
# encoding utf - 8
is needed when using non - ascii character for mesh names, like accents or other special symbols.

Results
== == == =
Results file start with this header in the first line :
GiD Post Results File 1.0

# encoding utf - 8
as before, it is needed when using non - ascii characters in strings

Results definitions start with following header :

Result "Result name" "Analysis name" StepValue ResultType Location GP_location

StepValue--> floating point number which defined the step value inside the analysis
ResultType--> type of result, one of : SCalar, Vector, Matrix, PlainDeformationMatrix, MainMatrix, LocalAxes, ComplexScalar, ComplexVector
(check http ://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files/results_format-_modelnamepostres/result)
Location--> OnNodes or OnGaussPoints tells GiD whether the results are defined on the nodes of the mesh or on gauss points.
GP_location--> needed if Location == OnGaussPoints defines which gauss points are used to define location of results, can be
one of the GiD standard gauss points of gid : GP_TRIANGLE_1, GP_TRIANGLE_3, GP_TETRAHEDRA_1, GP_TETRAHEDRA_4, etc.
or the name of the gauss points defined by the user using the "GaussPoint header"
(for further details check http ://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files/results_format-_modelnamepostres/gauss_points)

followed with some optional parameters,
check http ://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files/results_format-_modelnamepostres/result
followed by the Values / end values section :
node_id value_1 ... value_N--> if Result is located OnNodes, value_N depending on the result type
element_id value_gp_1_1 ... value_gp_1_N--> value_gp_1_N with N depending on the result type
...
value_gp_M_1 ... value_gp_M_N--> value_gp_M_N with M depending on the Gauss Point definition
element_id + 1 value_gp_1_...
...
value_gp_M_...

===================================================================================================================================================*/


using namespace  ::DEM_Inject;

#include "FEM_InjectorHandler.h"


#define newObject(className) new(m)className(m)



void gen_random(char *s, const int len) {
   static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
   for (int i = 0; i < len; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum)-1)];
   }
   s[len] = 0;
}
void FEM_InjectorHandler::StoreDEM_Simulation(std::string& _return, const std::string& sessionID, const std::string& model_name, const DEM_Inject::Simulation& theSimulation)
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
      m->writeAllObjectsToDatabase();

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
void FEM_InjectorHandler::DeleteModelContent(std::string& _return, const std::string& sessionID, const std::string& model_name)
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

void FEM_InjectorHandler::DeleteCurrentModelContent()
{
   CHECK(edmiDeleteModelContents(m->modelId));
}

void FEM_InjectorHandler::InitiateFileInjection()
{
   theSimulation = newObject(dem::Simulation);
   theSimulation->put_name("theSimulation");
}

void FEM_InjectorHandler::InjectFile(char *file_name)
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

void FEM_InjectorHandler::InjectMesh(char *MeshFileFolder, char *MeshName, char *MeshVersion)
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
                  facet = new(ma)Set<dem::Vertex*>(ma, sdaiINSTANCE, 32);
                  mesh->put_facets_element(facet);
               }
               dem::Vertex *v = newObject(dem::Vertex);
               v->put_vertex_position_element(atof(x)); v->put_vertex_position_element(atof(y)); v->put_vertex_position_element(atof(z));
               facet->add(v, ma);
               if (vertexNo >= 3) vertexNo = 0; // build another facet next time
            }
         }
         fclose(stlFile);
      } else {
         printf("Illegal Mesh file name: %s\n", fileName);
      }
   }
}



void FEM_InjectorHandler::store_TIMESTEP_CONTACTS()
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
               dem::FEM_mesh *mesh = meshes[WALL];
               if (mesh) {
                  pgc->put_geometry(mesh); ts->put_has_contact_element(pgc);
               } else {
                  printf("Illegal mesh ID \"%ld\" in file \"%s\"\n", WALL, injectorFileName);
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

dem::Particle *FEM_InjectorHandler::findParticle(int particleID)
{
   dem::Particle *p = particles[particleID];
   if (p) {
      return p;
   } else {
      printf("Error - Particle not defined.\n");
      return NULL;
   }
}

void FEM_InjectorHandler::defineParticle()
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

void FEM_InjectorHandler::addCoordinates()
{
   if (!p->exists_coordinates()) {
      p->put_coordinates_element(1, PX); p->put_coordinates_element(2, PY); p->put_coordinates_element(3, PZ);
   }
}

void FEM_InjectorHandler::addParticleResult()
{
   pr = newObject(dem::Particle_result);
   pr->put_valid_for(p); pr->put_calculated_for(ts);
}

void FEM_InjectorHandler::addVelocityMassVolume()
{
   dem::Velocity *v = newObject(dem::Velocity);
   v->put_Vx_Vy_Vz_element(1, VX); v->put_Vx_Vy_Vz_element(2, VY); v->put_Vx_Vy_Vz_element(3, VZ);
   pr->put_result_properties_element(v);
   dem::Mass *mass = newObject(dem::Mass); mass->put_mass(MASS);
   pr->put_result_properties_element(mass);
   dem::Volume *vol = newObject(dem::Volume); vol->put_volume(VOLUME);
   pr->put_result_properties_element(vol);
}

void FEM_InjectorHandler::store_TIMESTEP_PARTICLES()
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


char *FEM_InjectorHandler::readNextLine()
{
   return fgets(line, sizeof(line), fp);
}

void FEM_InjectorHandler::flushObjectsAndClose()
{
   m->writeAllObjectsToDatabase();
   m->close();
}

dem::Custom_property_vector *FEM_InjectorHandler::newCustomPropertyVector(char *name, double x, double y, double z)
{
   dem::Custom_property_vector *cpv = newObject(dem::Custom_property_vector);
   cpv->put_name(name);
   cpv->put_CPx_CPy_CPz_element(1, x); cpv->put_CPx_CPy_CPz_element(2, y); cpv->put_CPx_CPy_CPz_element(3, z);
   return cpv;
}

dem::Custom_property_scalar *FEM_InjectorHandler::newCustomPropertyScalar(char *name, double x)
{
   dem::Custom_property_scalar *cps = newObject(dem::Custom_property_scalar);
   cps->put_name(name); cps->put_custom_prop(x);
   return cps;
}
