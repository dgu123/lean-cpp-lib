/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_SIMPLE_HASH_MAP
#define LEAN_CONTAINERS_SIMPLE_HASH_MAP

#include "../lean.h"
#include <memory>
#include <stdexcept>
#include <functional>
#include <utility>

#include <unordered_map>

namespace lean 
{
namespace containers
{

/// Defines construction policies for the class simple_hash_map.
namespace simple_hash_map_policies
{
	/// Simple hash map element construction policy.
	template <bool RawMove = false, bool NoDestruct = false, bool RawKeyMove = RawMove, bool NoKeyDestruct = NoDestruct>
	struct policy
	{
		/// Specifies whether memory containing constructed elements may be moved as a whole, without invoking the contained elements' copy or move constructors.
		static const bool raw_move = RawMove;
		/// Specifies whether memory containing constructed elements may be freed as a whole, without invoking the contained elements' destructors.
		static const bool no_destruct = NoDestruct;
		/// Specifies whether memory containing constructed keys may be moved as a whole, without invoking the contained keys' copy or move constructors.
		static const bool raw_key_move = RawKeyMove;
		/// Specifies whether memory containing constructed keys may be freed as a whole, without invoking the contained keys' destructors.
		static const bool no_key_destruct = NoKeyDestruct;
	};

	/// Default element construction policy.
	typedef policy<> nonpod;
	/// Semi-POD key construction policy (raw move, yet proper destruction).
	typedef policy<false, false, true> semipodkey;
	/// Semi-POD element construction policy (raw move, yet proper destruction).
	typedef policy<true> semipod;
	/// POD key construction policy.
	typedef policy<false, false, true, true> podkey;
	/// POD key / Semi-POD element construction policy.
	typedef policy<true, false, true, true> podkey_semipod;
	/// POD element construction policy.
	typedef policy<true, true> pod;
}

/// Computes hash values from elements of the given type.
template<class Element>
struct hash : public std::unary_function<Element, size_t>
{
	/// Computes a hash value for the given element.
	size_t operator()(const Element &element) const;
};

/// Defines default values for invalid & end keys.
template <class Key>
struct default_keys
{
	/// Invalid key value that is guaranteed never to be used in key-value-pairs.
	static const Key invalid_key;
	/// Valid key value used as end marker. May still be used in actual key-value-pairs.
	static const Key end_key;
};

/// Simple and fast hash map class, partially implementing the STL hash map interface.
template < class Key, class Element,
	class Policy = simple_hash_map_policies::nonpod,
	class Hash = hash<Key>,
	class KeyValues = default_keys<Key>,
    class Pred = std::equal_to<Key>,
	class Allocator = std::allocator<Element> >
class simple_hash_map
{
private:
	typedef std::pair<Key, Element> value_type_;
	
	typedef typename Allocator::template rebind<value_type_>::other allocator_type_;
	allocator_type_ m_allocator;

	value_type_ *m_elements;
	value_type_ *m_elementsEnd;

	typedef typename allocator_type_::size_type size_type_;
	size_type_ m_count;
	size_type_ m_capacity;

	typedef Hash hasher_;
	hasher_ m_hasher;
	typedef Pred key_equal_;
	key_equal_ m_keyEqual;

	static const size_type_ s_maxSize = static_cast<size_type_>(-1) / sizeof(value_type_);
	static const size_type_ s_minSize = (32 < s_maxSize) ? 32 : s_maxSize;

	// Make sure size_type is unsigned
	LEAN_STATIC_ASSERT(s_maxSize > static_cast<size_type_>(0));

	/// Invalidates the given element.
	LEAN_INLINE void invalidate(value_type_ *dest)
	{
		new( addressof(dest->first) ) Key(KeyValues::invalid_key);
	}
	/// Invalidates the given element, terminating the calling application on exception.
	LEAN_NOINLINE void invalidate_or_terminate(value_type_ *dest)
	{
		try
		{
			invalidate(dest);
		}
		catch(...)
		{
			LEAN_ASSERT_DEBUG(false);
			std::terminate();
		}
	}
	/// Prepares the given element for actual data storage.
	LEAN_INLINE void revalidate(value_type_ *dest)
	{
		if (!Policy::no_key_destruct)
			dest->first.~Key();
	}

	/// Default constructs an element at the given location.
	LEAN_INLINE void default_construct(value_type_ *dest, const Key &key)
	{
		try
		{
			revalidate(dest);
			m_allocator.construct(dest, value_type_(key, Element()));
		}
		catch(...)
		{
			invalidate_or_terminate(dest);
			throw;
		}
	}
	/// Copies the given source element to the given destination.
	LEAN_INLINE void copy_construct(value_type_ *dest, const value_type_ &source)
	{
		try
		{
			revalidate(dest);
			m_allocator.construct(dest, source);
		}
		catch(...)
		{
			invalidate_or_terminate(dest);
			throw;
		}
	}
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move_construct(value_type_ *dest, value_type_ &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		try
		{
			revalidate(dest);
			m_allocator.construct(dest, std::move(source));
		}
		catch(...)
		{
			invalidate_or_terminate(dest);
			throw;
		}
#else
		copy_construct(dest, source);
#endif
	}
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move(value_type_ *dest, value_type_ &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		*dest = std::move(source);
#else
		*dest = source;
#endif
	}
	
	/// Destructs the given VALID element.
	LEAN_INLINE void destruct_element(value_type_ *destr)
	{
		if (!Policy::no_destruct)
			m_allocator.destroy(destr);
	}
	/// Destructs both valid and invalid elements in the given range.
	void destruct(Element *destr, Element *destrEnd)
	{
		if (!Policy::no_destruct || !Policy::no_key_destruct)
			for (; destr != destrEnd; ++destr)
				if (destr->first != KeyValues::invalid_key)
					destruct_element(destr);
				else
					revalidate(destr);
	}

	/// Checks whether the given element is physically contained by this hash map.
	LEAN_INLINE bool contains_element(const value_type_ *element) const
	{
		return (m_elements <= element) || (element < m_elementsEnd);
	}
	/// Checks whether the given element is physically contained by this hash map.
	LEAN_INLINE bool contains_element(const value_type_ &element) const
	{
		return contains_element(addressof(element));
	}

	/// Allocates space for the given number of elements.
	void reallocate(size_type_ newCapacity)
	{
		Element *newElements = m_allocator.allocate(newCapacity);

		if (!empty())
			try
			{
				if (Policy::raw_move)
					memcpy(newElements, m_elements, size() * sizeof(Element));
				else
					move_construct(m_elements, m_elementsEnd, newElements);
			}
			catch(...)
			{
				m_allocator.deallocate(newElements, newCapacity);
				throw;
			}

		Element *oldElements = m_elements;
		Element *oldElementsEnd = m_elementsEnd;
		size_type_ oldCapacity = capacity();
		
		// Mind the order, size() based on member variables!
		m_elementsEnd = newElements + size();
		m_capacityEnd = newElements + newCapacity;
		m_elements = newElements;

		if (oldElements)
		{
			// Do nothing on exception, resources leaking anyways!
			if (!Policy::raw_move)
				destruct(oldElements, oldElementsEnd);
			m_allocator.deallocate(oldElements, oldCapacity);
		}
	}

	/// Frees the given elements.
	LEAN_INLINE void free()
	{
		if (m_elements)
		{
			// Do nothing on exception, resources leaking anyways!
			destruct(m_elements, m_elementsEnd);
			m_allocator.deallocate(m_elements, actual_capacity());
		}
	}

	/// Gets the first element that might contain the given key.
	LEAN_INLINE value_type_* first_element(const key_type &key) const
	{
		return m_elements + m_hasher(key) % bucket_count();
	}
	/// Gets the element stored under the given key and returns false if existent, otherwise returns true and gets a fitting open element slot.
	std::pair<bool, value_type_*> locate_element(const key_type &key) const
	{
		Element *element = first_element(key);
		
		while (element->first != KeyValues::invalid_key)
		{
			if (element->first == key)
				return std::make_pair(false, element);
			
			// Wrap around
			if (++element == m_elementsEnd)
				element = m_elements;

			// ASSERT: one slot always remains open, automatically terminating this loop
		}

		return std::make_pair(true, element);
	}
	/// Gets the element stored under the given key, if existent, returns end otherwise.
	LEAN_INLINE value_type_* find_element(const key_type &key) const
	{
		Element *element = first_element(key);
		
		while (element->first != KeyValues::invalid_key)
		{
			if (element->first == key)
				return element;
			
			// Wrap around
			if (++element == m_elementsEnd)
				element = m_elements;

			// ASSERT: one slot always remains open, automatically terminating this loop
		}

		return m_elementsEnd;
	}

	/// Grows hash map storage to fit the given new count.
	LEAN_INLINE void growTo(size_type_ newCount)
	{
		reallocate(capacity_hint(newCount));
	}
	/// Grows hash map storage to fit the given additional number of elements.
	LEAN_INLINE void grow(size_type_ count)
	{
		growTo(size() + count);
	}
	/// Grows hash map storage to fit the given new count, not inlined.
	LEAN_NOINLINE void growToHL(size_type_ newCount)
	{
		growTo(newCount);
	}
	/// Grows hash map storage to fit the given additional number of elements, not inlined.
	LEAN_NOINLINE void growHL(size_type_ count)
	{
		grow(count);
	}

	/// Triggers an out of range error.
	LEAN_NOINLINE static void out_of_range()
	{
		throw std::out_of_range("simple_hash_map<T> out of range");
	}
	/// Checks the given position.
	LEAN_INLINE void check_pos(size_type_ pos) const
	{
		if (pos >= size())
			out_of_range();
	}

public:
	/// Simple hash map iterator class.
	template <class Element>
	class basic_iterator
	{
	friend class simple_hash_map;

	private:
		Element *m_element;

		/// Constructs an iterator from the given element.
		LEAN_INLINE basic_iterator(Element *element)
			: m_element(element) { }
		/// Constructs an iterator from the given element or the next valid element, should the current element prove invalid.
		LEAN_INLINE basic_iterator(Element *element, search_first_valid_t)
			: m_element(
				(element->first == KeyValues::invalid_key)
					? (++basic_iterator(element)).m_element
					: element
				) { }

	public:
		/// Type of the values iterated.
		typedef Element value_type;
		/// Type of references to the values iterated.
		typedef value_type& reference;
		/// Type of pointers to the values iterated.
		typedef value_type& pointer;

		/// Gets the current element.
		LEAN_INLINE reference operator *() const
		{
			return *m_element;
		}
		/// Gets the current element.
		LEAN_INLINE pointer operator ->() const
		{
			return m_element;
		}

		/// Continues iteration.
		LEAN_INLINE reference operator ++()
		{
			do
			{
				++m_element;
			}
			// ASSERT: End element is always valid
			while (m_element->first == KeyValues::invalid_key)
		}
		/// Continues iteration.
		LEAN_INLINE basic_iterator operator ++(int)
		{
			return ++basic_iterator(*this);
		}
	};

	/// Construction policy used.
	typedef Policy construction_policy;

	/// Type of the allocator used by this hash map.
	typedef allocator_type_ allocator_type;
	/// Type of the size returned by this hash map.
	typedef size_type_ size_type;
	/// Type of the difference between the addresses of two elements in this hash map.
	typedef typename allocator_type::difference_type difference_type;

	/// Type of pointers to the elements contained by this hash map.
	typedef typename allocator_type::pointer pointer;
	/// Type of constant pointers to the elements contained by this hash map.
	typedef typename allocator_type::const_pointer const_pointer;
	/// Type of references to the elements contained by this hash map.
	typedef typename allocator_type::reference reference;
	/// Type of constant references to the elements contained by this hash map.
	typedef typename allocator_type::const_reference const_reference;
	/// Type of the elements contained by this hash map.
	typedef typename allocator_type::value_type value_type;
	/// Type of the keys stored by this hash map.
	typedef Key key_type;
	/// Type of the elements contained by this hash map.
	typedef Element mapped_type;

	/// Type of iterators to the elements contained by this hash map.
	typedef basic_iterator<value_type> iterator;
	/// Type of constant iterators to the elements contained by this hash map.
	typedef basic_iterator<const value_type> const_iterator;

	/// Type of the hash function.
	typedef hasher_ hasher;
	/// Type of the key comparison function.
	typedef key_equal_ key_equal;

	/// Constructs an empty hash map.
	explicit simple_hash_map(size_t capacity)
		: m_elements(nullptr),
		m_elementsEnd(nullptr) { }
	/// Constructs an empty hash map.
	explicit simple_hash_map(allocator_type allocator)
		: m_allocator(allocator),
		m_elements(nullptr),
		m_elementsEnd(nullptr) { }
	/// Copies all elements from the given hash map to this hash map.
	simple_hash_map(const simple_hash_map &right)
		: m_allocator(right.m_allocator),
		m_elements(nullptr),
		m_elementsEnd(nullptr)
	{
		assign_disj(right.begin(), right.end());
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given hash map to this hash map.
	simple_hash_map(simple_hash_map &&right)
		: m_allocator(std::move(right.m_allocator)),
		m_elements(std::move(right.m_elements)),
		m_elementsEnd(std::move(right.m_elementsEnd))
	{
		right.m_elements = nullptr;
		right.m_elementsEnd = nullptr;
	}
#endif
	/// Destroys all elements in this hash map.
	~simple_hash_map()
	{
		free();
	}

	/// Copies all elements of the given hash map to this hash map.
	simple_hash_map& operator =(const simple_hash_map &right)
	{
		if (&right != this)
			assign_disj(right.begin(), right.end());
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given hash map to this hash map.
	simple_hash_map& operator =(simple_hash_map &&right)
	{
		if (&right != this)
		{
			free();

			m_elements = std::move(right.m_elements);
			m_elementsEnd = std::move(right.m_elementsEnd);

			right.m_elements = nullptr;
			right.m_elementsEnd = nullptr;

			m_allocator = std::move(right.m_allocator);
		}
		return *this;
	}
#endif

	

	/// Assigns the given range of elements to this hash map.
	template <class Iterator>
	void assign(Iterator source, Iterator sourceEnd)
	{
		LEAN_ASSERT(source <= sourceEnd);

		// Index is unsigned, make use of wrap-around
		if (static_cast<size_type>(addressof(*source) - m_elements) < size())
		{
			LEAN_ASSERT(addressof(*sourceEnd) <= m_elementsEnd);

			move(source, sourceEnd, m_elements);

			Element *oldElementsEnd = m_elementsEnd;
			m_elementsEnd = m_elements + (sourceEnd - source);
			destruct(m_elementsEnd, oldElementsEnd);
		}
		else
			assign_disj(source, sourceEnd);
	}
	/// Assigns the given disjoint range of elements to this hash map.
	template <class Iterator>
	void assign_disj(Iterator source, Iterator sourceEnd)
	{
		// Clear before reallocation to prevent full-range moves
		clear();

		size_type count = sourceEnd - source;

		if (count > capacity())
			growToHL(count);

		copy_construct(source, sourceEnd, m_elements);
		m_elementsEnd = m_elements + count;
	}

	/// Inserts a default-constructed value into the hash map using the given key, if none
	/// stored under the given key yet, otherwise returns the one currently stored.
	LEAN_INLINE reference insert(const key_type &key)
	{
		if (m_count == m_capacity)
			growHL(1);

		std::pair<bool, value_type*> element = locate_element(key);
		
		if (element.first)
		{
			default_construct(element.second, key);
			++m_count;
		}
		return *element.second;
	}
	/// Inserts the given key-value-pair into this hash map.
	LEAN_INLINE std::pair<bool, iterator> insert(const value_type &value)
	{
		if (m_count == m_capacity)
		{
			if (contains_element(value))
				return std::make_pair( false, iterator(const_cast<value_type*>(addressof(value))) );

			growHL(1);
		}

		std::pair<bool, value_type*> element = locate_element(key);

		if (element.first)
		{
			copy_construct(element.second, value);
			++m_count;
		}
		return std::make_pair(element.first, iterator(element.second));
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Inserts the given key-value-pair into this hash map.
	LEAN_INLINE std::pair<bool, iterator> insert(value_type &&value)
	{
		if (m_count == m_capacity)
		{
			if (contains_element(value))
				return std::make_pair( false, iterator(addressof(value)) );

			growHL(1);
		}

		std::pair<bool, value_type*> element = locate_element(key);

		if (element.first)
		{
			move_construct(element.second, value);
			++m_count;
		}
		return std::make_pair(element.first, iterator(element.second));
	}
#endif
	/// Removes the element stored under the given key, if any.
	LEAN_INLINE size_type erase(const key_type &key)
	{
		value_type* element = find_element(key);

		if (element != m_elementsEnd)
		{
			remove_element(element);
			return 1;
		}
		else
			return 0;
	}
	/// Removes the element that the given iterator is pointing to.
	LEAN_INLINE iterator erase(iterator where)
	{
		LEAN_ASSERT(contains_element(where.m_element));

		remove_element(where.m_element);
		
		return (where.m_element->first == KeyValues::invalid_key)
			? ++where
			: where;
	}

	/// Clears all elements from this hash map.
	LEAN_INLINE void clear()
	{
		Element *oldElementsEnd = m_elementsEnd;
		m_elementsEnd = m_elements;
		destruct(m_elements, oldElementsEnd);
	}

	/// Reserves space for the predicted number of elements given.
	LEAN_INLINE void reserve(size_type newCapacity)
	{
		if (newCapacity > capacity())
			reallocate(newCapacity);
	}
	/// Resizes this hash map, either appending empty elements to or removing elements from the back of this hash map.
	void resize(size_type newCount)
	{
		if (newCount > count())
		{
			if (newCount > capacity())
				growToHL(newCount);
			
			Elements *newElementsEnd = m_elements + newCount;
			default_construct(m_elementsEnd, newElementsEnd);
			m_elementsEnd = newElementsEnd;
		}
		else
		{
			Element *oldElementsEnd = m_elementsEnd;
			m_elementsEnd = m_elements + newCount;
			destruct(m_elementsEnd, oldElementsEnd);
		}
	}
	
	/// Gets an element by key, returning end() on failure.
	LEAN_INLINE iterator find(const key_type &key) { return iterator(find_element(key)); }
	/// Gets an element by key, returning end() on failure.
	LEAN_INLINE const_iterator find(const key_type &key) const { return const_iterator(find_element(key)); }

	/// Gets an element by key, inserts a new default-constructed one if none existent yet.
	LEAN_INLINE reference operator [](const key_type &key) { return insert(key); }

	/// Returns an iterator to the first element contained by this hash map.
	LEAN_INLINE iterator begin(void) { return iterator(m_elements, iterator::search_first_valid); }
	/// Returns a constant iterator to the first element contained by this hash map.
	LEAN_INLINE const_iterator begin(void) const { return const_iterator(m_elements, const_iterator::search_first_valid); }
	/// Returns an iterator beyond the last element contained by this hash map.
	LEAN_INLINE iterator end(void) { return iterator(m_elementsEnd); }
	/// Returns a constant iterator beyond the last element contained by this hash map.
	LEAN_INLINE const_iterator end(void) const { return const_iterator(m_elementsEnd); }

	/// Gets a copy of the allocator used by this hash map.
	LEAN_INLINE allocator_type get_allocator() const { return m_allocator; };

	/// Returns true if the hash map is empty.
	LEAN_INLINE bool empty(void) const { return (m_count == 0); };
	/// Returns the number of elements contained by this hash map.
	LEAN_INLINE size_type size(void) const { return m_count; };
	/// Returns the number of elements this hash map could contain without reallocation.
	LEAN_INLINE size_type capacity(void) const { return m_capacity; };
	/// Gets the current number of buckets.
	LEAN_INLINE size_type bucket_count() const { return m_elementsEnd - m_elements; }

	/// Computes a new capacity based on the given number of elements to be stored.
	size_type capacity_hint(size_type count) const
	{
		size_type capacityDelta = count / 2;

		// Assume count <= max size, increase by 1.5 or clamp to max_size, mind overflow
		size_type capacity = (s_maxSize - capacityDelta < count)
			? s_maxSize
			: count + capacityDelta;

		// Handle cases where count is larger than max size?
		if (s_maxSize < count)
			capacity = count;

		if (capacity < s_minSize)
			capacity = s_minSize;
		
		return capacity;
	}

	/// Estimates the maximum number of elements that may be constructed.
	LEAN_INLINE size_type max_size() const
	{
		return s_maxSize;
	}

	/// Swaps the contents of this hash map and the given hash map.
	LEAN_INLINE void swap(simple_hash_map &right) throw()
	{
		swap(m_keyEqual, right.m_keyEqual);
		swap(m_hasher, right.m_hasher);
		swap(m_allocator, right.m_allocator);
		std::swap(m_elements, right.m_elements);
		std::swap(m_elementsEnd, right.m_elementsEnd);
		std::swap(m_count, right.m_count);
		std::swap(m_capacity, right.m_capacity);
	}
};

/// Swaps the contents of the given hash maps.
template <class Element, class Policy, class Allocator>
LEAN_INLINE void swap(simple_hash_map<Element, Policy, Allocator> &left, simple_hash_map<Element, Policy, Allocator> &right)
{
	left.swap(right);
}

} // namespace

namespace simple_hash_map_policies = containers::simple_hash_map_policies;
using containers::simple_hash_map;

} // namespace

#endif