#include "BinarySerialization.h"

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;

BEGIN_GID_DECLS

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

boost::uint32_t BinarySerializer::Write( std::string &dest, const std::string &str )
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

BinaryDeserializer::BinaryDeserializer( )
  : m_Transport( new TMemoryBuffer( ) ), 
    m_Protocol( new TBinaryProtocol( this->m_Transport ) )
{
}

BinaryDeserializer::~BinaryDeserializer( )
{
}

boost::uint32_t BinaryDeserializer::ResetBuffer( const std::string &source, boost::uint32_t pos,
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

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
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

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
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

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
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

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
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

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
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

boost::uint32_t BinaryDeserializer::Read( const std::string &source,
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

END_GID_DECLS
