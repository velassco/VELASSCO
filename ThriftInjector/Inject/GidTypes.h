#ifndef __GidTypes_h__
#define __GidTypes_h__

#include <boost/cstdint.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#define BEGIN_GID_DECLS namespace GID {

#define END_GID_DECLS }

BEGIN_GID_DECLS

enum ElementType { TETRAHEDRA, TRIANGLE, CIRCLE };
enum ValueType { SCALAR, VECTOR };
enum LocationType { LOC_NODE, LOC_GPOINT };

boost::spirit::qi::symbols<char, ElementType> &GetElementSymbols( );
boost::spirit::qi::symbols<char, LocationType> &GetLocationSymbols( );
boost::spirit::qi::symbols<char, ValueType> &GetValueSymbols( );

typedef boost::uint64_t UInt64;
typedef boost::uint64_t IdNode;
typedef boost::uint64_t IdElement;
typedef boost::uint32_t UInt32;
typedef boost::uint32_t IdPartition;

const std::string &GetLocationAsString( LocationType et );
const std::string &GetElementTypeAsString( ElementType et );
const std::string &GetValueTypeAsString( ValueType rt );
UInt32 GetValueTypeSize( ValueType rt );

END_GID_DECLS

#endif
