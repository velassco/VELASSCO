// DEM_ingest_server.cpp : Defines the entry point for the console application.

#include "stdafx.h"

/*===================================================================================================================================================

   GiD ascii format for mesh and results files is explained at
   http://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files

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
   for order of nodes check http://www.gidhome.com/component/manual/customizationmanual/postprocess_data_files/mesh_format-_modelnamepostmsh

   http://www.gidhome.com/documents/customizationmanual/POSTPROCESS%20DATA%20FILES/Mesh%20format:%20ModelName.post.msh

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


//using namespace  ::DEM_Inject;

#include "FEM_InjectorHandler.h"


#define newObject(className) new(m)className(m)

double toDouble(char *s)
{
   double d = strtod(s, NULL);
   return d;
}
EDMULONG toEDMULONG(char *s)
{
   EDMULONG ul;
   sscanf(s, "%llu", &ul); return ul;
}

void FEM_InjectorHandler::DeleteCurrentModelContent()
{
   CHECK(edmiDeleteModelContents(m->modelId));
}

void FEM_InjectorHandler::InitiateFileInjection()
{
   //theSimulation = newObject(dem::Simulation);
   //theSimulation->put_name("theSimulation");
}

///*=============================================================================================================================*/
//fem::Analysis *FEM_InjectorHandler::getAnalysis(char *name)
///*=============================================================================================================================*/
//{
//   //if (cAnalysis && strEQL(cAnalysis->get_name(), name)) {
//   //   return cAnalysis;
//   //} else {
//      SdaiAggr agrID = m->getObjectSet(fem::et_Analysis);
//      Iterator<fem::Analysis*, fem::entityType> anIter(agrID, m);
//      for (fem::Analysis *a = anIter.first(); a; a = anIter.next()) {
//         if (strEQL(a->get_name(), name)) {
//            return a;
//         }
//      }
//   }
//   return NULL;
//}
///*=============================================================================================================================*/
//fem::TimeStep *FEM_InjectorHandler::getTimeStep(fem::Analysis *a, double tstep)
///*=============================================================================================================================*/
//{
//   Iterator<fem::TimeStep*, fem::entityType> tsIter(a->get_time_steps(), m);
//   for (fem::TimeStep *ts = tsIter.first(); ts; ts = tsIter.next()) {
//      if (ts->get_time_value() == tstep) {
//         return ts;
//      }
//   }
//   return NULL;
//}

//ResultType--> type of result, one of : SCalar, Vector, Matrix, PlainDeformationMatrix, MainMatrix, LocalAxes, ComplexScalar, ComplexVector
/*=============================================================================================================================*/
void FEM_InjectorHandler::InjectResultFile()
/*
   GiD Post Results File 1.0

   # encoding utf-8
   Result "PartitionId" "geometry" 1 Scalar OnNodes
   Values
   6 3
   14 3
===============================================================================================================================*/
{
   EDMULONG nodeID;
   double nID[11];
   SdaiAggr agrID = m->getObjectSet(fem::et_Analysis);
   Iterator<fem::Analysis*, fem::entityType> anIter(agrID, m);
   Iterator<fem::TimeStep*, fem::entityType> tsIter(agrID, m);
   EDMLONG64 nodetsId;

   readNextLine();
   if (strEQL(line, "GiD Post Results File 1.0\n")) {
      while (readNextLine() > 0) {
         if (line[0] == '#' || line[0] == '\n') {
         } else if (strnEQL(line, "Result", 6)) {
            //int nColumns = sscanf(line, "%s %s %s %lf %s %s %s", command, resultName, analysisName, &stepValue, resultType, Location, GP_location);
            int nColumns = scanInputLine();
            if (nColumns == 6 || nColumns == 7) {
               int nResultTypeErrors = 0, nLocationTypeErrors = 0;
               cResultHeader = newObject(fem::ResultHeader); cResultHeader->put_name(col[1]); cResultHeader->put_analysis(col[2]);
               if (newResultName(col[1])) {
                  resultNames->add(model_ma.allocString(col[1]));
               }
               
               //typedef enum {ValueType_SCALAR, ValueType_VECTOR, ValueType_MATRIX, ValueType_PLAINDEFORMATIONMATRIX, ValueType_MAINMATRIX, ValueType_LOCALAXES, ValueType_COMPLEXSCALAR, ValueType_COMPLEXVECTOR} ValueType;
               if (strEQL(col[4], "Scalar")) {
                  cResultHeader->put_rType(ValueType_SCALAR);
               } else if (strEQL(col[4], "Vector")) {
                  cResultHeader->put_rType(ValueType_VECTOR);
               } else if (nResultTypeErrors++ < 5) {
                  printf("\nIllegal result type: %s\n", col[4]);
               }
               //typedef enum { LocationType_ONNODES, LocationType_ONGAUSSPOINTS } LocationType; 
               if (strEQL(col[5], "OnNodes\n")) {
                  cResultHeader->put_location(LocationType_ONNODES);
               } else if (strEQL(col[5], "OnGaussPoints\n")) {
                  cResultHeader->put_location(LocationType_ONGAUSSPOINTS);
               } else if (nLocationTypeErrors++ < 5) {
                  printf("\nIllegal location type: %s\n", col[4]);
               }

               if (cAnalysis && strNEQ(cAnalysis->get_name(), col[2])) {
                  for (cAnalysis = anIter.first(); cAnalysis; cAnalysis = anIter.next()) {
                     if (strEQL(cAnalysis->get_name(), col[2])) break;
                  }
               }
               if (cAnalysis == NULL) {
                  cAnalysis = newObject(fem::Analysis); cAnalysis->put_name(col[2]);
               }
               double thisTimeStep = toDouble(col[3]);
               cResultHeader->put_step(thisTimeStep);
               
               if (cTimeStep && cTimeStep->get_time_value() != thisTimeStep) {
                  tsIter.init(cAnalysis->get_time_steps(), m);
                  for (cTimeStep = tsIter.first(); cTimeStep; cTimeStep = tsIter.next()) {
                     if (cTimeStep->get_time_value() == thisTimeStep) break;
                  }
               }
               if (cTimeStep == NULL) {
                  cTimeStep = newObject(fem::TimeStep); cTimeStep->put_time_value(thisTimeStep); cTimeStep->put_id(++cTimestepId);
                  if (cMesh) cTimeStep->put_mesh(cMesh);
                  if (cAnalysis) cTimeStep->put_analysis(cAnalysis);
               }
               cTimeStep->put_results_element(cResultHeader);
               readNextLine();
               if (strnEQL(line, "ComponentNames", 14)) {
                  int nComponentNames = scanInputLine();
                  for (int i = 1; i < nComponentNames; i++) {
                     if (strNEQ(col[i], ",")) {
                        cResultHeader->put_compName_element(col[i]);
                     }
                  }
               }
               if (strNEQ(line, "Values\n")) {
                  readNextLine();
               }
               if (strEQL(line, "Values\n")) readNextLine();
               while (bytesRead && strNEQ(line, "End values\n")) {
                  int nColumns = sscanf(line, "%llu %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                     &nodeID, &nID[0], &nID[1], &nID[2], &nID[3], &nID[4], &nID[5], &nID[6], &nID[7], &nID[8], &nID[9], &nID[10]);
                  fem::Result *r;
                  if (nColumns == 2) {
                     fem::ScalarResult *sr = newObject(fem::ScalarResult); sr->put_val(nID[0]); r = sr;
                  } else {
                     fem::VectorResult *vr = newObject(fem::VectorResult); r = vr;
                     for (int i = 0; i < nColumns - 1; i++) {
                        vr->put_values_element(nID[i]);
                     }
                  }
                  r->put_result_header(cResultHeader);
                  fem::Node *n = nodes[nodeID];
                  if (n) {
                     nodetsId = n->get_id();
                     nodetsId << 32;
                     nodetsId += cTimestepId;
                     r->put_NodeTimestepId(nodetsId);
                     r->put_result_for(n);
                  } else {
                     printError("Unknown node id in result line.");
                  }
                  readNextLine();
               }
            } else {
               printError("Illegal number of colums in result FEM file.");
            }
         }
      }
   } else {
      printError("FEM result file does not start with \"GiD Post Results File 1.0\"");
   }
}

/*=============================================================================================================================*/
void FEM_InjectorHandler::AnalyzeResultFile()
/*
===============================================================================================================================*/
{
   readNextLine();
   if (strEQL(line, "GiD Post Results File 1.0\n")) {
      while (readNextLine() > 0) {
         if (line[0] == '#' || line[0] == '\n') {
         } else if (strnEQL(line, "Result", 6)) {
            //int nColumns = sscanf(line, "%s %s %s %lf %s %s %s", command, resultName, analysisName, &stepValue, resultType, Location, GP_location);
            int nColumns = scanInputLine();
            if (nColumns == 6 || nColumns == 7) {
               int nResultTypeErrors = 0, nLocationTypeErrors = 0;
               if (newResultName(col[1])) {
                  resultNames->add(plugin_ma->allocString(col[1]));
               }
               double thisTimeStep = toDouble(col[3]);
               if (newTimestep(thisTimeStep)) {
                  temesteps->add(thisTimeStep);
                  ++cTimestepId;
               }
               readNextLine();
               if (strNEQ(line, "Values\n")) {
                  readNextLine();
               }
               if (strEQL(line, "Values\n")) readNextLine();
               while (bytesRead && strNEQ(line, "End values\n")) {
                  readNextLine();
               }
            } else {
               printError("Illegal number of colums in result FEM file.");
            }
         }
      }
   } else {
      printError("FEM result file does not start with \"GiD Post Results File 1.0\"");
   }
}


static struct {
   fem::ElementType     elementType;
   char                 *elementTypeName;
} elementTypes[] = {
   { fem::ElementType_LINE, "Line" },
   { fem::ElementType_TETRAHEDRA, "Tetrahedra" },
   { fem::ElementType_TRIANGLE, "Triangle" },
   { fem::ElementType_SPHERE, "Sphere" },
   { fem::ElementType_SPHERE, NULL },
};

//ElemType--> one of Point, Line, Triangle, Quadrilateral, Tetrahedra, Hexahedra, Prism, Pyramid, Sphere, Circle
/*=============================================================================================================================*/
void FEM_InjectorHandler::InjectMeshFile()
/*
   # encoding utf - 8
   MESH "Part" dimension 3 ElemType Tetrahedra Nnode 4
   # Color 0.600000 0.600000 0.600000
   Coordinates
   37 0.27317816 0.95038164 0.96116579
   100 0.2 1 1
   101 0.30000001 1 1
===============================================================================================================================*/
{
   //char command[256], meshName[256], dim[256], et[256], elemType[256], nn[256];
   EDMULONG nID[11], dimension, elementID, nodeID;
   double x, y, z;
   

   while (readNextLine() > 0) {
      if (line[0] == '#' || line[0] == '\n') {
      } else if (strnEQL(line, "MESH", 4)) {
         //int nColumns = sscanf(line, "%s %s %s %llu %s %s %s %llu", command, meshName, dim, &dimension, et, elemType, nn, &nNodes);
         int nColumns = scanInputLine();
         if (nColumns == 8) {
            nNodes = toEDMULONG(col[7]); dimension = toEDMULONG(col[3]);
            cMesh = newObject(fem::Mesh); cMesh->put_name(col[1]);  cMesh->put_numberOfNodes(nNodes);  cMesh->put_dimension(dimension);
            int i;
            for (i = 0; elementTypes[i].elementTypeName && strNEQ(col[5], elementTypes[i].elementTypeName); i++);
            if (elementTypes[i].elementTypeName) {
               cMesh->put_elementType(elementTypes[i].elementType);
            } else {
               printError("Illegal element type in file.");
            }
            readNextLine();
            if (line[0] == '#') readNextLine();
            if (strEQL(line, "Coordinates\n")) {
               readNextLine();
               while (bytesRead && strNEQ(line, "end coordinates\n")) {
                  nColumns = sscanf(line, "%llu %lf %lf %lf", &nodeID, &x, &y, &z);
                  fem::Node *n = newObject(fem::Node); n->put_id(nodeID); n->put_x(x); n->put_y(y); n->put_z(z);
                  cMesh->put_nodes_element(n); nodes[nodeID] = n;
                  readNextLine();
               }
            }
         } else {
            printError("Illegal number of colums in file.");
         }
      } else if (strEQL(line, "Elements\n")) {
         readNextLine();
         if (line[0] == '#') readNextLine();
         while (bytesRead && strNEQ(line, "end elements\n")) {
            int nColumns = sscanf(line, "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu ",
               &elementID, &nID[0], &nID[1], &nID[2], &nID[3], &nID[4], &nID[5], &nID[6], &nID[7], &nID[8], &nID[9], &nID[10]);
            if (nColumns > nNodes) {
               fem::Element *elem = newObject(fem::Element); elem->put_id(elementID);
               cMesh->put_elements_element(elem);
               for (int i = 0; i < nNodes; i++) {
                  fem::Node *n = nodes[nID[i]];
                  if (n) {
                     elem->put_nodes_element(n);
                  } else {
                     printError("Unknown node id in element.");
                  }
               }
            } else {
               printError("Illegal number of nodes in element.");
            }
            readNextLine();
         }
      }
   }
}




/*=============================================================================================================================*/
void FEM_InjectorHandler::AnalyzeMeshFile()
/*
===============================================================================================================================*/
{
   //char command[256], meshName[256], dim[256], et[256], elemType[256], nn[256];
   EDMULONG nID[11], dimension, elementID, nodeID;
   double x, y, z;
   

   while (readNextLine() > 0) {
      if (line[0] == '#' || line[0] == '\n') {
      } else if (strnEQL(line, "MESH", 4)) {
         int nColumns = scanInputLine();
         if (nColumns == 8) {
            nNodes = toEDMULONG(col[7]); dimension = toEDMULONG(col[3]);
            readNextLine();
            if (line[0] == '#') readNextLine();
            if (strEQL(line, "Coordinates\n")) {
               readNextLine();
               while (bytesRead && strNEQ(line, "end coordinates\n")) {
                  nColumns = sscanf(line, "%llu %lf %lf %lf", &nodeID, &x, &y, &z);
                  if (nodeID > maxNodeId) {
                     maxNodeId = nodeID;
                  }
                  readNextLine();
               }
            }
         } else {
            printError("Illegal number of colums in file.");
         }
      } else if (strEQL(line, "Elements\n")) {
         readNextLine();
         if (line[0] == '#') readNextLine();
         while (bytesRead && strNEQ(line, "end elements\n")) {
            int nColumns = sscanf(line, "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu ",
               &elementID, &nID[0], &nID[1], &nID[2], &nID[3], &nID[4], &nID[5], &nID[6], &nID[7], &nID[8], &nID[9], &nID[10]);
            if (nColumns > nNodes) {
               if (elementID > maxElementId) {
                  maxElementId = elementID;
               }
            } else {
               printError("Illegal number of nodes in element.");
            }
            readNextLine();
         }
      }
   }
}

//# encoding utf - 8
//MESH "Part" dimension 3 ElemType Tetrahedra Nnode 4
//# Color 0.600000 0.600000 0.600000
//Coordinates
//37 0.27317816 0.95038164 0.96116579
//100 0.2 1 1
//101 0.30000001 1 1




char *FEM_InjectorHandler::readNextLine()
{
   cLineno++; bytesRead = fgets(line, sizeof(line), fp);
   return bytesRead;
}

void FEM_InjectorHandler::flushObjectsAndClose()
{
   //m->writeAllObjectsToDatabase();
   m->close();
}

int FEM_InjectorHandler::scanInputLine()
{
   char *bp = line;
   int i;
   char stopChar;

   for (i = 0; i < MAX_COLUMNS && *bp && *bp != '\n'; i++) {
      while (*bp == ' ') bp++;
      if (*bp == '\"') {
         stopChar = '\"'; bp++;
      } else {
         stopChar = ' ';
      }
      col[i] = bp;
      while (*bp && *bp != stopChar) bp++;
      if (*bp == stopChar) *bp++ = 0;
   }
   return i;
}

void FEM_InjectorHandler::initAnalyze(CMemoryAllocator *ma)
{
   plugin_ma = ma;
   temesteps = new(&model_ma)Container<double>(ma);
}

bool FEM_InjectorHandler::newResultName(char *rn)
{
   for (char *crn = resultNames->first(); crn; crn = resultNames->next()) {
      if (strEQL(crn, rn)) return false;
   }
   return true;
}

bool FEM_InjectorHandler::newTimestep(double ts)
{
   for (double cts = temesteps->first(); temesteps->moreElements(); cts = temesteps->next()) {
      if (cts == ts) return false;
   }
   return true;
}

/*===============================================================================================*/
void FEM_InjectorHandler::setCurrentModel(const char *modelName)
/*===============================================================================================*/
{
   if (m == NULL) {
      m = new Model(currentRepository, &model_ma, currentSchema);
      EdmiError rstat = edmiDeleteModelBN(currentRepository->getRepositoryId(), (char*)modelName);
      SdaiModel modelID = edmiCreateModelBN(currentRepository->getRepositoryId(), (char*)modelName,
         currentSchemaName, NO_INSTANCE_REFERENCES | M_PACKED_MODEL);
      m->open((char*)modelName, sdaiRW);
   }
   CHECK(edmiStartModelWriteTransaction(m->modelId));
}