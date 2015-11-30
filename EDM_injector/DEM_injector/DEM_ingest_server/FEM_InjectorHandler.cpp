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

double toDouble(char *s)
{
   double d;
   sscanf(s, "%lf", &d); return d;
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
   char command[256], resultName[256], analysisName[256], resultType[256], Location[256], GP_location[256];
   EDMULONG dimension, elementID, nodeID;
   double stepValue;
   double nID[11];
   
   readNextLine();
   if (strEQL(line, "GiD Post Results File 1.0\n")) {
      while (readNextLine() > 0) {
         if (line[0] == '#' || line[0] == '\n') {
         } else if (strnEQL(line, "Result", 6)) {
            //int nColumns = sscanf(line, "%s %s %s %lf %s %s %s", command, resultName, analysisName, &stepValue, resultType, Location, GP_location);
            int nColumns = scanInputLine();
            if (nColumns == 6 || nColumns == 7) {
               cResultHeader = newObject(fem::ResultHeader); cResultHeader->put_name(col[1]); cResultHeader->put_analysis(col[2]);
               cResultHeader->put_step(toDouble(col[3]));
               cResultBlock = newObject(fem::ResultBlock); cResultBlock->put_header(cResultHeader);
               readNextLine();
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
                  fem::Node *n = nodes[nodeID];
                  if (n) {
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
               while (bytesRead && strNEQ(line, "end coordinates\n")) {
                  nColumns = sscanf(line, "%llu %f %f %f", &nodeID, &x, &y, &z);
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

   for (i = 0; i < MAX_COLUMNS && *bp; i++) {
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
