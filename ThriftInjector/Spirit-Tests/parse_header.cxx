/*=============================================================================
    Copyright (c) 2002-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  This sample demontrates a parser for a comma separated list of numbers.
//  The numbers are inserted in a vector using phoenix.
//
//  [ JDG May 10, 2002 ]    spirit1
//  [ JDG March 24, 2007 ]  spirit2
//
///////////////////////////////////////////////////////////////////////////////

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace client
{
enum EType { TETRAHEDRA, TRIANGLE, CIRCLE, POINT };

struct mesh_header
{
  std::string name;
  unsigned dimension;
  EType elementType;
  unsigned numberOfNodes;
  void set_name( std::string const &n ) { this->name = n; }
  void set_dimension( unsigned const &d ) { this->dimension = d; }
  void set_elemtype( EType const &t ) { this->elementType = t; }
};

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    ///////////////////////////////////////////////////////////////////////////
    //  Our number list compiler
    ///////////////////////////////////////////////////////////////////////////
    //[tutorial_numlist4
    template <typename Iterator>
    bool parse_numbers(Iterator first, Iterator last, mesh_header &header)
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
      symbols<char, EType> sym;

      sym.add
        ("tetrahedra", TETRAHEDRA)
        ("triangle", TRIANGLE)
        ("circle", CIRCLE)
        ("point", POINT)
        ;
        bool r = qi::phrase_parse
          (first, last,

           //  Begin grammar
           no_case[
             lit("MESH") >>
             as_string [lexeme['"' >> +(char_ - '"') >> '"']][boost::bind(&mesh_header::set_name, &header, ::_1)] >> 
             lit("DIMENSION") >> uint_ [boost::bind(&mesh_header::set_dimension, &header, ::_1)] >>
             lit("ELEMTYPE") >> 
           //ascii::no_case[(ascii::string("Tetra") | ascii::string("Triangle") )] >>
           sym [boost::bind(&mesh_header::set_elemtype, &header, ::_1)] >> 
           lit("NNODE") >> uint_]
           //  End grammar
           , space );

        if (first != last) // fail if we did not get a full match
            return false;
        return r;
    }
    //]
}

////////////////////////////////////////////////////////////////////////////
//  Main program
////////////////////////////////////////////////////////////////////////////
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tA comma separated list parser for Spirit...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";

    std::cout << "Give me a comma separated list of numbers.\n";
    std::cout << "The numbers will be inserted in a vector of numbers\n";
    std::cout << "Type [q or Q] to quit\n\n";

    std::string str;
    while (getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        client::mesh_header header;
        if (client::parse_numbers(str.begin(), str.end(), header))
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
