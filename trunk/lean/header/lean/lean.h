/*****************************************************/
/* lean                         (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN
#define LEAN

/// @addtogroup GlobalSwitches Global switches used for configuration
/// @{

#ifdef DOXYGEN_READ_THIS
	/// Define this when building a linked library from the lean sources.
	#define LEAN_BUILD_LIB
	#undef LEAN_BUILD_LIB
	
	/// Define this to use lean as a header-only library.
	#define LEAN_HEADER_ONLY
	#undef LEAN_HEADER_ONLY
	
	/// Define this to minimize lean header dependencies (compiles more source code into the link libray).
	#define LEAN_MIN_DEPENDENCY
	#undef LEAN_MIN_DEPENDENCY
#endif

/// @}

/// @addtogroup GlobalMacros Global macros
/// @{

#ifdef DOXYGEN_READ_THIS

	/// Defined in debug builds.
	#define LEAN_DEBUG_BUILD
	#undef LEAN_DEBUG_BUILD
	/// Defined in release builds.
	#define LEAN_RELEASE_BUILD
	#undef LEAN_RELEASE_BUILD

#else

#ifdef _DEBUG
	/// Defined in debug builds.
	#define LEAN_DEBUG_BUILD
#else
	/// Defined in release builds.
	#define LEAN_RELEASE_BUILD
#endif

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
	/// Instructs the compiler not to inline a specific method in a header file.
	#define LEAN_NOINLINE __declspec(noinline) inline
	/// Instructs the compiler not to inline a specific method at link time.
	#define LEAN_NOLTINLINE __declspec(noinline)

	#ifndef LEAN_DEBUG_BUILD
		/// Prefer default destructors over empty destructors (limited access control)
		#define LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	#endif
#else
	/// Instructs the compiler to inline a specific method.
	#define LEAN_INLINE inline
	/// Instructs the compiler not to inline a specific method in a header file.
	#define LEAN_NOINLINE inline
	/// Instructs the compiler not to inline a specific method at link time.
	#define LEAN_NOLTINLINE
#endif

#if defined(LEAN_HEADER_ONLY) && !defined(LEAN_BUILD_LIB)
	/// Picks the first argument in a linked library, the second one in a header-only library.
	#define LEAN_LINK_SELECT(linklib, headeronly) headeronly
#else
	/// Picks the first argument in a linked library, the second one in a header-only library.
	#define LEAN_LINK_SELECT(linklib, headeronly) linklib
	/// Using a linked library.
	#define LEAN_LINKING 1
#endif

/// Trys to avoid inlining in a header-only library.
#define LEAN_MAYBE_LINK LEAN_LINK_SELECT(, LEAN_NOINLINE)
/// Trys to avoid inlining in a header-only library as well as at link time.
#define LEAN_ALWAYS_LINK LEAN_LINK_SELECT(LEAN_NOLTINLINE, LEAN_NOINLINE)

#if (defined(LEAN_HEADER_ONLY) || !defined(LEAN_MIN_DEPENDENCY)) && !defined(LEAN_BUILD_LIB)
	/// Picks the first argument in a header-only / non-min-dependency library, the second one in a min-dependency / linked library.
	#define LEAN_INLINE_SELECT(maxdep, mindep) maxdep
	/// Inlining in header-only library
	#define LEAN_INLINING 1
#else
	/// Picks the first argument in a header-only / non-min-dependency library, the second one in a min-dependency / linked library.
	#define LEAN_INLINE_SELECT(maxdep, mindep) mindep
#endif

/// Inlines in a header-only / non-min-dependency library.
#define LEAN_MAYBE_INLINE LEAN_INLINE_SELECT(inline, )
/// Trys to avoid inlining in header-only / non-min-dependency library as well as at link time.
#define LEAN_NEVER_INLINE LEAN_INLINE_SELECT(LEAN_NOINLINE, LEAN_NOLTINLINE)

/// @}

/// @addtogroup Libray Lean cpp library
/// For a better overview, see <a href="namespaces.html">namespaces</a>.
/// @{

/// Main namespace of the lean library.
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

/// Returns the address of the given reference.
template<class Type>
LEAN_INLINE Type* addressof(Type& value)
{
	return reinterpret_cast<Type*>(&reinterpret_cast<char&>(value));
}

/// Returns the address of the given reference.
template<class Type>
LEAN_INLINE const Type* addressof(const Type& value)
{
	return reinterpret_cast<const Type*>(&reinterpret_cast<const char&>(value));
}

} // namespace

/// @}

#ifdef DOXYGEN_READ_THIS
	/// @ingroup GlobalSwitches
	/// Define this to disable global min/max templates.
	#define LEAN_NO_MINMAX
	#undef LEAN_NO_MINMAX
#endif

#ifndef LEAN_NO_MINMAX	
	using lean::min;
	using lean::max;
#endif

#include "macros.h"
#include "cpp0x.h"
#include "types.h"

#endif