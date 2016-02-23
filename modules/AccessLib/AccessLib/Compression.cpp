#include <Compression.h>
#include <string.h>
#include <zlib.h>

VL_CompressionStrategy *VL_CompressionStrategy::getCompressionStrategy( VL_Compression::CompressionType type) {
  VL_CompressionStrategy *ret = NULL;
  switch ( type) {
  case VL_Compression::CompressionType::None:   ret = NULL;                     break;
  case VL_Compression::CompressionType::Raw:    ret = NULL;                     break;
  case VL_Compression::CompressionType::Zlib:   ret = new VL_ZlibCompression(); break;
  case VL_Compression::CompressionType::Lzop:   ret = new VL_LzopCompression(); break;
  }
  return ret;
}

// data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
bool VL_ZlibCompression::compressData( const char *data_in, const size_t data_in_size, 
				       char *data_out, size_t *data_out_size) const {
  // check sizes ...
  if ( sizeof( size_t) > sizeof( uLong)) {
    // uLong / u Longf are declared as unsigned longs...
    if ( ( data_in_size & 0xffffffff00000000LL) || ( *data_out_size & 0xffffffff00000000LL)) {
      LOG_COMPRESS( std::string( "Too much data to compress > 4GB."));
      return false;
    }
  }
  if ( !data_in) {
    LOG_COMPRESS( std::string( "No data to compress."));
    return false;
  }
  if ( !data_out) {
    LOG_COMPRESS( std::string( "No buffer for compressed data."));
    return false;
  }
  bool ok = false;
  uLongf comp_size = ( uLongf)*data_out_size;
  int z_err = compress2( ( Bytef *)data_out, &comp_size, ( const Bytef *)data_in, ( uLongf)data_in_size, m_level);
  *data_out_size = ( size_t)comp_size;
  if ( z_err == Z_OK) {
    ok = true;
  } else { // one of:
    // Z_MEM_ERROR if there was not enough memory
    // Z_BUF_ERROR if there was not enough room in the output buffer
    // Z_STREAM_ERROR if the level parameter is invalid.
    LOG_COMPRESS( "Error compressing " << data_in_size << " bytes: " << std::string( zError( z_err)));
  }
  return ok;
}
// data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
bool VL_ZlibCompression::uncompressData( const char *data_in, const size_t data_in_size, 
					 char *data_out, size_t *data_out_size) const {
  // check sizes ...
  if ( sizeof( size_t) > sizeof( uLong)) {
    // uLong / u Longf are declared as unsigned longs...
    if ( ( data_in_size & 0xffffffff00000000LL) || ( *data_out_size & 0xffffffff00000000LL)) {
      LOG_COMPRESS( std::string( "Too much data to uncompress > 4GB."));
      return false;
    }
  }
  if ( !data_in) {
    LOG_COMPRESS( std::string( "No data to uncompress."));
    return false;
  }
  if ( !data_out) {
    LOG_COMPRESS( std::string( "No buffer for uncompressed data."));
    return false;
  }

  bool ok = false;
  uLongf ucomp_size = ( uLongf)*data_out_size;
  int z_err = uncompress( ( Bytef *)data_out, &ucomp_size, ( const Bytef *)data_in, ( uLongf)data_in_size);
  *data_out_size = ( size_t)ucomp_size;
  
  if ( z_err == Z_OK) {
    ok = true;
  } else { // one of:
    // Z_MEM_ERROR if there was not enough memory
    // Z_BUF_ERROR if there was not enough room in the output buffer
    // Z_DATA_ERROR if the input data was corrupted or incomplete.
    LOG_COMPRESS( "Error uncompressing " << data_in_size << " bytes: " << std::string( zError( z_err)));
  }
  return ok;
}

// data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
bool VL_LzopCompression::compressData( const char *data_in, const size_t data_in_size, char *data_out, size_t *data_size) const {
  return false;
}
// data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
bool VL_LzopCompression::uncompressData( const char *data_in, const size_t data_in_size, char *data_out, size_t *data_size) const {
  return false;
}

static const char *getStringFromCompressionType( const VL_Compression::CompressionType type) {
  // must be a 4 character string !!!
  const char *ret = "N/A ";
  switch ( type) {
  case VL_Compression::CompressionType::None:   assert( 0); ret = "None"; break; // shouldn't be here !!!
  case VL_Compression::CompressionType::Raw:    ret = "RAW "; break;
  case VL_Compression::CompressionType::Zlib:   ret = "ZLIB"; break;
  case VL_Compression::CompressionType::Lzop:   ret = "LZOP"; break;
  }
  return ret;
}

static VL_Compression::CompressionType getCompressionTypeFromString( const char *type) {
  VL_Compression::CompressionType ret = VL_Compression::CompressionType::None;
  if ( type && *type) {
    if ( ( tolower( type[ 0]) == 'n') && !strncasecmp( type, "none", 4)) {
      assert( 0); // should be here at all
      ret = VL_Compression::CompressionType::None;
    } else if ( ( tolower( type[ 0]) == 'r') && !strncasecmp( type, "raw ", 4)) {
      ret = VL_Compression::CompressionType::Raw;
    } else if ( ( tolower( type[ 0]) == 'z') && !strncasecmp( type, "zlib", 4)) {
      ret = VL_Compression::CompressionType::Zlib;
    } else if ( ( tolower( type[ 0]) == 'l') && !strncasecmp( type, "lzop", 4)) {
      ret = VL_Compression::CompressionType::Lzop;
    } else {
      ret = VL_Compression::CompressionType::None;
    }
  }
  return ret;
}

// a space separated list
std::string VL_Compression::getCompressionTypeList() {
  // return std::string( "Raw Zlib LZop");
  return std::string( "Raw Zlib"); // LZop not implemented yet
}

std::string VL_Compression::getCompressionTypeString() const {
  return std::string( getStringFromCompressionType( getCompressionType()));
}

bool VL_Compression::setCompressionTypeFromString( const std::string &str_comp_type) {
  return setCompressionType( getCompressionTypeFromString( str_comp_type.c_str()));
}


static char *buildCompressedFrame( const VL_Compression::CompressionType comp_type, const size_t data_in_size) {
  uint64_t data_size = ( uint64_t)data_in_size;
  
  // allocate buffer for compressed data & header
  // compression types: Raw, Zlib, Lzop, ...
  // data_block is compressed using one of the strategies and
  // 12 bytes are prepend to the data:
  // CCCC01234567 + data
  // CCCC --> 4 chars defined compression method: 'RAW ', 'ZLIB', 'LZOP', ... case does not matter
  // 01234567 --> uint64_t length of the uncompressed data
  size_t frame_size = data_in_size + 12;
  char *compressedFrame = new char[ frame_size];

  // header: 4 bytes for compression_type, 8 bytes for original_data_size
  const char *str_type = getStringFromCompressionType( comp_type);
  memcpy( compressedFrame, str_type, 4);
  typedef union {
    uint64_t i;
    char c[ 8];
  } tInt64Char8;
  tInt64Char8 tmp;
  tmp.i = data_size;
  for ( int i = 0; i < 8; i++) {
    compressedFrame[ 4 + i] = tmp.c[ i];
  }
  return compressedFrame;
}

static char *buildUncompressedFrame( const char *compressedFrame, size_t *data_out_size) {
  if ( !compressedFrame || !data_out_size)
    return NULL;
  
  // allocate buffer for uncompressed data
  // compression types: Raw, Zlib, Lzop, ...
  // data_block is compressed using one of the strategies and
  // 12 bytes are prepend to the data:
  // CCCC01234567 + data
  // CCCC --> 4 chars defined compression method: 'RAW ', 'ZLIB', 'LZOP', ... case does not matter
  // 01234567 --> uint64_t length of the uncompressed data
  typedef union {
    uint64_t i;
    char c[ 8];
  } tInt64Char8;
  tInt64Char8 tmp;
  for ( int i = 0; i < 8; i++) {
    tmp.c[ i] = compressedFrame[ 4 + i];
  }

  size_t frame_size = tmp.i;
  char *uncompressedFrame = new char[ frame_size];
  *data_out_size = frame_size;
  return uncompressedFrame;
}

bool VL_Compression::doCompress( const std::string &in, std::string **out) const {
  const char *data_in = in.data();
  size_t data_size = in.size();
  
  // if no compression set || or too little data, use Raw by default
  CompressionType comp_type = getCompressionType();
  const VL_CompressionStrategy *comp_alg = m_compression;
  const size_t data_size_limit = m_compression_threshold;
  if ( !comp_alg || ( data_size < data_size_limit)) {
    comp_type = CompressionType::Raw;
    comp_alg = NULL; // Raw
  }
  
  char *compressed_frame = buildCompressedFrame( comp_type, data_size);
  // header is 12 bytes long
  // data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
  bool ok = false;
  size_t compressed_data_size = data_size;
  if ( comp_alg) {
    ok = comp_alg->compressData( data_in, data_size, &compressed_frame[ 12], &compressed_data_size); // at least same size
    if ( !ok) {
      const char *str_type = getStringFromCompressionType( comp_type);
      LOG_COMPRESS( std::string( str_type) << ": Couldn't compress " << data_size << " bytes.");
    }
  } else {
    // Raw compression: already handled in next step
  }
  
  if ( !ok) {
    // couldn't compress, so copy it ( i.e. use Raw )
    const char *str_type = getStringFromCompressionType( CompressionType::Raw);
    memcpy( compressed_frame, str_type, 4); 
    // size already copied in buildCompressedFrame
    memcpy( &compressed_frame[ 12], data_in, data_size);
    compressed_data_size = data_size;
    ok = true;
  }
  // build string:
  *out = new std::string( compressed_frame, 12 + compressed_data_size); // header + compressed_data
  delete[] compressed_frame; // not needed any more

  return ok;
}

bool VL_Compression::doUncompress( const std::string &in, std::string **out) const {
  // read header( 4-char-type + 8-byte-original-size)
  const char *data_in = in.data();
  size_t data_size = in.size();
  
  if ( data_size < 13) {
    // header alone is 12 bytes big!
    return false;
  }
  
  CompressionType comp_type = getCompressionTypeFromString( data_in);
  if ( comp_type == CompressionType::None) {
    // un recognized compression method
    return false;
  }

  VL_CompressionStrategy *comp_alg = VL_CompressionStrategy::getCompressionStrategy( comp_type);

  size_t data_out_size = 0;
  char *uncompressed_frame = buildUncompressedFrame( data_in, &data_out_size);

  // compressed data starts just after the 12byte header.
  bool ok = false;
  if ( comp_alg) {
    ok = comp_alg->uncompressData( &data_in[ 12], data_size - 12, uncompressed_frame, &data_out_size);
  } else { // i.e. Raw compression handled in the next step
  }
  if ( !ok) {
    // couldn't uncompress, so copy it ( i.e. use Raw )
    memcpy( uncompressed_frame, &data_in[ 12], data_size - 12);
    data_out_size = data_size - 12;
    ok = true;
  }
  // build string:
  *out = new std::string( uncompressed_frame, data_out_size); // header + compressed_data
  delete[] uncompressed_frame; // not needed any more
  delete comp_alg;

  return ok;
}
