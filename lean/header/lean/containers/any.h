/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_CONTAINERS_ANY
#define LEAN_CONTAINERS_ANY

#include "../lean.h"
#include "../meta/strip.h"
#include "../meta/type_traits.h"
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
	LEAN_INLINE any& operator =(const any&) { return *this; }

	/// Gets a pointer to the stored value, if the given type matches the value stored by this object, nullptr otherwise.
	virtual void* get_any_ptr(const std::type_info& type) = 0;

public:
	virtual ~any() { }
	
	/// Gets the type of the stored value.
	virtual const std::type_info& type() const = 0;
};

/// Any value.
template <class Value, class UnionValue = Value, class Heap = default_heap>
class any_value : public heap_bound<Heap>, public any
{
private:
	Value m_value;

protected:
	/// Gets a pointer to the stored value, if the given type matches the value stored by this object, nullptr otherwise.
	void* get_any_ptr(const std::type_info& type)
	{
		return (typeid(Value) == type || !is_equal<Value, UnionValue>::value && typeid(UnionValue) == type)
			? &m_value
			: nullptr;
	}

public:
	/// Value type.
	typedef Value value_type;

	/// Constructor.
	LEAN_INLINE any_value()
		: m_value() { }
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
	LEAN_NOINLINE void throw_bad_cast()
	{
		throw std::bad_cast();
	}

} // namespace

/// Gets a value of the given type, if the given value type matches the value stored by the given object, throws bad_cast otherwise.
template <class Value>
LEAN_INLINE Value any_cast_checked(any *container)
{
	typedef typename lean::strip_reference<Value>::type nonref_value_type;
	nonref_value_type *pValue = any_cast<nonref_value_type>(container);
	
	if (!pValue)
		impl::throw_bad_cast();

	return *pValue;
}
/// Gets a value of the given type, if the given value type matches the value stored by the given object, throws bad_cast otherwise.
template <class Value>
LEAN_INLINE Value any_cast_checked(const any *container)
{
	typedef typename lean::strip_reference<Value>::type nonref_value_type;
	return any_cast_checked<const nonref_value_type&>(const_cast<any*>(container));
}
/// Gets a value of the given type, if the given value type matches the value stored by the given object, throws bad_cast otherwise.
template <class Value>
LEAN_INLINE Value any_cast_checked(volatile any *container)
{
	typedef typename lean::strip_reference<Value>::type nonref_value_type;
	return any_cast_checked<volatile nonref_value_type&>(const_cast<any*>(container));
}
/// Gets a value of the given type, if the given value type matches the value stored by the given object, throws bad_cast otherwise.
template <class Value>
LEAN_INLINE Value any_cast_checked(const volatile any *container)
{
	typedef typename lean::strip_reference<Value>::type nonref_value_type;
	return any_cast_checked<const volatile nonref_value_type&>(const_cast<any*>(container));
}

/// Gets a value of the given type, if the given value type matches the value stored by the given object, throws bad_cast otherwise.
template <class Value>
LEAN_INLINE Value any_cast(any &container)
{
	typedef typename lean::strip_reference<Value>::type nonref_value_type;
	return any_cast_checked<nonref_value_type&>(&container);
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

/// Gets a value of the given type, if the given value type matches the value stored by the given object, default otherwise.
template <class Value>
LEAN_INLINE Value any_cast_default(const any *container, const Value &defaultValue = Value())
{
	Value value;
	const Value *pValue = any_cast<Value>(container);
	value = (pValue) ? *pValue : defaultValue;
	return value;
}

/// Gets a value of the given type, if the given value type matches the value stored by the given object, default otherwise.
template <class Value>
LEAN_INLINE Value any_cast_default(const any &container, const Value &defaultValue = Value())
{
	return any_cast_default<Value>(&container, defaultValue);
}

} // namespace

using containers::any;
using containers::any_value;

using containers::any_cast;
using containers::any_cast_checked;
using containers::any_cast_default;

} // namespace

#ifdef DOXYGEN_READ_THIS
	/// @ingroup GlobalSwitches
	/// Define this to disable global any_cast and any_cast_checked templates.
	#define LEAN_NO_ANY_CAST
	#undef LEAN_NO_ANY_CAST
#endif

#ifndef LEAN_NO_ANY_CAST
	using lean::any_cast;
	using lean::any_cast_checked;
#endif

#endif