#include "BinarySerialization.h"
#include <iostream>

int main()
{
  GID::BinarySerializerNative binserBig;

  binserBig.SetEndianness( GID::BigEndian );

  std::string buffer;

  const char *words[] = { "HOLA", "MUNDO" };

  for( int i = 0; i < sizeof( words ) / sizeof(words[0]); ++i )
    {
    int l = binserBig.Write( buffer, words[i] );
    std::cout << words[i] << " needed " << l << " bytes\n";
    }

  GID::BinaryDeserializerNative bindeserBig;
  bindeserBig.SetEndianness( GID::BigEndian );
  
  boost::uint32_t pos = 0;
  boost::uint32_t length = buffer.length();
  std::string str;
  while( pos < length )
    {
    str.clear();
    std::cout << " (" << pos << ") -- ";
    pos = bindeserBig.Read( buffer, str, pos );
    std::cout << str << "\n";
    }
  return 0;
}
