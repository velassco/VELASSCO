
// CLib
#include <cstdlib>

// VELaSSCo
#include "AccessLibCommon.h"

/// Our client objects. One per session ID.
VELaSSCo::ClientMap g_clients;

/// A CSV file that provides a table of api call statistics
std::ofstream g_apiTraceFile;

/// AccessLib initialization
class AccessLibMain
{
public:
	AccessLibMain()
	{
		const char* apiTraceFileName = getenv("VAL_API_TRACE_FILE");
		if (apiTraceFileName)
		{
			g_apiTraceFile.open(apiTraceFileName);
			g_apiTraceFile << "FunctionName; ExecutionTime[sec]" << std::endl;
		}
	}
};

static AccessLibMain main;
