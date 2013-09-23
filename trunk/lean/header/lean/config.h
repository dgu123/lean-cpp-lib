/*****************************************************/
/* lean                         (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef _CONFIG_H_LEAN_
#define _CONFIG_H_LEAN_

//---------------- Switches ----------------//

/// @addtogroup GlobalSwitches Global switches used for configuration
/// @see GlobalMacros
/// @see AssortedSwitches
/// @{

#ifndef LEAN_MAYBE_EXPORT
	/// Instructs the compiler to export certain functions and methods from a dynamic link library when defined accordingly.
	#define LEAN_MAYBE_EXPORT
#endif

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
	/// Define this when including each component of lean exactly once to directly integrate it with your libary.
	#define LEAN_INTEGRATE_ONCE
	#undef LEAN_INTEGRATE_ONCE
#endif

/// @}

/// @defgroup AssortedSwitches Assorted Switches
/// @see GlobalSwitches

//---------------- Library configuration ----------------//

/// @addtogroup GlobalMacros
/// @{

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

#endif