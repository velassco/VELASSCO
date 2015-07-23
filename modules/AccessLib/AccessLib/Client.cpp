
// CLib
#include <cstdlib>

// STL
#include <iostream>

// VELaSSCo
#include "Client.h"
#include "Helpers.h"

using namespace VELaSSCo;


Client::Client()
{
	PING;
}

Client::~Client()
{
	PING;
}

VAL_Result Client::UserLogin( /* in */
	                          const std::string &url,
					          const std::string &name,
					          const std::string &password,

							  /* out */
					          VAL_SessionID &sessionID )
{
	PING;

	// Parse server URL
	const std::string host = URL(url).Host();
	const int         port = atoi(URL(url).Port().c_str());

	// Do we have a valid host and port
	if (host == "" || port == 0)
		return VAL_WRONG_URL;

	try
	{
		m_socket    = boost::shared_ptr<TTransport>(new TSocket(host, port));
		m_transport = boost::shared_ptr<TTransport>(new TBufferedTransport(m_socket));
		m_protocol  = boost::shared_ptr<TProtocol>(new TBinaryProtocol(m_transport));

		m_client = boost::shared_ptr<QueryManagerClient>(new QueryManagerClient(m_protocol));

		UserLogin_Result userLoginResult;
		m_transport->open();
		m_client->UserLogin(userLoginResult, url, name, password);
		m_transport->close();

		LOGGER << "UserLogin_Result: "                            << std::endl;
		LOGGER << "    result    : " << userLoginResult.result    << std::endl;
		LOGGER << "    sessionID : " << userLoginResult.sessionID << std::endl;

		sessionID = userLoginResult.sessionID;

		return (VAL_Result)(userLoginResult.result);
	} 
	catch (TException& tx)
	{
		LOGGER << "ERROR: " << tx.what() << std::endl;
		return VAL_SYSTEM_NOT_AVAILABLE;
	}
	catch (...)
	{
		return VAL_UNKNOWN_ERROR;
	}
}

VAL_Result Client::UserLogout( /* in */
					           VAL_SessionID sessionID )
{
	PING;

	try
	{
		UserLogout_Result userLogoutResult;
		m_transport->open();
		m_client->UserLogout(userLogoutResult, sessionID);
		m_transport->close();

		LOGGER << "UserLogout_Result: "                         << std::endl;
		LOGGER << "    result : " << userLogoutResult.result    << std::endl;

		return (VAL_Result)(userLogoutResult.result);
	} 
	catch (TException& tx)
	{
		LOGGER << "ERROR: " << tx.what() << std::endl;
		return VAL_SYSTEM_NOT_AVAILABLE;
	}
	catch (...)
	{
		return VAL_UNKNOWN_ERROR;
	}
}

VAL_Result Client::Query( /* in */
	                      SessionID          sessionID,
						  const std::string &query,

				          /* out */
				          const std::string* &data )
{
	PING;

	try
	{
		m_transport->open();
		m_client->Query(m_queryResult, sessionID, query);
		m_transport->close();

		LOGGER << "Query_Result: "                                 << std::endl;
		LOGGER << "    result : "   << m_queryResult.result        << std::endl;
		LOGGER << "    data   : \n" << Hexdump(m_queryResult.data) << std::endl;

		// Return pointer to result string (Thrift uses std::string)
		data = &(m_queryResult.data);

		return (VAL_Result)(m_queryResult.result);
	}
	catch (TException& tx)
	{
		LOGGER << "ERROR: " << tx.what() << std::endl;
		return VAL_SYSTEM_NOT_AVAILABLE;
	}
	catch (...)
	{
		return VAL_UNKNOWN_ERROR;
	}
}
