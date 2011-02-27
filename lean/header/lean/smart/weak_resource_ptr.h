/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_WEAK_RESOURCE_PTR
#define LEAN_SMART_WEAK_RESOURCE_PTR

#include "../cpp0x.h"
#include "ref_counter.h"

namespace lean
{
namespace smart
{

/// Resource pointer class that performs weak reference counting on the given resource type.
template <class Resource>
class weak_resource_ptr
{
private:
	Resource *m_resource;

	typedef typename Resource::ref_counter_type ref_counter_type;
	ref_counter_type m_refCounter;

public:
	/// Type of the resource stored by this resource pointer.
	typedef Resource resource_type;
	/// Type of the pointer stored by this resource pointer.
	typedef Resource* value_type;

	/// Constructs a resource pointer from the given resource.
	weak_resource_ptr(resource_type *resource = nullptr)
		: m_resource(resource),
		m_refCounter( (resource) ? resource->ref_counter() : ref_counter_type::null() ) { };
	/// Constructs a resource pointer from the given resource pointer.
	weak_resource_ptr(const weak_resource_ptr &right)
		: m_resource(right.m_resource),
		m_refCounter(right.m_refCounter) { };

	/// Replaces the stored resource with the given resource. <b>[ESA]</b>
	weak_resource_ptr& operator =(resource_type *resource)
	{
		if (m_resource != resource)
		{
			m_resource = resource;
			m_refCounter = (resource) ? resource->ref_counter() : ref_counter_type::null();
		}

		return *this;
	}
	/// Replaces the stored resource with one stored by the given resource pointer. <b>[ESA]</b>
	weak_resource_ptr& operator =(const weak_resource_ptr &right)
	{
		*this = right.m_resource;
		return *this;
	}

	/// Gets whether the resource stored by this pointer is still valid.
	bool check() const
	{
		return (m_resource && m_refCounter.valid());
	}
	/// Gets the resource stored by this resource pointer or null, if the resource has been destroyed.
	resource_type* get(void) const
	{
		return (check()) ? m_resource : nullptr;
	}
	/// Gets the resource stored by this resource pointer.
	resource_type* getunchecked(void) const { return m_resource; };

	/// Gets the resource stored by this resource pointer.
	resource_type& operator *() const { return *getunchecked(); };
	/// Gets the resource stored by this resource pointer.
	resource_type* operator ->() const { return getunchecked(); };

	/// Gets the resource stored by this resource pointer or null, if the resource has been destroyed.
	operator resource_type*() const { return get(); };
};

} // namespace

using smart::weak_resource_ptr;

} // namespace

#endif