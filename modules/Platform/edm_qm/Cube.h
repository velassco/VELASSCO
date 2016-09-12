
namespace EDMcube {

struct Dimension {
   EDMULONG             dimSize;
   SdaiPrimitiveType    indexValueType;
   void                 *indexValues;
} ;

struct Cube {
   EDMULONG             nOfDimensions;
   EDMULONG             elementSize;
   Dimension            dimensionTable[1];
} ;

/*===============================================================================================*/
template <typename ElementType>
class Matrix
/*
=================================================================================================*/
{
   ElementType             *data;
   EDMULONG                nElement;
   EDMULONG                dimTab[3];
   EDMULONG                getDataIndex(EDMULONG x, EDMULONG y, EDMULONG z) {
      if (x >= dimTab[0] || y >= dimTab[1] || z >= dimTab[2]) {
         THROW("Wrong matrix index.")
      }
      EDMULONG Index = x;
      Index += y * dimTab[1];
      Index += z * dimTab[2];
      return Index;
   }
   void              init(EDMULONG xSize, EDMULONG ySize, EDMULONG zSize, EDMULONG elementSize) {
      dimTab[0] = xSize; dimTab[1] = ySize; dimTab[2] = zSize;
      nElement = xSize*ySize*zSize;
   }
public:
   void* operator new(size_t sz, CMemoryAllocator *ma){ return ma->allocZeroFilled(sz); }
   Matrix::Matrix(EDMULONG xSize, EDMULONG ySize, EDMULONG zSize, EDMULONG elementSize, void *buffer) {
      //init(xSize, ySize, zSize, elementSize);
      //data = (ElementType*)buffer;
   }
   //Matrix::Matrix(EDMULONG xSize, EDMULONG ySize, EDMULONG zSize, EDMULONG elementSize, CMemoryAllocator *_ma) {
   //   init(xSize, ySize, zSize, elementSize);
   //   data = (ElementType*)ma->allocZeroFilled(elementSize * nElement);
   //}
   void setElement(EDMULONG x, EDMULONG y, EDMULONG z, ElementType elem) {
      data[getDataIndex(x, y, z)] = elem;
   }
   ElementType getElement(EDMULONG x, EDMULONG y, EDMULONG z) {
      return data[getDataIndex(x, y, z)];
   }

};



class CubeOnFile
{
   CMemoryAllocator     *ma;
   EDMULONG             nOfDimensions;
   Cube                 *theCube;
   void                 checkDimensionNo(EDMULONG dimensionNo);
public:
   CubeOnFile::CubeOnFile(CMemoryAllocator *_ma) { ma = _ma; }
   void createCubeOnFile(char *fileName, EDMULONG nOfDimensions, EDMULONG elementSize);
   void defineDimension(EDMULONG dimensionNo, EDMULONG size);
   void defineDimension(EDMULONG dimensionNo, Container<double> *realDimension);
   void defineDimension(EDMULONG dimensionNo, Container<char*> *stringDimension);
};

}