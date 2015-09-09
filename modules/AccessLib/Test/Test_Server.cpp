#include <stdio.h>
#include <string.h>

// CLib
#include <cstdlib>

// VELaSSCo
#include "AccessLib.h"

int main(int argc, char* argv[])
{
  int server_port = 9090; // default port
  if ( argc == 2) {
    if ( !strncasecmp( argv[ 1], "-h", 2) ||  !strncasecmp( argv[ 1], "--h", 3)) {
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
