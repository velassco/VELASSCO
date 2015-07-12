#include <fstream>
#include <boost/algorithm/string.hpp>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>
#include <boost/variant.hpp>

#include <iostream>
#include <string>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

enum BlockHeaderType
{ BLOCK_GAUSS_POINT, BLOCK_RANGE_TABLE, BLOCK_RESULT };
enum EType { TETRAHEDRA, TRIANGLE, CIRCLE };
enum RType { SCALAR, VECTOR };
typedef boost::variant<BlockHeaderType, EType, RType, double, std::string> Arg;
struct VecArg
{
  std::vector<Arg> vec;
  void init_type( BlockHeaderType a )
  {
    vec.clear( );
    vec.push_back( a );
  }
  void push_back( Arg const &a ) { vec.push_back( a ); }
};

template <typename Iterator>
bool parse_result_header(Iterator first, Iterator last, VecArg &vec)
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
  using qi::rule;

  symbols<char, EType> sym_elem;

  sym_elem.add
    ("tetrahedra", TETRAHEDRA)
    ("triangle", TRIANGLE)
    ("circle", CIRCLE)
    ;
  symbols<char, EType> &asym = sym_elem;
  rule<Iterator, std::string()> quoted_string;
  quoted_string = lexeme['"' >> +(char_ - '"') >> '"'];

  bool r = qi::phrase_parse
    (first, last,

     //  Begin grammar
     no_case[
       lit("GAUSSPOINTS")[ boost::bind(&VecArg::init_type, &vec, BLOCK_GAUSS_POINT) ] >>
       as_string[quoted_string][boost::bind(&VecArg::push_back, &vec, ::_1)] >>
       lit("ELEMTYPE") >> asym 
       |
       lit("RESULT")[ boost::bind(&VecArg::init_type, &vec, BLOCK_RESULT) ] >>
       as_string [lexeme['"' >> +(char_ - '"') >> '"']][boost::bind(&VecArg::push_back, &vec, ::_1)] >>
       as_string [lexeme['"' >> +(char_ - '"') >> '"']][boost::bind(&VecArg::push_back, &vec, ::_1)] >>
       double_[boost::bind(&VecArg::push_back, &vec, ::_1)]
       |
       lit("RESULTRANGESTABLE")[ boost::bind(&VecArg::init_type, &vec, BLOCK_RANGE_TABLE) ] >>
       as_string [lexeme['"' >> +(char_ - '"') >> '"']][boost::bind(&VecArg::push_back, &vec, ::_1)]]
       //  End grammar
       , space );

  if ( first != last )
    {
    r = false;
    }
  return r;
}

////////////////////////////////////////////////////////////////////////////
//  Main program
////////////////////////////////////////////////////////////////////////////
int
main()
{
  std::string str;
  while (getline(std::cin, str))
    {
    if (str.empty() || str[0] == 'q' || str[0] == 'Q')
      break;
    VecArg header;
    if (parse_result_header( str.begin(), str.end(), header ) )
      {
      std::cout << "-------------------------\n";
      std::cout << "Parsing succeeded\n";
      std::cout << str << " Parses OK: " << std::endl;
      std::cout << "\n-------------------------\n";
      }
    else
      {
      std::cout << "-------------------------\n";
      std::cout << "Parsing failed\n";
      std::cout << "-------------------------\n";
      }
    }
  
  std::cout << "Bye... :-) \n\n";
  return 0;
}
