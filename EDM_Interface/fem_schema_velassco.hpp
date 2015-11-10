#ifndef fem_schema_velassco_HPP
#define fem_schema_velassco_HPP
#pragma warning( disable : 4091 )

extern tEdmiEntityData fem_schema_velassco_Entities[];

namespace fem {

class fem_Schema;
class ModelInfo;
class Result;
class ResultBlock;
class ResultHeader;
class Element;
class Node;
class Mesh;
class ScalarResult;
class VectorResult;
typedef enum {LocationType_LOC_NODE, LocationType_LOC_GPOINT} LocationType;
typedef enum {ValueType_SCALAR, ValueType_VECTOR} ValueType;
typedef enum {ElementType_LINE, ElementType_TETRAHEDRA, ElementType_TRIANGLE, ElementType_SPHERE} ElementType;
typedef enum {dt_LocationType, dt_ValueType, dt_ElementType} definedTypeNames;


class ModelInfo :  public dbInstance
{
protected:
   ModelInfo() {}
public:
   static const entityType type = et_ModelInfo;
   double                               get_min_x();
   void                                 put_min_x(double v);
   void                                 unset_min_x() { unsetAttribute(0); }
   bool                                 exists_min_x() { return isAttrSet(0); }
   double                               get_min_y();
   void                                 put_min_y(double v);
   void                                 unset_min_y() { unsetAttribute(1); }
   bool                                 exists_min_y() { return isAttrSet(1); }
   double                               get_min_z();
   void                                 put_min_z(double v);
   void                                 unset_min_z() { unsetAttribute(2); }
   bool                                 exists_min_z() { return isAttrSet(2); }
   double                               get_max_x();
   void                                 put_max_x(double v);
   void                                 unset_max_x() { unsetAttribute(3); }
   bool                                 exists_max_x() { return isAttrSet(3); }
   double                               get_max_y();
   void                                 put_max_y(double v);
   void                                 unset_max_y() { unsetAttribute(4); }
   bool                                 exists_max_y() { return isAttrSet(4); }
   double                               get_max_z();
   void                                 put_max_z(double v);
   void                                 unset_max_z() { unsetAttribute(5); }
   bool                                 exists_max_z() { return isAttrSet(5); }
   char *                               get_index_file();
   void                                 put_index_file(char * v);
   void                                 unset_index_file() { unsetAttribute(6); }
   bool                                 exists_index_file() { return isAttrSet(6); }
   int                                  get_index_file_version();
   void                                 put_index_file_version(int v);
   void                                 unset_index_file_version() { unsetAttribute(7); }
   bool                                 exists_index_file_version() { return isAttrSet(7); }
   int                                  get_build_index_time();
   void                                 put_build_index_time(int v);
   void                                 unset_build_index_time() { unsetAttribute(8); }
   bool                                 exists_build_index_time() { return isAttrSet(8); }
   int                                  get_last_change_mesh_time();
   void                                 put_last_change_mesh_time(int v);
   void                                 unset_last_change_mesh_time() { unsetAttribute(9); }
   bool                                 exists_last_change_mesh_time() { return isAttrSet(9); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   ModelInfo(Model *m, entityType et=et_ModelInfo) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   ModelInfo(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Result :  public dbInstance
{
protected:
   Result() {}
public:
   static const entityType type = et_Result;
   Node*                                get_result_for();
   void                                 put_result_for(Node* v);
   void                                 unset_result_for() { unsetAttribute(0); }
   bool                                 exists_result_for() { return isAttrSet(0); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Result(Model *m, entityType et=et_Result) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Result(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class ResultBlock :  public dbInstance
{
protected:
   ResultBlock() {}
public:
   static const entityType type = et_ResultBlock;
   ResultHeader*                        get_header();
   void                                 put_header(ResultHeader* v);
   void                                 unset_header() { unsetAttribute(0); }
   bool                                 exists_header() { return isAttrSet(0); }
   List<Result*>*                       get_values();
   void                                 put_values(List<Result*>* v);
   void                                 unset_values() { unsetAttribute(1); }
   bool                                 exists_values() { return isAttrSet(1); }
   void                                 put_values_element(Result*);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   ResultBlock(Model *m, entityType et=et_ResultBlock) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   ResultBlock(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class ResultHeader :  public dbInstance
{
protected:
   ResultHeader() {}
public:
   static const entityType type = et_ResultHeader;
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(0); }
   bool                                 exists_name() { return isAttrSet(0); }
   char *                               get_analysis();
   void                                 put_analysis(char * v);
   void                                 unset_analysis() { unsetAttribute(1); }
   bool                                 exists_analysis() { return isAttrSet(1); }
   double                               get_step();
   void                                 put_step(double v);
   void                                 unset_step() { unsetAttribute(2); }
   bool                                 exists_step() { return isAttrSet(2); }
   char *                               get_gpName();
   void                                 put_gpName(char * v);
   void                                 unset_gpName() { unsetAttribute(3); }
   bool                                 exists_gpName() { return isAttrSet(3); }
   List<STRING>*                        get_compName();
   void                                 put_compName(List<STRING>* v);
   void                                 unset_compName() { unsetAttribute(4); }
   bool                                 exists_compName() { return isAttrSet(4); }
   void                                 put_compName_element(STRING);
   int                                  get_indexMData();
   void                                 put_indexMData(int v);
   void                                 unset_indexMData() { unsetAttribute(5); }
   bool                                 exists_indexMData() { return isAttrSet(5); }
   ValueType                            get_rType();
   void                                 put_rType(ValueType v);
   void                                 unset_rType() { unsetAttribute(6); }
   bool                                 exists_rType() { return isAttrSet(6); }
   LocationType                         get_location();
   void                                 put_location(LocationType v);
   void                                 unset_location() { unsetAttribute(7); }
   bool                                 exists_location() { return isAttrSet(7); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   ResultHeader(Model *m, entityType et=et_ResultHeader) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   ResultHeader(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Element :  public dbInstance
{
protected:
   Element() {}
public:
   static const entityType type = et_Element;
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(0); }
   bool                                 exists_name() { return isAttrSet(0); }
   List<Node*>*                         get_nodes();
   void                                 put_nodes(List<Node*>* v);
   void                                 unset_nodes() { unsetAttribute(1); }
   bool                                 exists_nodes() { return isAttrSet(1); }
   void                                 put_nodes_element(Node*);
   int                                  get_id();
   void                                 put_id(int v);
   void                                 unset_id() { unsetAttribute(2); }
   bool                                 exists_id() { return isAttrSet(2); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Element(Model *m, entityType et=et_Element) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Element(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Node :  public dbInstance
{
protected:
   Node() {}
public:
   static const entityType type = et_Node;
   int                                  get_id();
   void                                 put_id(int v);
   void                                 unset_id() { unsetAttribute(0); }
   bool                                 exists_id() { return isAttrSet(0); }
   double                               get_x();
   void                                 put_x(double v);
   void                                 unset_x() { unsetAttribute(1); }
   bool                                 exists_x() { return isAttrSet(1); }
   double                               get_y();
   void                                 put_y(double v);
   void                                 unset_y() { unsetAttribute(2); }
   bool                                 exists_y() { return isAttrSet(2); }
   double                               get_z();
   void                                 put_z(double v);
   void                                 unset_z() { unsetAttribute(3); }
   bool                                 exists_z() { return isAttrSet(3); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Node(Model *m, entityType et=et_Node) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Node(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Mesh :  public dbInstance
{
protected:
   Mesh() {}
public:
   static const entityType type = et_Mesh;
   char *                               get_name();
   void                                 put_name(char * v);
   void                                 unset_name() { unsetAttribute(0); }
   bool                                 exists_name() { return isAttrSet(0); }
   int                                  get_dimension();
   void                                 put_dimension(int v);
   void                                 unset_dimension() { unsetAttribute(1); }
   bool                                 exists_dimension() { return isAttrSet(1); }
   int                                  get_numberOfNodes();
   void                                 put_numberOfNodes(int v);
   void                                 unset_numberOfNodes() { unsetAttribute(2); }
   bool                                 exists_numberOfNodes() { return isAttrSet(2); }
   ElementType                          get_elementType();
   void                                 put_elementType(ElementType v);
   void                                 unset_elementType() { unsetAttribute(3); }
   bool                                 exists_elementType() { return isAttrSet(3); }
   List<Node*>*                         get_nodes();
   void                                 put_nodes(List<Node*>* v);
   void                                 unset_nodes() { unsetAttribute(4); }
   bool                                 exists_nodes() { return isAttrSet(4); }
   void                                 put_nodes_element(Node*);
   List<Element*>*                      get_elements();
   void                                 put_elements(List<Element*>* v);
   void                                 unset_elements() { unsetAttribute(5); }
   bool                                 exists_elements() { return isAttrSet(5); }
   void                                 put_elements_element(Element*);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Mesh(Model *m, entityType et=et_Mesh) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Mesh(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class ScalarResult :  public Result
{
protected:
   ScalarResult() {}
public:
   static const entityType type = et_ScalarResult;
   double                               get_val();
   void                                 put_val(double v);
   void                                 unset_val() { unsetAttribute(1); }
   bool                                 exists_val() { return isAttrSet(1); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   ScalarResult(Model *m, entityType et=et_ScalarResult) : Result(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   ScalarResult(Model *_m, tEdmiInstData *instData) : Result(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class VectorResult :  public Result
{
protected:
   VectorResult() {}
public:
   static const entityType type = et_VectorResult;
   List<REAL>*                          get_values();
   void                                 put_values(List<REAL>* v);
   void                                 unset_values() { unsetAttribute(1); }
   bool                                 exists_values() { return isAttrSet(1); }
   void                                 put_values_element(REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   VectorResult(Model *m, entityType et=et_VectorResult) : Result(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   VectorResult(Model *_m, tEdmiInstData *instData) : Result(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};

class fem_Schema : public dbSchema
{
public:
                           fem_Schema(tEdmiEntityData* ep, tEdmiDefinedTypeData *dt, const char **dtn) : dbSchema(ep, dt, dtn) { }
   void                    *generateObject(tEdmiInstData *instData, int *entityTypep, Model *ma);
};

typedef bool (*supertypeCastingFunc) (entityType wantedSuperType, void **p);

typedef dbInstance* (*dbInstanceCastingFunc) (void *p);

extern void *supertype_cast(entityType wantedSuperType, void *p, entityType subType);

extern dbInstance *dbInstance_cast(void *p, entityType subType);

extern const char *fem_schema_velassco_DefinedTypeNames[];

extern fem_Schema fem_schema_velassco_SchemaObject;
}
#endif
