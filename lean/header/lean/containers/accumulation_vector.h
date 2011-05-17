/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_ACCUMULATION_VECTOR
#define LEAN_CONTAINERS_ACCUMULATION_VECTOR

#include "../lean.h"
#include <vector>
#include <algorithm>
#include "default_reallocation_policy.h"

namespace lean
{
namespace containers
{
	
/// Vector wrapper class providing improved performance on repeated element accumulation.
/** This class stores elements of the given type without destructing them on remove
  * or clear, thus eliminating the overhead incurred by destruction and re-construction
  * of frequently removed elements.
  * @see lean::containers::accumulation_map */
template < class Container, class ReallocationPolicy = default_reallocation_policy<Container> >
class accumulation_vector
{
private:
	// Container
	typedef Container container_type_;
	container_type_ m_container;

	typedef typename container_type_::size_type size_type_;
	size_type_ m_size;
	
	/// Reserves space for the given number of elements.
	LEAN_INLINE void reserve_internal(size_type_ newCount)
	{
		ReallocationPolicy::reserve(m_container, newCount);
	}
	/// Grows vector storage to fit the given new count.
	LEAN_INLINE void growTo(size_type_ newCount)
	{
		ReallocationPolicy::pre_resize(m_container, newCount);
	}
	/// Grows vector storage to fit the given additional number of elements.
	LEAN_INLINE void grow(size_type_ count)
	{
		growTo(m_size + count);
	}

	/// Asserts that the given value lies outside this vector's element range.
	LEAN_INLINE assert_outside(const value& value)
	{
		LEAN_ASSERT(lean::addressof(value) < lean::addressof(*m_container.begin()) || lean::addressof(*m_container.end()) <= lean::addressof(value));
	}

	/// Triggers an out of range error.
	LEAN_NOINLINE static void out_of_range()
	{
		throw std::out_of_range("accumulation_vector<T> out of range");
	}
	/// Checks the given position.
	LEAN_INLINE void check_pos(size_type_ pos) const
	{
		if (pos >= m_size)
			out_of_range();
	}

public:
	/// Type of the container wrapped by this vector.
	typedef container_type_ container_type;
	/// Type of the policy used by this vector.
	typedef ReallocationPolicy reallocation_policy; 

	/// Type of the allocator used by this vector.
	typedef typename container_type::allocator_type allocator_type;
	/// Type of the size returned by this vector.
	typedef typename container_type::size_type size_type;
	/// Type of the difference between the addresses of two elements in this vector.
	typedef typename container_type::difference_type difference_type;

	/// Type of pointers to the elements contained by this vector.
	typedef typename container_type::pointer pointer;
	/// Type of constant pointers to the elements contained by this vector.
	typedef typename container_type::const_pointer const_pointer;
	/// Type of references to the elements contained by this vector.
	typedef typename container_type::reference reference;
	/// Type of constant references to the elements contained by this vector.
	typedef typename container_type::const_reference const_reference;
	/// Type of the elements contained by this vector.
	typedef typename container_type::value_type value_type;

	/// Type of iterators to the elements contained by this vector.
	typedef typename container_type::iterator iterator;
	/// Type of constant iterators to the elements contained by this vector.
	typedef typename container_type::const_iterator const_iterator;

	/// Type of reverse iterators to the elements contained by this vector.
	typedef typename container_type::reverse_iterator reverse_iterator;
	/// Type of constant reverse iterators to the elements contained by this vector.
	typedef typename container_type::const_reverse_iterator const_reverse_iterator;

	/// Constructs an empty accumulation vector.
	accumulation_vector()
		: m_size(0) { }
	/// Constructs an empty accumulation vector using the given allocator.
	explicit accumulation_vector(const allocator_type &allocator)
		: m_container(allocator),
		m_size(0) { }
	/// Constructs an accumulation vector containing the given number of elements.
	explicit accumulation_vector(size_type count)
		: m_container(count),
		m_size(count) { }
	/// Constructs an accumulation vector containing the given number of given elements.
	accumulation_vector(size_type count, const value_type& value)
		: m_container(count, value),
		m_size(count) { }
	/// Constructs an accumulation vector containing the given number of given elements using the given allocator.
	accumulation_vector(size_type count, const value_type& value, const allocator_type& allocator)
		: m_container(count, value, allocator),
		m_size(count) { }
	/// Copies elements from the given vector to this vector.
	accumulation_vector(const accumulation_vector& right)
		: m_container(right.m_container),
		m_size(right.m_size) { }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves elements from the given vector to this vector.
	accumulation_vector(accumulation_vector&& right)
		: m_container(std::move(right.m_container)),
		m_size(std::move(right.m_size)) { }
#endif
	/// Constructs an accumulation vector from the given range of elements.
	template<class Iterator>
	accumulation_vector(Iterator itFirst, Iterator itEnd)
		: m_container(itFirst, itEnd),
		m_size(m_container.size()) { }
	/// Constructs an accumulation vector from the given range of elements using the given allocator.
	template<class Iterator>
	accumulation_vector(Iterator itFirst, Iterator itEnd, const allocator_type& allocator)
		: m_container(itFirst, itEnd, allocator),
		m_size(m_container.size()) { }

	/// Copies all elements from the given vector to this vector.
	accumulation_vector& operator =(const accumulation_vector& right)
	{
		if (this != &right)
		{
			if (right.empty())
				// Clearing is extremely fast for this class
				clear();
			else
				// Suppress destruction, assign manually
				assign(right.begin(), right.end());
		}

		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Move Assignment operator.
	inline accumulation_vector& operator =(accumulation_vector&& right)
	{
		if (this != &right)
		{
			if (right.empty())
				// Keep own allocated storage
				clear();
			else
			{
				// Invoke default move operators
				m_container = std::move(right.m_container);
				m_size = std::move(right.m_size);
			}
		}

		return *this;
	}
#endif

	/// Gets the number of elements contained by this vector.
	LEAN_INLINE size_type size(void) const { return m_size; };
	/// Checks if this vector is empty.
	LEAN_INLINE bool empty(void) const { return (m_size == 0); };

	/// Appends an element at the back of this vector.
	LEAN_INLINE value_type& push_back(void)
	{
		if (m_size == m_container.size())
		{
			grow(1);
			m_container.push_back(Element());
		}

		return m_container[m_size++];
	}
	/// Appends an element at the back of this vector. Assumes value outside of vector range, copy manually otherwise.
	LEAN_INLINE void push_back(const value_type& value)
	{
		assert_outside(value);

		if (m_size == m_container.size())
		{
			grow(1);
			m_container.push_back(value);
		}
		else
			m_container[m_size] = value;

		++m_size;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Appends an element at the back of this vector. Assumes value outside of vector range, copy manually otherwise.
	LEAN_INLINE void push_back(value_type&& value)
	{
		assert_outside(value);

		if (m_size == m_container.size())
		{
			grow(1);
			m_container.push_back(std::move(value));
		}
		else
			m_container[m_size] = std::move(value);

		++m_size;
	}
#endif

	/// Removes one element at the back of this vector.
	LEAN_INLINE void pop_back(void)
	{
		LEAN_ASSERT(!empty());

		--m_size;
	}

	/// Inserts an element into this vector.
	iterator insert(iterator itWhere)
	{
		if (m_size == m_container.size())
		{
			size_type index = itWhere - m_container.begin();
			grow(1);
			itWhere = m_container.insert(m_container.begin() + index, Element());
		}
		else
			std::copy_backward(itWhere, end(), ++iterator(itWhere));

		++m_size;
		return itWhere;
	}
	/// Inserts an element into this vector. Assumes value outside of vector range, copy manually otherwise.
	iterator insert(iterator itWhere, const value_type& value)
	{
		assert_outside(value);

		if (m_size == m_container.size())
		{
			size_type index = itWhere - m_container.begin();
			grow(1);
			itWhere = m_container.insert(m_container.begin() + index, value);
			++m_size;
		}
		else
		{
			std::copy_backward(itWhere, end(), ++iterator(itWhere));
			++m_size;
			*itWhere = value;
		}
		
		return itWhere;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Inserts an element into this vector. Assumes value outside of vector range, copy manually otherwise.
	iterator insert(iterator itWhere, value_type&& value)
	{
		assert_outside(value);

		if (m_size == m_container.size())
		{
			size_type index = itWhere - m_container.begin();
			grow(1);
			itWhere = m_container.insert(m_container.begin() + index, std::move(value));
			++m_size;
		}
		else
		{
			std::copy_backward(itWhere, end(), ++iterator(itWhere));
			++m_size;
			*itWhere = std::move(value);
		}
		
		return itWhere;
	}
#endif

	/// Inserts the specified number of elements into this vector.
	iterator insert(iterator itWhere, size_type count)
	{
		size_type newSize = m_size + count;
		
		if(newSize > m_container.size())
		{
			size_type index = itWhere - m_container.begin();
			growTo(newSize);
			m_container.resize(newSize);
			itWhere = m_container.begin() + index;
		}
		
		std::copy_backward(itWhere, end(), itWhere + count);
		m_size = newSize;

		return itWhere;
	}
	/// Inserts the specified number of elements into this vector. Assumes value outside of vector range, copy manually otherwise.
	LEAN_INLINE void insert(iterator itWhere, size_type count, const value_type& value)
	{
		assert_outside(value);

		std::fill_n(
			insert(itWhere, count),
			count, value);
	}
	/// Inserts a range of elements into this vector. Assumes that the given positioning iterator does not split the given range.
	template<class Iterator>
	void insert(iterator itWhere, Iterator itFirst, Iterator itEnd)
	{
		LEAN_ASSERT(itFirst <= itEnd);

		size_type count = itEnd - itFirst;
		size_type index = lean::addressof(*itFirst) - lean::addressof(*m_container.begin());

		// Index is unsigned, make use of wrap-around
		if (index < m_size)
		{
			size_type endIndex = lean::addressof(*itEnd) - lean::addressof(*m_container.begin());
			
			LEAN_ASSERT(itEnd <= itWhere || itWhere <= itFirst);

			if (itWhere <= itFirst)
			{
				index += count;
				endIndex += count;
			}
			itWhere = insert(itWhere, count);

			itFirst = m_container.begin() + index;
			itEnd = m_container.begin() + endIndex;
		}
		else
			itWhere = insert(itWhere, count);
		
		std::copy(itFirst, itEnd, itWhere);
	}

	/// Removes one element by iterator.
	LEAN_INLINE iterator erase(iterator itWhere)
	{
		LEAN_ASSERT(!empty());
		
		std::copy(++iterator(itWhere), end(), itWhere);
		--m_size;

		return itWhere;
	}
	/// Removes the given range of elements by iterator.
	LEAN_INLINE void erase(iterator itFirst, iterator itEnd)
	{
		std::copy(itEnd, end(), itFirst);
		m_size -= itEnd - itFirst;
	}

	/// Assigns the given number of elements to this vector. Assumes value outside of vector range, copy manually otherwise.
	void assign(size_type count, const Element& value)
	{
		assert_outside(value);

		if(count > m_container.size())
		{
			growTo(count);
			m_container.resize(count);
		}

		std::fill_n(m_container.begin(), count, value);
		m_size = count;
	}
	/// Assigns the given range of elements to this vector.
	template<class Iterator>
	void assign(Iterator itFirst, Iterator itEnd)
	{
		LEAN_ASSERT(itFirst <= itEnd);

		size_t count = itEnd - itFirst;
		
		if(count > m_container.size())
		{
			size_type index = lean::addressof(*itFirst) - lean::addressof(*m_container.begin());
			size_type endIndex = lean::addressof(*itEnd) - lean::addressof(*m_container.begin());

			growTo(count);
			m_container.resize(count);

			// Index is unsigned, make use of wrap-around
			if (index < m_size)
			{
				itFirst = m_container.begin() + index;
				itEnd = m_container.begin() + endIndex;
			}
		}

		std::copy(itFirst, itEnd, m_container.begin());
		m_size = count;
	}

	/// Removes all elements from this vector.
	LEAN_INLINE void clear(void) { m_size = 0; }

	/// Gets a copy of the allocator used by this vector.
	LEAN_INLINE Allocator get_allocator() const { return m_container.get_allocator(); }
	/// Returns the maximum number of elements this vector could store.
	LEAN_INLINE size_type max_size(void) const { return m_container.max_size(); }

	/// Returns the number of elements this vector could contain without reallocation.
	LEAN_INLINE size_type capacity(void) const { return m_container.capacity(); }
	/// Reserves storage for the specified number of elements.
	LEAN_INLINE void reserve(size_type count)
	{
		reserve_internal(count);
	}
	/// Inserts or erases elements to match the new size specified.
	void resize(size_type count)
	{
		if(count > m_container.size())
		{
			growTo(count);
			m_container.resize(count);
		}

		m_size = count;
	}
	/// Inserts or erases elements to match the new size specified
	void resize(size_type count, const Element& value)
	{
		if(count > m_container.size())
		{
			growTo(count);
			m_container.resize(count);
		}

		if(count > m_size)
			std::fill_n(end(), count - m_size, value);

		m_size = count;
	}

	/// Gets an element by position.
	LEAN_INLINE reference at(size_type pos) { check_pos(pos); return m_container.at(pos); }
	/// Gets an element by position.
	LEAN_INLINE const_reference at(size_type pos) const { check_pos(pos); return m_container.at(pos); }
	/// Gets the first element in the vector.
	LEAN_INLINE reference front(void) { LEAN_ASSERT(!empty()); return m_container.front(); }
	/// Gets the first element in the vector.
	LEAN_INLINE const_reference front(void) const { LEAN_ASSERT(!empty()); return m_container.front(); }
	/// Gets the last element in the vector.
	LEAN_INLINE reference back(void) { LEAN_ASSERT(!empty()); return m_container[m_size - 1]; }
	/// Gets the last element in the vector.
	LEAN_INLINE const_reference back(void) const { LEAN_ASSERT(!empty()); return m_container[m_size - 1]; }

	/// Gets an element by position.
	LEAN_INLINE reference operator [] (size_type pos) { return m_container[pos]; }
	/// Gets an element by position.
	LEAN_INLINE const_reference operator [] (size_type pos) const { return m_container[pos]; }

	/// Returns an iterator to the first element contained by this vector.
	LEAN_INLINE iterator begin(void) { return m_container.begin(); }
	/// Returns a constant iterator to the first element contained by this vector.
	LEAN_INLINE const_iterator begin(void) const { return m_container.begin(); }
	/// Returns an iterator beyond the last element contained by this vector.
	LEAN_INLINE iterator end(void) { return m_container.begin() + m_size; }
	/// Returns a constant iterator beyond the last element contained by this vector.
	LEAN_INLINE const_iterator end(void) const { return m_container.begin() + m_size; }

	/// Returns a reverse iterator to the last element contained by this vector.
	LEAN_INLINE reverse_iterator rbegin(void) { return m_container.rend() - m_size; }
	/// Returns a constant reverse iterator to the last element contained by this vector.
	LEAN_INLINE const_reverse_iterator rbegin(void) const { return m_container.rend() - m_size; }
	/// Returns a reverse iterator beyond the first element contained by this vector.
	LEAN_INLINE reverse_iterator rend(void) { return m_container.rend(); }
	/// Returns a constant reverse iterator beyond the first element contained by this vector.
	LEAN_INLINE const_reverse_iterator rend(void) const { return m_container.rend(); }

	// Swaps the elements of two accumulation vectors.
	LEAN_INLINE void swap(accumulation_vector& right)
	{
		swap(m_container, right.m_container);
		std::swap(m_size, right.m_size);
	}
};

/// Comparison operator.
template<class Container, class Policy>
LEAN_INLINE bool operator ==(const accumulation_vector<Container, Policy>& left, 
	const accumulation_vector<Container, Policy>& right)
{
	return (left.size() == right.size()) && std::equal(left.begin(), left.end(), right.begin());
}

/// Comparison operator.
template<class Container, class Policy>
LEAN_INLINE bool operator !=(const accumulation_vector<Container, Policy>& left, 
	const accumulation_vector<Container, Policy>& right)
{
	return !(left == right);
}

/// Comparison operator.
template<class Container, class Policy>
LEAN_INLINE bool operator <(const accumulation_vector<Container, Policy>& left, 
	const accumulation_vector<Container, Policy>& right)
{
	return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
}

/// Comparison operator.
template<class Container, class Policy>
LEAN_INLINE bool operator >(const accumulation_vector<Container, Policy>& left, 
	const accumulation_vector<Container, Policy>& right)
{
	return (right < left);
}

/// Comparison operator.
template<class Container, class Policy>
LEAN_INLINE bool operator <=(const accumulation_vector<Container, Policy>& left, 
	const accumulation_vector<Container, Policy>& right)
{
	return !(right < left);
}

/// Comparison operator.
template<class Container, class Policy>
LEAN_INLINE bool operator >=(const accumulation_vector<Container, Policy>& left, 
	const accumulation_vector<Container, Policy>& right)
{
	return !(left < right);
}

// Swaps the elements of two accumulation vectors.
template<class Container, class Policy>
LEAN_INLINE void swap(accumulation_vector<Container, Policy>& left, 
	accumulation_vector<Container, Policy>& right)
{
	left.swap(right);
}

} // namespace

using containers::accumulation_vector;

} // namespace

#endif