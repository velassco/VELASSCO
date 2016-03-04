#if !defined(__ParseAsciiMesh_h__)
#define __ParseAsciiMesh_h__
#include "GidTypes.h"
#include <vector>
#include <list>
#include <string>
#include <map>
#include <boost/spirit/include/qi_symbols.hpp>

BEGIN_GID_DECLS

boost::spirit::qi::symbols<char, ElementType> &GetElementSymbols( );

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

  void Init()
  {
    this->setName( "" );
    this->setDimension( 3 );
    this->setElementType( TRIANGLE );
    this->setNumberOfNodes( 3 );
  }
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

  void Init()
  {
    this->header.Init();
    this->nodes.clear();
    this->elements.clear();
  }
};

int ParseMeshFile( const std::string& pathFile, std::list<MeshResultType> &meshes );

END_GID_DECLS

#endif
