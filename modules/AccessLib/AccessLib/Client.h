
#pragma once

// STL
#include <string>

// Thrift
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

// VELaSSCo
#include "AccessLib.h"

// Generated code
#include "../../thrift/QueryManager/gen-cpp/QueryManager.h"

#include "Compression.h"

namespace VELaSSCo
{

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class Client
{
public:

	Client();
	virtual ~Client();

	/**
     * User login
     */
	VAL_Result UserLogin( /* in */
	                      const std::string &url,
						  const std::string &name,
						  const std::string &password,

						  /* out */
						  VAL_SessionID &sessionID );

    /**
     * User logout
     */
	VAL_Result UserLogout( /* in */
						   VAL_SessionID sessionID );
	/**
     * Query
     */
	VAL_Result Query( /* in */ 
					  SessionID          sessionID,
					  const std::string &query,

					  /* out */
					  const std::string* &data );

	/**
	 * GetStatusDB
	 */
	VAL_Result GetStatusDB( /* in */ 
			       SessionID          sessionID,
			       /* out */
			       const std::string *&status );
	VAL_Result StopVELaSSCo(  /* in */
				SessionID   sessionID,
				/* out */ 
				const std::string *&status);

	void SetDefaultCompression() {
	  m_compression.setCompressionType( VL_Compression::CompressionType::Zlib);
	  m_compression.setCompressionLevel( 1);
	  m_compression.setCompressionThreshold( VL_COMPRESSION_MINIMUM_DATA_SIZE_TO_COMPRESS);
	  m_compression_enabled = false; // compression disabled until enabled explicitly by the AccessLib
	}
	void SetCompressionEnabledFlag( bool flag) { m_compression_enabled = flag;}
	bool GetCompressionEnabledFlag() const { return m_compression_enabled;}
private:

	boost::shared_ptr<TTransport> m_socket;
	boost::shared_ptr<TTransport> m_transport;
	boost::shared_ptr<TProtocol>  m_protocol;

	boost::shared_ptr<QueryManagerClient> m_client;
	
	Query_Result m_queryResult;
	StatusDB_Result m_getStatusDBResult;
	StopVELaSSCo_Result m_stopVELaSSCoResult;
	// handles compression between QM and AL
	// at the moment the AL only uncompresses the data from QM
	VL_Compression m_compression;
	bool m_compression_enabled;
};

}
