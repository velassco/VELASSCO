

/*===============================================================================================*/
template <typename ElementType>
class Matrix
   /*
   =================================================================================================*/
{
   ElementType       *data;
   int               nElement;
   unsigned int      dimTab[3];
   CMemoryAllocator  *ma;
   int               getDataIndex(unsigned int x, unsigned int y, unsigned int z) {
      if (x >= dimTab[0] || y >= dimTab[1] || z >= dimTab[2]) {
         THROW("Wrong matrix index.")
      }
      int Index = x;
      Index += y * dimTab[1];
      Index += z * dimTab[2];
      return Index;
   }
   void              init(int xSize, int ySize, int zSize, int elementSize) {
      dimTab[0] = xSize; dimTab[1] = ySize; dimTab[2] = zSize;
      nElement = xSize*ySize*zSize;
      data = (ElementType*)ma->allocZeroFilled(elementSize * nElement);
   }
public:
   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->allocZeroFilled(sz); }
   Matrix::Matrix(int xSize, int ySize, int zSize, int elementSize, CMemoryAllocator *_ma) {
      ma = _ma;
      init(xSize, ySize, zSize, elementSize);
   }
   void setElement(unsigned int x, unsigned int y, unsigned int z, ElementType elem) {
      data[getDataIndex(x, y, z)] = elem;
   }
   ElementType getElement(unsigned int x, unsigned int y, unsigned int z) {
      return data[getDataIndex(x, y, z)];
   }

};