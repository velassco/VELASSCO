
// ***
// *** Whithin the VELaSSCo AccessLib this server is only for testing purposes.
// ***

// STL
#include <sstream>

// Thrift
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

// Boost
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

// VELaSSCo
#include "AccessLib.h"
#include "Helpers.h"

// Generated code
#include "../Thrift/gen-cpp/QueryManager.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCo;


// ***************************************************************************
//
// Actual implementation of QueryManager functions.
//
// This is a derived class, so that our own code does not get overwritten
// when the Thrift interface needs to be regenerated.
//
// ***************************************************************************

class QueryManager : virtual public QueryManagerIf
{
	struct User
	{
		std::string loginName;
		std::string loginTime;
	};

	typedef std::map<SessionID, User> UserMap;
	UserMap m_users;

	bool ValidSessionID(SessionID sessionID)
	{
		const UserMap::iterator it = m_users.find(sessionID);
		return (it != m_users.end());
	}

	void UserLogin(UserLogin_Result& _return, const std::string& url, const std::string& name, const std::string& password)
	{
		LOGGER                              << std::endl;
		LOGGER << "----- UserLogin() -----" << std::endl;

		LOGGER                                << std::endl;
		LOGGER << "Input:"                    << std::endl;
		LOGGER << "  url      : " << url      << std::endl;
		LOGGER << "  name     : " << name     << std::endl;
		LOGGER << "  password : " << password << std::endl;

		if (name == "andreas" && password == "1234")
		{
			// Login time
			std::string time = boost::posix_time::to_simple_string(boost::posix_time::second_clock::universal_time());

			// Generate a session id randomly
			SessionID sessionID;
			do
			{
				const boost::uuids::uuid uuid = boost::uuids::random_generator()();
				sessionID = *((SessionID*)(uuid.data));
			}
			while (ValidSessionID(sessionID));

			// Store user
			User user = { name, time };
			m_users[sessionID] = user;

			// Return session id
			_return.__set_result( (Result::type)VAL_SUCCESS );
			_return.__set_sessionID( sessionID );
		}		
		else
		{
			_return.__set_result( (Result::type)VAL_USER_NOT_ACCEPTED );
			_return.__set_sessionID( 0 );
		}

		LOGGER                                          << std::endl;
		LOGGER << "Output:"                             << std::endl;
		LOGGER << "  result    : " << _return.result    << std::endl;
		LOGGER << "  sessionID : " << _return.sessionID << std::endl;
	}

	void UserLogout(UserLogout_Result& _return, const SessionID sessionID)
	{
		LOGGER                               << std::endl;
		LOGGER << "----- UserLogout() -----" << std::endl;

		LOGGER                                  << std::endl;
		LOGGER << "Input:"                      << std::endl;
		LOGGER << "  sessionID : " << sessionID << std::endl;

		// Check session ID
		if (!ValidSessionID(sessionID))
		{
			_return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );
			return;
		}

		// Remove session ID
		{
			m_users.erase(sessionID);
		}

		_return.__set_result( (Result::type)VAL_SUCCESS );
	}

	void Query(Query_Result& _return, const SessionID sessionID, const std::string& query)
	{
		LOGGER                          << std::endl;
		LOGGER << "----- Query() -----" << std::endl;

		LOGGER                                    << std::endl;
		LOGGER << "Input:"                        << std::endl;
		LOGGER << "  sessionID : "   << sessionID << std::endl;
		LOGGER << "  query     : \n" << query     << std::endl;

		// Check session ID
		if (!ValidSessionID(sessionID))
		{
			_return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );
			return;
		}

		// Parse query JSON
		std::string name;
		std::istringstream ss(query);
		boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);
		for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it)
			if (it->first == "name")
				name = it->second.get_value<std::string>();

		// Check query name
		if (name != "GetResultFromVerticesID")
		{
			_return.__set_result( (Result::type)VAL_INVALID_QUERY_PARAMETERS );
			return;
		}

		// Generate example result data
		{
			std::vector<int64_t> resultVertexIDs;
			std::vector<double>  resultValues;
  
			// vertex #1
			resultVertexIDs.push_back(6);
			resultValues.push_back(-0.105564);
			resultValues.push_back(-0.287896);
			resultValues.push_back(-0.377642);
			// vertex #2
			resultVertexIDs.push_back(7);
			resultValues.push_back( 0.259839);
			resultValues.push_back(-0.366375);
			resultValues.push_back(-0.377652);

			// Pack into string
			std::string result;
			result += "2 3\n";
			result += std::string((char*)(&resultVertexIDs[0]), sizeof(int64_t)*resultVertexIDs.size());
			result += std::string((char*)(&resultValues[0]),    sizeof(double)*resultValues.size());
			_return.__set_data(result); 
		}

		_return.__set_result( (Result::type)VAL_SUCCESS );
	}
};

int StartServer() {
  LOGGER << "Starting VELaSSCo Server..." << std::endl;

  int port = 9090;
  LOGGER << "  using port: " << port << std::endl;

  shared_ptr<QueryManager> handler(new QueryManager());
  shared_ptr<TProcessor> processor(new QueryManagerProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}
