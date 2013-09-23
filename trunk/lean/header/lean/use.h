/*****************************************************/
/* lean                         (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef _USE_H_LEAN_
#define _USE_H_LEAN_

#include "core.h"
#include "config.h"

namespace lean
{
	/// Returns true if execution should be continued normally.
	LEAN_MAYBE_EXPORT bool maybeIgnoreAssertion(const char *message, const char *file, unsigned int line);
}

/// @addtogroup GlobalMacros
/// @{

#ifdef LEAN_DEBUG_BUILD
	/// Asserts that the given expression is always true.
	#define LEAN_ASSERT_CTX(expr, msg, file, line) (void) ((expr) || ::lean::maybeIgnoreAssertion(msg, file, line) || (LEAN_DEBUGBREAK(), 0) )
	/// Asserts that the given expression is always true.
	#define LEAN_ASSERT(expr) LEAN_ASSERT_CTX(expr, #expr, __FILE__, __LINE__)
	/// Asserts that the given expression is always true, does not assume anything in release builds.
	#define LEAN_ASSERT_DEBUG_CTX(expr, msg, file, line) LEAN_ASSERT_CTX(expr, msg, file, line)
	/// Asserts that the given expression is always true, does not assume anything in release builds.
	#define LEAN_ASSERT_DEBUG(expr) LEAN_ASSERT(expr)
	/// Asserts that the calling point is unreachable.
	#define LEAN_ASSERT_UNREACHABLE() (::lean::maybeIgnoreAssertion("unreachable", __FILE__, __LINE__), LEAN_DEBUGBREAK(), LEAN_UNREACHABLE())
#else
	/// Asserts that the given expression is always true.
	#define LEAN_ASSERT_CTX(expr, msg, file, line) LEAN_ASSUME(expr)
	/// Asserts that the given expression is always true.
	#define LEAN_ASSERT(expr) LEAN_ASSUME(expr)
	/// Asserts that the given expression is always true, does not assume anything in release builds.
	#define LEAN_ASSERT_DEBUG_CTX(expr, msg, file, line)
	/// Asserts that the given expression is always true, does not assume anything in release builds.
	#define LEAN_ASSERT_DEBUG(expr)
	/// Asserts that the calling point is unreachable.
	#define LEAN_ASSERT_UNREACHABLE() LEAN_UNREACHABLE()
#endif

/// @}

#include "macros.h"
#include "meta/type.h"

namespace lean
{

/// Asserts that the given value is always true.
LEAN_INLINE void check(bool value, const char *expr = "check", const char *file = __FILE__, unsigned int line = __LINE__)
{
	LEAN_ASSERT_DEBUG_CTX(value, expr, file, line);
}

#ifndef LEAN0X_NO_RVALUE_REFERENCES

/// Asserts that the given value is not null.
template <class Value>
LEAN_INLINE Value assert_not_null(Value &&value, const char *expr = "not nullptr", const char *file = __FILE__, unsigned int line = __LINE__)
{
	LEAN_ASSERT_CTX(value != nullptr, expr, file, line);
	return std::forward<Value>(value);
}

/// Asserts that the given value is not null.
template <class Value>
LEAN_INLINE Value assert_not_null_debug(Value &&value, const char *expr = "not nullptr", const char *file = __FILE__, unsigned int line = __LINE__)
{
	LEAN_ASSERT_DEBUG_CTX(value != nullptr, expr, file, line);
	return std::forward<Value>(value);
}

#else

/// Asserts that the given value is not null.
template <class Value>
LEAN_INLINE Value& assert_not_null(Value &value, const char *expr = "not nullptr", const char *file = __FILE__, unsigned int line = __LINE__)
{
	LEAN_ASSERT_CTX(value != nullptr, expr, file, line);
	return value;
}

/// Asserts that the given value is not null.
template <class Value>
LEAN_INLINE const Value& assert_not_null(const Value &value, const char *expr = "not nullptr", const char *file = __FILE__, unsigned int line = __LINE__)
{
	LEAN_ASSERT_CTX(value != nullptr, expr, file, line);
	return value;
}

/// Asserts that the given value is not null.
template <class Value>
LEAN_INLINE Value& assert_not_null_debug(Value &value, const char *expr = "not nullptr", const char *file = __FILE__, unsigned int line = __LINE__)
{
	LEAN_ASSERT_DEBUG_CTX(value != nullptr, expr, file, line);
	return value;
}

/// Asserts that the given value is not null.
template <class Value>
LEAN_INLINE const Value& assert_not_null_debug(const Value &value, const char *expr = "not nullptr", const char *file = __FILE__, unsigned int line = __LINE__)
{
	LEAN_ASSERT_DEBUG_CTX(value != nullptr, expr, file, line);
	return value;
}

#endif

typedef size_t lean_arraylen_t;

} // namespace

#ifdef DOXYGEN_READ_THIS
	/// @ingroup GlobalSwitches
	/// Define this to disable global check function.
	#define LEAN_NO_CHECK
	#undef LEAN_NO_CHECK
	/// @ingroup GlobalSwitches
	/// Define this to disable global min/max templates.
	#define LEAN_NO_MINMAX
	#undef LEAN_NO_MINMAX
	/// @ingroup GlobalSwitches
	/// Define this to disable global arraylen macro.
	#define LEAN_NO_ARRAYLEN
	#undef LEAN_NO_ARRAYLEN
#endif

/// @addtogroup Libray
/// @{

/// Asserts that the given expression is never null, returning the expression.
#define LEAN_ASSERT_NOT_NULL(expr) ::lean::assert_not_null(expr, #expr " != nullptr", __FILE__, __LINE__)
/// Asserts that the given expression is never null, returning the expression.
#define LEAN_ASSERT_NOT_NULL_DEBUG(expr) ::lean::assert_not_null_debug(expr, #expr " != nullptr", __FILE__, __LINE__)
/// Asserts that no unhandled exceptions escape the preceding try block.
#define LEAN_ASSERT_NOEXCEPT catch (...) { LEAN_ASSERT_UNREACHABLE(); }

/// @}

#ifndef LEAN_NO_MINMAX	
	using lean::min;
	using lean::max;
#endif

#ifndef LEAN_NO_CHECK	
	using lean::check;
#endif

#ifndef LEAN_NO_ARRAYLEN
	using lean::lean_arraylen_t;
	#define arraylen(x) lean_arraylen_t(LEAN_ARRAYLEN(x))
#endif

#ifndef LEAN_NO_MOVE_FORWARD
	using lean::move;
	using lean::forward;
#endif

#include "types.h"

#ifdef LEAN_INCLUDE_LINKED
#include "logging/source/assert.cpp"
#endif

#endif