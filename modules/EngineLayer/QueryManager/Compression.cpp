#include <Compression.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>

// Lzo compression, using embedded minilzo, instead of std lzo library
#include "minilzo.h"

#ifdef _WIN32

/* strncasecmp */
static unsigned char charmap[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xe1, 0xe2, 0xe3, 0xe4, 0xc5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

/* First string, second string, Maximum number of characters to compare
   (stops earlier if the end of either string is reached). */
inline int strncasecmp( char const *s1, char const *s2, size_t length) {
    unsigned char u1, u2;

    for (; length != 0; length--, s1++, s2++) {
	u1 = (unsigned char) *s1;
	u2 = (unsigned char) *s2;
	if (charmap[u1] != charmap[u2]) {
	    return charmap[u1] - charmap[u2];
	}
	if (u1 == '\0') {
	    return 0;
	}
    }
    return 0;
}
#endif // _WIN32
class VL_ZlibCompression : public VL_CompressionStrategy  {
public:
  VL_ZlibCompression(): m_level( 1) {}
  ~VL_ZlibCompression() {};

  void setCompressionLevel( int level) { m_level = level;}
  VL_Compression::CompressionType getCompressionType() const { return VL_Compression::CompressionType::Zlib;}
private:
  // data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
  bool compressData( const char *data_in, const size_t data_in_size, char *data_out, size_t *data_size) const;
  bool uncompressData( const char *data_in, const size_t data_in_size, char *data_out, size_t *data_size) const;

private:
  int m_level;
};

class VL_LzoCompression : public VL_CompressionStrategy  {
public:
  VL_LzoCompression();
  ~VL_LzoCompression();
  void setCompressionLevel( int level) { m_level = level;}
  VL_Compression::CompressionType getCompressionType() const { return VL_Compression::CompressionType::Lzo;}
private:
  // data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
  bool compressData( const char *data_in, const size_t data_in_size, char *data_out, size_t *data_size) const;
  bool uncompressData( const char *data_in, const size_t data_in_size, char *data_out, size_t *data_size) const;

private:
  int m_level;
  bool m_initialized, m_init_ok;
  static const size_t m_working_memory_size =   // 128 KBytes
  sizeof( lzo_align_t) * ( ( ( LZO1X_1_MEM_COMPRESS) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t));
  // ((lzo_uint)1 << D_BITS) * sizeof(lzo_dict_t)
  lzo_align_t __LZO_MMODEL *m_working_memory;
};

VL_CompressionStrategy *VL_CompressionStrategy::getCompressionStrategy( VL_Compression::CompressionType type) {
  VL_CompressionStrategy *ret = NULL;
  switch ( type) {
  case VL_Compression::CompressionType::None:   ret = NULL;                     break;
  case VL_Compression::CompressionType::Raw:    ret = NULL;                     break;
  case VL_Compression::CompressionType::Zlib:   ret = new VL_ZlibCompression(); break;
  case VL_Compression::CompressionType::Lzo:    ret = new VL_LzoCompression(); break;
  }
  return ret;
}

// data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
bool VL_ZlibCompression::compressData( const char *data_in, const size_t data_in_size, 
				       char *data_out, size_t *data_out_size) const {
  // check sizes ...
  if ( sizeof( size_t) > sizeof( uLong)) {
    // uLong / uLongf are declared as unsigned longs...
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

// VL_LzoCompression following testmini.c from miniLZO example
VL_LzoCompression::VL_LzoCompression(): m_level( 1), 
					  m_initialized( false), m_init_ok( false), m_working_memory( NULL) {
  m_init_ok = ( lzo_init() == LZO_E_OK);
  m_initialized = true;
  if ( !m_init_ok) {
    LOG_COMPRESS( "Error initializing Lzo library \n\t(this usually indicates a compiler bug - try recompiling\n\twithout optimizations, and enable '-DLZO_DEBUG' for diagnostics)");
  } else {
    // working memory
    m_working_memory = ( lzo_align_t __LZO_MMODEL *)malloc( m_working_memory_size);
  }
}

VL_LzoCompression::~VL_LzoCompression() {
  free( m_working_memory);
}

 static const char *lzoErrorString( int lzo_error) {
   const char *ret = "Unknown error";
   switch( lzo_error) {
   case LZO_E_OK:                    ret = "LZO_E_OK"; break;
   case LZO_E_ERROR:                 ret = "LZO_E_ERROR"; break;
   case LZO_E_OUT_OF_MEMORY:         ret = "LZO_E_OUT_OF_MEMORY, [lzo_alloc_func_t failure]"; break;
   case LZO_E_NOT_COMPRESSIBLE:      ret = "LZO_E_NOT_COMPRESSIBLE"; break;  /* [not used right now] */
   case LZO_E_INPUT_OVERRUN:         ret = "LZO_E_INPUT_OVERRUN"; break;
   case LZO_E_OUTPUT_OVERRUN:        ret = "LZO_E_OUTPUT_OVERRUN"; break;
   case LZO_E_LOOKBEHIND_OVERRUN:    ret = "LZO_E_LOOKBEHIND_OVERRUN"; break;
   case LZO_E_EOF_NOT_FOUND:         ret = "LZO_E_EOF_NOT_FOUND"; break;
   case LZO_E_INPUT_NOT_CONSUMED:    ret = "LZO_E_INPUT_NOT_CONSUMED"; break;
   case LZO_E_NOT_YET_IMPLEMENTED:   ret = "LZO_E_NOT_YET_IMPLEMENTED"; break;  /* [not used right now] */
   case LZO_E_INVALID_ARGUMENT:      ret = "LZO_E_INVALID_ARGUMENT"; break;
   case LZO_E_INVALID_ALIGNMENT:     ret = "LZO_E_INVALID_ALIGNMENT, pointer argument is not properly aligned"; break;
   case LZO_E_OUTPUT_NOT_CONSUMED:   ret = "LZO_E_OUTPUT_NOT_CONSUMED"; break;
   case LZO_E_INTERNAL_ERROR:        ret = "LZO_E_INTERNAL_ERROR"; break;
   }
   return ret;
 }

// data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
bool VL_LzoCompression::compressData( const char *data_in, const size_t data_in_size, 
				       char *data_out, size_t *data_out_size) const {
  // check sizes ...
  if ( sizeof( size_t) > sizeof( lzo_uint)) {
    // lzo_uint / u Longf are declared as unsigned longs...
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
  lzo_uint comp_size = ( lzo_uint)*data_out_size;
  // deterministic, zero the working memory
  lzo_memset( m_working_memory, 0, m_working_memory_size);
  int lzo_err = lzo1x_1_compress( ( unsigned char *)data_in, data_in_size, ( unsigned char *)data_out, &comp_size, m_working_memory);
  *data_out_size = ( size_t)comp_size;
  if ( lzo_err == LZO_E_OK) {
    ok = true;
  } else {
    LOG_COMPRESS( "Error compressing " << data_in_size << " bytes: " << "error #" << lzo_err << ": " << lzoErrorString( lzo_err));
  }
  return ok;
}
// data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
bool VL_LzoCompression::uncompressData( const char *data_in, const size_t data_in_size, 
					 char *data_out, size_t *data_out_size) const {
  // check sizes ...
  if ( sizeof( size_t) > sizeof( lzo_uint)) {
    // lzo_uint are declared as unsigned longs...
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
  lzo_uint original_size = ( lzo_uint)*data_out_size;
  lzo_uint ucomp_size = original_size;
  // deterministic, zero the working memory
  lzo_memset( m_working_memory, 0, m_working_memory_size);
  int lzo_err = lzo1x_decompress( ( unsigned char *)data_in, data_in_size, ( unsigned char *)data_out, &ucomp_size, m_working_memory); 
  *data_out_size = ( size_t)ucomp_size;
  
  if ( ( lzo_err == LZO_E_OK) && ( ucomp_size == original_size)) {
    ok = true;
  } else {
    if ( ucomp_size != original_size) {
      LOG_COMPRESS( "Error uncompressing data: uncompressed size ( " << ucomp_size << ") does not match original size ( " << original_size <<").");
    }
    LOG_COMPRESS( "Error uncompressing " << data_in_size << " bytes: " << "error #" << lzo_err << ": " << lzoErrorString( lzo_err));
  }
  return ok;
}

static const char *getStringFromCompressionType( const VL_Compression::CompressionType type) {
  // must be a 4 character string !!!
  const char *ret = "N/A ";
  switch ( type) {
  case VL_Compression::CompressionType::None:   assert( 0); ret = "None"; break; // shouldn't be here !!!
  case VL_Compression::CompressionType::Raw:    ret = "RAW "; break;
  case VL_Compression::CompressionType::Zlib:   ret = "ZLIB"; break;
  case VL_Compression::CompressionType::Lzo:    ret = "LZO "; break;
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
    } else if ( ( tolower( type[ 0]) == 'l') && !strncasecmp( type, "lzo ", 4)) {
      ret = VL_Compression::CompressionType::Lzo;
    } else {
      ret = VL_Compression::CompressionType::None;
    }
  }
  return ret;
}

// a space separated list
std::string VL_Compression::getCompressionTypeList() {
  return std::string( "Raw Zlib Lzo");
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
  // compression types: Raw, Zlib, Lzo, ...
  // data_block is compressed using one of the strategies and
  // 12 bytes are prepend to the data:
  // CCCC01234567 + data
  // CCCC --> 4 chars defined compression method: 'RAW ', 'ZLIB', 'LZO ', ... case does not matter
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
  // compression types: Raw, Zlib, Lzo, ...
  // data_block is compressed using one of the strategies and
  // 12 bytes are prepend to the data:
  // CCCC01234567 + data
  // CCCC --> 4 chars defined compression method: 'RAW ', 'ZLIB', 'LZO ', ... case does not matter
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
  bool ok = false;
  // header is 12 bytes long
  // compressed_data_size is I/O parameter: in = size of buffer to compress data, out = size of compressed data
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
