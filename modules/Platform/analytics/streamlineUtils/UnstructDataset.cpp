#include "UnstructDataset.h"

#include "Grid.h"

// Boost
//#include <boost/algorithm/string.hpp>
//#include <boost/filesystem.hpp>
//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_real_distribution.hpp>


#include "helper.h"

// Geomteric Helper
#include "GeometricHelperTool.h"

// STD
#include <map>

UnstructDataset::UnstructDataset() : m_sceneAccel(0) {
}

UnstructDataset::~UnstructDataset() {

}

void UnstructDataset::init() {

}

void UnstructDataset::reset(const std::vector<VELASSCO::Coord>& coords, const std::vector<VELASSCO::Vector3D>& results, const std::vector<VELASSCO::Cell> cells)
{
  assert(coords.size() == results.size());

  m_cellBoxes.clear();
  m_points.clear();
  m_pointVectors.clear();
  m_cellPoints.clear();
  m_cellPointsBegIndices.clear();

  std::map<glm::i64, glm::i64> vertexID_map;
  for (size_t i = 0; i < coords.size(); i++) {
    m_points.push_back(glm::dvec3(coords[i].coord[0], coords[i].coord[1], coords[i].coord[2]));
    vertexID_map[coords[i].idx] = static_cast<glm::i64>(i);
  }

  m_pointVectors.resize(results.size());
  for (size_t i = 0; i < results.size(); i++) {
    glm::dvec3 result = glm::dvec3(results[i].vector[0], results[i].vector[1], results[i].vector[2]);
    m_pointVectors[vertexID_map[coords[i].idx]] = result;
  }

  m_cellPointsBegIndices.resize(cells.size());
  for (size_t c = 0; c < cells.size(); c++) {
    m_cellPointsBegIndices[c] = static_cast<glm::i64>(m_cellPoints.size());
    m_cellPoints.push_back(static_cast<glm::i64>(cells[c].cornerIndices.size()));
    for (size_t i = 0; i < cells[c].cornerIndices.size(); i++) {
      m_cellPoints.push_back(cells[c].cornerIndices[i]);
    }
  }

  m_cellBoxes.resize(cells.size());
  for (size_t c = 0; c < cells.size(); c++) {
    glm::i64 nCellPoints = m_cellPoints[m_cellPointsBegIndices[c]];
    for (glm::i64 i = 1; i <= nCellPoints; i++) {
      glm::dvec3 p = m_points[m_cellPoints[m_cellPointsBegIndices[c] + i]];
      m_cellBoxes[c].extend(p[0], p[1], p[2]);
    }
  }

  computeAccel(ACCEL_STRUCT_TYPE_GRID);
}

glm::dvec3 UnstructDataset::derivate(const glm::dvec3& p)
{
  glm::dvec3 point = p;
  glm::dvec3 d;

  glm::i64 prim_idx;
	if( !m_sceneAccel->getBoundingPrimitiveIndex(this, (double*)&point, prim_idx) )
		return glm::dvec3(0.0f, 0.0f, 0.0f);

  d = interpolateDerivateInCell(prim_idx, point);
  
  return d;
}

bool UnstructDataset::contains(const glm::dvec3 & point)
{
  return m_modelBox.contains((double*)&point);
}

glm::dvec3 UnstructDataset::interpolateDerivateInCell(const glm::i64& cellIndex, const glm::dvec3& point)
{
    if (m_cellPoints[m_cellPointsBegIndices[cellIndex]] == 4)
    {
      double barycoords[4];
      glm::dvec3 cellpoints[4] = {
        m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 1]],
        m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 2]],
        m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 3]],
        m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 4]]
      };

      const glm::dvec3 v1v0 = cellpoints[1] - cellpoints[0];
      const glm::dvec3 v2v0 = cellpoints[2] - cellpoints[0];
      const glm::dvec3 v3v2 = cellpoints[3] - cellpoints[2];
      const glm::dvec3 pv0 = point - cellpoints[0];

      double tetvolumeinv = 1.0f / glm::dot(v2v0, glm::cross(v1v0, v3v2));

      // calculate the barycentric coordinates
      barycoords[0] = glm::dot(cellpoints[2] - point, glm::cross(cellpoints[1] - point, v3v2)) * tetvolumeinv;
      barycoords[1] = glm::dot(v2v0, glm::cross(pv0, v3v2)) * tetvolumeinv;
      barycoords[2] = glm::dot(pv0, glm::cross(v1v0, cellpoints[3] - point)) * tetvolumeinv;
      barycoords[3] = 1.0f - barycoords[0] - barycoords[1] - barycoords[2];

      // compute barycentric interpolation
      return	barycoords[0] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 1]] +
              barycoords[1] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 2]] +
              barycoords[2] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 3]] +
              barycoords[3] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 4]];

    } else if (m_cellPoints[m_cellPointsBegIndices[cellIndex]] == 5){

      glm::dvec3 p0 = m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 1]];
      glm::dvec3 p_p0 = point - p0;

      const glm::dvec3 r_unit = glm::normalize( m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 2]] - p0 );
      const glm::dvec3 s_unit = glm::normalize( m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 4]] - p0 );
      const glm::dvec3 t_unit = glm::normalize( glm::cross(r_unit, s_unit) );
      const glm::float64 h   = glm::dot( t_unit, m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 5]] - p0 );

      double r = glm::dot(p_p0, r_unit);
      double s = glm::dot(p_p0, s_unit);
      double t = glm::dot(p_p0, t_unit) / h;

      // Point is outside of the pyramid     
      if( t < 0.0f || t > 1.0f )
        return glm::dvec3(0.0f, 0.0f, 0.0f);

      double W[5] = {
        (1 - r) * (1 - s) * (1 - t),
        r * (1 - s) * (1 - t),
        r * s * (1 - t),
        (1 - r) * s * (1 - t),
        t
      };

      glm::dvec3 deriv = 
        W[0] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 1]] +
        W[1] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 2]] +
        W[2] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 3]] +
        W[3] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 4]] +
        W[4] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 5]] ;

		  return deriv;

    } else if (m_cellPoints[m_cellPointsBegIndices[cellIndex]] == 8){

      glm::dvec3 p0   = m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 1]];
      glm::dvec3 p_p0 = point - p0;
      double 
        r = glm::dot(p_p0, glm::normalize(m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 2]] - p0)), 
        s = glm::dot(p_p0, glm::normalize(m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 4]] - p0)), 
        t = glm::dot(p_p0, glm::normalize(m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 5]] - p0));

      double W[8] = {
        (1 - r) * (1 - s) * (1 - t),    // W[0]
        r * (1 - s) * (1 - t),          // W[1]
        r * s * (1 - t),                // W[2]
        (1 - r) * s * (1 - t),          // W[3]
        (1 - r) * (1 - s) * t,          // W[4]
        r * (1 - s) * t,                // W[5]
        r * s * t,                      // W[6]
        (1 - r) * s * t                 // W[7]
      };

      glm::dvec3 deriv = 
        W[0] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 1]] +
        W[1] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 2]] +
        W[2] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 3]] +
        W[3] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 4]] +
        W[4] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 5]] +
        W[5] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 6]] +
        W[6] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 7]] +
        W[7] * m_pointVectors[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 8]];

		  return deriv;
	  }
    
    return glm::dvec3(0.0f, 0.0f, 0.0f);
}

bool UnstructDataset::insideCellBoundingBox(glm::i64 cellIndex, const glm::dvec3& point) const {

  if (m_cellBoxes[cellIndex].contains((double*)&point))
    return true;
  return false;

}

void UnstructDataset::computeAccel(AccelStructType accel_type)
{
  if (m_sceneAccel)	delete m_sceneAccel;

  std::cout << "Computing Acceleration Structure...";
  for (size_t i = 0; i<m_cellBoxes.size(); i++)
    m_modelBox.extend(m_cellBoxes[i]);

  switch (accel_type) {
  case ACCEL_STRUCT_TYPE_GRID:
    m_sceneAccel = new Grid();
    break;

  //case ACCEL_STRUCT_TYPE_BVH_TREE:
  //  m_sceneAccel = new BVHTree();
  //  static_cast<BVHTree*>(m_sceneAccel)->init(4, BVHTree::SplitMethod::SPLIT_SAH);
  //  break;

  //case ACCEL_STRUCT_TYPE_KD_TREE:
  //  m_sceneAccel = new KDTree();
  //  static_cast<KDTree*>(m_sceneAccel)->init(1, 1, 0.3f, 10);
  }

  m_sceneAccel->build(m_cellBoxes, m_cellPoints, m_cellPointsBegIndices);
  m_sceneAccel->showAccelInfo();

  std::cout << "Done\n";
}

bool UnstructDataset::insideCell(glm::i64 cellIndex, const glm::dvec3& point) const {

#ifdef STUPID_TRACING
      if( m_cellBoxes[cellIndex].contains( (double*)&point ) )
        return true;
      return false;
#endif

  if (!m_cellBoxes[cellIndex].contains((double*)&point))
    return false;

	if (m_cellPoints[m_cellPointsBegIndices[cellIndex]] == 4)
  {  
    double barycoords[4];
    glm::dvec3 cellpoints[4] = {
        m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 1]],
        m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 2]],
        m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 3]],
        m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 4]]
    };

    const glm::dvec3 v1v0 = cellpoints[1] - cellpoints[0];
    const glm::dvec3 v2v0 = cellpoints[2] - cellpoints[0];
    const glm::dvec3 v3v2 = cellpoints[3] - cellpoints[2];
    const glm::dvec3 pv0 = point - cellpoints[0];

    double tetvolumeinv = 1.0f / glm::dot(v2v0, glm::cross(v1v0, v3v2));

    // calculate the barycentric coordinates
    barycoords[0] = glm::dot(cellpoints[2] - point, glm::cross(cellpoints[1] - point, v3v2)) * tetvolumeinv;
    barycoords[1] = glm::dot(v2v0, glm::cross(pv0, v3v2)) * tetvolumeinv;
    barycoords[2] = glm::dot(pv0, glm::cross(v1v0, cellpoints[3] - point)) * tetvolumeinv;
    barycoords[3] = 1.0f - barycoords[0] - barycoords[1] - barycoords[2];

    // if the point is in the tetrahedron
    if (barycoords[0] < 0.0f || barycoords[1] < 0.0f || barycoords[2] < 0.0f || barycoords[3] < 0.0f)
        return false;

		return true;

  }
  
  else if (m_cellPoints[m_cellPointsBegIndices[cellIndex]] == 5){

    glm::dvec3 p0 = m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 1]];
    glm::dvec3 p_p0 = point - p0;

    const glm::dvec3 r_unit = glm::normalize( m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 2]] - p0 );
    const glm::dvec3 s_unit = glm::normalize( m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 4]] - p0 );
    const glm::dvec3 t_unit = glm::normalize( glm::cross(r_unit, s_unit) );
    const glm::float64 h   = glm::dot(t_unit, m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 5]] - p0);

    const double r = glm::dot(p_p0, r_unit);
    const double s = glm::dot(p_p0, s_unit);
    const double t = glm::dot(p_p0, t_unit) / h;

    // Point is outside of the pyramid     
    if( t < 0.0f || t > 1.0f )
      return false;

    const double W[5] = {
      (1 - r) * (1 - s) * (1 - t),
      r * (1 - s) * (1 - t),
      r * s * (1 - t),
      (1 - r) * s * (1 - t),
      t
    };

    for(int i = 0; i < 5; i++)
      if(W[i] > 1.0f || W[i] < 0.0f)
        return false;

		return true;
  }
  
  else if (m_cellPoints[m_cellPointsBegIndices[cellIndex]] == 8){

      return true;

      glm::dvec3 p0   = m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 1]];
      glm::dvec3 p_p0 = point - p0;
      double 
        r = glm::dot(p_p0, glm::normalize(m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 2]] - p0)), 
        s = glm::dot(p_p0, glm::normalize(m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 4]] - p0)), 
        t = glm::dot(p_p0, glm::normalize(m_points[m_cellPoints[m_cellPointsBegIndices[cellIndex] + 5]] - p0));

      double W[8] = {
        (1 - r) * (1 - s) * (1 - t),    // W[0]
        r * (1 - s) * (1 - t),          // W[1]
        r * s * (1 - t),                // W[2]
        (1 - r) * s * (1 - t),          // W[3]
        (1 - r) * (1 - s) * t,          // W[4]
        r * (1 - s) * t,                // W[5]
        r * s * t,                      // W[6]
        (1 - r) * s * t                 // W[7]
      };

      for(int i = 0; i < 8; i++)
        if(W[i] > 1.0f || W[i] < 0.0f)
          return false;

    return true;
	}

  return false;

}

// Performance
void UnstructDataset::resetPerformanceStats() {
  m_sceneAccel->resetPerforamnceStats();
}

void UnstructDataset::showPerformanceStats() {
  m_sceneAccel->showPerformanceStats();
}

//bool UnstructDataset::seedIsValid(glm::dvec3 seed) {
//    size_t i, j, k;
//    return seedIsValid(seed, i, j, k);
//}
//
////bool UnstructDataset::seedIsValid(glm::dvec3 seed, size_t &i, size_t &j, size_t &k)
//bool UnstructDataset::seedIsValid(glm::dvec3 seed)
//{
//    double *point = (double*)(&seed);
//
//    if (!m_modelBox.contains(point))
//        return false;
//
//    /*m_sceneAccel.locateCell(i, j, k, point);
//    return !(m_sceneAccel.emptyCell(i, j, k));*/
//
//	m_sceneAccel.getBoundingPrimitiveIndex(seed)
//}

// ==========================
// == Advanced Streamlines ==
// ==========================

//double FOAMdataset::chooseInitialStepSize(const glm::dvec3& p) {
//
//  // use bbox approach as default in the first place
//  double stepSize = getModelMinExtend() * 0.001f /*initialStepSizeFactor.getValue()*/;
//
//  // Get minimum grid element size
//  glm::dvec3 elementExtents = m_sceneAccel->getElementExtents();
//  double minDist = std::min(elementExtents.x, std::min(elementExtents.y, elementExtents.z));
//
//  // travel at most 1/5th of the min dist when doing the first step
//  double stepDist = minDist * 0.2f;
//  const double derivateLength = glm::length(derivate(p));
//
//  if (stepDist > 1.0e-7f/*initialStepDistanceUpperBound.getValue()*/)
//  {
//    if (derivateLength > 0.0f/*zeroCompare.getValue()*/)
//    {
//      stepSize = stepDist / derivateLength;
//    }
//    // else keep step size from plain bbox approach
//  }
//  else
//  {
//    // fallback to bbox approach
//    stepDist = getMinExtent() * 0.001f/*initialStepSizeFactor.getValue()*/;
//
//    if (derivateLength > 0.0f/*zeroCompare.getValue()*/)
//    {
//      stepSize = stepDist / derivateLength;
//    }
//    // else keep step size from plain bbox approach
//  }
//
//  // clamp initial step size to reasonable value
//  stepSize = std::max(stepSize, 0.0001f/*initialStepSizeLowerBound.getValue()*/);
//
//  return stepSize;
//
//}
//
//bool FOAMdataset::snapToMeshSurface(glm::dvec3& p)
//{
//  bool hasNewSeedPoint = false;
//  glm::dvec3 newPosition;
//
//  // HINEWISE! getElementDims
//  glm::dvec3 elementExtent = m_sceneAccel->getElementExtents();
//  double referenceDistance = std::min(elementExtent.x, std::min(elementExtent.y, elementExtent.z)) * 0.25f; // a reference distance to discard snapped positions which are too far away
//
//  glm::dvec3 point = glm::dvec3(m_world_to_model_transform * glm::vec4(p, 1.0f));
//
//  unsigned int nearestTetID = 0;
//  glm::vec4 baryCoords;
//  if (getCellInRange(point, std::numeric_limits<double>::infinity(), nearestTetID, baryCoords))
//  {
//
//    newPosition =
//      m_points[m_cellPoints[m_cellPointsBegIndices[nearestTetID] + 1]] * baryCoords.x +
//      glm::dvec3(m_points[m_cellPoints[m_cellPointsBegIndices[nearestTetID] + 2]] * baryCoords.y) +
//      glm::dvec3(m_points[m_cellPoints[m_cellPointsBegIndices[nearestTetID] + 3]] * baryCoords.z) +
//      glm::dvec3(m_points[m_cellPoints[m_cellPointsBegIndices[nearestTetID] + 4]] * baryCoords.w);
//
//    if ((point - newPosition).length() <= referenceDistance)
//    {
//      //L_INFO << "Snapped (" << p << ") in field free shape to TET " << newPosition << ".\n";
//      hasNewSeedPoint = true;
//    }
//    else
//    {
//      //L_INFO << "Snapped (" << p << ") in field free shape to TET (" << newPosition << "), but was discarded due to distance >= " << referenceDistance << " .\n";
//    }
//  }
//
//  if (hasNewSeedPoint)
//  {
//    p = glm::dvec3(m_model_to_world_transform * glm::vec4(newPosition, 1.0f));
//  }
//
//  return hasNewSeedPoint;
//}
//
//bool FOAMdataset::getCellInRange(const glm::dvec3& point, double range, unsigned int& nearestTetID, glm::vec4 &baryCoords)
//{
//  double minSquareDistance = std::numeric_limits<double>::max();
//  unsigned int minSquareDistance_tetID = std::numeric_limits<unsigned int>::max();
//  glm::vec4 minSquareDistance_baryCoords;
//
//  if (contains(point))
//    return false;
//
//  glm::i64 cell_idx;
//  if (!m_sceneAccel->getBoundingPrimitiveIndex(this, (double*)&point, cell_idx))
//    return false;
//
//  // TODO Only works for tet cells
//  if (m_cellPoints[m_cellPointsBegIndices[cell_idx]] != 4)
//    return false;
//
//  const glm::dvec3 tetCoords[4] =
//  {
//    m_points[m_cellPoints[m_cellPointsBegIndices[cell_idx] + 1]],
//    m_points[m_cellPoints[m_cellPointsBegIndices[cell_idx] + 2]],
//    m_points[m_cellPoints[m_cellPointsBegIndices[cell_idx] + 3]],
//    m_points[m_cellPoints[m_cellPointsBegIndices[cell_idx] + 4]]
//  };
//
//  glm::dvec3 localMinSquareDistance_pos;
//  glm::vec4 localMinSquareDistance_baryCoords;
//  double localMinSquareDistance = FLT_MAX;
//
//  // calculate distances to tet coords
//  double cSqareDist[4];
//  for (unsigned int pi = 0; pi < 4; ++pi)
//  {
//    cSqareDist[pi] = glm::length2(tetCoords[pi] - point);
//
//    if (cSqareDist[pi] < localMinSquareDistance)
//    {
//      localMinSquareDistance = cSqareDist[pi];
//      localMinSquareDistance_baryCoords = glm::vec4();
//      localMinSquareDistance_baryCoords[pi] = 1.0f;
//    }
//  }
//
//  // calculate distances to tet edges
//  double eSquareDist[6];
//  {
//    double* offset = eSquareDist;
//    for (unsigned int u = 0; u < 3; ++u) // check tet edges 0-1 0-2 0-3 1-2 1-3 2-3
//    {
//      for (unsigned int v = u + 1; v <= 3; ++v)
//      {
//        glm::dvec3 npseg;
//        if (nearestPointOnSegment(point, tetCoords[u], tetCoords[v], npseg))
//        {
//          *offset = glm::length2(npseg - point);
//        }
//        else
//        {
//          *offset = std::numeric_limits<double>::max();
//        }
//
//        if (*offset < localMinSquareDistance)
//        {
//          localMinSquareDistance = *offset;
//          localMinSquareDistance_baryCoords = glm::vec4();
//          localMinSquareDistance_baryCoords[u] = 1.0f - (npseg - tetCoords[u]).length() / (tetCoords[v] - tetCoords[u]).length();
//          localMinSquareDistance_baryCoords[v] = 1.0f - localMinSquareDistance_baryCoords[u];
//        }
//
//        ++offset;
//      }
//    }
//  }
//
//  // calculate distances to tet faces
//  double fSquareDist[4];
//  unsigned char indices[4][3] = { { 0, 1, 2 },{ 0, 2, 3 },{ 2, 3, 1 },{ 3, 1, 0 } };
//  for (unsigned int fi = 0; fi < 4; ++fi)
//  {
//    const glm::dvec3 faceCoords[3] =
//    {
//      tetCoords[indices[fi][0]],
//      tetCoords[indices[fi][1]],
//      tetCoords[indices[fi][2]]
//    };
//
//    // project position on triangle plane
//    const glm::dvec3 facePlaneNormal = calculateTriangleNormal(faceCoords[0], faceCoords[1], faceCoords[2]);
//    const glm::dvec3 projPos = projectPointOnPlane<true>(faceCoords[0], facePlaneNormal, point);
//
//    double bc[2];
//    pointInTriangle(faceCoords, projPos, bc);
//    if (pointInTriangle(faceCoords, projPos, bc))
//    {
//      fSquareDist[fi] = glm::length2(point - projPos);
//    }
//    else
//    {
//      fSquareDist[fi] = std::numeric_limits<double>::max();
//    }
//
//    if (fSquareDist[fi] < localMinSquareDistance)
//    {
//      localMinSquareDistance = fSquareDist[fi];
//      localMinSquareDistance_pos = projPos;
//      localMinSquareDistance_baryCoords = glm::vec4();
//
//      // re-compute barycentric coordinates for the tet using the projected position on the face
//      calculateBarycentricCoords(tetCoords, projPos, (double*)&localMinSquareDistance_baryCoords);
//    }
//  }
//
//  if (localMinSquareDistance < minSquareDistance)
//  {
//    minSquareDistance = localMinSquareDistance;
//    minSquareDistance_tetID = cell_idx;
//    minSquareDistance_baryCoords = localMinSquareDistance_baryCoords;
//  }
//
//  if (minSquareDistance_tetID != std::numeric_limits<unsigned int>::max())
//  {
//    if (sqrtf(minSquareDistance) <= range)
//    {
//      nearestTetID = minSquareDistance_tetID;
//      baryCoords = minSquareDistance_baryCoords;
//
//      // due to floating point issues, we might get values off 0.0..1.0 for the barycentric coordinate
//      // (actual reasons are unknown :), but it typically only happens for the case were the nearest
//      // point lies on a tet face)
//      // so eliminate those, move sightly inside the tet, and then normalize
//      const double epsilon = 1.5e-6f;
//      baryCoords[0] = std::max(baryCoords[0], epsilon);
//      baryCoords[1] = std::max(baryCoords[1], epsilon);
//      baryCoords[2] = std::max(baryCoords[2], epsilon);
//      baryCoords[3] = std::max(baryCoords[3], epsilon);
//
//      baryCoords[0] = std::min(baryCoords[0], 1.0f - epsilon);
//      baryCoords[1] = std::min(baryCoords[1], 1.0f - epsilon);
//      baryCoords[2] = std::min(baryCoords[2], 1.0f - epsilon);
//      baryCoords[3] = std::min(baryCoords[3], 1.0f - epsilon);
//
//      const double len =
//        baryCoords[0] +
//        baryCoords[1] +
//        baryCoords[2] +
//        baryCoords[3];
//
//      baryCoords[0] /= len;
//      baryCoords[1] /= len;
//      baryCoords[2] /= len;
//      baryCoords[3] /= len;
//
//      return true;
//    }
//  }
//
//  return false;
//}
