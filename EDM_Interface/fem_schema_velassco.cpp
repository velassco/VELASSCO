#include "stdafx.h"
#include "fem_schema_velassco_header.h"

namespace fem {

EDMLONG dbInstance_AttributeLayout[] = {0};
EDMLONG ModelInfo_AttributeLayout[] = {1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0};
tEdmiAttribute ModelInfo_Attributes[] = {
   {"min_x", 1, 0},
   {"min_y", 1, 0},
   {"min_z", 1, 0},
   {"max_x", 1, 0},
   {"max_y", 1, 0},
   {"max_z", 1, 0},
   {"index_file", 4, 0},
   {"index_file_version", 0, 0},
   {"build_index_time", 0, 0},
   {"last_change_mesh_time", 0, 0},
   {NULL, 0, 0},
};
EDMLONG ModelInfo_Subtypes[] = {0};
EDMLONG Result_AttributeLayout[] = {8, 0};
tEdmiAttribute Result_Attributes[] = {
   {"result_for", 8, 0},
   {NULL, 0, 0},
};
EDMLONG Result_Subtypes[] = {et_ScalarResult, et_VectorResult, 0};
EDMLONG ResultBlock_AttributeLayout[] = {8, 9, 0};
tEdmiAttribute ResultBlock_Attributes[] = {
   {"header", 8, 0},
   {"values", 9, 0},
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
EDMLONG Element_AttributeLayout[] = {4, 9, 0, 0};
tEdmiAttribute Element_Attributes[] = {
   {"name", 4, 0},
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
EDMLONG Mesh_AttributeLayout[] = {4, 0, 0, 6, 9, 9, 0};
tEdmiAttribute Mesh_Attributes[] = {
   {"name", 4, 0},
   {"dimension", 0, 0},
   {"numberOfNodes", 0, 0},
   {"elementType", 6, 0},
   {"nodes", 9, 0},
   {"elements", 9, 0},
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
const char *fem_schema_velassco_DefinedTypeNames[] = {
"LocationType",
"ValueType",
"ElementType",
NULL
};
tEdmiEntityData fem_schema_velassco_Entities[] = {
{"indeterminate"},
{"ModelInfo", 10, 653, 16, 112, 8, et_ModelInfo, ModelInfo_AttributeLayout, ModelInfo_Subtypes, NULL, ModelInfo_Attributes},
{"Result", 1, 647, 8, 24, 5, et_Result, Result_AttributeLayout, Result_Subtypes, NULL, Result_Attributes},
{"ResultBlock", 2, 645, 8, 32, 4, et_ResultBlock, ResultBlock_AttributeLayout, ResultBlock_Subtypes, NULL, ResultBlock_Attributes},
{"ResultHeader", 8, 643, 8, 80, 3, et_ResultHeader, ResultHeader_AttributeLayout, ResultHeader_Subtypes, NULL, ResultHeader_Attributes},
{"Element", 3, 641, 8, 40, 2, et_Element, Element_AttributeLayout, Element_Subtypes, NULL, Element_Attributes},
{"Node", 4, 639, 8, 48, 1, et_Node, Node_AttributeLayout, Node_Subtypes, NULL, Node_Attributes},
{"Mesh", 6, 637, 8, 64, 0, et_Mesh, Mesh_AttributeLayout, Mesh_Subtypes, NULL, Mesh_Attributes},
{"ScalarResult", 2, 649, 8, 32, 6, et_ScalarResult, ScalarResult_AttributeLayout, ScalarResult_Subtypes, NULL, ScalarResult_Attributes},
{"VectorResult", 2, 651, 8, 32, 7, et_VectorResult, VectorResult_AttributeLayout, VectorResult_Subtypes, NULL, VectorResult_Attributes},
{NULL},
};

/*====================================================================================================
   ModelInfo
====================================================================================================*/
double ModelInfo::get_min_x() { return getREAL(0, double, 0); }
void ModelInfo::put_min_x(double v) { putATTRIBUTE(0, double, v, min_x, 0, 1); }
double ModelInfo::get_min_y() { return getREAL(8, double, 1); }
void ModelInfo::put_min_y(double v) { putATTRIBUTE(8, double, v, min_y, 1, 1); }
double ModelInfo::get_min_z() { return getREAL(16, double, 2); }
void ModelInfo::put_min_z(double v) { putATTRIBUTE(16, double, v, min_z, 2, 1); }
double ModelInfo::get_max_x() { return getREAL(24, double, 3); }
void ModelInfo::put_max_x(double v) { putATTRIBUTE(24, double, v, max_x, 3, 1); }
double ModelInfo::get_max_y() { return getREAL(32, double, 4); }
void ModelInfo::put_max_y(double v) { putATTRIBUTE(32, double, v, max_y, 4, 1); }
double ModelInfo::get_max_z() { return getREAL(40, double, 5); }
void ModelInfo::put_max_z(double v) { putATTRIBUTE(40, double, v, max_z, 5, 1); }
char * ModelInfo::get_index_file() { return getATTRIBUTE(48, char *, 6); }
void ModelInfo::put_index_file(char * v) { putATTRIBUTE(48, char *, v, index_file, 6, 4); }
int ModelInfo::get_index_file_version() { return getATTRIBUTE(56, int, 7); }
void ModelInfo::put_index_file_version(int v) { putATTRIBUTE(56, int, v, index_file_version, 7, 0); }
int ModelInfo::get_build_index_time() { return getATTRIBUTE(64, int, 8); }
void ModelInfo::put_build_index_time(int v) { putATTRIBUTE(64, int, v, build_index_time, 8, 0); }
int ModelInfo::get_last_change_mesh_time() { return getATTRIBUTE(72, int, 9); }
void ModelInfo::put_last_change_mesh_time(int v) { putATTRIBUTE(72, int, v, last_change_mesh_time, 9, 0); }
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
void ResultBlock::put_values_element(Result* element) {
   List<Result*>* aggregate = get_values();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<Result*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_values(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
/*====================================================================================================
   ResultHeader
====================================================================================================*/
char * ResultHeader::get_name() { return getATTRIBUTE(0, char *, 0); }
void ResultHeader::put_name(char * v) { putATTRIBUTE(0, char *, v, name, 0, 4); }
char * ResultHeader::get_analysis() { return getATTRIBUTE(8, char *, 1); }
void ResultHeader::put_analysis(char * v) { putATTRIBUTE(8, char *, v, analysis, 1, 4); }
double ResultHeader::get_step() { return getREAL(16, double, 2); }
void ResultHeader::put_step(double v) { putATTRIBUTE(16, double, v, step, 2, 1); }
char * ResultHeader::get_gpName() { return getATTRIBUTE(24, char *, 3); }
void ResultHeader::put_gpName(char * v) { putATTRIBUTE(24, char *, v, gpName, 3, 4); }
List<STRING>* ResultHeader::get_compName() { return getAGGREGATE(32, List<STRING>*, 4); }
void ResultHeader::put_compName(List<STRING>* v) { putAGGREGATE(32, List<STRING>*, v, compName, 4, 9); }
void ResultHeader::put_compName_element(STRING element) {
   List<STRING>* aggregate = get_compName();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<STRING>(m->getMemoryAllocator(), sdaiSTRING, 3);
      put_compName(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
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
char * Element::get_name() { return getATTRIBUTE(0, char *, 0); }
void Element::put_name(char * v) { putATTRIBUTE(0, char *, v, name, 0, 4); }
List<Node*>* Element::get_nodes() { return getAGGREGATE(8, List<Node*>*, 1); }
void Element::put_nodes(List<Node*>* v) { putAGGREGATE(8, List<Node*>*, v, nodes, 1, 9); }
void Element::put_nodes_element(Node* element) {
   List<Node*>* aggregate = get_nodes();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<Node*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_nodes(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
int Element::get_id() { return getATTRIBUTE(16, int, 2); }
void Element::put_id(int v) { putATTRIBUTE(16, int, v, id, 2, 0); }
/*====================================================================================================
   Node
====================================================================================================*/
int Node::get_id() { return getATTRIBUTE(0, int, 0); }
void Node::put_id(int v) { putATTRIBUTE(0, int, v, id, 0, 0); }
double Node::get_x() { return getREAL(8, double, 1); }
void Node::put_x(double v) { putATTRIBUTE(8, double, v, x, 1, 1); }
double Node::get_y() { return getREAL(16, double, 2); }
void Node::put_y(double v) { putATTRIBUTE(16, double, v, y, 2, 1); }
double Node::get_z() { return getREAL(24, double, 3); }
void Node::put_z(double v) { putATTRIBUTE(24, double, v, z, 3, 1); }
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
void Mesh::put_nodes_element(Node* element) {
   List<Node*>* aggregate = get_nodes();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<Node*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_nodes(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
List<Element*>* Mesh::get_elements() { return getAGGREGATE(40, List<Element*>*, 5); }
void Mesh::put_elements(List<Element*>* v) { putAGGREGATE(40, List<Element*>*, v, elements, 5, 9); }
void Mesh::put_elements_element(Element* element) {
   List<Element*>* aggregate = get_elements();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<Element*>(m->getMemoryAllocator(), sdaiINSTANCE, 3);
      put_elements(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}
/*====================================================================================================
   ScalarResult
====================================================================================================*/
double ScalarResult::get_val() { return getREAL(8, double, 1); }
void ScalarResult::put_val(double v) { putATTRIBUTE(8, double, v, val, 1, 1); }
/*====================================================================================================
   VectorResult
====================================================================================================*/
List<REAL>* VectorResult::get_values() { return getAGGREGATE(8, List<REAL>*, 1); }
void VectorResult::put_values(List<REAL>* v) { putAGGREGATE(8, List<REAL>*, v, values, 1, 9); }
void VectorResult::put_values_element(REAL element) {
   List<REAL>* aggregate = get_values();
   if (aggregate == NULL) {
      aggregate = new(m->getMemoryAllocator())List<REAL>(m->getMemoryAllocator(), sdaiREAL, 3);
      put_values(aggregate);
   }
   aggregate->add(element, m->getMemoryAllocator());
}

void* fem_Schema::generateObject(tEdmiInstData *instData, int *entityTypep, Model *ma)
{
   void* theObject = NULL;

   ModelInfo* p_ModelInfo;
   Result* p_Result;
   ResultBlock* p_ResultBlock;
   ResultHeader* p_ResultHeader;
   Element* p_Element;
   Node* p_Node;
   Mesh* p_Mesh;
   ScalarResult* p_ScalarResult;
   VectorResult* p_VectorResult;
   EDMLONG entityType = getEntityType((EDMLONG)instData->entityData);
   *entityTypep = entityType;
   instData->entityData = &theEntities[entityType];
   switch(entityType) {
         case et_ModelInfo:
            p_ModelInfo = new(ma) ModelInfo(ma, instData);
            theObject = (void*)p_ModelInfo;
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
default:
      theObject = NULL;
      break;
   }
   return theObject;
}
tEdmiDefinedTypeData fem_schema_velassco_definedTypes[] = {
{"LocationType", 0, 0, dt_LocationType},
{"ValueType", 0, 0, dt_ValueType},
{"ElementType", 0, 0, dt_ElementType},
{NULL}
};

fem_Schema fem_schema_velassco_SchemaObject(fem_schema_velassco_Entities, fem_schema_velassco_definedTypes, fem_schema_velassco_DefinedTypeNames);

static bool supertypeOf_ModelInfo(entityType wantedSuperType, void **p);
static bool supertypeOf_Result(entityType wantedSuperType, void **p);
static bool supertypeOf_ResultBlock(entityType wantedSuperType, void **p);
static bool supertypeOf_ResultHeader(entityType wantedSuperType, void **p);
static bool supertypeOf_Element(entityType wantedSuperType, void **p);
static bool supertypeOf_Node(entityType wantedSuperType, void **p);
static bool supertypeOf_Mesh(entityType wantedSuperType, void **p);
static bool supertypeOf_ScalarResult(entityType wantedSuperType, void **p);
static bool supertypeOf_VectorResult(entityType wantedSuperType, void **p);
static dbInstance *dbInstanceOf_ModelInfo(void *obj) { ModelInfo *p = (ModelInfo*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_ModelInfo(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_ModelInfo;
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

static bool supertypeOf_indeterminate(entityType, void **)
{
   return false;
}


static supertypeCastingFunc castingFunctions[] = {
&supertypeOf_indeterminate,
   &supertypeOf_ModelInfo,
   &supertypeOf_Result,
   &supertypeOf_ResultBlock,
   &supertypeOf_ResultHeader,
   &supertypeOf_Element,
   &supertypeOf_Node,
   &supertypeOf_Mesh,
   &supertypeOf_ScalarResult,
   &supertypeOf_VectorResult,
};
void *supertype_cast(entityType wantedSuperType, void *p, entityType subType)
{
   if (subType >= 0 && subType < 10) {
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
   &dbInstanceOf_ModelInfo,
   &dbInstanceOf_Result,
   &dbInstanceOf_ResultBlock,
   &dbInstanceOf_ResultHeader,
   &dbInstanceOf_Element,
   &dbInstanceOf_Node,
   &dbInstanceOf_Mesh,
   &dbInstanceOf_ScalarResult,
   &dbInstanceOf_VectorResult,
};
dbInstance *dbInstance_cast(void *p, entityType subType)
{
   if (subType < 0 || subType >= 9) {
      THROW("Illegal object type index");
   }
   return dbInstanceCastingFunctions[subType](p);
}


}
