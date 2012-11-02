/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_SMART_CLONEABLE
#define LEAN_SMART_CLONEABLE

#include "../lean.h"

namespace lean
{
namespace smart
{

/// Base class that may be used to tag a specific class cloneable.
class LEAN_INTERFACE cloneable
{
	LEAN_INTERFACE_BEHAVIOR(cloneable)

public:
	/// Constructs and returns a clone of this cloneable.
	virtual cloneable* clone() const = 0;
	/// Destroys a clone.
	virtual void destroy() const = 0;
};

} // namespace

using smart::cloneable;

} // namespace

#endif