/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_LOG
#define LEAN_LOGGING_LOG

#include <iostream>
#include "../macros.h"

namespace lean
{
namespace logging
{

} // namespace

} // namespace

/// @addtogroup LoggingMacros Logging macros
/// @see lean::logging
/// @{

/// Logs the given message, prepending the caller's file and line.
#define LEAN_LOG(msg) ::std::clog << __FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) "): " << msg << ::std::endl

/// Logs the given error message, prepending the caller's file and line.
#define LEAN_LOG_ERROR(msg) ::std::cerr << __FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) "): " << msg << ::std::endl

/// @}

#endif