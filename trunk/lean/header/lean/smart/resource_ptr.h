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

/// Destroys the given resource by calling @code delete resource@endcode  (default policy implementation).
template <class Resource>
LEAN_INLINE void destroy_resource(Resource *resource)
{
	delete resource;
}

/// Resource pointer class that performs strong reference counting on the given resource type.
template <class Resource, bool Critical = false>
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
			destroy_resource(resource);
	}

protected:
	/// Constructs a resource pointer from the given resource and reference counter.
	template <class Counter, class Allocator>
	resource_ptr(Resource *resource, const ref_counter<Counter, Allocator>& refCounter)
		: m_resource( acquire(resource, refCounter) ) { };

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

	/// Allows for the binding of existing COM object references on COM pointer construction.
	enum bind_reference_t
	{
		bind_reference ///< Allows for the binding of existing COM object references on COM pointer construction.
	};
	/// Constructs a resource pointer from the given resource without incrementing its reference count.
	resource_ptr(Resource *resource, bind_reference_t)
		: m_resource(resource) { };

	/// Constructs a resource pointer from the given resource pointer.
	resource_ptr(const resource_ptr &right)
		: m_resource( acquire(right.m_resource) ) { };
	/// Constructs a resource pointer from the given resource pointer.
	template <class Resource2, bool Critical2>
	resource_ptr(const resource_ptr<Resource2, Critical2> &right)
		: m_resource( acquire(right.get()) ) { };

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Constructs a resource pointer from the given r-value resource pointer.
	template <class Resource2, bool Critical2>
	resource_ptr(resource_ptr<Resource2, Critical2> &&right)
		: m_resource(right.unbind()) { }
#endif

	/// Destroys the resource pointer.
	~resource_ptr() throw()
	{
		release(m_resource);
	}

	/// Binds the given resource reference to this resource pointer.
	static LEAN_INLINE resource_ptr<resource_type, true> bind(resource_type *resource)
	{
		return resource_ptr<resource_type, true>(resource, resource_ptr<resource_type, true>::bind_reference);
	}
	/// Unbinds the resource reference held by this resource pointer.
	resource_type* unbind()
	{
		Resource *prevResource = m_resource;
		m_resource = nullptr;
		return prevResource;
	}
	/// Transfers the resource reference held by this resource pointer to a new resource pointer.
	resource_ptr<resource_type, true> transfer()
	{
		// Visual C++ won't inline delegating function calls
		return resource_ptr<resource_type, true>(unbind(), resource_ptr<resource_type, true>::bind_reference);
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
	template <class Resource2, bool Critical2>
	resource_ptr& operator =(const resource_ptr<Resource2, Critical2> &right)
	{
		return (*this = right.get());
	}

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Replaces the stored resource with the one stored by the given r-value resource pointer. <b>[ESA]</b>
	template <class Resource2, bool Critical2>
	resource_ptr& operator =(resource_ptr<Resource2, Critical2> &&right)
	{
		// Self-assignment would be wrong
		if (addressof(right) != static_cast<void*>(this))
		{
			Resource *prevResource = m_resource;
			m_resource = right.unbind();
			release(prevResource);
		}

		return *this;
	}
#endif

	/// Gets the resource stored by this resource pointer.
	LEAN_INLINE resource_type* get(void) const { return m_resource; };

	/// Gets the resource stored by this resource pointer.
	LEAN_INLINE resource_type& operator *() const { return *get(); };
	/// Gets the resource stored by this resource pointer.
	LEAN_INLINE resource_type* operator ->() const { return get(); };

	/// Gets the resource stored by this resource pointer.
	LEAN_INLINE operator resource_type*() const
	{
		LEAN_STATIC_ASSERT_MSG_ALT(!Critical,
			"Cannot implicitly cast critical reference, use unbind() for (insecure) storage.",
			Cannot_implicitly_cast_critical_reference__use_unbind_for_insecure_storage);
		return get();
	}
};

/// Binds the given resource reference to a new resource pointer.
template <class Resource>
LEAN_INLINE resource_ptr<Resource, true> bind_resource(Resource *resource)
{
	// Visual C++ won't inline delegating function calls
	return resource_ptr<Resource, true>(resource, resource_ptr<Resource, true>::bind_reference);
}

} // namespace

using smart::resource_ptr;
using smart::bind_resource;

} // namespace

#endif