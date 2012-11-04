/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_TAGS_STRIDED_PTR
#define LEAN_TAGS_STRIDED_PTR

#include "../lean.h"

namespace lean
{
namespace tags
{

/// Transitive pointer class that applies pointer const modifiers to the objects pointed to.
template <class Type>
class strided_ptr
{
public:
	/// Type of the object pointed to.
	typedef Type object_type;
	/// Type of the pointer stored by this pointer.
	typedef object_type* value_type;

private:
	value_type m_object;
	ptrdiff_t m_stride;

public:
	/// Constructs a strided pointer from the given pointer.
	LEAN_INLINE explicit strided_ptr(object_type *object = nullptr, ptrdiff_t stride = sizeof(object_type))
		: m_object( object ),
		m_stride( stride ) { };
	/// Constructs a strided pointer from the given pointer.
	template <class Type2>
	LEAN_INLINE explicit strided_ptr(Type2 *object, ptrdiff_t stride = sizeof(Type2))
		: m_object( object ),
		m_stride( stride ) { };
	/// Constructs a strided pointer from the given pointer.
	template <class Type2>
	LEAN_INLINE strided_ptr(const strided_ptr<Type2> &right)
		: m_object( right.get() ),
		m_stride( right.get_stride() ) { };
	
	/// Replaces the stored pointer with the given pointer.
	template <class Type2>
	LEAN_INLINE strided_ptr& operator =(const strided_ptr<Type2> &right)
	{
		m_object = right.get();
		m_stride = right.get_stride();
		return *this;
	}

	/// Gets the pointer stored by this strided pointer. Don't call unless you know what you are doing!
	LEAN_INLINE value_type get() const { return m_object; }
	/// Gets the stride stored by this strided pointer. Don't call unless you know what you are doing!
	LEAN_INLINE ptrdiff_t get_stride() const { return m_stride; }

	/// Gets the first object stored by this strided pointer.
	LEAN_INLINE object_type& operator *() const { return *m_object; }
	/// Gets the first object stored by this strided pointer.
	LEAN_INLINE object_type* operator ->() const { return m_object; }

	/// Gets the n-th object stored by this strided pointer.
	LEAN_INLINE object_type& operator [](ptrdiff_t n) const
	{
		return *(object_type*) ((char*) m_object + m_stride * n);
	}
};

template <class T> LEAN_INLINE strided_ptr<T> operator +(const strided_ptr<T> &p, ptrdiff_t diff) { return strided_ptr<T>( &p[diff], p.get_stride() ); }
template <class T> LEAN_INLINE strided_ptr<T> operator -(const strided_ptr<T> &p, ptrdiff_t diff) { return strided_ptr<T>( &p[-diff], p.get_stride() ); }

template <class T>
LEAN_INLINE ptrdiff_t operator -(const strided_ptr<T> &p, const strided_ptr<T> &q)
{
	LEAN_ASSERT(p.get_stride() == q.get_stride());
	return ( (char*) p.get() - (char*) q.get() ) / p.get_stride();
}

} // namespace

using tags::strided_ptr;

} // namespace

#endif