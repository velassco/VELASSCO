#include "GidTypes.h"
#include <map>
#include <vector>

BEGIN_GID_DECLS

struct ElementTypeInfo
{
  std::string name;
  std::vector<size_t> sizes;
  UInt32 dimension;
};

static std::map<ElementType, ElementTypeInfo> _mapEType;

static std::map<LocationType, std::string> _mapLType;

struct ResultComponent
{
  std::string name;
  UInt32 size;
};

static std::map<ValueType, ResultComponent> _mapVType;

static void _InitElementTypeMap( )
{
  if ( _mapEType.size( ) == 0 )
    {
    _mapEType[LINE] = { "Line", { 2, 3 }, 1 };
    _mapEType[TETRAHEDRA] = { "Tetrahedra", { 4, 10 }, 3 };
    _mapEType[TRIANGLE] = { "Triangle", { 3, 6 }, 2 };
    _mapEType[SPHERE] = { "Sphere", { 1 }, 3 };
    _mapEType[POINT] = { "Point", { 1 }, 0 };
    }
}

const std::string &GetElementTypeAsString( ElementType et )
{
  _InitElementTypeMap( );
  return _mapEType[ et ].name;
}

UInt32 GetElementTypeDimension( ElementType et )
{
  _InitElementTypeMap( );
  const ElementTypeInfo &e = _mapEType[ et ];
  return e.dimension;
}

bool CheckElementSize( ElementType et, UInt32 size )
{
  _InitElementTypeMap( );
  const ElementTypeInfo &e = _mapEType[ et ];
  for( int i = 0; i < e.sizes.size( ); i++ )
    {
    if ( e.sizes[ i ] == size )
      {
      return true;
      }
    }
  return false;
}

const std::string & GetLocationAsString( LocationType rt )
{
  if ( _mapLType.size() == 0 )
    {
    _mapLType[ LOC_NODE ] = "OnNodes";
    _mapLType[ LOC_GPOINT ] = "OnGaussPoints";
    }
  return _mapLType[ rt ];
}

static void _InitValueTypeMap( )
{
  if ( _mapVType.size( ) == 0 )
    {
    _mapVType[ SCALAR ] = {"Scalar", 1};
    _mapVType[ VECTOR ] = {"Vector", 4};
    }
}

const std::string & GetValueTypeAsString( ValueType rt )
{
  _InitValueTypeMap( );
  return _mapVType[ rt ].name;
}

UInt32 GetValueTypeSize( ValueType rt )
{
  _InitValueTypeMap( );
  return _mapVType[ rt ].size;
}

END_GID_DECLS
