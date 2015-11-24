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
   char command[256], meshName[256], dim[256], et[256], elemType[256], nn[256];
   EDMULONG nNodes, dimension;

   injectorFileName = file_name;
   fp = fopen(file_name, "r");
   if (fp) {
      while (readNextLine() > 0) {
         if (line[0] == '#') {
         } else if (strncmp(line, "MESH", 4)) {
            int nColumns = sscanf(line, "%s %s %s %llu %s %s %s %llu", command, meshName, dim, &dimension, et, elemType, nn, &nNodes);
         }
      }
      fclose(fp);
   } else {
      THROW("Unaple to open DEM data file.");
   }
}
//# encoding utf - 8
//MESH "Part" dimension 3 ElemType Tetrahedra Nnode 4
//# Color 0.600000 0.600000 0.600000
//Coordinates
//37 0.27317816 0.95038164 0.96116579
//100 0.2 1 1
//101 0.30000001 1 1
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

//
//
//void FEM_InjectorHandler::store_TIMESTEP_CONTACTS()
//{
//   int               nOfContacts, nValues;
//   
//   newTimestep = false;
//   readNextLine();
//   sscanf(line, "%lf %d", &timeStep, &nOfContacts);
//   dem::Timestep *ts = timesteps[timeStep];
//   if (!ts) {
//      ts = newObject(dem::Timestep); ts->put_time_value(timeStep); timesteps[timeStep] = ts;
//      theSimulation->put_consists_of_element(ts); newTimestep = true;
//   }
//
//   readNextLine();
//   bool particle_particle_contact = strEQL(line, " P1 P2 CX CY CZ FX FY FZ\n") || strEQL(line, "P1 P2 CX CY CZ FX FY FZ\n");
//
//}
//
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



char *FEM_InjectorHandler::readNextLine()
{
   return fgets(line, sizeof(line), fp);
}

void FEM_InjectorHandler::flushObjectsAndClose()
{
   m->writeAllObjectsToDatabase();
   m->close();
}
