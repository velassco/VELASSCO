#include "ParseAsciiMesh.h"
#include "ParseAsciiResult.h"
#include "Logger.h"
#include <fstream>
#include <boost/algorithm/string.hpp>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>

using boost::lexical_cast;
using boost::bad_lexical_cast;

BEGIN_GID_DECLS

static
boost::spirit::qi::symbols<char, LocationType> _symLType;
static
boost::spirit::qi::symbols<char, ValueType> _symVType;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

std::string ResultHeaderType::GetAsString( )
{
  return std::string( "Result \"" ) + this->name + "\" \"" + 
    lexical_cast<std::string>( this->analysis ) + "\" " + 
    lexical_cast<std::string>( this->step ) + " " +
    GetValueTypeAsString( this->rType ) + " " + GetLocationAsString( this->location );
}

ResultContainerType::ResultContainerType( )
{
  gaussPoints[ "GP_LINE_1" ] = { "GP_LINE_1", "", LINE, 1, {}, false };
  gaussPoints[ "GP_TRIANGLE_1" ] = { "GP_TRIANGLE_1", "", TRIANGLE, 1, {}, false };
  gaussPoints[ "GP_TRIANGLE_3" ] = { "GP_TRIANGLE_3", "", TRIANGLE, 3, {}, false };
  gaussPoints[ "GP_TRIANGLE_6" ] = { "GP_TRIANGLE_6", "", TRIANGLE, 6, {}, false };
  gaussPoints[ "GP_TETRAHEDRA_1" ] = { "GP_TETRAHEDRA_1", "", TETRAHEDRA, 1, {}, false };
  gaussPoints[ "GP_TETRAHEDRA_4" ] = { "GP_TETRAHEDRA_4", "", TETRAHEDRA, 4, {}, false };
  gaussPoints[ "GP_TETRAHEDRA_10" ] = { "GP_TETRAHEDRA_10", "", TETRAHEDRA, 10, {}, false };
  gaussPoints[ "GP_SPHERE_1" ] = { "GP_SPHERE_1", "", SPHERE, 1, {}, false };
}

static
boost::spirit::qi::symbols<char, LocationType> & GetLocationSymbols( )
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

static
boost::spirit::qi::symbols<char, ValueType> & GetValueSymbols( )
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

enum SectionHeaderType
{ SECTION_GAUSS_POINT, SECTION_RANGE_TABLE, SECTION_RESULT, 
  SECTION_VALUES, SECTION_COMPONENT_NAMES, SECTION_END };

typedef boost::variant<SectionHeaderType, ElementType, ValueType, LocationType, double, unsigned int, std::string, boost::ulong_long_type> SectionArg;
struct VecSectionArg
{
  bool _trace;

  VecSectionArg()
  {
    _trace = false;
  }
  std::vector<SectionArg> vec;
  void init_type( SectionHeaderType a )
  {
    vec.clear( );
    vec.push_back( a );
  }
  void addArg( SectionArg const &a ) 
  { 
    if ( this->_trace )
      {
      LOG(trace) << "addArg( " << a << " )";
      }
    this->vec.push_back( a ); 
  }
  const SectionArg &operator[]( int i ) const { return this->vec[i]; }
  SectionArg &operator[]( int i ) { return this->vec[i]; }
  size_t size() { return this->vec.size(); }
};

template <typename Iterator>
bool parse_result_header(Iterator first, Iterator last, VecSectionArg &vec)
{
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
  using qi::rule;

  symbols<char, ElementType> &sym_element = GetElementSymbols( );
  symbols<char, LocationType> &sym_location = GetLocationSymbols( );
  symbols<char, ValueType> &sym_vtype = GetValueSymbols( );
  
  rule<Iterator, std::string()> quoted_string;
  quoted_string = lexeme['"' >> +(char_ - '"') >> '"'];

  bool r = qi::phrase_parse
    (first, last,

     //  Begin grammar
     no_case[
       lit("GAUSSPOINTS")[ boost::bind(&VecSectionArg::init_type, &vec, SECTION_GAUSS_POINT) ] >>
       as_string[quoted_string][boost::bind(&VecSectionArg::addArg, &vec, ::_1)] >>
       lit("ELEMTYPE") >> sym_element[boost::bind(&VecSectionArg::addArg, &vec, ::_1)]
       |
       lit("RESULTRANGESTABLE")[ boost::bind(&VecSectionArg::init_type, &vec, SECTION_RANGE_TABLE) ] >>
       as_string[quoted_string][boost::bind(&VecSectionArg::addArg, &vec, ::_1)]
       |
       lit("RESULT")[ boost::bind(&VecSectionArg::init_type, &vec, SECTION_RESULT) ] >>
       as_string [quoted_string][boost::bind(&VecSectionArg::addArg, &vec, ::_1)] >>
       as_string [quoted_string][boost::bind(&VecSectionArg::addArg, &vec, ::_1)] >>
       double_[boost::bind(&VecSectionArg::addArg, &vec, ::_1)] >>
       sym_vtype[boost::bind(&VecSectionArg::addArg, &vec, ::_1)] >>
       (lit("ONNODES")[boost::bind(&VecSectionArg::addArg, &vec, LOC_NODE)] |
        lit("ONGAUSSPOINTS")[boost::bind(&VecSectionArg::addArg, &vec, LOC_GPOINT)] >> 
        as_string[quoted_string][boost::bind(&VecSectionArg::addArg, &vec, ::_1)])
       ]
       //  End grammar
       , space );

  if ( first != last )
    {
    r = false;
    }
  if ( !r )
    {
    std::string rest( first, last );
    LOG(error) << "could not parse: " << rest;
    }
  return r;
}

int ParseResultFileHeader( const std::string &line, ResultContainerType &result )
{
  using qi::lit;
  using qi::uint_;
  using ascii::no_case;
  using ascii::space;
  
  std::string::const_iterator first = line.begin();
  std::string::const_iterator last = line.end();
  
  bool r = qi::phrase_parse
    (first, last,
     //  Begin grammar
     no_case[
       lit("GiD") >> lit("Post") >> lit("Results") >> lit("File") >>
       uint_[boost::bind(&ResultContainerType::setMajorVersion, &result, ::_1) ] >>
       '.' >>
       uint_[boost::bind(&ResultContainerType::setMinorVersion, &result, ::_1) ]
       ]
     //  End grammar
     , space );
  
  if ( first != last )
    {
    r = false;
    }
  if ( !r )
    {
    std::string rest( first, last );
    LOG(error) << "could not parse: " << rest;
    }
  return r ? 0 : -1;
};

int ParseResultSectionHeader( const std::string &line, ResultContainerType &result,
                              SectionHeaderType &section, std::string &name )
{
  VecSectionArg vec;
  //vec._trace = true;
  name = "";
  if ( parse_result_header( line.begin(), line.end(), vec ) )
    {
    section = boost::get<SectionHeaderType>( vec[ 0 ] );
    switch ( section )
      {
      case SECTION_GAUSS_POINT:
        name = boost::get<std::string>( vec[ 1 ] );
        result.gaussPoints[ name ] = GaussPointDefinition( );
        result.gaussPoints[ name ].name = name;
        result.gaussPoints[ name ].etype = boost::get<ElementType>( vec[ 2 ] );
        break;
      case SECTION_RANGE_TABLE:
        LOG(warning) << "found RangesTable section, still not processed";
        result.rangeTables.push_back( RangeTableType( ) );
        name = boost::get<std::string>( vec[ 1 ] );
        result.rangeTables.back( ).name = name;
        break;
      case SECTION_RESULT:
      result.results.push_back( ResultBlockType() );
      name = boost::get<std::string>( vec[ 1 ] );
      result.results.back().setName( name );
      result.results.back().setAnalysis( boost::get<std::string>( vec[ 2 ] ) );
      result.results.back().setStep( boost::get<double>( vec[ 3 ] ) );
      result.results.back().setResultType( boost::get<ValueType>( vec[ 4 ] ) );
      result.results.back().setLocation( boost::get<LocationType>( vec[ 5 ] ) );
      if ( result.results.back().getLocation( ) == LOC_GPOINT )
        {
        result.results.back().setGPName( boost::get<std::string>( vec[ 6 ] ) );
        }
      break;
      }
    return 0;
    }
  else
    {
    return -1;
    }
}

int ParseRangeTableProperties( const std::string &line, 
                               ResultContainerType &result, SectionHeaderType &section )
{
  using qi::lit;
  using qi::eoi;
  using ascii::no_case;
  using ascii::space;
  
  std::string::const_iterator first = line.begin();
  std::string::const_iterator last = line.end();
  
  VecSectionArg vec;

  bool r = qi::phrase_parse
    (first, last,
     //  Begin grammar
     no_case[
       lit("end") >> lit("resultrangestable")[boost::bind(&VecSectionArg::init_type, &vec, SECTION_END)]
       |
       eoi
       ]
     //  End grammar
     , space );
  
  if ( first != last )
    {
    r = false;
    }
  if ( r )
    {
    assert( vec.size() <= 1 );
    if ( vec.size() == 1 )
      {
      assert( boost::get<SectionHeaderType>( vec[ 0 ] ) == SECTION_END );
      section = SECTION_END;
      }
    }
  else
    {
    std::string rest( first, last );
    LOG(error) << "could not parse: " << rest;
    }
  return r ? 0 : -1;
}

struct GPProperties
{
  unsigned int numberOfGP;
  int nodesIncluded;
  int naturalCoordinates;
  std::vector<double> coords;
  bool endFound;
  bool errorFound;

  GPProperties()
    : numberOfGP( 0 ), nodesIncluded( -1 ), naturalCoordinates( -1 ), endFound( false ), errorFound( false )
  {
  }

  void init()
  {
    numberOfGP = 0;
    nodesIncluded = -1;
    naturalCoordinates = -1;
    coords.clear( );
    endFound = false;
    errorFound = false;
  }

  void setNumber( unsigned int v )
  {
    if ( numberOfGP > 0 )
      {
      LOG(error) << "'Number of Gauss Points' already provided with value " << numberOfGP << " while trying to set as " << v;
      errorFound = true;
      }
    else
      {
      numberOfGP = v;
      }
  }

  void setIncluded( bool v )
  {
    if ( nodesIncluded >= 0 )
      {
      LOG(error) << (nodesIncluded?"'Nodes included'":"'Nodes not included'")
                 << " already provided while trying to set as " 
                 << (v?"'Nodes included'":"'Nodes not included'");
      errorFound = true;
      }
    else
      {
      nodesIncluded = int(v);
      }
  }

  void setCoordinates( std::string const &v )
  {
    if ( naturalCoordinates >= 0 )
      {
      LOG(error) << "'Natural Coordinates' already provided with value " 
                 << (naturalCoordinates?"internal":"given") << " while to trying to set as '" << v << "'";
      errorFound = true;
      }
    else
      {
      if ( boost::iequals( v, "internal" ) )
        {
        naturalCoordinates = 1;
        }
      else
        {
        naturalCoordinates = 0;
        }
      }
  }

  void addCrd( double const &c )
  {
    if ( naturalCoordinates == 1 )
      {
       LOG(error) << "Coordinate value specified for a gausspoint definition with natural coordinate 'internal'";
       errorFound = true;
      }
    else
      {
      this->coords.push_back( c );
      }
  }

  void setEnd()
  {
    endFound = true;
  }
};

// TODO: for now we are skipping the gauss points definitions
int ParseGaussPointProperties( const std::string &line, 
                               GPProperties &gp )
{
  using qi::double_;
  using qi::uint_;
  using qi::lit;
  using qi::eoi;
  using ascii::no_case;
  using ascii::space;
  using qi::as_string;

  std::string::const_iterator first = line.begin();
  std::string::const_iterator last = line.end();
  
  bool r = qi::phrase_parse
    (first, last,
     //  Begin grammar
     no_case[
       lit("END") >> lit("GAUSSPOINTS")[boost::bind(&GPProperties::setEnd, &gp)]
       |
       lit("NUMBER") >> lit("OF") >> lit("GAUSS") >> lit("POINTS") >> lit(":") >>
       uint_ [boost::bind(&GPProperties::setNumber, &gp, ::_1)]
       |
       lit("NODES") >> lit("NOT") >> 
       lit("INCLUDED") [ boost::bind(&GPProperties::setIncluded, &gp, false) ]
       | 
       lit("NODES") >> lit("INCLUDED") [ boost::bind(&GPProperties::setIncluded, &gp, true) ]
       | 
       lit("NATURAL") >> lit("COORDINATES") >> lit(":") >> 
       as_string [(lit("INTERNAL") | lit("GIVEN"))] [ boost::bind(&GPProperties::setCoordinates, &gp, ::_1) ]
       |
       +double_[boost::bind(&GPProperties::addCrd, &gp, ::_1)]
       |
       eoi
       ]
     //  End grammar
     , space );
  
  if ( first != last )
    {
    r = false;
    }
  if ( r )
    {
    r = !gp.errorFound;
    }
  else
    {
    std::string rest( first, last );
    LOG(error) << "could not parse: " << rest;
    }
  return r ? 0 : -1;
}

int ParseResultDescription( const std::string &line, 
                            ResultContainerType &result, bool &beginValues )
{
  using qi::lit;
  using qi::char_;
  using qi::lexeme;
  using ascii::no_case;
  using ascii::space;
  using qi::rule;
  
  std::string::const_iterator first = line.begin();
  std::string::const_iterator last = line.end();
  
  rule<std::string::const_iterator, std::string()> quoted_string;
  quoted_string = lexeme['"' >> +(char_ - '"') >> '"'];

  VecSectionArg vec;
  //vec._trace = true;

  beginValues = false;
  bool r = qi::phrase_parse
    (first, last,
     //  Begin grammar
     no_case[
       lit("values")[boost::bind(&VecSectionArg::init_type, &vec, SECTION_VALUES)]
       |
       lit("componentnames")[boost::bind(&VecSectionArg::init_type, &vec, SECTION_COMPONENT_NAMES)] >>
       quoted_string[boost::bind(&VecSectionArg::addArg, &vec, ::_1)] % ','
       ]
     //  End grammar
     , space );
  
  if ( first != last )
    {
    r = false;
    }
  if ( r )
    {
    if ( vec.size() == 1 )
      {
      if ( boost::get<SectionHeaderType>( vec[ 0 ] ) == SECTION_VALUES )
        {
        beginValues = true;
        }
      else
        {
        LOG(error) << "expected values keyword, could not parse: " << vec[0];
        r = false;
        }
      }
    else
      {
      if ( boost::get<SectionHeaderType>( vec[ 0 ] ) == SECTION_COMPONENT_NAMES )
        {
        // TODO: validate number of component names
        for( int i = 1; i < vec.size(); i++ )
          {
          result.results.back().appendCompName( boost::get<std::string>( vec[ i ] ) );
          }
        }
      else
        {
        LOG(error) << "expected ComponentNames description";
        r = false;
        }
      }
    }
  else
    {
    std::string rest( first, last );
    LOG(error) << "could not parse: " << rest;
    }
  return r ? 0: -1;
}

int ParseValues( const std::string &line, 
                 ResultContainerType &result, bool &endValues )
{
  using qi::lit;
  using qi::ulong_long;
  using qi::double_;
  using ascii::no_case;
  using ascii::space;
  
  std::string::const_iterator first = line.begin();
  std::string::const_iterator last = line.end();
  
  VecSectionArg vec;

  endValues = false;

  bool r = qi::phrase_parse
    (first, last,
     //  Begin grammar
     no_case[
       lit("end") >> lit("values")[boost::bind(&VecSectionArg::init_type, &vec, SECTION_END)]
       |
       ulong_long[boost::bind(&VecSectionArg::addArg, &vec, ::_1)] >> 
       +double_[boost::bind(&VecSectionArg::addArg, &vec, ::_1)]
       ]
     //  End grammar
     , space );
  
  if ( first != last )
    {
    r = false;
    }
  if ( r )
    {
    if ( vec.size() == 1 )
      {
      if( boost::get<SectionHeaderType>( vec[ 0 ] ) == SECTION_END )
        {
        endValues = true;
        }
      else
        {
        LOG(error) << "expected 'end values'";
        r = false;
        }
      }
    else
      {
      // TODO: check cardinality of row values
      result.results.back().values.push_back( ResultRowType() );
      ResultRowType &row = result.results.back().values.back( );
      row.setId( boost::get<boost::ulong_long_type>( vec[ 0 ] ) );
      for( int i = 1; i < vec.size( ); i++ )
        {
        row.appendValue( boost::get<double>( vec[ i ] ) );
        }
      }
    }
  else
    {
    std::string rest( first, last );
    LOG(error) << "could not parse: " << rest;
    }
  return r ? 0 : -1;
}

int ParseResultFile( const std::string& pathFile, ResultContainerType &result )
{
  std::ifstream fin( pathFile );
  std::string line;
  enum ResultStateType 
  { 
    START,
    FILE_HEADER, 
    GAUSS_POINT, 
    RANGE_TABLE, 
    RESULT_HEADER,
    VALUES
  };
  bool flagValues;
  ResultStateType state = START;
  SectionHeaderType section;
  GPProperties gp;
  std::string section_name;

  while( std::getline( fin, line ) )
    {
    boost::trim( line );

    if ( line.empty( ) || line[0] == '#' )
      {
      continue;
      }
    switch ( state )
      {
      case START:
        if( ParseResultFileHeader( line, result ) != 0 )
          {
          LOG(error) << "parsing file header from: '" << line << "'";
          return -1;
          }
        else
          {
          state = FILE_HEADER;
          }
        break;
      case FILE_HEADER:
        if ( ParseResultSectionHeader( line, result, section, section_name ) == 0 )
          {
          LOG(trace) << line;
          switch ( section )
            {
            case SECTION_GAUSS_POINT:
              state = GAUSS_POINT;
              gp.init( );
              break;
            case SECTION_RANGE_TABLE:
              state = RANGE_TABLE;
              break;
            case SECTION_RESULT:
              state = RESULT_HEADER;
              break;
            }
          }
        else
          {
          LOG(error) << "expecting a valid section from line: '" << line << "'";
          return -1;
          }
        break;
      case GAUSS_POINT:
        if( ParseGaussPointProperties( line, gp ) == 0 )
          {
          if ( gp.endFound )
            {
            // TODO: validate if all properties are OK
            GaussPointDefinition &gpDef = result.gaussPoints[ section_name ];
            if ( gp.numberOfGP == 0 )
              {
              LOG(error) << "'Number of gauss points' not provided";
              return -1;
              }
            gpDef.size = gp.numberOfGP;
            if ( gp.nodesIncluded < 0 )
              {
              LOG(warning) << "'Nodes ?not? included' not provided, assuming 'not included'";
              }
            gpDef.includeNodes = bool(gp.nodesIncluded);
            
            if ( gp.naturalCoordinates < 0 )
              {
              LOG(warning) << "'Natural coordinates' not provided, assuming 'internal'";
              }
            if ( gp.naturalCoordinates )
              {
              if ( gp.coords.size() == 0 )
                {
                LOG(error) << "Natural Coordinates specified as 'given' but no coordinates values were given";
                return -1;
                }
              gpDef.coordinates.clear();
              std::copy( gp.coords.begin(), gp.coords.end(), gpDef.coordinates.begin() );
              }
            state = FILE_HEADER;
            }
          }
        else
          {
          LOG(error) << "expecting a valid gauss point property from line: '" << line << "'";
          return -1;
          }
        break;
      case RANGE_TABLE:
        if( ParseRangeTableProperties( line, result, section )  == 0 )
          {
          if ( section == SECTION_END )
            {
            // TODO: validate if all properties are OK
            state = FILE_HEADER;
            }
          }
        else
          {
          LOG(error) << "expecting a valid gauss point property from line: '" << line << "'";
          return -1;
          }
        break;
      case RESULT_HEADER:
        if( ParseResultDescription( line, result, flagValues ) == 0 )
          {
          if ( flagValues )
            {
            // TODO: validate if all properties are OK
            ResultHeaderType &header = result.results.back().header;
            if ( header.rType == VECTOR )
              {
              if ( !header.compName.size() )
                {
                header.compName.push_back( "Y_" + header.name );
                header.compName.push_back( "Y_" + header.name );
                header.compName.push_back( "Z_" + header.name );
                header.compName.push_back( "|" + header.name + "|" );
                }
              if( header.compName.size( ) < 2 || header.compName.size( ) > 4 )
                {
                LOG(error) << "wrong number of components at result: '" 
                           <<  header.GetAsString( ) << "'";
                return -1;
                }
              }
            state = VALUES;
            }
          }
        else
          {
          LOG(error) << "expecting a valid result description from line: '" << line << "'";
          return -1;
          }
        break;
      case VALUES:
        if( ParseValues( line, result, flagValues ) == 0 )
          {
          if ( flagValues )
            {
            // TODO: validate if last values inserted are OK
            state = FILE_HEADER;
            }
          }
        else
          {
          LOG(error) << "expecting a valid data values from line: '" << line << "'";
          return -1;
          }        
        break;
      }
    }
  LOG(trace) << result.results.size() << " blocks of results were read";
  return 0;
}

END_GID_DECLS
