/* -*- c++ -*- */
#pragma once

// STD
#include <string>
#include <vector>

#include <openssl/md5.h>

inline std::string computeMD5( const std::string &str_in) {
  MD5_CTX md5ctx;
  unsigned char digest[ MD5_DIGEST_LENGTH];
  MD5_Init( &md5ctx);
  MD5_Update( &md5ctx, str_in.data(), str_in.length());
  MD5_Final( digest, &md5ctx);
  return std::string( ( const char *)digest, MD5_DIGEST_LENGTH);
}

//VELaSSCo
// no need for AbstractDB.h anymore ...
// #include "AbstractDB.h"

/////////////////////////////////
// Thrift communication to HBase:
/////////////////////////////////
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

// from the Hbase thrift 1:
#include "Hbase.h"
using namespace  ::apache::hadoop::hbase::thrift;
typedef std::map<std::string,TCell> CellMap;

// for the common structures in the Access module
// from the former StorageModule thrift:
#include "VELaSSCoSM.h"
using namespace VELaSSCoSM;

#include "Helpers.h"

namespace VELaSSCo
{

  class HBase // : public AbstractDB
  {
  
  public:
    HBase(): _hbase_client( NULL), _socket( NULL), _transport( NULL), _protocol( NULL), _db_host("") {}
    ~HBase() { stopConnection(); }

    bool startConnection( const char *DB_hostname, const int DB_port);
    bool stopConnection();

    std::string getStatusDB();

    std::string getListOfModelNames( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
				     const std::string &sessionID, const std::string &model_group_qualifier, 
				     const std::string &model_name_pattern);
    std::string getListOfModelNames_curl( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					  const std::string &sessionID, const std::string &model_group_qualifier, 
					  const std::string &model_name_pattern);
    bool getListOfModelInfoFromTables( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
				       const std::string &table_name, const std::string &model_name_pattern);
    std::string getListOfModelNames_thrift( std::string &report, std::vector< FullyQualifiedModelName> &listOfModelNames, 
					    const std::string &sessionID, const std::string &model_group_qualifier, 
					    const std::string &model_name_pattern);
    std::string findModel( std::string &report, std::string &modelID, FullyQualifiedModelName &model_info,
			   const std::string &sessionID, const std::string &unique_model_name_pattern, 
			   const std::string &requested_access);

    bool getListOfMeshInfoFromTables( std::string &report, std::vector< MeshInfo> &listOfMeshes,
					     const std::string &metadata_table, const std::string &modelID,
					     const std::string &analysisID, const double stepValue, const char *format="%02x"); // for the stepvalue hex string
    std::string getListOfMeshes( std::string &report, std::vector< MeshInfo> &listOfMeshes,
				 const std::string &sessionID, const std::string &modelID,
				 const std::string &analysisID, const double stepValue);
    bool getListOfAnalysesNamesFromTables( std::string &report, std::vector< std::string> &listOfAnalyses,
					     const std::string &metadata_table, const std::string &modelID);
    std::string getListOfAnalyses( std::string &report, std::vector< std::string> &listOfAnalyses,
				   const std::string &sessionID, const std::string &modelID);

    bool getListOfStepsFromTables( std::string &report, std::vector< double> &listOfSteps,
				   const std::string &metadata_table, const std::string &modelID,
				   const std::string &analysisID);
    std::string getListOfSteps( std::string &report, std::vector< double> &listOfSteps,
				const std::string &sessionID, const std::string &modelID,
				const std::string &analysisID);

    bool getListOfResultsFromTables( std::string &report, std::vector< ResultInfo> &listOfResults,
				     const std::string &sessionID, const std::string &modelID,
				     const std::string &analysisID, const double stepValue, const char *format="%02x"); // for the stepvalue hex string
    std::string getListOfResults( std::string &report, std::vector< ResultInfo> &listOfResults,
				  const std::string &sessionID, const std::string &modelID,
				  const std::string &analysisID, const double stepValue);
    bool getListOfVerticesFromTables( std::string &report, std::vector< Vertex> &listOfVertices,
				      const std::string &sessionID, const std::string &modelID,
				      const std::string &analysisID, const double stepValue, 
				      const int32_t meshID, 
				      const char *format="%02x"); // for the stepvalue hex string
    std::string getListOfVerticesFromMesh( std::string &report, std::vector< Vertex> &listOfVertices,
					   const std::string &sessionID, const std::string &modelID, 
					   const std::string &analysisID, const double stepValue, 
					   const int32_t meshID);
    bool getListOfSelectedVerticesFromTables( std::string &report, std::vector< Vertex> &listOfVertices,
					      const std::string &sessionID, const std::string &modelID,
					      const std::string &analysisID, const double stepValue, 
					      const int32_t meshID, const std::vector<int64_t> &listOfVerticesID,
					      const char *format="%02x"); // for the stepvalue hex string
    std::string getListOfSelectedVerticesFromMesh( std::string &report, std::vector< Vertex> &listOfVertices,
						   const std::string &sessionID, const std::string &modelID, 
						   const std::string &analysisID, const double stepValue, 
						   const int32_t meshID, const std::vector<int64_t> &listOfVerticesID);
					   
    bool getResultFromVerticesIDFromTables( std::string& report, std::vector<ResultOnVertex> &listOfResults, const std::string& table_name,
           const std::string &sessionID,  const std::string &modelID,
           const std::string &analysisID, const double       timeStep,  
           const ResultInfo &resultInfo,  const std::vector<int64_t> &listOfVerticesID, const char *format = "%02x" );
    bool getResultFromVerticesIDFromTables_filter( std::string& report, std::vector<ResultOnVertex> &listOfResults, const std::string& table_name,
           const std::string &sessionID,  const std::string &modelID,
           const std::string &analysisID, const double       timeStep,  
           const ResultInfo &resultInfo, const std::vector<int64_t> &listOfVerticesID, const int64_t &minVertexID, const int64_t &maxVertexID, 
           const char *format = "%02x" );
    std::string getResultFromVerticesID( std::string& report, std::vector<ResultOnVertex> &listOfResults,
					 const std::string &sessionID, const std::string &modelID,
    			     const std::string &analysisID, const double       timeStep,  
				     const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID );
    std::string getResultFromVerticesID_curl( const std::string &sessionID,  const std::string &modelID, 
					  const std::string &analysisID, const double       timeStep,  
					  const std::string &resultID,   const std::string &listOfVertices );
	std::string getResultFromVerticesID_thrift( std::string& report, std::vector<ResultOnVertex> &listOfResults,
					 const std::string &sessionID, const std::string &modelID,
    			     const std::string &analysisID, const double       timeStep,  
				     const std::string &resultID,   const std::vector<int64_t> &listOfVerticesID );
	std::string getResultFromVerticesID_thrift_filter( std::string& report, std::vector<ResultOnVertex> &listOfResults,
					 const std::string &sessionID, const std::string &modelID,
    			     const std::string &analysisID, const double       timeStep,  
				     const std::string &resultID,   const std::vector<int64_t>& listOfVerticesID );
					    
	// GetCoordinatesAndElementsFromMesh
	bool        getMeshElementsFromTable(std::string& report,
					std::vector< Element > &listOfElements, std::vector< ElementAttrib > &listOfElementAttribs, 
					std::vector< ElementGroup > &listOfElementInfoGroups,
					const std::string& table_name,
					const std::string &sessionID, const std::string &modelID,
                    const std::string &analysisID,const double timeStep, const MeshInfo& meshInfo, const char *format = "%02x" );
	std::string getCoordinatesAndElementsFromMesh( std::string& report,
					std::vector<Vertex>& vertices,
					std::vector< Element > &listOfElements, std::vector< ElementAttrib > &listOfElementAttribs, 
					std::vector< ElementGroup > &listOfElementInfoGroups,
					const std::string &sessionID, const std::string &modelID,
                    const std::string &analysisID,const double timeStep, const MeshInfo& meshInfo);
	std::string getCoordinatesAndElementsFromMesh_curl(const std::string &sessionID, const std::string &modelID,
                    const std::string &analysisID,
                    const double       timeStep,
                    const unsigned     partitionID);
    std::string getCoordinatesAndElementsFromMesh_thrift( std::string& report,
					std::vector<Vertex>& vertices,
					std::vector< Element > &listOfElements, std::vector< ElementAttrib > &listOfElementAttribs, 
					std::vector< ElementGroup > &listOfElementInfoGroups,
					const std::string &sessionID, const std::string &modelID,
                    const std::string &analysisID,const double timeStep, const MeshInfo& meshInfo);

    void getStoredBoundaryOfAMesh( const std::string &sessionID,
				      const std::string &modelID,
				      const int meshID, const std::string &elementType,
				      const std::string &analysisID, const double stepValue,
				      std::string *return_binary_mesh, std::string *return_error_str);
    void deleteStoredBoundaryOfAMesh( const std::string &sessionID,
				      const std::string &modelID,
				      const int meshID, const std::string &elementType,
				      const std::string &analysisID, const double stepValue,
				      std::string *return_error_str);
    bool saveBoundaryOfAMesh( const std::string &sessionID,
			      const std::string &modelID,
			      const int meshID, const std::string &elementType,
			      const std::string &analysisID, const double stepValue,
			      const std::string &binary_mesh, std::string *return_error_str);

    void getStoredBoundingBox( const std::string &sessionID, const std::string &modelID, 
			       const std::string &analysisID, const int numSteps, const double *lstSteps,
			       const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			       double *return_bbox, std::string *return_error_str);
    void deleteStoredBoundingBox( const std::string &sessionID, const std::string &modelID, 
				  const std::string &analysisID, const int numSteps, const double *lstSteps,
				  const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
				  std::string *return_error_str);
    bool saveBoundingBox( const std::string &sessionID, const std::string &modelID, 
			  const std::string &analysisID, const int numSteps, const double *lstSteps,
			  const int64_t numVertexIDs, const int64_t *lstVertexIDs, 
			  const double *return_bbox, std::string *return_error_str);

    // retrieve only the 'Q' column family of the Simulations_VQuery_Results_Data table
    bool getStoredVQueryExtraDataSplitted( const std::string &sessionID,
					   const std::string &modelID,
					   const std::string &analysisID, const double stepValue,
					   const std::string &vqueryName, const std::string &vqueryParameters,
					   // list = 1 entry per partition, may be null only to check if there is data
					   std::vector< std::string> *lst_vquery_results);
    bool getStoredVQueryExtraData( const std::string &sessionID,
				   const std::string &modelID,
				   const std::string &analysisID, const double stepValue,
				   const std::string &vqueryName, const std::string &vqueryParameters,
				   // list = 1 entry per partition, may be null only to check if there is data
				   std::vector< std::string> *lst_vquery_results);

  private:

    double fRand(double fMin, double fMax);
    std::string parseStatusDB( std::string b);
    std::string parse1DEM(std::string b, std::string LOVertices);
    std::string parse1FEM(std::string b);
    // returns true if there are models on the table to be parsed
    bool parseListOfModelNames( std::string &report,
				std::vector< FullyQualifiedModelName> &listOfModelNames,
				std::string buffer, const std::string &table_name);

  public:
    // to store information on where is the model stored
    // support simulation data in several tables: 
    //    VELaSSCo_Models & co.
    //    VELaSSCo_Models_V4CIMNE & co.
    //    Test_VELaSSCo_Models & co.
    //    T_VELaSSCo_Models & co.
    class TableModelEntry {
    public:
      std::string _list_models;
      std::string _metadata;
      std::string _data;
      std::string _stored_vquery_metadata;
      std::string _stored_vquery_data;
    } ;
    // returns true if info is found ( i.e. OpenModel was issued)
    bool getVELaSSCoTableNames( const std::string &sessionID, const std::string &modelID, TableModelEntry &tables) const;

  private:
    HbaseClient *_hbase_client;
    boost::shared_ptr<TTransport> *_socket;
    boost::shared_ptr<TTransport> *_transport;
    boost::shared_ptr<TProtocol> *_protocol;
    std::string _db_host;

    typedef std::map< std::string, TableModelEntry> DicTableModels; // key is sessionID + modelID
    DicTableModels _table_models;
    // return true if velassco_model_table_name is known and could be inserted
    bool storeTableNames( const std::string &sessionID, const std::string &modelID, const std::string &velassco_model_table_name);
    std::vector< std::string> getModelListTables() const;

    std::string createMetaRowKey( const std::string modelID, const std::string &analysysID, const double stepValue, const char *format="%02x"); // for the stepvalue hex string
    std::string createDataRowKey( const std::string modelID, const std::string &analysysID, const double stepValue, const int partitionID, const char *format="%02x"); // for the stepvalue hex string
    std::string createMetaRowKeyPrefix( const std::string modelID, const std::string &analysysID);
    // to access stored data
    bool checkIfTableExists( const std::string table_name);
    std::string getVQueryID( const std::string &vqueryName, const std::string &vqueryParameters);
    std::string createStoredMetadataRowKey( const std::string modelID, const std::string &analysisID, const double stepValue,
					    const std::string &vqueryName, const std::string &vqueryParameters);
    std::string createStoredDataRowKey( const std::string modelID, const std::string &analysisID, const double stepValue,
					const std::string &vqueryName, const std::string &vqueryParameters, const int partitionID);
    bool createStoredMetadataTable( const std::string &table_name);
    bool createStoredDataTable( const std::string &table_name);

    bool getColumnQualifierStringFromTable( std::string &retValue, 
					    const std::string &tableName, 
					    const std::string &startRowKey, const std::string &stopRowKey,
					    const char *columnFamily, const char *columnQualifier,
					    const std::string &logMessagePrefix);
    bool getColumnQualifierStringListFromTable( std::vector< std::string> &retValue, 
						const std::string &tableName, 
						const std::string &startRowKey, const std::string &stopRowKey,
						const char *columnFamily, const char *columnQualifierPrefix,
						const int numStringsToRetrieve,
						const std::string &logMessagePrefix);

  };

  typedef std::vector< std::string> StrVec;
  typedef std::vector< ColumnDescriptor> ColVec;
  typedef std::map< std::string, std::string> StrMap;

  inline bool HBase::getVELaSSCoTableNames( const std::string &sessionID, const std::string &modelID, TableModelEntry &tables) const {
    const DicTableModels::const_iterator it = _table_models.find( sessionID + modelID);
    if ( it != _table_models.end())
      tables = it->second;
    return ( it != _table_models.end());
  }

  inline std::string HBase::createMetaRowKey( const std::string modelID, const std::string &analysisID, const double stepValue, const char *format) { // for the stepvalue hex string
    const size_t tmp_buf_size = 256;
    char tmp_buf[ tmp_buf_size];
    std::string modelID_hex( ModelID_DoHexStringConversionIfNecesary( modelID, tmp_buf, tmp_buf_size));
    size_t analysis_length = analysisID.length();
    // needs to be swapped !!!!!!!!
    std::string length_hex( toHexStringSwap< int>( ( int)analysis_length));
    std::string step_hex( toHexStringSwap< double>( stepValue, format));
    return ( analysis_length ? ( modelID_hex + length_hex + analysisID + step_hex) : ( modelID_hex + length_hex + step_hex));
  }
  inline std::string HBase::createMetaRowKeyPrefix( const std::string modelID, const std::string &analysisID) {
    const size_t tmp_buf_size = 256;
    char tmp_buf[ tmp_buf_size];
    std::string modelID_hex( ModelID_DoHexStringConversionIfNecesary( modelID, tmp_buf, tmp_buf_size));
    size_t analysis_length = analysisID.length();
    // needs to be swapped !!!!!!!!
    std::string length_hex( toHexStringSwap< int>( ( int)analysis_length));
    return ( analysis_length ? ( modelID_hex + length_hex + analysisID) : ( modelID_hex + length_hex));
  }
  inline std::string HBase::createDataRowKey( const std::string modelID, const std::string &analysisID, const double stepValue, const int partitionID, const char *format) { // for the stepvalue hex string
    const size_t tmp_buf_size = 256;
    char tmp_buf[ tmp_buf_size];
    std::string modelID_hex( ModelID_DoHexStringConversionIfNecesary( modelID, tmp_buf, tmp_buf_size));
    size_t analysis_length = analysisID.length();
    // needs to be swapped !!!!!!!!
    std::string length_hex( toHexStringSwap< int>( ( int)analysis_length));
    std::string step_hex( toHexStringSwap< double>( stepValue, format));
    std::string part_hex( toHexStringSwap< int>( partitionID, format));
    return ( analysis_length ? ( modelID_hex + length_hex + analysisID + step_hex + part_hex) : ( modelID_hex + length_hex + step_hex + part_hex));
  }

  inline std::string HBase::getVQueryID( const std::string &vqueryName, const std::string &vqueryParameters) {
    MD5_CTX md5ctx;
    unsigned char digest[ MD5_DIGEST_LENGTH];
    MD5_Init( &md5ctx);
    MD5_Update( &md5ctx, vqueryName.data(), vqueryName.length());
    MD5_Update( &md5ctx, vqueryParameters.data(), vqueryParameters.length());
    MD5_Final( digest, &md5ctx);
    return std::string( ( const char *)digest, MD5_DIGEST_LENGTH);
  }

  inline std::string HBase::createStoredMetadataRowKey( const std::string modelID, const std::string &analysisID, const double stepValue,
							const std::string &vqueryName, const std::string &vqueryParameters) {
    const size_t tmp_buf_size = 256;
    char tmp_buf[ tmp_buf_size];
    std::string modelID_hex( ModelID_DoHexStringConversionIfNecesary( modelID, tmp_buf, tmp_buf_size));
    size_t analysis_length = analysisID.length();
    // needs to be swapped !!!!!!!!
    std::string length_hex( toHexStringSwap< int>( ( int)analysis_length));
    const char *format = "%02x";
    std::string step_hex( toHexStringSwap< double>( stepValue, format));
    std::string qid_str = toHexString( getVQueryID( vqueryName, vqueryParameters));
    return ( analysis_length ? ( modelID_hex + length_hex + analysisID + step_hex + qid_str) : ( modelID_hex + length_hex + step_hex + qid_str));
  }

  inline std::string HBase::createStoredDataRowKey( const std::string modelID, const std::string &analysisID, const double stepValue,
						    const std::string &vqueryName, const std::string &vqueryParameters,
						    const int partitionID) {
    const size_t tmp_buf_size = 256;
    char tmp_buf[ tmp_buf_size];
    std::string modelID_hex( ModelID_DoHexStringConversionIfNecesary( modelID, tmp_buf, tmp_buf_size));
    size_t analysis_length = analysisID.length();
    // needs to be swapped !!!!!!!!
    std::string length_hex( toHexStringSwap< int>( ( int)analysis_length));
    const char *format = "%02x";
    std::string step_hex( toHexStringSwap< double>( stepValue, format));
    std::string qid_str = toHexString( getVQueryID( vqueryName, vqueryParameters));
    std::string part_hex( toHexStringSwap< int>( partitionID, format));
    return ( analysis_length ? ( modelID_hex + length_hex + analysisID + step_hex + qid_str + part_hex) : ( modelID_hex + length_hex + step_hex + qid_str + part_hex));
  }


} // namespace VELaSSCo

void printRow(const TRowResult &rowResult);
