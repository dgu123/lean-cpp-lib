/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_STATIC_ARRAY
#define LEAN_CONTAINERS_STATIC_ARRAY

#include "../lean.h"
#include "../functional/variadic.h"
#include "../meta/type_traits.h"

namespace lean 
{
namespace containers
{

/// Static array class.
template <class Element, size_t Capacity>
class static_array
{
public:
	/// Type of the size returned by this vector.
	typedef size_t size_type;
	/// Size.
	static const size_type capacity = Capacity;

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
	// Make sure size_type is unsigned
	LEAN_STATIC_ASSERT(is_unsigned<size_type>::value);
		
	char m_memory[sizeof(value_type) * capacity];
	value_type *m_elementsEnd;
	
	/// First element in the array.
	value_type *const elements() { return reinterpret_cast<value_type*>(&m_memory[0]); }
	/// First element in the array.
	const value_type *const elements() const { return reinterpret_cast<const value_type*>(&m_memory[0]); }
	/// One past the last element in the array.
	value_type*& elementsEnd() { return m_elementsEnd; }
	/// One past the last element in the array.
	const value_type *const& elementsEnd() const { return m_elementsEnd; }
	
	/// Default constructs an element at the given destination address.
	static LEAN_INLINE void default_construct(value_type *dest)
	{
		new (static_cast<void*>(dest)) value_type();
	}
	/// Default constructs elements in the given range.
	static void default_construct(value_type *dest, value_type *destEnd)
	{
		value_type *destr = dest;

		try
		{
			for (; dest != destEnd; ++dest)
				default_construct(dest);
		}
		catch (...)
		{
			destruct(destr, dest);
			throw;
		}
	}

	/// Copies the given source element to the given destination.
	static LEAN_INLINE void copy_construct(value_type *dest, const value_type &source)
	{
		new (static_cast<void*>(dest)) value_type(source);
	}
	/// Copies elements from the given source range to the given destination.
	template <class Iterator>
	static size_type copy_construct(Iterator source, Iterator sourceEnd, value_type *dest)
	{
		size_type count = 0;
		value_type *destr = dest;

		try
		{
			for (; source != sourceEnd; ++dest, ++source, ++count)
				copy_construct(dest, *source);
		}
		catch(...)
		{
			destruct(destr, dest);
			throw;
		}

		return count;
	}

	/// Moves the given source element to the given destination.
	static LEAN_INLINE void move_construct(value_type *dest, value_type &source)
	{
		new (static_cast<void*>(dest)) value_type( LEAN_MOVE(source) );
	}
	/// Moves elements from the given source range to the given destination.
	template <class Iterator>
	static size_type move_construct(Iterator source, Iterator sourceEnd, value_type *dest)
	{
		size_type count = 0;

		for (; source != sourceEnd; ++dest, ++source, ++count)
			move_construct(dest, *source);

		return count;
	}

	/// Moves the given source element to the given destination.
	static LEAN_INLINE void move(value_type *dest, value_type &source)
	{
		*dest = LEAN_MOVE(source);
	}
	/// Moves elements from the given source range to the given destination.
	template <class Iterator>
	static void move(Iterator source, Iterator sourceEnd, value_type *dest)
	{
		for (; source != sourceEnd; ++dest, ++source)
			move(dest, *source);
	}

	/// Moves elements from the given source range to the given destination.
	template <class Iterator>
	static void swap(value_type *left, value_type *leftEnd, value_type *right)
	{
		for (; left != leftEnd; ++left, ++right)
			swap(*left, *right);
	}

	/// Destructs the elements in the given range.
	static void destruct(value_type *destr)
	{
		destr->~value_type();
	}
	/// Destructs the elements in the given range.
	static void destruct(value_type *destr, value_type *destrEnd)
	{
		while (destr < destrEnd)
			destruct(--destrEnd);
	}

public:
	/// Constructs an empty vector.
	static_array()
		: m_elementsEnd(elements()) { }
	/// Copies all elements from the given vector to this vector.
	static_array(const static_array &right)
		: m_elementsEnd(elements())
	{
		copy_construct(right.elements(), right.elementsEnd(), elements());
		elementsEnd() += right.size();
	}
	/// Copies all elements from the given vector to this vector.
	template <size_t RightCapacity>
	explicit static_array(const static_array<value_type, RightCapacity> &right)
		: m_elementsEnd(elements())
	{
		copy_construct(right.elements(), right.elementsEnd(), elements());
		elementsEnd() += right.size();
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given vector to this vector.
	static_array(static_array &&right)
		: m_elementsEnd(elements())
	{
		move_construct(right.elements(), right.elementsEnd(), elements());
		elementsEnd() += right.size();
	}
	/// Moves all elements from the given vector to this vector.
	template <size_t RightCapacity>
	explicit static_array(static_array<value_type, RightCapacity> &&right)
		: m_elementsEnd(elements())
	{
		move_construct(right.elements(), right.elementsEnd(), elements());
		elementsEnd() += right.size();
	}
#endif
	/// Moves all elements from the given vector to this vector.
	template <size_t RightCapacity>
	static_array(static_array<value_type, RightCapacity> &right, consume_t)
		: m_elementsEnd(elements())
	{
		move_construct(right.elements(), right.elementsEnd(), elements());
		elementsEnd() += right.size();
	}
	/// Copies all elements from the given range to this vector.
	template <class Iterator>
	static_array(Iterator begin, Iterator end)
		: m_elementsEnd(elements())
	{
		elementsEnd() += copy_construct(begin, end, elements());
	}
	/// Moves all elements from the given range to this vector.
	template <class Iterator>
	static_array(Iterator begin, Iterator end, consume_t)
		: m_elementsEnd(elements())
	{
		elementsEnd() += move_construct(begin, end, elements());
	}
	/// Destroys all elements in this vector.
	~static_array()
	{
		destruct(elements(), elementsEnd());
	}

	/// Copies all elements of the given vector to this vector.
	template <size_t RightCapacity>
	void assign(const static_array<value_type, RightCapacity> &right)
	{
		if (&right != this)
		{
			clear();
			copy_construct(right.elements(), right.elementsEnd(), elements());
			elementsEnd() += right.size();
		}
	}
	/// Moves all elements from the given vector to this vector.
	template <size_t RightCapacity>
	void assign(static_array<value_type, RightCapacity> &right, consume_t)
	{
		if (&right != this)
		{
			clear();
			move_construct(right.elements(), right.elementsEnd(), elements());
			elementsEnd() += right.size();
		}
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given vector to this vector.
	template <size_t RightCapacity>
	LEAN_INLINE void assign(static_array<value_type, RightCapacity> &&right)
	{
		assign(right, consume);
	}
#endif

	/// Copies all elements of the given vector to this vector.
	LEAN_INLINE static_array& operator =(const static_array &right)
	{
		assign(right);
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given vector to this vector.
	LEAN_INLINE static_array& operator =(static_array &&right)
	{
		assign(right, consume);
		return *this;
	}
#endif
	
	/// Copies all elements from the given range to this vector.
	template <class Iterator>
	void assign_disjoint(Iterator begin, Iterator end)
	{
		clear();
		elementsEnd() += copy_construct(begin, end, elements());
	}
	/// Moves all elements from the given range to this vector.
	template <class Iterator>
	void assign_disjoint(Iterator begin, Iterator end, consume_t)
	{
		clear();
		elementsEnd() += move_construct(begin, end, elements());
	}

	/// Returns a pointer to the next non-constructed element.
	LEAN_INLINE void* allocate_back()
	{
		return elementsEnd();
	}
	/// Marks the next element as constructed.
	LEAN_INLINE reference shift_back(value_type *newElement)
	{
		LEAN_ASSERT(newElement == elementsEnd());

		return *elementsEnd()++;
	}

#ifdef DOXYGEN_READ_THIS
	/// Constructs an element at the back, passing the given arguments.
	reference emplace_back(...);
#else
	#define LEAN_DYNAMIC_ARRAY_EMPLACE_METHOD_DECL \
		reference emplace_back
	#define LEAN_DYNAMIC_ARRAY_EMPLACE_METHOD_BODY(call) \
		{ \
			return shift_back( new (allocate_back()) value_type##call ); \
		}
	LEAN_VARIADIC_PERFECT_FORWARDING(LEAN_DYNAMIC_ARRAY_EMPLACE_METHOD_DECL, LEAN_NOTHING, LEAN_DYNAMIC_ARRAY_EMPLACE_METHOD_BODY)
#endif

	/// Appends a default-constructed element to this vector.
	LEAN_INLINE reference push_back()
	{
		default_construct(elementsEnd());
		return *elementsEnd()++;
	}
	/// Appends a default-constructed element to this vector.
	LEAN_INLINE pointer push_back_n(size_type count)
	{
		Element *firstElement = elementsEnd();
		default_construct(firstElement, firstElement + count);
		elementsEnd() += count;
		return firstElement;
	}
	/// Appends the given element to this vector.
	LEAN_INLINE reference push_back(const value_type &value)
	{
		copy_construct(elementsEnd(), value);
		return *elementsEnd()++;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Appends the given element to this vector.
	LEAN_INLINE reference push_back(value_type &&value)
	{
		move_construct(elementsEnd(), value);
		return *elementsEnd()++;
	}
#endif
	/// Removes the last element from this vector.
	LEAN_INLINE void pop_back()
	{
		LEAN_ASSERT(!empty());

		destruct(--elementsEnd());
	}

	/// Clears all elements from this vector.
	LEAN_INLINE void clear()
	{
		Element *oldElementsEnd = elementsEnd();
		elementsEnd() = elements();
		destruct(elements(), oldElementsEnd);
	}
	
	/// Gets the first element in the vector, access violation on failure.
	LEAN_INLINE reference front(void) { LEAN_ASSERT(!empty()); return *elements(); };
	/// Gets the first element in the vector, access violation on failure.
	LEAN_INLINE const_reference front(void) const { LEAN_ASSERT(!empty()); return *elements(); };
	/// Gets the last element in the vector, access violation on failure.
	LEAN_INLINE reference back(void) { LEAN_ASSERT(!empty()); return elementsEnd()[-1]; };
	/// Gets the last element in the vector, access violation on failure.
	LEAN_INLINE const_reference back(void) const { LEAN_ASSERT(!empty()); return elementsEnd()[-1]; };

	/// Gets an element by position, access violation on failure.
	LEAN_INLINE reference operator [](size_type pos) { return elements()[pos]; };
	/// Gets an element by position, access violation on failure.
	LEAN_INLINE const_reference operator [](size_type pos) const { return elements()[pos]; };

	/// Returns an iterator to the first element contained by this vector.
	LEAN_INLINE iterator begin(void) { return elements(); };
	/// Returns a constant iterator to the first element contained by this vector.
	LEAN_INLINE const_iterator begin(void) const { return elements(); };
	/// Returns an iterator beyond the last element contained by this vector.
	LEAN_INLINE iterator end(void) { return elementsEnd(); };
	/// Returns a constant iterator beyond the last element contained by this vector.
	LEAN_INLINE const_iterator end(void) const { return elementsEnd(); };

	/// Returns true if the vector is empty.
	LEAN_INLINE bool empty(void) const { return (elements() == elementsEnd()); };
	/// Returns the number of elements contained by this vector.
	LEAN_INLINE size_type size(void) const { return elementsEnd() - elements(); };

	/// Swaps the contents of this vector and the given vector.
	template <size_t RightCapacity>
	LEAN_INLINE void swap(static_array<value_type, RightCapacity> &right)
	{
		value_type *min, *minEnd, *max, *maxEnd;

		if (size() < right.size())
		{
			min = elements();
			minEnd = elementsEnd();
			max = right.elements();
			maxEnd = right.elementsEnd();
		}
		else
		{
			min = right.elements();
			minEnd = right.elementsEnd();
			max = elements();
			maxEnd = elementsEnd();
		}

		swap(min, minEnd, max);
		move(max + (minEnd - min), maxEnd, minEnd);
		destruct(max + (minEnd - min), maxEnd);

		size_type leftSize = right.size();
		size_type rightSize = size();

		elementsEnd() = elements() + leftSize;
		right.elementsEnd() = right.elements() + rightSize;
	}
};

/// Swaps the contents of the given arrays.
template <class Element, size_t LeftCapacity, size_t RightCapacity>
LEAN_INLINE void swap(static_array<Element, LeftCapacity> &left, static_array<Element, RightCapacity> &right)
{
	left.swap(right);
}

} // namespace

using containers::static_array;

} // namespace

#endif