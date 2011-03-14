/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_COM_PTR
#define LEAN_SMART_COM_PTR

#include "../lean.h"

namespace lean
{
namespace smart
{


/// COM pointer class that performs reference counting on COM objects of the given type.
template <class COMType>
class com_ptr
{
private:
	COMType *m_object;

	/// Acquires the given object.
	static COMType* acquire(COMType *object)
	{
		if (object)
			object->AddRef();

		return object;
	}

	/// Releases the given object.
	static void release(COMType *object)
	{
		// Clean up, if this is the last reference
		if (object)
			object->Release();
	}

protected:
	enum bind_reference_t { bind_reference };

	/// Constructs a COM pointer from the given COM object without incrementing its reference count.
	com_ptr(COMType *object, bind_reference_t)
		: m_object(object) { };

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

	/// Constructs a COM pointer from the given COM pointer.
	com_ptr(const com_ptr &right)
		: m_object( acquire(right.m_object) ) { };
	/// Constructs a COM pointer from the given COM pointer.
	template <class COMType2>
	com_ptr(const com_ptr<COMType2> &right)
		: m_object( acquire(right.m_object) ) { };

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Constructs a COM pointer from the given COM pointer.
	template <class COMType2>
	com_ptr(com_ptr<COMType2> &&right)
		: m_object(std::move(right.m_object))
	{
		right.m_object = nullptr;
	}
#endif
	
	/// Destroys the COM pointer.
	~com_ptr() throw()
	{
		release(m_object);
	}

	/// Binds the given COM object reference to this COM pointer.
	static LEAN_INLINE com_ptr bind(com_type *object)
	{
		return object_ptr(object, bind_reference);
	}
	/// Unbinds the COM object reference held by this COM pointer.
	com_type* unbind()
	{
		COMType *preObject = m_object;
		m_object = nullptr;
		return preObject;
	}
	/// Transfers the COM object reference held by this COM pointer to a new COM pointer.
	object_ptr transfer()
	{
		return bind(unbind());
	}

	/// Replaces the stored COM object with the given object. <b>[ESA]</b>
	object_ptr& operator =(com_type *object)
	{
		if (m_object != object)
		{
			COMType *prevObject = m_object;
			m_object = acquire(object);
			release(prevObject);
		}

		return *this;
	}

	/// Replaces the stored COM object with one stored by the given COM pointer. <b>[ESA]</b>
	object_ptr& operator =(const object_ptr &right)
	{
		return (*this = right.m_object);
	}
	/// Replaces the stored COM object with one stored by the given COM pointer. <b>[ESA]</b>
	template <class COMType2>
	object_ptr& operator =(const object_ptr<COMType2> &right)
	{
		return (*this = right.m_object);
	}

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Replaces the stored COM object with the one stored by the given r-value COM pointer. <b>[ESA]</b>
	template <class COMType2>
	object_ptr& operator =(object_ptr<COMType2> &&right)
	{
		COMType *prevObject = m_object;
		m_object = right.m_object;
		right.m_object = nullptr;
		release(prevObject);

		return *this;
	}
#endif

	/// Gets the COM object stored by this COM pointer.
	LEAN_INLINE com_type* get(void) const { return m_object; };

	/// Gets the COM object stored by this COM pointer.
	LEAN_INLINE com_type& operator *() const { return *get(); };
	/// Gets the COM object stored by this COM pointer.
	LEAN_INLINE com_type* operator ->() const { return get(); };
	
	/// Gets the COM object stored by this COM pointer.
	LEAN_INLINE operator com_type*() const { return get(); };

	/// Rebind pointer class that allows for typical COM object retrieval passing pointers to pointers.
	class rebind_ptr
	{
	friend class com_ptr;

	private:
		com_ptr &m_ptr;
		COMType *m_object;

	protected:
		/// Constructs a rebind pointer object from the given COM pointer.
		rebind_ptr(com_ptr &ptr)
			: m_ptr(ptr),
			// This object is meant for assignment only, storing the initial
			// object here would result in severe loss of references when
			// assignment failed, leaving the old object to be re-bound and
			// possibly destructed right afterwards
			m_object(nullptr) { }

	public:
		/// Assigns the new COM object to the original COM pointer.
		~rebind_ptr()
		{
			// Assigns nullptr on failure
			m_ptr = bind(m_object);
		}

		/// Gets a pointer to a pointer that may be dereferenced to assign a new COM object to a COM pointer.
		LEAN_INLINE com_type** get() { return &m_object; };

		/// Gets a pointer to a pointer that may be dereferenced to assign a new COM object to a COM pointer.
		LEAN_INLINE operator com_type**() { return get(); };
	};

	/// Gets a double-pointer-like object allowing for COM object retrieval passing "pointers" to com_ptr objects.
	LEAN_INLINE rebind_ptr rebind() { return rebind_ptr(*this); };

	/// Gets a double-pointer-like object allowing for COM object retrieval passing "pointers" to com_ptr objects.
	LEAN_INLINE rebind_ptr operator &() { return rebind(); };
};

/// Binds the given COM reference to a new COM pointer.
template <class COMType>
LEAN_INLINE com_ptr<COMType> bind_com(COMType *object)
{
	return com_ptr<COMType>::bind(object);
}

} // namespace

using smart::com_ptr;

} // namespace

#endif