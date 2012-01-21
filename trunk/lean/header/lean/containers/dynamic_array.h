/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_DYNAMIC_ARRAY
#define LEAN_CONTAINERS_DYNAMIC_ARRAY

#include "../lean.h"
#include "../memory/default_heap.h"

namespace lean 
{
namespace containers
{

/// Dynamic array class.
template < class Element, class Heap = default_heap >
class dynamic_array
{
public:
	/// Type of the heap used by this vector.
	typedef Heap heap_type;
	/// Type of the size returned by this vector.
	typedef typename heap_type::size_type size_type;

	/// Type of the elements contained by this vector.
	typedef Element value_type;
	/// Type of pointers to the elements contained by this vector.
	typedef value_type* pointer;
	/// Type of constant pointers to the elements contained by this vector.
	typedef const value_type* const_pointer;
	/// Type of references to the elements contained by this vector.
	typedef value_type& reference;
	/// Type of constant references to the elements contained by this vector.
	typedef const value_type& const_reference;

	/// Type of iterators to the elements contained by this vector.
	typedef pointer iterator;
	/// Type of constant iterators to the elements contained by this vector.
	typedef const_pointer const_iterator;

private:
	value_type *m_elements;
	value_type *m_elementsEnd;

	// Make sure size_type is unsigned
	LEAN_STATIC_ASSERT(is_unsigned<size_type>::value);

	/// Default constructs an element at the given destination address.
	static LEAN_INLINE void default_construct(Element *dest)
	{
		new (static_cast<void*>(dest)) value_type();
	}

	/// Copies the given source element to the given destination.
	static LEAN_INLINE void copy_construct(Element *dest, const Element &source)
	{
		new (static_cast<void*>(dest)) value_type(source);
	}
	/// Copies elements from the given source range to the given destination.
	template <class Iterator>
	static void copy_construct(Iterator source, Iterator sourceEnd, Element *dest)
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
	static LEAN_INLINE void move_construct(Element *dest, Element &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		new (static_cast<void*>(dest)) value_type( std::move(source) );
#else
		copy_construct(dest, source);
#endif
	}

	/// Moves the given source element to the given destination.
	static LEAN_INLINE void move(Element *dest, Element &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		*dest = std::move(source);
#else
		*dest = source;
#endif
	}
	/// Moves elements from the given source range to the given destination.
	static void move(Element *source, Element *sourceEnd, Element *dest)
	{
		for (; source != sourceEnd; ++dest, ++source)
			move(dest, *source);
	}

	/// Destructs the elements in the given range.
	static void destruct(value_type *destr)
	{
		destr->~value_type();
	}
	/// Destructs the elements in the given range.
	static void destruct(value_type *destr, value_type *destrEnd)
	{
		for (; destr != destrEnd; ++destr)
			destruct(destr);
	}

	/// Allocates the given number of elements.
	static LEAN_INLINE value_type* allocate(size_type capacity)
	{
		return (capacity > 0)
			? static_cast<value_type*>( heap_type::allocate(capacity * sizeof(value_type)) )
			: nullptr;
	}
	
	/// Frees the given elements.
	LEAN_INLINE void free()
	{
		if (m_elements)
		{
			// Do nothing on exception, resources leaking anyways!
			clear();

			value_type *oldElements = m_elements;
			m_elements = nullptr;
			m_elementsEnd = nullptr;

			heap_type::free(oldElements);
		}
	}

public:
	/// Allows for consuming construction from reference.
	enum consume_t
	{
		consume		///< Consume the contents of the given array.
	};

	/// Constructs an empty vector.
	dynamic_array()
		: m_elements(nullptr),
		m_elementsEnd(nullptr) { }
	/// Constructs an empty vector.
	explicit dynamic_array(size_type capacity)
		: m_elements( allocate(capacity) ),
		m_elementsEnd( m_elements ) { }
	/// Copies all elements from the given vector to this vector.
	dynamic_array(const dynamic_array &right)
		: m_elements( allocate(right.size()) ),
		m_elementsEnd( m_elements + right.size() )
	{
		copy_construct(right.m_elements, right.m_elementsEnd, m_elements);
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given vector to this vector.
	dynamic_array(dynamic_array &&right)
		: m_elements(std::move(right.m_elements)),
		m_elementsEnd(std::move(right.m_elementsEnd))
	{
		right.m_elements = nullptr;
		right.m_elementsEnd = nullptr;
	}
#endif
	/// Moves all elements from the given vector to this vector.
	dynamic_array(dynamic_array &right, consume_t)
		: m_elements(right.m_elements),
		m_elementsEnd(right.m_elementsEnd)
	{
		right.m_elements = nullptr;
		right.m_elementsEnd = nullptr;
	}
	/// Destroys all elements in this vector.
	~dynamic_array()
	{
		free();
	}

	/// Copies all elements of the given vector to this vector.
	dynamic_array& operator =(const dynamic_array &right)
	{
		if (&right != this)
		{
			reset(right.size());

			copy_construct(right.m_elements, right.m_elementsEnd, m_elements);
			m_elementsEnd += right.size();
		}
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given vector to this vector.
	dynamic_array& operator =(dynamic_array &&right)
	{
		if (&right != this)
		{
			free();

			m_elements = std::move(right.m_elements);
			m_elementsEnd = std::move(right.m_elementsEnd);

			right.m_elements = nullptr;
			right.m_elementsEnd = nullptr;
		}
		return *this;
	}
#endif

	/// Returns a pointer to the next non-constructed element.
	LEAN_INLINE void* allocate_back()
	{
		return m_elementsEnd;
	}
	/// Marks the next element as constructed.
	LEAN_INLINE reference shift_back(value_type *newElement)
	{
		LEAN_ASSERT(newElement == m_elementsEnd);

		return *(m_elementsEnd++);
	}

	/// Appends a default-constructed element to this vector.
	LEAN_INLINE reference push_back()
	{
		default_construct(m_elementsEnd);
		return *(m_elementsEnd++);
	}
	/// Appends the given element to this vector.
	LEAN_INLINE void push_back(const value_type &value)
	{
		copy_construct(m_elementsEnd, value);
		++m_elementsEnd;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Appends the given element to this vector.
	LEAN_INLINE void push_back(value_type &&value)
	{
		move_construct(m_elementsEnd, value);
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
		Element *oldElementsEnd = m_elementsEnd;
		m_elementsEnd = m_elements;
		destruct(m_elements, oldElementsEnd);
	}

	/// Reserves space for the given number of elements.
	LEAN_INLINE void reset(size_type newCapacity)
	{
		free();

		if (newCapacity > 0)
		{
			m_elements = allocate(newCapacity);
			m_elementsEnd = m_elements;
		}
	}
	
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

	/// Returns true if the vector is empty.
	LEAN_INLINE bool empty(void) const { return (m_elements == m_elementsEnd); };
	/// Returns the number of elements contained by this vector.
	LEAN_INLINE size_type size(void) const { return m_elementsEnd - m_elements; };

	/// Swaps the contents of this vector and the given vector.
	LEAN_INLINE void swap(dynamic_array &right) throw()
	{
		using std::swap;

		swap(m_elements, right.m_elements);
		swap(m_elementsEnd, right.m_elementsEnd);
	}
};

/// Swaps the contents of the given vectors.
template <class Element, class Heap>
LEAN_INLINE void swap(dynamic_array<Element, Heap> &left, dynamic_array<Element, Heap> &right)
{
	left.swap(right);
}

} // namespace

using containers::dynamic_array;

} // namespace

#endif