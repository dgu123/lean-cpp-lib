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

#ifdef _MSC_VER
	/// Instructs the compiler to inline a specific method.
	#define LEAN_INLINE __forceinline
	/// Instructs the compiler not to inline a specific method.
	#define LEAN_NOINLINE __declspec(noinline)

	#ifndef LEAN_DEBUG_BUILD
		/// Prefer default destructors over empty destructors (limited access control)
		#define LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	#endif
#else
	/// Instructs the compiler to inline a specific method.
	#define LEAN_INLINE inline
	/// Instructs the compiler not to inline a specific method.
	#define LEAN_NOINLINE inline
#endif

namespace lean
{

/// Returns the smaller of both arguments.
template <class T1, class T2>
LEAN_INLINE T1 min(T1 a, T2 b)
{
	return (a < b) ? a : b;
}

/// Returns the larger of both arguments.
template <class T1, class T2>
LEAN_INLINE T1 max(T1 a, T2 b)
{
	return (a < b) ? b : a;
}

} // namespace

#ifndef LEAN_NO_MINMAX	
	using lean::min;
	using lean::max;
#endif

#include "macros.h"
#include "cpp0x.h"

#endif