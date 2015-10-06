
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
#include "../Thrift/gen-cpp/QueryManager.h"


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
			       const std::string* &status );

private:

	boost::shared_ptr<TTransport> m_socket;
	boost::shared_ptr<TTransport> m_transport;
	boost::shared_ptr<TProtocol>  m_protocol;

	boost::shared_ptr<QueryManagerClient> m_client;
	
	Query_Result m_queryResult;
	StatusDB_Result m_getStatusDBResult;
};

}
