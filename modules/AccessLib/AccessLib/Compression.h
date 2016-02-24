/* -*- c++ -*- */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <string>
#include "Helpers.h"

// make sure both Compression.h Compression.cpp are the same in the
// EngineLayer/QueryManager and
// AccessLib/AccessLib

#define LOG_COMPRESS( a)   LOGGER << a << std::endl;

// VL_Compression: several compression methods, included Raw, i.e. no compression
// compression types: Raw, Zlib, Lzo, ...
// data_block is compressed using one of the strategies and
// 12 bytes are prepend to the data:
// CCCC01234567 + data
// CCCC --> 4 chars defined compression method: 'RAW ', 'ZLIB', 'LZO ', ... case does not matter
// 01234567 --> uint64_t length of the uncompressed data
class VL_CompressionStrategy;

#define VL_COMPRESSION_MINIMUM_DATA_SIZE_TO_COMPRESS ( 1024 * 1024) // 1 MByte

class VL_Compression {
public:
  enum CompressionType {
    None, Raw, Zlib, Lzo
  };
  VL_Compression(): m_compression( NULL), m_level( 1), m_compression_threshold( VL_COMPRESSION_MINIMUM_DATA_SIZE_TO_COMPRESS) {};
  ~VL_Compression();
  bool setCompressionType( CompressionType type);
  bool setCompressionTypeFromString( const std::string &str_comp_type);
  void setCompressionLevel( int level);
  void setCompressionThreshold( size_t mem_bytes) { // set size limit to start compression
    if ( mem_bytes > 100) // absolute minimum
      m_compression_threshold = mem_bytes;
  }
  CompressionType getCompressionType() const;
  std::string getCompressionTypeString() const;
  // a space separated list
  static std::string getCompressionTypeList();
  int getCompressionLevel() const {
    return m_level;
  }
  size_t getCompressionThreshold() const { return m_compression_threshold;}

  bool doCompress( const std::string &in, std::string **out) const;
  bool doUncompress( const std::string &in, std::string **out) const;

private:
  VL_CompressionStrategy *m_compression;
  int m_level;
  size_t m_compression_threshold;
};

// compression strategies
class VL_CompressionStrategy {
public:
  virtual void setCompressionLevel( int level) = 0;
  virtual VL_Compression::CompressionType getCompressionType() const = 0;
  static VL_CompressionStrategy *getCompressionStrategy( VL_Compression::CompressionType type);

  // data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
  virtual bool compressData( const char *data_in, const size_t data_in_size, char *data_out, size_t *data_size) const = 0;
  virtual bool uncompressData( const char *data_in, const size_t data_in_size, char *data_out, size_t *data_size) const = 0;

private:
};

// specific strategies in Compression.cpp
// class VL_ZlibCompression
// class VL_LzoCompression

inline VL_Compression::~VL_Compression() { 
  delete m_compression;
}

inline VL_Compression::CompressionType VL_Compression::getCompressionType() const {
  VL_Compression::CompressionType ret = m_compression ? m_compression->getCompressionType() : None;
  return ret;
}

inline bool VL_Compression::setCompressionType( CompressionType type) {
  delete m_compression;
  m_compression = VL_CompressionStrategy::getCompressionStrategy( type);
  setCompressionLevel( m_level);
  return ( type == CompressionType::Raw) || m_compression;
}

inline void VL_Compression::setCompressionLevel( int level) {
  assert( ( level >= 1) && ( level <= 9));
  m_level = level;
  if ( m_compression)
    m_compression->setCompressionLevel( level);
}
