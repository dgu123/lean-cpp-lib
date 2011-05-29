/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_EXCEPTIONS
#define LEAN_LOGGING_EXCEPTIONS

#include "../lean.h"
#include "../strings/conversions.h"
#include <sstream>

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

/// Throws an invalid_argument exception.
LEAN_MAYBE_EXPORT void throw_invalid(const char *source);
/// Throws an invalid_argument exception.
LEAN_MAYBE_EXPORT void throw_invalid(const char *source, const char *reason);

/// Throws a bad_alloc exception.
LEAN_MAYBE_EXPORT void throw_bad_alloc(const char *source);
/// Throws a bad_alloc exception.
LEAN_MAYBE_EXPORT void throw_bad_alloc(const char *source, size_t size);

/// Throws a runtime_error exception.
template <class String1>
LEAN_INLINE void throw_error(const String1 &source)
{
	throw_error(utf_to_utf8(source).c_str());
}
/// Throws a runtime_error exception.
template <class String1, class String2>
LEAN_INLINE void throw_error(const String1 &source, const String2 &reason)
{
	throw_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str());
}
/// Throws a runtime_error exception.
template <class String1, class String2, class String3>
LEAN_INLINE void throw_error(const String1 &source, const String2 &reason, const String3 &context)
{
	throw_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(context).c_str());
}

/// Throws an invalid_argument exception.
template <class String1>
LEAN_INLINE void throw_invalid(const String1 &source)
{
	throw_invalid(utf_to_utf8(source).c_str());
}
/// Throws an invalid_argument exception.
template <class String1, class String2>
LEAN_INLINE void throw_invalid(const String1 &source, const String2 &reason)
{
	throw_invalid(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str());
}

} // namespace

using logging::throw_error;
using logging::throw_invalid;
using logging::throw_bad_alloc;

} // namespace

/// @addtogroup ExceptionMacros Exception macros
/// @see lean::logging
/// @{

/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR() ::lean::logging::throw_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")")
/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR_MSG(msg) ::lean::logging::throw_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg)
/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR_CTX(msg, ctx) ::lean::logging::throw_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg, ctx)
/// Throws a runtime_error exception.
#define LEAN_THROW_ERROR_ANY(msg) LEAN_THROW_ERROR_MSG(static_cast<::std::ostringstream&>(::std::ostringstream() << msg).str().c_str())

/// Throws an invalid_argument exception.
#define LEAN_THROW_INVALID() ::lean::logging::throw_invalid(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")")
/// Throws an invalid_argument exception.
#define LEAN_THROW_INVALID_MSG(msg) ::lean::logging::throw_invalid(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg)
/// Throws an invalid_argument exception.
#define LEAN_THROW_INVALID_ANY(msg) LEAN_THROW_INVALID_MSG(static_cast<::std::ostringstream&>(::std::ostringstream() << msg).str().c_str())

/// Throws a bad_alloc exception.
#define LEAN_THROW_BAD_ALLOC() ::lean::logging::throw_bad_alloc(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")")
/// Throws a bad_alloc exception.
#define LEAN_THROW_BAD_ALLOC_SIZE(size) ::lean::logging::throw_bad_alloc(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", size)

/// @}

#ifdef LEAN_INCLUDE_LINKED
#include "source/exceptions.cpp"
#endif

#endif