/* -*- c++ -*- */
#pragma once

// STD
#include <string>

//VELaSSCo
#include "AbstractDB.h"
#include "Helpers.h"

namespace VELaSSCo
{

  class HBase : public AbstractDB
  {
  
  public:
    HBase(): _hbase_client( NULL), _socket( NULL), _transport( NULL), _protocol( NULL) {}
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
					     const std::string &analysisID, const double stepValue);
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

    std::string getResultOnVertices( const std::string &sessionID,  const std::string &modelID, 
				     const std::string &analysisID, const double       timeStep,  
				     const std::string &resultID,   const std::string &listOfVertices );
    std::string getResultOnVertices_curl( const std::string &sessionID,  const std::string &modelID, 
					  const std::string &analysisID, const double       timeStep,  
					  const std::string &resultID,   const std::string &listOfVertices );
    std::string getResultOnVertices_thrift( const std::string &sessionID,  const std::string &modelID, 
					    const std::string &analysisID, const double       timeStep,  
					    const std::string &resultID,   const std::string &listOfVertices );
					    
	// GetCoordinatesAndElementsFromMesh
	std::string getCoordinatesAndElementsFromMesh(const std::string &sessionID, const std::string &modelID,
                                             const std::string &analysisID,
                                             const double       timeStep,
                                             const std::string &resultID);
	std::string getCoordinatesAndElementsFromMesh_curl(const std::string &sessionID, const std::string &modelID,
                                             const std::string &analysisID,
                                             const double       timeStep,
                                             const std::string &resultID);
	std::string getCoordinatesAndElementsFromMesh_thrift(const std::string &sessionID, const std::string &modelID,
                                             const std::string &analysisID,
                                             const double       timeStep,
                                             const std::string &resultID);
  private:

    double fRand(double fMin, double fMax);
    std::string parseStatusDB( std::string b);
    std::string parse1DEM(std::string b, std::string LOVertices);
    // returns true if there are models on the table to be parsed
    bool parseListOfModelNames( std::string &report,
				std::vector< FullyQualifiedModelName> &listOfModelNames,
				std::string buffer, const std::string &table_name);


  private:
    HbaseClient *_hbase_client;
    boost::shared_ptr<TTransport> *_socket;
    boost::shared_ptr<TTransport> *_transport;
    boost::shared_ptr<TProtocol> *_protocol;

    // to store information on where is the model stored
    class TableModelEntry {
    public:
      std::string _list_models;
      std::string _metadata;
      std::string _data;
    } ;
    typedef std::map< std::string, TableModelEntry> DicTableModels; // key is sessionID + modelID
    DicTableModels _table_models;
    // returns true if info is found ( i.e. OpenModel was issued)
    bool getTableNames( const std::string &sessionID, const std::string &modelID, TableModelEntry &tables) const;
    // return true if velassco_model_table_name is known and could be inserted
    bool storeTableNames( const std::string &sessionID, const std::string &modelID, const std::string &velassco_model_table_name);
    std::vector< std::string> getModelListTables() const;

    std::string createRowKey( const std::string modelID, const std::string &analysysID, const double stepValue);
    std::string createRowKeyPrefix( const std::string modelID, const std::string &analysysID);
  };

  typedef std::vector<std::string> StrVec;

  inline bool HBase::getTableNames( const std::string &sessionID, const std::string &modelID, TableModelEntry &tables) const {
    const DicTableModels::const_iterator it = _table_models.find( sessionID + modelID);
    if ( it != _table_models.end())
      tables = it->second;
    return ( it != _table_models.end());
  }

  inline std::string HBase::createRowKey( const std::string modelID, const std::string &analysisID, const double stepValue) {
    const size_t tmp_buf_size = 256;
    char tmp_buf[ tmp_buf_size];
    std::string modelID_hex( ModelID_DoHexStringConversionIfNecesary( modelID, tmp_buf, tmp_buf_size));
    size_t analysis_length = analysisID.length();
    // needs to be swapped !!!!!!!!
    std::string length_hex( toHexStringSwap< int>( ( int)analysis_length));
    std::string step_hex( toHexStringSwap< double>( stepValue));
    return ( analysis_length ? ( modelID_hex + length_hex + analysisID + step_hex) : ( modelID_hex + length_hex + step_hex));
  }
  inline std::string HBase::createRowKeyPrefix( const std::string modelID, const std::string &analysisID) {
    const size_t tmp_buf_size = 256;
    char tmp_buf[ tmp_buf_size];
    std::string modelID_hex( ModelID_DoHexStringConversionIfNecesary( modelID, tmp_buf, tmp_buf_size));
    size_t analysis_length = analysisID.length();
    // needs to be swapped !!!!!!!!
    std::string length_hex( toHexStringSwap< int>( ( int)analysis_length));
    return ( analysis_length ? ( modelID_hex + length_hex + analysisID) : ( modelID_hex + length_hex));
  }


} // namespace VELaSSCo

void printRow(const TRowResult &rowResult);
