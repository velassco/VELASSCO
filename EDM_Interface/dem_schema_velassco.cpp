#include "stdafx.h"
#include "dem_schema_velassco_header.h"

namespace dem {

EDMLONG dbInstance_AttributeLayout[] = {0};
EDMLONG Simulation_AttributeLayout[] = {9, 4, 0};
tEdmiAttribute Simulation_Attributes[] = {
   {"consists_of", 9, 0},
   {"name", 4, 0},
   {NULL, 0, 0},
};
EDMLONG Simulation_Subtypes[] = {0};
EDMLONG Contact_result_property_AttributeLayout[] = {0};
tEdmiAttribute Contact_result_property_Attributes[] = {
   {NULL, 0, 0},
};
EDMLONG Contact_result_property_Subtypes[] = {et_Custom_property_contact_vector, et_Custom_property_contact_scalar, et_Contact_Force, 0};
EDMLONG Contact_result_AttributeLayout[] = {8, 9, 8, 0};
tEdmiAttribute Contact_result_Attributes[] = {
   {"valid_for", 8, 0},
   {"contact_results_properties", 9, 0},
   {"calculated_for", 8, 0},
   {NULL, 0, 0},
};
EDMLONG Contact_result_Subtypes[] = {0};
EDMLONG Particle_result_property_AttributeLayout[] = {0};
tEdmiAttribute Particle_result_property_Attributes[] = {
   {NULL, 0, 0},
};
EDMLONG Particle_result_property_Subtypes[] = {et_Velocity, et_Custom_property_vector, et_Custom_property_scalar, et_Mass, et_Volume, 0};
EDMLONG Particle_result_AttributeLayout[] = {8, 9, 8, 0};
tEdmiAttribute Particle_result_Attributes[] = {
   {"valid_for", 8, 0},
   {"result_properties", 9, 0},
   {"calculated_for", 8, 0},
   {NULL, 0, 0},
};
EDMLONG Particle_result_Subtypes[] = {0};
EDMLONG Particle_AttributeLayout[] = {0, 0, 9, 0};
tEdmiAttribute Particle_Attributes[] = {
   {"id", 0, 0},
   {"group", 0, 0},
   {"coordinates", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Particle_Subtypes[] = {et_Sphere, et_Template_nn, 0};
EDMLONG Contact_AttributeLayout[] = {9, 0, 0};
tEdmiAttribute Contact_Attributes[] = {
   {"contact_location", 9, 0},
   {"id", 0, 0},
   {NULL, 0, 0},
};
EDMLONG Contact_Subtypes[] = {et_Particle_Particle_contact, et_Particle_Geometry_contact, 0};
EDMLONG Vertex_AttributeLayout[] = {9, 0};
tEdmiAttribute Vertex_Attributes[] = {
   {"vertex_position", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Vertex_Subtypes[] = {0};
EDMLONG FEM_mesh_AttributeLayout[] = {4, 9, 0};
tEdmiAttribute FEM_mesh_Attributes[] = {
   {"name", 4, 0},
   {"facets", 9, 0},
   {NULL, 0, 0},
};
EDMLONG FEM_mesh_Subtypes[] = {0};
EDMLONG Timestep_AttributeLayout[] = {1, 9, 9, 9, 0};
tEdmiAttribute Timestep_Attributes[] = {
   {"time_value", 1, 0},
   {"boundary", 9, 0},
   {"has_contact", 9, 0},
   {"consists_of", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Timestep_Subtypes[] = {0};
EDMLONG Particle_Particle_contact_AttributeLayout[] = {9, 0, 8, 8, 0};
tEdmiAttribute Particle_Particle_contact_Attributes[] = {
   {"contact_location", 9, 0},
   {"id", 0, 0},
   {"P1", 8, 0},
   {"P2", 8, 0},
   {NULL, 0, 0},
};
EDMLONG Particle_Particle_contact_Subtypes[] = {0};
EDMLONG Sphere_AttributeLayout[] = {0, 0, 9, 1, 0};
tEdmiAttribute Sphere_Attributes[] = {
   {"id", 0, 0},
   {"group", 0, 0},
   {"coordinates", 9, 0},
   {"radius", 1, 0},
   {NULL, 0, 0},
};
EDMLONG Sphere_Subtypes[] = {0};
EDMLONG Template_nn_AttributeLayout[] = {0, 0, 9, 0};
tEdmiAttribute Template_nn_Attributes[] = {
   {"id", 0, 0},
   {"group", 0, 0},
   {"coordinates", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Template_nn_Subtypes[] = {0};
EDMLONG Particle_Geometry_contact_AttributeLayout[] = {9, 0, 8, 8, 0};
tEdmiAttribute Particle_Geometry_contact_Attributes[] = {
   {"contact_location", 9, 0},
   {"id", 0, 0},
   {"P1", 8, 0},
   {"geometry", 8, 0},
   {NULL, 0, 0},
};
EDMLONG Particle_Geometry_contact_Subtypes[] = {0};
EDMLONG Velocity_AttributeLayout[] = {9, 0};
tEdmiAttribute Velocity_Attributes[] = {
   {"Vx_Vy_Vz", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Velocity_Subtypes[] = {0};
EDMLONG Custom_property_vector_AttributeLayout[] = {9, 4, 0};
tEdmiAttribute Custom_property_vector_Attributes[] = {
   {"CPx_CPy_CPz", 9, 0},
   {"name", 4, 0},
   {NULL, 0, 0},
};
EDMLONG Custom_property_vector_Subtypes[] = {0};
EDMLONG Custom_property_scalar_AttributeLayout[] = {1, 4, 0};
tEdmiAttribute Custom_property_scalar_Attributes[] = {
   {"custom_prop", 1, 0},
   {"name", 4, 0},
   {NULL, 0, 0},
};
EDMLONG Custom_property_scalar_Subtypes[] = {0};
EDMLONG Mass_AttributeLayout[] = {1, 0};
tEdmiAttribute Mass_Attributes[] = {
   {"mass", 1, 0},
   {NULL, 0, 0},
};
EDMLONG Mass_Subtypes[] = {0};
EDMLONG Volume_AttributeLayout[] = {1, 0};
tEdmiAttribute Volume_Attributes[] = {
   {"volume", 1, 0},
   {NULL, 0, 0},
};
EDMLONG Volume_Subtypes[] = {0};
EDMLONG Custom_property_contact_vector_AttributeLayout[] = {9, 4, 0};
tEdmiAttribute Custom_property_contact_vector_Attributes[] = {
   {"CPx_CPy_CPz", 9, 0},
   {"name", 4, 0},
   {NULL, 0, 0},
};
EDMLONG Custom_property_contact_vector_Subtypes[] = {0};
EDMLONG Custom_property_contact_scalar_AttributeLayout[] = {1, 4, 0};
tEdmiAttribute Custom_property_contact_scalar_Attributes[] = {
   {"property_value", 1, 0},
   {"name", 4, 0},
   {NULL, 0, 0},
};
EDMLONG Custom_property_contact_scalar_Subtypes[] = {0};
EDMLONG Contact_Force_AttributeLayout[] = {9, 0};
tEdmiAttribute Contact_Force_Attributes[] = {
   {"Fx_Fy_Fz", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Contact_Force_Subtypes[] = {0};
const char *dem_schema_velassco_DefinedTypeNames[] = {
NULL
};
tEdmiEntityData dem_schema_velassco_Entities[] = {
{"indeterminate"},
{"Simulation", 2, 673, 8, 32, 21, et_Simulation, Simulation_AttributeLayout, Simulation_Subtypes, NULL, Simulation_Attributes},
{"Contact_result_property", 0, 665, 0, 0, 17, et_Contact_result_property, Contact_result_property_AttributeLayout, Contact_result_property_Subtypes, NULL, Contact_result_property_Attributes},
{"Contact_result", 3, 663, 8, 40, 16, et_Contact_result, Contact_result_AttributeLayout, Contact_result_Subtypes, NULL, Contact_result_Attributes},
{"Particle_result_property", 0, 651, 0, 0, 10, et_Particle_result_property, Particle_result_property_AttributeLayout, Particle_result_property_Subtypes, NULL, Particle_result_property_Attributes},
{"Particle_result", 3, 649, 8, 40, 9, et_Particle_result, Particle_result_AttributeLayout, Particle_result_Subtypes, NULL, Particle_result_Attributes},
{"Particle", 3, 641, 8, 40, 5, et_Particle, Particle_AttributeLayout, Particle_Subtypes, NULL, Particle_Attributes},
{"Contact", 2, 637, 8, 32, 3, et_Contact, Contact_AttributeLayout, Contact_Subtypes, NULL, Contact_Attributes},
{"Vertex", 1, 635, 8, 24, 2, et_Vertex, Vertex_AttributeLayout, Vertex_Subtypes, NULL, Vertex_Attributes},
{"FEM_mesh", 2, 633, 8, 32, 1, et_FEM_mesh, FEM_mesh_AttributeLayout, FEM_mesh_Subtypes, NULL, FEM_mesh_Attributes},
{"Timestep", 4, 631, 8, 48, 0, et_Timestep, Timestep_AttributeLayout, Timestep_Subtypes, NULL, Timestep_Attributes},
{"Particle_Particle_contact", 4, 639, 8, 48, 4, et_Particle_Particle_contact, Particle_Particle_contact_AttributeLayout, Particle_Particle_contact_Subtypes, NULL, Particle_Particle_contact_Attributes},
{"Sphere", 4, 643, 8, 48, 6, et_Sphere, Sphere_AttributeLayout, Sphere_Subtypes, NULL, Sphere_Attributes},
{"Template_nn", 3, 645, 8, 40, 7, et_Template_nn, Template_nn_AttributeLayout, Template_nn_Subtypes, NULL, Template_nn_Attributes},
{"Particle_Geometry_contact", 4, 647, 8, 48, 8, et_Particle_Geometry_contact, Particle_Geometry_contact_AttributeLayout, Particle_Geometry_contact_Subtypes, NULL, Particle_Geometry_contact_Attributes},
{"Velocity", 1, 653, 8, 24, 11, et_Velocity, Velocity_AttributeLayout, Velocity_Subtypes, NULL, Velocity_Attributes},
{"Custom_property_vector", 2, 655, 8, 32, 12, et_Custom_property_vector, Custom_property_vector_AttributeLayout, Custom_property_vector_Subtypes, NULL, Custom_property_vector_Attributes},
{"Custom_property_scalar", 2, 657, 8, 32, 13, et_Custom_property_scalar, Custom_property_scalar_AttributeLayout, Custom_property_scalar_Subtypes, NULL, Custom_property_scalar_Attributes},
{"Mass", 1, 659, 8, 24, 14, et_Mass, Mass_AttributeLayout, Mass_Subtypes, NULL, Mass_Attributes},
{"Volume", 1, 661, 8, 24, 15, et_Volume, Volume_AttributeLayout, Volume_Subtypes, NULL, Volume_Attributes},
{"Custom_property_contact_vector", 2, 667, 8, 32, 18, et_Custom_property_contact_vector, Custom_property_contact_vector_AttributeLayout, Custom_property_contact_vector_Subtypes, NULL, Custom_property_contact_vector_Attributes},
{"Custom_property_contact_scalar", 2, 669, 8, 32, 19, et_Custom_property_contact_scalar, Custom_property_contact_scalar_AttributeLayout, Custom_property_contact_scalar_Subtypes, NULL, Custom_property_contact_scalar_Attributes},
{"Contact_Force", 1, 671, 8, 24, 20, et_Contact_Force, Contact_Force_AttributeLayout, Contact_Force_Subtypes, NULL, Contact_Force_Attributes},
{NULL},
};

/*====================================================================================================
   Simulation
====================================================================================================*/
List<Timestep*>* Simulation::get_consists_of() { return getAGGREGATE(0, List<Timestep*>*, 0); }
void Simulation::put_consists_of(List<Timestep*>* v) { putAGGREGATE(0, List<Timestep*>*, v, consists_of, 0, 9); }
void Simulation::put_consists_of_element(Timestep* element) {
   List<Timestep*>* aggregate = get_consists_of();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<Timestep*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_consists_of(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
char * Simulation::get_name() { return getATTRIBUTE(8, char *, 1); }
void Simulation::put_name(char * v) { putATTRIBUTE(8, char *, v, name, 1, 4); }
/*====================================================================================================
   Contact_result_property
====================================================================================================*/
/*====================================================================================================
   Contact_result
====================================================================================================*/
Contact* Contact_result::get_valid_for() { return getSupertypeInstance(0, Contact*, 0, et_Contact); }
void * Contact_result::get_valid_for(entityType *etp) { return getInstanceAndType(0, Contact*, 0, (int*)etp); }
void Contact_result::put_valid_for(Contact* v) { putInstance(0, Contact*, v, valid_for, 0, 8); v->addToUsedIn(c); }
List<Contact_result_property*>* Contact_result::get_contact_results_properties() { return getAGGREGATE(8, List<Contact_result_property*>*, 1); }
void Contact_result::put_contact_results_properties(List<Contact_result_property*>* v) { putAGGREGATE(8, List<Contact_result_property*>*, v, contact_results_properties, 1, 9); }
void Contact_result::put_contact_results_properties_element(Contact_result_property* element) {
   List<Contact_result_property*>* aggregate = get_contact_results_properties();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<Contact_result_property*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_contact_results_properties(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
Timestep* Contact_result::get_calculated_for() { return getInstance(16, Timestep*, 2); }
void Contact_result::put_calculated_for(Timestep* v) { putInstance(16, Timestep*, v, calculated_for, 2, 8); v->addToUsedIn(c); }
/*====================================================================================================
   Particle_result_property
====================================================================================================*/
/*====================================================================================================
   Particle_result
====================================================================================================*/
Particle* Particle_result::get_valid_for() { return getSupertypeInstance(0, Particle*, 0, et_Particle); }
void * Particle_result::get_valid_for(entityType *etp) { return getInstanceAndType(0, Particle*, 0, (int*)etp); }
void Particle_result::put_valid_for(Particle* v) { putInstance(0, Particle*, v, valid_for, 0, 8); v->addToUsedIn(c); }
List<Particle_result_property*>* Particle_result::get_result_properties() { return getAGGREGATE(8, List<Particle_result_property*>*, 1); }
void Particle_result::put_result_properties(List<Particle_result_property*>* v) { putAGGREGATE(8, List<Particle_result_property*>*, v, result_properties, 1, 9); }
void Particle_result::put_result_properties_element(Particle_result_property* element) {
   List<Particle_result_property*>* aggregate = get_result_properties();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<Particle_result_property*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_result_properties(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
Timestep* Particle_result::get_calculated_for() { return getInstance(16, Timestep*, 2); }
void Particle_result::put_calculated_for(Timestep* v) { putInstance(16, Timestep*, v, calculated_for, 2, 8); v->addToUsedIn(c); }
/*====================================================================================================
   Particle
====================================================================================================*/
int Particle::get_id() { return getATTRIBUTE(0, int, 0); }
void Particle::put_id(int v) { putATTRIBUTE(0, int, v, id, 0, 0); }
int Particle::get_group() { return getATTRIBUTE(8, int, 1); }
void Particle::put_group(int v) { putATTRIBUTE(8, int, v, group, 1, 0); }
Array<REAL>* Particle::get_coordinates() { return getAGGREGATE(16, Array<REAL>*, 2); }
void Particle::put_coordinates(Array<REAL>* v) { putAGGREGATE(16, Array<REAL>*, v, coordinates, 2, 9); }
void Particle::put_coordinates_element(int index, REAL element) {
   Array<REAL>* aggregate = get_coordinates();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())Array<REAL>(m->getMemoryAllocator(), sdaiREAL, 1, 3);
      put_coordinates(aggregate);
   }
   aggregate->put(index, element);
}
/*====================================================================================================
   Contact
====================================================================================================*/
Array<REAL>* Contact::get_contact_location() { return getAGGREGATE(0, Array<REAL>*, 0); }
void Contact::put_contact_location(Array<REAL>* v) { putAGGREGATE(0, Array<REAL>*, v, contact_location, 0, 9); }
void Contact::put_contact_location_element(int index, REAL element) {
   Array<REAL>* aggregate = get_contact_location();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())Array<REAL>(m->getMemoryAllocator(), sdaiREAL, 1, 3);
      put_contact_location(aggregate);
   }
   aggregate->put(index, element);
}
int Contact::get_id() { return getATTRIBUTE(8, int, 1); }
void Contact::put_id(int v) { putATTRIBUTE(8, int, v, id, 1, 0); }
/*====================================================================================================
   Vertex
====================================================================================================*/
List<REAL>* Vertex::get_vertex_position() { return getAGGREGATE(0, List<REAL>*, 0); }
void Vertex::put_vertex_position(List<REAL>* v) { putAGGREGATE(0, List<REAL>*, v, vertex_position, 0, 9); }
void Vertex::put_vertex_position_element(REAL element) {
   List<REAL>* aggregate = get_vertex_position();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<REAL>(m->getMemoryAllocator(), sdaiREAL, 3);
      put_vertex_position(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
/*====================================================================================================
   FEM_mesh
====================================================================================================*/
char * FEM_mesh::get_name() { return getATTRIBUTE(0, char *, 0); }
void FEM_mesh::put_name(char * v) { putATTRIBUTE(0, char *, v, name, 0, 4); }
Set<Set<Vertex*>*>* FEM_mesh::get_facets() { return getAGGREGATE(8, Set<Set<Vertex*>*>*, 1); }
void FEM_mesh::put_facets(Set<Set<Vertex*>*>* v) { putAGGREGATE(8, Set<Set<Vertex*>*>*, v, facets, 1, 9); }
void FEM_mesh::put_facets_element(Set<Vertex*>* element) {
   Set<Set<Vertex*>*>* aggregate = get_facets();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())Set<Set<Vertex*>*>(m->getMemoryAllocator(), sdaiAGGR, 3);
      put_facets(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
/*====================================================================================================
   Timestep
====================================================================================================*/
double Timestep::get_time_value() { return getREAL(0, double, 0); }
void Timestep::put_time_value(double v) { putATTRIBUTE(0, double, v, time_value, 0, 1); }
Set<FEM_mesh*>* Timestep::get_boundary() { return getAGGREGATE(8, Set<FEM_mesh*>*, 1); }
void Timestep::put_boundary(Set<FEM_mesh*>* v) { putAGGREGATE(8, Set<FEM_mesh*>*, v, boundary, 1, 9); }
void Timestep::put_boundary_element(FEM_mesh* element) {
   Set<FEM_mesh*>* aggregate = get_boundary();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())Set<FEM_mesh*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_boundary(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
List<Contact*>* Timestep::get_has_contact() { return getAGGREGATE(16, List<Contact*>*, 2); }
void Timestep::put_has_contact(List<Contact*>* v) { putAGGREGATE(16, List<Contact*>*, v, has_contact, 2, 9); }
void Timestep::put_has_contact_element(Contact* element) {
   List<Contact*>* aggregate = get_has_contact();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<Contact*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_has_contact(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
List<Particle*>* Timestep::get_consists_of() { return getAGGREGATE(24, List<Particle*>*, 3); }
void Timestep::put_consists_of(List<Particle*>* v) { putAGGREGATE(24, List<Particle*>*, v, consists_of, 3, 9); }
void Timestep::put_consists_of_element(Particle* element) {
   List<Particle*>* aggregate = get_consists_of();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<Particle*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_consists_of(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
/*====================================================================================================
   Particle_Particle_contact
====================================================================================================*/
Particle* Particle_Particle_contact::get_P1() { return getSupertypeInstance(16, Particle*, 2, et_Particle); }
void * Particle_Particle_contact::get_P1(entityType *etp) { return getInstanceAndType(16, Particle*, 2, (int*)etp); }
void Particle_Particle_contact::put_P1(Particle* v) { putInstance(16, Particle*, v, P1, 2, 8); v->addToUsedIn(c); }
Particle* Particle_Particle_contact::get_P2() { return getSupertypeInstance(24, Particle*, 3, et_Particle); }
void * Particle_Particle_contact::get_P2(entityType *etp) { return getInstanceAndType(24, Particle*, 3, (int*)etp); }
void Particle_Particle_contact::put_P2(Particle* v) { putInstance(24, Particle*, v, P2, 3, 8); v->addToUsedIn(c); }
/*====================================================================================================
   Sphere
====================================================================================================*/
double Sphere::get_radius() { return getREAL(24, double, 3); }
void Sphere::put_radius(double v) { putATTRIBUTE(24, double, v, radius, 3, 1); }
/*====================================================================================================
   Template_nn
====================================================================================================*/
/*====================================================================================================
   Particle_Geometry_contact
====================================================================================================*/
Particle* Particle_Geometry_contact::get_P1() { return getSupertypeInstance(16, Particle*, 2, et_Particle); }
void * Particle_Geometry_contact::get_P1(entityType *etp) { return getInstanceAndType(16, Particle*, 2, (int*)etp); }
void Particle_Geometry_contact::put_P1(Particle* v) { putInstance(16, Particle*, v, P1, 2, 8); v->addToUsedIn(c); }
FEM_mesh* Particle_Geometry_contact::get_geometry() { return getInstance(24, FEM_mesh*, 3); }
void Particle_Geometry_contact::put_geometry(FEM_mesh* v) { putInstance(24, FEM_mesh*, v, geometry, 3, 8); v->addToUsedIn(c); }
/*====================================================================================================
   Velocity
====================================================================================================*/
Array<REAL>* Velocity::get_Vx_Vy_Vz() { return getAGGREGATE(0, Array<REAL>*, 0); }
void Velocity::put_Vx_Vy_Vz(Array<REAL>* v) { putAGGREGATE(0, Array<REAL>*, v, Vx_Vy_Vz, 0, 9); }
void Velocity::put_Vx_Vy_Vz_element(int index, REAL element) {
   Array<REAL>* aggregate = get_Vx_Vy_Vz();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())Array<REAL>(m->getMemoryAllocator(), sdaiREAL, 1, 3);
      put_Vx_Vy_Vz(aggregate);
   }
   aggregate->put(index, element);
}
/*====================================================================================================
   Custom_property_vector
====================================================================================================*/
Array<REAL>* Custom_property_vector::get_CPx_CPy_CPz() { return getAGGREGATE(0, Array<REAL>*, 0); }
void Custom_property_vector::put_CPx_CPy_CPz(Array<REAL>* v) { putAGGREGATE(0, Array<REAL>*, v, CPx_CPy_CPz, 0, 9); }
void Custom_property_vector::put_CPx_CPy_CPz_element(int index, REAL element) {
   Array<REAL>* aggregate = get_CPx_CPy_CPz();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())Array<REAL>(m->getMemoryAllocator(), sdaiREAL, 1, 3);
      put_CPx_CPy_CPz(aggregate);
   }
   aggregate->put(index, element);
}
char * Custom_property_vector::get_name() { return getATTRIBUTE(8, char *, 1); }
void Custom_property_vector::put_name(char * v) { putATTRIBUTE(8, char *, v, name, 1, 4); }
/*====================================================================================================
   Custom_property_scalar
====================================================================================================*/
double Custom_property_scalar::get_custom_prop() { return getREAL(0, double, 0); }
void Custom_property_scalar::put_custom_prop(double v) { putATTRIBUTE(0, double, v, custom_prop, 0, 1); }
char * Custom_property_scalar::get_name() { return getATTRIBUTE(8, char *, 1); }
void Custom_property_scalar::put_name(char * v) { putATTRIBUTE(8, char *, v, name, 1, 4); }
/*====================================================================================================
   Mass
====================================================================================================*/
double Mass::get_mass() { return getREAL(0, double, 0); }
void Mass::put_mass(double v) { putATTRIBUTE(0, double, v, mass, 0, 1); }
/*====================================================================================================
   Volume
====================================================================================================*/
double Volume::get_volume() { return getREAL(0, double, 0); }
void Volume::put_volume(double v) { putATTRIBUTE(0, double, v, volume, 0, 1); }
/*====================================================================================================
   Custom_property_contact_vector
====================================================================================================*/
Array<REAL>* Custom_property_contact_vector::get_CPx_CPy_CPz() { return getAGGREGATE(0, Array<REAL>*, 0); }
void Custom_property_contact_vector::put_CPx_CPy_CPz(Array<REAL>* v) { putAGGREGATE(0, Array<REAL>*, v, CPx_CPy_CPz, 0, 9); }
void Custom_property_contact_vector::put_CPx_CPy_CPz_element(int index, REAL element) {
   Array<REAL>* aggregate = get_CPx_CPy_CPz();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())Array<REAL>(m->getMemoryAllocator(), sdaiREAL, 1, 3);
      put_CPx_CPy_CPz(aggregate);
   }
   aggregate->put(index, element);
}
char * Custom_property_contact_vector::get_name() { return getATTRIBUTE(8, char *, 1); }
void Custom_property_contact_vector::put_name(char * v) { putATTRIBUTE(8, char *, v, name, 1, 4); }
/*====================================================================================================
   Custom_property_contact_scalar
====================================================================================================*/
double Custom_property_contact_scalar::get_property_value() { return getREAL(0, double, 0); }
void Custom_property_contact_scalar::put_property_value(double v) { putATTRIBUTE(0, double, v, property_value, 0, 1); }
char * Custom_property_contact_scalar::get_name() { return getATTRIBUTE(8, char *, 1); }
void Custom_property_contact_scalar::put_name(char * v) { putATTRIBUTE(8, char *, v, name, 1, 4); }
/*====================================================================================================
   Contact_Force
====================================================================================================*/
List<REAL>* Contact_Force::get_Fx_Fy_Fz() { return getAGGREGATE(0, List<REAL>*, 0); }
void Contact_Force::put_Fx_Fy_Fz(List<REAL>* v) { putAGGREGATE(0, List<REAL>*, v, Fx_Fy_Fz, 0, 9); }
void Contact_Force::put_Fx_Fy_Fz_element(REAL element) {
   List<REAL>* aggregate = get_Fx_Fy_Fz();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<REAL>(m->getMemoryAllocator(), sdaiREAL, 3);
      put_Fx_Fy_Fz(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}

void* dem_Schema::generateObject(tEdmiInstData *instData, int *entityTypep, Model *ma)
{
   void* theObject = NULL;

   Simulation* p_Simulation;
   Contact_result_property* p_Contact_result_property;
   Contact_result* p_Contact_result;
   Particle_result_property* p_Particle_result_property;
   Particle_result* p_Particle_result;
   Particle* p_Particle;
   Contact* p_Contact;
   Vertex* p_Vertex;
   FEM_mesh* p_FEM_mesh;
   Timestep* p_Timestep;
   Particle_Particle_contact* p_Particle_Particle_contact;
   Sphere* p_Sphere;
   Template_nn* p_Template_nn;
   Particle_Geometry_contact* p_Particle_Geometry_contact;
   Velocity* p_Velocity;
   Custom_property_vector* p_Custom_property_vector;
   Custom_property_scalar* p_Custom_property_scalar;
   Mass* p_Mass;
   Volume* p_Volume;
   Custom_property_contact_vector* p_Custom_property_contact_vector;
   Custom_property_contact_scalar* p_Custom_property_contact_scalar;
   Contact_Force* p_Contact_Force;
   EDMLONG entityType = getEntityType((EDMLONG)instData->entityData);
   *entityTypep = entityType;
   instData->entityData = &theEntities[entityType];
   switch(entityType) {
         case et_Simulation:
            p_Simulation = new(ma) Simulation(ma, instData);
            theObject = (void*)p_Simulation;
            break;
         case et_Contact_result_property:
            p_Contact_result_property = new(ma) Contact_result_property(ma, instData);
            theObject = (void*)p_Contact_result_property;
            break;
         case et_Contact_result:
            p_Contact_result = new(ma) Contact_result(ma, instData);
            theObject = (void*)p_Contact_result;
            break;
         case et_Particle_result_property:
            p_Particle_result_property = new(ma) Particle_result_property(ma, instData);
            theObject = (void*)p_Particle_result_property;
            break;
         case et_Particle_result:
            p_Particle_result = new(ma) Particle_result(ma, instData);
            theObject = (void*)p_Particle_result;
            break;
         case et_Particle:
            p_Particle = new(ma) Particle(ma, instData);
            theObject = (void*)p_Particle;
            break;
         case et_Contact:
            p_Contact = new(ma) Contact(ma, instData);
            theObject = (void*)p_Contact;
            break;
         case et_Vertex:
            p_Vertex = new(ma) Vertex(ma, instData);
            theObject = (void*)p_Vertex;
            break;
         case et_FEM_mesh:
            p_FEM_mesh = new(ma) FEM_mesh(ma, instData);
            theObject = (void*)p_FEM_mesh;
            break;
         case et_Timestep:
            p_Timestep = new(ma) Timestep(ma, instData);
            theObject = (void*)p_Timestep;
            break;
         case et_Particle_Particle_contact:
            p_Particle_Particle_contact = new(ma) Particle_Particle_contact(ma, instData);
            theObject = (void*)p_Particle_Particle_contact;
            break;
         case et_Sphere:
            p_Sphere = new(ma) Sphere(ma, instData);
            theObject = (void*)p_Sphere;
            break;
         case et_Template_nn:
            p_Template_nn = new(ma) Template_nn(ma, instData);
            theObject = (void*)p_Template_nn;
            break;
         case et_Particle_Geometry_contact:
            p_Particle_Geometry_contact = new(ma) Particle_Geometry_contact(ma, instData);
            theObject = (void*)p_Particle_Geometry_contact;
            break;
         case et_Velocity:
            p_Velocity = new(ma) Velocity(ma, instData);
            theObject = (void*)p_Velocity;
            break;
         case et_Custom_property_vector:
            p_Custom_property_vector = new(ma) Custom_property_vector(ma, instData);
            theObject = (void*)p_Custom_property_vector;
            break;
         case et_Custom_property_scalar:
            p_Custom_property_scalar = new(ma) Custom_property_scalar(ma, instData);
            theObject = (void*)p_Custom_property_scalar;
            break;
         case et_Mass:
            p_Mass = new(ma) Mass(ma, instData);
            theObject = (void*)p_Mass;
            break;
         case et_Volume:
            p_Volume = new(ma) Volume(ma, instData);
            theObject = (void*)p_Volume;
            break;
         case et_Custom_property_contact_vector:
            p_Custom_property_contact_vector = new(ma) Custom_property_contact_vector(ma, instData);
            theObject = (void*)p_Custom_property_contact_vector;
            break;
         case et_Custom_property_contact_scalar:
            p_Custom_property_contact_scalar = new(ma) Custom_property_contact_scalar(ma, instData);
            theObject = (void*)p_Custom_property_contact_scalar;
            break;
         case et_Contact_Force:
            p_Contact_Force = new(ma) Contact_Force(ma, instData);
            theObject = (void*)p_Contact_Force;
            break;
default:
      theObject = NULL;
      break;
   }
   return theObject;
}
tEdmiDefinedTypeData dem_schema_velassco_definedTypes[] = {
{NULL}
};

dem_Schema dem_schema_velassco_SchemaObject(dem_schema_velassco_Entities, dem_schema_velassco_definedTypes, dem_schema_velassco_DefinedTypeNames);

static bool supertypeOf_Simulation(entityType wantedSuperType, void **p);
static bool supertypeOf_Contact_result_property(entityType wantedSuperType, void **p);
static bool supertypeOf_Contact_result(entityType wantedSuperType, void **p);
static bool supertypeOf_Particle_result_property(entityType wantedSuperType, void **p);
static bool supertypeOf_Particle_result(entityType wantedSuperType, void **p);
static bool supertypeOf_Particle(entityType wantedSuperType, void **p);
static bool supertypeOf_Contact(entityType wantedSuperType, void **p);
static bool supertypeOf_Vertex(entityType wantedSuperType, void **p);
static bool supertypeOf_FEM_mesh(entityType wantedSuperType, void **p);
static bool supertypeOf_Timestep(entityType wantedSuperType, void **p);
static bool supertypeOf_Particle_Particle_contact(entityType wantedSuperType, void **p);
static bool supertypeOf_Sphere(entityType wantedSuperType, void **p);
static bool supertypeOf_Template_nn(entityType wantedSuperType, void **p);
static bool supertypeOf_Particle_Geometry_contact(entityType wantedSuperType, void **p);
static bool supertypeOf_Velocity(entityType wantedSuperType, void **p);
static bool supertypeOf_Custom_property_vector(entityType wantedSuperType, void **p);
static bool supertypeOf_Custom_property_scalar(entityType wantedSuperType, void **p);
static bool supertypeOf_Mass(entityType wantedSuperType, void **p);
static bool supertypeOf_Volume(entityType wantedSuperType, void **p);
static bool supertypeOf_Custom_property_contact_vector(entityType wantedSuperType, void **p);
static bool supertypeOf_Custom_property_contact_scalar(entityType wantedSuperType, void **p);
static bool supertypeOf_Contact_Force(entityType wantedSuperType, void **p);
static dbInstance *dbInstanceOf_Simulation(void *obj) { Simulation *p = (Simulation*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Simulation(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Simulation;
}

static dbInstance *dbInstanceOf_Contact_result_property(void *obj) { Contact_result_property *p = (Contact_result_property*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Contact_result_property(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Contact_result_property;
}

static dbInstance *dbInstanceOf_Contact_result(void *obj) { Contact_result *p = (Contact_result*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Contact_result(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Contact_result;
}

static dbInstance *dbInstanceOf_Particle_result_property(void *obj) { Particle_result_property *p = (Particle_result_property*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Particle_result_property(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Particle_result_property;
}

static dbInstance *dbInstanceOf_Particle_result(void *obj) { Particle_result *p = (Particle_result*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Particle_result(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Particle_result;
}

static dbInstance *dbInstanceOf_Particle(void *obj) { Particle *p = (Particle*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Particle(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Particle;
}

static dbInstance *dbInstanceOf_Contact(void *obj) { Contact *p = (Contact*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Contact(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Contact;
}

static dbInstance *dbInstanceOf_Vertex(void *obj) { Vertex *p = (Vertex*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Vertex(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Vertex;
}

static dbInstance *dbInstanceOf_FEM_mesh(void *obj) { FEM_mesh *p = (FEM_mesh*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_FEM_mesh(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_FEM_mesh;
}

static dbInstance *dbInstanceOf_Timestep(void *obj) { Timestep *p = (Timestep*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Timestep(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Timestep;
}

static dbInstance *dbInstanceOf_Particle_Particle_contact(void *obj) { Particle_Particle_contact *p = (Particle_Particle_contact*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Particle_Particle_contact(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Particle_Particle_contact) {
      return true;
   } else {
      Particle_Particle_contact *sub = (Particle_Particle_contact*)*p;;
      Contact *super_1 = static_cast<Contact*>(sub); *p = super_1;
      if (supertypeOf_Contact(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Sphere(void *obj) { Sphere *p = (Sphere*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Sphere(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Sphere) {
      return true;
   } else {
      Sphere *sub = (Sphere*)*p;;
      Particle *super_1 = static_cast<Particle*>(sub); *p = super_1;
      if (supertypeOf_Particle(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Template_nn(void *obj) { Template_nn *p = (Template_nn*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Template_nn(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Template_nn) {
      return true;
   } else {
      Template_nn *sub = (Template_nn*)*p;;
      Particle *super_1 = static_cast<Particle*>(sub); *p = super_1;
      if (supertypeOf_Particle(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Particle_Geometry_contact(void *obj) { Particle_Geometry_contact *p = (Particle_Geometry_contact*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Particle_Geometry_contact(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Particle_Geometry_contact) {
      return true;
   } else {
      Particle_Geometry_contact *sub = (Particle_Geometry_contact*)*p;;
      Contact *super_1 = static_cast<Contact*>(sub); *p = super_1;
      if (supertypeOf_Contact(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Velocity(void *obj) { Velocity *p = (Velocity*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Velocity(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Velocity) {
      return true;
   } else {
      Velocity *sub = (Velocity*)*p;;
      Particle_result_property *super_1 = static_cast<Particle_result_property*>(sub); *p = super_1;
      if (supertypeOf_Particle_result_property(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Custom_property_vector(void *obj) { Custom_property_vector *p = (Custom_property_vector*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Custom_property_vector(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Custom_property_vector) {
      return true;
   } else {
      Custom_property_vector *sub = (Custom_property_vector*)*p;;
      Particle_result_property *super_1 = static_cast<Particle_result_property*>(sub); *p = super_1;
      if (supertypeOf_Particle_result_property(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Custom_property_scalar(void *obj) { Custom_property_scalar *p = (Custom_property_scalar*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Custom_property_scalar(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Custom_property_scalar) {
      return true;
   } else {
      Custom_property_scalar *sub = (Custom_property_scalar*)*p;;
      Particle_result_property *super_1 = static_cast<Particle_result_property*>(sub); *p = super_1;
      if (supertypeOf_Particle_result_property(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Mass(void *obj) { Mass *p = (Mass*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Mass(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Mass) {
      return true;
   } else {
      Mass *sub = (Mass*)*p;;
      Particle_result_property *super_1 = static_cast<Particle_result_property*>(sub); *p = super_1;
      if (supertypeOf_Particle_result_property(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Volume(void *obj) { Volume *p = (Volume*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Volume(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Volume) {
      return true;
   } else {
      Volume *sub = (Volume*)*p;;
      Particle_result_property *super_1 = static_cast<Particle_result_property*>(sub); *p = super_1;
      if (supertypeOf_Particle_result_property(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Custom_property_contact_vector(void *obj) { Custom_property_contact_vector *p = (Custom_property_contact_vector*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Custom_property_contact_vector(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Custom_property_contact_vector) {
      return true;
   } else {
      Custom_property_contact_vector *sub = (Custom_property_contact_vector*)*p;;
      Contact_result_property *super_1 = static_cast<Contact_result_property*>(sub); *p = super_1;
      if (supertypeOf_Contact_result_property(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Custom_property_contact_scalar(void *obj) { Custom_property_contact_scalar *p = (Custom_property_contact_scalar*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Custom_property_contact_scalar(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Custom_property_contact_scalar) {
      return true;
   } else {
      Custom_property_contact_scalar *sub = (Custom_property_contact_scalar*)*p;;
      Contact_result_property *super_1 = static_cast<Contact_result_property*>(sub); *p = super_1;
      if (supertypeOf_Contact_result_property(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Contact_Force(void *obj) { Contact_Force *p = (Contact_Force*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Contact_Force(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Contact_Force) {
      return true;
   } else {
      Contact_Force *sub = (Contact_Force*)*p;;
      Contact_result_property *super_1 = static_cast<Contact_result_property*>(sub); *p = super_1;
      if (supertypeOf_Contact_result_property(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static bool supertypeOf_indeterminate(entityType, void **)
{
   return false;
}


static supertypeCastingFunc castingFunctions[] = {
&supertypeOf_indeterminate,
   &supertypeOf_Simulation,
   &supertypeOf_Contact_result_property,
   &supertypeOf_Contact_result,
   &supertypeOf_Particle_result_property,
   &supertypeOf_Particle_result,
   &supertypeOf_Particle,
   &supertypeOf_Contact,
   &supertypeOf_Vertex,
   &supertypeOf_FEM_mesh,
   &supertypeOf_Timestep,
   &supertypeOf_Particle_Particle_contact,
   &supertypeOf_Sphere,
   &supertypeOf_Template_nn,
   &supertypeOf_Particle_Geometry_contact,
   &supertypeOf_Velocity,
   &supertypeOf_Custom_property_vector,
   &supertypeOf_Custom_property_scalar,
   &supertypeOf_Mass,
   &supertypeOf_Volume,
   &supertypeOf_Custom_property_contact_vector,
   &supertypeOf_Custom_property_contact_scalar,
   &supertypeOf_Contact_Force,
};
void *supertype_cast(entityType wantedSuperType, void *p, entityType subType)
{
   if (subType >= 0 && subType < 23) {
      void *superTypeObject = p;
      if (castingFunctions[subType](wantedSuperType, &superTypeObject)) {
         return superTypeObject;
      }
   }
   return NULL;
}
static dbInstance *dbInstanceOf_indeterminate(void *)
{
   return NULL;
}


static dbInstanceCastingFunc dbInstanceCastingFunctions[] = {
&dbInstanceOf_indeterminate,
   &dbInstanceOf_Simulation,
   &dbInstanceOf_Contact_result_property,
   &dbInstanceOf_Contact_result,
   &dbInstanceOf_Particle_result_property,
   &dbInstanceOf_Particle_result,
   &dbInstanceOf_Particle,
   &dbInstanceOf_Contact,
   &dbInstanceOf_Vertex,
   &dbInstanceOf_FEM_mesh,
   &dbInstanceOf_Timestep,
   &dbInstanceOf_Particle_Particle_contact,
   &dbInstanceOf_Sphere,
   &dbInstanceOf_Template_nn,
   &dbInstanceOf_Particle_Geometry_contact,
   &dbInstanceOf_Velocity,
   &dbInstanceOf_Custom_property_vector,
   &dbInstanceOf_Custom_property_scalar,
   &dbInstanceOf_Mass,
   &dbInstanceOf_Volume,
   &dbInstanceOf_Custom_property_contact_vector,
   &dbInstanceOf_Custom_property_contact_scalar,
   &dbInstanceOf_Contact_Force,
};
dbInstance *dbInstance_cast(void *p, entityType subType)
{
   if (subType < 0 || subType >= 22) {
      THROW("Illegal object type index");
   }
   return dbInstanceCastingFunctions[subType](p);
}


}
