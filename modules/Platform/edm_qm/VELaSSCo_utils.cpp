
#include "stdafx.h"
#include "../edm_plugin/EDM_plugin_1.h"
#include <omp.h>
#include "VELaSSCo_utils.h"


/*===================================================================================================================*/
MergeJob::MergeJob(CMemoryAllocator *ma, int maxContainers, int containerBufferSize)
/*===================================================================================================================*/
{
   nOfContainers = 0;
   targetContainer = new(ma)Container<EDMVD::Triangle>(ma, containerBufferSize);
   containers = (Container<EDMVD::Triangle>**)ma->alloc(sizeof(Container<EDMVD::Triangle>*) * maxContainers);
   initialContainers = (Container<EDMVD::Triangle>**)ma->alloc(sizeof(Container<EDMVD::Triangle>*) * maxContainers);
   cTriangles = (EDMVD::Triangle**)ma->alloc(sizeof(EDMVD::Triangle*) * maxContainers);
}

/*===================================================================================================================*/
void MergeJob::addContainer(Container<EDMVD::Triangle> *s)
/*===================================================================================================================*/
{
   containers[nOfContainers] = s; initialContainers[nOfContainers++] = s;
}
/*===================================================================================================================*/
void MergeJob::merge()
/*===================================================================================================================*/
{
   EDMULONG tot_triangles, n_skipped_triangles;
   tot_triangles = n_skipped_triangles = 0;
   nOfRemainingContainers = nOfContainers;
   for (int i = 0; i < nOfRemainingContainers; i++) {
      cTriangles[i] = containers[i]->firstp(); tot_triangles += containers[i]->size();
   }
   EDMVD::Triangle *cSmallest = cTriangles[0];
   int indexOfSmallest = 0;
   while (nOfRemainingContainers > 1) {
      for (int i = 0; i < nOfRemainingContainers; i++) {
         if (i != indexOfSmallest) {
            int comparison = cSmallest->conpare(cTriangles[i]);
            if (comparison == 0) {
               // Skip both 
               cTriangles[i] = containers[i]->nextp();
               if (cTriangles[i] == NULL) removeEmptyContainerFromMerge(i);
               cTriangles[indexOfSmallest] = containers[indexOfSmallest]->nextp();
               if (cTriangles[indexOfSmallest] == NULL) removeEmptyContainerFromMerge(indexOfSmallest);
               n_skipped_triangles++;
               cSmallest = NULL; break;
            } else if (comparison > 0) {
               indexOfSmallest = i; cSmallest = cTriangles[i];
            }
         }
      }
      if (cSmallest) {
         *targetContainer->createNext() = *cSmallest;
         cTriangles[indexOfSmallest] = containers[indexOfSmallest]->nextp();
         if (cTriangles[indexOfSmallest] == NULL) removeEmptyContainerFromMerge(indexOfSmallest);
      }
      cSmallest = cTriangles[0]; indexOfSmallest = 0;
   }
   Container<EDMVD::Triangle> *theLastContainer = containers[0];
   for (EDMVD::Triangle *t = cTriangles[0]; t; t = theLastContainer->nextp()) {
      *targetContainer->createNext() = *t;
   }
   EDMULONG diff = tot_triangles - (targetContainer->size() + n_skipped_triangles);
}
/*===================================================================================================================*/
void MergeJob::removeEmptyContainerFromMerge(int containerIndex)
/*===================================================================================================================*/
{
   nOfRemainingContainers--;
   for (int i = containerIndex; i < nOfRemainingContainers; i++) {
      cTriangles[i] = cTriangles[i + 1];
      containers[i] = containers[i + 1];
   }
}
