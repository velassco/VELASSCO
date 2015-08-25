

#include "Matrix.h"


/*===============================================================================================*/
void Matrix<ElementType>::init(int xSize, int ySize, int zSize, int sizeInBytes)
/*
=================================================================================================*/
{
}
/*===============================================================================================*/
void Matrix::getDataIndex(unsigned int x, unsigned int y, unsigned int z)
/*
=================================================================================================*/
{
}
   ElementType       *data;
   int               nElement;
   int               dimTab[3];
   int               getDataIndex(unsigned int x, unsigned int y, unsigned int z);
   void              ;
public:
   Matrix::Matrix(int xSize, int ySize, int zSize) {
      dimTab[0] = xSize; dimTab[1] = ySize; dimTab[2] = zSize;
      nElement = xSize*ySize*zSize;
      int sizeInBytes = sizeof(ElementType)* nElement;
      data = (ElementType*)malloc(sizeInBytes);
      memset(data, 0, sizeInBytes);
   }
   ElementType getElement(unsigned int x, unsigned int y, unsigned int z) {
      if (x >= dimTab[0] || y >= dimTab[1] || z >= dimTab[2]) {
         THROW("Wrong matrix index.")
      }
      int i, Index = ix[0];

      for (i = 1; i < 3; i++)	Index += ix[i] * DimTab[i];
      return data[Index];
   }

};