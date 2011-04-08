/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_STRINGS
#define LEAN_STRINGS_STRINGS

namespace lean
{
	/// Provides a generic character range type as well as conversion and streaming
	/// facilities to both simplify and generalize string passing and handling in
	/// your code.
	namespace strings { }
}

#include "char_traits.h"
#include "utf_traits.h"
#include "nullterminated.h"

#endif