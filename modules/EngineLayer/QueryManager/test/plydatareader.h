#include "rply.h"
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <vector>

class PlyDataReaderDestructor;				//!< Forward Decleration

class PlyDataReader
{
  friend class PlyDataReaderDestructor;
private:
  static PlyDataReader* m_SingletonPtr;
  static PlyDataReaderDestructor m_DestructorObject;

  static p_ply m_PlyParser;

  static unsigned int m_nVertices;
  static unsigned int m_nFaces;

  static char* m_VertexDataPtr;
  static int m_CurrVertexByte;
  static char* m_IndexDataPtr;
  static int m_CurrIndexByte;

private:
  PlyDataReader();
  ~PlyDataReader();

public:
  static PlyDataReader* getSingletonPtr();
  void readDataInfo(const char* _pFileName, void* _pUserDataPtr, const long int& _pUserDataLen);

  unsigned int getNumVertices();
  unsigned int getNumFaces();

  void readData(void* _pVertexBuffer, void* _pIndexBuffer);
  static int VertexHandler(p_ply_argument _pArgument);
  static int FaceHandler(p_ply_argument _pArgument);
  static void releaseDataHandles();
  static void PlyParserMessageHandlerProc(p_ply _pPlyObj, const char* _pMessage);
  static int getTypeLength(e_ply_type _pType);
};

class PlyDataReaderDestructor
{
private:
  PlyDataReader* m_SingletonPtr;

public:
  PlyDataReaderDestructor()
  {
  }

  void setSingletonPointer(PlyDataReader* _pSingletonPtr)
  {
    m_SingletonPtr = _pSingletonPtr;
  }

  ~PlyDataReaderDestructor()
  {
    delete m_SingletonPtr;
  }
};

