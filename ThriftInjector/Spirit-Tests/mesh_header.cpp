#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <iostream>
#include <string>
//#include <complex>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

struct mesh_header
{
  std::string name;
  unsigned dimension;
  std::string elementType;
  unsigned numberOfNodes;
};

BOOST_FUSION_ADAPT_STRUCT(
    mesh_header,
    (std::string, name)
    (unsigned, dimension)
    (std::string, elementType)
    (unsigned, numberOfNodes)
)

template <typename Iterator>
struct mesh_header_parser : qi::grammar<Iterator, mesh_header(), ascii::space_type>
{
  mesh_header_parser() : mesh_header_parser::base_type(start)
  {
    using qi::uint_;
    using qi::lit;
    using qi::double_;
    using qi::lexeme;
    using ascii::char_;
    
    quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];
    start %=
      ascii::no_case[lit("MESH")] >> quoted_string >> 
      ascii::no_case[lit("DIMENSION")] >> uint_ >>
      ascii::no_case[lit("ELEMTYPE")] >> ascii::no_case[(ascii::string("Tetra") | ascii::string("Triangle") )] >>
      ascii::no_case[lit("NNODE")] >> uint_
      ;
  }
  
  qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
  qi::rule<Iterator, mesh_header(), ascii::space_type> start;
};

////////////////////////////////////////////////////////////////////////////
//  Main program
////////////////////////////////////////////////////////////////////////////
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tA mesh header parser for Spirit...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";

    std::cout
        << "Give me mesh header of the form :"
        << "MESH \"Part\" dimension 3 ElemType Tetrahedra Nnode 4 \n";
    std::cout << "Type [q or Q] to quit\n\n";

    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef mesh_header_parser<iterator_type> mesh_header_parser;

    mesh_header_parser g; // Our grammar
    std::string str;
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        mesh_header mh;
        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = phrase_parse(iter, end, g, space, mh);

        if (r && iter == end)
          {
          std::cout << boost::fusion::tuple_open('[');
          std::cout << boost::fusion::tuple_close(']');
          std::cout << boost::fusion::tuple_delimiter(", ");

          std::cout << "-------------------------\n";
          std::cout << "Parsing succeeded\n";
          std::cout << "got: " << boost::fusion::as_vector(mh) << std::endl;
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
