#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include "boost/program_options.hpp" 

#include "Logger.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "Hbase.h"

#include "ParseAsciiMesh.h"
#include "ParseAsciiResult.h"
#include "BinarySerialization.h"

//#define DONT_APPLY_MUTATIONS
#define CHECK_KEY_ENCODING
//#define USE_THRIFT_SERIALIZATION
//#define USE_BINARY_ROWKEY

#if defined( USE_BINARY_ROWKEY )
#define MODEL_ROWKEY_LENGTH 16
#else
#define MODEL_ROWKEY_LENGTH 32
#endif

using namespace boost::filesystem;
using boost::lexical_cast;
using boost::bad_lexical_cast;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace apache::hadoop::hbase::thrift;

typedef std::vector<std::string> StrVec;
typedef std::map<std::string,std::string> StrMap;
typedef std::vector<ColumnDescriptor> ColVec;
typedef std::map<std::string,ColumnDescriptor> ColMap;
typedef std::vector<TCell> CellVec;
typedef std::map<std::string,TCell> CellMap;

typedef std::map<int, path> PathContainerType;

#if defined(USE_THRIFT_SERIALIZATION)
typedef  GID::BinarySerializerThrift BinarySerializerType;
typedef  GID::BinaryDeserializerThrift BinaryDeserializerType;
#else
typedef  GID::BinarySerializerNative BinarySerializerType;
typedef  GID::BinaryDeserializerNative BinaryDeserializerType;
#endif

struct ModelFileParts
{
  path m_PathRoot;
  path::string_type m_ModelName;
  PathContainerType m_Parts;

  int GetNumberOfParts( ) const
  {
    return this->m_Parts.size( );
  }
};

namespace 
{ 
const int SUCCESS                              = 0; 
const int ERROR_IN_COMMAND_LINE                = -1; 
const int ERROR_EXCEPTION                      = -2; 
const int ERROR_UNHANDLED_EXCEPTION            = -3;
const int ERROR_TABLE_DATA_NOTFOUND            = -4;
const int ERROR_TABLE_METADATA_NOTFOUND        = -5;
const int ERROR_TABLE_MODELS_NOTFOUND          = -6;
const int ERROR_PARSING_MESH                   = -7;
const int ERROR_PARSING_RESULT                 = -8;
const int ERROR_BAD_UUID_LENGTH                = -9;
const int ERROR_KEY_MINIMUM_LENGTH             = -10;
const int ERROR_KEY_BAD_READ                   = -11;
const int ERROR_INVALID_COLUMN_NAME_LENGHT     = -12;
const int ERROR_INVALID_COLUMN_NAME_PREFIX     = -13;
const int ERROR_INVALID_COLUMN_NAME_CSET       = -14;
const int ERROR_INVALID_COLUMN_NAME_ID         = -15;
} // namespace

struct GlobalMeshHeaderType
{
  GID::ElementType elementType;
  GID::UInt32 sizeElement;
  GID::IdNode numberOfNodes;
  GID::IdElement numberOfElements;
  std::string color;
  GID::UInt32 indexElementSet;

  GlobalMeshHeaderType()
  {
    this->elementType = GID::TETRAHEDRA;
    this->sizeElement = 0;
    this->numberOfNodes = 0;
    this->numberOfElements = 0;
    this->indexElementSet = 0;
  }

  int InitFromPartition( const GID::MeshResultType &meshPart, 
                         GID::UInt32 index )
  {
    if ( this->indexElementSet == 0 )
      {
      this->indexElementSet = index;
      this->elementType = meshPart.header.elementType;
      this->sizeElement = meshPart.header.numberOfNodes;
      }
    else
      {
      // TODO: check if meshPart matches first partition header
      }
    this->numberOfNodes += meshPart.nodes.size();
    this->numberOfElements += meshPart.elements.size();
    return SUCCESS;
  }
};

struct GlobalMeshInfoType
{
  typedef std::map<std::string, GlobalMeshHeaderType> MapHeaderType;

  MapHeaderType headers;

  int Update( const GID::MeshResultType &meshPart )
  {
    GlobalMeshHeaderType &meshInfo = this->headers[ meshPart.header.name ];
    
    return meshInfo.InitFromPartition( meshPart, this->headers.size( ) );
  }

  GID::UInt32 GetIndexElementSet( const std::string name )
  {
    MapHeaderType::const_iterator it = this->headers.find( name );
    
    if ( it != this->headers.end( ) )
      {
      return it->second.indexElementSet;
      }
    else
      {
      return 0;
      }
  }
};

struct GlobalResultInfoType
{
  // TODO: collect also GP & RT

  typedef std::map<std::string, GID::ResultHeaderType> MapHeaderType;
  typedef std::map<double, MapHeaderType> MapTimeStepType;
  typedef std::map<std::string, MapTimeStepType> MapAnalysisType;

  MapAnalysisType analysis;

  std::map<std::string, GID::GaussPointDefinition> gaussPoints;

  int Update( GID::ResultHeaderType & h )
  {
    MapTimeStepType &mapT = this->analysis[ h.analysis ];
    MapHeaderType &headers = mapT[ h.step ];
    GID::ResultHeaderType &header = headers[ h.name ];
    if( header.indexMData == 0 )
      {
      header = h;
      header.indexMData = headers.size( );
      }
    else
      {
      // TODO: check if resultPart matches first partition header
      }
    h.indexMData = header.indexMData;
    return SUCCESS;
  }

  // TODO: update GP & RT
  int Update( GID::ResultContainerType &resultPart )
  {
    int status = SUCCESS;
    for( size_t i = 0; i < resultPart.results.size(); i++ )
      {
      status = this->Update( resultPart.results[i].header );
      if( status != SUCCESS )
        {
        break;
        }
      }
    for( std::map<std::string, GID::GaussPointDefinition>::const_iterator it = resultPart.gaussPoints.begin( );
         it != resultPart.gaussPoints.end( ); ++it )
      {
      if ( this->gaussPoints.find( it->first ) == this->gaussPoints.end( ) )
        {
        this->gaussPoints[ it->first ] = it->second;
        }
      }
    return status;
  }

  GID::UInt64 GetIndexMetaData( const std::string analysis, double step,
                                const std::string &name )
  {
    MapAnalysisType::const_iterator itA = this->analysis.find( analysis );
    if ( itA == this->analysis.end( ) )
      {
      return 0;
      }
    MapTimeStepType::const_iterator itT = itA->second.find( step );
    if ( itT == itA->second.end( ) )
      {
      return 0;
      }
    MapHeaderType::const_iterator itH = itT->second.find( name );
    if ( itH != itT->second.end( ) )
      {
      return itH->second.indexMData;
      }
    else
      {
      return 0;
      }
  }
};

namespace po = boost::program_options;

static std::string strTableModels("VELaSSCo_Models");
static std::string strTableMetaData("Simulations_Metadata");
static std::string strTableData("Simulations_Data");

inline
std::string formatIndex( boost::uint32_t idx )
{
  std::ostringstream ss;
  ss << std::setfill( '0' ) << std::setw(  6) << idx;
  return ss.str( );
}

// TODO: use boost::lexical_cast
bool is_number(const path::string_type& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void StringToUUID( const std::string &bytes, boost::uuids::uuid &id )
{
  if ( bytes.length() != 16 )
    {
    id = boost::uuids::nil_uuid();
    }
  else
    {
    memcpy( &id, bytes.c_str(), 16 );
    }
}

int collect_sequence( const path::string_type &one_file, bool isbinary,
                      ModelFileParts &res_files,  ModelFileParts &msh_files )
{
  path::string_type name( one_file.begin(), one_file.end()-9 );
  LOG(info) << "colleting sequence for file \"" << name << "\"";
  int count = 0;
  path::string_type::const_reverse_iterator it;
  for( it = name.rbegin();
       it != name.rend() && isdigit(*it); ++it ) 
    {
    ++count;
    }
  if ( it == name.rend() && *it != '-' && *it != '_' )
    {
    LOG(error) << "the name \"" << name << "\" does not match a sequence file";
    return -1;
    }
  path::string_type prefix( name.begin( ), 
                            name.begin( ) + name.length( ) - count );
  LOG(info) << "using sequence's prefix \"" << prefix << "\"";
  path pathPrefix( prefix );
  path pathSearch( pathPrefix.parent_path() );
  path namePrefix( pathPrefix.filename( ) );
  size_t lenPrefix = namePrefix.native().length();

  LOG(info) << "Searching for prefix " << namePrefix;
  LOG(info) << "withing folder " << pathSearch;

  std::map<int,path> mapFiles;
  res_files.m_PathRoot = pathSearch;
  res_files.m_ModelName = pathPrefix.filename( ).string();
  msh_files.m_PathRoot = pathSearch;
  msh_files.m_ModelName = pathPrefix.filename( ).string();
  for ( directory_iterator it = directory_iterator( pathSearch );
        it != directory_iterator(); it++ )
    {
    path curname( it->path( ).filename( ) );
    size_t l = curname.native().length( );
    if ( l <= lenPrefix + 9 )
      {
      continue;
      } 
    path::string_type curprefix( curname.native().begin( ), 
                                 curname.native().begin( ) + lenPrefix );
    if ( curprefix != namePrefix.native() )
      {
      continue;
      }

    path::string_type strIndex( curname.native().begin( ) + lenPrefix, 
                                curname.native().end( ) - 9 );
    path::string_type::const_iterator iti;
    if ( !is_number( strIndex ) )
      {
      continue;
      }
    int idx = atoi( strIndex.c_str() );
    
    path::string_type gid_ext( curname.native().end()-9, curname.native().end() );
    
    if ( (isbinary && gid_ext == ".post.bin") || gid_ext == ".post.res" )
      {
      path tmp( pathSearch );
      tmp /= curname;
      res_files.m_Parts[idx] = tmp;
      }
    else if ( !isbinary && gid_ext == ".post.msh" )
      {
        path tmp( pathSearch );
        tmp /= curname;
        msh_files.m_Parts[idx] = tmp;
      }
    }
  return 0;
}

int DropVelasscoTables( const std::string &host, size_t port )
{
  boost::shared_ptr<TTransport> socket( new TSocket( host, port ) );
  boost::shared_ptr<TTransport> transport( new TBufferedTransport( socket ) );
  boost::shared_ptr<TProtocol> protocol( new TBinaryProtocol( transport ) );
  HbaseClient client( protocol );
  try 
    {    
    transport->open( );
    LOG(info) << "scanning tables...";
    StrVec tables;
    client.getTableNames( tables );
    for( StrVec::const_iterator it = tables.begin(); 
         it != tables.end(); ++it ) 
      {
      LOG(info) << "found table: " << *it;
      if ( strTableModels == *it || strTableMetaData == *it ||
           strTableData == *it ) 
        {
        if ( client.isTableEnabled( *it ) )
          {
          LOG(info) << "disabling table: " << *it;
          client.disableTable( *it );
          }
        LOG(info) << "deleting table: " << *it;
        client.deleteTable( *it );
        }
      }    
    transport->close( );
    }
  catch (const TException &tx) 
    {
    LOG(error) << tx.what( );
    return ERROR_EXCEPTION;
    }
  return SUCCESS;
}

int CheckVelasscoTables( const std::string &host, size_t port )
{
  boost::shared_ptr<TTransport> socket( new TSocket( host, port ) );
  boost::shared_ptr<TTransport> transport( new TBufferedTransport( socket ) );
  boost::shared_ptr<TProtocol> protocol( new TBinaryProtocol( transport ) );
  HbaseClient client( protocol );
  try 
    {    
    transport->open( );
    StrVec tables;
    client.getTableNames( tables );
    bool foundTableModels = false;
    bool foundTableMetaData = false;
    bool foundTableData = false;
    for( StrVec::const_iterator it = tables.begin(); 
         it != tables.end(); ++it ) 
      {
      if ( strTableModels == *it )
        {
        foundTableModels = true;
        }
      else if ( strTableMetaData == *it )
        {
        foundTableMetaData = true;
        }
      else if ( strTableData == *it )
        {
        foundTableData = true;
        }
      }
    if ( !foundTableData )
      {
      LOG(error) << "table not found '" << strTableData << "'";
      return ERROR_TABLE_DATA_NOTFOUND;
      }
    if ( !foundTableMetaData )
      {
      LOG(error) << "table not found '" << strTableMetaData << "'";
      return ERROR_TABLE_METADATA_NOTFOUND;
      }
    if ( !foundTableModels )
      {
      LOG(error) << "table not found '" << strTableModels << "'";
      return ERROR_TABLE_MODELS_NOTFOUND;
      }
     transport->close( );
   }
  catch (const TException &tx) 
    {
    LOG(error) << tx.what( );
    return ERROR_EXCEPTION;
    }
    return SUCCESS;
}

int CreateTable( HbaseClient &client, const std::string &tableName, 
                 std::vector<const char*> cfs )
{
  ColVec columns;
  for( int i = 0; i < cfs.size( ); i++ )
    {
    columns.push_back(ColumnDescriptor());
    columns.back().name = cfs[ i ];
    columns.back().maxVersions = 10;
    }
  LOG(info) << "creating table " << tableName;
  try 
    {
    client.createTable( tableName, columns );
    } 
  catch (const AlreadyExists &ae)
    {
    LOG(error) << ae.message;
    }
  return SUCCESS;
}

int CreateVelasscoTables( const std::string &host, size_t port )
{
  boost::shared_ptr<TTransport> socket( new TSocket( host, port ) );
  boost::shared_ptr<TTransport> transport( new TBufferedTransport( socket ) );
  boost::shared_ptr<TProtocol> protocol( new TBinaryProtocol( transport ) );
  HbaseClient client( protocol );
  try 
    {  
    transport->open( );
    //
    // Create the Models table with one column family: Properties
    //
    std::vector<const char*> cfModels{"Properties"};
    CreateTable( client, strTableModels, cfModels );
    //
    // Create the Simulations_Metadata table with one three column
    // families: G, M, R
    //
    std::vector<const char*> cfMetaData{"G", "M", "R"};
    CreateTable( client, strTableMetaData, cfMetaData );
    //
    // Create the Simulations_Data table with one two column families:
    // M, R
    //
    std::vector<const char*> cfData{"M", "R"};
    CreateTable( client, strTableData, cfData );
    transport->close( );
    }
  catch (const TException &tx) 
    {
    LOG(error) << tx.what( );
    return ERROR_EXCEPTION;
    }
  return SUCCESS;
}

int EncodeRowKey_MetaData( const std::string &keyModel,
                           const std::string &analysisName,
                           double step, std::string &rowKey )
{
  if ( keyModel.length( ) != MODEL_ROWKEY_LENGTH )
    {
    return ERROR_BAD_UUID_LENGTH;
    }

  boost::uint32_t lengthStr = analysisName.length( );
  rowKey.reserve( keyModel.length( ) +
                   sizeof( lengthStr )  + lengthStr +
                   sizeof( step ) );
  rowKey = keyModel;
  BinarySerializerType binWriter;
#if defined( USE_BINARY_ROWKEY )
  binWriter.SetConvertToHex( false );
#else
  binWriter.SetConvertToHex( true );
#endif
  binWriter.SetEndianness( GID::BigEndian );
  binWriter.Write( rowKey, analysisName );
  binWriter.Write( rowKey, step );
  return SUCCESS;
}

int DecodeRowKey_MetaData( const std::string &rowKey,
                           std::string &keyModel,
                           std::string &analysisName,
                           double &step )
{
#if defined(USE_BINARY_ROWKEY)
  const int minimumLength = 
    MODEL_ROWKEY_LENGTH + sizeof(boost::uint32_t) + sizeof(step);
#else
  const int minimumLength = 
    MODEL_ROWKEY_LENGTH + ( sizeof(boost::uint32_t) + sizeof(step) ) * 2;
#endif
  if ( rowKey.length( ) < minimumLength )
    {
    return ERROR_KEY_MINIMUM_LENGTH;
    }
  char uid[MODEL_ROWKEY_LENGTH];
  BinaryDeserializerType binReader;
  binReader.SetConvertFromHex( false );
  boost::uint32_t pos0 = binReader.Read( rowKey, reinterpret_cast<boost::int8_t*>(&uid[0]), MODEL_ROWKEY_LENGTH );
  if ( pos0 != MODEL_ROWKEY_LENGTH )
    {
    return ERROR_KEY_BAD_READ;
    }
  keyModel.assign( uid, MODEL_ROWKEY_LENGTH );
#if defined(USE_BINARY_ROWKEY)
  binReader.SetConvertFromHex( false );
#else
  binReader.SetConvertFromHex( true );
#endif
  binReader.SetEndianness( GID::BigEndian );
  boost::uint32_t pos1 = binReader.Read( rowKey, analysisName, pos0 );
  if ( pos1 <= pos0  )
    {
    return ERROR_KEY_BAD_READ;
    }
  pos0 = pos1;
  pos1 = binReader.Read( rowKey, &step, 1, pos0 );
  if ( pos1 <= pos0 )
    {
    return ERROR_KEY_BAD_READ;
    }
  return SUCCESS;
}

int InsertResult_MetaData( const std::string &host, int port,
                           const std::string &keyModel, 
                           const GlobalMeshInfoType &meshInfo,
                           const GlobalResultInfoType &resultInfo)
{
  boost::shared_ptr<TTransport> socket( new TSocket( host, port ) );
  boost::shared_ptr<TTransport> transport( new TBufferedTransport( socket ) );
  boost::shared_ptr<TProtocol> protocol( new TBinaryProtocol( transport ) );
  HbaseClient client( protocol );
  GID::MeshResultType mesh;
  int status;
  //boost::uuids::uuid id;
  //StringToUUID( keyModel, id );
  try 
    {    
    transport->open( );
    LOG(info) << "inserting result metada for model " << keyModel;
    // REVIEW: up to this point the mesh is static
    std::string analysisName( "" );
    double step = 0.0;
    std::string keyM;
    status = EncodeRowKey_MetaData( keyModel, analysisName, step, keyM );
    // REVIEW: first prototype use only one coordinate set
    if ( status == SUCCESS )
      {
      std::vector<Mutation> mutations;
      GID::UInt32 indexCSet = 1;
      const std::map<Text, Text>  dummyAttributes; // see HBASE-6806
                                                   // HBASE-4658
      // look for the first mesh with nodes defined
      bool csetFound = false;
      BinarySerializerType binWriter;
      binWriter.SetConvertToHex( false );
      binWriter.SetEndianness( GID::BigEndian );
      for( GlobalMeshInfoType::MapHeaderType::const_iterator it = meshInfo.headers.begin( );
           it  != meshInfo.headers.end( ); ++it  )
        {
        if ( !csetFound )
          {
          if ( it->second.numberOfNodes > 0 )
            {
            mutations.push_back( Mutation() );
            mutations.back( ).column = "M:c000001nm";
            mutations.back( ).value = it->first;
            mutations.push_back( Mutation() );
            mutations.back( ).column = "M:c000001nc";
            binWriter.Write( mutations.back( ).value, it->second.numberOfNodes );
            csetFound = true;
            }
          }
        std::string pm( "M:m" );
        //pm += boost::lexical_cast<std::string>( it->second.indexElementSet );
        pm += formatIndex( it->second.indexElementSet );
        mutations.push_back( Mutation() );
        mutations.back( ).column = pm;
        mutations.back( ).column += "nm";
        mutations.back( ).value = it->first;

        mutations.push_back( Mutation() );
        mutations.back( ).column = pm;
        mutations.back( ).column += "cn";
        // REVIEW: why not store only the index in binary form?
        mutations.back( ).value = "c000001";
        mutations.push_back( Mutation() );
        mutations.back( ).column = pm;
        mutations.back( ).column += "et";
        mutations.back( ).value = GID::GetElementTypeAsString( it->second.elementType );
        mutations.push_back( Mutation() );
        mutations.back( ).column = pm;
        mutations.back( ).column += "ne";
        binWriter.Write( mutations.back( ).value, it->second.numberOfElements );

        mutations.push_back( Mutation() );
        mutations.back( ).column = pm;
        mutations.back( ).column += "nn";
        binWriter.Write( mutations.back( ).value, it->second.sizeElement );

        mutations.push_back( Mutation() );
        mutations.back( ).column = pm;
        mutations.back( ).column += "cl";
        mutations.back( ).value = "";
        // push mesh columns
#ifndef DONT_APPLY_MUTATIONS
        client.mutateRow( strTableMetaData, keyM, mutations, dummyAttributes);
#endif
        }
      // process all analysis/timestep/result
      for( GlobalResultInfoType::MapAnalysisType::const_iterator itA = resultInfo.analysis.begin( );
           itA != resultInfo.analysis.end( ); ++itA )
        {
        for( GlobalResultInfoType::MapTimeStepType::const_iterator itS = itA->second.begin( );
             itS != itA->second.end( ); ++itS )
          {
          std::string keyR;
          mutations.clear( );
          status = EncodeRowKey_MetaData( keyModel, itA->first, itS->first, keyR );
          if ( status == SUCCESS )
            {
            for( GlobalResultInfoType::MapHeaderType::const_iterator itR = itS->second.begin( );
                 itR != itS->second.end( ); ++itR )
              {
              std::string pr( "R:r" );
              //pr += boost::lexical_cast<std::string>( itR->second.indexMData );
              pr += formatIndex( itR->second.indexMData );

              // nm
              mutations.push_back( Mutation() );
              mutations.back( ).column = pr;
              mutations.back( ).column += "nm";
              mutations.back( ).value = itR->second.name;
                
              // lc
              mutations.push_back( Mutation() );
              mutations.back( ).column = pr;
              mutations.back( ).column += "lc";
              mutations.back( ).value = GID::GetLocationAsString( itR->second.location );;

              // rt
              mutations.push_back( Mutation() );
              mutations.back( ).column = pr;
              mutations.back( ).column += "rt";
              mutations.back( ).value = GID::GetValueTypeAsString( itR->second.rType );

              // nc
              mutations.push_back( Mutation() );
              mutations.back( ).column = pr;
              mutations.back( ).column += "nc";
              binWriter.Write( mutations.back( ).value, itR->second.GetNumberOfComponents( ) );

              if ( itR->second.compName.size() )
                {
                // cn
                mutations.push_back( Mutation() );
                mutations.back( ).column = pr;
                mutations.back( ).column += "cn";
                for( std::vector<std::string>::const_iterator itC = itR->second.compName.begin( );
                     itC != itR->second.compName.end( ); ++itC )
                  {
                  binWriter.Write( mutations.back( ).value, *itC );
                  }
                }
                
              if( itR->second.location == GID::LOC_GPOINT )
                {
                // gp
                mutations.push_back( Mutation() );
                mutations.back( ).column = pr;
                mutations.back( ).column += "gp";
                mutations.back( ).value = itR->second.gpName;
                }
              else
                {
                // REVIEW: only one coordinate set is considered in
                // first prototype

                // lc
                mutations.push_back( Mutation() );
                mutations.back( ).column = pr;
                mutations.back( ).column += "co";
                mutations.back( ).value = "c000001";
                }

              // un
              mutations.push_back( Mutation() );
              mutations.back( ).column = pr;
              mutations.back( ).column += "un";
              mutations.back( ).value = "";
              }
#ifndef DONT_APPLY_MUTATIONS
            client.mutateRow( strTableMetaData, keyR, mutations, dummyAttributes);
#endif
            }
          else
            {
            // failed EncodeRowKey_MetaData
            break;
            }
          }
        }
      // process all gauss points definitions
      
      }
    else
      {
      // failed EncodeRowKey_MetaData
      }
    transport->close( );
    }
  catch (const TException &tx)
    {
    LOG(error) << tx.what( );
    return ERROR_EXCEPTION;
    }
  return status;
}

int EncodeRowKey_Data( const std::string &keyModel,
                       const std::string &analysisName,
                       double step, GID::IdPartition part, std::string &keyData )
{
  if ( keyModel.length() != MODEL_ROWKEY_LENGTH )
    {
    return ERROR_BAD_UUID_LENGTH;
    }

  boost::uint32_t lengthStr = analysisName.length( );
#if defined(USE_BINARY_ROWKEY)
  keyData.reserve( keyModel.length( ) +
                   sizeof( lengthStr )  + lengthStr +
                   sizeof( step ) +
                   sizeof( part ) );
#else
  keyData.reserve( keyModel.length( ) +
                   ( sizeof( lengthStr )  + lengthStr +
                     sizeof( step ) +
                     sizeof( part ) ) * 2 );
#endif
  keyData = keyModel;
  BinarySerializerType binWriter;
#if defined(USE_BINARY_ROWKEY)
  binWriter.SetConvertToHex( false );
#else
  binWriter.SetConvertToHex( true );
#endif
  binWriter.Write( keyData, analysisName );
  binWriter.Write( keyData, step );
  binWriter.Write( keyData, part );
  return SUCCESS;
}

int DecodeRowKey_Data( const std::string &keyData,
                       std::string &keyModel,
                       std::string &analysisName,
                       double &step, GID::IdPartition &part )
{
#if defined(USE_BINARY_ROWKEY)
  const int minimumLength = 
    MODEL_ROWKEY_LENGTH + sizeof(boost::uint32_t) + sizeof(step) + sizeof(part);
#else
  const int minimumLength = 
    MODEL_ROWKEY_LENGTH + ( sizeof(boost::uint32_t) + sizeof(step) + sizeof(part) ) * 2;
#endif
  if ( keyData.length( ) < minimumLength )
    {
    return ERROR_KEY_MINIMUM_LENGTH;
    }
  char uid[MODEL_ROWKEY_LENGTH];
  BinaryDeserializerType binReader;
  binReader.SetConvertFromHex( false );
  boost::uint32_t pos0 = binReader.Read( keyData, reinterpret_cast<boost::int8_t*>(&uid[0]), MODEL_ROWKEY_LENGTH );
  if ( pos0 != MODEL_ROWKEY_LENGTH )
    {
    LOG(error) << "while reading model rowkey";
    return ERROR_KEY_BAD_READ;
    }
  keyModel.assign( uid, MODEL_ROWKEY_LENGTH );
#if defined(USE_BINARY_ROWKEY)
  binReader.SetConvertToHex( false );
#else
  binReader.SetConvertFromHex( true );
#endif
  boost::uint32_t pos1 = binReader.Read( keyData, analysisName, pos0 );
  if ( pos1 <= pos0 )
    {
    LOG(error) << "while reading analysis name";
    return ERROR_KEY_BAD_READ;
    }
  pos0 = pos1;
  pos1 = binReader.Read( keyData, &step, 1, pos0 );
  if ( pos1 <= pos0 )
    {
    LOG(error) << "while reading step";
    return ERROR_KEY_BAD_READ;
    }
  pos0 = pos1;
  pos1 = binReader.Read( keyData, &part, 1, pos0 );
  if ( pos1 <= pos0 )
    {
    LOG(error) << "while reading partition";
    return ERROR_KEY_BAD_READ;
    }
  return SUCCESS;
}

int EncodeColumn_Data( char family, char prefix, GID::UInt32 indexSet,
                       const char * _SS, GID::UInt64 id, std::string &column)
{
  std::string binId;  
  BinarySerializerType binWriter;
  char cprefix[3];

  binWriter.SetConvertToHex( false );
  binWriter.SetEndianness( GID::BigEndian );
  cprefix[0] = family;
  cprefix[1] = ':';
  cprefix[2] = prefix;
  column.append( cprefix, 3 );
  
  std::stringstream ss;
  ss << std::setfill( '0' ) << std::setw( 6 );
  ss << indexSet << _SS;
  column += ss.str();
  // column += boost::lexical_cast<std::string>(indexSet);
  column += "_";
  binWriter.Write( binId, id );
  column += binId;
  return SUCCESS;
}

// REVIEW: this function must be updated see how EncodeColumn_Data
// works for indexSet coding & _SS
int DecodeColumn_Data( const std::string &column, char family, char prefix,
                       GID::UInt32 &indexSet, GID::UInt64 &id )
{
  if ( column.length( ) < 9 )
    {
    return ERROR_INVALID_COLUMN_NAME_LENGHT;
    }
  if ( column[0] != family || column[1] != ':' || column[2] != prefix )
    {
    return ERROR_INVALID_COLUMN_NAME_PREFIX;
    }
  const boost::uint32_t startCSET = 3;
  boost::uint32_t i = startCSET;
  while( column[ i ] && std::isdigit( column[ i ] ) )
    {
    ++i;
    }
  if ( i == startCSET || column[ i ] != '_' )
    {
    return ERROR_INVALID_COLUMN_NAME_CSET;
    }
  try
    {
    indexSet = boost::lexical_cast<GID::UInt32>( column.data() + startCSET, i - startCSET );
    }
  catch( const bad_lexical_cast & )
    {
    return ERROR_INVALID_COLUMN_NAME_CSET;
    }
  BinaryDeserializerType binReader;
  binReader.SetConvertFromHex( false );
  binReader.SetEndianness( GID::BigEndian );
  boost::uint32_t pos0 = binReader.Read( column, &id, i + 1 );
  if ( pos0 != i + 1 + sizeof( id ) )
    {
    return ERROR_INVALID_COLUMN_NAME_ID;
    }
  return SUCCESS;
}

void CheckDecodeRowKey_Data( const std::string & rowkey,
                             const std::string & keyModel,
                             const std::string & analysisName,
                             double step, GID::IdPartition indexPart )
{
  std::string checkKeyModel, checkAnalysisName;
  double checkStep;
  GID::IdPartition checkPart;
  int status = DecodeRowKey_Data( rowkey, checkKeyModel, checkAnalysisName, checkStep, checkPart );
  if ( status == SUCCESS )
    {
    if( checkKeyModel != keyModel ||
        checkAnalysisName != analysisName ||
        checkStep != step ||
        checkPart != indexPart )
      {
      LOG(info) << "checkKeyModel = " << checkKeyModel << ", " 
                << "checkAnalysisName = " << checkAnalysisName << ", " 
                << "checkStep = " << checkStep << ", " 
                << "checkPart = " << checkPart;
      LOG(error) << "DecodeRowKey_Data does not return the expected key fields";
      }
    }
  else
    {
    LOG(error) << "failed check of mesh data key with error code " << status;
    }
}

int InsertPartResult_Data( const std::string &host, int port,
                           const std::string &keyModel, 
                           GID::IdPartition indexPart,
                           GID::UInt32 indexESet,
                           const GID::MeshResultType &meshPart,
                           const GID::ResultContainerType &resultPart)
{
  boost::shared_ptr<TTransport> socket( new TSocket( host, port ) );
  boost::shared_ptr<TTransport> transport( new TBufferedTransport( socket ) );
  boost::shared_ptr<TProtocol> protocol( new TBinaryProtocol( transport ) );
  HbaseClient client( protocol );
  GID::MeshResultType mesh;
  int status;
  try 
    {    
    transport->open( );
    LOG(info) << "inserting data for partition " << indexPart;
    // REVIEW: up to this point the mesh is static
    std::string analysisName( "" );
    double step = 0.0;
    std::string keyM;
    status = EncodeRowKey_Data( keyModel, analysisName, step, indexPart, keyM );
    if ( status == SUCCESS )
      {
      LOG(info) << "keyM = " << keyM;
#if defined(CHECK_KEY_ENCODING)
      CheckDecodeRowKey_Data( keyM, keyModel, analysisName, step, indexPart );
#endif
      BinarySerializerType binWriter;
      binWriter.SetConvertToHex( false );
      binWriter.SetEndianness( GID::BigEndian );
      std::vector<Mutation> mutations;
      // TODO: first prototype use only one coordinate set
      GID::UInt32 indexCSet = 1;
      const std::map<Text, Text>  dummyAttributes; // see HBASE-6806
      // HBASE-4658
      // mutations for coordinate set
      for( std::vector<GID::MeshNodeType>::const_iterator it = meshPart.nodes.begin( );
           it != meshPart.nodes.end(); it++ )
        {
        mutations.push_back( Mutation( ) );
        
        status = EncodeColumn_Data( 'M', 'c', indexCSet, "", it->id, mutations.back().column );
        if ( status != SUCCESS )
          {
          break;
          }
        binWriter.Write( mutations.back().value, it->x );
        binWriter.Write( mutations.back().value, it->y );
        binWriter.Write( mutations.back().value, it->z );
        }
      // mutations for element set
      if ( status == SUCCESS )
        {
        for( std::vector<GID::MeshElementType>::const_iterator it = meshPart.elements.begin( );
             it != meshPart.elements.end(); it++ )
          {
          mutations.push_back( Mutation( ) );
          status = EncodeColumn_Data( 'M', 'm', indexESet, "cn",
                                      (*it)[0], mutations.back().column );
          if ( status != SUCCESS )
            {
            break;
            }
          BinarySerializerType binWriter;
          binWriter.SetConvertToHex( false );
          binWriter.SetEndianness( GID::BigEndian );
          for( size_t i = 1; i < it->size(); i++ )
            {
            // copy from boost::long_long_type to boost::uint64_t
            // which must be compatible
            GID::IdNode idN = (*it)[i];
            binWriter.Write( mutations.back().value, idN );
            }
          GID::UInt64 GG = 0;
          binWriter.Write( mutations.back().value, GG );
          }
        }
      // mutations for results
      if( status == SUCCESS )
        {
#ifndef DONT_APPLY_MUTATIONS
        client.mutateRow( strTableData, keyM, mutations, dummyAttributes);
#endif
        LOG(info) << "inserted " << meshPart.nodes.size() << " coordinates";
        LOG(info) << "inserted " << meshPart.elements.size() << " elements";
        
        for( std::vector<GID::ResultBlockType>::const_iterator itR = resultPart.results.begin( );
             itR != resultPart.results.end( ); itR++ )
          {
          std::string keyR;
          status = EncodeRowKey_Data( keyModel, itR->header.analysis, itR->header.step, indexPart, keyR );
          if ( status != SUCCESS )
            {
            break;
            }
#if defined(CHECK_KEY_ENCODING)
          CheckDecodeRowKey_Data( keyR, keyModel, itR->header.analysis,
                                  itR->header.step, indexPart );
#endif
          mutations.clear( );
          GID::UInt64 indexMData = itR->header.indexMData;
          for( std::vector<GID::ResultRowType>::const_iterator itV = itR->values.begin( );
               itV != itR->values.end( ); itV++ ) 
            {
            mutations.push_back( Mutation( ) );
            status = EncodeColumn_Data( 'R', 'r', indexMData, "", itV->id, mutations.back().column );
            if ( status != SUCCESS )
              {
              break;
              }
            for( std::vector<double>::const_iterator itD = itV->values.begin( );
                 itD != itV->values.end( ); itD++ )
              {
              binWriter.Write( mutations.back().value, *itD );
              }
            }
#ifndef DONT_APPLY_MUTATIONS
          client.mutateRow( strTableData, keyR, mutations, dummyAttributes);
#endif
          }
        }
      if ( status == SUCCESS )
        {
        LOG(info) << "inserted " << resultPart.results.size() << " results";
        }
      }
    else
      {
      LOG(error) << "failed creation of mesh data key with error code " << status;
      }
    transport->close( );
    }
  catch (const TException &tx)
    {
    LOG(error) << tx.what( );
    return ERROR_EXCEPTION;
    }
  return status;
}

int ProcessBinaryPart( const std::string &host, int port,
                       const std::string &keyModel,
                       GID::MeshResultType &meshPart,
                       GID::ResultContainerType &resultPart,
                       int index, const path &pathResult )
{
  boost::shared_ptr<TTransport> socket( new TSocket( host, port ) );
  boost::shared_ptr<TTransport> transport( new TBufferedTransport( socket ) );
  boost::shared_ptr<TProtocol> protocol( new TBinaryProtocol( transport ) );
  HbaseClient client( protocol );
  try 
    {    
    transport->open( );
    LOG(info) << "processing binary part " << pathResult;
    transport->close( );
    }
  catch (const TException &tx) 
    {
    LOG(error) << tx.what( );
    return ERROR_EXCEPTION;
    }
  return SUCCESS;
}

// REMOVE: should use Thrift API
std::string Int2Array( int x )
{
  char *buffer = static_cast<char*>(static_cast<void*>(&x));
  std::string tmp(buffer, buffer + sizeof(x) );
  return tmp;
};

int InsertModelInfo( const std::string &host, int port,
                     const ModelFileParts &modelInfo, 
                     std::string &keyModel )
{
  boost::shared_ptr<TTransport> socket( new TSocket( host, port ) );
  boost::shared_ptr<TTransport> transport( new TBufferedTransport( socket ) );
  boost::shared_ptr<TProtocol> protocol( new TBinaryProtocol( transport ) );
  HbaseClient client( protocol );
  BinarySerializerType binBuffer;
  binBuffer.SetConvertToHex( false );
  binBuffer.SetEndianness( GID::BigEndian );
  try 
    {    
    transport->open( );
    std::vector<Mutation> mutations;
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:nm";
    mutations.back().value = modelInfo.m_ModelName;
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:fp";
    mutations.back().value = canonical( modelInfo.m_PathRoot ).string();
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:np";
    //mutations.back().value = Int2Array( modelInfo.GetNumberOfParts() );
    binBuffer.Write( mutations.back( ).value, modelInfo.GetNumberOfParts( ) );
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:bb";
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:uAll";
    mutations.back().value = "read-only";
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:uAccess";
    mutations.back().value = "rwd";
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:uName";
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:vs";
    mutations.back().value = "Not validated";
    const std::map<Text, Text>  dummyAttributes; // see HBASE-6806
                                                 // HBASE-4658
    // the row key is generated as an UUID
    boost::uuids::uuid uid;
    //std::stringstream ss;
    //ss << uid;
    //LOG(trace) << "UUID as string has length = " << ss.str( ).length( );
    //keyModel = ss.str( );
    std::string tmp( uid.begin( ), uid.end( ) );
#if defined( USE_BINARY_ROWKEY )
    keyModel = tmp;
#else
    keyModel = GID::BinarySerializer::BinToHex( tmp );
#endif
    LOG(trace) << "using UUID = " << uid << "( '" << keyModel << "')"
               << " length = " << keyModel.length( );
#ifndef DONT_APPLY_MUTATIONS
    client.mutateRow( strTableModels, keyModel, mutations, dummyAttributes );
#endif
    transport->close( );
    }
  catch (const TException &tx) 
    {
    LOG(error) << tx.what( );
    return ERROR_EXCEPTION;
    }
    return SUCCESS;  
}

int ProcessInput( const po::variables_map &vm )
{
  std::string input( vm["input"].as<std::string>( ) );
  std::string host( vm["host"].as<std::string>( ) );
  size_t port( vm["port"].as<size_t>( ) );
  int status;

  path pathInputFile( input );
  if ( !is_regular_file( pathInputFile ) )
    {
    LOG(error) << "Input file " << pathInputFile << " is not a regular file";
    return -1;
    }
  LOG(info) << "Going to process sequence for file " << pathInputFile;
  if ( pathInputFile.native().length() <= 9 )
    {
    LOG(error) << "Input file must have one of the extensions: "
               << ".post.msh, .post.res or .post.bin";
    return -1;
    }
  path::string_type gid_ext( pathInputFile.native().end()-9, pathInputFile.native().end() );
  LOG(trace) << "ext = " << gid_ext;
  bool isbinary = false;
  if ( gid_ext == ".post.bin")
    {
    isbinary = true;
    }
  else if ( gid_ext != ".post.msh" || gid_ext != ".post.res" )
    {
    isbinary = false;
    }
  else
    {
    LOG(error) << "Input file must have one of the extensions: "
               << ".post.msh, .post.res or .post.bin";
    return -1;
    }
  
  ModelFileParts res_files, msh_files;
  status = collect_sequence( pathInputFile.native(), isbinary, res_files, msh_files );
  if ( status != SUCCESS )
    {
    return -1;
    }

  if ( !isbinary && res_files.m_Parts.size( ) != msh_files.m_Parts.size( ) )
    {
    LOG(error) << "ASCII format requires the same number of msh and res files";
    return -1;
    }
  if ( !isbinary) 
    {
    for( PathContainerType::const_iterator it = res_files.m_Parts.begin( );
         it != res_files.m_Parts.end( ); it++ )
      {
      if ( msh_files.m_Parts.find( it->first ) == msh_files.m_Parts.end( ) )
        {
        LOG(error) << "ASCII format missmatch, index " << it->first << " not found in msh container";
        return -1;      
        }
      }
    for( PathContainerType::const_iterator it = msh_files.m_Parts.begin( );
         it != msh_files.m_Parts.end( ); it++ )
      {
      if ( res_files.m_Parts.find( it->first ) == res_files.m_Parts.end( ) )
        {
        LOG(error) << "ASCII format missmatch, index " << it->first << " not found in res container";
        return -1;      
        }
      }
    }
  
  if ( vm.count("drop") )
    {
    status = DropVelasscoTables( host, port );
    if ( status != SUCCESS )
      {
      return status;
      }
    }
  if ( CheckVelasscoTables( host, port ) != SUCCESS )
    {
    CreateVelasscoTables( host, port );
    }
  GlobalMeshInfoType meshInfo;
  GlobalResultInfoType resultInfo;
  std::string keyModel;
  status = InsertModelInfo( host, port, res_files, keyModel );
  for( PathContainerType::const_iterator it = res_files.m_Parts.begin( );
          it != res_files.m_Parts.end( ); it++ )
    {
    GID::MeshResultType meshPart;
    GID::ResultContainerType resultPart;

    if ( isbinary )
      {
      status = ProcessBinaryPart( host, port, keyModel, meshPart, resultPart,
                                  it->first, it->second );
      if ( status != SUCCESS )
        {
        return status;
        }
      }
    else
      {
      LOG(trace) << "START PARSING: \"" << msh_files.m_Parts[it->first].string() << "\"";
      status = GID::ParseMeshFile( msh_files.m_Parts[it->first].string(), meshPart );
      LOG(trace) << "END PARSING: \"" << msh_files.m_Parts[it->first].string() << "\"";
      if ( status != SUCCESS )
        {
        return status;
        }
      // TODO: check status returned
      meshInfo.Update( meshPart );
      LOG(trace) << "START PARSING: \"" << it->second.string() << "\"";
      status = GID::ParseResultFile( it->second.string(), resultPart );
      LOG(trace) << "END PARSING: \"" << it->second.string() << "\"";
      if ( status != SUCCESS )
        {
        return status;
        }
      resultInfo.Update( resultPart );
      status = InsertPartResult_Data( host, port, keyModel, it->first,
                                      meshInfo.GetIndexElementSet( meshPart.header.name ),
                                      meshPart, resultPart );
      if ( status != SUCCESS )
        {
        return status;
        }
      }
    }
  status = InsertResult_MetaData( host, port, keyModel, meshInfo, resultInfo );
  if ( status != SUCCESS )
    {
    return status;
    }  
  return SUCCESS;
}

int main( int argc, char *argv[] )
{
  init_logger();
  try 
  { 
    std::string appName = boost::filesystem::basename(argv[0]); 
 
    /** Define and parse the program options 
     */ 
    namespace po = boost::program_options; 
    po::options_description desc("Options"); 
    desc.add_options() 
      ("help,", "Print help messages") 
      ("input,i", po::value<std::string>()->required(), "one of the input files") 
      ("drop,d", "Drop the tables before insert") 
      ("force,f", "Force update if the model already exists") 
      ("suffix,s", po::value<std::string>()->default_value("_V4CIMNE"), "Table name suffix") 
      ("host,h", po::value<std::string>()->default_value("localhost"), "Thrift server host") 
      ("port,p", po::value<size_t>()->default_value(9090), "Thrift server port");
 
    po::positional_options_description positionalOptions; 
    positionalOptions.add("input", 1); 
 
    po::variables_map vm; 
 
    try 
    { 
      po::store(po::command_line_parser(argc, argv).options(desc) 
                .positional(positionalOptions).run(), 
                vm); // throws on error 
 
      /** --help option 
       */ 
      if ( vm.count("help")  ) 
      { 
        std::cout << "This is just a template app that should be modified" 
                  << " and added to in order to create a useful command" 
                  << " line application" << std::endl << std::endl; 
        std::cout << desc << std::endl;
        return SUCCESS;
      } 
 
      po::notify(vm); // throws on error, so do after help in case 
                      // there are any problems 
    } 
    catch(boost::program_options::required_option& e) 
    { 
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
      std::cout << desc << std::endl;
      return ERROR_IN_COMMAND_LINE; 
    } 
    catch(boost::program_options::error& e) 
    { 
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
      std::cout << desc << std::endl;
      return ERROR_IN_COMMAND_LINE; 
    } 
 
    std::string suffix( vm["suffix"].as<std::string>( ) );
    strTableModels += suffix;
    strTableMetaData += suffix;
    strTableData += suffix;
    return ProcessInput( vm ); 
  } 
  catch(std::exception& e) 
  { 
    std::cerr << "Unhandled Exception reached the top of main: " 
              << e.what() << ", application will now exit" << std::endl; 
    return ERROR_UNHANDLED_EXCEPTION; 
 
  } 
 
  return SUCCESS; 
}
