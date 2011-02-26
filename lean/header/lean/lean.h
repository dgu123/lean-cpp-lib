/*****************************************************/
/* lean                         (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN
#define LEAN

#ifdef _DEBUG
	/// Defined in debug builds
	#define LEAN_DEBUG_BUILD
#else
	/// Defined in release builds
	#define LEAN_RELEASE_BUILD
#endif

#ifdef LEAN_DEBUG_BUILD
	#include <cassert>
	/// Asserts that the given expression is always true.
	#define LEAN_ASSERT(expr) assert(expr)
#else
	#ifdef _MSC_VER
		/// Asserts that the given expression is always true.
		#define LEAN_ASSERT(expr) __assume(expr)
	#endif
#endif

#define LEAN_JOIN_TOKENS_IMPL(a, b) a##b
/// Appends token b to token a.
#define LEAN_JOIN_TOKENS(a, b) LEAN_JOIN_TOKENS_IMPL(a, b)

/// Quotes the given expression.
#define LEAN_QUOTE(expr) #expr
/// Quotes the given value (first evaluating macros).
#define LEAN_QUOTE_VALUE(value) LEAN_QUOTE(value)

#include "cpp0x.h"

#endif