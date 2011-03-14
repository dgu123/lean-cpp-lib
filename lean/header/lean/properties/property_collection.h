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

template <class Class>
class property_collection
{
private:
	std::vector<property_description> m_properties;

public:
	property_collection();
	property_collection(const property_collection &inherit);

	size_t add()
	{
		size_t id = m_properties.size();
		// TODO: add
		return id;
	}
	
	size_t count() const
	{
		return m_properties.size();
	}

	template <class Value>
	void set(Class &object, size_t id, const Value &value) const
	{
		set(object, id, &value, 1);
	}

	template <class Value>
	void set(Class &object, size_t id, const Value *values, size_t count) const
	{
		if (id < m_properties.size())
			m_properties[id].setter(object, typeid(Value), values, count);
	}

	template <class Value>
	void get(const Class &object, size_t id, Value &value) const
	{
		get(object, id, &value, 1);
	}

	template <class Value>
	void get(const Class &object, size_t id, Value *values, size_t count) const
	{
		if (id < m_properties.size())
			m_properties[id].getter(object, typeid(Value), values, count);
	}

};

} // namespace

using properties::property_collection;

} // namespace

#endif