
#pragma once

// CLib
#include <cctype>

// STL
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

// Boost
#include <boost/regex.hpp>

// ---------------------------------------------------------------------------

#ifndef FUNCTION_NAME
#   define FUNCTION_NAME ""
#   ifdef __PRETTY_FUNCTION__
#		undef FUNCTION_NAME
#		define FUNCTION_NAME __PRETTY_FUNCTION__
#	endif
#	ifdef __FUNCTION__
#		undef FUNCTION_NAME
#		define FUNCTION_NAME __FUNCTION__
#	endif
#	ifdef __func__
#		undef FUNCTION_NAME
#		define FUNCTION_NAME __func__
#	endif
#endif

#define ENABLE_LOGGING
#ifdef ENABLE_LOGGING
#	define LOGGER (std::cerr << "[VELaSSCo] ")
#else
#	define LOGGER (std::ostream(0))
#endif
#define DEBUG( a)   LOGGER << ( a) << std::endl;

#define PING                                  \
	do                                        \
	{                                         \
		LOGGER << FUNCTION_NAME << std::endl; \
	}                                         \
	while (0)

// ---------------------------------------------------------------------------

namespace VELaSSCo
{

/**
 * Converts a std::string containing binary to a hex dump ASCII string.
 */ 

inline std::string Hexdump(const std::string input)
{
	std::stringstream out;

	for (size_t i=0; i<input.size(); i+=16)
	{
		out << std::hex << std::setw(2*sizeof(size_t)) << std::setfill('0') << (size_t)i << ": ";
	    for (size_t j=0; j<16; j++) 
			if (i+j < input.size())
				out << std::setw(2) << (unsigned)(unsigned char)(input[i+j]) << " ";
			else
				out << "   ";

		out << " ";
	 	for (size_t j=0; j<16; j++) 
			if (i+j < input.size())
				if (isprint((unsigned char)input[i+j]))
					out << input[i+j];
				else
					out << '.';

		out << std::endl;
	}

	return out.str();
}

// ---------------------------------------------------------------------------
	
/**
 * Separates a URL string into substrings like host, port, etc.
 */

class URL
{
public:

	URL(std::string url) : m_url(url) 
	{
		const std::string URL_FORMAT = "((?<scheme>[a-zA-Z][a-zA-Z0-9+.-]*)://)?((?<userinfo>[^@]*)@)?(?<host>[a-zA-Z0-9.-]*)(:(?<port>[\\d]{1,5}))?(?<path>[/\\\\][^?#]*)?(\\?(?<query>[^#]*))?(#(?<fragment>.*))?";

		boost::regex expression(URL_FORMAT, boost::regex::icase);
		boost::smatch matches;
		
		if ( boost::regex_match(m_url, matches, expression) )
		{
			m_scheme   = matches["scheme"];
			m_userinfo = matches["uersinfo"];
			m_host     = matches["host"];
			m_port     = matches["port"];
			m_part     = matches["part"];
			m_query    = matches["query"];
			m_fragment = matches["fragment"];
		}
	};

	std::string Scheme()   { return m_scheme;   }
	std::string Userinfo() { return m_userinfo; }
	std::string Host()     { return m_host;     }
	std::string Port()     { return m_port;     }
	std::string Part()     { return m_part;     }
	std::string Query()    { return m_query;    }
	std::string Fragment() { return m_fragment; }

private:
	
	std::string m_url;

	std::string m_scheme;
	std::string m_userinfo;
	std::string m_host;
	std::string m_port;
	std::string m_part;
	std::string m_query;
	std::string m_fragment;
};

}
