#ifndef dem_schema_velassco_HPP
#define dem_schema_velassco_HPP
#pragma warning( disable : 4091 )

extern tEdmiEntityData dem_schema_velassco_Entities[];

namespace dem {

class dem_Schema;
class Simulation;
class Contact_result_property;
class Contact_result;
class Particle_result_property;
class Particle_result;
class Particle;
class Contact;
class Vertex;
class FEM_mesh;
class Timestep;
class Particle_Particle_contact;
class Sphere;
class Template_nn;
class Particle_Geometry_contact;
class Velocity;
class Custom_property_vector;
class Custom_property_scalar;
class Mass;
class Volume;
class Custom_property_contact_vector;
class Custom_property_contact_scalar;
class Contact_Force;
typedef enum {} definedTypeNames;


class Simulation :  public dbInstance
{
protected:
   Simulation() {}
public:
   static const entityType type = et_Simulation;
   List<Timestep*>*                     get_consists_of();
   void                                 put_consists_of(List<Timestep*>* v);
   void                                 unset_consists_of() { unsetAttribute(0); }
   bool                                 exists_consists_of() { return isAttrSet(0); }
   void                                 put_consists_of_element(Timestep*);
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Simulation(Model *m, entityType et=et_Simulation) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Simulation(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Contact_result_property :  public dbInstance
{
protected:
   Contact_result_property() {}
public:
   static const entityType type = et_Contact_result_property;
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Contact_result_property(Model *m, entityType et=et_Contact_result_property) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Contact_result_property(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Contact_result :  public dbInstance
{
protected:
   Contact_result() {}
public:
   static const entityType type = et_Contact_result;
   Contact*                             get_valid_for();
   void *                             get_valid_for(entityType *etp);
   void                                 put_valid_for(Contact* v);
   void                                 unset_valid_for() { unsetAttribute(0); }
   bool                                 exists_valid_for() { return isAttrSet(0); }
   List<Contact_result_property*>*      get_contact_results_properties();
   void                                 put_contact_results_properties(List<Contact_result_property*>* v);
   void                                 unset_contact_results_properties() { unsetAttribute(1); }
   bool                                 exists_contact_results_properties() { return isAttrSet(1); }
   void                                 put_contact_results_properties_element(Contact_result_property*);
   Timestep*                            get_calculated_for();
   void                                 put_calculated_for(Timestep* v);
   void                                 unset_calculated_for() { unsetAttribute(2); }
   bool                                 exists_calculated_for() { return isAttrSet(2); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Contact_result(Model *m, entityType et=et_Contact_result) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Contact_result(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Particle_result_property :  public dbInstance
{
protected:
   Particle_result_property() {}
public:
   static const entityType type = et_Particle_result_property;
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Particle_result_property(Model *m, entityType et=et_Particle_result_property) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Particle_result_property(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Particle_result :  public dbInstance
{
protected:
   Particle_result() {}
public:
   static const entityType type = et_Particle_result;
   Particle*                            get_valid_for();
   void *                            get_valid_for(entityType *etp);
   void                                 put_valid_for(Particle* v);
   void                                 unset_valid_for() { unsetAttribute(0); }
   bool                                 exists_valid_for() { return isAttrSet(0); }
   List<Particle_result_property*>*     get_result_properties();
   void                                 put_result_properties(List<Particle_result_property*>* v);
   void                                 unset_result_properties() { unsetAttribute(1); }
   bool                                 exists_result_properties() { return isAttrSet(1); }
   void                                 put_result_properties_element(Particle_result_property*);
   Timestep*                            get_calculated_for();
   void                                 put_calculated_for(Timestep* v);
   void                                 unset_calculated_for() { unsetAttribute(2); }
   bool                                 exists_calculated_for() { return isAttrSet(2); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Particle_result(Model *m, entityType et=et_Particle_result) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Particle_result(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Particle :  public dbInstance
{
protected:
   Particle() {}
public:
   static const entityType type = et_Particle;
   int                                  get_id();
   void                                 put_id(int v);
   void                                 unset_id() { unsetAttribute(0); }
   bool                                 exists_id() { return isAttrSet(0); }
   int                                  get_group();
   void                                 put_group(int v);
   void                                 unset_group() { unsetAttribute(1); }
   bool                                 exists_group() { return isAttrSet(1); }
   Array<REAL>*                         get_coordinates();
   void                                 put_coordinates(Array<REAL>* v);
   void                                 unset_coordinates() { unsetAttribute(2); }
   bool                                 exists_coordinates() { return isAttrSet(2); }
   void                                 put_coordinates_element(int index, REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Particle(Model *m, entityType et=et_Particle) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Particle(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Contact :  public dbInstance
{
protected:
   Contact() {}
public:
   static const entityType type = et_Contact;
   Array<REAL>*                         get_contact_location();
   void                                 put_contact_location(Array<REAL>* v);
   void                                 unset_contact_location() { unsetAttribute(0); }
   bool                                 exists_contact_location() { return isAttrSet(0); }
   void                                 put_contact_location_element(int index, REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Contact(Model *m, entityType et=et_Contact) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Contact(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Vertex :  public dbInstance
{
protected:
   Vertex() {}
public:
   static const entityType type = et_Vertex;
   List<REAL>*                          get_vertex_position();
   void                                 put_vertex_position(List<REAL>* v);
   void                                 unset_vertex_position() { unsetAttribute(0); }
   bool                                 exists_vertex_position() { return isAttrSet(0); }
   void                                 put_vertex_position_element(REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Vertex(Model *m, entityType et=et_Vertex) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Vertex(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class FEM_mesh :  public dbInstance
{
protected:
   FEM_mesh() {}
public:
   static const entityType type = et_FEM_mesh;
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(0); }
   bool                                 exists_name() { return isAttrSet(0); }
   Set<Set<Vertex*>*>*                  get_facets();
   void                                 put_facets(Set<Set<Vertex*>*>* v);
   void                                 unset_facets() { unsetAttribute(1); }
   bool                                 exists_facets() { return isAttrSet(1); }
   void                                 put_facets_element(Set<Vertex*>*);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   FEM_mesh(Model *m, entityType et=et_FEM_mesh) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   FEM_mesh(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Timestep :  public dbInstance
{
protected:
   Timestep() {}
public:
   static const entityType type = et_Timestep;
   double                               get_time_value();
   void                                 put_time_value(double v);
   void                                 unset_time_value() { unsetAttribute(0); }
   bool                                 exists_time_value() { return isAttrSet(0); }
   Set<FEM_mesh*>*                      get_boundary();
   void                                 put_boundary(Set<FEM_mesh*>* v);
   void                                 unset_boundary() { unsetAttribute(1); }
   bool                                 exists_boundary() { return isAttrSet(1); }
   void                                 put_boundary_element(FEM_mesh*);
   List<Contact*>*                      get_has_contact();
   void                                 put_has_contact(List<Contact*>* v);
   void                                 unset_has_contact() { unsetAttribute(2); }
   bool                                 exists_has_contact() { return isAttrSet(2); }
   void                                 put_has_contact_element(Contact*);
   List<Particle*>*                     get_consists_of();
   void                                 put_consists_of(List<Particle*>* v);
   void                                 unset_consists_of() { unsetAttribute(3); }
   bool                                 exists_consists_of() { return isAttrSet(3); }
   void                                 put_consists_of_element(Particle*);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Timestep(Model *m, entityType et=et_Timestep) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Timestep(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Particle_Particle_contact :  public Contact
{
protected:
   Particle_Particle_contact() {}
public:
   static const entityType type = et_Particle_Particle_contact;
   Particle*                            get_P1();
   void *                            get_P1(entityType *etp);
   void                                 put_P1(Particle* v);
   void                                 unset_P1() { unsetAttribute(1); }
   bool                                 exists_P1() { return isAttrSet(1); }
   Particle*                            get_P2();
   void *                            get_P2(entityType *etp);
   void                                 put_P2(Particle* v);
   void                                 unset_P2() { unsetAttribute(2); }
   bool                                 exists_P2() { return isAttrSet(2); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Particle_Particle_contact(Model *m, entityType et=et_Particle_Particle_contact) : Contact(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Particle_Particle_contact(Model *_m, tEdmiInstData *instData) : Contact(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Sphere :  public Particle
{
protected:
   Sphere() {}
public:
   static const entityType type = et_Sphere;
   double                               get_radius();
   void                                 put_radius(double v);
   void                                 unset_radius() { unsetAttribute(3); }
   bool                                 exists_radius() { return isAttrSet(3); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Sphere(Model *m, entityType et=et_Sphere) : Particle(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Sphere(Model *_m, tEdmiInstData *instData) : Particle(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Template_nn :  public Particle
{
protected:
   Template_nn() {}
public:
   static const entityType type = et_Template_nn;
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Template_nn(Model *m, entityType et=et_Template_nn) : Particle(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Template_nn(Model *_m, tEdmiInstData *instData) : Particle(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Particle_Geometry_contact :  public Contact
{
protected:
   Particle_Geometry_contact() {}
public:
   static const entityType type = et_Particle_Geometry_contact;
   Particle*                            get_P1();
   void *                            get_P1(entityType *etp);
   void                                 put_P1(Particle* v);
   void                                 unset_P1() { unsetAttribute(1); }
   bool                                 exists_P1() { return isAttrSet(1); }
   FEM_mesh*                            get_geometry();
   void                                 put_geometry(FEM_mesh* v);
   void                                 unset_geometry() { unsetAttribute(2); }
   bool                                 exists_geometry() { return isAttrSet(2); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Particle_Geometry_contact(Model *m, entityType et=et_Particle_Geometry_contact) : Contact(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Particle_Geometry_contact(Model *_m, tEdmiInstData *instData) : Contact(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Velocity :  public Particle_result_property
{
protected:
   Velocity() {}
public:
   static const entityType type = et_Velocity;
   Array<REAL>*                         get_Vx_Vy_Vz();
   void                                 put_Vx_Vy_Vz(Array<REAL>* v);
   void                                 unset_Vx_Vy_Vz() { unsetAttribute(0); }
   bool                                 exists_Vx_Vy_Vz() { return isAttrSet(0); }
   void                                 put_Vx_Vy_Vz_element(int index, REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Velocity(Model *m, entityType et=et_Velocity) : Particle_result_property(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Velocity(Model *_m, tEdmiInstData *instData) : Particle_result_property(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Custom_property_vector :  public Particle_result_property
{
protected:
   Custom_property_vector() {}
public:
   static const entityType type = et_Custom_property_vector;
   Array<REAL>*                         get_CPx_CPy_CPz();
   void                                 put_CPx_CPy_CPz(Array<REAL>* v);
   void                                 unset_CPx_CPy_CPz() { unsetAttribute(0); }
   bool                                 exists_CPx_CPy_CPz() { return isAttrSet(0); }
   void                                 put_CPx_CPy_CPz_element(int index, REAL);
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Custom_property_vector(Model *m, entityType et=et_Custom_property_vector) : Particle_result_property(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Custom_property_vector(Model *_m, tEdmiInstData *instData) : Particle_result_property(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Custom_property_scalar :  public Particle_result_property
{
protected:
   Custom_property_scalar() {}
public:
   static const entityType type = et_Custom_property_scalar;
   double                               get_custom_prop();
   void                                 put_custom_prop(double v);
   void                                 unset_custom_prop() { unsetAttribute(0); }
   bool                                 exists_custom_prop() { return isAttrSet(0); }
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Custom_property_scalar(Model *m, entityType et=et_Custom_property_scalar) : Particle_result_property(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Custom_property_scalar(Model *_m, tEdmiInstData *instData) : Particle_result_property(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Mass :  public Particle_result_property
{
protected:
   Mass() {}
public:
   static const entityType type = et_Mass;
   double                               get_mass();
   void                                 put_mass(double v);
   void                                 unset_mass() { unsetAttribute(0); }
   bool                                 exists_mass() { return isAttrSet(0); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Mass(Model *m, entityType et=et_Mass) : Particle_result_property(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Mass(Model *_m, tEdmiInstData *instData) : Particle_result_property(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Volume :  public Particle_result_property
{
protected:
   Volume() {}
public:
   static const entityType type = et_Volume;
   double                               get_volume();
   void                                 put_volume(double v);
   void                                 unset_volume() { unsetAttribute(0); }
   bool                                 exists_volume() { return isAttrSet(0); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Volume(Model *m, entityType et=et_Volume) : Particle_result_property(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Volume(Model *_m, tEdmiInstData *instData) : Particle_result_property(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Custom_property_contact_vector :  public Contact_result_property
{
protected:
   Custom_property_contact_vector() {}
public:
   static const entityType type = et_Custom_property_contact_vector;
   Array<REAL>*                         get_CPx_CPy_CPz();
   void                                 put_CPx_CPy_CPz(Array<REAL>* v);
   void                                 unset_CPx_CPy_CPz() { unsetAttribute(0); }
   bool                                 exists_CPx_CPy_CPz() { return isAttrSet(0); }
   void                                 put_CPx_CPy_CPz_element(int index, REAL);
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Custom_property_contact_vector(Model *m, entityType et=et_Custom_property_contact_vector) : Contact_result_property(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Custom_property_contact_vector(Model *_m, tEdmiInstData *instData) : Contact_result_property(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Custom_property_contact_scalar :  public Contact_result_property
{
protected:
   Custom_property_contact_scalar() {}
public:
   static const entityType type = et_Custom_property_contact_scalar;
   double                               get_property_value();
   void                                 put_property_value(double v);
   void                                 unset_property_value() { unsetAttribute(0); }
   bool                                 exists_property_value() { return isAttrSet(0); }
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(1); }
   bool                                 exists_name() { return isAttrSet(1); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Custom_property_contact_scalar(Model *m, entityType et=et_Custom_property_contact_scalar) : Contact_result_property(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Custom_property_contact_scalar(Model *_m, tEdmiInstData *instData) : Contact_result_property(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Contact_Force :  public Contact_result_property
{
protected:
   Contact_Force() {}
public:
   static const entityType type = et_Contact_Force;
   List<REAL>*                          get_Fx_Fy_Fz();
   void                                 put_Fx_Fy_Fz(List<REAL>* v);
   void                                 unset_Fx_Fy_Fz() { unsetAttribute(0); }
   bool                                 exists_Fx_Fy_Fz() { return isAttrSet(0); }
   void                                 put_Fx_Fy_Fz_element(REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Contact_Force(Model *m, entityType et=et_Contact_Force) : Contact_result_property(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Contact_Force(Model *_m, tEdmiInstData *instData) : Contact_result_property(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};

class dem_Schema : public dbSchema
{
public:
                           dem_Schema(tEdmiEntityData* ep, tEdmiDefinedTypeData *dt, const char **dtn) : dbSchema(ep, dt, dtn) { }
   void                    *generateObject(tEdmiInstData *instData, int *entityTypep, Model *ma);
};

typedef bool (*supertypeCastingFunc) (entityType wantedSuperType, void **p);

typedef dbInstance* (*dbInstanceCastingFunc) (void *p);

extern void *supertype_cast(entityType wantedSuperType, void *p, entityType subType);

extern dbInstance *dbInstance_cast(void *p, entityType subType);

extern const char *dem_schema_velassco_DefinedTypeNames[];

extern dem_Schema dem_schema_velassco_SchemaObject;
}
#endif
