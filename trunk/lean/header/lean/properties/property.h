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
	virtual void operator ()(Class &object, const type_info &type, const void *values, size_t count) = 0;
};

/// Fetches data from a specific source.
template <class Class>
class property_getter : public cloneable
{
public:
	/// Fetches the given number of values of the given type from the given object.
	virtual void operator ()(const Class &object, const type_info &type, void *values, size_t count) const = 0;
};

/// Destribes a property.
template <class Class>
struct property_desc
{
	const std::type_info *type;							///< Element type.
	size_t count;										///< Number of elements.
	
	cloneable_obj< property_setter<Class> > setter;		///< Value setter.
	cloneable_obj< property_getter<Class> > getter;		///< Value getter.
};

/// Describes a named property.
template <class Class>
struct named_property_desc : public property_desc<Class>
{
	std::wstring name;	///< Property name.
};


} // namespace

using properties::property_setter;
using properties::property_getter;

using properties::property_desc;
using properties::named_property_desc;

} // namespace

#endif