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

template < class Element, class Allocator = std::allocator<Element> >
class simple_vector
{
private:
	typedef typename Allocator::template rebind<Element>::other allocator_type_;
	allocator_type_ m_allocator;

	Element *m_elements;
	Element *m_elementsEnd;
	Element *m_capacityEnd;

	static const size_t s_maxSize = static_cast<size_t>(-1) / sizeof(Element);
	static const size_t s_minSize = (16 < s_maxSize) ? 16 : s_maxSize;

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
	LEAN_INLINE void copy_construct(Element *dest, const Element *source)
	{
		m_allocator.construct(dest, *source);
	}
	/// Copies elements from the given source range to the given destination.
	void copy_construct(const Element *source, const Element *sourceEnd, Element *dest)
	{
		Element *destr = dest;

		try
		{
			for (; source != sourceEnd; ++dest, ++source)
				copy_construct(dest, source);
		}
		catch(...)
		{
			destruct(destr, dest);
			throw;
		}
	}
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move_construct(Element *dest, Element *source)
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
		m_allocator.destroy(destr);
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

		if (!empty())
			try
			{
//				move_construct(m_elements, m_elementsEnd, newElements);
				memcpy(newElements, m_elements, size() * sizeof(Element));
			}
			catch(...)
			{
				m_allocator.deallocate(newElements, newCapacity);
				throw;
			}

		Element *oldElements = m_elements;
		Element *oldElementsEnd = m_elementsEnd;
		size_t oldCapacity = capacity();
		
		// Mind the order, size() based on member variables!
		m_elementsEnd = newElements + size();
		m_capacityEnd = newElements + newCapacity;
		m_elements = newElements;

		if (oldElements)
		{
			try
			{
//				destruct(oldElements, oldElementsEnd);
			}
			catch(...)
			{
				m_allocator.deallocate(oldElements, oldCapacity);
				throw;
			}

			m_allocator.deallocate(oldElements, oldCapacity);
		}
	}

	/// Frees the given elements.
	void free(Element *elements, Element *elementsEnd, size_t capacity)
	{
		if (elements)
		{
			try
			{
				destruct(elements, elementsEnd);
			}
			catch(...)
			{
				m_allocator.deallocate(elements, capacity);
				throw;
			}

			m_allocator.deallocate(elements, capacity);
		}
	}

	/// Triggers an out of range error.
	LEAN_NOINLINE static void out_of_range()
	{
		throw std::out_of_range("simple_vector<T> out of range");
	}
	/// Checks the given position.
	LEAN_INLINE void check_pos(size_t pos) const
	{
		if (m_elementsEnd <= m_elements + pos)
			out_of_range();
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
		m_elementsEnd(nullptr),
		m_capacityEnd(nullptr) { }
	/// Constructs an empty vector.
	simple_vector(allocator_type allocator)
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
		assign(right.begin(), right.end());
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
		free(m_elements, m_elementsEnd, capacity());
	}

	/// Copies all elements of the given vector to this vector.
	simple_vector& operator =(const simple_vector &right)
	{
		if (&right != this)
			assign(right.begin(), right.end());
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given vector to this vector.
	simple_vector& operator =(simple_vector &&right)
	{
		if (&right != this)
		{
			Element *prevElements = m_elements;
			Element *prevElementsEnd = m_elementsEnd;
			size_t prevCapacity = capacity();

			m_allocator = std::move(right.m_allocator);
			m_elements = std::move(right.m_elements);
			m_elementsEnd = std::move(right.m_elementsEnd);
			m_capacityEnd = std::move(right.m_capacityEnd);

			right.m_elements = nullptr;
			right.m_elementsEnd = nullptr;
			right.m_capacityEnd = nullptr;

			free(prevElements, prevElementsEnd, prevCapacity);
		}
		return *this;
	}
#endif

	/// Assigns the given range of elements to this vector.
	void assign(const_iterator source, const_iterator sourceEnd)
	{
		clear();

		size_t count = sourceEnd - source;

		if (m_elements + count > m_capacityEnd)
			reallocate(capacity_hint(count));

		copy_construct(source, sourceEnd, m_elements);
		m_elementsEnd = m_elements + count;
	}

	/// Appends the given element to this vector.
	LEAN_INLINE void push_back(const value_type &value)
	{
		if (m_elementsEnd >= m_capacityEnd)
			reallocate(capacity_hint(size() + 1));

		m_allocator.construct(m_elementsEnd, value);
		++m_elementsEnd;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Appends the given element to this vector.
	LEAN_INLINE void push_back(value_type &&value)
	{
		if (m_elementsEnd >= m_capacityEnd)
			reallocate(capacity_hint(size() + 1));

		m_allocator.construct(m_elementsEnd, std::forward<value_type>(value));
		++m_elementsEnd;
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
		if (!empty())
		{
			Element *oldElementsEnd = m_elementsEnd;
			m_elementsEnd = m_elements;
			destruct(m_elements, oldElementsEnd);
		}
	}

	/// Reserves space for the predicted number of elements given.
	LEAN_INLINE void reserve(size_t newCapacity)
	{
		if (m_elements + newCapacity > m_capacityEnd)
			reallocate(newCapacity);
	}
	/// Resizes this vector, either appending empty elements to or removing elements from the back of this vector.
	void resize(size_t newCount)
	{
		Elements *newElementsEnd = m_elements + newCount;

		if (newElementsEnd > m_elementsEnd)
		{
			if (newElementsEnd > m_capacityEnd)
			{
				reallocate(capacity_hint(newCount));
				// Update pointers!
				newElementsEnd = m_elements + newCount;
			}
			
			default_construct(m_elementsEnd, newElementsEnd);
			m_elementsEnd = newElementsEnd;
		}
		// Handle newCount == m_count == 0 (m_elements might still contain nullptr)
		else if (newElementsEnd < m_elementsEnd)
		{
			Element *oldElementsEnd = m_elementsEnd;
			m_elementsEnd = newElementsEnd;
			destruct(newElementsEnd, oldElementsEnd);
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
	LEAN_NOINLINE size_t capacity_hint(size_t count) const
	{
		size_t capacityDelta = count / 2;

		// Assume count <= max size, increase by 1.5 or clamp to max_size, mind overflow
		size_t capacity = (s_maxSize - capacityDelta < count)
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
	LEAN_INLINE size_t max_size() const
	{
		return s_maxSize;
	}

	/// Swaps the contents of this vector and the given vector.
	LEAN_INLINE void swap(simple_vector &right) throw()
	{
		std::swap(m_allocator, right.m_allocator);
		std::swap(m_elements, right.m_elements);
		std::swap(m_elementsEnd, right.m_elementsEnd);
		std::swap(m_capacityEnd, right.m_capacityEnd);
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