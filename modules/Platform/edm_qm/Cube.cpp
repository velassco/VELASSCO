#include "stdafx.h"
#include "Cube.h"


namespace EDMcube {

void CubeOnFile::defineDimension(EDMULONG dimensionNo, EDMULONG size)
{
   checkDimensionNo(dimensionNo);
   theCube->dimensionTable[dimensionNo].dimSize = size;
   theCube->dimensionTable[dimensionNo].indexValueType = sdaiINTEGER;
}
void CubeOnFile::defineDimension(EDMULONG dimensionNo, Container<double> *realDimension)
{
   checkDimensionNo(dimensionNo);
   theCube->dimensionTable[dimensionNo].dimSize = realDimension->size();
   theCube->dimensionTable[dimensionNo].indexValueType = sdaiREAL;
   double *dimValues = (double*)ma->alloc(sizeof(double) * realDimension->size());
   int i = 0;
   for (double *dp = realDimension->firstp(); realDimension->moreElements(); dp = realDimension->nextp()) {
      dimValues[i++] = *dp;
   }
   theCube->dimensionTable[dimensionNo].indexValues = (void*)dimValues;
}
void CubeOnFile::defineDimension(EDMULONG dimensionNo, Container<char*> *stringDimension)
{
   checkDimensionNo(dimensionNo);
   theCube->dimensionTable[dimensionNo].dimSize = stringDimension->size();
   theCube->dimensionTable[dimensionNo].indexValueType = sdaiSTRING;
   char **strings = (char**)ma->alloc(sizeof(char*) * stringDimension->size());
   int i = 0;
   for (char *cp = stringDimension->first(); cp; cp = stringDimension->next()) {
      strings[i++] = (char*)ma->allocString(cp);
   }
   theCube->dimensionTable[dimensionNo].indexValues = (char**)strings;

}
void CubeOnFile::createCubeOnFile(char *fileName, EDMULONG nOfDimensions, EDMULONG elementSize)
{
   if (nOfDimensions <= 3) {
      int cubeSize = sizeof(Cube) + sizeof(Dimension) * (nOfDimensions - 1);
      theCube = (Cube*)ma->allocZeroFilled(cubeSize);
   } else {
      THROW("Too many Dimensions.");
   }
}

void CubeOnFile::checkDimensionNo(EDMULONG dimensionNo)
{
   if (dimensionNo > nOfDimensions) THROW("Illegal dimension number");
}


}