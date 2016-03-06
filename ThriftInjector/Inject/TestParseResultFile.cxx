#include "ParseAsciiResult.h"
#include <iostream>

int main( int argc, char *argv[] )
{
  if ( argc != 2 )
    {
    std::cout << "Usage: " << argv[0] << "file.post.res\n";
    return -1;
    }

  GID::ResultContainerType resultPart;
  int status = GID::ParseResultFile( argv[1], resultPart );
  return 0;
}
