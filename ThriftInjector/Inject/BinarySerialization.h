#if !defined(__BinarySerialization_h__)
#define __BinarySerialization_h__

#include "GidTypes.h"
#include <boost/shared_ptr.hpp>
#include "thrift/transport/TBufferTransports.h"
#include "thrift/protocol/TBinaryProtocol.h"

BEGIN_GID_DECLS

enum Endianness { LittleEndian, BigEndian, HostEndian };

template <typename T>
T bswap( T w )
{
  T r = 0;
  
  // decent compilers will unroll this (gcc)
  // or even convert straight into single bswap (clang)
  for (int i = 0; i < sizeof(r); i++) 
    {
    r <<= 8;
    r |= w & 0xff;
    w >>= 8;
    }
  return r;
}

double bswap( double w );

template <typename T>
T endian(T w, Endianness endian )
{
  if ( endian == HostEndian ) return w;
  // this gets optimized out into if (endian == host_endian) return w;
  union { uint64_t quad; uint32_t islittle; } t;
  t.quad = 1;
  if (t.islittle ^ uint32_t(endian) ) return w;
  return bswap( w );

  /*
  T r = 0;
  
  // decent compilers will unroll this (gcc)
  // or even convert straight into single bswap (clang)
  for (int i = 0; i < sizeof(r); i++) {
  r <<= 8;
  r |= w & 0xff;
  w >>= 8;
  }
  return r;
  */
};

class BinarySerializer
{
public:
  BinarySerializer( ) : m_ConvertToHex( false ), m_Endianness( HostEndian ) { };
  virtual ~BinarySerializer( ) { };
  
  void SetConvertToHex( bool x )
  {
    this->m_ConvertToHex = x;
  }

  bool GetConvertToHex()
  {
    return this->m_ConvertToHex;
  }

  static std::string BinToHex( const std::string &source );
  static void BinToHex( char c, char buffer[2] );

  void SetEndianness( Endianness t )
  {
    this->m_Endianness = t;
  }

  Endianness GetEndianness( )
  {
    return this->m_Endianness;
  }

  bool IsEndiannessNative();

  // int8_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int8_t *values, boost::uint32_t n ) = 0;
  boost::uint32_t Write( std::string &dest, const boost::int8_t value )
  {
    return this->Write( dest, &value, 1 );
  }
  boost::uint32_t Write( std::string &dest, const boost::uint8_t *values, boost::uint32_t n )
  {
    return this->Write( dest, reinterpret_cast<const boost::int8_t *>( values ), n );
  }
  boost::uint32_t Write( std::string &dest, boost::uint8_t value )
  {
    return this->Write( dest, &value, 1 );
  }
  boost::uint32_t Write( std::string &dest, const char *values, boost::uint32_t n )
  {
    return this->Write( dest, reinterpret_cast<const boost::int8_t *>( values ), n );
  }
  boost::uint32_t Write( std::string &dest, char value )
  {
    return this->Write( dest, &value, 1 );
  }
  virtual boost::uint32_t Write( std::string &dest, const std::string &str ) = 0;
  
  // int16_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int16_t *values, boost::uint32_t n ) = 0;
  boost::uint32_t Write( std::string &dest, const boost::int16_t value )
  {
    return this->Write( dest, &value, 1 );
  }
  boost::uint32_t Write( std::string &dest, const boost::uint16_t *values, boost::uint32_t n )
  {
    return this->Write( dest, reinterpret_cast<const boost::int16_t *>( values ), n );
  }
  boost::uint32_t Write( std::string &dest, const boost::uint16_t value )
  {
    return this->Write( dest, &value, 1 );
  }
  
  // int32_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int32_t *values, boost::uint32_t n ) = 0;
  boost::uint32_t Write( std::string &dest, const boost::int32_t value )
  {
    return this->Write( dest, &value, 1 );
  }
  boost::uint32_t Write( std::string &dest, const boost::uint32_t *values, boost::uint32_t n )
  {
    return this->Write( dest, reinterpret_cast<const boost::int32_t *>( values ), n );
  }
  boost::uint32_t Write( std::string &dest, const boost::uint32_t value )
  {
    return this->Write( dest, &value, 1 );
  }

  // int64_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int64_t *values, boost::uint32_t n ) = 0;
  boost::uint32_t Write( std::string &dest, const boost::int64_t value )
  {
    return this->Write( dest, &value, 1 );
  }
  boost::uint32_t Write( std::string &dest, const boost::uint64_t *values, boost::uint32_t n )
  {
    return this->Write( dest, reinterpret_cast<const boost::int64_t *>( values ), n );
  }
  boost::uint32_t Write( std::string &dest, const boost::uint64_t value )
  {
    return this->Write( dest, &value, 1 );
  }

  // double
  virtual boost::uint32_t Write( std::string &dest, const double *values, boost::uint32_t n ) = 0;
  boost::uint32_t Write( std::string &dest, const double &value )
  {
    return this->Write( dest, &value, 1 );
  }

protected:
  void AppendWithEndianness( std::string & dest, const char *buffer, int length );

 private:
  bool m_ConvertToHex;
  Endianness m_Endianness;
};

class BinarySerializerThrift : public BinarySerializer
{
public:
  BinarySerializerThrift( );
  ~BinarySerializerThrift( );

  using BinarySerializer::Write;

  // int8_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int8_t *values, boost::uint32_t n );
  virtual boost::uint32_t Write( std::string &dest, const std::string &str );
  
  // int16_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int16_t *values, boost::uint32_t n );
  
  // int32_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int32_t *values, boost::uint32_t n );

  // int64_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int64_t *values, boost::uint32_t n );

  // double
  virtual boost::uint32_t Write( std::string &dest, const double *values, boost::uint32_t n );
  
private:
  void MakeRoom( boost::uint32_t size );
  boost::shared_ptr<apache::thrift::transport::TMemoryBuffer> m_Transport;
  boost::shared_ptr<apache::thrift::protocol::TBinaryProtocol> m_Protocol;
};

class BinarySerializerNative : public BinarySerializer
{
public:
  BinarySerializerNative( );
  ~BinarySerializerNative( );

  using BinarySerializer::Write;

  // int8_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int8_t *values, boost::uint32_t n );
  virtual boost::uint32_t Write( std::string &dest, const std::string &str );
  
  // int16_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int16_t *values, boost::uint32_t n );
  
  // int32_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int32_t *values, boost::uint32_t n );

  // int64_t
  virtual boost::uint32_t Write( std::string &dest, const boost::int64_t *values, boost::uint32_t n );

  // double
  virtual boost::uint32_t Write( std::string &dest, const double *values, boost::uint32_t n );
};

class BinaryDeserializer
{
public:
  BinaryDeserializer( )  : m_ConvertFromHex( false ), m_Endianness( HostEndian ) { };
  ~BinaryDeserializer( ) { };

  void SetConvertFromHex( bool x )
  {
    this->m_ConvertFromHex = x;
  }

  bool GetConvertFromHex()
  {
    return this->m_ConvertFromHex;
  }

  void SetEndianness( Endianness t )
  {
    this->m_Endianness = t;
  }

  Endianness GetEndianness()
  {
    return this->m_Endianness;
  }

  bool IsEndiannessNative();

  // int8_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int8_t *values, boost::uint32_t n,
				boost::uint32_t pos = 0 ) = 0;
  boost::uint32_t Read( const std::string &source,
                        boost::uint8_t *values, boost::uint32_t n,
			boost::uint32_t pos = 0 )
  {
    return this->Read( source, reinterpret_cast<boost::int8_t *>( values ), n, pos );
  }
  virtual boost::uint32_t Read( const std::string &source,
				std::string &str,
				boost::uint32_t pos = 0 ) = 0;

  // int16_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int16_t *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 ) = 0;
  boost::uint32_t Read( const std::string &source,
                        boost::uint16_t *values, boost::uint32_t n, 
			boost::uint32_t pos = 0 )
  {
    return this->Read( source, reinterpret_cast<boost::int16_t *>( values ), n, pos );
  }

  // int32_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int32_t *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 ) = 0;
  boost::uint32_t Read( const std::string &source,
                        boost::uint32_t *values, boost::uint32_t n, 
			boost::uint32_t pos = 0 )
  {
    return this->Read( source, reinterpret_cast<boost::int32_t *>( values ), n, pos );
  }

  // int64_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int64_t *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 ) = 0;
  boost::uint32_t Read( const std::string &source,
                        boost::uint64_t *values, boost::uint32_t n,  
			boost::uint32_t pos = 0 )
  {
    return this->Read( source, reinterpret_cast<boost::int64_t *>( values ), n, pos );
  }

  // double
  virtual boost::uint32_t Read( const std::string &source,
				double *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 ) = 0;

protected:

  template <typename T>
  T endian( T w )
  {
    return GID::endian( w, this->GetEndianness( ) );
  }

private:
  bool m_ConvertFromHex;
  Endianness m_Endianness;
};

class BinaryDeserializerThrift : public BinaryDeserializer
{
public:

  using BinaryDeserializer::Read;

  BinaryDeserializerThrift( );
  ~BinaryDeserializerThrift( );

  // int8_t
  virtual boost::uint32_t Read( const std::string &source,
			       boost::int8_t *values, boost::uint32_t n,
			       boost::uint32_t pos = 0 );
  virtual boost::uint32_t Read( const std::string &source,
				std::string &str,
				boost::uint32_t pos = 0  );

  // int16_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int16_t *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 );


  // int32_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int32_t *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 );

  // int64_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int64_t *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 );

  // double
  virtual boost::uint32_t Read( const std::string &source,
				double *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 );
private:
  boost::uint32_t ResetBuffer( const std::string &source, boost::uint32_t pos,
			       boost::uint32_t needed );

  boost::shared_ptr<apache::thrift::transport::TMemoryBuffer> m_Transport;
  boost::shared_ptr<apache::thrift::protocol::TBinaryProtocol> m_Protocol;
};

class BinaryDeserializerNative : public BinaryDeserializer
{
public:

  using BinaryDeserializer::Read;

  BinaryDeserializerNative( );
  ~BinaryDeserializerNative( );

  // int8_t
  virtual boost::uint32_t Read( const std::string &source,
			       boost::int8_t *values, boost::uint32_t n,
			       boost::uint32_t pos = 0 );
  virtual boost::uint32_t Read( const std::string &source,
				std::string &str,
				boost::uint32_t pos = 0  );

  // int16_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int16_t *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 );


  // int32_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int32_t *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 );

  // int64_t
  virtual boost::uint32_t Read( const std::string &source,
				boost::int64_t *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 );

  // double
  virtual boost::uint32_t Read( const std::string &source,
				double *values, boost::uint32_t n, 
				boost::uint32_t pos = 0 );
private:
  bool CheckBuffer( const std::string &source, boost::uint32_t pos,
		    boost::uint32_t needed )
  {
    boost::uint32_t available = source.size( ) - pos;
    if ( pos < 0 || pos >= source.size( ) || needed > available )
      {
	return false;
      }
    return true;
  }
};

END_GID_DECLS

#endif
