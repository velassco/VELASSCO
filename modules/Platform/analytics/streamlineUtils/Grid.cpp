#include "Grid.h"
#include "helper.h"

#include "UnstructDataset.h"

// STD
#include <string>
#include <iostream>
#include <fstream>

void Grid::compileShader(){

}

/// builds the grid structure
void Grid::build( std::vector<AABB>& cellBoxes, std::vector<glm::i64>& cellPoints, std::vector<glm::i64>& cellPointsBegIndices )
{
  std::cout << "Creating grid acceleration structure..." << std::endl;
  m_n_levels = 0;
	AABB modelBox;
	for (size_t i = 0; i<cellBoxes.size(); i++)
    modelBox.extend(cellBoxes[i]);

  // Enlarge grid box to avoid primitives on boundaries
  const double EPSILON = 0.0001f;
  AABB accelBox = modelBox;
  accelBox.enlarge(EPSILON);

  reset(accelBox, 500, 500, 500);
  insertPrimitiveList(cellBoxes);

  std::cout << "Creating grid acceleration structure...Done" << std::endl;
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

bool Grid::loadAccelStruct(std::string filename){
  m_accelType = ACCEL_STRUCT_TYPE_GRID;

  std::ifstream in(filename.c_str(), std::ios_base::binary);
  if(in){

    std::cout << "Loading acceleration structures..." << std::endl;    

    in.read((char*)&m_bounds, sizeof(AABB));
    in.read((char*)&m_xDim, sizeof(size_t));
    in.read((char*)&m_yDim, sizeof(size_t));
    in.read((char*)&m_zDim, sizeof(size_t));

    std::cout << "Bounds = min: " << m_bounds.min[0] << " " << m_bounds.min[1] << " " << m_bounds.min[2] << std::endl
              << "         max: " << m_bounds.max[0] << " " << m_bounds.max[1] << " " << m_bounds.max[2] << std::endl
              << "         xDim = " << m_xDim << " , " << "yDim = " << m_yDim << " , zDim = " << m_zDim << std::endl;

    resize(m_xDim, m_yDim, m_zDim);

    for ( size_t i=0; i<m_xDim; i++ )
		  for ( size_t j=0; j<m_yDim; j++ )
			  for ( size_t k=0; k<m_zDim; k++ ){
          std::vector<glm::i64>& primitives = getPrimitives( i, j, k );
        
          size_t nPrimitives = 0;
          in.read((char*)&nPrimitives, sizeof(size_t));
          primitives.resize(nPrimitives);

          in.read((char*)primitives.data(), sizeof(glm::i64) * nPrimitives);
      }

      std::cout << "Loading acceleration structures...Done." << std::endl;
      return true;
  }

  return false;
}

bool Grid::saveAccelStruct(std::string filename){
  std::ofstream out(filename.c_str(), std::ios_base::binary);
  if(out){
    std::cout << "Writing acceleration structure..." << std::endl;
    out.write((char*)&m_bounds, sizeof(AABB));
    out.write((char*)&m_xDim, sizeof(size_t));
    out.write((char*)&m_yDim, sizeof(size_t));
    out.write((char*)&m_zDim, sizeof(size_t));

    for ( size_t i=0; i<m_xDim; i++ )
		  for ( size_t j=0; j<m_yDim; j++ )
		  	for ( size_t k=0; k<m_zDim; k++ ){
          std::vector<glm::i64>& primitives = getPrimitives( i, j, k );
        
          size_t nPrimitives = primitives.size();
          out.write((char*)&nPrimitives, sizeof(size_t));

          out.write((char*)primitives.data(), sizeof(glm::i64) * nPrimitives);
      }

    std::cout << "Writing acceleration structure...Done." << std::endl;
    return true;
  }

  return false;
}