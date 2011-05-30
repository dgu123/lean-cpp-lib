/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_WIN_ERRORS
#define LEAN_LOGGING_WIN_ERRORS

#include "../lean.h"
#include "../strings/types.h"
#include "../strings/conversions.h"
#include <string>
#include <sstream>

namespace lean
{
namespace logging
{

/// Logs the last WinAPI error.
LEAN_MAYBE_EXPORT void log_last_win_error(const char *source);
/// Logs the last WinAPI error.
LEAN_MAYBE_EXPORT void log_last_win_error(const char *source, const char *reason);
/// Logs the last WinAPI error.
LEAN_MAYBE_EXPORT void log_last_win_error(const char *source, const char *reason, const char *context);

/// Throws a runtime_error exception containing the last WinAPI error.
LEAN_MAYBE_EXPORT void throw_last_win_error(const char *source);
/// Throws a runtime_error exception containing the last WinAPI error.
LEAN_MAYBE_EXPORT void throw_last_win_error(const char *source, const char *reason);
/// Throws a runtime_error exception containing the last WinAPI error.
LEAN_MAYBE_EXPORT void throw_last_win_error(const char *source, const char *reason, const char *context);

/// Gets an error message describing the last WinAPI error that occurred. Returns the number of characters used.
LEAN_MAYBE_EXPORT size_t get_last_win_error_msg(utf16_t *buffer, size_t maxCount);
/// Gets an error message describing the last WinAPI error that occurred.
inline utf16_string get_last_win_error_msg()
{
	utf16_string msg;
	msg.resize(1024);
	msg.erase(get_last_win_error_msg(&msg[0], msg.size()));
	return msg;
}

/// Gets an error message describing the last WinAPI error that occurred.
template <class String>
String get_last_win_error_msg();

#ifndef DOXYGEN_SKIP_THIS

/// Gets an error message describing the last WinAPI error that occurred.
template <>
inline utf8_string get_last_win_error_msg()
{
	return utf_to_utf8(get_last_win_error_msg());
}
/// Gets an error message describing the last WinAPI error that occurred.
template <>
inline utf16_string get_last_win_error_msg()
{
	return get_last_win_error_msg();
}

#endif

/// Logs the last WinAPI error.
template <class String1>
inline void log_last_win_error(const String1 &source)
{
	log_last_win_error(utf_to_utf8(source).c_str());
}
/// Logs the last WinAPI error.
template <class String1, class String2>
inline void log_last_win_error(const String1 &source, const String2 &reason)
{
	log_last_win_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str());
}
/// Logs the last WinAPI error.
template <class String1, class String2, class String3>
inline void log_last_win_error(const String1 &source, const String2 &reason, const String3 &context)
{
	log_last_win_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(context).c_str());
}

/// Throws a runtime_error exception containing the last WinAPI error.
template <class String1>
inline void throw_last_win_error(const String1 &source)
{
	throw_last_win_error(utf_to_utf8(source).c_str());
}
/// Throws a runtime_error exception containing the last WinAPI error.
template <class String1, class String2>
inline void throw_last_win_error(const String1 &source, const String2 &reason)
{
	throw_last_win_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str());
}
/// Throws a runtime_error exception containing the last WinAPI error.
template <class String1, class String2, class String3>
inline void throw_last_win_error(const String1 &source, const String2 &reason, const String3 &context)
{
	throw_last_win_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(context).c_str());
}

} // namespace

using logging::log_last_win_error;
using logging::throw_last_win_error;
using logging::get_last_win_error_msg;

} // namespace

/// @addtogroup ExceptionMacros
/// @see lean::logging
/// @{

/// Logs an error message, prepending the caller's file and line.
#define LEAN_LOG_WIN_ERROR() ::lean::logging::log_last_win_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")")
/// Logs the given error message, prepending the caller's file and line.
#define LEAN_LOG_WIN_ERROR_MSG(msg) ::lean::logging::log_last_win_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg)
/// Logs the given error message and context, prepending the caller's file and line.
#define LEAN_LOG_WIN_ERROR_CTX(msg, ctx) ::lean::logging::log_last_win_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg, ctx)
/// Logs the given error message and context, prepending the caller's file and line.
#define LEAN_LOG_WIN_ERROR_ANY(msg) LEAN_THROW_WIN_ERROR_MSG(static_cast<::std::ostringstream&>(::std::ostringstream() << msg).str().c_str())

/// Throws a runtime_error exception.
#define LEAN_THROW_WIN_ERROR() ::lean::logging::throw_last_win_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")")
/// Throws a runtime_error exception.
#define LEAN_THROW_WIN_ERROR_MSG(msg) ::lean::logging::throw_last_win_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg)
/// Throws a runtime_error exception.
#define LEAN_THROW_WIN_ERROR_CTX(msg, ctx) ::lean::logging::throw_last_win_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg, ctx)
/// Throws a runtime_error exception.
#define LEAN_THROW_WIN_ERROR_ANY(msg) LEAN_THROW_WIN_ERROR_MSG(static_cast<::std::ostringstream&>(::std::ostringstream() << msg).str().c_str())

/// @}

#ifdef LEAN_INCLUDE_LINKED
#include "source/win_errors.cpp"
#endif

#endif