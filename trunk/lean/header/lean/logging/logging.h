/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING
#define LEAN_LOGGING

namespace lean
{
	/// Provides utility functions and classes, automatically logging raised exceptions,
	/// redirecting logged output to various targets, enhancing logged informtion by
	/// source information (caller file and line), etc.
	/// @see LoggingMacros
	/// @see ExceptionMacros
	namespace logging { }
}

#include "exceptions.h"
#include "log.h"

#endif