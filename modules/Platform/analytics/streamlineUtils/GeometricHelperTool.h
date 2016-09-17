
/**
 *
 * Helper Functions used as tools in geomteric calculations.
 *
 * Some of the functions are copied from the iFX libraries and some changes are made to make them compatiable with OpenSG.
 */

#pragma once

// STD
#include <vector>

// GLM
#include "glm/glm.hpp"

#ifdef WIN32
#    define GEOM_TOOL_INLINE static __forceinline
#else
#    define GEOM_TOOL_INLINE inline
#endif

GEOM_TOOL_INLINE bool nearestPointOnSegment(const glm::dvec3& p, const glm::dvec3& s0, const glm::dvec3& s1, glm::dvec3& nearestPoint);
GEOM_TOOL_INLINE glm::dvec3 calculateTriangleNormal(const glm::dvec3 &p0, const glm::dvec3 &p1, const glm::dvec3 &p2);
GEOM_TOOL_INLINE double distancePointPlane(const glm::dvec3& p, const glm::dvec3& n, const glm::dvec3& point);
GEOM_TOOL_INLINE glm::dvec3 projectPointOnPlane(const glm::dvec3& p, const glm::dvec3& n, const glm::dvec3& point);
GEOM_TOOL_INLINE bool pointInTriangle(const glm::dvec3 vertices[3], const glm::dvec3& point, double baryCoords[2], double epsilon = 1e-6);
GEOM_TOOL_INLINE bool pointInTriangle(const glm::dvec3& firstTrianglePoint, const glm::dvec3 triangleMainEdges[2], const glm::dvec3& point, double baryCoords[2], double epsilon);
GEOM_TOOL_INLINE void calculateBarycentricCoords(const glm::dvec3 tetPos[4], const glm::dvec3& pos, double baryCoords[4]);
GEOM_TOOL_INLINE void solveLinearEq(const std::vector< std::vector< glm::float64 > >& distanceMat, std::vector< glm::dvec3 >& weights, const std::vector< glm::dvec3 >& FuncVals);
GEOM_TOOL_INLINE bool rayIntersectsTriangle(const glm::dvec3 p, glm::dvec3 d, glm::dvec3 p0, glm::dvec3 p1, glm::dvec3 p2);
GEOM_TOOL_INLINE bool gaussianElimination(std::vector<std::vector< glm::float64 > > m, std::vector<glm::float64>& x, std::vector<glm::float64> f);

/**
* calculates the point on line made by s0 and s1, which the line made by that point and p, has the minimum length.
*/
GEOM_TOOL_INLINE bool nearestPointOnSegment(const glm::dvec3& p, const glm::dvec3& s0, const glm::dvec3& s1, glm::dvec3& nearestPoint)
{
	glm::dvec3 direction = s1 - s0;
	double l = direction.length();

	double t = glm::dot(direction, p - s0) / l;
	double t_norm = t / l;
	
	if (t_norm >= 0.0f && t_norm <= 1.0f)
	{
		// between s0 an s1
		nearestPoint = s0 + direction*t_norm;
		return true;
	}
	else
	{
		// outside s0 and s1
		nearestPoint = glm::dvec3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
		return false;
	}
}

/**
* calculates the normal of a triangle: (v1-v0) x (v2-v0)
*/
GEOM_TOOL_INLINE glm::dvec3 calculateTriangleNormal(const glm::dvec3 &p0, const glm::dvec3 &p1, const glm::dvec3 &p2)
{
  glm::dvec3 normal = glm::cross(p1 - p0, p2 - p0);
  normal = glm::normalize(normal);
  return normal;
}

/**
* calculates the minimum distance from a point to the plane.
*/
template <bool nIsNormalized>
GEOM_TOOL_INLINE double distancePointPlane(const glm::dvec3& p, const glm::dvec3& n, const glm::dvec3& point)
{
	if (!nIsNormalized)
	{
		return glm::dot(n, (point - p)) / n.length();
	}
	else
	{
		return glm::dot(n, (point - p));
	}
}

/**
* calculates the point projected on the plane(p is a point on plane and n is the normal vector).
*/
template <bool nIsNormalized>
GEOM_TOOL_INLINE glm::dvec3 projectPointOnPlane(const glm::dvec3& p, const glm::dvec3& n, const glm::dvec3& point)
{
	if (!nIsNormalized)
	{
    glm::dvec3 nn(glm::normalize(n));

		return projectPointOnPlane<true>(p, nn, point);
	}
	else
	{
		return point - distancePointPlane<true>(p, n, point)*n;
	}
}

/**
* notice the u, v semantic
* point = u * v[0] + v * v[1] + (1-u-v) * v[2] 
*/
GEOM_TOOL_INLINE bool pointInTriangle(const glm::dvec3 vertices[3], const glm::dvec3& point, double baryCoords[2], double epsilon)
{
	glm::dvec3 triangleMainEdges[2];
	triangleMainEdges[0] = vertices[0] - vertices[2];
	triangleMainEdges[1] = vertices[1] - vertices[2];

	return pointInTriangle(vertices[2], triangleMainEdges, point, baryCoords, epsilon);
}

/**
* notice the u, v semantic
* P = firstTrianglePoint + u * (triangleMainEdges[0]) + v * (triangleMainEdges[1])
* is the same like
* P = u * v[0] + v * v[1] + (1-u-v) * v[2]
*/
GEOM_TOOL_INLINE bool pointInTriangle(const glm::dvec3& firstTrianglePoint, const glm::dvec3 triangleMainEdges[2], const glm::dvec3& point, double baryCoords[2], double epsilon)
{
	// http://www.blackpawn.com/texts/pointinpoly/default.html

	// Compute vectors        
	const glm::dvec3& v0 = triangleMainEdges[0];
	const glm::dvec3& v1 = triangleMainEdges[1];
	const glm::dvec3 v2 = point - firstTrianglePoint;

	// Compute dot products
  double dot00 = glm::length2(v0); // vo.dot(v0)
	double dot01 = glm::dot(v0, v1);
	double dot02 = glm::dot(v0, v2);
  double dot11 = glm::length2(v1); // v1.dot(v1)
	double dot12 = glm::dot(v1, v2);

	// Compute barycentric coordinates
	double invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);

	// Check if point is in triangle
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	if (u >= -epsilon)
	{
		u = std::max(u, 0.0);
		double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
		if (v >= -epsilon)
		{
			v = std::max(v, 0.0);

			if ((u + v) <= (1.0 + epsilon))
			{
				baryCoords[0] = u;
				baryCoords[1] = v;

				return true;
			}
		}
	}

	baryCoords[0] = 0.0f;
	baryCoords[1] = 0.0f;
	return false;
}

GEOM_TOOL_INLINE void calculateBarycentricCoords(const glm::dvec3 tetPos[4], const glm::dvec3& pos, double baryCoords[4])
{
	const glm::dvec3 V1V0 = tetPos[1] - tetPos[0];
	const glm::dvec3 V2V0 = tetPos[2] - tetPos[0];
	const glm::dvec3 V3V2 = tetPos[3] - tetPos[2];
	const glm::dvec3 PV0 = pos - tetPos[0];

  double tetVolumeInv = 1.0f / glm::dot(V2V0, glm::cross(V1V0, V3V2));
	
  baryCoords[0] = glm::dot(tetPos[2] - pos, glm::cross(tetPos[1] - pos, V3V2)) * tetVolumeInv;
  baryCoords[1] = glm::dot((V2V0), glm::cross(PV0, V3V2)) * tetVolumeInv;
	baryCoords[2] = glm::dot(PV0, glm::cross((V1V0), (tetPos[3] - pos))) * tetVolumeInv;
	//baryCoords[3] = (V2V0).dot((V1V0).cross(pos - tetPos[2])) * tetVolumeInv;
	baryCoords[3] = 1.0f - baryCoords[0] - baryCoords[1] - baryCoords[2];
}

GEOM_TOOL_INLINE void solveLinearEq(const std::vector< std::vector < glm::float64 > >& distanceMat, std::vector<glm::dvec3>& weights, const std::vector<glm::dvec3>& FuncVals)
{
	std::vector<glm::float64> wVecX, wVecY, wVecZ;
	std::vector<glm::float64> fVecX, fVecY, fVecZ;
	
	fVecX.resize(FuncVals.size(), 1);
	fVecY.resize(FuncVals.size(), 1);
	fVecZ.resize(FuncVals.size(), 1);
	for(size_t i = 0; i < FuncVals.size(); i++)
	{
		fVecX[i] = FuncVals[i].x;
		fVecY[i] = FuncVals[i].y;
		fVecZ[i] = FuncVals[i].z;
	}

	wVecX.resize(weights.size(), 0.0f);
	wVecY.resize(weights.size(), 0.0f);
	wVecZ.resize(weights.size(), 0.0f);

	if(!gaussianElimination(distanceMat, wVecX, fVecX) || !gaussianElimination(distanceMat, wVecY, fVecY) || !gaussianElimination(distanceMat, wVecZ, fVecZ))
	{
		std::cout << "Solving the Equations to find out the weights is not possible!" << std::endl;
		return;
	}
	

	for(int i = 0; i < wVecX.size(); i++)
	{
		weights[i] = glm::dvec3(wVecX[i], wVecY[i], wVecZ[i]);
	}
}

// Reference: http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
GEOM_TOOL_INLINE bool rayIntersectsTriangle(const glm::dvec3 p, glm::dvec3 d, glm::dvec3 p0, glm::dvec3 p1, glm::dvec3 p2)
{
	// u, v, and 1-u-v are barycentric coords of p in the triangle
	// they should be between 0 and 1 if the point is in the triangle.
	double f, u, v;
	glm::dvec3 e1 = p1 - p0, e2 = p2 - p0, h, s, q;
	h = glm::cross(d, e2);
	
	// if the ray is parallel to the triangle
	if (glm::abs(glm::dot(h, e1)) / (glm::length(h) * glm::length(e1)) < 0.0000001f)
		return(false);

	f = 1 / glm::dot(h, e1);
	s = p - p0;
	u = f * (glm::dot(s, h));

	if (u < 0.0 || u > 1.0)
		return(false);

	q = glm::cross(s, e1);
	v = f * glm::dot(d, q);

	if (v < 0.0f || u + v > 1.0f)
		return(false);

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	double t = f * glm::dot(e2, q);

	if (t > 0.0f)		// ray intersection
		return(true);
	else				// line intersection
		return (false);
}


// =================================================================
// =================================================================
// ======= Gaussian Elimination Linear Equation Solver   ===========
// =================================================================
// =================================================================

GEOM_TOOL_INLINE void swapRows(std::vector< std::vector< glm::float64 > >& m, std::vector<glm::float64>& f, const size_t row1, const size_t row2)
{
	double tmp;
	for(size_t i = 0; i < m.size(); i++)
	{
		tmp = m[row1][i];
		m[row1][i] = m[row2][i];
		m[row2][i] = tmp;
	}

	tmp = f[row1];
	f[row1] = f[row2];
	f[row2] = tmp;
}

GEOM_TOOL_INLINE void multRowByVal(std::vector< std::vector< glm::float64 > >& m, std::vector<glm::float64>& f, const size_t& row, const double& val)
{
	for(size_t i = 0; i < m.size(); i++)
		m[row][i] *= val;
	f[row] *= val;
}

GEOM_TOOL_INLINE void subtractRowsByFactor(std::vector< std::vector< glm::float64 > >& m, std::vector<glm::float64>& f, const size_t& row1, const size_t& row2, const double factor)
{
	if(factor == 0.0f)
		return;

	for(size_t i = 0; i < m.size(); i++)
		m[row2][i] -= (factor * m[row1][i]);
	f[row2] -= factor * f[row1];
}

GEOM_TOOL_INLINE size_t getMaxElementRowIdx(std::vector< std::vector< glm::float64 > >& m, const size_t& col)
{
	size_t largestElemIdx = col;
	for(size_t i = col+1; i < m.size(); i++)
	{
		if(std::abs(m[i][col]) > std::abs(m[largestElemIdx][col]))
			largestElemIdx = i;
	}
	return largestElemIdx;
}

GEOM_TOOL_INLINE bool gaussianElimination(std::vector< std::vector< glm::float64 > > m, std::vector<glm::float64>& x, std::vector<glm::float64> f)
{
	for(size_t i = 0; i < x.size(); i++)
		x[i] = 0.0f;

	for(size_t c = 0; c < m.size(); c++)
	{
		// find the element with largest absloute value in col c
		size_t rowWithMaxElemIdx = getMaxElementRowIdx(m, c);
		if(m[rowWithMaxElemIdx][c] == 0.0f)
			return false;
		if(rowWithMaxElemIdx != c)
			swapRows(m, f, c, rowWithMaxElemIdx);

		multRowByVal(m, f, c, 1.0f / (m[c][c]));

		// clear lower columns entries
		for(size_t r = c+1; r < m.size(); r++)
			subtractRowsByFactor(m, f, c, r, m[r][c]);
	}

	for(size_t r = m.size(); r > 0; r--)
	{
		x[r - 1] = f[r - 1];
		for(size_t c = r; c < m.size(); c++)
		{
			x[r - 1] -= m[r - 1][c] * x[c];
		}
	}
	return true;
}

// =================================================================
// =================================================================
