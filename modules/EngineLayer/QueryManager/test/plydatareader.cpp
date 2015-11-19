#include "plydatareader.h"

#include <iostream>

char* PlyDataReader::m_VertexDataPtr = 0;
int PlyDataReader::m_CurrVertexByte = 0;
unsigned int PlyDataReader::m_nVertices = 0;
unsigned int PlyDataReader::m_nFaces = 0;

char* PlyDataReader::m_IndexDataPtr = 0;
int PlyDataReader::m_CurrIndexByte = 0;

PlyDataReader* PlyDataReader::m_SingletonPtr = 0;
PlyDataReaderDestructor PlyDataReader::m_DestructorObject;
p_ply PlyDataReader::m_PlyParser;

PlyDataReader::PlyDataReader()
{
}

PlyDataReader::~PlyDataReader()
{
}

PlyDataReader* PlyDataReader::getSingletonPtr()
{
  if (m_SingletonPtr == 0)
  {
    m_SingletonPtr = new PlyDataReader;
    m_DestructorObject.setSingletonPointer(m_SingletonPtr);
  }

  return m_SingletonPtr;
}

void PlyDataReader::readDataInfo(const char* _pFileName, void* _pUserDataPtr, const long int& _pUserDataLen)
{
  // Open the PLY file
  m_PlyParser = ply_open(_pFileName, PlyParserMessageHandlerProc, _pUserDataLen, _pUserDataPtr);

  /**  Read the header of the ply which shows that which data is offered by PLY file. For example, the vertices has which format and how many vertices
  * exists. The same information for other elements (Faces, Normal Vectors, ...) of the PLY object.
  */
  ply_read_header(m_PlyParser);

  p_ply_element CurrElement;
  CurrElement = ply_get_next_element(m_PlyParser, NULL);

  while (CurrElement != 0)
  {
    long nElements;
    const char* ElementName;

    ply_get_element_info(CurrElement, ((const char**)&ElementName), &nElements);

    if (!strcmp(ElementName, "vertex"))
    {
      this->m_nVertices = nElements;
    }
    else if (!strcmp(ElementName, "face"))
    {
      this->m_nFaces = nElements;
    }

    CurrElement = ply_get_next_element(m_PlyParser, CurrElement);

    ElementName = 0;
  }
}

unsigned int PlyDataReader::getNumVertices()
{
  return m_nVertices;
}

unsigned int PlyDataReader::getNumFaces()
{
  return m_nFaces;
}

void PlyDataReader::readData(void* _pVertexBuffer, void* _pIndexBuffer)
{
  m_VertexDataPtr = (char*)_pVertexBuffer;
  m_IndexDataPtr = (char*)_pIndexBuffer;

  ply_set_read_cb(m_PlyParser, "vertex", "x", VertexHandler, 0, 0);
  ply_set_read_cb(m_PlyParser, "vertex", "y", VertexHandler, 0, 0);
  ply_set_read_cb(m_PlyParser, "vertex", "z", VertexHandler, 0, 0);
  ply_set_read_cb(m_PlyParser, "vertex", "nx", VertexHandler, 0, 0);
  ply_set_read_cb(m_PlyParser, "vertex", "ny", VertexHandler, 0, 0);
  ply_set_read_cb(m_PlyParser, "vertex", "nz", VertexHandler, 0, 0);

  ply_set_read_cb(m_PlyParser, "face", "vertex_indices", FaceHandler, 0, 0);

  ply_read(m_PlyParser);
}

int PlyDataReader::VertexHandler(p_ply_argument _pArgument)
{
  p_ply_property Property;
  const char* PropertyName;
  e_ply_type PropertyType;
  e_ply_type PropertyValueType;

  float Value = (float)ply_get_argument_value(_pArgument);

  ply_get_argument_property(_pArgument, &Property, 0, 0);
  ply_get_property_info(Property, &PropertyName, &PropertyType, 0, &PropertyValueType);

  int WriteLength = getTypeLength(PropertyType);
  memcpy(m_VertexDataPtr + m_CurrVertexByte, &Value, WriteLength);
  m_CurrVertexByte += WriteLength;

  return 1;
}

int PlyDataReader::FaceHandler(p_ply_argument _pArgument)
{
  p_ply_property Property;

  unsigned int Value = (unsigned int)ply_get_argument_value(_pArgument);
  long Value_Index = -1;

  ply_get_argument_property(_pArgument, &Property, 0, &Value_Index);

  if (Value_Index >= 0)
  {
    int WriteLength = 4;
    memcpy(m_IndexDataPtr + m_CurrIndexByte, &Value, sizeof(unsigned int));
    m_CurrIndexByte += WriteLength;
  }

  return 1;
}

void PlyDataReader::releaseDataHandles()
{
  //m_VertexDataPtr = 0;
  m_CurrVertexByte = 0;


  //m_IndexDataPtr = 0;
  m_CurrIndexByte = 0;
}

void PlyDataReader::PlyParserMessageHandlerProc(p_ply _pPlyObj, const char* _pMessage)
{
  std::cout << "===> PLY Parser Message: " << _pMessage << std::endl;
}

int PlyDataReader::getTypeLength(e_ply_type _pType)
{
  switch (_pType)
  {
  case PLY_CHAR:
    return 1;
    break;

  case PLY_DOUBLE:
    return 8;
    break;
  case PLY_FLOAT:
    return 4;
    break;

  case PLY_FLOAT32:
    return 4;
    break;

  case PLY_FLOAT64:
    return 8;
    break;

  case PLY_INT:
    return 4;
    break;

  case PLY_INT8:
    return 1;
    break;

  case PLY_INT16:
    return 2;
    break;

  case PLY_INT32:
    return 4;
    break;

  case PLY_SHORT:
    return 2;
    break;

  case PLY_UCHAR:
    return 1;
    break;

  case PLY_UINT:
    return 4;
    break;

  case PLY_UINT8:
    return 1;
    break;

  case PLY_UINT16:
    return 2;
    break;

  case PLY_UIN32:
    return 4;
    break;

  case PLY_USHORT:
    return 2;
    break;

  default:
    return -1;
  }
}