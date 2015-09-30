
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
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

// VELaSSCo
#include "AccessLib.h"
#include "Helpers.h"

// Generated code
#include "../../thrift/QueryManager/gen-cpp/QueryManager.h"

#include "queryManagerModule.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::VELaSSCo;

template <typename T>
std::vector<T> as_vector(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
{
    std::vector<T> result;
    
    BOOST_FOREACH(boost::property_tree::ptree::value_type it, pt.get_child(key)) {
        result.push_back(it.second.get_value<T>());
    }
    
    return result;
}

template <typename T>
std::string as_string(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
{
    std::stringstream ss;

    BOOST_FOREACH(boost::property_tree::ptree::value_type it, pt.get_child(key)) {
		ss << (ss.str().size()?",":"") << it.second.get_value<T>();
    }
    
    return std::string("[") + ss.str() + "]";
}

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

		if (   (name == "velassco" && password == "VELaSSCo")
		    || (name == "andreas" && password == "1234") )
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

			LOGGER                                    << std::endl;
			LOGGER << "Output:"                       << std::endl;
			LOGGER << "  result : " << _return.result << std::endl;

			return;
		}

		// Remove session ID
		{
			m_users.erase(sessionID);
		}

		_return.__set_result( (Result::type)VAL_SUCCESS );

		LOGGER                                    << std::endl;
		LOGGER << "Output:"                       << std::endl;
		LOGGER << "  result : " << _return.result << std::endl;
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

			LOGGER                                    << std::endl;
			LOGGER << "Output:"                       << std::endl;
			LOGGER << "  result : " << _return.result << std::endl;

			return;
		}

		// Parse query JSON
		std::istringstream ss(query);
		boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);

		std::string name       = pt.get<std::string>("name");
		std::string modelID    = pt.get<std::string>("modelID");
		std::string resultID   = pt.get<std::string>("resultID");
		std::string analysisID = pt.get<std::string>("analysisID");
		std::string vertexIDs  = as_string<size_t>(pt, "vertexIDs");
		double      timeStep   = pt.get<double>("timeStep");

		// Check query name
		if (name != "GetResultFromVerticesID")
		{
			_return.__set_result( (Result::type)VAL_INVALID_QUERY_PARAMETERS );

			LOGGER                                    << std::endl;
			LOGGER << "Output:"                       << std::endl;
			LOGGER << "  result : " << _return.result << std::endl;

			return;
		}
	
	    stringstream listOfVertices;
        listOfVertices << "{\"id\":" << "[]" /*vertexIDs*/ << "}";
        
        std::stringstream sessionIDStr;
        sessionIDStr << sessionID;
 		
		std::string _return_;

		std::cout << "S " << sessionID  << std::endl;
		std::cout << "M " << modelID    << std::endl;
		std::cout << "R " << resultID   << std::endl;
		std::cout << "A " << analysisID << std::endl;
		std::cout << "V " << vertexIDs  << std::endl;
		std::cout << "T " << timeStep   << std::endl;

        queryManagerModule::Instance()->getResultFormVerticesID(_return_ ,sessionIDStr.str() ,modelID ,analysisID ,timeStep ,resultID ,listOfVertices.str());

		std::vector<int64_t> resultVertexIDs;
		std::vector<double>  resultValues;

		std::cout << _return_ << std::endl;

		std::istringstream iss(_return_);
		while (iss)
		{
			int64_t id;
			double  value[3];
			
			std::string line;
			std::getline(iss, line);
			std::istringstream(line) >> id >> value[0] >> value[1] >> value[2];
			
			resultVertexIDs.push_back(id);
			resultValues.push_back(value[0]);
			resultValues.push_back(value[1]);
			resultValues.push_back(value[2]);
		}

		// Pack into string
		std::string result;
		std::ostringstream oss;
		oss << (resultValues.size() / 3) << " 3" << "\n";
		result += oss.str();
		result += std::string((char*)(&resultVertexIDs[0]), sizeof(int64_t)*resultVertexIDs.size());
		result += std::string((char*)(&resultValues[0]),    sizeof(double)*resultValues.size());
		_return.__set_data(result); 

		// Generate example result data
		if (0)
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

		LOGGER                                             << std::endl;
		LOGGER << "Output:"                                << std::endl;
		LOGGER << "  result : "   << _return.result        << std::endl;
		LOGGER << "  data   : \n" << Hexdump(_return.data) << std::endl;
	}

	void GetStatusDB(StatusDB_Result& _return, const SessionID sessionID) {
		LOGGER                              << std::endl;
		LOGGER << "----- GetStatusDB() -----" << std::endl;

		LOGGER                                << std::endl;
		LOGGER << "Input:"                        << std::endl;
		LOGGER << "  sessionID : "   << sessionID << std::endl;

		// Check session ID
		if (!ValidSessionID(sessionID))
		{
			_return.__set_result( (Result::type)VAL_INVALID_SESSION_ID );

			LOGGER                                    << std::endl;
			LOGGER << "Output:"                       << std::endl;
			LOGGER << "  result : " << _return.result << std::endl;

			return;
		}

		  std::string status;
		  queryManagerModule::Instance()->getStatusDB( status);
		  _return.__set_result( (Result::type)VAL_SUCCESS );
		  _return.__set_status( status );

		LOGGER                                          << std::endl;
		LOGGER << "Output:"                             << std::endl;
		LOGGER << "  result    : " << _return.result    << std::endl;
		LOGGER << "  status    : " << _return.status << std::endl;
	}
};

int StartServer( const int server_port) {
  LOGGER << "Starting VELaSSCo Server..." << std::endl;

  int port = server_port;
  LOGGER << "  using port: " << port << std::endl;

  boost::shared_ptr<QueryManager> handler(new QueryManager());
  boost::shared_ptr<TProcessor> processor(new QueryManagerProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    DEBUG( "  before serving ...");
  server.serve();
    DEBUG( "  after serving ...");
  return 0;
}
