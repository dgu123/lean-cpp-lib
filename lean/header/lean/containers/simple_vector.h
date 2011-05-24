/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_SIMPLE_VECTOR
#define LEAN_CONTAINERS_SIMPLE_VECTOR

#include "../lean.h"
#include <memory>
#include <stdexcept>

namespace lean 
{
namespace containers
{

/// Defines construction policies for the class simple_vector.
namespace simple_vector_policies
{
	/// Simple vector element construction policy.
	template <bool RawMove = false, bool NoDestruct = false>
	struct policy
	{
		/// Specifies whether memory containing constructed elements may be moved as a whole, without invoking the contained elements' copy or move constructors.
		static const bool raw_move = RawMove;
		/// Specifies whether memory containing constructed elements may be freed as a whole, without invoking the contained elements' destructors.
		static const bool no_destruct = NoDestruct;
	};

	/// Default element construction policy.
	typedef policy<> nonpod;
	/// Semi-POD element construction policy (raw move, yet proper destruction).
	typedef policy<true> semipod;
	/// POD element construction policy.
	typedef policy<true, true> pod;
}

/// Simple and fast vector class, partially implementing the STL vector interface.
template < class Element, class Policy = simple_vector_policies::nonpod, class Allocator = std::allocator<Element> >
class simple_vector
{
private:
	typedef typename Allocator::template rebind<Element>::other allocator_type_;
	allocator_type_ m_allocator;

	Element *m_elements;
	Element *m_elementsEnd;
	Element *m_capacityEnd;

	typedef typename allocator_type_::size_type size_type_;
	static const size_type_ s_maxSize = static_cast<size_type_>(-1) / sizeof(Element);
	static const size_type_ s_minSize = (16 < s_maxSize) ? 16 : s_maxSize;

	// Make sure size_type is unsigned
	LEAN_STATIC_ASSERT(s_maxSize > static_cast<size_type_>(0));

	/// Default constructs an element at the given location.
	LEAN_INLINE void default_construct(Element *dest)
	{
		m_allocator.construct(dest, Element());
	}
	/// Default constructs elements in the given range.
	void default_construct(Element *dest, Element *destEnd)
	{
		Element *destr = dest;

		try
		{
			for (; dest != destEnd; ++dest)
				default_construct(dest);
		}
		catch(...)
		{
			destruct(destr, dest);
			throw;
		}
	}
	/// Copies the given source element to the given destination.
	LEAN_INLINE void copy_construct(Element *dest, const Element &source)
	{
		m_allocator.construct(dest, source);
	}
	/// Copies elements from the given source range to the given destination.
	template <class Iterator>
	void copy_construct(Iterator source, Iterator sourceEnd, Element *dest)
	{
		Element *destr = dest;

		try
		{
			for (; source != sourceEnd; ++dest, ++source)
				copy_construct(dest, *source);
		}
		catch(...)
		{
			destruct(destr, dest);
			throw;
		}
	}
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move_construct(Element *dest, Element &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		m_allocator.construct(dest, std::move(source));
#else
		copy_construct(dest, source);
#endif
	}
	/// Moves elements from the given source range to the given destination.
	void move_construct(Element *source, Element *sourceEnd, Element *dest)
	{
		Element *destr = dest;

		try
		{
			for (; source != sourceEnd; ++dest, ++source)
				move_construct(dest, *source);
		}
		catch(...)
		{
			destruct(destr, dest);
			throw;
		}
	}
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move(Element *dest, Element &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		*dest = std::move(source);
#else
		*dest = source;
#endif
	}
	/// Moves elements from the given source range to the given destination.
	void move(Element *source, Element *sourceEnd, Element *dest)
	{
		for (; source != sourceEnd; ++dest, ++source)
			move(dest, *source);
	}

	/// Destructs the elements in the given range.
	LEAN_INLINE void destruct(Element *destr)
	{
		if (!Policy::no_destruct)
			m_allocator.destroy(destr);
	}
	/// Destructs the elements in the given range.
	void destruct(Element *destr, Element *destrEnd)
	{
		if (!Policy::no_destruct)
			for (; destr != destrEnd; ++destr)
				destruct(destr);
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
			m_allocator.deallocate(m_elements, capacity());
		}
	}

	/// Grows vector storage to fit the given new count.
	LEAN_INLINE void growTo(size_type_ newCount, bool checkLength = true)
	{
		// Mind overflow
		if (checkLength)
			check_length(newCount);

		reallocate(next_capacity_hint(newCount));
	}
	/// Grows vector storage to fit the given additional number of elements.
	LEAN_INLINE void grow(size_type_ count)
	{
		size_type_ oldSize = size();

		// Mind overflow
		if (count > s_maxSize || s_maxSize - count < oldSize)
			length_exceeded();

		growTo(oldSize + count, false);
	}
	/// Grows vector storage and inserts the given element at the end of the vector.
	LEAN_INLINE Element& grow_and_relocate(Element &value)
	{
		size_type_ index = lean::addressof(value) - m_elements;
		grow(1);
		
		// Index is unsigned, make use of wrap-around
		return (index < size())
			? m_elements[index]
			: value;
	}

	/// Grows vector storage to fit the given new count, not inlined.
	LEAN_NOINLINE void growToHL(size_type_ newCount)
	{
		growTo(newCount);
	}
	/// Grows vector storage to fit the given additional number of elements, not inlined.
	LEAN_NOINLINE void growHL(size_type_ count)
	{
		grow(count);
	}
	/// Grows vector storage and inserts the given element at the end of the vector.
	LEAN_NOINLINE void grow_and_pushHL(const Element &value)
	{
		copy_construct(m_elementsEnd, grow_and_relocate(const_cast<Element&>(value)));
		++m_elementsEnd;
	}
	/// Grows vector storage and inserts the given element at the end of the vector.
	LEAN_NOINLINE void grow_and_pushHL(Element &&value)
	{
		move_construct(m_elementsEnd, grow_and_relocate(value));
		++m_elementsEnd;
	}

	/// Triggers an out of range error.
	LEAN_NOINLINE static void out_of_range()
	{
		throw std::out_of_range("simple_vector<T> out of range");
	}
	/// Checks the given position.
	LEAN_INLINE void check_pos(size_type_ pos) const
	{
		if (pos >= size())
			out_of_range();
	}
	/// Triggers a length error.
	LEAN_NOINLINE static void length_exceeded()
	{
		throw std::length_error("simple_vector<T> too long");
	}
	/// Checks the given length.
	LEAN_INLINE static void check_length(size_type_ count)
	{
		if (count > s_maxSize)
			length_exceeded();
	}

public:
	/// Construction policy used.
	typedef Policy construction_policy;

	/// Type of the allocator used by this vector.
	typedef allocator_type_ allocator_type;
	/// Type of the size returned by this vector.
	typedef size_type_ size_type;
	/// Type of the difference between the addresses of two elements in this vector.
	typedef typename allocator_type::difference_type difference_type;

	/// Type of pointers to the elements contained by this vector.
	typedef typename allocator_type::pointer pointer;
	/// Type of constant pointers to the elements contained by this vector.
	typedef typename allocator_type::const_pointer const_pointer;
	/// Type of references to the elements contained by this vector.
	typedef typename allocator_type::reference reference;
	/// Type of constant references to the elements contained by this vector.
	typedef typename allocator_type::const_reference const_reference;
	/// Type of the elements contained by this vector.
	typedef typename allocator_type::value_type value_type;

	/// Type of iterators to the elements contained by this vector.
	typedef pointer iterator;
	/// Type of constant iterators to the elements contained by this vector.
	typedef const_pointer const_iterator;

	/// Constructs an empty vector.
	simple_vector()
		: m_elements(nullptr),
		m_elementsEnd(nullptr),
		m_capacityEnd(nullptr) { }
	/// Constructs an empty vector.
	explicit simple_vector(allocator_type allocator)
		: m_allocator(allocator),
		m_elements(nullptr),
		m_elementsEnd(nullptr),
		m_capacityEnd(nullptr) { }
	/// Copies all elements from the given vector to this vector.
	simple_vector(const simple_vector &right)
		: m_allocator(right.m_allocator),
		m_elements(nullptr),
		m_elementsEnd(nullptr),
		m_capacityEnd(nullptr)
	{
		assign_disj(right.begin(), right.end());
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given vector to this vector.
	simple_vector(simple_vector &&right)
		: m_allocator(std::move(right.m_allocator)),
		m_elements(std::move(right.m_elements)),
		m_elementsEnd(std::move(right.m_elementsEnd)),
		m_capacityEnd(std::move(right.m_capacityEnd))
	{
		right.m_elements = nullptr;
		right.m_elementsEnd = nullptr;
		right.m_capacityEnd = nullptr;
	}
#endif
	/// Destroys all elements in this vector.
	~simple_vector()
	{
		free();
	}

	/// Copies all elements of the given vector to this vector.
	simple_vector& operator =(const simple_vector &right)
	{
		if (&right != this)
			assign_disj(right.begin(), right.end());
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given vector to this vector.
	simple_vector& operator =(simple_vector &&right)
	{
		if (&right != this)
		{
			free();

			m_elements = std::move(right.m_elements);
			m_elementsEnd = std::move(right.m_elementsEnd);
			m_capacityEnd = std::move(right.m_capacityEnd);

			right.m_elements = nullptr;
			right.m_elementsEnd = nullptr;
			right.m_capacityEnd = nullptr;

			m_allocator = std::move(right.m_allocator);
		}
		return *this;
	}
#endif

	/// Assigns the given range of elements to this vector.
	template <class Iterator>
	void assign(Iterator source, Iterator sourceEnd)
	{
		LEAN_ASSERT(source <= sourceEnd);

		Element *sourceElements = const_cast<Element*>( lean::addressof(*source) );

		// Index is unsigned, make use of wrap-around
		if (static_cast<size_type>(sourceElements - m_elements) < size())
		{
			Element *sourceElementsEnd = const_cast<Element*>( lean::addressof(*sourceEnd) );
			LEAN_ASSERT(sourceElementsEnd <= m_elementsEnd);

			// Move (always back to front)
			LEAN_ASSERT(m_elements <= sourceElements);
			move(sourceElements, sourceElementsEnd, m_elements);

			Element *oldElementsEnd = m_elementsEnd;
			m_elementsEnd = m_elements + (sourceElementsEnd - sourceElements);
			destruct(m_elementsEnd, oldElementsEnd);
		}
		else
			assign_disj(source, sourceEnd);
	}
	/// Assigns the given disjoint range of elements to this vector.
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

	/// Appends a default-constructed element to this vector.
	LEAN_INLINE reference push_back()
	{
		if (m_elementsEnd == m_capacityEnd)
			growHL(1);

		default_construct(m_elementsEnd);
		return *(m_elementsEnd++);
	}
	/// Appends the given element to this vector.
	LEAN_INLINE void push_back(const value_type &value)
	{
		if (m_elementsEnd == m_capacityEnd)
			grow_and_pushHL(value);
		else
		{
			copy_construct(m_elementsEnd, value);
			++m_elementsEnd;
		}
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Appends the given element to this vector.
	LEAN_INLINE void push_back(value_type &&value)
	{
		if (m_elementsEnd == m_capacityEnd)
			grow_and_pushHL(std::move(value));
		else
		{
			move_construct(m_elementsEnd, value);
			++m_elementsEnd;
		}
	}
#endif
	/// Removes the last element from this vector.
	LEAN_INLINE void pop_back()
	{
		LEAN_ASSERT(!empty());

		destruct(m_elementsEnd--);
	}

	/// Clears all elements from this vector.
	LEAN_INLINE void clear()
	{
		Element *oldElementsEnd = m_elementsEnd;
		m_elementsEnd = m_elements;
		destruct(m_elements, oldElementsEnd);
	}

	/// Reserves space for the predicted number of elements given.
	LEAN_INLINE void reserve(size_type newCapacity)
	{
		// Mind overflow
		check_length(newCapacity);

		if (newCapacity > capacity())
			reallocate(newCapacity);
	}
	/// Resizes this vector, either appending empty elements to or removing elements from the back of this vector.
	void resize(size_type newCount)
	{
		// Mind overflow
		check_length(newCapacity);

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
	
	/// Gets an element by position, access violation on failure.
	LEAN_INLINE reference at(size_type pos) { check_pos(pos); return m_elements[pos]; };
	/// Gets an element by position, access violation on failure.
	LEAN_INLINE const_reference at(size_type pos) const { check_pos(pos); return m_elements[pos]; };
	/// Gets the first element in the vector, access violation on failure.
	LEAN_INLINE reference front(void) { LEAN_ASSERT(!empty()); return *m_elements; };
	/// Gets the first element in the vector, access violation on failure.
	LEAN_INLINE const_reference front(void) const { LEAN_ASSERT(!empty()); return *m_elements; };
	/// Gets the last element in the vector, access violation on failure.
	LEAN_INLINE reference back(void) { LEAN_ASSERT(!empty()); return m_elementsEnd[-1]; };
	/// Gets the last element in the vector, access violation on failure.
	LEAN_INLINE const_reference back(void) const { LEAN_ASSERT(!empty()); return m_elementsEnd[-1]; };

	/// Gets an element by position, access violation on failure.
	LEAN_INLINE reference operator [](size_type pos) { return m_elements[pos]; };
	/// Gets an element by position, access violation on failure.
	LEAN_INLINE const_reference operator [](size_type pos) const { return m_elements[pos]; };

	/// Returns an iterator to the first element contained by this vector.
	LEAN_INLINE iterator begin(void) { return m_elements; };
	/// Returns a constant iterator to the first element contained by this vector.
	LEAN_INLINE const_iterator begin(void) const { return m_elements; };
	/// Returns an iterator beyond the last element contained by this vector.
	LEAN_INLINE iterator end(void) { return m_elementsEnd; };
	/// Returns a constant iterator beyond the last element contained by this vector.
	LEAN_INLINE const_iterator end(void) const { return m_elementsEnd; };

	/// Gets a copy of the allocator used by this vector.
	LEAN_INLINE allocator_type get_allocator() const { return m_allocator; };

	/// Returns true if the vector is empty.
	LEAN_INLINE bool empty(void) const { return (m_elements == m_elementsEnd); };
	/// Returns the number of elements contained by this vector.
	LEAN_INLINE size_type size(void) const { return m_elementsEnd - m_elements; };
	/// Returns the number of elements this vector could contain without reallocation.
	LEAN_INLINE size_type capacity(void) const { return m_capacityEnd - m_elements; };

	/// Computes a new capacity based on the given number of elements to be stored.
	size_type next_capacity_hint(size_type count) const
	{
		size_type oldCapacity = capacity();
		LEAN_ASSERT(oldCapacity <= s_maxSize);
		size_type capacityDelta = oldCapacity / 2;

		// Try to increase capacity by 1.5 (mind overflow)
		size_type capacity = (s_maxSize - capacityDelta < oldCapacity)
			? 0
			: oldCapacity + capacityDelta;

		if (capacity < count)
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

	/// Swaps the contents of this vector and the given vector.
	LEAN_INLINE void swap(simple_vector &right) throw()
	{
		using std::swap;

		swap(m_allocator, right.m_allocator);
		swap(m_elements, right.m_elements);
		swap(m_elementsEnd, right.m_elementsEnd);
		swap(m_capacityEnd, right.m_capacityEnd);
	}
};

/// Swaps the contents of the given vectors.
template <class Element, class Policy, class Allocator>
LEAN_INLINE void swap(simple_vector<Element, Policy, Allocator> &left, simple_vector<Element, Policy, Allocator> &right)
{
	left.swap(right);
}

} // namespace

namespace simple_vector_policies = containers::simple_vector_policies;
using containers::simple_vector;

} // namespace

#endif