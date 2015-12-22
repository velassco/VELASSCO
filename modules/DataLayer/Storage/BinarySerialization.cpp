#include "BinarySerialization.h"
#include <iostream>
#include <stdint.h>

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;

BEGIN_GID_DECLS

double bswap( double w )
{
  union { uint64_t quad; double d; } t;
  t.d = w;
  t.quad = bswap( t.quad );
  return t.d;
}

inline
static bool is_big_endian(void)
{
  union {
    uint32_t i;
    char c[4];
  } bint = {0x01020304};
  
  return bint.c[0] == 1; 
}

inline
static Endianness get_my_endianness()
{
  return is_big_endian() ? BigEndian : LittleEndian;
}

inline
static bool is_endianness_native( Endianness t )
{
  return (t == HostEndian) ? true : (get_my_endianness( ) == t); 
}

inline
void rappend( std::string &dest, const char *from, int length )
{
  int i = length;
  while( i > 0 )
    {
    --i;
    dest += from[ i ];
    }
}

static char hexTable[] =
{
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'a', 'b', 'c', 'd', 'e', 'f'
};

inline void CharToHex( char c, char hex[2] )
{
  hex[0] = hexTable[ (0xF0 & c) >> 4 ];
  hex[1] = hexTable[ (0x0F & c) ];
};

template <class T>
inline void BinToHex( T v, char hex[ sizeof( T ) * 2 ] )
{
  char *addr =reinterpret_cast<char*> (&v);
  char tmp[2];
  for( int i = sizeof( T ) - 1, j = 0; i >= 0; --i, ++j )
    {
    CharToHex( addr[ i ], tmp );
    hex[ 2 * j ] = tmp[ 0 ];
    hex[ 2 * j + 1 ] = tmp[ 1 ];
    } 
}

inline bool HexValue( char h, char & c )
{
  if ( h >= '0' && h <= '9' )
    {
    c = h - '0';
    return true;
    }
  if ( h >= 'a' && h <= 'f' )
    {
    c = h - 'a' + 10;
    return true;
    }
  c = 0xff;
  return false;
}

inline bool HexToChar( const char hex[2], char & c )
{
  char h0, h1;
  if ( HexValue( hex[0], h0 ) && HexValue( hex[1], h1 ) )
    {
    c = (h0 << 4) | h1;
    return true;
    }
  return false;
};

template <class T>
inline bool HexToBin( const char hex[ sizeof( T ) * 2 ], T & v )
{
  char *addr =reinterpret_cast<char*> (&v);
  char h;
  for( int i = sizeof( T ) - 1, j = 0; i >= 0; --i, j+=2 )
    {
    if ( !HexToChar( hex + j, h ) )
      {
      return false;
      }
    addr[i] = h;
    }
  return true;
}

void BinarySerializer::BinToHex( char c, char buffer[2] )
{
  CharToHex( c, buffer );
}

std::string BinarySerializer::BinToHex( const std::string &source )
{
  std::string hexStr;
  char tmp[2];
  for( std::string::const_iterator it = source.begin( );
       it != source.end( ); it++ )
    {
    CharToHex( *it, tmp );
    hexStr.append( tmp, 2 );
    }
  return hexStr;
}

bool BinarySerializer::IsEndiannessNative()
{
  return is_endianness_native( this->GetEndianness( ) );
}

void BinarySerializer::AppendWithEndianness( std::string &dest, const char *buffer, int length )
{
  if ( this->IsEndiannessNative( ) )
    {
    dest.append( buffer, sizeof( length ) );
    }
  else
    {
    rappend( dest, buffer, sizeof( length ) );
    }
}

BinarySerializerThrift::BinarySerializerThrift( )
  : m_Transport( new TMemoryBuffer( ) ), 
    m_Protocol( new TBinaryProtocol( this->m_Transport ) )
{
}

BinarySerializerThrift::~BinarySerializerThrift( )
{
}

void BinarySerializerThrift::MakeRoom( boost::uint32_t size )
{
  this->m_Transport->resetBuffer( );
  if ( this->m_Transport->available_write( ) < size )
    {
    this->m_Transport->resetBuffer( size );
    }
}

boost::uint32_t BinarySerializerThrift::Write( std::string &dest,
                                               const boost::int8_t *values, boost::uint32_t n )
{
  this->MakeRoom( n );
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->writeByte( values[ i ] );
    }
  this->m_Transport->appendBufferToString( dest );
  return n;
}

boost::uint32_t BinarySerializerThrift::Write( std::string &dest, const std::string &str )
{
  boost::uint32_t lengthStr = str.length( );
  boost::uint32_t n = sizeof( lengthStr ) + lengthStr;
  this->MakeRoom( n );
  this->m_Protocol->writeI32( lengthStr );
  for( boost::uint32_t i = 0; i < lengthStr; i++ )
    {
    this->m_Protocol->writeByte( str.c_str()[i] );
    }
  this->m_Transport->appendBufferToString( dest );
  return n;
}


boost::uint32_t BinarySerializerThrift::Write( std::string &dest,
                                               const boost::int16_t *values, boost::uint32_t n )
{
  boost::uint32_t needed = sizeof( boost::int16_t ) * n;
  this->MakeRoom( needed );
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->writeI16( values[ i ] );
    }
  this->m_Transport->appendBufferToString( dest );
  return needed;
}

boost::uint32_t BinarySerializerThrift::Write( std::string &dest,
                                               const boost::int32_t *values, boost::uint32_t n )
{
  boost::uint32_t needed = sizeof( boost::int32_t ) * n;
  this->MakeRoom( needed );
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->writeI32( values[ i ] );
    }
  this->m_Transport->appendBufferToString( dest );
  return needed;
}

boost::uint32_t BinarySerializerThrift::Write( std::string &dest,
                                               const boost::int64_t *values, boost::uint32_t n )
{
  boost::uint32_t needed = sizeof( boost::int64_t ) * n;
  this->MakeRoom( needed );
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->writeI64( values[ i ] );
    }
  this->m_Transport->appendBufferToString( dest );
  return needed;
}

boost::uint32_t BinarySerializerThrift::Write( std::string &dest,
                                               const double *values, boost::uint32_t n )
{
  boost::uint32_t needed = sizeof( double ) * n;
  this->MakeRoom( needed );
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->writeDouble( values[ i ] );
    }
  this->m_Transport->appendBufferToString( dest );
  return needed;
}

BinarySerializerNative::BinarySerializerNative( )
{
}

BinarySerializerNative::~BinarySerializerNative( )
{
}

boost::uint32_t BinarySerializerNative::Write( std::string &dest,
                                               const boost::int8_t *values, boost::uint32_t n )
{
  const char *pChar = reinterpret_cast<const char*>(values);
  if ( this->GetConvertToHex( ) )
    {
    std::string tmp0( pChar, n );
    std::string tmp = BinarySerializer::BinToHex( tmp0 );
    dest.append( tmp );
    return 2 * n;
    }
  else
    {
    dest.append( pChar, n );
    return n;
    }
}

boost::uint32_t BinarySerializerNative::Write( std::string &dest, const std::string &str )
{
  boost::uint32_t lengthStr = str.length( );
  boost::uint32_t n;
  
  // write string length
  if ( this->GetConvertToHex( ) )
    {
    char tmp[ sizeof(lengthStr)*2 ];
    GID::BinToHex( lengthStr, tmp );
    dest.append( tmp, sizeof( lengthStr ) * 2 );
    n = sizeof( lengthStr ) * 2;
    }
  else
    {
    boost::uint32_t *pLength = &lengthStr;
    char *pChar = reinterpret_cast<char*>(pLength);
    this->AppendWithEndianness( dest, pChar, sizeof( lengthStr ) );
    n = sizeof( lengthStr );
    }

  // write the string's content
  dest.append( str );
  n += lengthStr;
  return n;
}

boost::uint32_t BinarySerializerNative::Write( std::string &dest,
                                               const boost::int16_t *values, boost::uint32_t n )
{
  const boost::uint32_t sizeValue = sizeof( boost::int16_t );
  boost::uint32_t needed = sizeValue * n;
  if ( this->GetConvertToHex( ) )
    {
    char tmp[sizeValue * 2];
    for( int i = 0; i < n; i++ )
      {
      GID::BinToHex( values[i], tmp );
      dest.append( tmp, sizeValue * 2 );
      }
    return needed * 2;    
    }
  if( this->IsEndiannessNative( ) )
    {
    const char *pChar = reinterpret_cast<const char*>( values );
    dest.append( pChar, needed );
    }
  else
    {
    for( int i = 0; i < n; i++ )
      {
      const char *pChar = reinterpret_cast<const char*>( values+i );
      rappend( dest, pChar, sizeValue );
      }
    }
  return needed;   
}

boost::uint32_t BinarySerializerNative::Write( std::string &dest,
                                               const boost::int32_t *values, boost::uint32_t n )
{
  const boost::uint32_t sizeValue = sizeof( boost::int32_t );
  boost::uint32_t needed = sizeValue * n;

  if ( this->GetConvertToHex( ) )
    {
    char tmp[sizeValue * 2];
    for( int i = 0; i < n; i++ )
      {
      GID::BinToHex( values[i], tmp );
      dest.append( tmp, sizeValue *2 );      
      }
    return needed * 2;    
    }
  if( this->IsEndiannessNative( ) )
    {
    const char *pChar = reinterpret_cast<const char*>(values);
    dest.append( pChar, needed );
    }
  else
    {
    for( int i = 0; i < n; i++ )
      {
      const char *pChar = reinterpret_cast<const char*>( values+i );
      rappend( dest, pChar, sizeValue );
      }
    }
  return needed;
}

boost::uint32_t BinarySerializerNative::Write( std::string &dest,
                                               const boost::int64_t *values, boost::uint32_t n )
{
  const boost::uint32_t sizeValue = sizeof( boost::int64_t );
  boost::uint32_t needed = sizeValue * n;

  if ( this->GetConvertToHex( ) )
    {
    char tmp[sizeValue * 2];
    for( int i = 0; i < n; i++ )
      {
      GID::BinToHex( values[i], tmp );
      dest.append( tmp, sizeValue * 2 );      
      }
    return needed * 2;    
    }
  if( this->IsEndiannessNative( ) )
    {
    const char *pChar = reinterpret_cast<const char*>(values);
    dest.append( pChar, needed );
    }
  else
    {
    for( int i = 0; i < n; i++ )
      {
      const char *pChar = reinterpret_cast<const char*>( values+i );
      rappend( dest, pChar, sizeValue );
      }
    }
  return needed;
}

boost::uint32_t BinarySerializerNative::Write( std::string &dest,
                                               const double *values, boost::uint32_t n )
{
  const boost::uint32_t sizeValue = sizeof( double );
  boost::uint32_t needed = sizeValue * n;

  if ( this->GetConvertToHex( ) )
    {
    char tmp[ sizeValue * 2 ];
    for( int i = 0; i < n; i++ )
      {
      GID::BinToHex( values[i], tmp );
      dest.append( tmp, sizeValue * 2 );      
      }
    return needed * 2;    
    }
  if( this->IsEndiannessNative( ) )
    {
    const char *pChar = reinterpret_cast<const char*>(values);
    dest.append( pChar, needed );
    }
  else
    {
    for( int i = 0; i < n; i++ )
      {
      const char *pChar = reinterpret_cast<const char*>( values+i );
      rappend( dest, pChar, sizeValue );
      }
    }
  return needed;
}

BinaryDeserializerThrift::BinaryDeserializerThrift( )
  : m_Transport( new TMemoryBuffer( ) ), 
    m_Protocol( new TBinaryProtocol( this->m_Transport ) )
{
}

BinaryDeserializerThrift::~BinaryDeserializerThrift( )
{
}

boost::uint32_t BinaryDeserializerThrift::ResetBuffer( const std::string &source,
                                                       boost::uint32_t pos,
                                                       boost::uint32_t needed )
{
  boost::uint32_t lengthBuffer = source.length();
  boost::uint32_t actualLength = lengthBuffer - pos;
  
  if ( actualLength < needed )
    {
    return 0;
    }
  this->m_Transport->resetBuffer( (uint8_t*)(source.c_str()) + pos, actualLength );
  return actualLength;
}

boost::uint32_t BinaryDeserializerThrift::Read( const std::string &source,
                                                boost::int8_t *values, boost::uint32_t n,
                                                boost::uint32_t pos )
{
  if ( !this->ResetBuffer( source, pos, n ) )
    {
    return pos;
    }
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->readByte( values[ i ] );
    }
  return pos + n;
}

boost::uint32_t BinaryDeserializerThrift::Read( const std::string &source,
                                                std::string &str,
                                                boost::uint32_t pos )
{
  const boost::uint32_t minLength = sizeof( boost::uint32_t );
  boost::uint32_t actualLength = this->ResetBuffer( source, pos, minLength );
  if ( !actualLength )
    {
    return pos;
    }
  // read string length
  boost::int32_t ilengthStr;
  this->m_Protocol->readI32( ilengthStr );
  boost::uint32_t lengthStr = *( reinterpret_cast<boost::uint32_t*>( &ilengthStr ) );
  if ( actualLength - minLength < lengthStr )
    {
    return pos;
    }
  if ( lengthStr )
    {
    boost::int8_t *tmp = new boost::int8_t[ lengthStr ];   
    for( boost::uint32_t i = 0; i < lengthStr; i++ )
      {
      this->m_Protocol->readByte( tmp[ i ] );
      }
    str.append( reinterpret_cast<char*>(&tmp[0]), lengthStr );
    delete []tmp;
    }
  else
    {
    str.clear( );
    }
  return pos + minLength + lengthStr;
}

boost::uint32_t BinaryDeserializerThrift::Read( const std::string &source,
                                          boost::int16_t *values, boost::uint32_t n,
                                          boost::uint32_t pos )
{
  boost::uint32_t needed = sizeof( boost::int16_t ) * n;

  if ( !this->ResetBuffer( source, pos, needed ) )
    {
    return pos;
    }
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->readI16( values[ i ] );
    }
  return pos + needed;
}

boost::uint32_t BinaryDeserializerThrift::Read( const std::string &source,
                                          boost::int32_t *values, boost::uint32_t n,
                                          boost::uint32_t pos )
{
  boost::uint32_t needed = sizeof( boost::int32_t ) * n;

  if ( !this->ResetBuffer( source, pos, needed ) )
    {
    return pos;
    }
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->readI32( values[ i ] );
    }
  return pos + needed;
}

boost::uint32_t BinaryDeserializerThrift::Read( const std::string &source,
                                          boost::int64_t *values, boost::uint32_t n,
                                          boost::uint32_t pos )
{
  boost::uint32_t needed = sizeof( boost::int64_t ) * n;

  if ( !this->ResetBuffer( source, pos, needed ) )
    {
    return pos;
    }
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->readI64( values[ i ] );
    }
  return pos + needed;
}

boost::uint32_t BinaryDeserializerThrift::Read( const std::string &source,
                                                double *values, boost::uint32_t n,
                                                boost::uint32_t pos )
{
  boost::uint32_t needed = sizeof( double ) * n;

  if ( !this->ResetBuffer( source, pos, needed ) )
    {
    return pos;
    }
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->readDouble( values[ i ] );
    }
  return pos + needed;
}

// BinaryDeserializerNative
BinaryDeserializerNative::BinaryDeserializerNative( )
{
}

BinaryDeserializerNative::~BinaryDeserializerNative( )
{
}

boost::uint32_t BinaryDeserializerNative::Read( const std::string &source,
                                                boost::int8_t *values, boost::uint32_t n,
                                                boost::uint32_t pos )
{
  boost::uint32_t needed;
  int step;
  if ( this->GetConvertFromHex( ) )
    {
    needed = n * 2;
    }
  else
    {
    needed = n;
    }
  if ( !this->CheckBuffer( source, pos, needed ) )
    {
    return pos;
    }
  const char *ptrValues = reinterpret_cast<const char*>( source.c_str( ) + pos );  
  char c;
  if ( this->GetConvertFromHex( ) )
    {
    for( boost::uint32_t i = 0; i < n; i += 2 )
      {
      if ( HexToChar( ptrValues + i, c ) )
        {
        values[ i ] = c;
        }
      else
        {
        return pos;
        }
      }
    }
  else
    {
    for( boost::uint32_t i = 0; i < n; ++i )
      {
      values[ i ] = ptrValues[ i ];
      }
    }
  return pos + needed;
}

boost::uint32_t BinaryDeserializerNative::Read( const std::string &source,
                                                std::string &str,
                                                boost::uint32_t pos )
{
  boost::uint32_t minLength;
  if ( this->GetConvertFromHex( ) )
    {
    minLength = sizeof( boost::uint32_t ) * 2;
    }
  else
    {
    minLength = sizeof( boost::uint32_t );
    }
  if ( !this->CheckBuffer( source, pos, minLength ) )
    {
    return pos;
    }
  // read string length
  boost::uint32_t lengthStr;
  if ( this->GetConvertFromHex( ) )
    {
    if ( !GID::HexToBin( source.c_str( ) + pos, lengthStr ) )
      {
      return pos;
      }
    }
  else
    {
    const char *addr = source.c_str( ) + pos;
    lengthStr = this->endian( *( reinterpret_cast<const boost::uint32_t*>( addr ) ) );
    }
  const boost::uint32_t pos1 = pos + minLength;
  if ( !this->CheckBuffer( source, pos1, lengthStr ) )
    {
    return pos;
    }
  if ( lengthStr )
    {
    str.append( source.c_str( ) + pos1, lengthStr );
    }
  return pos1 + lengthStr;
}

boost::uint32_t BinaryDeserializerNative::Read( const std::string &source,
                                          boost::int16_t *values, boost::uint32_t n,
                                          boost::uint32_t pos )
{
  const size_t sizeValue = sizeof( boost::int16_t );
  boost::uint32_t needed = sizeValue * n;

  if ( this->GetConvertFromHex( ) )
    {
    needed <<= 1;
    }
  if ( !this->CheckBuffer( source, pos, needed ) )
    {
    return pos;
    }
  if ( this->GetConvertFromHex( ) )
    {
    const char *ptrHex = reinterpret_cast<const char*>( source.c_str( ) + pos );
    for( boost::uint32_t i = 0; i < n; i += sizeValue )
      {
      if ( !GID::HexToBin( ptrHex + i, values[ i ] ) )
        {
        return pos;
        }
      }
    }
  else
    {
    const boost::int16_t *ptrValues = reinterpret_cast<const boost::int16_t*>( source.c_str( ) + pos );
    for( boost::uint32_t i = 0; i < n; i++ )
      {
      values[ i ] = this->endian( ptrValues[ i ] );
      }
    }
  return pos + needed;
}

boost::uint32_t BinaryDeserializerNative::Read( const std::string &source,
                                          boost::int32_t *values, boost::uint32_t n,
                                          boost::uint32_t pos )
{
  const size_t sizeValue = sizeof( boost::int32_t );
  boost::uint32_t needed = sizeValue * n;

  if ( this->GetConvertFromHex( ) )
    {
    needed <<= 1;
    }
  if ( !this->CheckBuffer( source, pos, needed ) )
    {
    return pos;
    }
  if ( this->GetConvertFromHex( ) )
    {
    const char *ptrHex = reinterpret_cast<const char*>( source.c_str( ) + pos );
    for( boost::uint32_t i = 0; i < n; i += sizeValue )
      {
      if ( !GID::HexToBin( ptrHex + i, values[ i ] ) )
        {
        return pos;
        }
      }
    }
  else
    {
    const boost::int32_t *ptrValues = reinterpret_cast<const boost::int32_t*>( source.c_str( ) + pos );
    for( boost::uint32_t i = 0; i < n; i++ )
      {
      values[ i ] = this->endian( ptrValues[ i ] );
      }
    }
  return pos + needed;
}

boost::uint32_t BinaryDeserializerNative::Read( const std::string &source,
                                          boost::int64_t *values, boost::uint32_t n,
                                          boost::uint32_t pos )
{
  const size_t sizeValue = sizeof( boost::int64_t );
  boost::uint32_t needed = sizeValue * n;

  if ( this->GetConvertFromHex( ) )
    {
    needed <<= 1;
    }
  if ( !this->CheckBuffer( source, pos, needed ) )
    {
    return pos;
    }
  if ( this->GetConvertFromHex( ) )
    {
    const char *ptrHex = reinterpret_cast<const char*>( source.c_str( ) + pos );
    for( boost::uint32_t i = 0; i < n; i += sizeValue )
      {
      if ( !GID::HexToBin( ptrHex + i, values[ i ] ) )
        {
        return pos;
        }
      }
    }
  else
    {
    const boost::int64_t *ptrValues = reinterpret_cast<const boost::int64_t*>( source.c_str( ) + pos );
    for( boost::uint32_t i = 0; i < n; i++ )
      {
      values[ i ] = this->endian( ptrValues[ i ] );
      }
    }
  return pos + needed;
}

boost::uint32_t BinaryDeserializerNative::Read( const std::string &source,
                                                double *values, boost::uint32_t n,
                                                boost::uint32_t pos )
{
  const size_t sizeValue = sizeof( double );
  boost::uint32_t needed = sizeValue * n;

  if ( this->GetConvertFromHex( ) )
    {
    needed <<= 1;
    }
  if ( !this->CheckBuffer( source, pos, needed ) )
    {
    return pos;
    }
  if ( this->GetConvertFromHex( ) )
    {
    const char *ptrHex = reinterpret_cast<const char*>( source.c_str( ) + pos );
    for( boost::uint32_t i = 0; i < n; i += sizeValue )
      {
      if ( !GID::HexToBin( ptrHex + i, values[ i ] ) )
        {
        return pos;
        }
      }
    }
  else
    {
    const double *ptrValues = reinterpret_cast<const double*>( source.c_str( ) + pos );
    for( boost::uint32_t i = 0; i < n; i++ )
      {
      values[ i ] = this->endian( ptrValues[ i ] );
      }
    }
  return pos + needed;
}

END_GID_DECLS
