/*****************************************************/
/* lean Properties              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PROPERTIES_PROPERTY_COLLECTION
#define LEAN_PROPERTIES_PROPERTY_COLLECTION

#include <typeinfo>
#include <vector>
#include "../tags/noncopyable.h"

namespace lean
{
namespace properties
{

template <class Class, class Description>
class property_collection
{
private:
	typedef std::vector<Description> property_vector;
	property_vector m_properties;

public:
	/// Property description type.
	typedef Description property_desc;
	/// Iterator type.
	typedef typename property_vector::const_iterator iterator;
	/// Iterator type.
	typedef typename property_vector::const_iterator const_iterator;

	/// Constructs an empty property collection.
	property_collection() { }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves the contents of the given collection to this collection.
	property_collection(property_collection &&right)
		: m_properties( std::move(right.m_properties) ) { }
#endif

	/// Allows for fully inplace construction of whole property collections.
	template <class Collection = property_collection>
	class inplace_collection
	{
	friend class property_collection;

	protected:
		/// Temporary collection.
		Collection m_collection;

	public:
		/// Adds a property created from the given property description.
		LEAN_INLINE inplace_collection& operator <<(const property_desc& propertyDesc)
		{
			m_collection.add(propertyDesc);
			return *this;
		}
	};
	/// Moves the contents of the given collection to this collection.
	property_collection(inplace_collection<> &right)
	{
		swap(m_properties, right.m_collection.m_properties);
	}
	/// Constructs a new temporary collection that may be filled using operator <<.
	static LEAN_INLINE inplace_collection<> construct_inplace()
	{
		return inplace_collection<>();
	}

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves the contents of the given collection to this collection.
	LEAN_INLINE property_collection& operator =(property_collection &&right)
	{
		m_properties = std::move(right.m_properties);
		return *this;
	}
#endif
	/// Moves the contents of the given collection to this collection.
	LEAN_INLINE property_collection& operator =(inplace_collection<> &right)
	{
		swap(m_properties, right.m_collection.m_properties);
		return *this;
	}

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

	/// Gets a const iterator to the first property description.
	LEAN_INLINE const_iterator begin() const
	{
		return m_properties.begin();
	}
	/// Gets a const iterator one past the last property description.
	LEAN_INLINE const_iterator end() const
	{
		return m_properties.end();
	}

	/// Assigns the given value to the property identified by the given id, fails silently returning false.
	template <class Value>
	LEAN_INLINE bool set(Class &object, size_t id, const Value &value) const
	{
		return set(object, id, &value, 1);
	}
	/// Assigns the given values to the property identified by the given id, fails silently returning false.
	template <class Value>
	LEAN_INLINE bool set(Class &object, size_t id, const Value *values, size_t count) const
	{
		return (id < m_properties.size())
			? set_property(object, m_properties[id].setter, values, count)
			: false;
	}

	/// Reads a value from the property identified by the given id, fails silently returning false.
	template <class Value>
	LEAN_INLINE bool get(const Class &object, size_t id, Value &value) const
	{
		return get(object, id, &value, 1);
	}
	/// Reads values from the property identified by the given id, fails silently returning false.
	template <class Value>
	LEAN_INLINE bool get(const Class &object, size_t id, Value *values, size_t count) const
	{
		return (id < m_properties.size())
			? get_property(object, m_properties[id].getter, values, count)
			: false;
	}

};

} // namespace

using properties::property_collection;

} // namespace

#endif