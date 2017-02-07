#ifndef __FOAM_DATASET_H__
#define __FOAM_DATASET_H__

// GLM
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

// STD
#include <vector>
#include "AABB.h"
#include "AccelStruct.h"

// VELASSCO
#include "VELaSSCo_Data.h"

class UnstructDataset {
public:
  UnstructDataset();
  ~UnstructDataset();

  void init();
  void reset(const std::vector<VELASSCO::Coord>& coords, const std::vector<VELASSCO::Vector3D>& results, const std::vector<VELASSCO::Cell> cells);
  
  virtual glm::dvec3 derivate(const glm::dvec3& point);
  virtual bool contains(const glm::dvec3& point);

  bool insideCell(glm::i64 idx, const glm::dvec3& p) const;
  bool insideCellBoundingBox(glm::i64 cellIndex, const glm::dvec3& point) const;

  void computeAccel(AccelStructType accel_type);

 /* virtual double chooseInitialStepSize(const glm::dvec3& p);
  virtual bool snapToMeshSurface(glm::dvec3& p);*/

  // Performance
  void resetPerformanceStats();
  void showPerformanceStats();

private: 
  glm::dvec3 interpolateDerivateInCell(const glm::i64& cellIndex, const glm::dvec3& point);

  AccelStruct* m_sceneAccel;
  AABB         m_modelBox;

  std::vector<AABB>       m_cellBoxes;

  std::vector<glm::dvec3> m_points;
  std::vector<glm::dvec3> m_pointVectors;

  std::vector<glm::i64> m_cellPoints;
  std::vector<glm::i64> m_cellPointsBegIndices;
};
#endif
