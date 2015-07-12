#include <iostream>
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
  // TODO: collect also GP & RT
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
  typedef std::map<std::string, GID::ResultHeaderType> MapHeaderType;

  MapHeaderType headers;

  int Update( GID::ResultHeaderType & h )
  {
    GID::ResultHeaderType &header = this->headers[ h.name ];
    if( header.indexMData == 0 )
      {
      h.indexMData = this->headers.size( );
      }
    else
      {
      // TODO: check if resultPart matches first partition header
      }
    header = h;
    return SUCCESS;
  }

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
    return status;
  }

  GID::UInt64 GetIndexMetaData( const std::string &name )
  {
    MapHeaderType::const_iterator it = this->headers.find( name );
    if ( it != this->headers.end( ) )
      {
      return it->second.indexMData;
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

int InsertMesh_MetaData( const std::string &host, int port,
                         const std::string &keyModel, 
                         GlobalMeshInfoType &meshInfo )
{
  boost::shared_ptr<TTransport> socket( new TSocket( host, port ) );
  boost::shared_ptr<TTransport> transport( new TBufferedTransport( socket ) );
  boost::shared_ptr<TProtocol> protocol( new TBinaryProtocol( transport ) );
  HbaseClient client( protocol );
  GID::MeshResultType mesh;
  int status;
  boost::uuids::uuid id;
  StringToUUID( keyModel, id );
  try 
    {    
    transport->open( );
    LOG(info) << "inserting mesh metada for " << id;
    transport->close( );
    }
  catch (const TException &tx)
    {
    LOG(error) << tx.what( );
    return ERROR_EXCEPTION;
    }
  return status;
}

int InsertResult_MetaData( const std::string &host, int port,
                           const std::string &keyModel, 
                           GlobalResultInfoType &resultInfo )
{
  boost::shared_ptr<TTransport> socket( new TSocket( host, port ) );
  boost::shared_ptr<TTransport> transport( new TBufferedTransport( socket ) );
  boost::shared_ptr<TProtocol> protocol( new TBinaryProtocol( transport ) );
  HbaseClient client( protocol );
  GID::MeshResultType mesh;
  int status;
  boost::uuids::uuid id;
  StringToUUID( keyModel, id );
  try 
    {    
    transport->open( );
    LOG(info) << "inserting result metada for " << id;
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
  if ( keyModel.length() != 16 )
    {
    return ERROR_BAD_UUID_LENGTH;
    }

  boost::uint32_t lengthStr = analysisName.length( );
  keyData.reserve( keyModel.length( ) +
                   sizeof( lengthStr )  + lengthStr +
                   sizeof( step ) +
                   sizeof( part ) );
  keyData = keyModel;
  GID::BinarySerializer binWriter;
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
  const int minimumLength = 
    16 + sizeof(boost::uint32_t) +
    sizeof(step) + sizeof(part);
  if ( keyData.length( ) < minimumLength )
    {
    return ERROR_KEY_MINIMUM_LENGTH;
    }
  char uid[16];
  GID::BinaryDeserializer binReader;
  boost::uint32_t pos0 = binReader.Read( keyData, reinterpret_cast<boost::int8_t*>(&uid[0]), 16 );
  if ( pos0 != 16 )
    {
    return ERROR_KEY_BAD_READ;
    }
  keyModel.assign( uid, 16 );
  boost::uint32_t pos1 = binReader.Read( keyData, analysisName, pos0 );
  if ( pos1 - pos0 < sizeof(boost::uint32_t) )
    {
    return ERROR_KEY_BAD_READ;
    }
  pos0 = pos1;
  pos1 = binReader.Read( keyData, &step, 1, pos0 );
  if ( pos1 - pos0 != sizeof( step ) )
    {
    return ERROR_KEY_BAD_READ;
    }
  pos0 = pos1;
  pos1 = binReader.Read( keyData, &part, 1, pos0 );
  if ( pos1 - pos0 != sizeof( part ) )
    {
    return ERROR_KEY_BAD_READ;
    }
  return SUCCESS;
}

int EncodeColumn_Data( char family, char prefix, GID::UInt32 indexSet, GID::UInt64 id, std::string &column)
{
  std::string binId;  
  GID::BinarySerializer binWriter;
  char cprefix[3];
  
  cprefix[0] = family;
  cprefix[1] = ':';
  cprefix[2] = prefix;
  column.append( cprefix, 3 );
  column += boost::lexical_cast<std::string>(indexSet);
  column += "_";
  binWriter.Write( binId, id );
  column += binId;
  return SUCCESS;
}

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
  GID::BinaryDeserializer binReader;
  boost::uint32_t pos0 = binReader.Read( column, &id, i + 1 );
  if ( pos0 != i + 1 + sizeof( id ) )
    {
    return ERROR_INVALID_COLUMN_NAME_ID;
    }
  return SUCCESS;
}

int InsertPartResult_Data( const std::string &host, int port,
                           const std::string &keyModel, 
                           GID::MeshResultType &meshPart,
                           GID::IdPartition part,
                           GID::UInt32 indexESet,
                           GID::ResultContainerType &resultPart)
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
    LOG(info) << "inserting mesh data '" << meshPart.header.name << "' for partition " << part;
    // REVIEW: analysis & step must be provided as arguments
    std::string analysisName( "" );
    double step = 0.0;
    std::string key;
    status = EncodeRowKey_Data( keyModel, analysisName, step, part, key );
    if ( status == SUCCESS )
      {
      LOG(info) << "key = " << key;
      GID::BinarySerializer binWriter;
      std::string checkKeyModel, checkAnalysisName;
      double checkStep;
      GID::IdPartition checkPart;
      status = DecodeRowKey_Data( key, checkKeyModel, checkAnalysisName, checkStep, checkPart );
      if ( status == SUCCESS )
        {
        LOG(info) << "checkKeyModel = " << checkKeyModel << ", " 
                  << "checkAnalysisName = " << checkAnalysisName << ", " 
                  << "checkStep = " << checkStep << ", " 
                  << "checkPart = " << checkPart;
        assert( checkKeyModel == keyModel );
        assert( checkAnalysisName == analysisName );
        assert( checkStep == step );
        assert( checkPart == part );
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
          
          status = EncodeColumn_Data( 'M', 'c', indexCSet, it->id, mutations.back().column );
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
            status = EncodeColumn_Data( 'M', 'm', indexESet, (*it)[0], mutations.back().column );
            if ( status != SUCCESS )
              {
              break;
              }
            GID::BinarySerializer binWriter;
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
          for( std::vector<GID::ResultBlockType>::const_iterator itR = resultPart.results.begin( );
               itR != resultPart.results.end( ); itR++ )
            {
            GID::UInt64 indexMData = itR->header.indexMData;
            for( std::vector<GID::ResultRowType>::const_iterator itV = itR->values.begin( );
                 itV != itR->values.begin( ); itV++ ) 
              {
              mutations.push_back( Mutation( ) );
              status = EncodeColumn_Data( 'R', 'r', indexMData, itV->id, mutations.back().column );
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
            }
          }
        if ( status == SUCCESS )
          {
          client.mutateRow( strTableData, key, mutations, dummyAttributes);
          LOG(info) << "inserted " << meshPart.nodes.size() << " coordinates";
          LOG(info) << "inserted " << meshPart.elements.size() << " elements";
          LOG(info) << "inserted " << resultPart.results.size() << " results";
          }
        }
      else
        {
        LOG(error) << "failed check of mesh data key with error code " << status;
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
  GID::BinarySerializer binBuffer;
  try 
    {    
    transport->open( );
    std::vector<Mutation> mutations;
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:nm";
    mutations.back().value = modelInfo.m_ModelName;
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:fp";
    mutations.back().value = modelInfo.m_PathRoot.string();
    mutations.push_back( Mutation( ) );
    mutations.back().column = "Properties:np";
    //mutations.back().value = Int2Array( modelInfo.GetNumberOfParts() );
    binBuffer.Write( mutations.back( ).value, modelInfo.GetNumberOfParts( ) );
    const std::map<Text, Text>  dummyAttributes; // see HBASE-6806
                                                 // HBASE-4658
    // the row key is generated as an UUID
    boost::uuids::uuid uid;
    //std::stringstream ss;
    //ss << uid;
    //LOG(trace) << "UUID as string has length = " << ss.str( ).length( );
    //keyModel = ss.str( );
    std::string tmp( uid.begin( ), uid.end( ) );
    keyModel = tmp;
    LOG(trace) << "using UUID = " << uid << "( '" << keyModel << " ')"
               << " length = " << keyModel.length( );
    client.mutateRow( strTableModels, keyModel, mutations, dummyAttributes );
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
      status = GID::ParseMeshFile( msh_files.m_Parts[it->first].string(), meshPart );
      if ( status != SUCCESS )
        {
        return status;
        }
      // TODO: check status returned
      meshInfo.Update( meshPart );
      status = GID::ParseResultFile( it->second.string(), resultPart );
      if ( status != SUCCESS )
        {
        return status;
        }
      resultInfo.Update( resultPart );
      status = InsertPartResult_Data( host, port, keyModel, meshPart, it->first,
                                      meshInfo.GetIndexElementSet( meshPart.header.name ),
                                      resultPart );
      if ( status != SUCCESS )
        {
        return status;
        }
      }
    }
  status = InsertMesh_MetaData( host, port, keyModel, meshInfo );
  if ( status != SUCCESS )
    {
    return status;
    }
  status = InsertResult_MetaData( host, port, keyModel, resultInfo );
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
    int add = 0; 
    int like = 0; 
    std::vector<std::string> sentence; 
 
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
