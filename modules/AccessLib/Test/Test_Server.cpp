#include <stdio.h>
#include <string.h>

// CLib
#include <cstdlib>

// VELaSSCo
#include "AccessLib.h"

#ifdef _WIN32
#ifndef strcasecmp
#define strcasecmp  _stricmp
#endif
#endif // _WIN32

bool askForHelp( const char *txt) {
  return !strcasecmp( txt, "-h") || !strcasecmp( txt, "--h") || !strcasecmp( txt, "-help") || !strcasecmp( txt, "--help");
}

int main(int argc, char* argv[])
{
  int listen_port = 9990; // default port
  if ( argc == 2) {
    if ( askForHelp( argv[ 1])) {
      printf( "Usage: %s [ listen_port (default %d)]\n", argv[ 0], listen_port);
      return EXIT_FAILURE;
    }
    int new_port = listen_port;
    int n = sscanf( argv[ 1], "%d", &new_port);
    if ( n == 1) // sscanf ok
      listen_port = new_port;
  }
  
  valStartTestServer( listen_port);
  
  return EXIT_SUCCESS;
}
