#include "ParseAsciiResult.h"
#include "Logger.h"
#include <fstream>
#include <boost/algorithm/string.hpp>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>
#include <boost/variant.hpp>

#include <iostream>
#include <string>

BEGIN_GID_DECLS

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

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
       lit("ELEMTYPE") >> sym_element
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

int ParseResultSectionHeader( const std::string &line, ResultContainerType &result, SectionHeaderType &section )
{
  VecSectionArg vec;
  //vec._trace = true;
  if ( parse_result_header( line.begin(), line.end(), vec ) )
    {
    section = boost::get<SectionHeaderType>( vec[ 0 ] );
    switch ( section )
      {
      case SECTION_GAUSS_POINT:
      LOG(warning) << "found GaussPoints section, still not processed";
      result.gaussPoints.push_back( GaussPointType( ) );
      result.gaussPoints.back( ).name = boost::get<std::string>( vec[ 1 ] );
      break;
      case SECTION_RANGE_TABLE:
      LOG(warning) << "found RangesTable section, still not processed";
      result.rangeTables.push_back( RangeTableType( ) );
      result.rangeTables.back( ).name = boost::get<std::string>( vec[ 1 ] );
      break;
      case SECTION_RESULT:
      result.results.push_back( ResultBlockType() );
      result.results.back().setName( boost::get<std::string>( vec[ 1 ] ) );
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

// TODO: for now we are skipping the gauss points definitions
int ParseGaussPointProperties( const std::string &line, 
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
       lit("end") >> lit("gausspoints")[boost::bind(&VecSectionArg::init_type, &vec, SECTION_END)]
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
        if ( ParseResultSectionHeader( line, result, section ) == 0 )
          {
          LOG(trace) << line;
          switch ( section )
            {
            case SECTION_GAUSS_POINT:
              state = GAUSS_POINT;
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
        if( ParseGaussPointProperties( line, result, section ) == 0 )
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
