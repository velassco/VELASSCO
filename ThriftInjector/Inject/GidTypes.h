#ifndef __GidTypes_h__
#define __GidTypes_h__

#include <boost/cstdint.hpp>
#include <string>

#define BEGIN_GID_DECLS namespace GID {

#define END_GID_DECLS }

BEGIN_GID_DECLS

typedef boost::uint64_t UInt64;
typedef boost::uint64_t IdNode;
typedef boost::uint64_t IdElement;
typedef boost::uint32_t UInt32;
typedef boost::uint32_t IdPartition;

enum ElementType { LINE, TETRAHEDRA, TRIANGLE, SPHERE, POINT };
enum ValueType { SCALAR, VECTOR };
enum LocationType { LOC_NODE, LOC_GPOINT };
enum GaussPointType { GP_TRIANGLE, GP_QUADRILATERAL, GP_TETRAHEDRA,
		      GP_HEXAHEDRA, GP_PRISM, GP_PYRAMID };

UInt32 GetElementTypeDimension( ElementType etype );
bool CheckElementSize( ElementType etype, UInt32 size );
bool CheckGaussPointSize( GaussPointType gptype, UInt32 size );

const std::string &GetLocationAsString( LocationType et );
const std::string &GetElementTypeAsString( ElementType et );
const std::string &GetValueTypeAsString( ValueType rt );
UInt32 GetValueTypeSize( ValueType rt );

END_GID_DECLS

#endif
