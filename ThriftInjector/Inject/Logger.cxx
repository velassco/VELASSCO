#include "Logger.h"

using namespace std;

boost::log::sources::severity_logger< severity_level > __logger;

void init_logger( )
{
  logging::add_file_log
    (
      keywords::file_name = "sample.log",
      // This makes the sink to write log records that look like this:
      // 1: <normal> A normal severity message
      // 2: <error> An error severity message
      keywords::format =
      (
        expr::stream
        << expr::attr< unsigned int >("LineID") << " |"
        << " [ " << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S") << "]"
        << ": <" << logging::trivial::severity << "> "
        << expr::smessage
        )
      );

  logging::add_console_log
    (
      std::clog,
      keywords::format =
      (
        expr::stream
        << expr::attr< unsigned int >("LineID") << " |"
        << " [ " << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S") << "]"
        << ": <" << logging::trivial::severity << "> "
        << expr::smessage
        )
      );
  
  logging::add_common_attributes();
}
