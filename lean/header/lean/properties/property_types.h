/*****************************************************/
/* lean Properties              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PROPERTIES_PROPERTY_TYPES
#define LEAN_PROPERTIES_PROPERTY_TYPES

#include "../lean.h"
#include "property.h"
#include <typeinfo>
#include <iostream>
#include "../strings/charstream.h"
#include "../io/numeric.h"

namespace lean
{
namespace properties
{

template <class Class, class Type, size_t MaxCount, utf8_t Delimiter = ';'>
struct generic_property_type : public property_type<Class>
{
	/// Gets the maximum length of the given number of values when serialized. Zero if unpredictable.
	size_t max_length(size_t count) const
	{
		return 0;
	}
	// Writes the given number of values to the given stream.
	bool write(std::basic_ostream<utf8_t> &stream, const property_getter<Class> &getter, const Class &object, size_t count) const
	{
		Type values[MaxCount];
		count = min(count, MaxCount);
		
		if (!getter(object, values, count))
			return false;
		
		for (size_t i = 0; i < count; ++i)
		{
			if (i != 0)
				stream << Delimiter;

			stream << values[i];
		}

		return !stream.fail();
	}
	// Writes the given number of values to the given character buffer, returning the first character not written to.
	utf8_t* write(utf8_t *begin, const property_getter<Class> &getter, const Class &object, size_t count) const
	{
		basic_charstream<utf8_t> stream(begin);
		generic_property_type::write(stream, getter, object, count);
		return stream.write_end();
	}

	// Reads the given number of values from the given stream.
	bool read(std::basic_istream<utf8_t> &stream, property_setter<Class> &setter, Class &object, size_t count) const
	{
		Type values[MaxCount] = { Type() };
		count = min(count, MaxCount);
		
		for (size_t i = 0; i < count; ++i)
		{
			if (i != 0)
				stream.ignore(std::numeric_limits<std::streamsize>::max(), Delimiter);

			stream >> values[i];
		}

		return setter(object, values, count) && !stream.fail();
	}
	// Reads the given number of values from the given range of characters, returning the first character not read.
	const utf8_t* read(const utf8_t *begin, const utf8_t *end, property_setter<Class> &setter, Class &object, size_t count) const
	{
		basic_charstream<utf8_t> stream(begin);
		generic_property_type::read(stream, setter, object, count);
		return stream.read_end();
	}

	/// Gets the STD lib typeid.
	const std::type_info& type_info() const { return typeid(Type); }
};

/// Gets the property type info for the given type.
template <class Class, class Type, size_t MaxCount>
LEAN_INLINE const property_type& get_generic_property_type()
{
	static generic_property_type<Class, Type, MaxCount> info;
	return info;
}

template <class Class, class Type, size_t MaxCount, utf8_t Delimiter = ';'>
struct int_property_type : public generic_property_type<Class, Type, MaxCount, Delimiter>
{
	/// Gets the maximum length of the given number of values when serialized. Zero if unpredictable.
	size_t max_length(size_t count) const
	{
		// N numbers & delimiters
		return (max_int_string_length<Type>::value + 1) * count;
	}
	// Writes the given number of values to the given character buffer, returning the first character not written to.
	utf8_t* write(utf8_t *begin, const property_getter<Class> &getter, const Class &object, size_t count) const
	{
		Type values[MaxCount];
		count = min(count, MaxCount);
		
		if (!getter(object, values, count))
			return begin;
		
		for (size_t i = 0; i < count; ++i)
		{
			if (i != 0)
				*(begin++) = Delimiter;

			begin = int_to_char(begin, values[i]);
		}

		return begin;
	}

	// Reads the given number of values from the given range of characters, returning the first character not read.
	const utf8_t* read(const utf8_t *begin, const utf8_t *end, property_setter<Class> &setter, Class &object, size_t count) const
	{
		Type values[MaxCount] = { Type() };
		count = min(count, MaxCount);

		for (size_t i = 0; i < count; ++i)
		{
			if (i != 0)
				while (begin != end && *(begin++) != Delimiter);

			begin = char_to_int(begin, end, values[i]);
		}

		setter(object, values, count);

		return begin;
	}
};

/// Gets the property type info for the given type.
template <class Class, class Type, size_t MaxCount>
LEAN_INLINE const property_type& get_int_property_type()
{
	static int_property_type<Class, Type, MaxCount> info;
	return info;
}

template <class Class, class Type, size_t MaxCount, utf8_t Delimiter = ';'>
struct float_property_type : public generic_property_type<Class, Type, MaxCount, Delimiter>
{
	/// Gets the maximum length of the given number of values when serialized. Zero if unpredictable.
	size_t max_length(size_t count) const
	{
		// N numbers & delimiters
		return (max_float_string_length<Type>::value + 1) * count;
	}
	// Writes the given number of values to the given character buffer, returning the first character not written to.
	utf8_t* write(utf8_t *begin, const property_getter<Class> &getter, const Class &object, size_t count) const
	{
		Type values[MaxCount];
		count = min(count, MaxCount);
		
		if (!getter(object, values, count))
			return begin;
		
		for (size_t i = 0; i < count; ++i)
		{
			if (i != 0)
				*(begin++) = Delimiter;

			begin = float_to_char(begin, values[i]);
		}

		return begin;
	}

	// Reads the given number of values from the given range of characters, returning the first character not read.
	const utf8_t* read(const utf8_t *begin, const utf8_t *end, property_setter<Class> &setter, Class &object, size_t count) const
	{
		Type values[MaxCount] = { Type() };
		count = min(count, MaxCount);

		for (size_t i = 0; i < count; ++i)
		{
			if (i != 0)
				while (begin != end && *(begin++) != Delimiter);

			begin = char_to_float(begin, end, values[i]);
		}

		setter(object, values, count);

		return begin;
	}
};

/// Gets the property type info for the given type.
template <class Class, class Type, size_t MaxCount>
LEAN_INLINE const property_type& get_float_property_type()
{
	static float_property_type<Class, Type, MaxCount> info;
	return info;
}

} // namespace

using properties::generic_property_type;
using properties::get_generic_property_type;

using properties::int_property_type;
using properties::get_int_property_type;

using properties::float_property_type;
using properties::get_float_property_type;

} // namespace

#endif