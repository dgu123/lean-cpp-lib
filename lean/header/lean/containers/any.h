/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_ANY
#define LEAN_CONTAINERS_ANY

#include "../lean.h"
#include "../meta/strip.h"
#include "../smart/cloneable.h"
#include "../memory/heap_bound.h"
#include <typeinfo>

namespace lean
{
namespace containers
{

/// Any interface.
class any : public cloneable
{
	template <class Value>
	friend Value* any_cast(any*);

protected:
	LEAN_INLINE any(const any&) { }
	LEAN_INLINE any& operator =(const any&) { return *this; }

	/// Gets a pointer to the stored value, if the given type matches the value stored by this object, nullptr otherwise.
	virtual void* get_any_ptr(const std::type_info& type) = 0;

public:
	virtual ~any() { }
	
	/// Gets the type of the stored value.
	virtual const std::type_info& type() const = 0;
};

/// Any value.
template <class Value, class Heap = default_heap>
class any_value : public heap_bound<Heap>, public any
{
private:
	Value m_value;

protected:
	/// Gets a pointer to the stored value, if the given type matches the value stored by this object, nullptr otherwise.
	void* get_any_ptr(const std::type_info& type)
	{
		return (typeid(Value) == type)
			? &m_value
			: nullptr;
	}

public:
	/// Value type.
	typedef Value value_type;

	/// Constructor.
	LEAN_INLINE any_value() { }
	/// Constructor.
	LEAN_INLINE any_value(const value_type &value)
		: m_value(value) { }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Constructor.
	LEAN_INLINE any_value(value_type &&value)
		: m_value(std::move(value)) { }
#endif

	/// Assignment.
	LEAN_INLINE any_value& operator =(const value_type &value)
	{
		m_value = value;
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Assignment.
	LEAN_INLINE any_value& operator =(value_type &&value)
	{
		m_value = std::move(value);
		return *this;
	}
#endif

	/// Gets the stored value.
	LEAN_INLINE value_type& get() { return m_value; }
	/// Gets the stored value.
	LEAN_INLINE const value_type& get() const { return m_value; }
	/// Gets the stored value.
	LEAN_INLINE volatile value_type& get() volatile { return m_value; }
	/// Gets the stored value.
	LEAN_INLINE const volatile value_type& get() const volatile { return m_value; }

	/// Gets the type of the stored value.
	LEAN_INLINE const std::type_info& type() const
	{
		return typeid(value_type);
	}

	/// Clones this value.
	LEAN_INLINE any_value* clone() const { return new any_value(*this); }
	/// Destroys a clone.
	LEAN_INLINE void destroy() const { delete this; }
};

/// Gets a pointer to the value of the given type, if the given value type matches the value stored by the given object, nullptr otherwise.
template <class Value>
LEAN_INLINE Value* any_cast(any *pContainer)
{
	return static_cast<Value*>(
		pContainer
			? pContainer->get_any_ptr(typeid(Value))
			: nullptr );
}
/// Gets a pointer to the value of the given type, if the given value type matches the value stored by the given object, nullptr otherwise.
template <class Value>
LEAN_INLINE const Value* any_cast(const any *pContainer)
{
	return any_cast<const Value>(const_cast<any*>(pContainer));
}
/// Gets a pointer to the value of the given type, if the given value type matches the value stored by the given object, nullptr otherwise.
template <class Value>
LEAN_INLINE volatile Value* any_cast(volatile any *pContainer)
{
	return any_cast<volatile Value>(const_cast<any*>(pContainer));
}
/// Gets a pointer to the value of the given type, if the given value type matches the value stored by the given object, nullptr otherwise.
template <class Value>
LEAN_INLINE const volatile Value* any_cast(const volatile any *pContainer)
{
	return any_cast<const volatile Value>(const_cast<any*>(pContainer));
}

namespace impl
{
	/// Throws a bad_cast exception.
	LEAN_NOLTINLINE void throw_bad_cast()
	{
		throw std::bad_cast();
	}

} // namespace

/// Gets a value of the given type, if the given value type matches the value stored by the given object, throws bad_cast otherwise.
template <class Value>
LEAN_INLINE Value any_cast(any &container)
{
	typedef typename lean::strip_reference<Value>::type nonref_value_type;
	nonref_value_type *pValue = any_cast<nonref_value_type>(&container);
	
	if (!pValue)
		impl::throw_bad_cast();

	return *pValue;
}
/// Gets a value of the given type, if the given value type matches the value stored by the given object, throws bad_cast otherwise.
template <class Value>
LEAN_INLINE Value any_cast(const any &container)
{
	typedef typename lean::strip_reference<Value>::type nonref_value_type;
	return any_cast<const nonref_value_type&>(const_cast<any&>(container));
}
/// Gets a value of the given type, if the given value type matches the value stored by the given object, throws bad_cast otherwise.
template <class Value>
LEAN_INLINE Value any_cast(volatile any &container)
{
	typedef typename lean::strip_reference<Value>::type nonref_value_type;
	return any_cast<volatile nonref_value_type&>(const_cast<any&>(container));
}
/// Gets a value of the given type, if the given value type matches the value stored by the given object, throws bad_cast otherwise.
template <class Value>
LEAN_INLINE Value any_cast(const volatile any &container)
{
	typedef typename lean::strip_reference<Value>::type nonref_value_type;
	return any_cast<const volatile nonref_value_type&>(const_cast<any&>(container));
}

} // namespace

using containers::any;
using containers::any_value;

} // namespace

#endif