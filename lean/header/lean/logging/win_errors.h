/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_WIN_ERRORS
#define LEAN_LOGGING_WIN_ERRORS

#include "../lean.h"
#include <string>
#include <sstream>

namespace lean
{
namespace logging
{

/// Throws a runtime_error exception containing the last WinAPI error.
LEAN_MAYBE_EXPORT void throw_last_win_error(const char *source);
/// Throws a runtime_error exception containing the last WinAPI error.
LEAN_MAYBE_EXPORT void throw_last_win_error(const char *source, const char *reason);
/// Throws a runtime_error exception containing the last WinAPI error.
LEAN_MAYBE_EXPORT void throw_last_win_error(const char *source, const char *reason, const char *context);

/// Gets an error message describing the last WinAPI error that occurred. Returns the number of characters used.
LEAN_MAYBE_EXPORT size_t get_last_win_error_msg(char *buffer, size_t maxCount);
/// Gets an error message describing the last WinAPI error that occurred.
inline std::string get_last_win_error_msg()
{
	std::string msg(1024, 0);
	msg.erase(get_last_win_error_msg(&msg[0], msg.size()));
	return msg;
}

} // namespace

using logging::throw_last_win_error;
using logging::get_last_win_error_msg;

} // namespace

/// @addtogroup ExceptionMacros
/// @see lean::logging
/// @{

/// Throws a runtime_error exception.
#define LEAN_THROW_WIN_ERROR() ::lean::logging::throw_last_win_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")")
/// Throws a runtime_error exception.
#define LEAN_THROW_WIN_ERROR_MSG(msg) ::lean::logging::throw_last_win_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg)
/// Throws a runtime_error exception.
#define LEAN_THROW_WIN_ERROR_CTX(msg, ctx) ::lean::logging::throw_last_win_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg, ctx)
/// Throws a runtime_error exception.
#define LEAN_THROW_WIN_ERROR_LOG(msg) LEAN_THROW_WIN_ERROR_MSG(static_cast<::std::ostringstream&>(::std::ostringstream() << msg).str().c_str())

/// @}

#ifndef LEAN_LINKING
#include "source/win_errors.cpp"
#endif

#endif