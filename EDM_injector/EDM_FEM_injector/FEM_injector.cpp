
#include "stdafx.h"

#include "FEM_injector.h"


#define newObject(className) new(m)className(m)


void FEM_Injector::storeFEM_mesh(GID::MeshResultType &mesh)
{
   fem::Mesh *mp = newObject(fem::Mesh);
   for (std::vector<GID::MeshNodeType>::iterator nodeIter = mesh.nodes.begin(); nodeIter != mesh.nodes.end(); nodeIter++) {
      fem::Node *np = newObject(fem::Node);
      np->put_id(nodeIter->id); np->put_x(nodeIter->x); np->put_y(nodeIter->y); np->put_z(nodeIter->z);
      mp->put_nodes_element(np);
      nodeMap[nodeIter->id] = np;
   }
   for (std::vector<GID::MeshElementType>::iterator elemIter = mesh.elements.begin(); elemIter != mesh.elements.end(); elemIter++) {
      fem::Element *ep = newObject(fem::Element);
      std::vector<boost::long_long_type>::iterator nodesOfElement = elemIter->begin();
      ep->put_id(*nodesOfElement++);
      while (nodesOfElement != elemIter->end()) {
         fem::Node *np = nodeMap[*nodesOfElement++];
         if (np) ep->put_nodes_element(np);
      }
      mp->put_elements_element(ep);
   }

}


void FEM_Injector::storeFEM_results(GID::ResultContainerType &resultContainer)
{
   CMemoryAllocator *ma = m->getMemoryAllocator();

   for (std::vector<GID::ResultBlockType>::iterator resultIter = resultContainer.results.begin(); resultIter != resultContainer.results.end(); resultIter++) {
      fem::ResultHeader *rh = newObject(fem::ResultHeader);
      rh->put_name(ma->allocString((char*)resultIter->header.name.data()));
      rh->put_analysis(ma->allocString((char*)resultIter->header.analysis.data()));
      rh->put_gpName(ma->allocString((char*)resultIter->header.gpName.data()));
      rh->put_step(resultIter->header.step);
      rh->put_indexMData(resultIter->header.indexMData);
      rh->put_rType(resultIter->header.rType == GID::SCALAR ? ValueType_SCALAR : ValueType_VECTOR);
      rh->put_location(resultIter->header.location == GID::LOC_NODE ? LocationType_LOC_NODE : LocationType_LOC_GPOINT);
      fem::ResultBlock *rb = newObject(fem::ResultBlock);
      rb->put_header(rh);
      for (std::vector<GID::ResultRowType>::iterator rowIter = resultIter->values.begin(); rowIter != resultIter->values.end(); rowIter++) {
         std::vector<double>::iterator valIter = rowIter->values.begin();
         fem::Result *r;
         if (rowIter->values.size() == 1) {
            fem::ScalarResult *sr = newObject(fem::ScalarResult); sr->put_val(*valIter); r = sr;
         } else {
            fem::VectorResult *vr = newObject(fem::VectorResult); r = vr;
            while (valIter != rowIter->values.end()) {
               vr->put_values_element(*valIter); valIter++;
            }
         }
         fem::Node *n = nodeMap[rowIter->id];
         if (n) r->put_result_for(n);
         rb->put_values_element(r);
      }
   }
}


void FEM_Injector::writeAndClose()
{
   m->writeAllObjectsToDatabase();
   m->close();
   edmiCloseDatabase("");
}




/* -*- c++ -*- */

// point inside first tetrahedron of the VELaSSCo_HbaseBasicTest example
// is 0.034967938,0.689002752,0.025866667
// it is the centre of the first tetrahedron
// assuming numbering starts with 1, then:
// first tetrahedron ( id = 1) has nodes = 1794 1660 1802 1768
// in c/c++, numberations starts with 0, so nodes = 1793 1659 1801 1767
// coordinates of these nodes are:
//    1660 0.07139438390731812 0.7269042730331421 0.0
//    1768 0.0 0.683978796005249 0.10346666723489761
//    1794 0.06847736984491348 0.6451279520988464 0.0
//    1802 0.0 0.699999988079071 0.0

bool IsPointInsideTetrahedron(double p_in_x, double p_in_y, double p_in_z,
   double a_x, double a_y, double a_z,
   double b_x, double b_y, double b_z,
   double c_x, double c_y, double c_z,
   double d_x, double d_y, double d_z,
   double epsilon,
   double r_a, double r_b, double r_c, double r_d, double *dst)
{
   double r, s, t;
   r = s = t = 0;

   double da_x = a_x - d_x;
   double da_y = a_y - d_y;
   double da_z = a_z - d_z;
   double db_x = b_x - d_x;
   double db_y = b_y - d_y;
   double db_z = b_z - d_z;
   double dc_x = c_x - d_x;
   double dc_y = c_y - d_y;
   double dc_z = c_z - d_z;
   double dp_x = p_in_x - d_x;
   double dp_y = p_in_y - d_y;
   double dp_z = p_in_z - d_z;
   double r_denominator = da_x * (db_y * dc_z - dc_y * db_z) + db_x * (dc_y * da_z - da_y * dc_z) +  dc_x * (da_y * db_z - db_y * da_z);
   double r_numerator = dp_x * (db_y * dc_z - dc_y * db_z) + db_x * (dc_y * dp_z - dp_y * dc_z) + dc_x * (dp_y * db_z - db_y * dp_z);

   if (fabs(r_numerator) < epsilon)
      r_numerator = 0.0f;
   if (fabs(r_denominator) > epsilon)
      r = r_numerator / r_denominator;
   else
      r = 0.0f;
   if ((r < 0.0f) || (r > 1.0f))
      return false;

   double s_denominator = db_y * dc_z - dc_y * db_z;
   double s_numerator = dp_y * dc_z - dc_y * dp_z + r * (dc_y * da_z - da_y * dc_z);
   if (fabs(s_numerator) < epsilon)
      s_numerator = 0.0f;
   if (fabs(s_denominator) > epsilon)
      s = s_numerator / s_denominator;
   else {
      s_denominator = db_y * dc_x - dc_y * db_x;
      s_numerator = dp_y * dc_x - dc_y * dp_x + r * (dc_y * da_x - da_y * dc_x);
      if (fabs(s_denominator) > epsilon) {
         s = s_numerator / s_denominator;
      } else {
         s_denominator = db_x * dc_z - dc_x * db_z;
         s_numerator = dp_x * dc_z - dc_x * dp_z + r * (dc_x * da_z - da_x * dc_z);
         if (fabs(s_denominator) > epsilon) {
            s = s_numerator / s_denominator;
         } else {
            s = 0.0f;
         }
      }
   }
   if ((s < 0.0f) || (s > 1.0f))
      return false;

   double t_denominator = dc_z;
   double t_numerator = dp_z - r * da_z - s * db_z;
   if (fabs(t_numerator) < epsilon)
      t_numerator = 0.0f;
   if (fabs(t_denominator) > epsilon)
      t = t_numerator / t_denominator;
   else {
      t_denominator = dc_x;
      t_numerator = dp_x - r * da_x - s * db_x;
      if (fabs(t_denominator) > epsilon)
         t = t_numerator / t_denominator;
      else {
         t_denominator = dc_y;
         t_numerator = dp_y - r * da_y - s * db_y;
         if (fabs(t_denominator) > epsilon)
            t = t_numerator / t_denominator;
         else {
            t = 0.0f;
         }
      }
   }
   if ((t < 0.0f) || (t > 1.0f))
      return false;

   if (((r + s + t) >= -0.0) && ((r + s + t) <= 1.0f)) {
      double v = 1.0 - r - s - t;
      *dst = r_a * r + r_b * s + r_c * t + r_d * v;
      return true;
   } else {
      return false;
   }
}

double myRandGen_0_1()
{
   int r10000 = rand() % 10000;
   double r = r10000;
   return r / 10000.0;
}

static int classesToRead[] = { (int)fem::et_Element, (int)fem::et_Mesh, (int)fem::et_Node, 0 };

void FEM_Injector::buildFemSearchStructures(char *modelName)
{
   setCurrentModel(modelName);

   m->defineObjectSet(fem::et_Element, 0x4000, true);
   m->defineObjectSet(fem::et_Mesh, 0x4000, true);
   m->defineObjectSet(fem::et_Node, 0x4000, true);
   m->readObjectClassesToMemory(classesToRead);

   Iterator<fem::Element*, fem::entityType> elemIter(m->getObjectSet(fem::et_Element));
   Iterator<fem::Node*, fem::entityType> nodeIter(m->getObjectSet(fem::et_Node));
   Iterator<fem::Mesh*, fem::entityType> meshIter(m->getObjectSet(fem::et_Mesh));

   fem::Node *np = nodeIter.first();
   if (np) {
      double min_x = np->get_x(), max_x = min_x;
      double min_y = np->get_y(), max_y = min_y;
      double min_z = np->get_z(), max_z = min_z;

      for (np = nodeIter.next(); np; np = nodeIter.next()) {
         double x = np->get_x(), y = np->get_y(), z = np->get_z();
         if (x < min_x) min_x = x;
         if (y < min_y) min_y = y;
         if (z < min_z) min_z = z;
         if (x > max_x) max_x = x;
         if (y > max_y) max_y = y;
         if (z > max_z) max_z = z;
      }
      double dx = max_x - min_x, dy = max_y - min_y, dz = max_z - min_z;
      EDMULONG nElem = elemIter.size();
      double sx = cbrt((dx * dx * nElem) / (dy * dz));
      double sy = cbrt((dy * dy * nElem) / (dx * dz));
      double sz = cbrt((dz * dz * nElem) / (dy * dx));
      double test = sx * sy * sz;
      test = 0;


   }

   time_t start = time(NULL);
   int nOutside = 0, nInside = 0, nPoint = 1000;
   FILE *pFile = fopen("pfile.txt", "w");

   for (int i = 0; i < nPoint; i++) {
      double p_in_x = myRandGen_0_1(), p_in_y = myRandGen_0_1(), p_in_z = myRandGen_0_1();
      //fprintf(pFile, "{%10f, %10f, %10f},\n", p_in_x, p_in_y, p_in_z);
      int nNode = 0, nElem = 0;
      bool outsideAll = true;
      for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
         nElem++;
         int elemId = ep->get_id();

         Iterator<fem::Node*, fem::entityType> nodeOfElemIter(ep->get_nodes());
         if (nodeOfElemIter.size() == 4) {
            nNode += 4;
            fem::Node *np = nodeOfElemIter.first(); 
            double a_x = np->get_x(), a_y = np->get_y(), a_z = np->get_z(); np = nodeOfElemIter.next();
            double b_x = np->get_x(), b_y = np->get_y(), b_z = np->get_z(); np = nodeOfElemIter.next();
            double c_x = np->get_x(), c_y = np->get_y(), c_z = np->get_z(); np = nodeOfElemIter.next();
            double d_x = np->get_x(), d_y = np->get_y(), d_z = np->get_z(); np = nodeOfElemIter.next();
            double dst, epsilon = 0.000;
            double r_a = 0.0, r_b = 0.0, r_c = 0.0, r_d = 0.0;
            if (IsPointInsideTetrahedron(p_in_x, p_in_y, p_in_z, a_x, a_y, a_z, b_x, b_y, b_z, c_x, c_y, c_z, d_x, d_y, d_z,
               epsilon, r_a, r_b, r_c, r_d, &dst)) {
               if ((i % 10) == 0) {
                  fprintf(pFile, "\n   ");
               }
               int elemId = ep->get_id();
               int adf = 9999; nInside++;
               outsideAll = false;
               fprintf(pFile, "%8d, ", elemId);
            }
         }
      }
      if (outsideAll) {
         nOutside++;
      }
   }
   time_t ferdig = time(NULL);
   fclose(pFile);
   printf("Time used in seconds: %d\nNumber of points tested: %d\nNumber of hit inside a tetraheder: %d\nNumber of points outside all tetraheders: %d\n",
      ferdig - start, nPoint, nInside, nOutside);
}