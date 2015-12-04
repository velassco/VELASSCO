#include <iostream>
#include <sstream>
#include <iomanip>
#include "BinarySerialization.h"

std::string to_hex( const std::string &buffer )
{
  std::stringstream str;
  for (int i = 0; i < buffer.length(); i++) 
    {
    str << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i];
    }
  return str.str();
}

int main()
{
  GID::BinarySerializerNative binserBig;
  GID::BinarySerializerNative binserLittle;

  binserBig.SetEndianness( GID::BigEndian );
  binserLittle.SetEndianness( GID::LittleEndian );

  std::string buffer;

  binserBig.Write( buffer, 10L );
  std::cout << "1 in big-endian is    " << to_hex( buffer ) << std::endl;
  buffer.clear();
  binserLittle.Write( buffer, 10L );
  std::cout << "1 in little-endian is " << to_hex( buffer ) << std::endl;
  
  return 0;
}
