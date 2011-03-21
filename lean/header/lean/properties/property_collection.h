/*****************************************************/
/* lean Properties              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PROPERTIES_PROPERTY_COLLECTION
#define LEAN_PROPERTIES_PROPERTY_COLLECTION

#include <cstddef>
#include <typeinfo>
#include <vector>

namespace lean
{
namespace properties
{

template <class Class, class Description>
class property_collection
{
private:
	std::vector<Description> m_properties;

public:
	/// Property description type.
	typedef Description property_desc;

	property_collection();
	property_collection(const property_collection &inherit);

	/// Adds a property created from the given property description.
	LEAN_INLINE size_t add(const property_desc& propertyDesc)
	{
		size_t id = m_properties.size();
		m_properties.push_back(propertyDesc);
		return id;
	}
	/// Gets the property description for the given ID, access violation on failure.
	LEAN_INLINE const property_desc& desc(size_t id) const
	{
		return m_properties[id];
	}
	/// Gets the number of properties stored by this collection.
	LEAN_INLINE size_t count() const
	{
		return m_properties.size();
	}

	/// Assigns the given value to the property identified by the given id, fails silently.
	template <class Value>
	LEAN_INLINE void set(Class &object, size_t id, const Value &value)
	{
		set(object, id, &value, 1);
	}
	/// Assigns the given values to the property identified by the given id, fails silently.
	template <class Value>
	LEAN_INLINE void set(Class &object, size_t id, const Value *values, size_t count)
	{
		if (id < m_properties.size())
		{
			Description &desc = m_properties[id];
			
			if (desc.setter.getptr())
				desc.setter(object, typeid(Value), values, count);
		}
	}

	/// Reads a value from the property identified by the given id, fails silently.
	template <class Value>
	LEAN_INLINE void get(const Class &object, size_t id, Value &value) const
	{
		get(object, id, &value, 1);
	}
	/// Reads values from the property identified by the given id, fails silently.
	template <class Value>
	LEAN_INLINE void get(const Class &object, size_t id, Value *values, size_t count) const
	{
		if (id < m_properties.size())
		{
			const Description &desc = m_properties[id];

			if (desc.getter.getptr())
				desc.getter(object, typeid(Value), values, count);
		}
	}

};

} // namespace

using properties::property_collection;

} // namespace

#endif