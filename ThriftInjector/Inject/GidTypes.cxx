#include "GidTypes.h"
#include <map>

BEGIN_GID_DECLS

static
boost::spirit::qi::symbols<char, ElementType> _symEType;
static
boost::spirit::qi::symbols<char, LocationType> _symLType;
static
boost::spirit::qi::symbols<char, ValueType> _symVType;

static std::map<ElementType, std::string> _mapEType;

static std::map<LocationType, std::string> _mapLType;

struct ResultComponent
{
  std::string name;
  UInt32 size;
};

static std::map<ValueType, ResultComponent> _mapRType;

boost::spirit::qi::symbols<char, ElementType> &GetElementSymbols( )
{
  if ( !_symEType.find( "triangle" ) )
    {
    _symEType.add
      ( "tetrahedra", TETRAHEDRA )
      ( "triangle", TRIANGLE )
      ( "circle", CIRCLE )
      ;
    _mapEType[TETRAHEDRA] = "Tetrahedra";
    _mapEType[TRIANGLE] = "Triangle";
    _mapEType[CIRCLE] = "Circle";
   }
  return _symEType;
}

const std::string &GetElementTypeAsString( ElementType et )
{
  return _mapEType[ et ];
}

boost::spirit::qi::symbols<char, LocationType> &GetLocationSymbols( )
{
  if ( !_symLType.find( "onnodes" ) )
    {
    _symLType.add
      ( "onnodes", LOC_NODE )
      ( "ongausspoints", LOC_GPOINT )
      ;
    _mapLType[ LOC_NODE ] = "OnNodes";
    _mapLType[ LOC_GPOINT ] = "OnGaussPoints";
    }
  return _symLType;
}

boost::spirit::qi::symbols<char, ValueType> &GetValueSymbols( )
{
  if ( !_symVType.find( "scalar" ) )
    {
    _symVType.add
      ( "scalar", SCALAR )
      ( "vector", VECTOR )
      ;
    _mapRType[ SCALAR ] = {"Scalar", 1};
    _mapRType[ VECTOR ] = {"Vector", 4};
    }
  return _symVType;
}

const std::string &GetLocationAsString( LocationType rt )
{
  return _mapLType[ rt ];
}

const std::string &GetValueTypeAsString( ValueType rt )
{
  return _mapRType[ rt ].name;
}

UInt32 GetValueTypeSize( ValueType rt )
{
  return _mapRType[ rt ].size;
}

END_GID_DECLS
