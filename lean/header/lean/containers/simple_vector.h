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

	/// Moves elements from the given source range to the given destination.
	void move_construct(Element *source, Element *sourceEnd, Element *dest)
	{
		Element *destr = dest;

		try
		{
			for (; source != sourceEnd; ++dest, ++source)
#ifndef LEAN0X_NO_RVALUE_REFERENCES
				m_allocator.construct(dest, std::move(*source));
#else
				m_allocator.construct(dest, *source);
#endif
		}
		catch(...)
		{
			destruct(destr, dest);
			throw;
		}
	}

	/// Destructs the elements in the given range.
	void destruct(Element *destr, Element *destrEnd)
	{
		for (; destr != destrEnd; ++destr)
			m_allocator.destroy(*destr);
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
				m_allocator.deallocate(newElements, capacity);
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
		// TODO: implement
	}

	/// Clears all elements from this vector.
	LEAN_INLINE void clear()
	{
		if (m_elements)
		{
			Element *end = m_elements + m_count;
			m_count = 0;
			destruct(m_elements, end);
		}
	}

	/// Reserves space for the predicted number of elements given.
	LEAN_INLINE void reserve(size_t newCapacity)
	{
		if (newCapacity > m_capacity)
			reallocate(newCapacity);
	}

	// TODO: resize

	// TODO: element access

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
};

} // namespace

using containers::simple_vector;

} // namespace

#endif