#ifndef _BASE_64_H_
#define _BASE_64_H_
#include <string>

std::string base64_encode( char const* );
std::string base64_encode( char const* , size_t len);
std::string base64_decode(std::string const& s);
inline std::string base64_decode( const char *data) {
  return data ? base64_decode( std::string( data)) : "(null)";
}
#endif // _BASE_64_H_
