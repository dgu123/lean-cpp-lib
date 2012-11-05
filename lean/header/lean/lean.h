/*****************************************************/
/* lean                         (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN
#define LEAN

/// @addtogroup GlobalSwitches Global switches used for configuration
/// @see GlobalMacros
/// @see AssortedSwitches
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

	/// Define this when compiling lean into a dynamic link library.
	#define LEAN_MAYBE_EXPORT
	#undef LEAN_MAYBE_EXPORT

	/// Define this when including each component of lean exactly once to directly integrate it with your libary.
	#define LEAN_INTEGRATE_ONCE
	#undef LEAN_INTEGRATE_ONCE
#endif

/// @}

/// @defgroup AssortedSwitches Assorted Switches
/// @see GlobalSwitches

/// @addtogroup GlobalMacros Global macros
/// @see GlobalSwitches
/// @see CPP0X
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

#ifndef LEAN_MAYBE_EXPORT
	/// Instructs the compiler to export certain functions and methods when defined accordingly.
	#define LEAN_MAYBE_EXPORT
#endif

#ifdef _MSC_VER

	#pragma warning(push)
	// Decorated name length exceeded all the time by various STL containers
	#pragma warning(disable : 4503)
	// Can't do anything about methods not being inlined
	#pragma warning(disable : 4714)
	// Formal parameters named for documentation purposes
	#pragma warning(disable : 4100)
	// Constant conditional expressions occur quite often in template code
	#pragma warning(disable : 4127)
	// Sometimes, using 'this' in initializier lists is unavoidable
	#pragma warning(disable : 4355)
	// We actually want arrays & PODs to be default-initialized
	#pragma warning(disable : 4351)
	#pragma warning(disable : 4345)
	// Assignment operators suppressed intentionally
	#pragma warning(disable : 4512)
	// Extern template now standard
	#pragma warning(disable : 4231)

#endif

#ifdef _MSC_VER
	/// Assumes that the given expression is always true.
	#define LEAN_ASSUME(expr) __assume(expr)
	/// Defined, if LEAN_ASSUME supported.
	#define LEAN_HAS_ASSUME 1
#else
	/// Assumes that the given expression is always true.
	#define LEAN_ASSUME(expr)
#endif

#ifdef LEAN_DEBUG_BUILD
	#include <cassert>
	#ifdef LEAN_HAS_ASSUME
		/// Asserts that the given expression is always true.
		#define LEAN_ASSERT(expr) ( assert(expr), LEAN_ASSUME(expr) )
	#else
		/// Asserts that the given expression is always true.
		#define LEAN_ASSERT(expr) assert(expr)
	#endif
	/// Asserts that the given expression is always true, does not assume anything in release builds.
	#define LEAN_ASSERT_DEBUG(expr) assert(expr)
#else
	#ifdef LEAN_HAS_ASSUME
		/// Asserts that the given expression is always true.
		#define LEAN_ASSERT(expr) LEAN_ASSUME(expr)
	#else
		/// Asserts that the given expression is always true.
		#define LEAN_ASSERT(expr)
	#endif
	/// Asserts that the given expression is always true, does not assume anything in release builds.
	#define LEAN_ASSERT_DEBUG(expr)
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

/// Instructs the compiler not to inline a specific template function in a header file.
#define LEAN_NOTINLINE LEAN_NOLTINLINE

#ifdef _MSC_VER
	/// Reduces overhead for purely virtual classes.
	#define LEAN_INTERFACE __declspec(novtable)
#else
	/// Reduces overhead for purely virtual classes.
	#define LEAN_INTERFACE
#endif

#ifndef LEAN_INTERFACE_BEHAVIOR
	/// Makes the given class behave like an interface.
	#define LEAN_INTERFACE_BEHAVIOR(name) \
			protected: \
				LEAN_INLINE name& operator =(const name&) throw() { return *this; } \
				LEAN_INLINE ~name() throw() { } \
			private:

	/// Makes the given class behave like an interface supporting shared ownership.
	#define LEAN_SHARED_INTERFACE_BEHAVIOR(name) \
			public: \
				virtual ~name() throw() { } \
			protected: \
				LEAN_INLINE name& operator =(const name&) throw() { return *this; } \
			private:
#endif

#if !defined(LEAN_INTEGRATE_ONCE) && defined(LEAN_HEADER_ONLY) && !defined(LEAN_BUILD_LIB)
	/// Picks the first argument in a linked library, the second one in a header-only library.
	#define LEAN_LINK_SELECT(linklib, headeronly) headeronly
#else
	/// Picks the first argument in a linked library, the second one in a header-only library.
	#define LEAN_LINK_SELECT(linklib, headeronly) linklib
	/// Using a linked library.
	#define LEAN_LINKING 1
#endif

#if !defined(LEAN_LINKING) || defined(LEAN_INTEGRATE_ONCE)
	/// Also include linked function definitions.
	#define LEAN_INCLUDE_LINKED 1
#endif

/// Trys to avoid inlining in a header-only library.
#define LEAN_MAYBE_LINK LEAN_LINK_SELECT(, LEAN_NOINLINE)
/// Trys to avoid inlining in a header-only library as well as at link time.
#define LEAN_ALWAYS_LINK LEAN_LINK_SELECT(LEAN_NOLTINLINE, LEAN_NOINLINE)

#if !defined(LEAN_INTEGRATE_ONCE) && (defined(LEAN_HEADER_ONLY) || !defined(LEAN_MIN_DEPENDENCY)) && !defined(LEAN_BUILD_LIB)
	/// Picks the first argument in a header-only / non-min-dependency library, the second one in a min-dependency / linked library.
	#define LEAN_INLINE_SELECT(maxdep, mindep) maxdep
	/// Inlining in header-only library
	#define LEAN_INLINING 1
#else
	/// Picks the first argument in a header-only / non-min-dependency library, the second one in a min-dependency / linked library.
	#define LEAN_INLINE_SELECT(maxdep, mindep) mindep
#endif

#if defined(LEAN_INLINING) || defined(LEAN_INTEGRATE_ONCE)
	/// Include inlined function definitions.
	#define LEAN_INCLUDE_INLINED 1
#endif

/// Inlines in a header-only / non-min-dependency library.
#define LEAN_MAYBE_INLINE LEAN_INLINE_SELECT(inline, )
/// Trys to avoid inlining in header-only / non-min-dependency library as well as at link time.
#define LEAN_NEVER_INLINE LEAN_INLINE_SELECT(LEAN_NOINLINE, LEAN_NOLTINLINE)

/// @}

/// @addtogroup Libray Lean cpp library
/// For a better overview, see <a href="namespaces.html">Namespaces</a>.
/// @see <a href="namespaces.html">Namespaces</a>
/// @{

/// Main namespace of the lean library.
namespace lean
{

/// Allows for the construction of uninitialized data.
enum uninitialized_t
{
	uninitialized ///< Do not initialize.
};

/// Allows for explicitly consuming construction (aka move).
enum consume_t
{
	consume ///< Consume the contents of the given object.
};

/// Returns the address of the given reference.
template <class Type>
LEAN_INLINE Type* addressof(Type& value)
{
	// Use C-style cast as const_casting would only make it worse
	return reinterpret_cast<Type*>( &(char&)value );
}

/// Returns the length of the given array.
template <class Type, size_t Size>
LEAN_INLINE size_t arraylen(Type (&)[Size])
{
	return Size;
}

/// Returns the next iterator.
template <class Iterator>
LEAN_INLINE Iterator next(Iterator iterator)
{
	return ++iterator;
}

/// Returns the previous iterator.
template <class Iterator>
LEAN_INLINE Iterator prev(Iterator iterator)
{
	return --iterator;
}

} // namespace

/// @}

#include "macros.h"
#include "cpp0x.h"
#include "meta/literal.h"
#include "meta/type.h"

namespace lean
{

/// Asserts that the given value is always true.
LEAN_INLINE void check(bool value)
{
	LEAN_ASSERT_DEBUG(value);
}

#ifndef LEAN0X_NO_RVALUE_REFERENCES

/// Asserts that the given value is not null.
template <class Value>
Value assert_not_null(Value &&value)
{
	LEAN_ASSERT(value != nullptr);
	return std::forward<Value>(value);
}

/// Asserts that the given value is not null.
template <class Value>
Value assert_not_null_debug(Value &&value)
{
	LEAN_ASSERT_DEBUG(value != nullptr);
	return std::forward<Value>(value);
}

#else

/// Asserts that the given value is not null.
template <class Value>
Value& assert_not_null(Value &value)
{
	LEAN_ASSERT(value != nullptr);
	return value;
}

/// Asserts that the given value is not null.
template <class Value>
const Value& assert_not_null(const Value &value)
{
	LEAN_ASSERT(value != nullptr);
	return value;
}

/// Asserts that the given value is not null.
template <class Value>
Value& assert_not_null_debug(Value &value)
{
	LEAN_ASSERT_DEBUG(value != nullptr);
	return value;
}

/// Asserts that the given value is not null.
template <class Value>
const Value& assert_not_null_debug(const Value &value)
{
	LEAN_ASSERT_DEBUG(value != nullptr);
	return value;
}

#endif

/// Returns the smaller of both arguments.
template <class Type>
LEAN_INLINE const Type& min(const Type &a,const Type &b)
{
	return (a < b) ? a : b;
}

/// Returns the larger of both arguments.
template <class Type>
LEAN_INLINE const Type& max(const Type &a, const Type &b)
{
	return (a < b) ? b : a;
}

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
#endif

/// @addtogroup Libray
/// @{

/// Asserts that the given expression is never null, returning the expression.
#define LEAN_ASSERT_NOT_NULL(expr) ::lean::assert_not_null(expr)
/// Asserts that the given expression is never null, returning the expression.
#define LEAN_ASSERT_NOT_NULL_DEBUG(expr) ::lean::assert_not_null_debug(expr)

/// @}

#ifndef LEAN_NO_MINMAX	
	using lean::min;
	using lean::max;
#endif

#ifndef LEAN_NO_CHECK	
	using lean::check;
#endif

#include "types.h"

#endif