#include "ParseAsciiMesh.h"
#include <iostream>

int main( int argc, char *argv[] )
{
  if ( argc != 2 )
    {
    std::cout << "Usage: " << argv[0] << "file.post.msh\n";
    return -1;
    }
  std::list<GID::MeshResultType> meshes;
  int status = GID::ParseMeshFile( argv[1], meshes );
  return status;
}
