/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_SIMPLE_HASH_MAP
#define LEAN_CONTAINERS_SIMPLE_HASH_MAP

#include "../lean.h"
#include "../smart/terminate_guard.h"
#include "../tags/noncopyable.h"
#include "../functional/hashing.h"
#include <memory>
#include <utility>
#include <functional>
#include <limits>
#include <string>

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

namespace impl
{
	template <class Key>
	struct default_numeric_keys
	{
		LEAN_STATIC_ASSERT_MSG_ALT(std::numeric_limits<Key>::is_specialized,
			"No invalid key default available for the given type.",
			No_invalid_key_default_available_for_the_given_type);

		// WORKAROUND: Using methods here circumvents compiler bug that won't initialize some static constant variables
		// when these are copied from to initialize other constant static variables
		LEAN_INLINE static Key invalid_key()
		{
			return (std::numeric_limits<Key>::has_infinity)
				? std::numeric_limits<Key>::infinity()
				: (std::numeric_limits<Key>::min() != Key())
					? std::numeric_limits<Key>::min()
					: std::numeric_limits<Key>::max();
		}
		LEAN_INLINE static Key end_key()
		{
			return Key();
		}
	};

	template <class Key>
	struct default_numeric_keys<Key*>
	{
		LEAN_INLINE static Key* invalid_key()
		{
			return nullptr;
		}
		LEAN_INLINE static Key* end_key()
		{
			return reinterpret_cast<Key*>( static_cast<uintptr_t>(-1) );
		}
	};
}

/// Defines default values for invalid & end keys.
template <class Key>
struct default_keys
{
	/// Invalid key value that is guaranteed never to be used in key-value-pairs.
	static const Key invalid_key;
	/// Valid key value used as end marker. May still be used in actual key-value-pairs.
	static const Key end_key;
	/// Predicate used in key validity checks.
	struct key_equal
	{
		/// Predicate.
		static const std::equal_to<Key> predicate;		
	};
};

// Numeric / generic defaults
template <class Key> const Key default_keys<Key>::invalid_key = impl::default_numeric_keys<Key>::invalid_key();
template <class Key> const Key default_keys<Key>::end_key = impl::default_numeric_keys<Key>::end_key();
template <class Key> const std::equal_to<Key> default_keys<Key>::key_equal::predicate = std::equal_to<Key>();

template <class Char, class Traits, class Allocator>
struct default_keys< std::basic_string<Char, Traits, Allocator> >
{
	typedef std::basic_string<Char, Traits, Allocator> key_type;

	static const key_type invalid_key;
	static const key_type end_key;
	struct key_equal
	{
		static const std::equal_to<key_type> predicate;		
	};
};

// String defaults
template <class Char, class Traits, class Allocator>
const std::basic_string<Char, Traits, Allocator>
	default_keys< std::basic_string<Char, Traits, Allocator> >::invalid_key = std::basic_string<Char, Traits, Allocator>();
template <class Char, class Traits, class Allocator>
const std::basic_string<Char, Traits, Allocator>
	default_keys< std::basic_string<Char, Traits, Allocator> >::end_key = std::basic_string<Char, Traits, Allocator>(1, Char());

namespace impl
{

/// Gets the first prime number available that is both less than or equal to the given maximum value
/// and greater than or equal to the given capacity, if possible.
LEAN_MAYBE_EXPORT size_t next_prime_capacity(size_t capacity, size_t max);

/// Simple hash map base.
template < class Key, class Element,
	class Policy,
	class KeyValues,
	class Allocator >
class simple_hash_map_base
{
protected:
	typedef std::pair<const Key, Element> value_type_;

	typedef typename Allocator::template rebind<value_type_>::other allocator_type_;
	allocator_type_ m_allocator;

	value_type_ *m_elements;
	value_type_ *m_elementsEnd;

	typedef typename allocator_type_::size_type size_type_;
	size_type_ m_count;
	size_type_ m_capacity;

	float m_maxLoadFactor;
	
	static const size_type_ s_maxElements = static_cast<size_type_>(-1) / sizeof(value_type_);
	// Use end element to allow for proper iteration termination
	static const size_type_ s_maxBuckets = s_maxElements - 1U;
	// Keep one slot open at all times to simplify wrapped find loop termination conditions
	static const size_type_ s_maxSize = s_maxBuckets - 1U;
	static const size_type_ s_minSize = (32U < s_maxSize) ? 32U : s_maxSize;

	// Make sure size_type is unsigned
	LEAN_STATIC_ASSERT(s_maxSize > static_cast<size_type_>(0));

	/// Gets the number of buckets required from the given capacity.
	LEAN_INLINE size_type_ buckets_from_capacity(size_type_ capacity)
	{
		LEAN_ASSERT(capacity <= s_maxSize);

		float bucketHint = capacity / m_maxLoadFactor;
		
		size_type_ bucketCount = (bucketHint >= s_maxSize)
			? s_maxSize
			: static_cast<size_type_>(bucketHint);

		// Keep one slot open at all times to simplify wrapped find loop termination conditions
		return max(bucketCount, capacity) + 1U;
	}
	/// Gets the capacity from the given number of buckets.
	LEAN_INLINE size_type_ capacity_from_buckets(size_type_ buckets)
	{
		// Keep one slot open at all times to simplify wrapped find loop termination conditions
		// -> Unsigned overflow handles buckets == 0
		return min(
			static_cast<size_type_>(buckets * m_maxLoadFactor),
			buckets - 1U );
	}

	/// Checks whether the given element is physically contained by this hash map.
	LEAN_INLINE bool contains_element(const value_type_ *element) const
	{
		return (m_elements <= element) && (element < m_elementsEnd);
	}
	/// Checks whether the given element is physically contained by this hash map.
	LEAN_INLINE bool contains_element(const value_type_ &element) const
	{
		return contains_element(lean::addressof(element));
	}

	/// Marks the given element as end element.
	static LEAN_INLINE void mark_end(value_type_ *dest)
	{
		new( const_cast<void*>(static_cast<const void*>(lean::addressof(dest->first))) ) Key(KeyValues::end_key);
	}
	/// Invalidates the given element.
	static LEAN_INLINE void invalidate(value_type_ *dest)
	{
		new( const_cast<void*>(static_cast<const void*>(lean::addressof(dest->first))) ) Key(KeyValues::invalid_key);
	}
	/// Invalidates the elements in the given range.
	static void invalidate(value_type_ *dest, value_type_ *destEnd)
	{
		value_type_ *destr = dest;

		try
		{
			for (; dest != destEnd; ++dest)
				invalidate(dest);
		}
		catch (...)
		{
			destruct_keys(destr, dest);
			throw;
		}
	}
	/// Prepares the given element for actual data storage.
	static LEAN_INLINE void revalidate(value_type_ *dest)
	{
		destruct_key(dest);
	}

	/// Destructs only the key of the given element.
	static LEAN_INLINE void destruct_key(value_type_ *destr)
	{
		if (!Policy::no_key_destruct)
			destr->first.~Key();
	}
	/// Destructs the keys of the elements in the given range.
	static void destruct_keys(value_type_ *destr, value_type_ *destrEnd)
	{
		if (!Policy::no_key_destruct)
			for (; destr != destrEnd; ++destr)
				destruct_key(destr);
	}

	/// Helper class that moves element construction exception handling
	/// into a destructor, resulting in less code being generated and
	/// providing automated handling of unexpected invalidation exceptions.
	class invalidate_guard : public noncopyable
	{
	private:
		value_type_ *m_dest;
		bool m_armed;

	public:
		/// Stores an element to be invalidated on destruction, if not disarmed.
		LEAN_INLINE explicit invalidate_guard(value_type_ *dest, bool armed = true)
			: m_dest(dest),
			m_armed(armed) { }
		/// Destructs the stored element, of not disarmed.
		LEAN_INLINE ~invalidate_guard()
		{
			if (m_armed)
				invalidate(m_dest);
		}
		///  Disarms this guard.
		LEAN_INLINE void disarm() { m_armed = false; }
	};

	/// Helper class that moves element destruction exception handling
	/// into a destructor, resulting in less code being generated and
	/// providing automated handling of unexpected invalidation exceptions.
	class invalidate_n_guard : public noncopyable
	{
	private:
		value_type_ *m_dest;
		value_type_ *m_destEnd;
		bool m_armed;

	public:
		/// Stores elements to be invalidated on destruction, if not disarmed.
		LEAN_INLINE explicit invalidate_n_guard(value_type_ *dest, value_type_ *destEnd, bool armed = true)
			: m_dest(dest),
			m_destEnd(destEnd),
			m_armed(armed) { }
		/// Destructs the stored elements, of not disarmed.
		LEAN_INLINE ~invalidate_n_guard()
		{
			if (m_armed)
				invalidate(m_dest, m_destEnd);
		}
		///  Disarms this guard.
		LEAN_INLINE void disarm() { m_armed = false; }
	};

	/// Default constructs an element at the given location.
	LEAN_INLINE void default_construct(value_type_ *dest, const Key &key)
	{
		invalidate_guard guard(dest);

		revalidate(dest);
		m_allocator.construct(dest, value_type_(key, Element()));

		guard.disarm();
	}
	/// Copies the given source element to the given destination.
	LEAN_INLINE void copy_construct(value_type_ *dest, const value_type_ &source)
	{
		invalidate_guard guard(dest);

		revalidate(dest);
		m_allocator.construct(dest, source);

		guard.disarm();
	}
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move_construct(value_type_ *dest, value_type_ &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		invalidate_guard guard(dest);
		
		revalidate(dest);
		m_allocator.construct(dest, std::move(source));
		
		guard.disarm();
#else
		copy_construct(dest, source);
#endif
	}
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move(value_type_ *dest, value_type_ &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		const_cast<Key&>(dest->first) = std::move(const_cast<Key&>(source.first));
		dest->second = std::move(source.second);
#else
		const_cast<Key&>(dest->first) = const_cast<Key&>(source.first);
		dest->second = source.second;
#endif
	}
	
	/// Destructs the given VALID element.
	LEAN_INLINE void destruct_element(value_type_ *destr)
	{
		if (!Policy::no_destruct)
			m_allocator.destroy(destr);
	}

	/// Initializes the this hash map base.
	LEAN_INLINE explicit simple_hash_map_base(float maxLoadFactor)
		: m_elements(nullptr),
		m_elementsEnd(nullptr),
		m_count(0),
		m_capacity(0),
		m_maxLoadFactor(maxLoadFactor) { }
	/// Initializes the this hash map base.
	LEAN_INLINE simple_hash_map_base(float maxLoadFactor, const allocator_type_ &allocator)
		: m_allocator(allocator),
		m_elements(nullptr),
		m_elementsEnd(nullptr),
		m_count(0),
		m_capacity(0),
		m_maxLoadFactor(maxLoadFactor) { }
	/// Initializes the this hash map base.
	LEAN_INLINE simple_hash_map_base(const simple_hash_map_base &right)
		: m_allocator(right.m_allocator),
		m_elements(nullptr),
		m_elementsEnd(nullptr),
		m_count(0),
		m_capacity(0),
		m_maxLoadFactor(right.m_maxLoadFactor) { }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Initializes the this hash map base.
	LEAN_INLINE simple_hash_map_base(simple_hash_map_base &&right)
		: m_allocator(std::move(right.m_allocator)),
		m_elements(std::move(right.m_elements)),
		m_elementsEnd(std::move(right.m_elementsEnd)),
		m_count(std::move(right.m_count)),
		m_capacity(std::move(right.m_capacity)),
		m_maxLoadFactor(std::move(right.m_maxLoadFactor)) { }
#endif

	/// Does nothing.
	LEAN_INLINE simple_hash_map_base& operator =(const simple_hash_map_base&) { return *this; }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Does nothing.
	LEAN_INLINE simple_hash_map_base& operator =(simple_hash_map_base&&) { return *this; }
#endif

	/// Swaps the contents of this hash map base and the given hash map base.
	LEAN_INLINE void swap(simple_hash_map_base &right) throw()
	{
		swap(m_allocator, right.m_allocator);
		std::swap(m_elements, right.m_elements);
		std::swap(m_elementsEnd, right.m_elementsEnd);
		std::swap(m_count, right.m_count);
		std::swap(m_capacity, right.m_capacity);
		std::swap(m_maxLoadFactor, right.m_maxLoadFactor);
	}
};

}

/// Simple and fast hash map class, partially implementing the STL hash map interface.
template < class Key, class Element,
	class Policy = simple_hash_map_policies::nonpod,
	class Hash = hash<Key>,
	class KeyValues = default_keys<Key>,
    class Pred = std::equal_to<Key>,
	class Allocator = std::allocator<Element> >
class simple_hash_map : private impl::simple_hash_map_base<Key, Element, Policy, KeyValues, Allocator>
{
private:
	typedef impl::simple_hash_map_base<Key, Element, Policy, KeyValues, Allocator> base_type;

	typedef Hash hasher_;
	hasher_ m_hasher;
	typedef Pred key_equal_;
	key_equal_ m_keyEqual;

	/// Destructs both valid and invalid elements in the given range.
	void destruct(value_type_ *destr, value_type_ *destrEnd)
	{
		if (!Policy::no_destruct || !Policy::no_key_destruct)
			for (; destr != destrEnd; ++destr)
				if (key_valid(destr->first))
					destruct_element(destr);
				else
					destruct_key(destr);
	}

	/// Allocates space for the given number of elements.
	void reallocate(size_type_ newBucketCount)
	{
		// Make prime (required for universal modulo hashing)
		// TODO: will lead to constant rehashing when reaching s_maxBuckets
		newBucketCount = impl::next_prime_capacity(newBucketCount, s_maxBuckets);

		LEAN_ASSERT(newBucketCount <= s_maxBuckets);

		// Use end element to allow for proper iteration termination
		const size_type_ newElementCount = newBucketCount + 1U;

		value_type_ *newElements = m_allocator.allocate(newElementCount);
		value_type_ *newElementsEnd = newElements + newBucketCount;

		try
		{
			// ASSERT: End element key always valid to allow for proper iteration termination
			mark_end(newElementsEnd);

			try
			{
				invalidate(newElements, newElementsEnd);
			}
			catch(...)
			{
				destruct_key(newElementsEnd);
				throw;
			}

			if (!empty())
				try
				{
					// ASSERT: One slot always remains open, automatically terminating find loops
					LEAN_ASSERT(size() < newBucketCount);

					for (value_type_ *element = m_elements; element != m_elementsEnd; ++element)
						if (key_valid(element->first))
							move_construct(
								locate_element(element->first, newElements, newElementsEnd, newBucketCount).second,
								*element );
				}
				catch(...)
				{
					destruct(newElements, newElementsEnd);
					destruct_key(newElementsEnd);
					throw;
				}
		}
		catch(...)
		{
			m_allocator.deallocate(newElements, newElementCount);
			throw;
		}
		
		value_type_ *oldElements = m_elements;
		value_type_ *oldElementsEnd = m_elementsEnd;
		const size_type_ oldBucketCount = bucket_count();
		
		m_elements = newElements;
		m_elementsEnd = newElementsEnd;
		m_capacity = capacity_from_buckets(newBucketCount);

		if (oldElements)
			free(oldElements, oldElementsEnd, oldBucketCount + 1U);
	}

	/// Frees the given elements.
	LEAN_INLINE void free(value_type_ *elements, value_type_ *elementsEnd, size_type_ elementCount)
	{
		// ASSERT: End element key always valid to allow for proper iteration termination

		// Do nothing on exception, resources leaking anyways!
		destruct(elements, elementsEnd);
		destruct_key(elementsEnd);
		m_allocator.deallocate(elements, elementCount);
	}

	/// Frees all elements.
	LEAN_INLINE void free()
	{
		if (m_elements)
			free(m_elements, m_elementsEnd, bucket_count() + 1U);
	}

	/// Gets the first element that might contain the given key.
	LEAN_INLINE value_type_* first_element(const Key &key) const
	{
		return first_element(key, m_elements, bucket_count());
	}
	/// Gets the first element that might contain the given key.
	LEAN_INLINE value_type_* first_element(const Key &key, value_type_ *elements, size_type_ bucketCount) const
	{
		return elements + m_hasher(key) % bucketCount;
	}
	/// Gets the element stored under the given key and returns false if existent, otherwise returns true and gets a fitting open element slot.
	LEAN_INLINE std::pair<bool, value_type_*> locate_element(const Key &key) const
	{
		return locate_element(key, m_elements, m_elementsEnd, bucket_count());
	}
	/// Gets the element stored under the given key and returns false if existent, otherwise returns true and gets a fitting open element slot.
	std::pair<bool, value_type_*> locate_element(const Key &key, value_type_ *elements, value_type_ *elementsEnd, size_type_ bucketCount) const
	{
		LEAN_ASSERT(key_valid(key));

		value_type_ *element = first_element(key, elements, bucketCount);
		
		while (key_valid(element->first))
		{
			if (m_keyEqual(element->first, key))
				return std::make_pair(false, element);
			
			// Wrap around
			if (++element == elementsEnd)
				element = elements;

			// ASSERT: One slot always remains open, automatically terminating this loop
		}

		return std::make_pair(true, element);
	}
	/// Gets the element stored under the given key, if existent, returns end otherwise.
	LEAN_INLINE value_type_* find_element(const Key &key) const
	{
		value_type_ *element = first_element(key);
		
		while (key_valid(element->first))
		{
			if (m_keyEqual(element->first, key))
				return element;
			
			// Wrap around
			if (++element == m_elementsEnd)
				element = m_elements;

			// ASSERT: One slot always remains open, automatically terminating this loop
		}

		return m_elementsEnd;
	}
	/// Removes the element stored at the given location.
	LEAN_INLINE void remove_element(value_type_ *element)
	{
		// If anything goes wrong, we won't be able to fix it
		terminate_guard terminateGuard;

		value_type_ *hole = element;
		--m_count;
		
		// Wrap around
		if (++element == m_elementsEnd)
			element = m_elements;

		// Find next empty position
		while (key_valid(element->first))
		{
			value_type_ *auxElement = first_element(element->first);
			
			bool tooLate = (auxElement <= hole);
			bool tooEarly = (element < auxElement);
			bool wrong = (hole <= element) ? (tooLate || tooEarly) : (tooLate && tooEarly);
			
			// Move wrongly positioned elements into hole
			if (wrong)
			{
				move(hole, *element);
				hole = element;
			}
			
			// Wrap around
			if (++element == m_elementsEnd)
				element = m_elements;

			// ASSERT: One slot always remains open, automatically terminating this loop
		}

		destruct_element(hole);
		invalidate(hole);

		terminateGuard.disarm();
	}

	/// Grows hash map storage to fit the given new count.
	LEAN_INLINE void growTo(size_type_ newCount)
	{
		reallocate(buckets_from_capacity(grow_to_capacity_hint(newCount)));
	}
	/// Grows hash map storage to fit the given additional number of elements.
	LEAN_INLINE void grow(size_type_ count)
	{
		// Mind overflow
		LEAN_ASSERT(count <= s_maxSize);
		LEAN_ASSERT(s_maxSize - count >= size());

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

public:
	/// Simple hash map iterator class.
	template <class Element>
	class basic_iterator
	{
	friend class simple_hash_map;

	private:
		Element *m_element;

		/// Allows for the automated validation of iterators on construction.
		enum search_first_valid_t
		{
			/// Allows for the automated validation of iterators on construction.
			search_first_valid
		};

		/// Constructs an iterator from the given element.
		LEAN_INLINE basic_iterator(Element *element)
			: m_element(element) { }
		/// Constructs an iterator from the given element or the next valid element, should the current element prove invalid.
		LEAN_INLINE basic_iterator(Element *element, search_first_valid_t)
			: m_element(
				(!key_valid(element->first))
					? (++basic_iterator(element)).m_element
					: element
				) { }

	public:
		/// Type of the values iterated.
		typedef Element value_type;
		/// Type of references to the values iterated.
		typedef value_type& reference;
		/// Type of pointers to the values iterated.
		typedef value_type* pointer;

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
		LEAN_INLINE basic_iterator& operator ++()
		{
			do
			{
				++m_element;
			}
			// ASSERT: End element key is always valid
			while (!key_valid(m_element->first));

			return *this;
		}
		/// Continues iteration.
		LEAN_INLINE basic_iterator operator ++(int)
		{
			return ++basic_iterator(*this);
		}

		/// Comparison operator.
		LEAN_INLINE bool operator ==(const basic_iterator &right) const
		{
			return (m_element == right.m_element);
		}
		/// Comparison operator.
		LEAN_INLINE bool operator !=(const basic_iterator &right) const
		{
			return (m_element != right.m_element);
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
	simple_hash_map()
		: base_type(0.75f)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		reallocate(s_minSize);
	}
	/// Constructs an empty hash map.
	explicit simple_hash_map(size_type capacity, float maxLoadFactor = 0.75f)
		: base_type(maxLoadFactor)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		growTo(capacity);
	}
	/// Constructs an empty hash map.
	simple_hash_map(size_type capacity, float maxLoadFactor, const hasher& hash)
		: base_type(maxLoadFactor),
		m_hasher(hash)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		growTo(capacity);
	}
	/// Constructs an empty hash map.
	simple_hash_map(size_type capacity, float maxLoadFactor, const hasher& hash, const key_equal& keyComp)
		: base_type(maxLoadFactor),
		m_hasher(hash),
		m_keyEqual(keyComp)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		growTo(capacity);
	}
	/// Constructs an empty hash map.
	simple_hash_map(size_type capacity, float maxLoadFactor, const hasher& hash, const key_equal& keyComp, const allocator_type &allocator)
		: base_type(maxLoadFactor, allocator),
		m_hasher(hash),
		m_keyEqual(keyComp)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		growTo(capacity);
	}
	/// Copies all elements from the given hash map to this hash map.
	simple_hash_map(const simple_hash_map &right)
		: base_type(right),
		m_hasher(right.m_hasher),
		m_keyEqual(right.m_keyEqual)
	{
		assign(right.begin(), right.end());
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given hash map to this hash map.
	simple_hash_map(simple_hash_map &&right)
		: base_type(std::move(right)),
		m_hasher(std::move(right.m_hasher)),
		m_keyEqual(std::move(right.m_keyEqual))
	{
		right.m_elements = nullptr;
		right.m_elementsEnd = nullptr;
		right.m_count = 0;
		right.m_capacity = 0;
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
			assign(right.begin(), right.end());
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

		// Clear before reallocation to prevent full-range moves
		clear();

		size_type count = sourceEnd - source;

		if (count > capacity())
			growToHL(count);

		while (source != sourceEnd)
			insert(*(source++));
	}

	/// Inserts a default-constructed value into the hash map using the given key, if none
	/// stored under the given key yet, otherwise returns the one currently stored.
	LEAN_INLINE reference insert(const key_type &key)
	{
		LEAN_ASSERT(key_valid(key));

		if (m_count == capacity())
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
		LEAN_ASSERT(key_valid(value.first));

		if (m_count == capacity())
		{
			if (contains_element(value))
				return std::make_pair( false, iterator(const_cast<value_type*>(lean::addressof(value))) );

			growHL(1);
		}

		std::pair<bool, value_type*> element = locate_element(value.first);

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
		LEAN_ASSERT(key_valid(value.first));

		if (m_count == capacity())
		{
			if (contains_element(value))
				return std::make_pair( false, iterator(lean::addressof(value)) );

			growHL(1);
		}

		std::pair<bool, value_type*> element = locate_element(value.first);

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
		return iterator(where.m_element, iterator::search_first_valid);
	}

	/// Clears all elements from this hash map.
	LEAN_INLINE void clear()
	{
		// Elements invalidated by guard in all cases
		invalidate_n_guard invalidateGuard(m_elements, m_elementsEnd);
		m_count = 0;

		// Don't handle exceptions, memory leaking anyways
		destruct(m_elements, m_elementsEnd);
	}

	/// Reserves space for the predicted number of elements given.
	LEAN_INLINE void reserve(size_type newCapacity)
	{
		if (newCapacity > capacity())
			reallocate(buckets_from_capacity(newCapacity));
	}
	/// Tries to grow or shrink the hash map to fit the given number of elements given.
	/// The hash map will never shrink below the number of elements currently stored.
	LEAN_INLINE void rehash(size_type newCapacity)
	{
		newCapacity = max(count(), newCapacity);

		if (newCapacity != capacity())
			reallocate(buckets_from_capacity(newCapacity));
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

	/// Returns true if the given key is valid.
	LEAN_INLINE static bool key_valid(const key_type &key) { return !KeyValues::key_equal::predicate(key, KeyValues::invalid_key); }

	/// Returns true if the hash map is empty.
	LEAN_INLINE bool empty(void) const { return (m_count == 0); };
	/// Returns the number of elements contained by this hash map.
	LEAN_INLINE size_type size(void) const { return m_count; };
	/// Returns the number of elements this hash map could contain without reallocation.
	LEAN_INLINE size_type capacity(void) const { return m_capacity; };
	/// Gets the current number of buckets.
	LEAN_INLINE size_type bucket_count() const { return m_elementsEnd - m_elements; }

	/// Gets the maximum load factor.
	LEAN_INLINE float max_load_factor() const { return m_maxLoadFactor; }
	/// Sets the maximum load factor.
	void max_load_factor(float factor)
	{
		m_maxLoadFactor = factor;

		// Make sure capacity never goes below the number of elements currently stored
		// -> Capacity equal count will result in reallocation on next element insertion
		m_capacity = max(
			capacity_from_buckets(bucket_count()),
			count());
	}

	/// Computes a new capacity based on the given number of elements to be stored.
	size_type grow_to_capacity_hint(size_type count) const
	{
		size_type oldCapacity = capacity();
		LEAN_ASSERT(oldCapacity <= s_maxSize);

		// Try to double capacity (mind overflow)
		size_type newCapacity = (s_maxSize - oldCapacity < oldCapacity)
			? 0
			: oldCapacity + oldCapacity;

		// Mind overflow
		LEAN_ASSERT(count <= s_maxSize);
		
		if (newCapacity < count)
			newCapacity = count;
		
		return newCapacity;
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
		base_type::swap(right);
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

#ifdef LEAN_INCLUDE_LINKED
#include "source/simple_hash_map.cpp"
#endif

#endif