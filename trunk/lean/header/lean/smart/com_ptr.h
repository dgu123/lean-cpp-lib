/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_COM_PTR
#define LEAN_SMART_COM_PTR

#include "../lean.h"
#include "../meta/dependent_false.h"
#include "common.h"

namespace lean
{
namespace smart
{

/// Acquires a reference to the given COM object.
template <class COMType>
LEAN_INLINE void acquire_com(COMType &object)
{
	object.AddRef();
}
/// Releases a reference to the given COM object.
template <class COMType>
LEAN_INLINE void release_com(COMType *object)
{
	if (object)
		object->Release();
}

/// COM pointer class that performs reference counting on COM objects of the given type.
template <class COMType, bool Critical = false>
class com_ptr
{
private:
	COMType *m_object;

	/// Acquires the given object.
	static COMType* acquire(COMType *object)
	{
		if (object)
			acquire_com(*object);

		return object;
	}

	/// Releases the given object.
	static void release(COMType *object)
	{
		release_com(object);
	}

public:
	/// Type of the COM object stored by this COM pointer.
	typedef COMType com_type;
	/// Type of the pointer stored by this COM pointer.
	typedef COMType* value_type;

	/// Constructs a COM pointer from the given COM object.
	com_ptr(com_type *object = nullptr)
		: m_object( acquire(object) ) { };
	/// Constructs a COM pointer from the given COM object.
	template <class COMType2>
	com_ptr(COMType2 *object)
		: m_object( acquire(object) ) { };

	/// Constructs a COM pointer from the given COM object without incrementing its reference count.
	com_ptr(COMType *object, bind_reference_t)
		: m_object(object) { };

	/// Constructs a COM pointer from the given COM pointer.
	com_ptr(const com_ptr &right)
		: m_object( acquire(right.m_object) ) { };
	/// Constructs a COM pointer from the given COM pointer.
	template <class COMType2, bool Critical2>
	com_ptr(const com_ptr<COMType2, Critical2> &right)
		: m_object( acquire(right.get()) ) { };

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Constructs a COM pointer from the given COM pointer.
	template <class COMType2, bool Critical2>
	com_ptr(com_ptr<COMType2, Critical2> &&right)
		: m_object(right.unbind()) { }
#endif
	
	/// Destroys the COM pointer.
	~com_ptr() throw()
	{
		release(m_object);
	}

	/// Binds the given COM object reference to this COM pointer.
	static LEAN_INLINE com_ptr<com_type, true> bind(com_type *object)
	{
		return com_ptr<com_type, true>(object, com_ptr<com_type, true>::bind_reference);
	}
	/// Unbinds the COM object reference held by this COM pointer.
	LEAN_INLINE com_type* unbind()
	{
		COMType *preObject = m_object;
		m_object = nullptr;
		return preObject;
	}
	/// Transfers the COM object reference held by this COM pointer to a new COM pointer.
	LEAN_INLINE com_ptr<com_type, true> transfer()
	{
		// Visual C++ won't inline delegating function calls
		return com_ptr<com_type, true>(unbind(), com_ptr<com_type, true>::bind_reference);
	}

	/// Replaces the stored COM object with the given object. <b>[ESA]</b>
	com_ptr& operator =(com_type *object)
	{
		// Do not check for redundant assignment
		// -> The code handles redundant assignment just fine
		// -> Checking generates up to twice the code due to unfortunate compiler optimization application order
		COMType *prevObject = m_object;
		m_object = acquire(object);
		release(prevObject);
	
		return *this;
	}

	/// Replaces the stored COM object with one stored by the given COM pointer. <b>[ESA]</b>
	com_ptr& operator =(const com_ptr &right)
	{
		return (*this = right.m_object);
	}
	/// Replaces the stored COM object with one stored by the given COM pointer. <b>[ESA]</b>
	template <class COMType2, bool Critical2>
	com_ptr& operator =(const com_ptr<COMType2, Critical2> &right)
	{
		return (*this = right.get());
	}

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Replaces the stored COM object with the one stored by the given r-value COM pointer. <b>[ESA]</b>
	template <class COMType2, bool Critical2>
	com_ptr& operator =(com_ptr<COMType2, Critical2> &&right)
	{
		// Self-assignment would be wrong
		if (right.get() != m_object)
		{
			COMType *prevObject = m_object;
			m_object = right.unbind();
			release(prevObject);
		}

		return *this;
	}
#endif

	/// Gets the COM object stored by this COM pointer.
	LEAN_INLINE com_type*const& get(void) const { return m_object; };

	/// Gets the COM object stored by this COM pointer.
	LEAN_INLINE com_type& operator *() const { return *get(); };
	/// Gets the COM object stored by this COM pointer.
	LEAN_INLINE com_type* operator ->() const { return get(); };

	/// Gets the COM object stored by this COM pointer.
	LEAN_INLINE operator com_type*() const
	{
		LEAN_STATIC_ASSERT_MSG_ALT(!Critical,
			"Cannot implicitly cast critical reference, use unbind() for (insecure) storage.",
			Cannot_implicitly_cast_critical_reference__use_unbind_for_insecure_storage);
		return get();
	};

	/// Gets a double-pointer allowing for COM-style object retrieval. The pointer returned may
	/// only ever be used until the next call to one of this COM pointer's methods.
	LEAN_INLINE com_type** rebind()
	{
		*this = nullptr;
		return &m_object;
	}

	/// Swaps the given pointers.
	void swap(com_ptr& right)
	{
		value_type prevObject = m_object;
		m_object = right.m_object;
		right.m_object = prevObject;
	}
};

/// Swaps the given pointers.
template <class T, bool C>
LEAN_INLINE void swap(com_ptr<T, C> &left, com_ptr<T, C> &right)
{
	left.swap(right);
}

/// Binds the given COM reference to a new COM pointer.
template <class COMType>
LEAN_INLINE com_ptr<COMType, true> bind_com(COMType *object)
{
	// Visual C++ won't inline delegating function calls
	return com_ptr<COMType, true>(object, com_ptr<COMType, true>::bind_reference);
}

} // namespace

using smart::com_ptr;

using smart::bind_com;

} // namespace

#endif