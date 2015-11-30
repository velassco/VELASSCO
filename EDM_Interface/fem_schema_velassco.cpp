#include "stdafx.h"
#include "fem_schema_velassco_header.h"

namespace fem {

EDMLONG dbInstance_AttributeLayout[] = {0};
EDMLONG CacheEntry_AttributeLayout[] = {4, 15, 0, 0};
tEdmiAttribute CacheEntry_Attributes[] = {
   {"cache_type", 4, 0},
   {"teh_file", 15, 0},
   {"size", 0, 0},
   {NULL, 0, 0},
};
EDMLONG CacheEntry_Subtypes[] = {0};
EDMLONG GaussPoint_AttributeLayout[] = {8, 9, 6, 0};
tEdmiAttribute GaussPoint_Attributes[] = {
   {"gauss_point_for", 8, 0},
   {"values", 9, 0},
   {"GPtype", 6, 0},
   {NULL, 0, 0},
};
EDMLONG GaussPoint_Subtypes[] = {0};
EDMLONG Result_AttributeLayout[] = {8, 0};
tEdmiAttribute Result_Attributes[] = {
   {"result_for", 8, 0},
   {NULL, 0, 0},
};
EDMLONG Result_Subtypes[] = {et_ScalarResult, et_VectorResult, et_Matrix_2D, et_Matrix_3D, et_Matrix_Deformated, 0};
EDMLONG ResultBlock_AttributeLayout[] = {8, 9, 8, 0};
tEdmiAttribute ResultBlock_Attributes[] = {
   {"header", 8, 0},
   {"values", 9, 0},
   {"gauss_points", 8, 0},
   {NULL, 0, 0},
};
EDMLONG ResultBlock_Subtypes[] = {0};
EDMLONG ResultHeader_AttributeLayout[] = {4, 4, 1, 4, 9, 0, 6, 6, 0};
tEdmiAttribute ResultHeader_Attributes[] = {
   {"name", 4, 0},
   {"analysis", 4, 0},
   {"step", 1, 0},
   {"gpName", 4, 0},
   {"compName", 9, 0},
   {"indexMData", 0, 0},
   {"rType", 6, 0},
   {"location", 6, 0},
   {NULL, 0, 0},
};
EDMLONG ResultHeader_Subtypes[] = {0};
EDMLONG Element_AttributeLayout[] = {9, 0, 0};
tEdmiAttribute Element_Attributes[] = {
   {"nodes", 9, 0},
   {"id", 0, 0},
   {NULL, 0, 0},
};
EDMLONG Element_Subtypes[] = {0};
EDMLONG Node_AttributeLayout[] = {0, 1, 1, 1, 0};
tEdmiAttribute Node_Attributes[] = {
   {"id", 0, 0},
   {"x", 1, 0},
   {"y", 1, 0},
   {"z", 1, 0},
   {NULL, 0, 0},
};
EDMLONG Node_Subtypes[] = {0};
EDMLONG Mesh_AttributeLayout[] = {4, 0, 0, 6, 9, 9, 9, 0};
tEdmiAttribute Mesh_Attributes[] = {
   {"name", 4, 0},
   {"dimension", 0, 0},
   {"numberOfNodes", 0, 0},
   {"elementType", 6, 0},
   {"nodes", 9, 0},
   {"elements", 9, 0},
   {"results", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Mesh_Subtypes[] = {0};
EDMLONG ScalarResult_AttributeLayout[] = {8, 1, 0};
tEdmiAttribute ScalarResult_Attributes[] = {
   {"result_for", 8, 0},
   {"val", 1, 0},
   {NULL, 0, 0},
};
EDMLONG ScalarResult_Subtypes[] = {0};
EDMLONG VectorResult_AttributeLayout[] = {8, 9, 0};
tEdmiAttribute VectorResult_Attributes[] = {
   {"result_for", 8, 0},
   {"values", 9, 0},
   {NULL, 0, 0},
};
EDMLONG VectorResult_Subtypes[] = {0};
EDMLONG Matrix_2D_AttributeLayout[] = {8, 9, 0};
tEdmiAttribute Matrix_2D_Attributes[] = {
   {"result_for", 8, 0},
   {"values", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Matrix_2D_Subtypes[] = {0};
EDMLONG Matrix_3D_AttributeLayout[] = {8, 9, 0};
tEdmiAttribute Matrix_3D_Attributes[] = {
   {"result_for", 8, 0},
   {"values", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Matrix_3D_Subtypes[] = {0};
EDMLONG Matrix_Deformated_AttributeLayout[] = {8, 9, 0};
tEdmiAttribute Matrix_Deformated_Attributes[] = {
   {"result_for", 8, 0},
   {"values", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Matrix_Deformated_Subtypes[] = {0};
const char *fem_schema_velassco_DefinedTypeNames[] = {
"GaussPointType",
"LocationType",
"ValueType",
"ElementType",
NULL
};
tEdmiEntityData fem_schema_velassco_Entities[] = {
{"indeterminate"},
{"CacheEntry", 3, 663, 8, 40, 12, et_CacheEntry, CacheEntry_AttributeLayout, CacheEntry_Subtypes, NULL, CacheEntry_Attributes},
{"GaussPoint", 3, 661, 8, 40, 11, et_GaussPoint, GaussPoint_AttributeLayout, GaussPoint_Subtypes, NULL, GaussPoint_Attributes},
{"Result", 1, 649, 8, 24, 5, et_Result, Result_AttributeLayout, Result_Subtypes, NULL, Result_Attributes},
{"ResultBlock", 3, 647, 8, 40, 4, et_ResultBlock, ResultBlock_AttributeLayout, ResultBlock_Subtypes, NULL, ResultBlock_Attributes},
{"ResultHeader", 8, 645, 8, 80, 3, et_ResultHeader, ResultHeader_AttributeLayout, ResultHeader_Subtypes, NULL, ResultHeader_Attributes},
{"Element", 2, 643, 8, 32, 2, et_Element, Element_AttributeLayout, Element_Subtypes, NULL, Element_Attributes},
{"Node", 4, 641, 8, 48, 1, et_Node, Node_AttributeLayout, Node_Subtypes, NULL, Node_Attributes},
{"Mesh", 7, 639, 8, 72, 0, et_Mesh, Mesh_AttributeLayout, Mesh_Subtypes, NULL, Mesh_Attributes},
{"ScalarResult", 2, 651, 8, 32, 6, et_ScalarResult, ScalarResult_AttributeLayout, ScalarResult_Subtypes, NULL, ScalarResult_Attributes},
{"VectorResult", 2, 653, 8, 32, 7, et_VectorResult, VectorResult_AttributeLayout, VectorResult_Subtypes, NULL, VectorResult_Attributes},
{"Matrix_2D", 2, 655, 8, 32, 8, et_Matrix_2D, Matrix_2D_AttributeLayout, Matrix_2D_Subtypes, NULL, Matrix_2D_Attributes},
{"Matrix_3D", 2, 657, 8, 32, 9, et_Matrix_3D, Matrix_3D_AttributeLayout, Matrix_3D_Subtypes, NULL, Matrix_3D_Attributes},
{"Matrix_Deformated", 2, 659, 8, 32, 10, et_Matrix_Deformated, Matrix_Deformated_AttributeLayout, Matrix_Deformated_Subtypes, NULL, Matrix_Deformated_Attributes},
{NULL},
};

/*====================================================================================================
   CacheEntry
====================================================================================================*/
char * CacheEntry::get_cache_type() { return getATTRIBUTE(0, char *, 0); }
void CacheEntry::put_cache_type(char * v) { putATTRIBUTE(0, char *, v, cache_type, 0, 4); }
int CacheEntry::get_teh_file() { return getATTRIBUTE(8, int, 1); }
void CacheEntry::put_teh_file(int v) { putATTRIBUTE(8, int, v, teh_file, 1, 15); }
int CacheEntry::get_size() { return getATTRIBUTE(16, int, 2); }
void CacheEntry::put_size(int v) { putATTRIBUTE(16, int, v, size, 2, 0); }
/*====================================================================================================
   GaussPoint
====================================================================================================*/
Element* GaussPoint::get_gauss_point_for() { return getInstance(0, Element*, 0); }
void GaussPoint::put_gauss_point_for(Element* v) { putInstance(0, Element*, v, gauss_point_for, 0, 8); v->addToUsedIn(c); }
Set<REAL>* GaussPoint::get_values() { return getAGGREGATE(8, Set<REAL>*, 1); }
void GaussPoint::put_values(Set<REAL>* v) { putAGGREGATE(8, Set<REAL>*, v, values, 1, 9); }
SdaiAggr  GaussPoint::get_values_aggrId() { return getAGGRID(1); }
void GaussPoint::put_values_element(REAL element) {
   SdaiAggr agId = get_values_aggrId();
   Set<REAL> aggregate(m, agId);
   if (agId == 0) {
      put_values(&aggregate);
   }
   aggregate.add(element);
}
GaussPointType GaussPoint::get_GPtype() { return getATTRIBUTE(16, GaussPointType, 2); }
void GaussPoint::put_GPtype(GaussPointType v) { putATTRIBUTE(16, GaussPointType, v, GPtype, 2, 6); }
/*====================================================================================================
   Result
====================================================================================================*/
Node* Result::get_result_for() { return getInstance(0, Node*, 0); }
void Result::put_result_for(Node* v) { putInstance(0, Node*, v, result_for, 0, 8); v->addToUsedIn(c); }
/*====================================================================================================
   ResultBlock
====================================================================================================*/
ResultHeader* ResultBlock::get_header() { return getInstance(0, ResultHeader*, 0); }
void ResultBlock::put_header(ResultHeader* v) { putInstance(0, ResultHeader*, v, header, 0, 8); v->addToUsedIn(c); }
List<Result*>* ResultBlock::get_values() { return getAGGREGATE(8, List<Result*>*, 1); }
void ResultBlock::put_values(List<Result*>* v) { putAGGREGATE(8, List<Result*>*, v, values, 1, 9); }
SdaiAggr  ResultBlock::get_values_aggrId() { return getAGGRID(1); }
void ResultBlock::put_values_element(Result* element) {
   SdaiAggr agId = get_values_aggrId();
   List<Result*> aggregate(m, agId);
   if (agId == 0) {
      put_values(&aggregate);
   }
   aggregate.add(element);
}
GaussPoint* ResultBlock::get_gauss_points() { return getInstance(16, GaussPoint*, 2); }
void ResultBlock::put_gauss_points(GaussPoint* v) { putInstance(16, GaussPoint*, v, gauss_points, 2, 8); v->addToUsedIn(c); }
/*====================================================================================================
   ResultHeader
====================================================================================================*/
char * ResultHeader::get_name() { return getATTRIBUTE(0, char *, 0); }
void ResultHeader::put_name(char * v) { putATTRIBUTE(0, char *, v, name, 0, 4); }
char * ResultHeader::get_analysis() { return getATTRIBUTE(8, char *, 1); }
void ResultHeader::put_analysis(char * v) { putATTRIBUTE(8, char *, v, analysis, 1, 4); }
double ResultHeader::get_step() { return getREAL(16, double, 2); }
void ResultHeader::put_step(double v) { putREAL(16, double, v, step, 2, 1); }
char * ResultHeader::get_gpName() { return getATTRIBUTE(24, char *, 3); }
void ResultHeader::put_gpName(char * v) { putATTRIBUTE(24, char *, v, gpName, 3, 4); }
List<STRING>* ResultHeader::get_compName() { return getAGGREGATE(32, List<STRING>*, 4); }
void ResultHeader::put_compName(List<STRING>* v) { putAGGREGATE(32, List<STRING>*, v, compName, 4, 9); }
SdaiAggr  ResultHeader::get_compName_aggrId() { return getAGGRID(4); }
void ResultHeader::put_compName_element(STRING element) {
   SdaiAggr agId = get_compName_aggrId();
   List<STRING> aggregate(m, agId);
   if (agId == 0) {
      put_compName(&aggregate);
   }
   aggregate.add(element);
}
int ResultHeader::get_indexMData() { return getATTRIBUTE(40, int, 5); }
void ResultHeader::put_indexMData(int v) { putATTRIBUTE(40, int, v, indexMData, 5, 0); }
ValueType ResultHeader::get_rType() { return getATTRIBUTE(48, ValueType, 6); }
void ResultHeader::put_rType(ValueType v) { putATTRIBUTE(48, ValueType, v, rType, 6, 6); }
LocationType ResultHeader::get_location() { return getATTRIBUTE(56, LocationType, 7); }
void ResultHeader::put_location(LocationType v) { putATTRIBUTE(56, LocationType, v, location, 7, 6); }
/*====================================================================================================
   Element
====================================================================================================*/
List<Node*>* Element::get_nodes() { return getAGGREGATE(0, List<Node*>*, 0); }
void Element::put_nodes(List<Node*>* v) { putAGGREGATE(0, List<Node*>*, v, nodes, 0, 9); }
SdaiAggr  Element::get_nodes_aggrId() { return getAGGRID(0); }
void Element::put_nodes_element(Node* element) {
   SdaiAggr agId = get_nodes_aggrId();
   List<Node*> aggregate(m, agId);
   if (agId == 0) {
      put_nodes(&aggregate);
   }
   aggregate.add(element);
}
int Element::get_id() { return getATTRIBUTE(8, int, 1); }
void Element::put_id(int v) { putATTRIBUTE(8, int, v, id, 1, 0); }
/*====================================================================================================
   Node
====================================================================================================*/
int Node::get_id() { return getATTRIBUTE(0, int, 0); }
void Node::put_id(int v) { putATTRIBUTE(0, int, v, id, 0, 0); }
double Node::get_x() { return getREAL(8, double, 1); }
void Node::put_x(double v) { putREAL(8, double, v, x, 1, 1); }
double Node::get_y() { return getREAL(16, double, 2); }
void Node::put_y(double v) { putREAL(16, double, v, y, 2, 1); }
double Node::get_z() { return getREAL(24, double, 3); }
void Node::put_z(double v) { putREAL(24, double, v, z, 3, 1); }
/*====================================================================================================
   Mesh
====================================================================================================*/
char * Mesh::get_name() { return getATTRIBUTE(0, char *, 0); }
void Mesh::put_name(char * v) { putATTRIBUTE(0, char *, v, name, 0, 4); }
int Mesh::get_dimension() { return getATTRIBUTE(8, int, 1); }
void Mesh::put_dimension(int v) { putATTRIBUTE(8, int, v, dimension, 1, 0); }
int Mesh::get_numberOfNodes() { return getATTRIBUTE(16, int, 2); }
void Mesh::put_numberOfNodes(int v) { putATTRIBUTE(16, int, v, numberOfNodes, 2, 0); }
ElementType Mesh::get_elementType() { return getATTRIBUTE(24, ElementType, 3); }
void Mesh::put_elementType(ElementType v) { putATTRIBUTE(24, ElementType, v, elementType, 3, 6); }
List<Node*>* Mesh::get_nodes() { return getAGGREGATE(32, List<Node*>*, 4); }
void Mesh::put_nodes(List<Node*>* v) { putAGGREGATE(32, List<Node*>*, v, nodes, 4, 9); }
SdaiAggr  Mesh::get_nodes_aggrId() { return getAGGRID(4); }
void Mesh::put_nodes_element(Node* element) {
   SdaiAggr agId = get_nodes_aggrId();
   List<Node*> aggregate(m, agId);
   if (agId == 0) {
      put_nodes(&aggregate);
   }
   aggregate.add(element);
}
List<Element*>* Mesh::get_elements() { return getAGGREGATE(40, List<Element*>*, 5); }
void Mesh::put_elements(List<Element*>* v) { putAGGREGATE(40, List<Element*>*, v, elements, 5, 9); }
SdaiAggr  Mesh::get_elements_aggrId() { return getAGGRID(5); }
void Mesh::put_elements_element(Element* element) {
   SdaiAggr agId = get_elements_aggrId();
   List<Element*> aggregate(m, agId);
   if (agId == 0) {
      put_elements(&aggregate);
   }
   aggregate.add(element);
}
List<ResultHeader*>* Mesh::get_results() { return getAGGREGATE(48, List<ResultHeader*>*, 6); }
void Mesh::put_results(List<ResultHeader*>* v) { putAGGREGATE(48, List<ResultHeader*>*, v, results, 6, 9); }
SdaiAggr  Mesh::get_results_aggrId() { return getAGGRID(6); }
void Mesh::put_results_element(ResultHeader* element) {
   SdaiAggr agId = get_results_aggrId();
   List<ResultHeader*> aggregate(m, agId);
   if (agId == 0) {
      put_results(&aggregate);
   }
   aggregate.add(element);
}
/*====================================================================================================
   ScalarResult
====================================================================================================*/
double ScalarResult::get_val() { return getREAL(8, double, 1); }
void ScalarResult::put_val(double v) { putREAL(8, double, v, val, 1, 1); }
/*====================================================================================================
   VectorResult
====================================================================================================*/
List<REAL>* VectorResult::get_values() { return getAGGREGATE(8, List<REAL>*, 1); }
void VectorResult::put_values(List<REAL>* v) { putAGGREGATE(8, List<REAL>*, v, values, 1, 9); }
SdaiAggr  VectorResult::get_values_aggrId() { return getAGGRID(1); }
void VectorResult::put_values_element(REAL element) {
   SdaiAggr agId = get_values_aggrId();
   List<REAL> aggregate(m, agId);
   if (agId == 0) {
      put_values(&aggregate);
   }
   aggregate.add(element);
}
/*====================================================================================================
   Matrix_2D
====================================================================================================*/
Array<REAL>* Matrix_2D::get_values() { return getAGGREGATE(8, Array<REAL>*, 1); }
void Matrix_2D::put_values(Array<REAL>* v) { putAGGREGATE(8, Array<REAL>*, v, values, 1, 9); }
SdaiAggr  Matrix_2D::get_values_aggrId() { return getAGGRID(1); }
void Matrix_2D::put_values_element(int index, REAL element) {
   SdaiAggr agId = get_values_aggrId();
   Array<REAL> aggregate(m, agId);
   if (agId == 0) {
      put_values(&aggregate);
   }
   aggregate.put(index, element);
}
/*====================================================================================================
   Matrix_3D
====================================================================================================*/
Array<REAL>* Matrix_3D::get_values() { return getAGGREGATE(8, Array<REAL>*, 1); }
void Matrix_3D::put_values(Array<REAL>* v) { putAGGREGATE(8, Array<REAL>*, v, values, 1, 9); }
SdaiAggr  Matrix_3D::get_values_aggrId() { return getAGGRID(1); }
void Matrix_3D::put_values_element(int index, REAL element) {
   SdaiAggr agId = get_values_aggrId();
   Array<REAL> aggregate(m, agId);
   if (agId == 0) {
      put_values(&aggregate);
   }
   aggregate.put(index, element);
}
/*====================================================================================================
   Matrix_Deformated
====================================================================================================*/
Array<REAL>* Matrix_Deformated::get_values() { return getAGGREGATE(8, Array<REAL>*, 1); }
void Matrix_Deformated::put_values(Array<REAL>* v) { putAGGREGATE(8, Array<REAL>*, v, values, 1, 9); }
SdaiAggr  Matrix_Deformated::get_values_aggrId() { return getAGGRID(1); }
void Matrix_Deformated::put_values_element(int index, REAL element) {
   SdaiAggr agId = get_values_aggrId();
   Array<REAL> aggregate(m, agId);
   if (agId == 0) {
      put_values(&aggregate);
   }
   aggregate.put(index, element);
}

void* fem_Schema::generateObject(tEdmiInstData *instData, int *entityTypep, Model *ma)
{
   void* theObject = NULL;

   CacheEntry* p_CacheEntry;
   GaussPoint* p_GaussPoint;
   Result* p_Result;
   ResultBlock* p_ResultBlock;
   ResultHeader* p_ResultHeader;
   Element* p_Element;
   Node* p_Node;
   Mesh* p_Mesh;
   ScalarResult* p_ScalarResult;
   VectorResult* p_VectorResult;
   Matrix_2D* p_Matrix_2D;
   Matrix_3D* p_Matrix_3D;
   Matrix_Deformated* p_Matrix_Deformated;
   EDMLONG entityType = getEntityType((EDMLONG)instData->entityData);
   *entityTypep = entityType;
   instData->entityData = &theEntities[entityType];
   switch(entityType) {
         case et_CacheEntry:
            p_CacheEntry = new(ma) CacheEntry(ma, instData);
            theObject = (void*)p_CacheEntry;
            break;
         case et_GaussPoint:
            p_GaussPoint = new(ma) GaussPoint(ma, instData);
            theObject = (void*)p_GaussPoint;
            break;
         case et_Result:
            p_Result = new(ma) Result(ma, instData);
            theObject = (void*)p_Result;
            break;
         case et_ResultBlock:
            p_ResultBlock = new(ma) ResultBlock(ma, instData);
            theObject = (void*)p_ResultBlock;
            break;
         case et_ResultHeader:
            p_ResultHeader = new(ma) ResultHeader(ma, instData);
            theObject = (void*)p_ResultHeader;
            break;
         case et_Element:
            p_Element = new(ma) Element(ma, instData);
            theObject = (void*)p_Element;
            break;
         case et_Node:
            p_Node = new(ma) Node(ma, instData);
            theObject = (void*)p_Node;
            break;
         case et_Mesh:
            p_Mesh = new(ma) Mesh(ma, instData);
            theObject = (void*)p_Mesh;
            break;
         case et_ScalarResult:
            p_ScalarResult = new(ma) ScalarResult(ma, instData);
            theObject = (void*)p_ScalarResult;
            break;
         case et_VectorResult:
            p_VectorResult = new(ma) VectorResult(ma, instData);
            theObject = (void*)p_VectorResult;
            break;
         case et_Matrix_2D:
            p_Matrix_2D = new(ma) Matrix_2D(ma, instData);
            theObject = (void*)p_Matrix_2D;
            break;
         case et_Matrix_3D:
            p_Matrix_3D = new(ma) Matrix_3D(ma, instData);
            theObject = (void*)p_Matrix_3D;
            break;
         case et_Matrix_Deformated:
            p_Matrix_Deformated = new(ma) Matrix_Deformated(ma, instData);
            theObject = (void*)p_Matrix_Deformated;
            break;
default:
      theObject = NULL;
      break;
   }
   return theObject;
}
tEdmiDefinedTypeData fem_schema_velassco_definedTypes[] = {
{"GaussPointType", 0, 0, dt_GaussPointType},
{"LocationType", 0, 0, dt_LocationType},
{"ValueType", 0, 0, dt_ValueType},
{"ElementType", 0, 0, dt_ElementType},
{NULL}
};

fem_Schema fem_schema_velassco_SchemaObject(fem_schema_velassco_Entities, fem_schema_velassco_definedTypes, fem_schema_velassco_DefinedTypeNames);

static bool supertypeOf_CacheEntry(entityType wantedSuperType, void **p);
static bool supertypeOf_GaussPoint(entityType wantedSuperType, void **p);
static bool supertypeOf_Result(entityType wantedSuperType, void **p);
static bool supertypeOf_ResultBlock(entityType wantedSuperType, void **p);
static bool supertypeOf_ResultHeader(entityType wantedSuperType, void **p);
static bool supertypeOf_Element(entityType wantedSuperType, void **p);
static bool supertypeOf_Node(entityType wantedSuperType, void **p);
static bool supertypeOf_Mesh(entityType wantedSuperType, void **p);
static bool supertypeOf_ScalarResult(entityType wantedSuperType, void **p);
static bool supertypeOf_VectorResult(entityType wantedSuperType, void **p);
static bool supertypeOf_Matrix_2D(entityType wantedSuperType, void **p);
static bool supertypeOf_Matrix_3D(entityType wantedSuperType, void **p);
static bool supertypeOf_Matrix_Deformated(entityType wantedSuperType, void **p);
static dbInstance *dbInstanceOf_CacheEntry(void *obj) { CacheEntry *p = (CacheEntry*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_CacheEntry(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_CacheEntry;
}

static dbInstance *dbInstanceOf_GaussPoint(void *obj) { GaussPoint *p = (GaussPoint*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_GaussPoint(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_GaussPoint;
}

static dbInstance *dbInstanceOf_Result(void *obj) { Result *p = (Result*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Result(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Result;
}

static dbInstance *dbInstanceOf_ResultBlock(void *obj) { ResultBlock *p = (ResultBlock*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_ResultBlock(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_ResultBlock;
}

static dbInstance *dbInstanceOf_ResultHeader(void *obj) { ResultHeader *p = (ResultHeader*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_ResultHeader(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_ResultHeader;
}

static dbInstance *dbInstanceOf_Element(void *obj) { Element *p = (Element*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Element(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Element;
}

static dbInstance *dbInstanceOf_Node(void *obj) { Node *p = (Node*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Node(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Node;
}

static dbInstance *dbInstanceOf_Mesh(void *obj) { Mesh *p = (Mesh*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Mesh(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Mesh;
}

static dbInstance *dbInstanceOf_ScalarResult(void *obj) { ScalarResult *p = (ScalarResult*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_ScalarResult(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_ScalarResult) {
      return true;
   } else {
      ScalarResult *sub = (ScalarResult*)*p;;
      Result *super_1 = static_cast<Result*>(sub); *p = super_1;
      if (supertypeOf_Result(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_VectorResult(void *obj) { VectorResult *p = (VectorResult*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_VectorResult(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_VectorResult) {
      return true;
   } else {
      VectorResult *sub = (VectorResult*)*p;;
      Result *super_1 = static_cast<Result*>(sub); *p = super_1;
      if (supertypeOf_Result(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Matrix_2D(void *obj) { Matrix_2D *p = (Matrix_2D*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Matrix_2D(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Matrix_2D) {
      return true;
   } else {
      Matrix_2D *sub = (Matrix_2D*)*p;;
      Result *super_1 = static_cast<Result*>(sub); *p = super_1;
      if (supertypeOf_Result(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Matrix_3D(void *obj) { Matrix_3D *p = (Matrix_3D*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Matrix_3D(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Matrix_3D) {
      return true;
   } else {
      Matrix_3D *sub = (Matrix_3D*)*p;;
      Result *super_1 = static_cast<Result*>(sub); *p = super_1;
      if (supertypeOf_Result(wantedSuperType, p)) {
         return true;
      }
      return false;
   }
}

static dbInstance *dbInstanceOf_Matrix_Deformated(void *obj) { Matrix_Deformated *p = (Matrix_Deformated*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Matrix_Deformated(entityType wantedSuperType, void **p)
{
   if (wantedSuperType == et_Matrix_Deformated) {
      return true;
   } else {
      Matrix_Deformated *sub = (Matrix_Deformated*)*p;;
      Result *super_1 = static_cast<Result*>(sub); *p = super_1;
      if (supertypeOf_Result(wantedSuperType, p)) {
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
   &supertypeOf_CacheEntry,
   &supertypeOf_GaussPoint,
   &supertypeOf_Result,
   &supertypeOf_ResultBlock,
   &supertypeOf_ResultHeader,
   &supertypeOf_Element,
   &supertypeOf_Node,
   &supertypeOf_Mesh,
   &supertypeOf_ScalarResult,
   &supertypeOf_VectorResult,
   &supertypeOf_Matrix_2D,
   &supertypeOf_Matrix_3D,
   &supertypeOf_Matrix_Deformated,
};
void *supertype_cast(entityType wantedSuperType, void *p, entityType subType)
{
   if (subType >= 0 && subType < 14) {
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
   &dbInstanceOf_CacheEntry,
   &dbInstanceOf_GaussPoint,
   &dbInstanceOf_Result,
   &dbInstanceOf_ResultBlock,
   &dbInstanceOf_ResultHeader,
   &dbInstanceOf_Element,
   &dbInstanceOf_Node,
   &dbInstanceOf_Mesh,
   &dbInstanceOf_ScalarResult,
   &dbInstanceOf_VectorResult,
   &dbInstanceOf_Matrix_2D,
   &dbInstanceOf_Matrix_3D,
   &dbInstanceOf_Matrix_Deformated,
};
dbInstance *dbInstance_cast(void *p, entityType subType)
{
   if (subType < 0 || subType >= 13) {
      THROW("Illegal object type index");
   }
   return dbInstanceCastingFunctions[subType](p);
}


}
