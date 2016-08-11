
// CLib
#include <cstdlib>

// STL
#include <iostream>

// VELaSSCo
#include "Client.h"
#include "Helpers.h"
#include "Extras.h" // strcasestr

using namespace VELaSSCo;


Client::Client()
{
  PING; // seems not to work in Linux ...
  DEBUG( __FUNCTION__);
  SetDefaultCompression();
}

Client::~Client()
{
  PING; // seems not to work in Linux ...
  DEBUG( __FUNCTION__);
}

VAL_Result Client::UserLogin( /* in */
			     const std::string &url,
			     const std::string &name,
			     const std::string &password,

			     /* out */
			     VAL_SessionID &sessionID )
{
  PING; // seems not to work in Linux ...
  DEBUG( __FUNCTION__);

  // Parse server URL
  URL my_url( url);
  const std::string host = my_url.Host();
  const int         port = atoi(my_url.Port().c_str());

  // Do we have a valid host and port
  if (host == "" || port == 0)
    return VAL_WRONG_URL;

  VAL_Result ret =  VAL_UNKNOWN_ERROR;

  try
    {
      m_socket    = boost::shared_ptr<TTransport>(new TSocket(host, port));
      //m_transport = boost::shared_ptr<TTransport>(new TBufferedTransport(m_socket));
		
		
		
      // AUTHOR: iCores. This code allows multi-user and single-user behaviour.
      bool multiUserSupport = false;
      char *valueEnvV = NULL;
      valueEnvV = getenv("MULTIUSER_VELASSCO");
      if (valueEnvV)
	{
	  if (atoi(valueEnvV))
	    {
	      LOGGER << " WARNING: Multiuser support enabled. Under development ... \n" << std::endl;		
	      multiUserSupport =  true;
	    }
	}	

      // AUTHOR: Miguel Pasenau. Also allowing passing multiuser as parameter, for visualization plug-ins and user's who 
      // don't know if server is single or multi-user.
      // in AccessLib.h:   
      // * url may end have this parameter '?multiuser' for a multi-user connection
      // This does not work on MS VisualStudio 2012, after this, parameters is "", but my_url.Query().c_str() is "multiuser"
      // const char *parameters = my_url.Query().c_str();
      if ( my_url.Query().c_str() && *(my_url.Query().c_str())) {
	const char *keyword = "multiuser";
	const char *multiuser_flag = strcasestr( my_url.Query().c_str(), keyword);
	if ( multiuser_flag) {
	  LOGGER << " WARNING: Multiuser support enabled. Under development ... \n" << std::endl;		
	  multiUserSupport =  true;
	}
      }

      if (multiUserSupport)
	{
	  //New behaviour. Multiple users per port.
	  m_transport = boost::shared_ptr<TTransport>(new TFramedTransport(m_socket));
	}
      else
	{
	  //Old behaviour. Sigle user per port.
	  m_transport = boost::shared_ptr<TTransport>(new TBufferedTransport(m_socket));
	}
      // END: AUTHOR: iCores. This code allows multi-user and single-user behaviour.
		
		
		
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

      ret = (VAL_Result)(userLoginResult.result);
    } 
  catch (TException& tx)
    {
      LOGGER << FUNCTION_NAME << "EXCEPTION ERROR: " << tx.what() << std::endl;
      ret = VAL_SYSTEM_NOT_AVAILABLE;
    }
  catch (...)
    {
      LOGGER << FUNCTION_NAME << "EXCEPTION ERROR: undefined" << std::endl;
      ret = VAL_UNKNOWN_ERROR;
    }

  return ret;
}

VAL_Result Client::UserLogout( /* in */
			      VAL_SessionID sessionID )
{
  PING; // seems not to work in Linux ...
  DEBUG( __FUNCTION__);

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
      LOGGER << FUNCTION_NAME << " ERROR: " << tx.what() << std::endl;
      return VAL_SYSTEM_NOT_AVAILABLE;
    }
  catch (...)
    {
      LOGGER << FUNCTION_NAME << "EXCEPTION ERROR: undefined" << std::endl;
      return VAL_UNKNOWN_ERROR;
    }
}

VAL_Result Client::Query( /* in */
			 SessionID          sessionID,
			 const std::string &query,
			 
			 /* out */
			 const std::string* &data )
{
  PING; // seems not to work in Linux ...
  DEBUG( __FUNCTION__);

  try
    {
      m_queryResult.data.clear();
      // too long queries in the log
      // LOGGER << "Doing " << query << std::endl;
      LOGGER << "Doing " << query.substr( 0, 1000) << 
	( ( query.length() > 1000) ? "..." : "") << std::endl;
      m_transport->open();
      m_client->Query(m_queryResult, sessionID, query);
      m_transport->close();

      LOGGER << "Query_Result: "                                      << std::endl;
      LOGGER << "    result : "   << m_queryResult.result             << std::endl;
      // LOGGER << "    data   : \n" << Hexdump(m_queryResult.data, 128) << std::endl;
      // LOGGER << "    data   : \n" << Hexdump(m_queryResult.data, 32) << std::endl;

      size_t num_bytes = sizeof( m_queryResult.result) + m_queryResult.data.size();
      LOGGER << "    --> result size: " << GetNiceSizeString( num_bytes) << " received." << std::endl;

      // Return pointer to result string (Thrift uses std::string)
      // uncompress the data:
      if ( m_compression_enabled && ( m_queryResult.data.size() > 0)) {
	std::string *uncompressed_result = NULL;
	bool ok = m_compression.doUncompress( m_queryResult.data, &uncompressed_result);
	LOGGER << "    --> uncompressed with " << ( ok ? "no error" : "ERROR") << std::endl;
	if ( uncompressed_result) {
	  LOGGER << "        to " << GetNiceSizeString( uncompressed_result->size())
		 << " from " << GetNiceSizeString( m_queryResult.data.size() - 12) << std::endl; // header
	  m_queryResult.data = *uncompressed_result;
	  delete uncompressed_result;
	  uncompressed_result = NULL;
	  LOGGER << "    uncdata: \n" << Hexdump(m_queryResult.data, 128) << std::endl;
	}
      }
      data = &(m_queryResult.data);
      return (VAL_Result)(m_queryResult.result);
    }
  catch (TException& tx)
    {
      LOGGER << FUNCTION_NAME << " ERROR: " << tx.what() << std::endl;
      return VAL_SYSTEM_NOT_AVAILABLE;
    }
  catch (...)
    {
      LOGGER << FUNCTION_NAME << "EXCEPTION ERROR: undefined" << std::endl;
      return VAL_UNKNOWN_ERROR;
    }
}

/**
 * GetStatusDB
 */
VAL_Result Client::GetStatusDB( /* in */
			       SessionID          sessionID,
			       /* out */
			       const std::string* &status ) {

  PING; // seems not to work in Linux ...
  DEBUG( __FUNCTION__);
  
  try
    {
      m_transport->open();
      m_client->GetStatusDB( m_getStatusDBResult, sessionID); // using a object's property so that the returned pointers are valid, as they point to the objects data...
      m_transport->close();
      
      LOGGER << "StatusDB_Result: "                                 << std::endl;
      LOGGER << "        result : " << m_getStatusDBResult.result        << std::endl;
      LOGGER << "        status : " << m_getStatusDBResult.status << std::endl;
      
      // Return pointer to result string (Thrift uses std::string)
      status = &(m_getStatusDBResult.status);
      
      return (VAL_Result)(m_getStatusDBResult.result);
    }
  catch (TException& tx)
    {
      LOGGER << FUNCTION_NAME << " ERROR: " << tx.what() << std::endl;
      return VAL_SYSTEM_NOT_AVAILABLE;
    }
  catch (...)
    {
      LOGGER << FUNCTION_NAME << "EXCEPTION ERROR: undefined" << std::endl;
      return VAL_UNKNOWN_ERROR;
    }
  
}


VAL_Result Client::StopVELaSSCo(  /* in */
				SessionID   sessionID,
				/* out */ 
				const std::string *&status) {

  PING; // seems not to work in Linux ...
  DEBUG( __FUNCTION__);
  
  try
    {
      m_transport->open();
      m_client->StopVELaSSCo( m_stopVELaSSCoResult, sessionID); // using a object's property so that the returned pointers are valid, as they point to the objects data...
      m_transport->close();
      
      LOGGER << "StopVELaSSCo_Result: "                                 << std::endl;
      LOGGER << "        result : " << m_stopVELaSSCoResult.result        << std::endl;
      LOGGER << "        status : " << m_stopVELaSSCoResult.status << std::endl;
      
      // Return pointer to result string (Thrift uses std::string)
      status = &(m_stopVELaSSCoResult.status);
      
      return (VAL_Result)(m_stopVELaSSCoResult.result);
    }
  catch (TException& tx)
    {
      LOGGER << FUNCTION_NAME << " ERROR: " << tx.what() << std::endl;
      return VAL_SYSTEM_NOT_AVAILABLE;
    }
  catch (...)
    {
      LOGGER << FUNCTION_NAME << "EXCEPTION ERROR: undefined" << std::endl;
      return VAL_UNKNOWN_ERROR;
    }
  
}
