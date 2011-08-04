/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_ACCUMULATION_MAP
#define LEAN_CONTAINERS_ACCUMULATION_MAP

#include "../lean.h"
#include <utility>

namespace lean
{
namespace containers
{
	
/// Map wrapper class providing improved performance on repeated element accumulation.
/** This class stores elements of the given type without destructing them on remove
  * or clear, thus eliminating the overhead incurred by destruction and re-construction
  * of frequently removed elements.
  * @see lean::containers::accumulation_vector */
template <class Container>
class accumulation_map
{
public:
	/// Type of the container wrapped by this map.
	typedef Container container_type;
	/// Type of the allocator used by this map.
	typedef typename container_type::allocator_type allocator_type;
	/// Type of the size returned by this map.
	typedef typename container_type::size_type size_type;
	/// Type of the difference between the addresses of two elements in this map.
	typedef typename container_type::difference_type difference_type;

	/// Type of pointers to the elements contained by this map.
	typedef typename container_type::pointer pointer;
	/// Type of constant pointers to the elements contained by this map.
	typedef typename container_type::const_pointer const_pointer;
	/// Type of references to the elements contained by this map.
	typedef typename container_type::reference reference;
	/// Type of constant references to the elements contained by this map.
	typedef typename container_type::const_reference const_reference;
	/// Type of the elements contained by this map.
	typedef typename container_type::value_type value_type;

	/// Type of iterators to the elements contained by this map.
	typedef typename container_type::iterator iterator;
	/// Type of constant iterators to the elements contained by this map.
	typedef typename container_type::const_iterator const_iterator;

	/// Type of reverse iterators to the elements contained by this map.
	typedef typename container_type::reverse_iterator reverse_iterator;
	/// Type of constant reverse iterators to the elements contained by this map.
	typedef typename container_type::const_reverse_iterator const_reverse_iterator;

	/// Type of the function object used to compare the keys of two elements contained by this map.
	typedef typename container_type::key_compare key_compare;
	/// Type of the keys used to compare two elements contained by this map.
	typedef typename container_type::key_type key_type;
	/// Type of the values mapped to by this map.
	typedef typename container_type::mapped_type mapped_type;
	/// Type of the function object used to compare two elements contained by this map.
	typedef typename container_type::value_compare value_compare;

private:
	// Container
	container_type m_container;

	// Invalid marker
	mapped_type m_invalidValue;

	/// Assigns an invalid value to the given value object.
	LEAN_INLINE void invalidate(mapped_type &value) const
	{
		// Const method modifier makes m_invalidValue const
		value = m_invalidValue;
	}

public:
	/// Constructs an empty accumulation map.
	accumulation_map() { }
	/// Constructs an accumulation map containing the given key comparison predicate.
	explicit accumulation_map(const key_compare& predicate)
		: m_container(predicate) { }
	/// Constructs an accumulation map containing the given key comparison predicate using the given allocator.
	accumulation_map(const key_compare& predicate, const allocator_type& allocator)
		: m_container(predicate, allocator) { }
	/// Copies elements from the given map to this map.
	accumulation_map(const accumulation_map& right)
		: m_container(right.m_container) { }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves elements from the given map to this map.
	accumulation_map(accumulation_map&& right)
		: m_container(std::move(right.m_container)) { }
#endif
	/// Constructs an accumulation map from the given range of elements.
	template<class Iterator>
	accumulation_map(Iterator itFirst, Iterator itEnd)
		: m_container(itFirst, itEnd) { }
	/// Constructs an accumulation map from the given range of elements using the given key comparison predicate.
	template<class Iterator>
	accumulation_map(Iterator itFirst, Iterator itEnd, const key_compare& predicate)
		: m_container(itFirst, itEnd, predicate) { }
	/// Constructs an accumulation map from the given range of elements using the given key comparison predicate and allocator.
	template<class Iterator>
	accumulation_map(Iterator itFirst, Iterator itEnd, const key_compare& predicate, const allocator_type& allocator)
		: m_container(itFirst, itEnd, predicate, allocator) { }

	/// Copies all elements from the given map to this map.
	accumulation_map& operator =(const accumulation_map &right)
	{
		if (this != &right)
		{
			m_invalidValue = right.m_invalidValue;

			key_compare keyComp = m_container.key_comp();

			iterator itDest = m_container.begin();
			const_iterator itSource = right.begin();

			while (itSource != right.end() && itDest != m_container.end())
			{
				// Replace old value with new one on exact match
				if (itSource->first == itDest->first)
					(itDest++)->second = (itSource++)->second;
				// Wait for next source element, if still less than dest
				else if (keyComp(itSource->first, itDest->first))
					m_container.insert(itDest, *(itSource++));
				// Source element already greater than dest, invalidate dest
				else
					invalidate((itDest++)->second);
			}

			// Invalidate remaining old elements
			while (itDest != m_container.end())
				invalidate((itDest++)->second);

			// Append remaining source elements
			while (itSource != right.end())
				m_container.insert(m_container.end(), *(itSource++));
		}
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given map to this map.
	inline accumulation_map& operator =(accumulation_map &&right)
	{
		m_container = std::move(right.m_container);
		return *this;
	}
#endif

	/// Sets a new invalid element marker value to be used. Leaves previously marked elements untouched, best used when container is empty!
	LEAN_INLINE void inv_elem(const mapped_type &invalid) { m_invalidValue = invalid; }
	/// Gets a copy of the invalid element marker value currently in use.
	LEAN_INLINE mapped_type inv_elem(void) { return m_invalidValue; }

	/// Gets the number of elements contained by this map.
	LEAN_INLINE size_type size(void) const { return m_container.size(); };
	/// Checks if this map is empty.
	LEAN_INLINE bool empty(void) const { return m_container.empty(); };

	/// Inserts the given element into this map.
	LEAN_INLINE std::pair<iterator, bool> insert(const value_type &element) { return m_container.insert(element); }
	/// Inserts the given element into this map using the given positioning hint.
	LEAN_INLINE iterator insert(iterator itWhere, const value_type &element) { return m_container.insert(itWhere, element); }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Inserts the given element into this map.
	LEAN_INLINE std::pair<iterator, bool> insert(value_type &&element) { return m_container.insert(std::move(element)); }
	/// Inserts the given element into this map using the given positioning hint.
	LEAN_INLINE iterator insert(iterator itWhere, value_type &&element) { return m_container.insert(itWhere, std::move(element)); }
#endif
	/// Inserts the given range of elements into this map.
	template<class Iterator>
	LEAN_INLINE void insert(Iterator itFirst, Iterator itEnd) { m_container.insert(itFirst, itEnd); }

	/// Invalidates an element in this map by key.
	LEAN_INLINE size_type erase(const key_type &key)
	{
		iterator itElem = m_container.find(key);

		if (itElem != m_container.end())
		{
			invalidate(itElem->second);
			return 1;
		}
		else
			return 0;
	}
	/// Invalidates an element in this map by iterator.
	LEAN_INLINE iterator erase(iterator itWhere)
	{
		invalidate(itWhere->second);
		return itWhere;
	}
	/// Invalidates the given range of elements in this map.
	LEAN_INLINE void erase(iterator itFirst, iterator itEnd)
	{
		while (itFirst != itEnd)
			invalidate((itFirst++)->second);
	}

	/// Removes an element from this map by key.
	LEAN_INLINE size_type erase_fully(const key_type &key) { return m_container.erase(key); }
	/// Removes an element from this map by iterator.
	LEAN_INLINE iterator erase_fully(iterator itWhere) { return m_container.erase(itWhere); }
	/// Removes the given range of elements from this map.
	LEAN_INLINE void erase_fully(iterator itFirst, iterator itEnd) { m_container.erase(itFirst, itEnd); }

	/// Invalidates all elements in this map.
	LEAN_INLINE void clear(void)
	{
		// Invalidate all elements
		for (iterator itElem = begin(); itElem != end(); ++itElem)
			invalidate(itElem->second);
	}
	/// Removes all elements from this map.
	LEAN_INLINE void reset(void) { m_container.clear(); }

	/// Gets a comparison object that can be used to compare map keys.
	LEAN_INLINE key_compare key_comp(void) const { return m_container.key_comp(); }
	/// Gets a comparison object that can be used to compare map values.
	LEAN_INLINE value_compare value_comp(void) const { return m_container.value_comp(); }

	/// Gets a copy of the allocator used by this map.
	LEAN_INLINE allocator_type get_allocator() const { return m_container.get_allocator(); }
	/// Returns the maximum number of elements this map could hold.
	LEAN_INLINE size_type max_size(void) const { return m_container.max_size(); }

	/// Gets the first element whose key is greater than or equal to the given key.
	LEAN_INLINE iterator lower_bound(const key_type &key) { return m_container.lower_bound(key); }
	/// Gets the first element whose key is greater than or equal to the given key.
	LEAN_INLINE const_iterator lower_bound(const key_type &key) const { return m_container.lower_bound(key); }
	/// Gets the first element whose key is greater than the given key.
	LEAN_INLINE iterator upper_bound(const key_type &key) { return m_container.upper_bound(key); }
	/// Gets the first element whose key is greater than the given key.
	LEAN_INLINE const_iterator upper_bound(const key_type &key) const { return m_container.upper_bound(key); }
	/// Gets the range of elements from lower_bound to upper_bound.
	LEAN_INLINE std::pair<iterator, iterator> equal_range(const key_type &key) { return m_container.equal_range(key); }
	/// Gets the range of elements from lower_bound to upper_bound.
	LEAN_INLINE std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const { return m_container.equal_range(key); }

	/// Gets an element by key.
	LEAN_INLINE iterator find(const key_type &key) { return m_container.find(key); }
	/// Gets an element by key.
	LEAN_INLINE const_iterator find(const key_type &key) const { return m_container.find(key); }
	/// Gets the number of elements matching the given key.
	LEAN_INLINE size_type count(const key_type &key) const { return m_container.count(key); }

	/// Gets an element by key, inserting one if none existent so far.
	LEAN_INLINE mapped_type& operator [](const key_type &key) { return m_container[key]; }

	/// Returns an iterator to the first element contained by this map.
	LEAN_INLINE iterator begin(void) { return m_container.begin(); }
	/// Returns a constant iterator to the first element contained by this map.
	LEAN_INLINE const_iterator begin(void) const { return m_container.begin(); }
	/// Returns an iterator beyond the last element contained by this map.
	LEAN_INLINE iterator end(void) { return m_container.end(); }
	/// Returns a constant iterator beyond the last element contained by this map.
	LEAN_INLINE const_iterator end(void) const { return m_container.end(); }

	/// Returns a reverse iterator to the last element contained by this map.
	LEAN_INLINE reverse_iterator rbegin(void) { return m_container.rbegin(); }
	/// Returns a constant reverse iterator to the last element contained by this map.
	LEAN_INLINE const_reverse_iterator rbegin(void) const { return m_container.rbegin(); }
	/// Returns a reverse iterator beyond the first element contained by this map.
	LEAN_INLINE reverse_iterator rend(void) { return m_container.rend(); }
	/// Returns a constant reverse iterator beyond the first element contained by this map.
	LEAN_INLINE const_reverse_iterator rend(void) const { return m_container.rend(); }

	// Swaps the elements of two accumulation maps.
	LEAN_INLINE void swap(accumulation_map& right)
	{
		using std::swap;

		swap(m_container, right.m_container);
	}
};

// Swaps the elements of two accumulation maps.
template<class Container>
LEAN_INLINE void swap(accumulation_map<Container>& left, accumulation_map<Container>& right)
{
	left.swap(right);
}

} // namespace

using containers::accumulation_map;

} // namespace

#endif