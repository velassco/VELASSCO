/* -*- c++ -*- */
#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <stddef.h>  // defines NULL
#include <stdlib.h>

#include <math.h>
#include <float.h>
#include <assert.h>

#include "Helpers.h"

namespace VELaSSCo {
  namespace Simplification {
    /* Parameters */
    class Parameters {
    public:
      Parameters(): _size( 0), _max_num_elements( 0), _boundary_weight( 1.0) {}
      bool fromString( const std::string str_parameters);
      std::string toString() const;
      std::string asRowKeyPartString() const;
      int getMaxNumberOfElements() const { return _max_num_elements;}
    private:
      // "GridSize=1024;MaximumNumberOfElements=10000000;BoundaryWeight=100.0;"
      int _size, _max_num_elements;
      double _boundary_weight;
    };
  
    inline bool Parameters::fromString( const std::string str_parameters) {
      std::istringstream is_params( str_parameters);
      std::string key, value;
      int scanned = 0;
      while ( getline( is_params, key, '=') && getline( is_params, value, ';')) {
	if ( AreEqualNoCase( key, "GridSize")) {
	  std::istringstream is_val( value);
	  is_val >> _size;
	  scanned++;
	} else if ( AreEqualNoCase( key, "MaximumNumberOfElements")) {
	  std::istringstream is_val( value);
	  is_val >> _max_num_elements;
	  scanned++;
	} else if ( AreEqualNoCase( key, "BoundaryWeight")) {
	  std::istringstream is_val( value);
	  is_val >> _boundary_weight;
	  scanned++;
	} else {
	  DEBUG( "Parameters::fromString unknown parameter: " << key);
	}
	// is_params.ignore(); // the new line
      }
      return scanned > 0;
    }
  
    inline std::string Parameters::toString() const {
      // "GridSize=1024;MaximumNumberOfElements=10000000;BoundaryWeight=100.0;"
      std::ostringstream oss;
      oss << "GridSize=" << _size << ";";
      oss << "MaximumNumberOfElements=" << _max_num_elements << ";";
      oss << "BoundaryWeight=" << _boundary_weight << ";";
      return oss.str();
    }

    // not all parameters are suitable as storage discriminator, i.e. they are not used to generate the information,
    // for instance parameters == "GridSize=1024;MaximumNumberOfElements=10000000;BoundaryWeight=100.0;" 
    // and "MaximumNumberOfElements" is not used to generate the simplified mesh but only as output restriction criteria
    // so removing it from the vqueryParameters
    inline std::string Parameters::asRowKeyPartString() const {
      // "GridSize=1024;MaximumNumberOfElements=10000000;BoundaryWeight=100.0;"
      std::ostringstream oss;
      oss << "GridSize=" << _size << ";";
      // oss << "MaximumNumberOfElements=" << _max_num_elements << ";";
      oss << "BoundaryWeight=" << _boundary_weight << ";";
      return oss.str();
    }

    /* Point = Vector */
    class Point {
    public:
      double x, y, z;
  
      Point() {};
      Point( double xx, double yy, double zz = 0.0f):
	x( xx), y( yy), z( zz) {
      };
      Point( double vv): x( vv), y( vv), z( vv) { };
      Point( const Point &cf): x( cf.x), y( cf.y), z( cf.z) {};
      Point( const double *v) {
	x = v[ 0]; y = v[ 1]; z = v[ 2];
      }
  
      void set( double xx, double yy, double zz) {
	x = xx; y = yy; z = zz;
      }
      void set( const double *v) {
	x = v[ 0]; y = v[ 1]; z = v[ 2];
      }
  
      void init( double val = 0.0f) {
	x = val; y = val; z = val;
      }
  
      Point &operator=( const Point &cf) {
	x = cf.x; y = cf.y; z = cf.z;
	return *this;
      }
      Point operator+( const Point &cf) const {
	Point tmp( x + cf.x, y + cf.y, z + cf.z);
	return tmp;
      }
      Point operator-( const Point &cf) const {
	Point tmp( x - cf.x, y - cf.y, z - cf.z);
	return tmp;
      }
      Point &operator+=( const Point &cf) {
	x += cf.x; y += cf.y; z += cf.z;
	return *this;
      }
      Point &operator-=( const Point &cf) {
	x -= cf.x; y -= cf.y; z -= cf.z;
	return *this;
      }
      Point &operator*=( double a) {
	x *= a; y *= a; z *= a;
	return *this;
      }
      Point &operator/=( double a) {
	assert( a != 0.0f);
	x /= a; y /= a; z /= a;
	return *this;
      }

      double mod( void) const {
	return sqrt( x * x + y * y + z * z);
      }
      double mod2( void) const {
	return ( x * x + y * y + z * z);
      }
      double sum_abs( void) const {
	return fabsf( x) + fabsf( y) + fabsf( z);
      }
      Point &abs( void) {
	x = fabsf( x); y = fabsf( y); z = fabsf( z); 
	return *this;
      }

      Point &normalize( void) {
	double l = mod2();
	if ( ( l <= 0.0) || ( l == 1.0))
	  return *this;
	l = mod();
	double il = 1.0 / l;
	x *= il;
	y *= il;
	z *= il;
	return *this;
      }
    };

    inline Point operator*( double a, const Point &cf) {
      return Point( a * cf.x, a * cf.y, a * cf.z);
    }
    inline Point operator*( const Point &cf, double a) {
      return Point( a * cf.x, a * cf.y, a * cf.z);
    }
    inline Point operator/( double a, const Point &cf) {
      assert( ( cf.x != 0.0) &&  ( cf.y != 0.0) &&  ( cf.z != 0.0));
      return Point( a / cf.x, a / cf.y, a / cf.z);
    }
    inline Point operator/( const Point &cf, double b) {
      assert( b != 0.0);
      double a = 1.0 / b;
      return Point( a * cf.x, a * cf.y, a * cf.z);
    }
    inline Point operator/( const Point &cf_a, const Point &cf_b) {
      assert( ( cf_b.x != 0.0f) &&  ( cf_b.y != 0.0f) &&  ( cf_b.z != 0.0f));
      return Point( cf_a.x / cf_b.x, cf_a.y / cf_b.y, cf_a.z / cf_b.z);
    }
    inline double scalar_product( const Point &a, const Point &b) {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    inline Point component_product( const Point &a, const Point &b) {
      return Point( a.x * b.x, a.y * b.y, a.z * b.z);
    }
    // end Point
    typedef Point Vector;

    inline Point calc_normal( const Point &p1, const Point &p2, const Point &p3) {
      Point normal;
      Point u,v;
      u = p2 - p1;
      v = p3 - p1;
      double x = u.y * v.z - u.z * v.y;
      double y = u.z * v.x - u.x * v.z;
      double z = u.x * v.y - u.y * v.x;
      normal.set( x, y, z);
      normal.normalize();
      return normal;
    }
    // end Point
  
    /* Bounding Box / Cube */
#ifndef MAXDOUBLE
#define MAXDOUBLE     FLT_MAX
#endif
#define EPSILON_CMP          	1e-6

    class Box {
    public:
      Box( void): _min( MAXDOUBLE, MAXDOUBLE, MAXDOUBLE), _max( -MAXDOUBLE, -MAXDOUBLE, -MAXDOUBLE) {};
      Box( const Point &min, const Point &max): _min( min), _max( max) {};
  
      void init( const Point &p) { _min = p; _max = p; }
      void init( double x, double y, double z = 0.0f) { 
	_min.set( x, y, z); 
	_max.set( x, y, z);
      }
  
      void update( const Point &p) {
	if ( p.x < _min.x) _min.x = p.x;
	if ( p.y < _min.y) _min.y = p.y;
	if ( p.z < _min.z) _min.z = p.z;
	if ( p.x > _max.x) _max.x = p.x;
	if ( p.y > _max.y) _max.y = p.y;
	if ( p.z > _max.z) _max.z = p.z;
      }
      void update( double x, double y, double z = 0.0f) {
	if ( x < _min.x) _min.x = x;
	if ( y < _min.y) _min.y = y;
	if ( z < _min.z) _min.z = z;
	if ( x > _max.x) _max.x = x;
	if ( y > _max.y) _max.y = y;
	if ( z > _max.z) _max.z = z;
      }
  
      double diagonal2() const { Vector dif = _max - _min; return dif.mod2(); }
      double diagonal() const { Vector dif = _max - _min; return dif.mod(); }
  
      Point min() const { return _min;}
      Point max() const { return _max;}
      Point centre() const { Point c = 0.5f * ( _min + _max); return c;}
  
      bool inside( const Point &p) const { return inside( p.x, p.y, p.z);}
      bool inside( double x, double y, double z = 0.0f) const;
      bool insideEPS( const Point &p) const { return insideEPS( p.x, p.y, p.z);}
      bool insideEPS( double x, double y, double z = 0.0f) const;
  
      Box get_bounding_cube() const;
      Box get_centered_bounding_cube() const;
  
      void print( char *str) {
	printf( "bbox %s = ( %g, %g, %g) - ( %g, %g, %g)\n", str, 
		_min.x, _min.y, _min.z, _max.x, _max.y, _max.z);
      }

      std::string toString() const;
  
    private:
      Point _min, _max;
    };

    inline bool Box::inside( double x, double y, double z) const {
      bool ret = false;
      if ( ( x >= _min.x) && ( x <= _max.x) &&
	   ( y >= _min.y) && ( y <= _max.y) &&
	   ( z >= _min.z) && ( z <= _max.z) )
	ret = true;
      return ret;
    }

    inline bool Box::insideEPS( double x, double y, double z) const {
      double eps = ( double)( EPSILON_CMP * diagonal());
      bool ret = false;
      if ( ( x > _min.x - eps) && ( x < _max.x + eps) &&
	   ( y > _min.y - eps) && ( y < _max.y + eps) &&
	   ( z > _min.z - eps) && ( z < _max.z + eps) )
	ret = true;
      return ret;
    }

    inline Box Box::get_bounding_cube() const {
      Point dif = _max - _min;
      double max = dif.x;
      if ( dif.y > max) max = dif.y;
      if ( dif.z > max) max = dif.z;
      dif.set( max, max, max);
      return Box( _min, _min + dif);
    }
    
    inline Box Box::get_centered_bounding_cube() const {
      Point dif = _max - _min;
      double max = dif.x;
      if ( dif.y > max) max = dif.y;
      if ( dif.z > max) max = dif.z;
      // we want a centered cube
      dif.set( max - dif.x, max - dif.y, max - dif.z);
      dif *= 0.5f;
      return Box( _min - dif, _max + dif);
    }
    
    inline std::string Box::toString() const {
      // "GridSize=1024;MaximumNumberOfElements=10000000;BoundaryWeight=100.0;"
      std::ostringstream oss;
      oss.precision( 18);
      oss << _min.x << ";" << _min.y << ";" << _min.z << ";"
	  << _max.x << ";" << _max.y << ";" << _max.z;
      return oss.str();
    }

  } // namespace Simplification
} // namespace VELaSSCo
