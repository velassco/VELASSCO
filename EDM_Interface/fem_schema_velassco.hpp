#ifndef fem_schema_velassco_HPP
#define fem_schema_velassco_HPP
#pragma warning( disable : 4091 )

extern tEdmiEntityData fem_schema_velassco_Entities[];

namespace fem {

class fem_Schema;
class GaussPoint;
class Result;
class ResultBlock;
class ResultHeader;
class Element;
class Node;
class Mesh;
class ScalarResult;
class VectorResult;
class Matrix_2D;
class Matrix_3D;
class Matrix_Deformated;
typedef enum {GaussPointType_GP_LINE_1, GaussPointType_GP_TRIANGLE_1, GaussPointType_GP_TRIANGLE_3, GaussPointType_GP_TRIANGLE_6, GaussPointType_GP_TETRAHEDRA_1, GaussPointType_GP_TETRAHEDRA_4, GaussPointType_GP_TETRAHEDRA_10, GaussPointType_GP_SPHERE_1, GaussPointType_GP_QUADRILATERAL_1, GaussPointType_GP_QUADRILATERAL_4, GaussPointType_GP_QUADRILATERAL_9, GaussPointType_GP_HEXAHEDRA_1, GaussPointType_GP_HEXAHEDRA_8, GaussPointType_GP_HEXAHEDRA_27, GaussPointType_GP_PRISM_1, GaussPointType_GP_PRISM_6, GaussPointType_GP_PIRAMID_1, GaussPointType_GP_PIRAMID_5, GaussPointType_GP_CIRCLE_1} GaussPointType;
typedef enum {LocationType_ONNODES, LocationType_ONGAUSSPOINTS} LocationType;
typedef enum {ValueType_SCALAR, ValueType_VECTOR, ValueType_MATRIX, ValueType_PLAINDEFORMATIONMATRIX, ValueType_MAINMATRIX, ValueType_LOCALAXES, ValueType_COMPLEXSCALAR, ValueType_COMPLEXVECTOR} ValueType;
typedef enum {ElementType_POINT, ElementType_LINE, ElementType_TRIANGLE, ElementType_QUADRILATERAL, ElementType_TETRAHEDRA, ElementType_HEXAHEDRA, ElementType_PRISM, ElementType_PYRAMID, ElementType_SPHERE, ElementType_CIRCLE} ElementType;
typedef enum {dt_GaussPointType, dt_LocationType, dt_ValueType, dt_ElementType} definedTypeNames;


class GaussPoint :  public dbInstance
{
protected:
   GaussPoint() {}
public:
   static const entityType type = et_GaussPoint;
   Element*                             get_gauss_point_for();
   void                                 put_gauss_point_for(Element* v);
   void                                 unset_gauss_point_for() { unsetAttribute(0); }
   bool                                 exists_gauss_point_for() { return isAttrSet(0); }
   Set<REAL>*                           get_values();
   void                                 put_values(Set<REAL>* v);
   void                                 unset_values() { unsetAttribute(1); }
   bool                                 exists_values() { return isAttrSet(1); }
   SdaiAggr                             get_values_aggrId();
   void                                 put_values_element(REAL);
   GaussPointType                       get_GPtype();
   void                                 put_GPtype(GaussPointType v);
   void                                 unset_GPtype() { unsetAttribute(2); }
   bool                                 exists_GPtype() { return isAttrSet(2); }
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   GaussPoint(Model *m, entityType et=et_GaussPoint) : dbInstance(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   GaussPoint(Model *_m, tEdmiInstData *instData) :  dbInstance(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
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
   SdaiAggr                             get_values_aggrId();
   void                                 put_values_element(Result*);
   GaussPoint*                          get_gauss_points();
   void                                 put_gauss_points(GaussPoint* v);
   void                                 unset_gauss_points() { unsetAttribute(2); }
   bool                                 exists_gauss_points() { return isAttrSet(2); }
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
   SdaiAggr                             get_compName_aggrId();
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
   List<Node*>*                         get_nodes();
   void                                 put_nodes(List<Node*>* v);
   void                                 unset_nodes() { unsetAttribute(0); }
   bool                                 exists_nodes() { return isAttrSet(0); }
   SdaiAggr                             get_nodes_aggrId();
   void                                 put_nodes_element(Node*);
   int                                  get_id();
   void                                 put_id(int v);
   void                                 unset_id() { unsetAttribute(1); }
   bool                                 exists_id() { return isAttrSet(1); }
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
   SdaiAggr                             get_nodes_aggrId();
   void                                 put_nodes_element(Node*);
   List<Element*>*                      get_elements();
   void                                 put_elements(List<Element*>* v);
   void                                 unset_elements() { unsetAttribute(5); }
   bool                                 exists_elements() { return isAttrSet(5); }
   SdaiAggr                             get_elements_aggrId();
   void                                 put_elements_element(Element*);
   List<ResultHeader*>*                 get_results();
   void                                 put_results(List<ResultHeader*>* v);
   void                                 unset_results() { unsetAttribute(6); }
   bool                                 exists_results() { return isAttrSet(6); }
   SdaiAggr                             get_results_aggrId();
   void                                 put_results_element(ResultHeader*);
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
   SdaiAggr                             get_values_aggrId();
   void                                 put_values_element(REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   VectorResult(Model *m, entityType et=et_VectorResult) : Result(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   VectorResult(Model *_m, tEdmiInstData *instData) : Result(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Matrix_2D :  public Result
{
protected:
   Matrix_2D() {}
public:
   static const entityType type = et_Matrix_2D;
   Array<REAL>*                         get_values();
   void                                 put_values(Array<REAL>* v);
   void                                 unset_values() { unsetAttribute(1); }
   bool                                 exists_values() { return isAttrSet(1); }
   SdaiAggr                             get_values_aggrId();
   void                                 put_values_element(int index, REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Matrix_2D(Model *m, entityType et=et_Matrix_2D) : Result(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Matrix_2D(Model *_m, tEdmiInstData *instData) : Result(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Matrix_3D :  public Result
{
protected:
   Matrix_3D() {}
public:
   static const entityType type = et_Matrix_3D;
   Array<REAL>*                         get_values();
   void                                 put_values(Array<REAL>* v);
   void                                 unset_values() { unsetAttribute(1); }
   bool                                 exists_values() { return isAttrSet(1); }
   SdaiAggr                             get_values_aggrId();
   void                                 put_values_element(int index, REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Matrix_3D(Model *m, entityType et=et_Matrix_3D) : Result(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Matrix_3D(Model *_m, tEdmiInstData *instData) : Result(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
};


class Matrix_Deformated :  public Result
{
protected:
   Matrix_Deformated() {}
public:
   static const entityType type = et_Matrix_Deformated;
   Array<REAL>*                         get_values();
   void                                 put_values(Array<REAL>* v);
   void                                 unset_values() { unsetAttribute(1); }
   bool                                 exists_values() { return isAttrSet(1); }
   SdaiAggr                             get_values_aggrId();
   void                                 put_values_element(int index, REAL);
   void* operator new(size_t sz, Model *m) { return m->allocZeroFilled(sz); }
   Matrix_Deformated(Model *m, entityType et=et_Matrix_Deformated) : Result(m, et) { if (! c) dbInstance::init(m, et); c->cppObject = (void*)this; }
   Matrix_Deformated(Model *_m, tEdmiInstData *instData) : Result(_m, instData) { c = instData; m = _m; c->cppObject = (void*)this; }
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
