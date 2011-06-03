/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_EXCEPTIONS
#define LEAN_LOGGING_EXCEPTIONS

#include "../lean.h"
#include "../strings/types.h"
#include "../strings/conversions.h"
#include <sstream>
#include "streamconv.h"

namespace lean
{
namespace logging
{

/// Throws a runtime_error exception.
LEAN_MAYBE_EXPORT void throw_error(const char *source);
/// Throws a runtime_error exception.
LEAN_MAYBE_EXPORT void throw_error(const char *source, const char *reason);
/// Throws a runtime_error exception.
LEAN_MAYBE_EXPORT void throw_error(const char *source, const char *reason, const char *context);
/// Throws a runtime_error exception.
LEAN_MAYBE_EXPORT void throw_error_ex(const char *source, const char *reason, const char *origin);
/// Throws a runtime_error exception.
LEAN_MAYBE_EXPORT void throw_error_ex(const char *source, const char *reason, const char *origin, const char *context);

/// Throws an invalid_argument exception.
LEAN_MAYBE_EXPORT void throw_invalid(const char *source);
/// Throws an invalid_argument exception.
LEAN_MAYBE_EXPORT void throw_invalid(const char *source, const char *reason);

/// Throws a bad_alloc exception.
LEAN_MAYBE_EXPORT void throw_bad_alloc(const char *source);
/// Throws a bad_alloc exception.
LEAN_MAYBE_EXPORT void throw_bad_alloc(const char *source, size_t size);

/// Logs an error.
LEAN_MAYBE_EXPORT void log_error(const char *source);
/// Logs an error.
LEAN_MAYBE_EXPORT void log_error(const char *source, const char *reason);
/// Logs an error.
LEAN_MAYBE_EXPORT void log_error(const char *source, const char *reason, const char *context);
/// Logs an error.
LEAN_MAYBE_EXPORT void log_error_ex(const char *source, const char *reason, const char *origin);
/// Logs an error.
LEAN_MAYBE_EXPORT void log_error_ex(const char *source, const char *reason, const char *origin, const char *context);


/// Throws a runtime_error exception.
template <class String1>
inline void throw_error(const String1 &source)
{
	throw_error(utf_to_utf8(source).c_str());
}
/// Throws a runtime_error exception.
template <class String1, class String2>
inline void throw_error(const String1 &source, const String2 &reason)
{
	throw_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str());
}
/// Throws a runtime_error exception.
template <class String1, class String2, class String3>
inline void throw_error(const String1 &source, const String2 &reason, const String3 &context)
{
	throw_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(context).c_str());
}
/// Throws a runtime_error exception.
template <class String1, class String2, class String3>
inline void throw_error_ex(const String1 &source, const String2 &reason, const String3 &origin)
{
	throw_error_ex(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(origin).c_str());
}
/// Throws a runtime_error exception.
template <class String1, class String2, class String3, class String4>
inline void throw_error_ex(const String1 &source, const String2 &reason, const String3 &origin, const String4 &context)
{
	throw_error_ex(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(origin).c_str(), utf_to_utf8(context).c_str());
}

/// Throws an invalid_argument exception.
template <class String1>
inline void throw_invalid(const String1 &source)
{
	throw_invalid(utf_to_utf8(source).c_str());
}
/// Throws an invalid_argument exception.
template <class String1, class String2>
inline void throw_invalid(const String1 &source, const String2 &reason)
{
	throw_invalid(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str());
}


/// Logs an error.
template <class String1>
inline void log_error(const String1 &source)
{
	log_error(utf_to_utf8(source).c_str());
}
/// Logs an error.
template <class String1, class String2>
inline void log_error(const String1 &source, const String2 &reason)
{
	log_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str());
}
/// Logs an error.
template <class String1, class String2, class String3>
inline void log_error(const String1 &source, const String2 &reason, const String3 &context)
{
	log_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(context).c_str());
}
/// Logs an error.
template <class String1, class String2, class String3>
inline void log_error_ex(const String1 &source, const String2 &reason, const String3 &origin)
{
	log_error_ex(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(origin).c_str());
}
/// Logs an error.
template <class String1, class String2, class String3, class String4>
inline void log_error_ex(const String1 &source, const String2 &reason, const String3 &origin, const String4 &context)
{
	log_error_ex(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(origin).c_str(), utf_to_utf8(context).c_str());
}

} // namespace

using logging::throw_error;
using logging::throw_error_ex;
using logging::throw_invalid;
using logging::throw_bad_alloc;

using logging::log_error;
using logging::log_error_ex;

} // namespace

/// @addtogroup ExceptionMacros Exception macros
/// @see lean::logging
/// @{

/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR() ::lean::logging::throw_error(LEAN_SOURCE_STRING)
/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR_MSG(msg) ::lean::logging::throw_error(LEAN_SOURCE_STRING, msg)
/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR_CTX(msg, ctx) ::lean::logging::throw_error(LEAN_SOURCE_STRING, msg, ctx)
/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR_XMSG(msg, orig) ::lean::logging::throw_error_ex(LEAN_SOURCE_STRING, msg, orig)
/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR_XCTX(msg, orig, ctx) ::lean::logging::throw_error_ex(LEAN_SOURCE_STRING, msg, orig, ctx)
/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR_ANY(msg) LEAN_THROW_ERROR_MSG(static_cast<::std::ostringstream&>(::std::ostringstream() << msg).str().c_str())

/// Throws an invalid_argument exception.
#define LEAN_THROW_INVALID() ::lean::logging::throw_invalid(LEAN_SOURCE_STRING)
/// Throws an invalid_argument exception.
#define LEAN_THROW_INVALID_MSG(msg) ::lean::logging::throw_invalid(LEAN_SOURCE_STRING, msg)
/// Throws an invalid_argument exception.
#define LEAN_THROW_INVALID_ANY(msg) LEAN_THROW_INVALID_MSG(static_cast<::std::ostringstream&>(::std::ostringstream() << msg).str().c_str())

/// Throws a bad_alloc exception.
#define LEAN_THROW_BAD_ALLOC() ::lean::logging::throw_bad_alloc(LEAN_SOURCE_STRING)
/// Throws a bad_alloc exception.
#define LEAN_THROW_BAD_ALLOC_SIZE(size) ::lean::logging::throw_bad_alloc(LEAN_SOURCE_STRING, size)

/// @}


/// @addtogroup LoggingMacros
/// @see lean::logging
/// @{

/// Logs an error message, prepending the caller's file and line.
#define LEAN_LOG_ERROR_NIL() ::lean::logging::log_error(LEAN_SOURCE_STRING)
/// Logs the given error message, prepending the caller's file and line.
#define LEAN_LOG_ERROR_MSG(msg) ::lean::logging::log_error(LEAN_SOURCE_STRING, msg)
/// Logs the given error message and context, prepending the caller's file and line.
#define LEAN_LOG_ERROR_CTX(msg, ctx) ::lean::logging::log_error(LEAN_SOURCE_STRING, msg, ctx)
/// Logs the given error message and context, prepending the caller's file and line.
#define LEAN_LOG_ERROR_XMSG(msg, orig) ::lean::logging::log_error_ex(LEAN_SOURCE_STRING, msg, orig)
/// Logs the given error message and context, prepending the caller's file and line.
#define LEAN_LOG_ERROR_XCTX(msg, orig, ctx) ::lean::logging::log_error_ex(LEAN_SOURCE_STRING, msg, orig, ctx)

/// @}

#ifdef LEAN_INCLUDE_LINKED
#include "source/errors.cpp"
#endif

#endif