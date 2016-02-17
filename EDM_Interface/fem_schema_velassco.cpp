#include "stdafx.h"
#include "fem_schema_velassco_header.h"

namespace fem {

EDMLONG dbInstance_AttributeLayout[] = {0};
EDMLONG TimeStep_AttributeLayout[] = {8, 1, 8, 9, 4, 0};
tEdmiAttribute TimeStep_Attributes[] = {
   {"analysis", 8, 0},
   {"time_value", 1, 0},
   {"mesh", 8, 0},
   {"results", 9, 0},
   {"name", 4, 0},
   {NULL, 0, 0},
};
EDMLONG TimeStep_Subtypes[] = {0};
EDMLONG Analysis_AttributeLayout[] = {4, 9, 0};
tEdmiAttribute Analysis_Attributes[] = {
   {"name", 4, 0},
   {"time_steps", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Analysis_Subtypes[] = {0};
EDMLONG GaussPoint_AttributeLayout[] = {8, 9, 6, 0};
tEdmiAttribute GaussPoint_Attributes[] = {
   {"gauss_point_for", 8, 0},
   {"values", 9, 0},
   {"GPtype", 6, 0},
   {NULL, 0, 0},
};
EDMLONG GaussPoint_Subtypes[] = {0};
EDMLONG ResultHeader_AttributeLayout[] = {4, 4, 1, 4, 9, 6, 6, 9, 9, 0};
tEdmiAttribute ResultHeader_Attributes[] = {
   {"name", 4, 0},
   {"analysis", 4, 0},
   {"step", 1, 0},
   {"gpName", 4, 0},
   {"compName", 9, 0},
   {"rType", 6, 0},
   {"location", 6, 0},
   {"gauss_points", 9, 0},
   {"values", 9, 0},
   {NULL, 0, 0},
};
EDMLONG ResultHeader_Subtypes[] = {0};
EDMLONG Result_AttributeLayout[] = {8, 8, 0};
tEdmiAttribute Result_Attributes[] = {
   {"result_for", 8, 0},
   {"result_header", 8, 0},
   {NULL, 0, 0},
};
EDMLONG Result_Subtypes[] = {et_ScalarResult, et_VectorResult, et_Matrix_2D, et_Matrix_3D, et_Matrix_Deformated, 0};
EDMLONG Element_AttributeLayout[] = {9, 0, 0};
tEdmiAttribute Element_Attributes[] = {
   {"nodes", 9, 0},
   {"id", 0, 0},
   {NULL, 0, 0},
};
EDMLONG Element_Subtypes[] = {0};
EDMLONG Node_AttributeLayout[] = {0, 1, 1, 1, 9, 9, 0};
tEdmiAttribute Node_Attributes[] = {
   {"id", 0, 0},
   {"x", 1, 0},
   {"y", 1, 0},
   {"z", 1, 0},
   {"elements", 9, 0},
   {"results", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Node_Subtypes[] = {0};
EDMLONG Mesh_AttributeLayout[] = {4, 0, 0, 6, 9, 9, 9, 0, 0, 0, 0, 0};
tEdmiAttribute Mesh_Attributes[] = {
   {"name", 4, 0},
   {"dimension", 0, 0},
   {"numberOfNodes", 0, 0},
   {"elementType", 6, 0},
   {"nodes", 9, 0},
   {"elements", 9, 0},
   {"results", 9, 0},
   {"maxNodeID", 0, 0},
   {"minNodeID", 0, 0},
   {"minElementID", 0, 0},
   {"maxElementID", 0, 0},
   {NULL, 0, 0},
};
EDMLONG Mesh_Subtypes[] = {0};
EDMLONG ScalarResult_AttributeLayout[] = {8, 8, 1, 0};
tEdmiAttribute ScalarResult_Attributes[] = {
   {"result_for", 8, 0},
   {"result_header", 8, 0},
   {"val", 1, 0},
   {NULL, 0, 0},
};
EDMLONG ScalarResult_Subtypes[] = {0};
EDMLONG VectorResult_AttributeLayout[] = {8, 8, 9, 0};
tEdmiAttribute VectorResult_Attributes[] = {
   {"result_for", 8, 0},
   {"result_header", 8, 0},
   {"values", 9, 0},
   {NULL, 0, 0},
};
EDMLONG VectorResult_Subtypes[] = {0};
EDMLONG Matrix_2D_AttributeLayout[] = {8, 8, 9, 0};
tEdmiAttribute Matrix_2D_Attributes[] = {
   {"result_for", 8, 0},
   {"result_header", 8, 0},
   {"values", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Matrix_2D_Subtypes[] = {0};
EDMLONG Matrix_3D_AttributeLayout[] = {8, 8, 9, 0};
tEdmiAttribute Matrix_3D_Attributes[] = {
   {"result_for", 8, 0},
   {"result_header", 8, 0},
   {"values", 9, 0},
   {NULL, 0, 0},
};
EDMLONG Matrix_3D_Subtypes[] = {0};
EDMLONG Matrix_Deformated_AttributeLayout[] = {8, 8, 9, 0};
tEdmiAttribute Matrix_Deformated_Attributes[] = {
   {"result_for", 8, 0},
   {"result_header", 8, 0},
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
{"TimeStep", 5, 663, 8, 56, 12, et_TimeStep, TimeStep_AttributeLayout, TimeStep_Subtypes, NULL, TimeStep_Attributes},
{"Analysis", 2, 661, 8, 32, 11, et_Analysis, Analysis_AttributeLayout, Analysis_Subtypes, NULL, Analysis_Attributes},
{"GaussPoint", 3, 657, 8, 40, 9, et_GaussPoint, GaussPoint_AttributeLayout, GaussPoint_Subtypes, NULL, GaussPoint_Attributes},
{"ResultHeader", 9, 655, 16, 104, 8, et_ResultHeader, ResultHeader_AttributeLayout, ResultHeader_Subtypes, NULL, ResultHeader_Attributes},
{"Result", 2, 643, 8, 32, 2, et_Result, Result_AttributeLayout, Result_Subtypes, NULL, Result_Attributes},
{"Element", 2, 659, 8, 32, 10, et_Element, Element_AttributeLayout, Element_Subtypes, NULL, Element_Attributes},
{"Node", 6, 641, 8, 64, 1, et_Node, Node_AttributeLayout, Node_Subtypes, NULL, Node_Attributes},
{"Mesh", 11, 639, 16, 120, 0, et_Mesh, Mesh_AttributeLayout, Mesh_Subtypes, NULL, Mesh_Attributes},
{"ScalarResult", 3, 645, 8, 40, 3, et_ScalarResult, ScalarResult_AttributeLayout, ScalarResult_Subtypes, NULL, ScalarResult_Attributes},
{"VectorResult", 3, 647, 8, 40, 4, et_VectorResult, VectorResult_AttributeLayout, VectorResult_Subtypes, NULL, VectorResult_Attributes},
{"Matrix_2D", 3, 649, 8, 40, 5, et_Matrix_2D, Matrix_2D_AttributeLayout, Matrix_2D_Subtypes, NULL, Matrix_2D_Attributes},
{"Matrix_3D", 3, 651, 8, 40, 6, et_Matrix_3D, Matrix_3D_AttributeLayout, Matrix_3D_Subtypes, NULL, Matrix_3D_Attributes},
{"Matrix_Deformated", 3, 653, 8, 40, 7, et_Matrix_Deformated, Matrix_Deformated_AttributeLayout, Matrix_Deformated_Subtypes, NULL, Matrix_Deformated_Attributes},
{NULL},
};

/*====================================================================================================
   TimeStep
====================================================================================================*/
Analysis* TimeStep::get_analysis() { return getInstance(0, Analysis*, 0); }
void TimeStep::put_analysis(Analysis* v) { putInstance(0, Analysis*, v, analysis, 0, 8); v->addToUsedIn(c); }
double TimeStep::get_time_value() { return getREAL(8, double, 1); }
void TimeStep::put_time_value(double v) { putREAL(8, double, v, time_value, 1, 1); }
Mesh* TimeStep::get_mesh() { return getInstance(16, Mesh*, 2); }
void TimeStep::put_mesh(Mesh* v) { putInstance(16, Mesh*, v, mesh, 2, 8); v->addToUsedIn(c); }
List<ResultHeader*>* TimeStep::get_results() { return getAGGREGATE(24, List<ResultHeader*>*, 3); }
void TimeStep::put_results(List<ResultHeader*>* v) { putAGGREGATE(24, List<ResultHeader*>*, v, results, 3, 9); }
SdaiAggr  TimeStep::get_results_aggrId() { return getAGGRID(3); }
void TimeStep::put_results_element(ResultHeader* element) {
   SdaiAggr agId = get_results_aggrId();
   List<ResultHeader*> aggregate(m, agId);
   if (agId == 0) {
      put_results(&aggregate);
   }
   aggregate.add(element);
}
char * TimeStep::get_name() { return getATTRIBUTE(32, char *, 4); }
void TimeStep::put_name(char * v) { putATTRIBUTE(32, char *, v, name, 4, 4); }
/*====================================================================================================
   Analysis
====================================================================================================*/
char * Analysis::get_name() { return getATTRIBUTE(0, char *, 0); }
void Analysis::put_name(char * v) { putATTRIBUTE(0, char *, v, name, 0, 4); }
Set<TimeStep*>* Analysis::get_time_steps() { return getAGGREGATE(8, Set<TimeStep*>*, 1); }
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
ValueType ResultHeader::get_rType() { return getATTRIBUTE(40, ValueType, 5); }
void ResultHeader::put_rType(ValueType v) { putATTRIBUTE(40, ValueType, v, rType, 5, 6); }
LocationType ResultHeader::get_location() { return getATTRIBUTE(48, LocationType, 6); }
void ResultHeader::put_location(LocationType v) { putATTRIBUTE(48, LocationType, v, location, 6, 6); }
Set<GaussPoint*>* ResultHeader::get_gauss_points() { return getAGGREGATE(56, Set<GaussPoint*>*, 7); }
void ResultHeader::put_gauss_points(Set<GaussPoint*>* v) { putAGGREGATE(56, Set<GaussPoint*>*, v, gauss_points, 7, 9); }
SdaiAggr  ResultHeader::get_gauss_points_aggrId() { return getAGGRID(7); }
void ResultHeader::put_gauss_points_element(GaussPoint* element) {
   SdaiAggr agId = get_gauss_points_aggrId();
   Set<GaussPoint*> aggregate(m, agId);
   if (agId == 0) {
      put_gauss_points(&aggregate);
   }
   aggregate.add(element);
}
Set<Result*>* ResultHeader::get_values() { return getAGGREGATE(64, Set<Result*>*, 8); }
/*====================================================================================================
   Result
====================================================================================================*/
Node* Result::get_result_for() { return getInstance(0, Node*, 0); }
void Result::put_result_for(Node* v) { putInstance(0, Node*, v, result_for, 0, 8); v->addToUsedIn(c); }
ResultHeader* Result::get_result_header() { return getInstance(8, ResultHeader*, 1); }
void Result::put_result_header(ResultHeader* v) { putInstance(8, ResultHeader*, v, result_header, 1, 8); v->addToUsedIn(c); }
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
Set<Element*>* Node::get_elements() { return getAGGREGATE(32, Set<Element*>*, 4); }
Set<Result*>* Node::get_results() { return getAGGREGATE(40, Set<Result*>*, 5); }
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
int Mesh::get_maxNodeID() { return getATTRIBUTE(56, int, 7); }
void Mesh::put_maxNodeID(int v) { putATTRIBUTE(56, int, v, maxNodeID, 7, 0); }
int Mesh::get_minNodeID() { return getATTRIBUTE(64, int, 8); }
void Mesh::put_minNodeID(int v) { putATTRIBUTE(64, int, v, minNodeID, 8, 0); }
int Mesh::get_minElementID() { return getATTRIBUTE(72, int, 9); }
void Mesh::put_minElementID(int v) { putATTRIBUTE(72, int, v, minElementID, 9, 0); }
int Mesh::get_maxElementID() { return getATTRIBUTE(80, int, 10); }
void Mesh::put_maxElementID(int v) { putATTRIBUTE(80, int, v, maxElementID, 10, 0); }
/*====================================================================================================
   ScalarResult
====================================================================================================*/
double ScalarResult::get_val() { return getREAL(16, double, 2); }
void ScalarResult::put_val(double v) { putREAL(16, double, v, val, 2, 1); }
/*====================================================================================================
   VectorResult
====================================================================================================*/
List<REAL>* VectorResult::get_values() { return getAGGREGATE(16, List<REAL>*, 2); }
void VectorResult::put_values(List<REAL>* v) { putAGGREGATE(16, List<REAL>*, v, values, 2, 9); }
SdaiAggr  VectorResult::get_values_aggrId() { return getAGGRID(2); }
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
Array<REAL>* Matrix_2D::get_values() { return getAGGREGATE(16, Array<REAL>*, 2); }
void Matrix_2D::put_values(Array<REAL>* v) { putAGGREGATE(16, Array<REAL>*, v, values, 2, 9); }
SdaiAggr  Matrix_2D::get_values_aggrId() { return getAGGRID(2); }
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
Array<REAL>* Matrix_3D::get_values() { return getAGGREGATE(16, Array<REAL>*, 2); }
void Matrix_3D::put_values(Array<REAL>* v) { putAGGREGATE(16, Array<REAL>*, v, values, 2, 9); }
SdaiAggr  Matrix_3D::get_values_aggrId() { return getAGGRID(2); }
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
Array<REAL>* Matrix_Deformated::get_values() { return getAGGREGATE(16, Array<REAL>*, 2); }
void Matrix_Deformated::put_values(Array<REAL>* v) { putAGGREGATE(16, Array<REAL>*, v, values, 2, 9); }
SdaiAggr  Matrix_Deformated::get_values_aggrId() { return getAGGRID(2); }
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

   TimeStep* p_TimeStep;
   Analysis* p_Analysis;
   GaussPoint* p_GaussPoint;
   ResultHeader* p_ResultHeader;
   Result* p_Result;
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
         case et_TimeStep:
            p_TimeStep = new(ma) TimeStep(ma, instData);
            theObject = (void*)p_TimeStep;
            break;
         case et_Analysis:
            p_Analysis = new(ma) Analysis(ma, instData);
            theObject = (void*)p_Analysis;
            break;
         case et_GaussPoint:
            p_GaussPoint = new(ma) GaussPoint(ma, instData);
            theObject = (void*)p_GaussPoint;
            break;
         case et_ResultHeader:
            p_ResultHeader = new(ma) ResultHeader(ma, instData);
            theObject = (void*)p_ResultHeader;
            break;
         case et_Result:
            p_Result = new(ma) Result(ma, instData);
            theObject = (void*)p_Result;
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

static bool supertypeOf_TimeStep(entityType wantedSuperType, void **p);
static bool supertypeOf_Analysis(entityType wantedSuperType, void **p);
static bool supertypeOf_GaussPoint(entityType wantedSuperType, void **p);
static bool supertypeOf_ResultHeader(entityType wantedSuperType, void **p);
static bool supertypeOf_Result(entityType wantedSuperType, void **p);
static bool supertypeOf_Element(entityType wantedSuperType, void **p);
static bool supertypeOf_Node(entityType wantedSuperType, void **p);
static bool supertypeOf_Mesh(entityType wantedSuperType, void **p);
static bool supertypeOf_ScalarResult(entityType wantedSuperType, void **p);
static bool supertypeOf_VectorResult(entityType wantedSuperType, void **p);
static bool supertypeOf_Matrix_2D(entityType wantedSuperType, void **p);
static bool supertypeOf_Matrix_3D(entityType wantedSuperType, void **p);
static bool supertypeOf_Matrix_Deformated(entityType wantedSuperType, void **p);
static dbInstance *dbInstanceOf_TimeStep(void *obj) { TimeStep *p = (TimeStep*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_TimeStep(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_TimeStep;
}

static dbInstance *dbInstanceOf_Analysis(void *obj) { Analysis *p = (Analysis*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Analysis(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Analysis;
}

static dbInstance *dbInstanceOf_GaussPoint(void *obj) { GaussPoint *p = (GaussPoint*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_GaussPoint(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_GaussPoint;
}

static dbInstance *dbInstanceOf_ResultHeader(void *obj) { ResultHeader *p = (ResultHeader*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_ResultHeader(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_ResultHeader;
}

static dbInstance *dbInstanceOf_Result(void *obj) { Result *p = (Result*)obj; dbInstance *dbi = p; return dbi;}

static bool supertypeOf_Result(entityType wantedSuperType, void **)
{
   return wantedSuperType == et_Result;
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
   &supertypeOf_TimeStep,
   &supertypeOf_Analysis,
   &supertypeOf_GaussPoint,
   &supertypeOf_ResultHeader,
   &supertypeOf_Result,
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
   &dbInstanceOf_TimeStep,
   &dbInstanceOf_Analysis,
   &dbInstanceOf_GaussPoint,
   &dbInstanceOf_ResultHeader,
   &dbInstanceOf_Result,
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
