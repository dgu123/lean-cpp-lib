/*****************************************************/
/* lean Properties              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PROPERTIES_PROPERTIES
#define LEAN_PROPERTIES_PROPERTIES

#include <string>
#include <typeinfo>
#include "../lean.h"
#include "../smart/cloneable.h"
#include "../smart/cloneable_obj.h"
#include "../meta/strip.h"
#include "../logging/log.h"

namespace lean
{
namespace properties
{

template <class Class>
class property_setter : public cloneable
{
public:
	virtual void operator ()(Class &object, const type_info &type, const void *values, size_t count) = 0;
};

template <class Class>
class property_getter : public cloneable
{
public:
	virtual void operator ()(const Class &object, const type_info &type, void *values, size_t count) const = 0;
};

namespace impl
{

struct property_logging_policy
{
	template <class Property>
	static void type_mismatch(const type_info &value)
	{
		LEAN_LOG_ERROR("Property / value type mismatch: "
			<< typeid(Property).name() << " vs. " << value.name());
	}

	static void count_mismatch(size_t propertyCount, size_t valueCount)
	{
		LEAN_LOG_ERROR("Property / value count mismatch: "
			<< propertyCount << " vs. " << valueCount);
	}
};

typedef property_logging_policy property_error_policy;

} // namespace

#pragma region property_constant

template <class Class, class Value>
class property_constant : public property_getter<Class>
{
private:
	const Value *m_constantValues;
	size_t m_count;

public:
	typedef typename strip_modifiers<Value>::type value_type;

	property_constant(const Value *constantValues, size_t count)
		: m_constantValues(constantValues),
		m_count(count) { };

	void operator ()(const Class &object, const type_info &type, void *values, size_t count) const
	{
		if (type == typeid(value_type))
			std::copy_n(m_constantValues, min(count, m_count), static_cast<value_type*>(values));
		else
			impl::property_error_policy::type_mismatch<value_type>(type);
	}
};

template <class Class, class Value>
LEAN_INLINE property_constant<Class, Value> make_property_constant(const Value *constantValues, size_t count)
{
	return property_constant<Class, Value>(constantValues, count);
}

#pragma endregion

namespace impl
{

template <class UnionValue, class Value>
struct union_helper
{
	LEAN_STATIC_ASSERT_MSG_ALT(
		(sizeof(UnionValue) >= sizeof(Value))
			? (sizeof(UnionValue) % sizeof(Value) == 0)
			: (sizeof(Value) % sizeof(UnionValue) == 0),
		"Size of union type must be positive multiple of value type.",
		Size_of_union_type_must_be_positive_multiple_of_value_type);

	template <class Count>
	static LEAN_INLINE Count union_count(Count count)
	{
		return (sizeof(UnionValue) >= sizeof(Value))
			? count / (sizeof(UnionValue) / sizeof(Value))
			: count * (sizeof(Value) / sizeof(UnionValue));
	}
};

} // namespace

#pragma region property_n_accessors

template <class Class, class UnionValue, class Count, class Return,
	Return (Class::*Setter)(const UnionValue*, Count),
	class Value = UnionValue>
class property_n_setter : public property_setter<Class>
{
public:
	typedef typename strip_modifiers<Value>::type value_type;
	typedef typename strip_modifiers<UnionValue>::type union_type;

	void operator ()(Class &object, const type_info &type, const void *values, size_t count)
	{
		typedef impl::union_helper<union_type, value_type> union_helper;

		if (type == typeid(value_type))
			(object.*Setter)(
					static_cast<const union_type*>( values ),
					static_cast<Count>( union_helper::union_count(count) )
				);
		else
			impl::property_error_policy::type_mismatch<value_type>(type);
	}

	property_n_setter* clone() const { return new property_n_setter(*this); }
	void destroy() const { delete this; }
};

template <class Class, class UnionValue, class Count, class Return,
	Return (Class::*Getter)(UnionValue*, Count) const,
	class Value = UnionValue>
class property_n_getter : public property_getter<Class>
{
public:
	typedef typename strip_modifiers<Value>::type value_type;
	typedef typename strip_modifiers<UnionValue>::type union_type;
	
	void operator ()(const Class &object, const type_info &type, void *values, size_t count) const
	{
		typedef impl::union_helper<union_type, value_type> union_helper;

		if (type == typeid(value_type))
			(object.*Getter)(
					static_cast<union_type*>( values ),
					static_cast<Count>( union_helper::union_count(count) )
				);
		else
			impl::property_error_policy::type_mismatch<value_type>(type);
	}

	property_n_getter* clone() const { return new property_n_getter(*this); }
	void destroy() const { delete this; }
};

namespace impl
{

template <class Class, class UnionValue, class Count, class Return,
	class Value = UnionValue>
struct property_n_accessor_binder
{
	template <Return (Class::*Setter)(const UnionValue*, Count)>
	LEAN_INLINE property_n_setter<Class, UnionValue, Count, Return, Setter, Value> bind_setter()
	{
		return property_n_setter<Class, UnionValue, Count, Return, Setter, Value>();
	};

	template <Return (Class::*Getter)(UnionValue*, Count) const>
	LEAN_INLINE property_n_getter<Class, UnionValue, Count, Return, Getter, Value> bind_getter()
	{
		return property_n_getter<Class, UnionValue, Count, Return, Getter, Value>();
	};
};

template <class Class, class UnionValue, class Count, class Return>
LEAN_INLINE property_n_accessor_binder<Class, UnionValue, Count, Return>
	deduce_accessor_binder(Return (Class::*)(const UnionValue*, Count))
{
	return property_n_accessor_binder<Class, UnionValue, Count, Return>();
}

template <class Class, class UnionValue, class Count, class Return>
LEAN_INLINE property_n_accessor_binder<Class, UnionValue, Count, Return>
	deduce_accessor_binder(Return (Class::*)(UnionValue*, Count) const)
{
	return property_n_accessor_binder<Class, UnionValue, Count, Return>();
}

template <class Value, class Class, class UnionValue, class Count, class Return>
LEAN_INLINE property_n_accessor_binder<Class, UnionValue, Count, Return, Value>
	deduce_accessor_binder(Return (Class::*)(const UnionValue*, Count))
{
	return property_n_accessor_binder<Class, UnionValue, Count, Return, Value>();
}

template <class Value, class Class, class UnionValue, class Count, class Return>
LEAN_INLINE property_n_accessor_binder<Class, UnionValue, Count, Return, Value>
	deduce_accessor_binder(Return (Class::*)(UnionValue*, Count) const)
{
	return property_n_accessor_binder<Class, UnionValue, Count, Return, Value>();
}

} // namespace

#pragma endregion

#pragma region property_c_accessors

namespace impl
{

template <class Class, class ValueArg, int ArgCount = 1, class Return = void>
struct property_c_helper;

template <class Class, class ValueArg, class Return>
struct property_c_helper<Class, ValueArg, 1, Return>
{
	typedef Return (Class::*setter_type)(ValueArg);
	typedef Return (Class::*getter_type)(ValueArg) const;
	static const int argument_count = 1;

	template <class Value>
	static void set(Class &object, setter_type setter, const Value *values)
	{
		(object.*setter)(*values);
	}
	template <class Value>
	static void get(const Class &object, getter_type getter, Value *values)
	{
		(object.*getter)(*values);
	}
};

template <class Class, class ValueArg, class Return>
struct property_c_helper<Class, ValueArg, 2, Return>
{
	typedef Return (Class::*setter_type)(ValueArg, ValueArg);
	typedef Return (Class::*getter_type)(ValueArg, ValueArg) const;
	static const int argument_count = 2;

	template <class Value>
	static void set(Class &object, setter_type setter, const Value *values)
	{
		(object.*setter)(*values, values[1]);
	}
	template <class Value>
	static void get(const Class &object, getter_type getter, Value *values)
	{
		(object.*getter)(*values, values[1]);
	}
};

template <class Class, class ValueArg, class Return>
struct property_c_helper<Class, ValueArg, 3, Return>
{
	typedef Return (Class::*setter_type)(ValueArg, ValueArg, ValueArg);
	typedef Return (Class::*getter_type)(ValueArg, ValueArg, ValueArg) const;
	static const int argument_count = 3;

	template <class Value>
	static void set(Class &object, setter_type setter, const Value *values)
	{
		(object.*setter)(*values, values[1], values[2]);
	}
	template <class Value>
	static void get(const Class &object, getter_type getter, Value *values)
	{
		(object.*getter)(*values, values[1], values[2]);
	}
};

template <class Class, class ValueArg, class Return>
struct property_c_helper<Class, ValueArg, 4, Return>
{
	typedef Return (Class::*setter_type)(ValueArg, ValueArg, ValueArg, ValueArg);
	typedef Return (Class::*getter_type)(ValueArg, ValueArg, ValueArg, ValueArg) const;
	static const int argument_count = 4;

	template <class Value>
	static void set(Class &object, setter_type setter, const Value *values)
	{
		(object.*setter)(*values, values[1], values[2], values[3]);
	}
	template <class Value>
	static void get(const Class &object, getter_type getter, Value *values)
	{
		(object.*getter)(*values, values[1], values[2], values[3]);
	}
};

} // namespace

template <class Class, class UnionValueArg, int ArgCount, class Return,
	typename impl::property_c_helper<Class, UnionValueArg, ArgCount, Return>::setter_type Setter,
	class ValueArg = UnionValueArg>
class property_c_setter : public property_setter<Class>
{
private:
	typedef impl::property_c_helper<Class, UnionValueArg, ArgCount, Return> setter_helper;

public:
	typedef typename strip_modifiers<typename strip_reference<ValueArg>::type>::type value_type;
	typedef typename strip_modifiers<typename strip_reference<UnionValueArg>::type>::type union_type;

	void operator ()(Class &object, const type_info &type, const void *values, size_t count)
	{
		typedef impl::union_helper<union_type, value_type> union_helper;
		
		if (type == typeid(value_type))
		{
			size_t unionCount = union_helper::union_count(count);

			if (unionCount >= setter_helper::argument_count)
				setter_helper::set(object, Setter, static_cast<const union_type*>(values));
			else
				impl::property_error_policy::count_mismatch(setter_helper::argument_count, unionCount);
		}
		else
			impl::property_error_policy::type_mismatch<value_type>(type);
	}

	property_c_setter* clone() const { return new property_c_setter(*this); }
	void destroy() const { delete this; }
};

template <class Class, class UnionValueArg, int ArgCount, class Return,
	typename impl::property_c_helper<Class, UnionValueArg, ArgCount, Return>::getter_type Getter,
	class ValueArg = UnionValueArg>
class property_c_getter : public property_getter<Class>
{
private:
	typedef impl::property_c_helper<Class, UnionValueArg, ArgCount, Return> getter_helper;

public:
	typedef typename strip_modifiers<typename strip_reference<ValueArg>::type>::type value_type;
	typedef typename strip_modifiers<typename strip_reference<UnionValueArg>::type>::type union_type;

	void operator ()(const Class &object, const type_info &type, void *values, size_t count) const
	{
		typedef impl::union_helper<union_type, value_type> union_helper;

		if (type == typeid(value_type))
		{
			size_t unionCount = union_helper::union_count(count);

			if (unionCount >= getter_helper::argument_count)
				getter_helper::get(object, Getter, static_cast<union_type*>(values));
			else
				impl::property_error_policy::count_mismatch(getter_helper::argument_count, unionCount);
		}
		else
			impl::property_error_policy::type_mismatch<value_type>(type);
	}

	property_c_getter* clone() const { return new property_c_getter(*this); }
	void destroy() const { delete this; }
};

namespace impl
{

template <class Class, class UnionValueArg, int ArgCount, class Return,
	class ValueArg = UnionValueArg>
struct property_c_accessor_binder
{
	template <typename impl::property_c_helper<Class, UnionValueArg, ArgCount, Return>::setter_type Setter>
	LEAN_INLINE property_c_setter<Class, UnionValueArg, ArgCount, Return, Setter, ValueArg> bind_setter()
	{
		return property_c_setter<Class, UnionValueArg, ArgCount, Return, Setter, ValueArg>();
	};
	
	template <typename impl::property_c_helper<Class, UnionValueArg, ArgCount, Return>::getter_type Getter>
	LEAN_INLINE property_c_getter<Class, UnionValueArg, ArgCount, Return, Getter, ValueArg> bind_getter()
	{
		return property_c_getter<Class, UnionValueArg, ArgCount, Return, Getter, ValueArg>();
	};
};

template <class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 1, Return>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg))
{
	return property_c_accessor_binder<Class, UnionValueArg, 1, Return>();
}
template <class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 1, Return>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg) const)
{
	return property_c_accessor_binder<Class, UnionValueArg, 1, Return>();
}
template <class ValueArg, class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 1, Return, ValueArg>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg))
{
	return property_c_accessor_binder<Class, UnionValueArg, 1, Return, ValueArg>();
}
template <class ValueArg, class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 1, Return, ValueArg>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg) const)
{
	return property_c_accessor_binder<Class, UnionValueArg, 1, Return, ValueArg>();
}

template <class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 2, Return>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg))
{
	return property_c_accessor_binder<Class, UnionValueArg, 2, Return>();
}
template <class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 2, Return>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg) const)
{
	return property_c_accessor_binder<Class, UnionValueArg, 2, Return>();
}
template <class ValueArg, class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 2, Return, ValueArg>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg))
{
	return property_c_accessor_binder<Class, UnionValueArg, 2, Return, ValueArg>();
}
template <class ValueArg, class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 2, Return, ValueArg>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg) const)
{
	return property_c_accessor_binder<Class, UnionValueArg, 2, Return, ValueArg>();
}

template <class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 3, Return>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg, UnionValueArg))
{
	return property_c_accessor_binder<Class, UnionValueArg, 3, Return>();
}
template <class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 3, Return>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg, UnionValueArg) const)
{
	return property_c_accessor_binder<Class, UnionValueArg, 3, Return>();
}
template <class ValueArg, class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 3, Return, ValueArg>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg, UnionValueArg))
{
	return property_c_accessor_binder<Class, UnionValueArg, 3, Return, ValueArg>();
}
template <class ValueArg, class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 3, Return, ValueArg>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg, UnionValueArg) const)
{
	return property_c_accessor_binder<Class, UnionValueArg, 3, Return, ValueArg>();
}

template <class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 4, Return>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg, UnionValueArg, UnionValueArg))
{
	return property_c_accessor_binder<Class, UnionValueArg, 4, Return>();
}
template <class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 4, Return>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg, UnionValueArg, UnionValueArg) const)
{
	return property_c_accessor_binder<Class, UnionValueArg, 4, Return>();
}
template <class ValueArg, class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 4, Return, ValueArg>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg, UnionValueArg, UnionValueArg))
{
	return property_c_accessor_binder<Class, UnionValueArg, 4, Return, ValueArg>();
}
template <class ValueArg, class Class, class UnionValueArg, class Return>
LEAN_INLINE property_c_accessor_binder<Class, UnionValueArg, 4, Return, ValueArg>
	deduce_accessor_binder(Return (Class::*)(UnionValueArg, UnionValueArg, UnionValueArg, UnionValueArg) const)
{
	return property_c_accessor_binder<Class, UnionValueArg, 4, Return, ValueArg>();
}

} // namespace

#pragma endregion

#pragma region property_r_accessors

template <class Class, class UnionValueReturn,
	UnionValueReturn (Class::*Getter)() const,
	class ValueReturn = UnionValueReturn>
class property_r_getter : public property_getter<Class>
{
public:
	typedef typename strip_modifiers<typename strip_reference<ValueReturn>::type>::type value_type;
	typedef typename strip_modifiers<typename strip_reference<UnionValueReturn>::type>::type union_type;

	void operator ()(const Class &object, const type_info &type, void *values, size_t count) const
	{
		typedef impl::union_helper<union_type, value_type> union_helper;

		if (type == typeid(value_type))
		{
			size_t unionCount = union_helper::union_count(count);

			if (unionCount >= 1)
				*static_cast<union_type*>(values) = (object.*Getter)();
			else
				impl::property_error_policy::count_mismatch(1, unionCount);
		}
		else
			impl::property_error_policy::type_mismatch<value_type>(type);
	}

	property_r_getter* clone() const { return new property_r_getter(*this); }
	void destroy() const { delete this; }
};

namespace impl
{

template <class Class, class UnionValueReturn,
	class ValueReturn = UnionValueReturn>
struct property_r_accessor_binder
{
	template <UnionValueReturn (Class::*Getter)() const>
	LEAN_INLINE property_r_getter<Class, UnionValueReturn, Getter, ValueReturn> bind_getter()
	{
		return property_c_getter<Class, UnionValueReturn, Getter, ValueReturn>();
	};
};

template <class Class, class UnionValueReturn>
LEAN_INLINE property_r_accessor_binder<Class, UnionValueReturn>
	deduce_accessor_binder(UnionValueReturn (Class::*Getter)() const)
{
	return property_r_accessor_binder<Class, UnionValueReturn>();
}

template <class Class, class UnionValueReturn, class ValueReturn>
LEAN_INLINE property_r_accessor_binder<Class, UnionValueReturn, ValueReturn>
	deduce_accessor_binder(UnionValueReturn (Class::*Getter)() const)
{
	return property_r_accessor_binder<Class, UnionValueReturn, ValueReturn>();
}

} // namespace

#pragma endregion

} // namespace

using properties::property_setter;
using properties::property_getter;

using properties::make_property_constant;

} // namespace

#define LEAN_MAKE_PROPERTY_CONSTANT(constants, count) lean::properties::make_property_constant(constants, count)

#define LEAN_MAKE_PROPERTY_SETTER(setter) lean::properties::impl::deduce_accessor_binder(setter).bind_setter<setter>()
#define LEAN_MAKE_PROPERTY_GETTER(getter) lean::properties::impl::deduce_accessor_binder(getter).bind_getter<getter>()

#define LEAN_MAKE_PROPERTY_SETTER_UNION(setter, value_type) lean::properties::impl::deduce_accessor_binder<value_type>(setter).bind_setter<setter>()
#define LEAN_MAKE_PROPERTY_GETTER_UNION(getter, value_type) lean::properties::impl::deduce_accessor_binder<value_type>(getter).bind_getter<getter>()

#endif