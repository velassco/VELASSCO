
enum ModelType {mtFEM, mtDEM, mtAP209, mtUnknown};

/*===============================================================================================*/
class EDMmodelCache : public Model
{
public:
   ModelType                        type;
   virtual void                     initCache() {}

   EDMmodelCache::EDMmodelCache(Repository *r, CMemoryAllocator *_ma, dbSchema *_schema);
};

/*===============================================================================================*/
class FEMmodelCache : public EDMmodelCache
{
   CMemoryAllocator                 ma;
public:
   Iterator<fem::Element*, fem::entityType> elemIter();
   Iterator<fem::Node*, fem::entityType> nodeIter();
   Iterator<fem::Mesh*, fem::entityType> meshIter();
   void                             *voidElemBoxMatrix;
   double                           dx, dy, dz; // Lengt of box size in x, y, z direction;

   FEMmodelCache::FEMmodelCache(Repository *r, CMemoryAllocator *_ma, dbSchema *_schema);
   void                             initCache();
};