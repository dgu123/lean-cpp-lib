/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_SIMPLE_VECTOR
#define LEAN_CONTAINERS_SIMPLE_VECTOR

#include "../lean.h"
#include <memory>

namespace lean 
{
namespace containers
{

// TODO: atomic support

template < class Element, class Allocator = std::allocator<Element> >
class simple_vector
{
private:
	typedef typename Allocator::template rebind<Element>::other allocator_type_;
	allocator_type_ m_allocator;

	Element *m_elements;
	size_t m_count;
	size_t m_capacity;

	static const size_t s_max_size = reinterpret_cast<size_t>(-1) / sizeof(Element);

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
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move_construct(Element *source, Element *dest)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		m_allocator.construct(dest, std::move(*source));
#else
		m_allocator.construct(dest, *source);
#endif
	}
	/// Moves elements from the given source range to the given destination.
	void move_construct(Element *source, Element *sourceEnd, Element *dest)
	{
		Element *destr = dest;

		try
		{
			for (; source != sourceEnd; ++dest, ++source)
				move_construct(dest, source);
		}
		catch(...)
		{
			destruct(destr, dest);
			throw;
		}
	}

	/// Destructs the elements in the given range.
	LEAN_INLINE void destruct(Element *destr)
	{
		m_allocator.destroy(*destr);
	}
	/// Destructs the elements in the given range.
	void destruct(Element *destr, Element *destrEnd)
	{
		for (; destr != destrEnd; ++destr)
			destruct(destr);
	}

	/// Allocates space for the given number of elements.
	LEAN_NOINLINE void reallocate(size_t newCapacity)
	{
		Element *newElements = m_allocator.allocate(newCapacity);

		if (m_elements)
			try
			{
				move_construct(m_elements, m_elements + m_count, newElements);
			}
			catch(...)
			{
				m_allocator.deallocate(newElements, newCapacity);
				throw;
			}

		Element *oldElements = m_elements;
		size_t oldCapacity = m_capacity;
		
		m_elements = newElements;
		m_capacity = newCapacity;

		if (oldElements)
		{
			try
			{
				destruct(oldElements, oldElements + m_count);
			}
			catch(...)
			{
				m_allocator.deallocate(oldElements, oldCapacity);
				throw;
			}

			m_allocator.deallocate(oldElements, oldCapacity);
		}
	}

public:
	/// Type of the allocator used by this vector.
	typedef allocator_type_ allocator_type;
	/// Type of the size returned by this vector.
	typedef typename allocator_type::size_type size_type;
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
		m_count(0),
		m_capacity(0) { }
	/// Constructs an empty vector.
	simple_vector(allocator_type allocator)
		: m_allocator(allocator),
		m_elements(nullptr),
		m_count(0),
		m_capacity(0) { }
	/// Destroys all elements in this vector.
	~simple_vector()
	{
		if (m_elements)
		{
			try
			{
				destruct(m_elements, m_elements + m_count);
			}
			catch(...)
			{
				m_allocator.deallocate(m_elements, m_capacity);
				throw;
			}

			m_allocator.deallocate(m_elements, m_capacity);
		}
	}

	// TODO: copy, assignment (+ move)

	/// Appends the given element to this vector.
	LEAN_INLINE void push_back(const value_type &value)
	{
		if (m_count >= m_capacity)
			reallocate(capacity_hint(m_count + 1));

		m_allocator.construct(m_elements + m_count, value);
		++m_count;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Appends the given element to this vector.
	LEAN_INLINE void push_back(value_type &&value)
	{
		if (m_count >= m_capacity)
			reallocate(capacity_hint(m_count + 1));

		m_allocator.construct(m_elements + m_count, std::forward<value_type>(value));
		++m_count;
	}
#endif
	/// Removes the last element from this vector.
	LEAN_INLINE void pop_back()
	{
		if (m_count > 0)
		{
			--m_count;
			destruct(m_elements + m_count);
		}

		// TODO: exception?
	}

	/// Clears all elements from this vector.
	LEAN_INLINE void clear()
	{
		if (m_count > 0)
		{
			Element *elementsOldEnd = m_elements + m_count;
			m_count = 0;
			destruct(m_elements, elementsOldEnd);
		}
	}

	/// Reserves space for the predicted number of elements given.
	LEAN_INLINE void reserve(size_t newCapacity)
	{
		if (newCapacity > m_capacity)
			reallocate(newCapacity);
	}
	/// Resizes this vector, either appending empty elements to or removing elements from the back of this vector.
	void resize(size_t newCount)
	{
		if (newCount > m_count)
		{
			if (newCount > m_capacity)
				reallocate(capacity_hint(newCount));
			
			default_construct(m_elements + m_count, m_elements + newCount);
			m_count = newCount;
		}
		// Handle newCount == m_count == 0 (m_elements might still contain nullptr)
		else if (newCount < m_count)
		{
			Element *elementsOldEnd = m_elements + m_count;
			m_count = newCount;
			destruct(m_elements + newCount, elementsOldEnd);
		}
	}

	// TODO: exceptions?

	/// Gets an element by position, access violation on failure.
	LEAN_INLINE reference at(size_type pos) { return m_elements[pos]; };
	/// Gets an element by position, access violation on failure.
	LEAN_INLINE const_reference at(size_type pos) const { return m_elements[pos]; };
	/// Gets the first element in the vector, access violation on failure.
	LEAN_INLINE reference front(void) { return *m_elements; };
	/// Gets the first element in the vector, access violation on failure.
	LEAN_INLINE const_reference front(void) const { return *m_elements; };
	/// Gets the last element in the vector, access violation on failure.
	LEAN_INLINE reference back(void) { return m_elements[m_count - 1]; };
	/// Gets the last element in the vector, access violation on failure.
	LEAN_INLINE const_reference back(void) const { return m_elements[m_count - 1]; };

	/// Gets an element by position, access violation on failure.
	LEAN_INLINE reference operator [](size_type pos) { return m_elements[pos]; };
	/// Gets an element by position, access violation on failure.
	LEAN_INLINE const_reference operator [](size_type pos) const { return m_elements[pos]; };

	// TODO: check compilation behavior for repeated end() calls

	/// Returns an iterator to the first element contained by this vector.
	LEAN_INLINE iterator begin(void) { return m_elements; };
	/// Returns a constant iterator to the first element contained by this vector.
	LEAN_INLINE const_iterator begin(void) const { return m_elements; };
	/// Returns an iterator beyond the last element contained by this vector.
	LEAN_INLINE iterator end(void) { return m_elements + m_count; };
	/// Returns a constant iterator beyond the last element contained by this vector.
	LEAN_INLINE const_iterator end(void) const { return m_elements + m_count; };

	/// Gets a copy of the allocator used by this vector.
	LEAN_INLINE allocator_type get_allocator() const { return m_allocator; };

	/// Returns the number of elements contained by this vector.
	LEAN_INLINE size_type size(void) const { return m_count; };
	/// Returns the number of elements this vector could contain without reallocation.
	LEAN_INLINE size_type capacity(void) const { return m_capacity; };

	/// Computes a new capacity based on the given number of elements to be stored.
	LEAN_NOINLINE size_t capacity_hint(size_t count) const
	{
		size_t capacityDelta = count / 2;

		// Assume count <= max size, increase by 1.5 or clamp to max_size, mind overflow
		size_t capacity = (s_max_size - capacityDelta < count)
			? s_max_size
			: count + capacityDelta;

		// Handle cases where count is larger than max size?
		if (s_max_size < count)
			capacity = count;
		
		return capacity;
	}

	/// Estimates the maximum number of elements that may be constructed.
	LEAN_INLINE size_t max_size() const
	{
		return s_max_size;
	}

	/// Swaps the contents of this vector and the given vector.
	LEAN_INLINE void swap(simple_vector &right) throw()
	{
		std::swap(m_allocator, right.m_allocator);
		std::swap(m_elements, right.m_elements);
		std::swap(m_count, right.m_count);
		std::swap(m_capacity, right.m_capacity);
	}
};

/// Swaps the contents of the given vectors.
template <class Element, class Allocator>
LEAN_INLINE void swap(simple_vector<Element, Allocator> &left, simple_vector<Element, Allocator> &right)
{
	left.swap(right);
}

} // namespace

using containers::simple_vector;

} // namespace

#endif