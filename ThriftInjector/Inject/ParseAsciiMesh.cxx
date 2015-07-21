#include "GidTypes.h"
#include "ParseAsciiMesh.h"
#include "Logger.h"
#include <fstream>
#include <boost/algorithm/string.hpp>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <string>

BEGIN_GID_DECLS

static
boost::spirit::qi::symbols<char, ElementType> _symEType;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

boost::spirit::qi::symbols<char, ElementType> &GetElementSymbols( )
{
  if ( !_symEType.find( "triangle" ) )
    {
    _symEType.add
      ( "line", LINE )
      ( "tetrahedra", TETRAHEDRA )
      ( "triangle", TRIANGLE )
      ( "sphere", SPHERE )
      ;
   }
  return _symEType;
}

template <typename Iterator>
bool parse_mesh_header(Iterator first, Iterator last, MeshHeaderType &header)
{
  using qi::uint_;
  using qi::lit;
  using qi::double_;
  using qi::lexeme;
  using qi::as_string;
  using ascii::char_;
  using qi::phrase_parse;
  using qi::_1;
  using ascii::space;
  using ascii::no_case;
  using qi::symbols;

  symbols<char, ElementType> &sym = GetElementSymbols( );

  bool r = qi::phrase_parse
    (first, last,

     //  Begin grammar
     no_case[
       lit("MESH") >>
       as_string [lexeme['"' >> +(char_ - '"') >> '"']][boost::bind(&MeshHeaderType::setName, &header, ::_1)] >> 
       lit("DIMENSION") >> 
       uint_ [boost::bind(&MeshHeaderType::setDimension, &header, ::_1)] >>
       lit("ELEMTYPE") >> 
       sym [boost::bind(&MeshHeaderType::setElementType, &header, ::_1)] >> 
       lit("NNODE") >> 
       uint_ [boost::bind(&MeshHeaderType::setNumberOfNodes, &header, ::_1)]]
     //  End grammar
     , space );

  if (first != last) // fail if we did not get a full match
    return false;
  return r;
}

struct VecDouble
{
  std::vector<double> v;
  void push_back( double const &c ) { this->v.push_back( c ); }
};

template <typename Iterator>
bool parse_mesh_coordinate(UInt32 dim, Iterator first, Iterator last, MeshNodeType &node)
{
  using qi::ulong_long;
  using qi::double_;
  using qi::phrase_parse;
  using qi::_1;
  using ascii::space;

  VecDouble crd;
  bool r = qi::phrase_parse
    (first, last,

     //  Begin grammar
     ulong_long[boost::bind(&MeshNodeType::setId, &node, ::_1)] >> +double_[boost::bind(&VecDouble::push_back, &crd, ::_1) ]
     //  End grammar
     , space );

  if (first != last) // fail if we did not get a full match
    return false;
  if ( r )
    {
    if ( dim != crd.v.size() )
      {
      LOG(error) << "expected " << dim << " coordinates";
      return false;
      }
    node.x = crd.v[0];
    node.y = crd.v[1];
    if ( dim == 3 )
      {
      node.z = crd.v[2];
      }
    else
      {
      node.z = 0.0;
      }
    }
  return r;
}

template <typename Iterator>
bool parse_mesh_element(unsigned int nnode, Iterator first, Iterator last, MeshElementType &elem)
{
  using qi::ulong_long;
  using qi::double_;
  using qi::phrase_parse;
  using qi::_1;
  using ascii::space;

  bool r = qi::phrase_parse
    (first, last,

     //  Begin grammar
     +ulong_long
     //  End grammar
     , space, elem );

  if (first != last) // fail if we did not get a full match
    return false;
  if ( r )
    {
    if ( nnode+1 != elem.size() )
      {
      LOG(error) << "expected " << nnode+1 << " integers";
      return false;
      }
    }
  return r;
}

int ParseMeshHeader( const std::string &line, MeshHeaderType &mheader )
{
  if ( parse_mesh_header( line.begin(), line.end(), mheader ) )
    {
    LOG(trace) << "Parsing succeeded: " << line;
    return 0;
    }
  else
    {
    LOG(error) << "Parsing failed:" << line;
    return -1;
    }
}

int ParseNode( unsigned int dim, const std::string &line, MeshNodeType &mnode )
{
  if ( parse_mesh_coordinate( dim, line.begin(), line.end(), mnode ) )
    {
    return 0;
    }
  else
    {
    LOG(error) << "Parsing failed:" << line;
    return -1;
    }
  return 0;
}

int ParseElement( unsigned int nnode, const std::string &line, MeshElementType &element )
{
   if ( parse_mesh_element( nnode, line.begin(), line.end(), element ) )
    {
    return 0;
    }
  else
    {
    LOG(error) << "Parsing failed:" << line;
    return -1;
    }
  return 0;
}

int ParseMeshFile( const std::string& pathFile, MeshResultType &mesh )
{
  std::ifstream fin( pathFile );
  std::string line;
  enum MeshStateType {S, H, C, CE, E, EE};
  MeshStateType state = S;
  std::string kwCoordinates( "Coordinates" );

  while( std::getline( fin, line ) )
    {
    boost::trim( line );

    if ( line.empty( ) || line[0] == '#' )
      {
      continue;
      }
    switch ( state )
      {
      case S:
        if( ParseMeshHeader( line, mesh.header ) != 0 )
          {
          LOG(error) << "parsing mesh header from: '" << line << "'";
          return -1;
          }
        else
          {
          state = H;
          }
        break;
      case H:
        if ( boost::iequals( line, "Coordinates" ) )
          {
          LOG(trace) << line;
          state = C;
          }
        else
          {
          LOG(error) << "expecting Coordinate keyword at line: '" << line << "'";
          return -1;
          }
        break;
      case C:
        if ( boost::iequals( line, "End Coordinates" ) )
          {
          LOG(trace) << mesh.nodes.size() << " nodes where read";
          LOG(trace) << line;
          state = CE;
          }
        else 
          {
          MeshNodeType node;
          if ( ParseNode( mesh.header.dimension, line,  node ) != 0 )
            {
            LOG(error) << "parsing mesh node from line: '" << line << "'";
            return -1;
            }
          mesh.nodes.push_back( node );
          }
        break;
      case CE:
        if ( boost::iequals( line, "Elements" ) )
          {
          LOG(trace) << line;
          state = E;
          }
        else
          {
          LOG(error) << "expecting Elements keyword at line: '" << line << "'";
          return -1;
          }
        break;
      case E:
        if ( boost::iequals( line, "End Elements" ) )
          {
          LOG(trace) << mesh.elements.size() << " elements where read";
          LOG(trace) << line;
          state = EE;
          }
        else 
          {
          MeshElementType elem;
          if ( ParseElement( mesh.header.numberOfNodes, line,  elem ) != 0 )
            {
            LOG(error) << "parsing mesh elem from line: '" << line << "'";
            return -1;
            }
          mesh.elements.push_back( elem );
          //std::cout << "last element has size = " << mesh.elements.back().size() << std::endl;
          }
        break;
      case EE:
        LOG(error) << "unexpected line after 'end elements': " << line;
        return -1;
        break;
      }
    }
  return 0;
}

END_GID_DECLS
