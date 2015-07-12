#include "boost/cstdint.hpp"
#include <boost/shared_ptr.hpp>
#include "thrift/transport/TBufferTransports.h"
#include "thrift/protocol/TBinaryProtocol.h"

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;

class BinarySerializer
{
public:
  BinarySerializer( );
  ~BinarySerializer( );
  boost::uint32_t Write( std::string &dest,
                         const boost::int8_t *values, boost::uint32_t n );
  boost::uint32_t Write( std::string &dest,
                         const boost::uint8_t *values, boost::uint32_t n );
  boost::uint32_t Write( std::string &dest,
                         const boost::int16_t *values, boost::uint32_t n );
  boost::uint32_t Write( std::string &dest,
                         const boost::uint16_t *values, boost::uint32_t n );
  boost::uint32_t Write( std::string &dest,
                         const boost::int32_t *values, boost::uint32_t n );
  boost::uint32_t Write( std::string &dest,
                         const boost::uint32_t *values, boost::uint32_t n );
  boost::uint32_t Write( std::string &dest,
                         const boost::int64_t *values, boost::uint32_t n );
  boost::uint32_t Write( std::string &dest,
                         const boost::uint64_t *values, boost::uint32_t n );
private:
  void MakeRoom( boost::uint32_t size );
  boost::shared_ptr<TMemoryBuffer> m_Transport;
  boost::shared_ptr<TBinaryProtocol> m_Protocol;
};

BinarySerializer::BinarySerializer( )
  : m_Transport( new TMemoryBuffer( ) ), 
    m_Protocol( new TBinaryProtocol( this->m_Transport ) )
{
}

BinarySerializer::~BinarySerializer( )
{
}

void BinarySerializer::MakeRoom( boost::uint32_t size )
{
  this->m_Transport->resetBuffer( );
  if ( this->m_Transport->available_write( ) < size )
    {
    this->m_Transport->resetBuffer( size );
    }
}

boost::uint32_t BinarySerializer::Write( std::string &dest,
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

boost::uint32_t BinarySerializer::Write( std::string &dest,
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

boost::uint32_t BinarySerializer::Write( std::string &dest,
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

boost::uint32_t BinarySerializer::Write( std::string &dest,
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

boost::uint32_t BinarySerializer::Write( std::string &dest,
                                         const boost::uint8_t *values, boost::uint32_t n )
{
  return this->Write( dest, reinterpret_cast<const boost::int8_t *>( values ), n );
}

boost::uint32_t BinarySerializer::Write( std::string &dest,
                                         const boost::uint16_t *values, boost::uint32_t n )
{
  return this->Write( dest, reinterpret_cast<const boost::int16_t *>( values ), n );
}

boost::uint32_t BinarySerializer::Write( std::string &dest,
                                         const boost::uint32_t *values, boost::uint32_t n )
{
  return this->Write( dest, reinterpret_cast<const boost::int32_t *>( values ), n );
}

boost::uint32_t BinarySerializer::Write( std::string &dest,
                                         const boost::uint64_t *values, boost::uint32_t n )
{
  return this->Write( dest, reinterpret_cast<const boost::int64_t *>( values ), n );
}

class BinaryDeserializer
{
public:
  BinaryDeserializer( );
  ~BinaryDeserializer( );
  boost::uint32_t Read( const std::string &source,
                        boost::int8_t *values, boost::uint32_t n );
  boost::uint32_t Read( const std::string &source,
                        boost::uint8_t *values, boost::uint32_t n );
  boost::uint32_t Read( const std::string &source,
                        boost::int16_t *values, boost::uint32_t n );
  boost::uint32_t Read( const std::string &source,
                        boost::uint16_t *values, boost::uint32_t n );
  boost::uint32_t Read( const std::string &source,
                        boost::int32_t *values, boost::uint32_t n );
  boost::uint32_t Read( const std::string &source,
                        boost::uint32_t *values, boost::uint32_t n );
  boost::uint32_t Read( const std::string &source,
                        boost::int64_t *values, boost::uint32_t n );
  boost::uint32_t Read( const std::string &source,
                        boost::uint64_t *values, boost::uint32_t n );
private:
  boost::shared_ptr<TMemoryBuffer> m_Transport;
  boost::shared_ptr<TBinaryProtocol> m_Protocol;
};

BinaryDeserializer::BinaryDeserializer( )
  : m_Transport( new TMemoryBuffer( ) ), 
    m_Protocol( new TBinaryProtocol( this->m_Transport ) )
{
}

BinaryDeserializer::~BinaryDeserializer( )
{
}

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
                                          boost::int8_t *values, boost::uint32_t n )
{
  if ( source.length( ) < n )
    {
    return 0;
    }
  this->m_Transport->resetBuffer( (uint8_t*)(source.c_str()), source.length( ) );
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->readByte( values[ i ] );
    }
  return n;
}

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
                                          boost::int16_t *values, boost::uint32_t n )
{
  boost::uint32_t needed = sizeof( boost::int16_t ) * n;
  if ( source.length( ) < needed )
    {
    return 0;
    }
  this->m_Transport->resetBuffer( (uint8_t*)(source.c_str()), source.length( ) );
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->readI16( values[ i ] );
    }
  return needed;
}

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
                                          boost::int32_t *values, boost::uint32_t n )
{
  boost::uint32_t needed = sizeof( boost::int32_t ) * n;
  if ( source.length( ) < needed )
    {
    return 0;
    }
  this->m_Transport->resetBuffer( (uint8_t*)(source.c_str()), source.length( ) );
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->readI32( values[ i ] );
    }
  return needed;
}

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
                                          boost::int64_t *values, boost::uint32_t n )
{
  boost::uint32_t needed = sizeof( boost::int64_t ) * n;
  if ( source.length( ) < needed )
    {
    return 0;
    }
  this->m_Transport->resetBuffer( (uint8_t*)(source.c_str()), source.length( ) );
  for( boost::uint32_t i = 0; i < n; i++ )
    {
    this->m_Protocol->readI64( values[ i ] );
    }
  return needed;
}

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
                                          boost::uint8_t *values, boost::uint32_t n )
{
  return this->Read( source, reinterpret_cast<boost::int8_t *>( values ), n );
}

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
                                          boost::uint16_t *values, boost::uint32_t n )
{
  return this->Read( source, reinterpret_cast<boost::int16_t *>( values ), n );
}

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
                                          boost::uint32_t *values, boost::uint32_t n )
{
  return this->Read( source, reinterpret_cast<boost::int32_t *>( values ), n );
}

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
                                          boost::uint64_t *values, boost::uint32_t n )
{
  return this->Read( source, reinterpret_cast<boost::int64_t *>( values ), n );
}

int main( int argc, char *argv[] )
{
  //boost::shared_ptr<TMemoryBuffer> transportOut(new TMemoryBuffer( ) );
  //boost::shared_ptr<TBinaryProtocol> protocolOut( new TBinaryProtocol( transportOut ) );

  BinarySerializer binBuffer;

  int32_t i32Value = 1000;
  //protocolOut->writeI32( i32Value );
  std::string serialized_string;
  binBuffer.Write( serialized_string, &i32Value, 1 );

  //std::string serialized_string = transportOut->getBufferAsString();
  
  std::cout << "length( serialized_string ) = " << serialized_string.length( ) << std::endl;

  int32_t i32ValueCheck;
  //boost::shared_ptr<TMemoryBuffer> transportInput(new TMemoryBuffer((uint8_t*)(serialized_string.c_str()), serialized_string.length( ) ));
  //boost::shared_ptr<TBinaryProtocol> protocolInput(new TBinaryProtocol(transportInput));
  //protocolInput->readI32( i32ValueCheck );
  BinaryDeserializer binBufferInput;
  binBufferInput.Read( serialized_string, &i32ValueCheck, 1 );
 
  std::cout << "i32ValueCheck = " << i32ValueCheck << std::endl;
  return 0;
}
