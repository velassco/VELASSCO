/* -*- c++ -*- */
#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <stddef.h>  // defines NULL
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace VELaSSCo {
  class BoundaryBinaryMesh {
    // BoundaryBinaryMesh can be a mesh of triangles or a mesh of quadrilaterals, but not both !!!
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
    typedef struct {
      int _num_nodes;
      int64_t _nodes[ 4];
    } BoundaryQuadrilateral;
#pragma pack( pop)
  
    BoundaryBinaryMesh(): _lst_vertices( NULL), _lst_triangles( NULL), _lst_quadrilaterals( NULL), 
			  _num_vertices( 0), _num_triangles( 0), _num_quadrilaterals( 0), _allocation_strategy( BoundaryBinaryMesh::NONE) {};
    void set( const MeshPoint *l_v, const int64_t n_v, 
	      const BoundaryTriangle *l_t, const int64_t n_t,
	      const t_memory_management mem_str);
    void set( const MeshPoint *l_v, const int64_t n_v, 
	      const BoundaryQuadrilateral *l_t, const int64_t n_t,
	      const t_memory_management mem_str);
    BoundaryBinaryMesh( const BoundaryBinaryMesh &bm) { // always copy the data
      if ( bm._num_triangles > 0) {
	set( bm._lst_vertices, bm._num_vertices, bm._lst_triangles, bm._num_triangles, BoundaryBinaryMesh::COPY);
      } else if ( bm._num_quadrilaterals > 0) {
	set( bm._lst_vertices, bm._num_vertices, bm._lst_quadrilaterals, bm._num_quadrilaterals, BoundaryBinaryMesh::COPY);
      }
    }
    void reset();
    ~BoundaryBinaryMesh() { reset();}
    std::string toString();
    bool fromString( const char *binaryMeshData, const size_t binaryMeshSize, const t_memory_management mem_str);
    bool fromString( const std::string str, const t_memory_management mem_str) {
      return fromString( str.data(), str.length(), mem_str);
    }
    bool toFile( const std::string &filename);
    bool fromFile( const std::string &filename);

    const MeshPoint *getLstVertices() const { return _lst_vertices;}
    const BoundaryTriangle *getLstTriangles() const { return _lst_triangles;}
    const BoundaryQuadrilateral *getLstQuadrilaterals() const { return _lst_quadrilaterals;}
    int64_t getNumVertices() const { return _num_vertices;}
    int64_t getNumTriangles() const { return _num_triangles;}
    int64_t getNumQuadrilaterals() const { return _num_quadrilaterals;}
    
  private:
    const MeshPoint *_lst_vertices;
    const BoundaryTriangle *_lst_triangles;
    const BoundaryQuadrilateral *_lst_quadrilaterals;
    int64_t _num_vertices, _num_triangles, _num_quadrilaterals;
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
      BoundaryTriangle *lst_tri = new BoundaryTriangle[ n_t];
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
  
  inline void BoundaryBinaryMesh::set( const MeshPoint *l_v, const int64_t n_v, 
				       const BoundaryQuadrilateral *l_t, const int64_t n_t,
				       const BoundaryBinaryMesh::t_memory_management mem_str) {
    if ( mem_str == BoundaryBinaryMesh::COPY) {
      MeshPoint *lst_vert = new MeshPoint[ n_v];
      for ( int64_t i = 0; i < n_v; i++) {
	lst_vert[ i] = l_v[ i];
      }
      BoundaryQuadrilateral *lst_tri = new BoundaryQuadrilateral[ n_v];
      for ( int64_t i = 0; i < n_t; i++) {
	lst_tri[ i] = l_t[ i];
      }
      _lst_vertices = lst_vert;
      _lst_quadrilaterals = lst_tri;
      _num_vertices = n_v;
      _num_quadrilaterals = n_t;
      _allocation_strategy = BoundaryBinaryMesh::DELEGATE_NEW;
    } else {
      _lst_vertices = l_v;
      _lst_quadrilaterals = l_t;
      _num_vertices = n_v;
      _num_quadrilaterals = n_t;
      _allocation_strategy = mem_str;
    }
  }
  
  inline void BoundaryBinaryMesh::reset() {
    switch( _allocation_strategy) {
    case BoundaryBinaryMesh::DELEGATE_NEW:
      delete[] _lst_vertices;
      delete[] _lst_triangles;
      delete[] _lst_quadrilaterals;
      break;
    case BoundaryBinaryMesh::DELEGATE_MALLOC:
      free( ( void *)_lst_vertices);
      free( ( void *)_lst_triangles);
      free( ( void *)_lst_quadrilaterals);
      break;
    case BoundaryBinaryMesh::COPY: // it never can be COPY, as set() changes it to DELEGATE_NEW
    case BoundaryBinaryMesh::STATIC: // do nothing on STATIC
    case BoundaryBinaryMesh::NONE:
      break;
    }
    _lst_vertices = NULL;
    _lst_triangles = NULL;
    _lst_quadrilaterals = NULL;
    _num_vertices = 0;
    _num_triangles = 0;
    _num_quadrilaterals = 0;
    _allocation_strategy = BoundaryBinaryMesh::NONE;
  }
  
  inline std::string BoundaryBinaryMesh::toString() {
    std::ostringstream oss;
    oss << "NumberOfVertices: " << _num_vertices << std::endl;
    if ( _num_triangles > 0) {
      oss << "NumberOfFaces: " << _num_triangles << std::endl;
      oss.write( ( const char *)_lst_vertices, ( size_t)_num_vertices * sizeof( MeshPoint));
      oss.write( ( const char *)_lst_triangles, ( size_t)_num_triangles * sizeof( BoundaryTriangle));
    } else if ( _num_quadrilaterals > 0) {
      oss << "NumberOfFaces: " << _num_quadrilaterals << std::endl;
      oss.write( ( const char *)_lst_vertices, ( size_t)_num_vertices * sizeof( MeshPoint));
      oss.write( ( const char *)_lst_quadrilaterals, ( size_t)_num_quadrilaterals * sizeof( BoundaryQuadrilateral));
    } else {
      oss << "NumberOfFaces: " << 0 << std::endl;
    }
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
    int64_t numReadVertices = 0, numReadFaces = 0;
    int n = sscanf( binaryMeshData, "NumberOfVertices: %" SCNi64 " NumberOfFaces: %"SCNi64, &numReadVertices, &numReadFaces);
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

    MeshPoint *lst_vertices = ( MeshPoint *)&binaryMeshData[ mesh_data_pos];

    // first try as triangles ( they are smaller)
    // then if num nodes of triangles == 4, we can map it to quadrilaterals
    BoundaryTriangle *lst_triangles = ( BoundaryTriangle *)&binaryMeshData[ mesh_data_pos + numReadVertices * sizeof( MeshPoint)];
    BoundaryQuadrilateral *lst_quadrilaterals = ( BoundaryQuadrilateral *)&binaryMeshData[ mesh_data_pos + numReadVertices * sizeof( MeshPoint)];

    if ( numReadVertices && numReadFaces) {
      if ( lst_triangles[ 0]._num_nodes == 3) { // they are effectivelly triangles
#ifndef NDEBUG
        size_t last = mesh_data_pos + numReadVertices * sizeof( MeshPoint) + numReadFaces * sizeof( BoundaryTriangle);
        assert( last == binaryMeshSize);
#endif // NDEBUG
	this->set( lst_vertices, numReadVertices, lst_triangles, numReadFaces, mem_str);
      } else if ( lst_quadrilaterals[ 0]._num_nodes == 4) { // they are in fact quadrilaterals !!!
#ifndef NDEBUG
        size_t last = mesh_data_pos + numReadVertices * sizeof( MeshPoint) + numReadFaces * sizeof( BoundaryQuadrilateral);
        assert( last == binaryMeshSize);
#endif // NDEBUG
	this->set( lst_vertices, numReadVertices, lst_quadrilaterals, numReadFaces, mem_str);
      }
    }
    return ( numReadVertices && numReadFaces);
  }

  inline bool BoundaryBinaryMesh::toFile( const std::string &filename) {
    std::ofstream fileOut;
    fileOut.open( filename, std::ios::binary | std::ios::out);
    bool ok = fileOut.good();
    if ( ok) {
      int magicNumber = 0xb0b1f11e; // BOundary BInary FILE, also to detect Big/Little endianess
      fileOut.write( ( char *)&magicNumber, sizeof( int));
      const std::string &str = this->toString();
      size_t str_len = str.size();
      fileOut.write( ( char *)&str_len, sizeof( size_t));
      fileOut.write( str.data(), str.size());
      ok = fileOut.good();
      fileOut.close();
    }
    return ok;
  }

  inline bool BoundaryBinaryMesh::fromFile( const std::string &filename) {
    std::ifstream fileIn;
    fileIn.open( filename, std::ios::binary | std::ios::in);
    bool ok = fileIn.good();
    if ( ok) {
      int magicNumber = 0;
      fileIn.read( ( char *)&magicNumber, sizeof( int));
      // check if it's == 0xb0b1f11e; // BOundary BInary FILE
      size_t str_len = 0;
      fileIn.read( ( char *)&str_len, sizeof( size_t));
      char *fileContents = new char[ str_len];
      fileIn.read( fileContents, str_len);;
      fileIn.close();
      ok = this->fromString( fileContents, str_len, COPY);
      delete[] fileContents;
    }
    return ok;
  }
  
} // namespace VELaSSCo
