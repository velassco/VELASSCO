
enum ModelType {mtFEM, mtDEM, mtAP209, mtUnknown};

/*===============================================================================================*/
class EDMmodelCache : public Model
/*===============================================================================================*/
{
protected:
   CMemoryAllocator                 ma;
public:
   ModelType                        type;
   virtual void                     initCache() {}

   EDMmodelCache::EDMmodelCache(Repository *r, dbSchema *_schema);
};

/*===============================================================================================*/
class FEMmodelCache : public EDMmodelCache
/*===============================================================================================*/
{
public:
   void                             *voidElemBoxMatrix;
   double                           dx, dy, dz; // Lengt of box size in x, y, z direction;

   FEMmodelCache::FEMmodelCache(Repository *r, dbSchema *_schema);
   void                             initCache();
};

/*===============================================================================================*/
class DEMmodelCache : public EDMmodelCache
/*===============================================================================================*/
{
public:
   DEMmodelCache::DEMmodelCache(Repository *r, dbSchema *_schema);
   void                             initCache();
};