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

/// Resource pointer class that performs strong reference counting on the given resource type.
template <class Resource>
class resource_ptr
{
private:
	Resource *m_resource;

	/// Acquires the given resource.
	static Resource* acquire(Resource *resource)
	{
		return (resource && resource->ref_counter().increment())
			? resource
			: nullptr;
	}

	/// Releases the given resource.
	static void release(Resource *resource)
	{
		// Clean up, if this is the last reference
		if (resource && !resource->ref_counter().decrement())
			resource->destroy();
	}

public:
	/// Type of the resource stored by this resource pointer.
	typedef Resource resource_type;
	/// Type of the pointer stored by this resource pointer.
	typedef Resource* value_type;

	/// Constructs a resource pointer from the given resource.
	resource_ptr(resource_type *resource = nullptr)
		: m_resource( acquire(resource) ) { };
	/// Constructs a resource pointer from the given resource pointer.
	resource_ptr(const resource_ptr &right)
		: m_resource( acquire(right.m_resource) ) { };
	/// Destroys the resource pointer.
	~resource_ptr()
	{
		release(m_resource);
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
		*this = right.m_resource;
		return *this;
	}

	/// Gets the resource stored by this resource pointer.
	resource_type* get(void) const { return m_resource; };

	/// Gets the resource stored by this resource pointer.
	resource_type& operator *() const { return *get(); };
	/// Gets the resource stored by this resource pointer.
	resource_type* operator ->() const { return get(); };

	/// Gets the resource stored by this resource pointer.
	operator resource_type*() const { return get(); };
};

} // namespace

using smart::resource_ptr;

} // namespace

#endif