/* -*- c++ -*- */
#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <stddef.h>  // defines NULL
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace VELaSSCo {
  class BoundaryBinaryMesh {
  public:
    typedef enum { NONE = 0, STATIC, DELEGATE_NEW, DELEGATE_MALLOC, COPY} t_memory_management;
  // if this is really needed, then may be it's better to change num_nodes to be int64_t ...
#pragma pack( push, 1)
    typedef struct {
      int64_t _id;
      double _coords[ 3];
    } MeshPoint;
    typedef struct {
      int _num_nodes;
      int64_t _nodes[ 3];
    } BoundaryTriangle;
#pragma pack( pop)
  
    BoundaryBinaryMesh(): _lst_vertices( NULL), _lst_triangles( NULL), 
			  _num_vertices( 0), _num_triangles( 0), _allocation_strategy( BoundaryBinaryMesh::NONE) {};
    void set( const MeshPoint *l_v, const int64_t n_v, 
	      const BoundaryTriangle *l_t, const int64_t n_t,
	      const t_memory_management mem_str);
    BoundaryBinaryMesh( const BoundaryBinaryMesh &bm) { // always copy the data
      set( bm._lst_vertices, bm._num_vertices, bm._lst_triangles, bm._num_triangles, BoundaryBinaryMesh::COPY);
    }
    void reset();
    ~BoundaryBinaryMesh() { reset();}
    std::string toString();
    bool fromString( const char *binaryMeshData, const size_t binaryMeshSize, const t_memory_management mem_str);
    bool fromString( const std::string str, const t_memory_management mem_str) {
      return fromString( str.data(), str.length(), mem_str);
    }

    const MeshPoint *getLstVertices() const { return _lst_vertices;}
    const BoundaryTriangle *getLstTriangles() const { return _lst_triangles;}
    int64_t getNumVertices() const { return _num_vertices;}
    int64_t getNumTriangles() const { return _num_triangles;}
    
  private:
    const MeshPoint *_lst_vertices;
    const BoundaryTriangle *_lst_triangles;
    int64_t _num_vertices, _num_triangles;
    t_memory_management _allocation_strategy;
  };
  
  inline void BoundaryBinaryMesh::set( const MeshPoint *l_v, const int64_t n_v, 
				       const BoundaryTriangle *l_t, const int64_t n_t,
				       const BoundaryBinaryMesh::t_memory_management mem_str) {
    if ( mem_str == BoundaryBinaryMesh::COPY) {
      MeshPoint *lst_vert = new MeshPoint[ n_v];
      for ( int64_t i = 0; i < n_v; i++) {
	lst_vert[ i] = l_v[ i];
      }
      BoundaryTriangle *lst_tri = new BoundaryTriangle[ n_v];
      for ( int64_t i = 0; i < n_t; i++) {
	lst_tri[ i] = l_t[ i];
      }
      _lst_vertices = lst_vert;
      _lst_triangles = lst_tri;
      _num_vertices = n_v;
      _num_triangles = n_t;
      _allocation_strategy = BoundaryBinaryMesh::DELEGATE_NEW;
    } else {
      _lst_vertices = l_v;
      _lst_triangles = l_t;
      _num_vertices = n_v;
      _num_triangles = n_t;
      _allocation_strategy = mem_str;
    }
  }
  
  inline void BoundaryBinaryMesh::reset() {
    switch( _allocation_strategy) {
    case BoundaryBinaryMesh::DELEGATE_NEW:
      delete[] _lst_vertices;
      delete[] _lst_triangles;
      break;
    case BoundaryBinaryMesh::DELEGATE_MALLOC:
      free( ( void *)_lst_vertices);
      free( ( void *)_lst_triangles);
      break;
    case BoundaryBinaryMesh::COPY: // it never can be COPY, as set() changes it to DELEGATE_NEW
    case BoundaryBinaryMesh::STATIC: // do nothing on STATIC
    case BoundaryBinaryMesh::NONE:
      break;
    }
    _lst_vertices = NULL;
    _lst_triangles = NULL;
    _num_vertices = 0;
    _num_triangles = 0;
    _allocation_strategy = BoundaryBinaryMesh::NONE;
  }
  
  inline std::string BoundaryBinaryMesh::toString() {
    std::ostringstream oss;
    oss << "NumberOfVertices: " << _num_vertices << std::endl;
    oss << "NumberOfFaces: " << _num_triangles << std::endl;
    oss.write( ( const char *)_lst_vertices, ( size_t)_num_vertices * sizeof( MeshPoint));
    oss.write( ( const char *)_lst_triangles, ( size_t)_num_triangles * sizeof( BoundaryTriangle));
    return oss.str();
  }

#define __STDC_FORMAT_MACROS
#ifndef _WIN32
#include <inttypes.h>
#else
#include <stdint.h>
#endif
  // needed for:
  // int64_t nv = 0;
  // int n = sscanf( data, "Key: %" SCNi64, &nv);
  // printf( "nv = %" PRIi64 "\n", nv);
  // it can be the case that inttypes.h was already included wihout the __STDC_FORMAT_MACROS ...
#ifndef __PRI64_PREFIX
# if __WORDSIZE == 64
#  define __PRI64_PREFIX	"l"
# else
#  define __PRI64_PREFIX	"ll"
# endif
#endif

#ifndef SCNi64
# define SCNi64		__PRI64_PREFIX "i"
#endif
  
#ifndef PRIi64
# define PRIi64		__PRI64_PREFIX "i"
#endif

  inline bool BoundaryBinaryMesh::fromString( const char *binaryMeshData, const size_t binaryMeshSize, 
					      const BoundaryBinaryMesh::t_memory_management mem_str) {
    int64_t nv = 0, nf = 0;
    int n = sscanf( binaryMeshData, "NumberOfVertices: %" SCNi64 " NumberOfFaces: %"SCNi64, &nv, &nf);
    if ( n != 2) {
      return false;
    }

    // skip the two lines:
    int num_lines = 2;
    size_t mesh_data_pos = 0; 
    while ( mesh_data_pos < binaryMeshSize) {
      char c = binaryMeshData[ mesh_data_pos];
      if ( ( c == '\n') || ( c == '\r')) {
	mesh_data_pos++;
	c = binaryMeshData[ mesh_data_pos]; // check for consecutive \n\r as in windows...
	if ( ( c == '\n') || ( c == '\r')) {
	  mesh_data_pos++;
	}
	num_lines--;
	if ( !num_lines)
	  break;
      } else {
	mesh_data_pos++;
      }
    }

#ifndef NDEBUG
    size_t last = mesh_data_pos + nv * sizeof( MeshPoint) + nf * sizeof( BoundaryTriangle);
    assert( last == binaryMeshSize);
#endif // NDEBUG
    MeshPoint *lst_vertices = ( MeshPoint *)&binaryMeshData[ mesh_data_pos];
    BoundaryTriangle *lst_faces = ( BoundaryTriangle *)&binaryMeshData[ mesh_data_pos + nv * sizeof( MeshPoint)];

    if ( nv && nf) {
      this->set( lst_vertices, nv, lst_faces, nf, mem_str);
    }
    return ( nv && nf);
  }
  
} // namespace VELaSSCo
