/*****************************************************/
/* lean Tags                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_TAGS_TRANSITIVE_PTR
#define LEAN_TAGS_TRANSITIVE_PTR

#include "../lean.h"
#include "../meta/conditional.h"

namespace lean
{
namespace tags
{

/// Transitive pointer class that applies pointer const modifiers to the objects pointed to.
template <class Type, bool Immutable = false>
class transitive_ptr
{
public:
	/// Type of the object pointed to.
	typedef Type object_type;
	/// Type of the pointer stored by this pointer.
	typedef typename lean::conditional_type<Immutable, object_type *const, object_type*>::type value_type;
	/// Type of the pointer stored by this pointer.
	typedef typename lean::conditional_type<Immutable, const object_type *const, const object_type*>::type const_value_type;

private:
	value_type m_object;

public:
	/// Constructs a transitive pointer from the given pointer.
	LEAN_INLINE transitive_ptr(object_type *object = nullptr)
		: m_object( object ) { };
	/// Constructs a transitive pointer from the given pointer.
	template <class Type2>
	LEAN_INLINE transitive_ptr(Type2 *object)
		: m_object( object ) { };
	/// Constructs a transitive pointer from the given pointer.
	template <class Type2>
	LEAN_INLINE transitive_ptr(const transitive_ptr<Type2> &right)
		: m_object( right.get() ) { };
	
	/// Replaces the stored pointer with the given pointer.
	transitive_ptr& operator =(object_type *object)
	{
		m_object = object;
		return *this;
	}
	/// Replaces the stored pointer with the given pointer.
	template <class Type2>
	transitive_ptr& operator =(const transitive_ptr<Type2> &right)
	{
		return (*this = right.get());
	}

	/// Gets the object stored by this transitive pointer.
	LEAN_INLINE const value_type& get() { return m_object; }
	/// Gets the object stored by this transitive pointer.
	LEAN_INLINE const const_value_type& get() const { return m_object; }

	/// Gets the object stored by this transitive pointer.
	LEAN_INLINE object_type& operator *() { return *get(); }
	/// Gets the object stored by this transitive pointer.
	LEAN_INLINE const object_type& operator *() const { return *get(); }
	
	/// Gets the object stored by this transitive pointer.
	LEAN_INLINE object_type* operator ->() { return get(); }
	/// Gets the object stored by this transitive pointer.
	LEAN_INLINE const object_type* operator ->() const { return get(); }

	/// Gets the object stored by this transitive pointer.
	LEAN_INLINE operator value_type() { return get(); }
	/// Gets the object stored by this transitive pointer.
	LEAN_INLINE operator const_value_type() const { return get(); }
};

} // namespace

using tags::transitive_ptr;

} // namespace

#endif