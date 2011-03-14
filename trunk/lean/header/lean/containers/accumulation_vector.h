/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_ACCUMULATION_VECTOR
#define LEAN_CONTAINERS_ACCUMULATION_VECTOR

#include <vector>
#include "../cpp0x.h"
#include "default_reallocation_policy.h"

namespace lean
{
namespace containers
{
	
/// Vector wrapper class providing improved performance on repeated element accumulation.
/** This class stores elements of the given type without destructing them on remove
  * or clear, thus eliminating the overhead incurred by destruction and re-construction
  * of frequently removed elements.
  * @see lean::containers::unordered_vector */
template < class Element,
	template <class Container>  class ReallocationPolicy = default_reallocation_policy<Container>,
	class Allocator = ::std::allocator<Element> >
class accumulation_vector
{
private:
	// Container
	typedef ::std::vector<Element, Allocator> container_type;
	container_type m_container;

	typename container_type::size_type m_size;

	/// Reserves memory for the given number of elements.
	inline void reserve_internal(typename container_type::size_type newCount)
	{
		ReallocationPolicy<container_Type>::reserve(m_container, newCount);
	}
	/// Reserves memory for the given number of elements.
	inline void reallocate_internal(typename container_type::size_type newCount)
	{
		ReallocationPolicy<container_Type>::pro_reserve(m_container, newCount);
	}
	/// Grows the reserved memory of this vector by the given number of elements.
	inline void grow_internal(typename container_type::size_type count)
	{
		reallocate_internal(m_size + count);
	}

public:
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

	/// Constructor.
	inline accumulation_vector() : m_size(0) { }; 
	/// Constructor.
	inline explicit accumulation_vector(const Allocator& allocator) : m_container(allocator), m_size(0) { };
	/// Constructor.
	inline explicit accumulation_vector(size_type count) : m_container(count), m_size(count) { };
	/// Constructor.
	inline accumulation_vector(size_type count, const Element& value) : m_container(count, value), m_size(count) { };
	/// Copy Constructor.
	inline accumulation_vector(size_type count, const Element& value, const Allocator& allocator) : m_container(count, value, allocator), m_size(count) { };
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Move Constructor.
	inline accumulation_vector(accumulation_vector&& right) : m_container(::std::move(right.m_container)), m_size(::std::move(right.m_size)) { };
#endif
	/// Constructor.
	inline accumulation_vector(const accumulation_vector& right) : m_container(right.m_container), m_size(right.m_size) { };
	/// Constructor.
	template<class Iterator>
	inline accumulation_vector(Iterator itFirst, Iterator itLast) : m_container(itFirst, itLast), m_size(m_container.size()) { };
	/// Constructor.
	template<class Iterator>
	inline accumulation_vector(Iterator itFirst, Iterator itLast, const Allocator& allocator) : m_container(itFirst, itLast, allocator), m_size(m_container.size()) { };
	/// Destructor.
	inline ~accumulation_vector() { };

	/// Assignment operator.
	inline accumulation_vector& operator =(const accumulation_vector& right)
	{
		if (this != &right)
		{
			// Short cut
			if (right.empty())
				clear();
			// Suppress destruction
			else
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
			// Short cut
			if (right.empty())
				clear();
			else
			{
				m_container = ::std::move(right.m_container);
				m_size = ::std::move(right.m_size);
			}
		}

		return *this;
	}
#endif

	/// Gets the number of elements contained by this vector.
	inline size_type size(void) const { return m_size; };
	/// Checks if this vector is empty.
	inline bool empty(void) const { return (m_size == 0); };

	/// Appends an element at the back of this vector.
	inline Element& push_back(void)
	{
		if (m_size == m_container.size())
		{
			grow_internal(1);
			m_container.push_back(Element());
		}

		return m_container[m_size++];
	}
	/// Appends an element at the back of this vector.
	inline void push_back(const Element& value)
	{
		// Reset existing element, if still available
		if (m_size < m_container.size())
			m_container[m_size] = value;
		// Add element otherwise
		else
		{
			grow_internal(1);
			m_container.push_back(value);
		}

		++m_size;
	}

	/// Inserts an element into this vector
	inline iterator insert(iterator itWhere)
	{
		// Reset existing element, if still available
		if(m_size < m_container.size())
			// Make way for this element by shifting the following elements
			::std::copy_backward(itWhere, end(), ++iterator(itWhere));
		else
		{
			// Grow reserved memory
			difference_type _Index = itWhere - m_container.begin();
			grow_internal(1);
			itWhere = m_container.begin() + _Index;

			// Add element
			itWhere = m_container.insert(itWhere, Element());
		}

		++m_size;

		// Return iterator
		return itWhere;
	}
	/// Inserts an element into this vector
	inline iterator insert(iterator itWhere, const Element& value)
	{
		if(m_size < m_container.size())
		{
			// Make way for this element by shifting the following elements
			::std::copy_backward(itWhere, end(), ++iterator(itWhere));
			m_size++;

			// Set element
			*itWhere = value;
		}
		else
		{
			// Grow reserved memory
			difference_type _Index = itWhere - m_container.begin();
			grow_internal(1);
			itWhere = m_container.begin() + _Index;

			// Add element
			itWhere = m_container.insert(itWhere, value);
			m_size++;
		}

		// Return iterator
		return itWhere;
	}
	/// Inserts the specified number of elements into this vector
	inline iterator insert(iterator itWhere, size_type count)
	{
		size_type _Newsize = m_size + count;

		// Allocate new elements
		if(_Newsize > m_container.size())
		{
			difference_type _Index = itWhere - m_container.begin();
			reallocate_internal(_Newsize);
			m_container.resize(_Newsize);
			itWhere = m_container.begin() + _Index;
		}

		// Make way for these elements by shifting the following elements
		::std::copy_backward(itWhere, end(), itWhere + count);
		m_size = _Newsize;

		return itWhere;
	}
	/// Inserts the specified number of elements into this vector
	inline void insert(iterator itWhere, size_type count, const Element& value)
	{
		// Insert elements
		itWhere = insert(itWhere, count);

		// Set elements
		::std::fill_n(itWhere, count, value);
	}
	/// Inserts a range of elements into this vector
	template<class Iterator>
	inline void insert(iterator itWhere, Iterator itFirst, Iterator itLast)
	{
		// Insert elements
		itWhere = insert(itWhere, itLast - itFirst);

		// Set elements
		::std::copy(itFirst, itLast, itWhere);
	}
	/// Removes an element at the back of this vector
	inline void pop_back(void)
	{
		// Remove element
		if(!empty())
			m_size--;
	}
	/// Removes an element by iterator
	inline iterator erase(iterator itWhere)
	{
		// Remove element
		if(!empty())
		{
			// Replace this element by the following elements
			::std::copy(++iterator(itWhere), end(), itWhere);
			m_size--;
		}

		return itWhere;
	}
	/// Removes a range of elements by iterator
	inline void erase(iterator itFirst, iterator itLast)
	{
		// Replace these element by the following elements
		::std::copy(itLast, end(), itFirst);
		m_size -= itLast - itFirst;
	}

	/// Assigns the given number of elements to this vector
	inline void assign(size_type count, const Element& value)
	{
		// Allocate new elements
		if(count > m_container.size())
		{
			reallocate_internal(count);
			m_container.resize(count);
		}
		m_size = count;

		// Set elements
		::std::fill_n(m_container.begin(), count, value);
	}
	/// Assigns a range of elements to this vector
	template<class Iterator>
	inline void assign(Iterator itFirst, Iterator itLast)
	{
		size_t count = itLast - itFirst;

		// Allocate new elements
		if(count > m_container.size())
		{
			reallocate_internal(count);
			m_container.resize(count);
		}
		m_size = count;

		// Set elements
		::std::copy(itFirst, itLast, m_container.begin());
	}

	/// Clears all elements contained by this vector
	inline void clear(void) { m_size = 0; };

	/// Gets a copy of the allocator used by this vector.
	inline Allocator get_allocator() const { return m_container.get_allocator(); };
	/// Returns the maximum number of elements this vector could contain
	inline size_type max_size(void) const { return m_container.max_size(); };

	/// Returns the number of elements this vector could contain without reallocation
	inline size_type capacity(void) const { return m_container.capacity(); };
	/// Reserves storage for the specified number of elements
	inline void reserve(size_type count) { reserve_internal(count); };
	/// Inserts or erases elements to match the new size specified
	inline void resize(size_type _Newsize)
	{
		// Resize
		if(_Newsize > m_container.size())
		{
			reallocate_internal(_Newsize);
			m_container.resize(_Newsize);
		}
		m_size = _Newsize;
	}
	/// Inserts or erases elements to match the new size specified
	inline void resize(size_type _Newsize, const Element& value)
	{
		size_type _Oldsize = m_size;

		// Resize
		if(_Newsize > m_container.size())
		{
			reallocate_internal(_Newsize);
			m_container.resize(_Newsize);
		}
		m_size = _Newsize;

		// Set elements
		if(_Newsize > _Oldsize)
			::std::fill_n(m_container.begin() + _Oldsize, _Newsize - _Oldsize, value);
	};

	/// Gets an element by position
	inline reference at(size_type _Pos) { return m_container.at(_Pos); };
	/// Gets an element by position
	inline const_reference at(size_type _Pos) const { return m_container.at(_Pos); };
	/// Gets the first element in the vector
	inline reference front(void) { return m_container.front(); };
	/// Gets the first element in the vector
	inline const_reference front(void) const { return m_container.front(); };
	/// Gets the last element in the vector
	inline reference back(void) { return m_container[m_size - 1]; };
	/// Gets the last element in the vector
	inline const_reference back(void) const { return m_container[m_size - 1]; };

	/// Gets an element by position
	inline reference operator [] (size_type _Pos) { return m_container[_Pos]; };
	/// Gets an element by position
	inline const_reference operator [] (size_type _Pos) const { return m_container[_Pos]; };

	/// Returns an iterator to the first element contained by this vector
	inline iterator begin(void) { return m_container.begin(); };
	/// Returns a constant iterator to the first element contained by this vector
	inline const_iterator begin(void) const { return m_container.begin(); };
	/// Returns an iterator beyond the last element contained by this vector
	inline iterator end(void) { return m_container.begin() + m_size; };
	/// Returns a constant iterator beyond the last element contained by this vector
	inline const_iterator end(void) const { return m_container.begin() + m_size; };

	/// Returns a reverse iterator to the last element contained by this vector
	inline reverse_iterator rbegin(void) { return m_container.rend() - m_size; };
	/// Returns a constant reverse iterator to the last element contained by this vector
	inline const_reverse_iterator rbegin(void) const { return m_container.rend() - m_size; };
	/// Returns a reverse iterator beyond the first element contained by this vector
	inline reverse_iterator rend(void) { return m_container.rend(); };
	/// Returns a constant reverse iterator beyond the first element contained by this vector
	inline const_reverse_iterator rend(void) const { return m_container.rend(); };

	// Swaps the elements of two unordered vectors
	inline void swap(accumulation_vector& right)
	{
		m_container.swap(right.m_container);
		::std::swap(m_size, right.m_size);
	}
};

/// Comparison operator
template<class Element, class reserve_internal, class allocatorloc>
inline bool operator ==(const accumulation_vector<Element, reserve_internal, allocatorloc>& _Left, 
	const accumulation_vector<Element, reserve_internal, allocatorloc>& right)
{
	return _Left.size() == right.size() && ::std::equal(_Left.begin(), _Left.end(), right.begin());
}

/// Comparison operator
template<class Element, class reserve_internal, class allocatorloc>
inline bool operator !=(const accumulation_vector<Element, reserve_internal, allocatorloc>& _Left,
	const accumulation_vector<Element,reserve_internal,  allocatorloc>& right)
{
	return !(_Left == right);
}

/// Comparison operator
template<class Element, class reserve_internal, class allocatorloc>
inline bool operator <(const accumulation_vector<Element, reserve_internal, allocatorloc>& _Left,
	const accumulation_vector<Element, reserve_internal, allocatorloc>& right)
{
	return ::std::lexicographical_compare(_Left.begin(), _Left.end(), right.begin(), right.end());
}

/// Comparison operator
template<class Element, class reserve_internal, class allocatorloc>
inline bool operator >(const accumulation_vector<Element, reserve_internal, allocatorloc>& _Left,
	const accumulation_vector<Element, reserve_internal, allocatorloc>& right)
{
	return right < _Left;
}

/// Comparison operator
template<class Element, class reserve_internal, class allocatorloc>
inline bool operator <=(const accumulation_vector<Element, reserve_internal, allocatorloc>& _Left,
	const accumulation_vector<Element, reserve_internal, allocatorloc>& right)
{
	return !(right < _Left);
}

/// Comparison operator
template<class Element, class reserve_internal, class allocatorloc>
inline bool operator >=(const accumulation_vector<Element, reserve_internal, allocatorloc>& _Left,
	const accumulation_vector<Element, reserve_internal, allocatorloc>& right)
{
	return !(_Left < right);
}

// Swaps the elements of two vectors
template<class Element, class reserve_internal, class allocatorloc>
inline void swap(accumulation_vector<Element, reserve_internal, allocatorloc>& _Left,
	accumulation_vector<Element, reserve_internal, allocatorloc>& right)
{
	_Left.swap(right);
}

} // namespace
} // namespace

#endif