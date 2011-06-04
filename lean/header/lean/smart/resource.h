/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_RESOURCE
#define LEAN_SMART_RESOURCE

#include "ref_counter.h"

namespace lean
{
namespace smart
{

// Prototypes
template <class Resource, bool Critical>
class resource_ptr;
template <class Resource>
class weak_resource_ptr;

/// Base class that may be used to tag a specific class as a resource.
template < class Counter = long, class Allocator = std::allocator<Counter> >
class resource
{
	template <class Resource, bool Critical>
	friend class resource_ptr;
	template <class Resource>
	friend class weak_resource_ptr;

private:
	/// Reference counter type.
	typedef ref_counter<Counter, Allocator> ref_counter_type;
	ref_counter_type m_refCounter;

	/// Returns the reference counter of this resource.
	const ref_counter_type& ref_counter() const { return m_refCounter; }

protected:
	/// Reference counter allocator type.
	typedef typename ref_counter_type::allocator_type ref_counter_allocator_type;

	/// Default constructor.
	resource() { }
	/// Allocator constructor.
	resource(const ref_counter_allocator_type& allocator) : m_refCounter(allocator) { }
	/// Copy constructor.
	resource(const resource& right) { }
	/// Assignment operator.
	resource& operator =(const resource& right) { return *this; }
#ifndef LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	/// Destructor.
	~resource() throw() { }
#endif
};

} // namespace

using smart::resource;

} // namespace

#endif