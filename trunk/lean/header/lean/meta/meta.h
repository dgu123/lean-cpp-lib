/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_META
#define LEAN_META_META

namespace lean
{
	/// Defines helper templates that may be useful in a meta-programming context.
	namespace meta { }
}

#include "dependent_false.h"
#include "type_traits.h"
#include "conditional.h"
#include "strip.h"
#include "dereference.h"
#include "enable_if.h"
#include "constexpr.h"

#endif