#include "Grid.h"
#include "helper.h"

#include "UnstructDataset.h"

void Grid::compileShader(){

}

/// builds the grid structure
void Grid::build( std::vector<AABB>& cellBoxes, std::vector<glm::i64>& cellPoints, std::vector<glm::i64>& cellPointsBegIndices )
{
  m_n_levels = 0;
	AABB modelBox;
	for (size_t i = 0; i<cellBoxes.size(); i++)
    modelBox.extend(cellBoxes[i]);

  // Enlarge grid box to avoid primitives on boundaries
  const double EPSILON = 0.0001f;
  AABB accelBox = modelBox;
  accelBox.enlarge(EPSILON);

  reset(accelBox, 100, 100, 100);
  insertPrimitiveList(cellBoxes);
}

bool Grid::getBoundingPrimitiveIndex( const UnstructDataset* const datasetPtr, const double point[3], glm::i64& cell_index )
{
	if (!m_bounds.contains(point)) return false;

	size_t i, j, k;
    locateCell(i, j, k, point);
	if(emptyCell(i, j, k))
		return false;

  int n_checks = 0;
  m_n_traverses++;
  size_t t = 0;
  std::vector<glm::i64>& primitives = getPrimitives(i, j, k);
  glm::i64 n_primitives = static_cast<glm::i64>(primitives.size());
  glm::dvec3 p(point[0], point[1], point[2]);
  
  for (glm::i64 i = 0; i < n_primitives; i++)
  {
    n_checks++;
    if (datasetPtr->insideCell(primitives[i], p))
    {
      cell_index = primitives[i];
      m_avg_check_per_traverse = (m_avg_check_per_traverse * (m_n_traverses - 1) + n_checks) / m_n_traverses;
      return true;
    }
  }

  for (glm::i64 i = 0; i < n_primitives; i++)
  {
    n_checks++;
    if (datasetPtr->insideCellBoundingBox(primitives[i], p))
    {
      cell_index = primitives[i];
      m_avg_check_per_traverse = (m_avg_check_per_traverse * (m_n_traverses - 1) + n_checks) / m_n_traverses;
      return true;
    }
  }

  m_avg_check_per_traverse = (m_avg_check_per_traverse * (m_n_traverses - 1) + n_checks) / m_n_traverses;
	return false; 
}