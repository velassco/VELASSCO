#include "glm/glm.hpp"

#include "StreamTracer.h"
#include "Streamline.h"
#include "UnstructDataset.h"

#include "VELaSSCo_Data.h"
#include "main.h"

int main() {

  std::vector<VELASSCO::Coord>    test_coords;
  std::vector<VELASSCO::Vector3D> test_results;
  std::vector<VELASSCO::Cell>     test_cells;

  test_coords.resize(9);
  test_results.resize(9);
  test_cells.resize(1);

  for (glm::i64 i = 0; i < 8; i++) {
    test_coords[i].idx = i;
    test_cells[0].cornerIndices.push_back(i);
  }

  test_coords[0].coord[0] = -1.0; test_coords[0].coord[1] = -1.0; test_coords[0].coord[2] = -1.0;
  test_coords[1].coord[0] =  1.0; test_coords[1].coord[1] = -1.0; test_coords[1].coord[2] = -1.0;
  test_coords[2].coord[0] = -1.0; test_coords[2].coord[1] =  1.0; test_coords[2].coord[2] = -1.0;
  test_coords[3].coord[0] =  1.0; test_coords[3].coord[1] =  1.0; test_coords[3].coord[2] = -1.0;
  test_coords[4].coord[0] = -1.0; test_coords[4].coord[1] = -1.0; test_coords[4].coord[2] =  1.0;
  test_coords[5].coord[0] =  1.0; test_coords[5].coord[1] = -1.0; test_coords[5].coord[2] =  1.0;
  test_coords[6].coord[0] = -1.0; test_coords[6].coord[1] =  1.0; test_coords[6].coord[2] =  1.0;
  test_coords[7].coord[0] =  1.0; test_coords[7].coord[1] =  1.0; test_coords[7].coord[2] =  1.0;
  test_coords[8].coord[0] =  0.0; test_coords[8].coord[1] =  0.0; test_coords[8].coord[2] =  0.0;
  
  test_results[0].vector[0] = -1.0; test_results[0].vector[1] = -1.0; test_results[0].vector[2] = -1.0;
  test_results[1].vector[0] =  1.0; test_results[1].vector[1] = -1.0; test_results[1].vector[2] = -1.0;
  test_results[2].vector[0] = -1.0; test_results[2].vector[1] =  1.0; test_results[2].vector[2] = -1.0;
  test_results[3].vector[0] =  1.0; test_results[3].vector[1] =  1.0; test_results[3].vector[2] = -1.0;
  test_results[4].vector[0] = -1.0; test_results[4].vector[1] = -1.0; test_results[4].vector[2] =  1.0;
  test_results[5].vector[0] =  1.0; test_results[5].vector[1] = -1.0; test_results[5].vector[2] =  1.0;
  test_results[6].vector[0] = -1.0; test_results[6].vector[1] =  1.0; test_results[6].vector[2] =  1.0;
  test_results[7].vector[0] =  1.0; test_results[7].vector[1] =  1.0; test_results[7].vector[2] =  1.0;
  test_results[8].vector[0] =  0.0; test_results[8].vector[1] =  0.0; test_results[8].vector[2] =  0.0;

  std::vector<Streamline> streamlines;

  streamlines.push_back(Streamline(glm::dvec3(0.0)));

  UnstructDataset dataset;

  dataset.reset(test_coords, test_results, test_cells);

  StreamTracer tracer;
  tracer.traceStreamline(&dataset, streamlines, 0.001);

  return 0;
}
