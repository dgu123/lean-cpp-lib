/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_RESOURCE
#define LEAN_SMART_RESOURCE

namespace lean
{
namespace smart
{

template < class Counter = long, class Allocator = std::allocator<Counter> >
class ref_counter;

/// Base class that may be used to tag a specific class as a resource.
template < class Counter = long, class Allocator = std::allocator<Counter> >
class resource
{
public:
	/// Constructs and returns a clone of this cloneable.
	virtual cloneable* clone() const = nullptr;
	/// Destroys this cloneable.
	virtual void destroy() const = nullptr;
};

} // namespace

using smart::resource;

} // namespace

#endif