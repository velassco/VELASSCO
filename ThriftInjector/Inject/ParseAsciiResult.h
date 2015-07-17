#if !defined(__ParseAsciiResult_h__)
#define __ParseAsciiResult_h__

#include "GidTypes.h"

#include <vector>
#include <map>
#include <string>

BEGIN_GID_DECLS

struct ResultHeaderType
{
  std::string name;
  std::string analysis;
  double step;
  ValueType rType;
  LocationType location;
  std::string gpName;
  std::vector<std::string> compName;
  UInt64 indexMData;

  ResultHeaderType()
  {
    step = 0.0;
    indexMData = 0;
  }

  std::string GetAsString( );

  UInt32 GetNumberOfComponents( ) const
  {
    if ( this->rType == VECTOR )
      {
	return this->compName.size( );
      }
    else
      {
	return GetValueTypeSize( this->rType );
      }
  }
};

struct GaussPointType
{
  std::string name;
};

struct RangeTableType
{
  std::string name;
};

struct ResultRowType
{
  UInt64 id;
  std::vector<double> values;

  void setId( UInt64 const &i ) { this->id = i; }
  void appendValue( double const &v ) { this->values.push_back( v ); }
};

struct ResultBlockType
{
  ResultHeaderType header;
  std::vector<ResultRowType> values;

  void setName( std::string const &n ) { this->header.name = n; }
  void setAnalysis( std::string const &a ) { this->header.analysis = a; }
  void setStep( double const &s ) { this->header.step = s; }
  void setResultType( ValueType const &t ) { this->header.rType = t; }
  void setLocation( LocationType const &l ) { this->header.location = l; }
  LocationType getLocation( ) const { return this->header.location; }
  void setGPName( std::string const &n ) { this->header.gpName = n; }
  void appendCompName( std::string const&n ) { this->header.compName.push_back( n ); } 
};

struct ResultContainerType
{
  unsigned int majorVersion;
  unsigned int minorVersion;
  std::vector<GaussPointType> gaussPoints;
  std::vector<RangeTableType> rangeTables;
  std::vector<ResultBlockType> results;
  void setMajorVersion( unsigned int const &v) { this->majorVersion = v; }
  void setMinorVersion( unsigned int const &v) { this->minorVersion = v; }
};

int ParseResultFile( const std::string& pathFile, ResultContainerType &result );

END_GID_DECLS

#endif
