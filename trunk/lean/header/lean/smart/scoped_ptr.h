/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_SCOPED_PTR
#define LEAN_SMART_SCOPED_PTR

#include "../lean.h"
#include "../tags/noncopyable.h"
#include "../meta/dependent_false.h"
#include "../meta/strip.h"

namespace lean
{
namespace smart
{

/// Deletes the given object (default policy implementation).
template <class Type>
LEAN_INLINE void release_ptr(Type *object)
{
	if (sizeof(Type) > 0)
		delete object;
}

/// Deletes the given array (default policy implementation).
template <class Type>
LEAN_INLINE void release_array_ptr(Type *object)
{
	if (sizeof(Type) > 0)
		delete[] object;
}

/// Generic scoped pointer policy.
template <class Type>
struct generic_ptr_policy
{
	/// Calls @code release_ptr@endcode on the given object.
	static LEAN_INLINE void release(Type *object)
	{
		release_ptr(object);
	}
};
template <class Type>
struct generic_ptr_policy<Type[]>
{
	static LEAN_INLINE void release(Type *object)
	{
		release_array_ptr(object);
	}
};

/// Delete scoped pointer policy.
template <class Type>
struct delete_ptr_policy
{
	/// Deletes the given object.
	static LEAN_INLINE void release(Type *object)
	{
		if (sizeof(Type) > 0)
			delete object;
	}
};
template <class Type>
struct delete_ptr_policy<Type[]>
{
	static LEAN_INLINE void release(Type *object)
	{
		if (sizeof(Type) > 0)
			delete[] object;
	}
};

/// Destroy scoped pointer policy.
template <class Type>
struct destroy_ptr_policy
{
	/// Calls @code object->destroy()@endcode.
	static LEAN_INLINE void release(Type *object)
	{
		if (object)
			object->destroy();
	}
};
template <class Type>
struct destroy_ptr_policy<Type[]>;

/// Release scoped pointer policy.
template <class Type>
struct release_ptr_policy
{
	/// Calls @code object->release()@endcode.
	static LEAN_INLINE void release(Type *object)
	{
		if (object)
			object->release();
	}
};
template <class Type>
struct release_ptr_policy<Type[]>;

/// Scoped pointer class that releases the object pointed to on destruction.
template < class Type, class ReleasePolicy = generic_ptr_policy<Type> >
class scoped_ptr : public noncopyable
{
public:
	/// Type of the object pointed to.
	typedef typename lean::strip_array<Type>::type object_type;
	/// Type of the pointer stored by this scoped pointer.
	typedef object_type* value_type;

private:
	value_type m_object;

public:
	/// Constructs a scoped pointer from the given object pointer.
	explicit scoped_ptr(object_type *object = nullptr)
		: m_object( object ) { };
	/// Constructs a scoped pointer from the given object pointer.
	template <class Type2>
	explicit scoped_ptr(Type2 *object)
		: m_object( object ) { }

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Constructs a scoped pointer from the given scoped pointer.
	template <class Type2>
	scoped_ptr(scoped_ptr<Type2, ReleasePolicy> &&right)
		: m_object( right.detach() ) { }
#endif
	
	/// Releases the object pointed to.
	LEAN_INLINE ~scoped_ptr()
	{
		ReleasePolicy::release(m_object);
	}

	/// Detaches the object pointed to.
	LEAN_INLINE object_type* detach()
	{
		value_type prevObject = m_object;
		m_object = nullptr;
		return prevObject;
	}

	/// Replaces the stored object with the given object. <b>[ESA]</b>
	scoped_ptr& operator =(object_type *object)
	{
		// Self-assignment would be wrong
		if (object != m_object)
		{
			value_type prevObject = m_object;
			m_object = object;
			ReleasePolicy::release(prevObject);
		}
		
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Replaces the stored object with the one stored by the given r-value scoped pointer. <b>[ESA]</b>
	template <class Type2>
	scoped_ptr& operator =(scoped_ptr<Type2, ReleasePolicy> &&right)
	{
		// Self-assignment would be wrong
		if (addressof(right) != static_cast<void*>(this))
		{
			value_type prevObject = m_object;
			m_object = right.detach();
			ReleasePolicy::release(prevObject);
		}

		return *this;
	}
#endif

	/// Gets the object stored by this scoped pointer.
	LEAN_INLINE object_type*const& get() const { return m_object; }

	/// Gets the object stored by this scoped pointer.
	LEAN_INLINE object_type& operator *() const { return *get(); }
	/// Gets the object stored by this scoped pointer.
	LEAN_INLINE object_type* operator ->() const { return get(); }

	/// Gets the object stored by this scoped pointer.
	LEAN_INLINE operator object_type*() const { return get(); }

	/// Gets a double-pointer allowing for COM-style object retrieval. The pointer returned may
	/// only ever be used until the next call to one of this pointer's methods.
	LEAN_INLINE object_type** rebind()
	{
		*this = nullptr;
		return &m_object;
	}

	/// Swaps the given pointers.
	void swap(scoped_ptr& right)
	{
		value_type prevObject = m_object;
		m_object = right.m_object;
		right.m_object = prevObject;
	}
};

/// Swaps the given pointers.
template <class T, class R>
LEAN_INLINE void swap(scoped_ptr<T, R> &left, scoped_ptr<T, R> &right)
{
	left.swap(right);
}

} // namespace

using smart::scoped_ptr;

} // namespace

#endif