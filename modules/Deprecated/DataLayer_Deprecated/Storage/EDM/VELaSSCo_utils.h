


/*===================================================================================================================*/
struct MergeJob
/*===================================================================================================================*/
{
   int                              nOfContainers;
   int                              nOfRemainingContainers;
   Container<EDMVD::Triangle>       *targetContainer;
   Container<EDMVD::Triangle>       **containers;
   Container<EDMVD::Triangle>       **initialContainers;
   EDMVD::Triangle                  **cTriangles;
   
   MergeJob(CMemoryAllocator *ma, int maxContainers, int containBufferSize);
   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->alloc(sz); }

   void                             addContainer(Container<EDMVD::Triangle> *s);
   void                             merge();
   void                             removeEmptyContainerFromMerge(int containerIndex);
};

