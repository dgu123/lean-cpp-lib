/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_SMART
#define LEAN_SMART_SMART

namespace lean
{
	/// Defines RAII-style classes that automatically take care of resource handling, (un)locking and other common and tedious tasks.
	namespace smart { }
}

#include "auto_restore.h"

#include "cloneable.h"
#include "cloneable_obj.h"

#include "ref_counter.h"
#include "resource.h"
#include "resource_ptr.h"
#include "weak_resource_ptr.h"

#include "com_ptr.h"
#include "scoped_lock.h"

#endif