#include "GidTypes.h"

BEGIN_GID_DECLS

static
boost::spirit::qi::symbols<char, ElementType> _symEType;
static
boost::spirit::qi::symbols<char, LocationType> _symLType;
static
boost::spirit::qi::symbols<char, ValueType> _symVType;

boost::spirit::qi::symbols<char, ElementType> &GetElementSymbols( )
{
  if ( !_symEType.find( "triangle" ) )
    {
    _symEType.add
      ( "tetrahedra", TETRAHEDRA )
      ( "triangle", TRIANGLE )
      ( "circle", CIRCLE )
      ;
   }
  return _symEType;
}

boost::spirit::qi::symbols<char, LocationType> &GetLocationSymbols( )
{
  if ( !_symLType.find( "onnodes" ) )
    {
    _symLType.add
      ( "onnodes", LOC_NODE )
      ( "ongausspoints", LOC_GPOINT )
      ;
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
    }
  return _symVType;
}

END_GID_DECLS
