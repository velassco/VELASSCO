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
  int server_port = 9090; // default port
  if ( argc == 2) {
    if ( askForHelp( argv[ 1])) {
      printf( "Usage: %s [ port]\n", argv[ 0]);
      return EXIT_FAILURE;
    }
    int n = sscanf( argv[ 1], "%d", &server_port);
    if ( n != 1) // some error in sscanf
      server_port = 9090; // default port
  }
  
  valStartTestServer( server_port);
  
  return EXIT_SUCCESS;
}
