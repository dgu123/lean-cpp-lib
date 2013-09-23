/*****************************************************/
/* lean                         (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef _PLATFORM_H_LEAN_
#define _PLATFORM_H_LEAN_

//---------------- Warnings ----------------//

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
	// 'override' specifier is now standard
	#pragma warning(disable : 4481)
#endif

//---------------- Platform ----------------//

/// @addtogroup GlobalMacros
/// @{

#if !defined(LEAN_DEBUG_BUILD) && !defined(LEAN_RELEASE_BUILD)
	/// Defined in debug builds.
	#define LEAN_DEBUG_BUILD 1
	/// Defined in release builds.
	#define LEAN_RELEASE_BUILD 1

	#ifdef NDEBUG
		#undef LEAN_DEBUG_BUILD
	#else
		#undef LEAN_RELEASE_BUILD
	#endif
#endif

#ifdef _MSC_VER
	/// Reduces overhead for purely virtual classes.
	#define LEAN_INTERFACE __declspec(novtable)
#else
	/// Reduces overhead for purely virtual classes.
	#define LEAN_INTERFACE
#endif

#ifdef _MSC_VER
	/// Assumes that the given expression is always true.
	#define LEAN_ASSUME(expr) __assume(expr)
	/// Defined, if LEAN_ASSUME supported.
	#define LEAN_HAS_ASSUME 1
	/// Interrups program execution.
	#define LEAN_DEBUGBREAK() __debugbreak()
	/// May be used to mark specific branches unreachable.
	#define LEAN_UNREACHABLE() __assume(false)
#else
	/// Assumes that the given expression is always true.
	#define LEAN_ASSUME(expr)
	/// Interrups program execution.
	#define LEAN_DEBUGBREAK() __builtin_trap()
	/// May be used to mark specific branches unreachable.
	#define LEAN_UNREACHABLE() __builtin_unreachable()
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
	#define LEAN_NOINLINE inline __attribute__((noinline)) 
	/// Instructs the compiler not to inline a specific method at link time.
	#define LEAN_NOLTINLINE __attribute__((noinline)) 
#endif

/// Instructs the compiler not to inline a specific template function in a header file.
#define LEAN_NOTINLINE LEAN_NOLTINLINE

/// @}

#endif