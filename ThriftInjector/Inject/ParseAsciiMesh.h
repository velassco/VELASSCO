#if !defined(__ParseAsciiMesh_h__)
#define __ParseAsciiMesh_h__
#include "GidTypes.h"
#include <vector>
#include <string>
#include <map>

BEGIN_GID_DECLS

struct MeshHeaderType
{
  std::string name;
  UInt32 dimension;
  ElementType elementType;
  IdNode numberOfNodes;
  void setName( std::string const &n ) { this->name = n; }
  void setDimension( UInt32 const &d ) { this->dimension = d; }
  void setElementType( ElementType const &t ) { this->elementType = t; }
  void setNumberOfNodes( IdNode const &n ) { this->numberOfNodes = n; }
};

struct MeshNodeType
{
  GID::IdNode id;
  double x, y, z;
  void setId( boost::long_long_type const &i ) { this->id = i; }
};

typedef std::vector<boost::long_long_type> MeshElementType;

struct MeshResultType
{
  MeshHeaderType header;
  std::vector<MeshNodeType> nodes;
  std::vector<MeshElementType> elements;
};

int ParseMeshFile( const std::string& pathFile, MeshResultType &mesh );

END_GID_DECLS

#endif
