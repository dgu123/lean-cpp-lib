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

namespace lean
{
namespace properties
{

template <class Class, class Type, size_t MaxCount, utf8_t Delimter>
struct generic_property_type : public property_type<Class>
{
	// Writes the given number of values to the given stream.
	bool write(std::basic_ostream<utf8_t> &stream, const property_getter<Class> &getter, const Class &object, size_t count) const
	{
		Type values[MaxCount];
		count = min(count, MaxCount);
		
		getter(object, values, count);
		
		for (size_t i = 0; i < count; ++i)
		{
			if (i != 0)
				stream << Delimter;

			stream << values[i];
		}

		return stream.fail();
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

		setter(object, values, count);

		return stream.fail();
	}
	// Reads the given number of values from the given range of characters, returning the first character not read.
	const utf8_t* read(const utf8_t *begin, const utf8_t *end, property_setter<Class> &setter, Class &object, size_t count) const
	{
		basic_charstream<utf8_t> stream(begin);
		generic_property_type::read(stream, setter, object, count);
		return stream.read_end();
	}

	/// Writes the given value to the given stream.
	void write(std::basic_ostream<utf8_t> &stream, const void *values, size_t index) const
	{
		stream << static_cast<const Type*>(value)[index];
	}
	/// Writes the given value to the given range of characters.
	utf8_t* write(utf8_t *begin, const void *value, size_t index) const
	{
		return begin;
	}
	/// Reads the given number of values from the given range of characters.
	const utf8_t* read(const utf8_t *begin, const utf8_t *end, void *value, size_t index) const
	{
		basic_charstream<utf8_t> stream(begin, end);
		stream >> static_cast<Type*>(value)[index];
		return begin;
	}

	/// Gets the STD lib typeid.
	const std::type_info& type_info() const { return typeid(Type); }
};

/// Gets the property type info for the given type.
template <class Class, class Type>
LEAN_INLINE const property_type_info& get_generic_property_type()
{
	static generic_property_type<Class, Type> info;
	return info;
}

} // namespace

using properties::generic_property_type;
using properties::get_generic_property_type;

} // namespace

#endif