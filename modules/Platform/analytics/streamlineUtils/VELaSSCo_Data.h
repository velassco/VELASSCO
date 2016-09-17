#ifndef __VELASSCO_DATA_H__
#define __VELASSCO_DATA_H__

#include <stdint.h>
#include <vector>

namespace VELASSCO {

  struct Coord {
    int64_t idx;
    double  coord[3];
  };

  struct Vector3D {
    int64_t idx;
    double  vector[3];
  };

  struct Cell {
    std::vector<int64_t> cornerIndices;
  };

}

#endif

