/* -*- c++ -*- */
#ifndef _ACCESS_LIB_COMMON_H_
#define _ACCESS_LIB_COMMON_H_

// STL
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// VELaSSCo
#include "AccessLib.h"
#include "Client.h"
#include "Helpers.h"
#include "DemoServer.h" // for the StartServer() function

namespace VELaSSCo
{

typedef std::map< VAL_SessionID, boost::shared_ptr<VELaSSCo::Client> > ClientMap;

}

// declared in AccessLibCommon.cpp
extern VELaSSCo::ClientMap g_clients;
extern std::ofstream       g_apiTraceFile;

// defines an AutoTimer object that times an API call and writes the result to the trace file
#define API_TRACE                                                                                                                                                    \
	VELaSSCo::AutoTimer autoTimer(                                                                                                                                   \
		std::string(FUNCTION_NAME).substr(std::string(FUNCTION_NAME).find("val"), std::string(FUNCTION_NAME).find("(")-std::string(FUNCTION_NAME).find("val"))+"; ", \
		g_apiTraceFile                                                                                                                                               \
	)

#define CHECK_SESSION_ID( id )                                       \
	do                                                               \
	{                                                                \
		const VELaSSCo::ClientMap::iterator it = g_clients.find(id); \
		if (it == g_clients.end())                                   \
			return VAL_INVALID_SESSION_ID;                           \
	}                                                                \
	while (0)

#define CHECK_QUERY_POINTER( ptr )               \
	do                                           \
	{                                            \
		if (ptr == NULL)                         \
			return VAL_INVALID_QUERY_PARAMETERS; \
	}                                            \
	while (0)

#define CHECK_NON_ZERO_VALUE( val )               \
	do                                           \
	{                                            \
		if (val == 0)                         \
			return VAL_INVALID_QUERY_PARAMETERS; \
	}                                            \
	while (0)

#define CHECK_VALUE( val, cte )			     \
	do                                           \
	{                                            \
		if (val == cte)                         \
			return VAL_INVALID_QUERY_PARAMETERS; \
	}                                            \
	while (0)

#define CATCH_ERROR               \
    catch (...)                   \
	{                             \
	        DEBUG( "CATCH_ERROR");	  \
		return VAL_UNKNOWN_ERROR; \
	}                             \

#endif // _ACCESS_LIB_COMMON_H_
