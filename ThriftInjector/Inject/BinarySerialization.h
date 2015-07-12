#if !defined(__BinarySerialization_h__)
#define __BinarySerialization_h__

#include "GidTypes.h"
#include <boost/shared_ptr.hpp>
#include "thrift/transport/TBufferTransports.h"
#include "thrift/protocol/TBinaryProtocol.h"

BEGIN_GID_DECLS

class BinarySerializer
{
public:
  BinarySerializer( );
  ~BinarySerializer( );

  // int8_t
  boost::uint32_t Write( std::string &dest, const boost::int8_t *values, boost::uint32_t n );
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
  boost::uint32_t Write( std::string &dest, const std::string &str );
  
  // int16_t
  boost::uint32_t Write( std::string &dest, const boost::int16_t *values, boost::uint32_t n );
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
  boost::uint32_t Write( std::string &dest, const boost::int32_t *values, boost::uint32_t n );
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
  boost::uint32_t Write( std::string &dest, const boost::int64_t *values, boost::uint32_t n );
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
  boost::uint32_t Write( std::string &dest, const double *values, boost::uint32_t n );
  boost::uint32_t Write( std::string &dest, const double value )
  {
    return this->Write( dest, &value, 1 );
  }
  
private:
  void MakeRoom( boost::uint32_t size );
  boost::shared_ptr<apache::thrift::transport::TMemoryBuffer> m_Transport;
  boost::shared_ptr<apache::thrift::protocol::TBinaryProtocol> m_Protocol;
};

class BinaryDeserializer
{
public:
  BinaryDeserializer( );
  ~BinaryDeserializer( );

  // int8_t
  boost::uint32_t Read( const std::string &source,
                        boost::int8_t *values, boost::uint32_t n,
			boost::uint32_t pos = 0 );
  boost::uint32_t Read( const std::string &source,
                        boost::uint8_t *values, boost::uint32_t n,
			boost::uint32_t pos = 0 )
  {
    return this->Read( source, reinterpret_cast<boost::int8_t *>( values ), n, pos );
  }
  boost::uint32_t Read( const std::string &source,
                        std::string &str,
			boost::uint32_t pos = 0  );

  // int16_t
  boost::uint32_t Read( const std::string &source,
                        boost::int16_t *values, boost::uint32_t n, 
			boost::uint32_t pos = 0 );
  boost::uint32_t Read( const std::string &source,
                        boost::uint16_t *values, boost::uint32_t n, 
			boost::uint32_t pos = 0 )
  {
    return this->Read( source, reinterpret_cast<boost::int16_t *>( values ), n, pos );
  }


  // int32_t
  boost::uint32_t Read( const std::string &source,
                        boost::int32_t *values, boost::uint32_t n, 
			boost::uint32_t pos = 0 );
  boost::uint32_t Read( const std::string &source,
                        boost::uint32_t *values, boost::uint32_t n, 
			boost::uint32_t pos = 0 )
  {
    return this->Read( source, reinterpret_cast<boost::int32_t *>( values ), n, pos );
  }

  // int64_t
  boost::uint32_t Read( const std::string &source,
                        boost::int64_t *values, boost::uint32_t n, 
			boost::uint32_t pos = 0 );
  boost::uint32_t Read( const std::string &source,
                        boost::uint64_t *values, boost::uint32_t n,  
			boost::uint32_t pos = 0 )
  {
    return this->Read( source, reinterpret_cast<boost::int64_t *>( values ), n, pos );
  }

  // double
  boost::uint32_t Read( const std::string &source,
                        double *values, boost::uint32_t n, 
			boost::uint32_t pos = 0 );
private:
  boost::uint32_t ResetBuffer( const std::string &source, boost::uint32_t pos,
			       boost::uint32_t needed );

  boost::shared_ptr<apache::thrift::transport::TMemoryBuffer> m_Transport;
  boost::shared_ptr<apache::thrift::protocol::TBinaryProtocol> m_Protocol;
};

END_GID_DECLS

#endif
