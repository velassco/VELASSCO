// EDMmodelCache.cpp :
//

#include "stdafx.h"
#include "..\..\..\..\EDM_Interface\EDM_interface.h"
#include "CLogger.h"
#include "EDMmodelCache.h"
#include "VELaSSCoHandler.h"
#include "Matrix.h"





void calculateBoundingBox(fem::Element *ep, BoundingBox *bb)
{
   Iterator<fem::Node*, fem::entityType> nodeIter(ep->get_nodes());
   fem::Node*np = nodeIter.first();
   if (np) {
      double max_x = np->get_x(), min_x = max_x;
      double max_y = np->get_y(), min_y = max_y;
      double max_z = np->get_z(), min_z = max_z;
      for (np = nodeIter.next(); np; np = nodeIter.next()) {
         double x = np->get_x(), y = np->get_y(), z = np->get_z();
         if (x < min_x) min_x = x;
         if (y < min_y) min_y = y;
         if (z < min_z) min_z = z;
         if (x > max_x) max_x = x;
         if (y > max_y) max_y = y;
         if (z > max_z) max_z = z;
      }
      bb->max_x = max_x; bb->max_y = max_y; bb->max_z = max_z; bb->min_x = min_x; bb->min_y = min_y; bb->min_z = min_z;
   } else {
      bb->max_x = 0.0; bb->max_y = 0.0; bb->max_z = 0.0; bb->min_x = 0.0; bb->min_y = 0.0; bb->min_z = 0.0;
   }
}


static int FEMclassesToRead[] = { (int)fem::et_Element, (int)fem::et_Mesh, (int)fem::et_Node, (int)fem::et_ResultHeader, 0 };


EDMmodelCache::EDMmodelCache(Repository *r, dbSchema *_schema)
: Model(r, &ma, _schema)
{
}


FEMmodelCache::FEMmodelCache(Repository *r, dbSchema *_schema)
: EDMmodelCache(r, _schema)
{
   type = mtFEM;
}

DEMmodelCache::DEMmodelCache(Repository *r, dbSchema *_schema)
: EDMmodelCache(r, _schema)
{
   type = mtDEM;
}

void DEMmodelCache::initCache()
{
   //defineObjectSet(dem::et_Particle, 0x4000, true);
   //defineObjectSet(dem::et_Particle_Geometry_contact, 0x4000, true);
   //defineObjectSet(dem::et_Particle_Particle_contact, 0x4000, true);
   //readObjectClassesToMemory(FEMclassesToRead);
   defineObjectSet(dem::et_Simulation, 0x4000, true);
   readAllObjectsToMemory();
   //Iterator<dem::Particle*, dem::entityType> pIter(getObjectSet(dem::et_Particle));
   //for (dem::Particle *p = pIter.first(); p; p = pIter.next()) {
   //   particles[p->get_id()] = p;
   //}
   //Iterator<dem::Particle_Geometry_contact*, dem::entityType> pgcIter(getObjectSet(dem::et_Particle_Geometry_contact));
   //for (dem::Particle_Geometry_contact *pgc = pgcIter.first(); pgc; pgc = pgcIter.next()) {
   //   pgContacts[pgc->get_id()] = pgc;
   //}
   //Iterator<dem::Particle_Particle_contact*, dem::entityType> ppcIter(getObjectSet(dem::et_Particle_Particle_contact));
   //for (dem::Particle_Particle_contact *ppc = ppcIter.first(); ppc; ppc = ppcIter.next()) {
   //   ppContacts[ppc->get_id()] = ppc;
   //}
}
void FEMmodelCache::initCache()
{
   defineObjectSet(fem::et_Element, 0x4000, true);
   defineObjectSet(fem::et_Mesh, 0x4000, true);
   defineObjectSet(fem::et_Node, 0x4000, true);
   defineObjectSet(fem::et_ResultHeader, 0x4000, true);
   //readObjectClassesToMemory(FEMclassesToRead);
   readAllObjectsToMemory();

   Iterator<fem::Element*, fem::entityType> elemIter(getObjectSet(fem::et_Element));
   Iterator<fem::Node*, fem::entityType> nodeIter(getObjectSet(fem::et_Node));

   SetInMem<fem::Element *> *elemsInThisBox;
   ma.init(0x100000);
   // elemsInThisBox = new(&ma)SetInMem<fem::Element*>(&ma, sdaiINTEGER, 100);



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
      double lx = max_x - min_x, ly = max_y - min_y, lz = max_z - min_z;
      EDMULONG nElem = elemIter.size();
      double dnx = cbrt((lx * lx * nElem) / (ly * lz));
      double dny = cbrt((ly * ly * nElem) / (lx * lz));
      double dnz = cbrt((lz * lz * nElem) / (ly * lx));
      double test = dnx * dny * dnz;
      int nx = dnx, ny = dny, nz = dnz;
      nx++; ny++; nz++;

      Matrix<Set<fem::Element*>*> *elemBoxMatrix = new(&ma) Matrix<Set<fem::Element*>*>(nx, ny, nz, sizeof(Set<fem::Element*>*), &ma);
      voidElemBoxMatrix = (void*)elemBoxMatrix;

      for (int ix = 0; ix < nx; ix++) {
         for (int iy = 0; iy < ny; iy++) {
            for (int iz = 0; iz < nz; iz++) {
               elemBoxMatrix->setElement(ix, iy, iz, new(&ma)Set<fem::Element*>(&ma, sdaiINSTANCE, 32));
            }
         }
      }

      dx = lx / dnx, dy = ly / dny, dz = lz / dnz;
      BoundingBox bb;
      int nElemInBox = 0;
      for (fem::Element *ep = elemIter.first(); ep; ep = elemIter.next()) {
         calculateBoundingBox(ep, &bb);
         int min_ix = (bb.min_x - min_x) / dx;
         int min_iy = (bb.min_y - min_y) / dy;
         int min_iz = (bb.min_z - min_z) / dz;
         int max_ix = (bb.max_x - min_x) / dx;
         int max_iy = (bb.max_y - min_y) / dy;
         int max_iz = (bb.max_z - min_z) / dz;
         int sdasd = 99;
         for (int ix = min_ix; ix <= max_ix; ix++) {
            for (int iy = min_iy; iy <= max_iy; iy++) {
               for (int iz = min_iz; iz <= max_iz; iz++) {
                  Set<fem::Element*> *elemBox = elemBoxMatrix->getElement(ix, iy, iz);
                  elemBox->add(ep, &ma);
                  nElemInBox++;
               }
            }
         }
      }
   }
}
/*===============================================================================================*/
void VELaSSCoHandler::InitQueryCaches()
/*
   
=================================================================================================*/
{
   char *repositoryName;
   SdaiBoolean userDefined;
   SdaiInteger  maxBufferSize = sizeof(SdaiInstance), index = 10, numberOfHits = 1;
   SdaiInstance resultBuffer[1], repositoryID, sdaiModelID;

   do {
      edmiSelectInstancesBN(edmiGetModelBN("SystemRepository", "ExpressDataManager"), "edm_model", NULL, 0,
         maxBufferSize, &index, &numberOfHits, resultBuffer);
      index++;
      if (numberOfHits > 0) {
         void *sp = sdaiGetAttrBN(resultBuffer[0], "Repository", sdaiINSTANCE, &repositoryID);
         if (sp) {
            sp = sdaiGetAttrBN(repositoryID, "USER_DEFINED", sdaiBOOLEAN, &userDefined);
            sp = sdaiGetAttrBN(repositoryID, "Name", sdaiSTRING, &repositoryName);
            if (sp) {
               if (strEQL(repositoryName, "DataRepository") || userDefined) {
                  sp = sdaiGetAttrBN(resultBuffer[0], "Sdai_Model", sdaiINSTANCE, &sdaiModelID);
                  if (sp) {
                     setCurrentModelCache(sdaiModelID);
                  }
               }
            }
         }
      }
   } while (numberOfHits > 0);
}

EDMmodelCache *VELaSSCoHandler::setCurrentModelCache(SdaiModel sdaiModelID)
{
   EDMmodelCache *emc = caches[sdaiModelID];
   if (emc == NULL) {
      ModelType mType = mtUnknown;

      char *schemaName, *modelName;
      SdaiInteger  maxBufferSize = sizeof(SdaiInstance), index = 0, numberOfHits = 1;
      SdaiInstance resultBuffer[2], schemaID, edmModelID;

      void *sp = sdaiGetAttrBN(sdaiModelID, "edm_model", sdaiINSTANCE, &edmModelID);
      if (sp) {
         sp = sdaiGetAttrBN(edmModelID, "schema", sdaiINSTANCE, &schemaID);
         if (sp) {
            sp = sdaiGetAttrBN(sdaiModelID, "name", sdaiSTRING, &modelName);
            if (sp) {
               sp = sdaiGetAttrBN(schemaID, "name", sdaiSTRING, &schemaName);
               int rk = sdaiErrorQuery();
               if (sp && strEQL(schemaName, "DEM_SCHEMA_VELASSCO")) {
                  mType = mtDEM;
               } else if (sp && strEQL(schemaName, "FEM_SCHEMA_VELASSCO")) {
                  mType = mtFEM;
               }
               if (mType == mtDEM) {
                  DEMmodelCache *dm = new DEMmodelCache(cDEMrep, &dem_schema_velassco_SchemaObject);
                  dm->open(modelName, sdaiRO); emc = dm;
                  dm->initCache(); caches[dm->modelId] = dm;
               } else if (mType == mtFEM) {
                  FEMmodelCache *fm = new FEMmodelCache(cFEMrep, &fem_schema_velassco_SchemaObject);
                  fm->open(modelName, sdaiRO); emc = fm;
                  fm->initCache(); caches[fm->modelId] = fm;
               }
            }
         }
      }
   }
   return emc;
}