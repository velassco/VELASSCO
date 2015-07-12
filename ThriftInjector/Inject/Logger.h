#ifndef __Logger_h__
#define __Logger_h__

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace trivial = boost::log::trivial;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

using namespace logging::trivial;

extern boost::log::sources::severity_logger< severity_level > __logger;

#define LOG(severity) \
    BOOST_LOG_SEV(__logger, severity)

#define LOG_EXT(severity) \
    BOOST_LOG_SEV(__logger, severity) << "(" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ") "

void init_logger( );

#endif
