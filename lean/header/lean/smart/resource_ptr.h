/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_RESOURCE_PTR
#define LEAN_SMART_RESOURCE_PTR

#include "../cpp0x.h"

namespace lean
{
namespace smart
{

// Prototypes
template <class Counter, class Allocator>
class ref_counter;

/// Resource pointer class that performs strong reference counting on the given resource type.
template <class Resource>
class resource_ptr
{
	template <class Resource>
	friend class weak_resource_ptr;

private:
	Resource *m_resource;

	/// Acquires the given resource.
	template <class Counter, class Allocator>
	static Resource* acquire(Resource *resource, const ref_counter<Counter, Allocator>& refCounter)
	{
		return (resource && refCounter.increment_checked())
			? resource
			: nullptr;
	}

	/// Acquires the given resource.
	static Resource* acquire(Resource *resource)
	{
		if (resource)
			resource->ref_counter().increment();

		return resource;
	}

	/// Releases the given resource.
	static void release(Resource *resource)
	{
		// Clean up, if this is the last reference
		if (resource && !resource->ref_counter().decrement())
			resource->destroy();
	}

protected:
	enum bind_reference_t { bind_reference };

	/// Constructs a resource pointer from the given resource and reference counter.
	template <class Counter, class Allocator>
	resource_ptr(Resource *resource, const ref_counter<Counter, Allocator>& refCounter)
		: m_resource( acquire(resource, refCounter) ) { };
	/// Constructs a resource pointer from the given resource without incrementing its reference count.
	resource_ptr(Resource *resource, bind_reference_t)
		: m_resource(resource) { };

public:
	/// Type of the resource stored by this resource pointer.
	typedef Resource resource_type;
	/// Type of the pointer stored by this resource pointer.
	typedef Resource* value_type;

	/// Constructs a resource pointer from the given resource.
	resource_ptr(resource_type *resource = nullptr)
		: m_resource( acquire(resource) ) { };
	/// Constructs a resource pointer from the given resource.
	template <class Resource2>
	resource_ptr(Resource2 *resource)
		: m_resource( acquire(resource) ) { };

	/// Constructs a resource pointer from the given resource pointer.
	resource_ptr(const resource_ptr &right)
		: m_resource( acquire(right.m_resource) ) { };
	/// Constructs a resource pointer from the given resource pointer.
	template <class Resource2>
	resource_ptr(const resource_ptr<Resource2> &right)
		: m_resource( acquire(right.m_resource) ) { };

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Constructs a resource pointer from the given r-value resource pointer.
	template <class Resource2>
	resource_ptr(resource_ptr<Resource2> &&right)
		: m_resource(right.m_resource)
	{
		right.m_resource = nullptr;
	}
#endif

	/// Destroys the resource pointer.
	~resource_ptr() throw()
	{
		release(m_resource);
	}

	/// Binds the given resource reference to this resource pointer.
	static resource_ptr bind(resource_type *resource)
	{
		return resource_ptr(resource, bind_reference);
	}
	/// Unbinds the resource reference held by this resource pointer.
	resource_type* unbind()
	{
		Resource *prevResource = m_resource;
		m_resource = nullptr;
		return prevResource;
	}
	/// Transfers the resource reference held by this resource pointer to a new resource pointer.
	resource_ptr transfer()
	{
		return bind(unbind());
	}

	/// Replaces the stored resource with the given resource. <b>[ESA]</b>
	resource_ptr& operator =(resource_type *resource)
	{
		if (m_resource != resource)
		{
			Resource *prevResource = m_resource;
			m_resource = acquire(resource);
			release(prevResource);
		}

		return *this;
	}

	/// Replaces the stored resource with one stored by the given resource pointer. <b>[ESA]</b>
	resource_ptr& operator =(const resource_ptr &right)
	{
		return (*this = right.m_resource);
	}
	/// Replaces the stored resource with one stored by the given resource pointer. <b>[ESA]</b>
	template <class Resource2>
	resource_ptr& operator =(const resource_ptr<Resource2> &right)
	{
		return (*this = right.m_resource);
	}

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Replaces the stored resource with one stored by the r-value given resource pointer. <b>[ESA]</b>
	template <class Resource2>
	resource_ptr& operator =(resource_ptr<Resource2> &&right)
	{
		Resource *prevResource = m_resource;
		m_resource = right.m_resource;
		right.m_resource = prevResource;

		return *this;
	}
#endif

	/// Gets the resource stored by this resource pointer.
	resource_type* get(void) const { return m_resource; };

	/// Gets the resource stored by this resource pointer.
	resource_type& operator *() const { return *get(); };
	/// Gets the resource stored by this resource pointer.
	resource_type* operator ->() const { return get(); };

	/// Gets the resource stored by this resource pointer.
	operator resource_type*() const { return get(); };
};

/// Binds the given resource reference to a new resource pointer.
template <class Resource>
inline resource_ptr<Resource> bind_resource(Resource *resource)
{
	return resource_ptr<Resource>::bind(resource);
}

} // namespace

using smart::resource_ptr;
using smart::bind_resource;

} // namespace

#endif