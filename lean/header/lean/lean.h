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

#include "macros.h"
#include "cpp0x.h"

#endif