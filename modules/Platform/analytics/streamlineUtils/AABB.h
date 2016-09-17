
/**
 *
 * Axis-aligned bounding box
 *
 * This class is intended as a lightweight alternative to OpenSG and VTK bounding boxes.
 *
 */
 
#ifndef __AABB_H__
#define __AABB_H__

#include <algorithm>
#include <limits>
#include <stdint.h>

#include "glm/glm.hpp"

#ifdef WIN32
#    define AABB_INLINE __forceinline
#else
#    define AABB_INLINE inline
#endif

class AABB
{
public:

	/// Initialize with an invalid box.
	AABB_INLINE AABB();

	/// Copy constructor
	AABB_INLINE AABB(const AABB& c);

	/// Initialize with new min and max vectors.
	AABB_INLINE AABB( const double min[3], const double max[3] );

	/// Check if the box is valid (i.e., min < max).
	AABB_INLINE bool valid() const;

	/// Check if the box contains a 3D point.
	AABB_INLINE bool contains( const double point[3] ) const;

  /// Check if the box contains a 3D point based on the splitAxis.
  AABB_INLINE bool contains( const double point[3], uint8_t splitAxis ) const;

	/// Extend the box with a 3D point.
	AABB_INLINE void extend( const double x, const double y, const double z );

	/// Extend the box with a 3D point.
	AABB_INLINE void extend( const double point[3] );

	/// Extend the box with another box.
	AABB_INLINE void extend( const AABB &aabb );

	/// Enlarge min and max by delta.
	AABB_INLINE void enlarge( const double delta );

	/// Returns the volume of the bounding box
	AABB_INLINE double volume() const;

  /// Returns the surface area of the bounding box
  AABB_INLINE double surfaceArea() const;

  /// Returns the largest spatial extent
  AABB_INLINE uint32_t maxExtentAxis() const;

  AABB_INLINE glm::dvec3 center() const;

	/// Bounding min and max vectors.
	glm::dvec3 min;
	glm::dvec3 max;
};


AABB_INLINE AABB::AABB()
{
	min[0] = min[1] = min[2] = +std::numeric_limits<double>::max();
	max[0] = max[1] = max[2] = -std::numeric_limits<double>::max();
}


AABB_INLINE AABB::AABB(const AABB& c)
{
	min[0] = c.min[0];	min[1] = c.min[1];	min[2] = c.min[2];
	max[0] = c.max[0];	max[1] = c.max[1];	max[2] = c.max[2];
}

AABB_INLINE AABB::AABB( const double min[3], const double max[3] )
{
	this->min[0] = min[0];
	this->min[1] = min[1];
	this->min[2] = min[2];

	this->max[0] = max[0];
	this->max[1] = max[1];
	this->max[2] = max[2];
}

AABB_INLINE bool AABB::valid() const
{
	return    min[0] <= max[0]
		   && min[1] <= max[1]
		   && min[2] <= max[2];
}

AABB_INLINE bool AABB::contains( const double point[3] ) const
{
	return    point[0] >= min[0] && point[0] <= max[0]
		   && point[1] >= min[1] && point[1] <= max[1]
		   && point[2] >= min[2] && point[2] <= max[2];
}

AABB_INLINE bool AABB::contains( const double point[3], uint8_t splitAxis ) const
{
  const uint8_t xAxis = (splitAxis + 2) % 3;
  const uint8_t yAxis = (splitAxis + 4) % 3;


  return    point[splitAxis] >= min[splitAxis] && point[splitAxis] <= max[splitAxis]
		   && point[xAxis] >= min[xAxis] && point[xAxis] <= max[xAxis]
		   && point[yAxis] >= min[yAxis] && point[yAxis] <= max[yAxis];
}

AABB_INLINE void AABB::extend( const double x, const double y, const double z )
{
	min[0] = std::min(min[0], x);
	min[1] = std::min(min[1], y);
	min[2] = std::min(min[2], z);

	max[0] = std::max(max[0], x);
	max[1] = std::max(max[1], y);
	max[2] = std::max(max[2], z);
}

AABB_INLINE void AABB::extend( const double point[3] )
{
	min[0] = std::min(min[0], point[0]);
	min[1] = std::min(min[1], point[1]);
	min[2] = std::min(min[2], point[2]);

	max[0] = std::max(max[0], point[0]);
	max[1] = std::max(max[1], point[1]);
	max[2] = std::max(max[2], point[2]);
}

AABB_INLINE void AABB::extend( const AABB &aabb )
{
	min[0] = std::min(min[0], aabb.min[0]);
	min[1] = std::min(min[1], aabb.min[1]);
	min[2] = std::min(min[2], aabb.min[2]);

	max[0] = std::max(max[0], aabb.max[0]);
	max[1] = std::max(max[1], aabb.max[1]);
	max[2] = std::max(max[2], aabb.max[2]);
}

AABB_INLINE void AABB::enlarge( const double delta )
{
	min[0] -= delta;
	min[1] -= delta;
	min[2] -= delta;

	max[0] += delta;
	max[1] += delta;
	max[2] += delta;
}

AABB_INLINE double AABB::volume() const
{
	return (max[0] - min[0]) * (max[1] - min[1]) * (max[2] - min[2]);
}

AABB_INLINE double AABB::surfaceArea() const
{
  glm::dvec3 dims( max[0] - min[0], max[1] - min[1], max[2] - min[2] );
  return 2 * (dims.x * dims.y + dims.y * dims.z + dims.x * dims.z);
}

AABB_INLINE uint32_t AABB::maxExtentAxis() const
{
  glm::dvec3 dims( max[0] - min[0], max[1] - min[1], max[2] - min[2] );
  if( dims.x >= dims.y && dims.x >= dims.z)
    return 0;

  if( dims.y >= dims.x && dims.y >= dims.z)
    return 1;

  if( dims.z >= dims.x && dims.z >= dims.y)
    return 2;

  return 3;
}

AABB_INLINE glm::dvec3 AABB::center() const
{
  return glm::dvec3((min[0] + max[0]) / 2.0f, (min[1] + max[1]) / 2.0f, (min[2] + max[2]) / 2.0f);
}

#endif
