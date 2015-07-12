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
   struct EType
   {
     unsigned int id;
     std::vector<double> conn;
       void print(unsigned int const& i) const
        {
            std::cout << i << std::endl;
        }
     void set_id( unsigned int const & _id ) { this->id = _id; }
     void push_back( double const &x ) { this->conn.push_back( x ); }
   };

// A plain function
    void print(unsigned int const& i)
    {
        std::cout << i << std::endl;
    }

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    ///////////////////////////////////////////////////////////////////////////
    //  Our number list compiler
    ///////////////////////////////////////////////////////////////////////////
    //[tutorial_numlist4
    template <typename Iterator>
    bool parse_numbers(Iterator first, Iterator last, EType &elem)
    {
        using qi::double_;
        using qi::uint_;
        using qi::phrase_parse;
        using qi::_1;
        using ascii::space;

        bool r = qi::phrase_parse(first, last,

              //  Begin grammar
                           //qi::uint_[&print]
                           //qi::uint_[boost::bind(&EType::set_id,
                           //&elem, ::_1)]
                                  //uint_ >> +double_
                                  uint_[boost::bind(&EType::set_id, &elem, ::_1)] >> +double_[boost::bind(&EType::push_back, &elem, ::_1) ]
                                  //uint_[boost::bind(&EType::set_id, &elem, ::_1)] >> ( double_ % space) [boost::bind(&EType::push_back, &elem, ::_1) ]
            //  End grammar
                                  , space
            );

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

        client::EType elem;
        if (client::parse_numbers(str.begin(), str.end(), elem))
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << str << " Parses OK: " << std::endl;

            for (std::vector<double>::size_type i = 0; i < elem.conn.size(); ++i)
                std::cout << i << ": " << elem.conn[i] << std::endl;

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
