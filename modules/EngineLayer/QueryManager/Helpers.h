
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
// to know that the messages belongs to the EngineLayer
#	define LOGGER (std::cerr << "[VELaSSCo-EL] ")
#else
#	define LOGGER (std::ostream(0))
#endif
#define DEBUG( a)   LOGGER << a << std::endl;

#define PING					\
  do						\
    {						\
      LOGGER << FUNCTION_NAME << std::endl;	\
    }						\
  while (0)

// ---------------------------------------------------------------------------

// not using namespace in DataLayer at the moment
// namespace VELaSSCo
// {

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

// not using namespace in DataLayer at the moment
// }
