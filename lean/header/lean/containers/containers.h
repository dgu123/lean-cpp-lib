/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_CONTAINERS
#define LEAN_CONTAINERS_CONTAINERS

namespace lean
{
	/// Provides optimized container classes and wrappers.
	namespace containers { }
}

#include "simple_vector.h"

#include "default_reallocation_policy.h"
#include "move_reallocation_policy.h"
#include "accumulation_vector.h"

#include "accumulation_map.h"

#include "any.h"

#endif