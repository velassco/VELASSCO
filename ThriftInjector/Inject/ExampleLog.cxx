//Link: -lpthread -lboost_log -lboost_system -lboost_thread

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>

using namespace std;

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace trivial = boost::log::trivial;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;


#define LOG(logger, severity) \
    BOOST_LOG_SEV(logger, severity) << "(" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ") "

int main()
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

    using namespace logging::trivial;
    src::severity_logger< severity_level > lg;

    LOG(lg, warning) << "Hello world";
}
