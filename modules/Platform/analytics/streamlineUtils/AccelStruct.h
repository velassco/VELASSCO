#ifndef __ACCEL_STRUCT_H__
#define __ACCEL_STRUCT_H__

#include <iostream>
#include <vector>

#include "AABB.h"

#ifdef WIN32
#	define ACCEL_STRUCT_INLINE	__forceinline
#else
#	define ACCEL_STRUCT_INLINE inline
#endif

enum AccelStructType {
	ACCEL_STRUCT_TYPE_GRID		  = 0,
	ACCEL_STRUCT_TYPE_BVH_TREE	= 1,
  ACCEL_STRUCT_TYPE_KD_TREE   = 2
};


class UnstructDataset;

class AccelStruct {
public:

	ACCEL_STRUCT_INLINE AccelStruct(AccelStructType accelType);
	ACCEL_STRUCT_INLINE virtual ~AccelStruct();

	virtual void build( std::vector<AABB>& cellBoxes, std::vector<glm::i64>& cellPoints, std::vector<glm::i64>& cellPointsBegIndices ) = 0;
  virtual bool getBoundingPrimitiveIndex(const UnstructDataset* const datasetPtr, const double point[3], glm::i64& cell_index) = 0;

	void setAsDirty()	{	dirty = true;	}

	ACCEL_STRUCT_INLINE AccelStructType getAccelStructType() const;

  virtual glm::dvec3 getElementExtents() const = 0; 

  // Performance Measurment
  void resetPerforamnceStats() {
    m_avg_check_per_traverse = 0.0f;
    m_n_traverses = 0;
  }

  void showPerformanceStats() {
    std::cout << "Average Checks Per Traverse = " << m_avg_check_per_traverse << std::endl;
  }
  void showAccelInfo(){
    // Performance output
    std::cout << "Occupied Memory = " << m_occupied_memory << std::endl;
    std::cout << "Tree Levels = " << m_n_levels << std::endl;
    std::cout << "Max Cells per Leaf = " << m_max_cells_per_leaf << " Avg. Cells per Leaf = " << m_avg_cells_per_leaf << std::endl;
  }

protected:
	AccelStructType m_accelType;
	bool dirty;

  // Peformance
  int m_max_cells_per_leaf;
  double m_avg_cells_per_leaf;
  uint32_t m_n_non_empty_leaf;
  int m_n_levels;
  uint64_t m_occupied_memory;

  double m_avg_check_per_traverse;
  uint32_t m_n_traverses;
};

ACCEL_STRUCT_INLINE AccelStruct::AccelStruct(AccelStructType accelType) {
	m_accelType = accelType;
}

ACCEL_STRUCT_INLINE AccelStructType AccelStruct::getAccelStructType() const {
	return m_accelType;
}

ACCEL_STRUCT_INLINE AccelStruct::~AccelStruct() {
}

#endif