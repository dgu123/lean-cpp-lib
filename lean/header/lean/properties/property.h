/*****************************************************/
/* lean Properties              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PROPERTIES_PROPERTY
#define LEAN_PROPERTIES_PROPERTY

#include <string>
#include <typeinfo>
#include "../lean.h"
#include "../smart/cloneable.h"
#include "../smart/cloneable_obj.h"
#include "../meta/non_void.h"

namespace lean
{
namespace properties
{

/// Passes data to a specific destination.
template <class Class>
class property_setter : public cloneable
{
public:
	/// Passes the given values of the given type to the given object.
	virtual bool operator ()(Class &object, const type_info &type, const void *values, size_t count) = 0;

	/// Passes the given values to the given object.
	template <class Value>
	LEAN_INLINE bool operator ()(Class &object, const Value *values, size_t count)
	{
		return (*this)(object, typeid(Value), values, count);
	}
};

/// Fetches data from a specific source.
template <class Class>
class property_getter : public cloneable
{
public:
	/// Fetches the given number of values of the given type from the given object.
	virtual bool operator ()(const Class &object, const type_info &type, void *values, size_t count) const = 0;

	/// Fetches the given number of values from the given object.
	template <class Value>
	LEAN_INLINE bool operator ()(const Class &object, Value *values, size_t count) const
	{
		return (*this)(object, typeid(Value), values, count);
	}
};

/// Destribes a property.
template <class Class, class Derived = void>
struct property_desc
{
	/// Type of the most derived structure.
	typedef typename non_void<Derived, property_desc>::type full_type;

	const std::type_info *type;					///< Element type.
	size_t count;								///< Number of elements.

	/// Setter type.
	typedef property_setter<Class> setter_type;
	typedef cloneable_obj<setter_type, true> setter_storage_type;
	/// Getter type.
	typedef property_getter<Class> getter_type;
	typedef cloneable_obj<getter_type, true> getter_storage_type;

	setter_storage_type setter;	///< Value setter.
	getter_storage_type getter;	///< Value getter.

	/// Constructs an empty property destriction.
	property_desc()
		: type(nullptr),
		count(0),
		setter(setter_storage_type::null()),
		getter(getter_storage_type::null()) { }
	/// Constructs a property destriction from the given parameters.
	property_desc(const std::type_info &type, size_t count)
		: type(&type),
		count(count),
		setter(setter_storage_type::null()),
		getter(getter_storage_type::null()) { }

	/// Sets the setter.
	full_type& set_setter(const setter_type &setter) { this->setter = setter; return static_cast<full_type&>(*this); }
	/// Sets the getter.
	full_type& set_getter(const getter_type &getter) { this->getter = getter; return static_cast<full_type&>(*this); }
};

/// Describes a named property.
template <class Class, class Derived = void>
struct named_property_desc : public property_desc<Class, typename non_void< Derived, named_property_desc<Class, Derived> >::type>
{
	std::wstring name;	///< Property name.

	/// Constructs an empty property destriction.
	named_property_desc() { }
	/// Constructs a property destriction from the given parameters.
	named_property_desc(const std::wstring &name, const std::type_info &type, size_t count)
		: property_desc<Class, full_type>(type, count),
		name(name) { }
};

/// Describes a UI property.
template <class Class, class Widget, class Derived = void>
struct ui_property_desc : public named_property_desc<Class, typename non_void< Derived, ui_property_desc<Class, Widget, Derived> >::type>
{
	Widget widget;	///< UI widget used to display/edit this property.

	/// Value getter type.
	typedef property_getter<Class> value_type;
	typedef cloneable_obj<value_type, true> value_storage_type;

	value_storage_type default_value;	///< Default value getter.

	value_storage_type min_value;		///< Min value getter.
	value_storage_type value_step;		///< Value step getter.
	value_storage_type max_value;		///< Max value getter.

	/// Constructs an empty property destriction.
	ui_property_desc()
		: default_value(value_storage_type::null()),
		min_value(value_storage_type::null()),
		value_step(value_storage_type::null()),
		max_value(value_storage_type::null()) { }
	/// Constructs a property destriction from the given parameters.
	ui_property_desc(const std::wstring &name, const std::type_info &type, size_t count, const Widget &widget)
		: named_property_desc<Class, full_type>(name, type, count),
		widget(widget),
		default_value(value_storage_type::null()),
		min_value(value_storage_type::null()),
		value_step(value_storage_type::null()),
		max_value(value_storage_type::null()) { }

	/// Sets the default value getter.
	full_type& set_default_value(const value_type &getter) { this->default_value = getter; return static_cast<full_type&>(*this); }
	/// Sets the min value getter.
	full_type& set_min_value(const value_type &getter) { this->min_value = getter; return static_cast<full_type&>(*this); }
	/// Sets the value step getter.
	full_type& set_value_step(const value_type &getter) { this->value_step = getter; return static_cast<full_type&>(*this); }
	/// Sets the max value getter.
	full_type& set_max_value(const value_type &getter) { this->max_value = getter; return static_cast<full_type&>(*this); }
};

/// Passes the given values to the given object using the given setter.
template <class Class, class Value>
LEAN_INLINE bool set_property(Class &object, property_setter<Class> *setter, const Value *values, size_t count)
{
	return (setter) ? (*setter)(object, values, count) : false;
}
/// Passes the given values to the given object using the given setter.
template <class Class, class Value>
LEAN_INLINE bool set_property(Class &object, cloneable_obj<property_setter<Class>, false> &setter, const Value *values, size_t count)
{
	return (*setter)(object, values, count);
}
/// Passes the given values to the given object using the given setter.
template <class Class, class Value>
LEAN_INLINE bool set_property(Class &object, const cloneable_obj<property_setter<Class>, true> &setter, const Value *values, size_t count)
{
	return set_property(object, setter.getptr(), values, count);
}

/// Fetches the given number of values from the given object.
template <class Class, class Value>
LEAN_INLINE bool get_property(const Class &object, const property_getter<Class> *getter, Value *values, size_t count)
{
	return (getter) ? (*getter)(object, values, count) : false;
}
/// Fetches the given number of values from the given object.
template <class Class, class Value, bool PointerSem>
LEAN_INLINE bool get_property(const Class &object, const cloneable_obj<property_getter<Class>, PointerSem> &getter, Value *values, size_t count)
{
	return (PointerSem)
		? get_property(object, getter.getptr(), values, count)
		: (*getter)(object, values, count);
}

} // namespace

using properties::property_setter;
using properties::property_getter;

using properties::property_desc;
using properties::named_property_desc;
using properties::ui_property_desc;

using properties::get_property;
using properties::set_property;

} // namespace

#endif