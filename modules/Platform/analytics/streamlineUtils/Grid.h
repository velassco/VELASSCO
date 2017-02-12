
/**
 *
 * Grid acceleration structure
 *
 * This class indexes AABBs in a regular grid structure.
 *
 */
 
 #ifndef __GRID_H__
 #define __GRID_H__

// STD
#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

// RPE
#include "AccelStruct.h"
#include "AABB.h"

#include <iostream>

#ifdef WIN32
#    define GRID_INLINE __forceinline
#else
#    define GRID_INLINE inline
#endif

class UnstructDataset;

class Grid: public AccelStruct
{
public:

  static void compileShader();

	/// builds the grid structure
	virtual void build( std::vector<AABB>& cellBoxes, std::vector<glm::i64>& cellPoints, std::vector<glm::i64>& cellPointsBegIndices );

	/// return the bounding primitive indices
  virtual bool getBoundingPrimitiveIndex(const UnstructDataset* const datasetPtr, const double point[3], glm::i64& cell_index);

	/// Create an uninitialized grid.
	GRID_INLINE Grid( void );

	/// Create a grid from an AABB and automatically compute dimensions based on a granularity factor.
	GRID_INLINE Grid( const AABB &bounds, const double granularity=1000.0f );

	/// Create a grid from an AABB and with given dimensions.
	GRID_INLINE Grid( const AABB &bounds, const size_t xDim, const size_t yDim, const size_t zDim );

	/// Reset the grid with a new AABB and automatically compute dimensions based on a granularity factor.
	GRID_INLINE void reset( const AABB &bounds, const double granularity=1000.0f );

	/// Reset the grid with a new AABB and with given dimensions.
	GRID_INLINE void reset( const AABB &bounds, const size_t xDim, const size_t yDim, const size_t zDim );

	/// Resize the grid's dimensions, but keep the bounding box. All contained data is cleared.
	GRID_INLINE void resize( const size_t xDim, const size_t yDim, const size_t zDim );

	/// Insert a primitive index into the grid cell which contains the given primitive box.
	GRID_INLINE void insertPrimitive( const AABB &primitiveBox, const glm::i64 index );

	/// Insert a list of primitives into the grid depending on their boxes. Primitive indices are list indices.
	GRID_INLINE void insertPrimitiveList( const std::vector<AABB> &primitiveBoxes );

	/// Find a grid cell that contains a given point. Result is undefined if the point is out of bounds.
	GRID_INLINE void locateCell( size_t &i, size_t &j, size_t &k, const double point[3] ) const;

	/// Return false if the specified cell contains at least one primitive index.
	GRID_INLINE bool emptyCell( const size_t i, const size_t j, const size_t k ) const;

	/// Get the list of primitive indices stored in a given cell.
	GRID_INLINE std::vector<glm::i64>& getPrimitives( const size_t i, const size_t j, const size_t k );

  GRID_INLINE glm::dvec3 getElementExtents() const;

  // Performance Measurment
  void resetPerforamnceStats() {
    m_avg_check_per_traverse = 0.0f;
    m_n_traverses = 0;
  }

  void showPerformanceStats() {
    std::cout << "Average Checks Per Traverse = " << m_avg_check_per_traverse << std::endl;
  }

protected:

	AABB m_bounds;
	size_t m_xDim, m_yDim, m_zDim;
  
	std::vector< std::vector<glm::i64> > m_cells;
};

GRID_INLINE Grid::Grid( void ):  AccelStruct(ACCEL_STRUCT_TYPE_GRID)
{
	m_xDim = m_yDim = m_zDim = 0;
}

GRID_INLINE Grid::Grid( const AABB &bounds, const double granularity ):  AccelStruct(ACCEL_STRUCT_TYPE_GRID)
{
	reset( bounds, granularity );
}
GRID_INLINE Grid::Grid( const AABB &bounds, const size_t xDim, const size_t yDim, const size_t zDim ):  AccelStruct(ACCEL_STRUCT_TYPE_GRID)
{
	reset( bounds, xDim, yDim, zDim );
}


GRID_INLINE void Grid::reset( const AABB &bounds, const double granularity )
{
	reset(bounds,
		    (size_t)(granularity * (bounds.max[0] - bounds.min[0])),
   		    (size_t)(granularity * (bounds.max[1] - bounds.min[1])),
		    (size_t)(granularity * (bounds.max[2] - bounds.min[2])) );
}

GRID_INLINE void Grid::reset( const AABB &bounds, const size_t xDim, const size_t yDim, const size_t zDim )
{
	m_bounds = bounds;

	resize( xDim, yDim, zDim );
}

GRID_INLINE void Grid::resize( const size_t xDim, const size_t yDim, const size_t zDim )
{
	assert( xDim>0 && yDim>0 && zDim>0 );

	m_xDim = xDim;
	m_yDim = yDim;
	m_zDim = zDim;

	m_cells.clear();
	m_cells.resize( xDim * yDim * zDim );
}

GRID_INLINE void Grid::insertPrimitive( const AABB &primitiveBox, const glm::i64 index )
{
	size_t iMin, jMin, kMin;
	size_t iMax, jMax, kMax;

	locateCell( iMin, jMin, kMin, (double*)&primitiveBox.min );
	locateCell( iMax, jMax, kMax, (double*)&primitiveBox.max );

	for ( size_t i=iMin; i<=iMax; i++ )
		for ( size_t j=jMin; j<=jMax; j++ )
			for ( size_t k=kMin; k<=kMax; k++ )
				getPrimitives( i, j, k ).push_back( index );
}

GRID_INLINE void Grid::insertPrimitiveList( const std::vector<AABB> &primitiveBoxes )
{
  m_occupied_memory     = 0;
  m_avg_cells_per_leaf  = 0.0f;
  m_n_non_empty_leaf    = 0;
  m_max_cells_per_leaf  = 0;

	for ( size_t i=0; i<primitiveBoxes.size(); i++ ){
    //if(i % 100000 == 0) std::cout << (double)i / (primitiveBoxes.size()) << std::endl;
		insertPrimitive( primitiveBoxes[i], (glm::i64)i );
  }

  for ( size_t i=0; i<m_xDim; i++ )
		for ( size_t j=0; j<m_yDim; j++ )
			for ( size_t k=0; k<m_zDim; k++ ){
        if(getPrimitives( i, j, k ).size() > m_max_cells_per_leaf){
          m_max_cells_per_leaf = static_cast<int>( getPrimitives(i, j, k).size() );
        }
        if(getPrimitives( i, j, k ).size() > 0)
          m_n_non_empty_leaf++;
        m_avg_cells_per_leaf += getPrimitives( i, j, k ).size();
        m_occupied_memory += getPrimitives( i, j, k ).size() * sizeof(glm::i64);
      }

  m_avg_cells_per_leaf /= m_n_non_empty_leaf;

}

GRID_INLINE void Grid::locateCell( size_t &i, size_t &j, size_t &k, const double point[3] ) const
{
	i = (size_t)( (double)m_xDim * (point[0] - m_bounds.min[0]) / (m_bounds.max[0] - m_bounds.min[0]) );
	j = (size_t)( (double)m_yDim * (point[1] - m_bounds.min[1]) / (m_bounds.max[1] - m_bounds.min[1]) );
	k = (size_t)( (double)m_zDim * (point[2] - m_bounds.min[2]) / (m_bounds.max[2] - m_bounds.min[2]) );
}

GRID_INLINE bool Grid::emptyCell( const size_t i, const size_t j, const size_t k ) const
{
	assert( i<m_xDim && j<m_yDim && k<m_zDim );
	
	const size_t index = i + ( j * m_xDim ) + ( k * m_xDim*m_yDim );
	return ( m_cells[index].size() == 0 );
}

GRID_INLINE std::vector<glm::i64>& Grid::getPrimitives( const size_t i, const size_t j, const size_t k )
{
	assert( i<m_xDim && j<m_yDim && k<m_zDim );

	const size_t index = i + ( j * m_xDim ) + ( k * m_xDim*m_yDim );
	return m_cells[index];
}

GRID_INLINE glm::dvec3 Grid::getElementExtents() const {
  return glm::dvec3( 
    (m_bounds.max.x - m_bounds.min.x) / m_xDim,
    (m_bounds.max.y - m_bounds.min.y) / m_yDim,
    (m_bounds.max.z - m_bounds.min.z) / m_zDim);
}

#endif
