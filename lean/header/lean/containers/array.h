/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_ARRAY
#define LEAN_CONTAINERS_ARRAY

#include "../lean.h"

namespace lean 
{
namespace containers
{

/// Array class.
template<class Element, size_t Size>
class array
{
public:
	/// Size.
	static const size_t count = Size;

	/// Type of the size returned by this vector.
	typedef Element value_type;
	/// Type of the corresponding static array.
	typedef value_type array_type[count];
	/// Type of the size returned by this vector.
	typedef size_t size_type;
	/// Pointer difference type.
	typedef ptrdiff_t difference_type;
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

	/// Allows for the construction of uninitialized arrays.
	enum uninitialized_t
	{
		/// Allows for the construction of uninitialized arrays.
		uninitialized
	};

private:
	char m_data[sizeof(array_type)];

public:
	/// Constructs an array.
	array()
	{
		for (pointer it = data(); it < data_end(); ++it)
			new (static_cast<void*>(it)) value_type();
	}
	
	/// Constructs an uninitialized array.
	LEAN_INLINE array(uninitialized_t) { }

	/// Copies all elements from the given array to this array.
	array(const array &right)
	{
		const_iterator rit = right.begin();

		for (pointer it = data(); it < data_end(); ++it, ++rit)
			new (static_cast<void*>(it)) value_type(*rit);
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given array to this array.
	array(array &&right)
	{
		iterator rit = right.begin();

		for (pointer it = data(); it < data_end(); ++it, ++rit)
			new (static_cast<void*>(it)) value_type(std::move(*rit));
	}
#endif

	/// Constructs an array by passing the given arguments to the constructor of every element.
	template <class V1>
	explicit array(const V1 &v1)
	{
		for (pointer it = data(); it < data_end(); ++it)
			new (static_cast<void*>(it)) value_type(v1);
	}
	/// Constructs an array by passing the given arguments to the constructor of every element.
	template <class V1, class V2>
	explicit array(const V1 &v1, const V2 &v2)
	{
		for (pointer it = data(); it < data_end(); ++it)
			new (static_cast<void*>(it)) value_type(v1, v2);
	}
	/// Constructs an array by passing the given arguments to the constructor of every element.
	template <class V1, class V2, class V3>
	explicit array(const V1 &v1, const V2 &v2, const V3 &v3)
	{
		for (pointer it = data(); it < data_end(); ++it)
			new (static_cast<void*>(it)) value_type(v1, v2, v3);
	}

	/// Destroys all elements in this array (even if uninitialized!).
	~array()
	{
		for (pointer it = data(); it < data_end(); ++it)
			it->~value_type();
	}

	/// Copies all elements from the given array to this array.
	array& operator =(const array &right)
	{
		const_iterator rit = right.begin();

		for (pointer it = data(); it < data_end(); ++it, ++rit)
			*it = *rit;

		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given array to this array.
	array& operator =(array &&right)
	{
		iterator rit = right.begin();

		for (pointer it = data(); it < data_end(); ++it, ++rit)
			*it = std::move(*rit);

		return *this;
	}
#endif

	/// Copies the given value to every element of this array.
	LEAN_INLINE void fill(const value_type& value)
	{
		std::fill_n(data(), count, value);
	}
	/// Copies the given value to every element of this array.
	LEAN_INLINE void assign(const value_type& value) { fill(value); }

	/// Gets a pointer to the first element of this array.
	LEAN_INLINE pointer data() { return reinterpret_cast<pointer>(&m_data[0]); }
	/// Gets a pointer to the first element of this array.
	LEAN_INLINE const_pointer data() const { return reinterpret_cast<const_pointer>(&m_data[0]); }
	/// Gets a pointer to the first element of this array.
	LEAN_INLINE const_pointer cdata() const { return data(); }

	/// Gets a pointer to one past the last element of this array.
	LEAN_INLINE pointer data_end() { return data() + count; }
	/// Gets a pointer to one past the last element of this array.
	LEAN_INLINE const_pointer data_end() const { return data() + count; }
	/// Gets a pointer to one past the last element of this array.
	LEAN_INLINE const_pointer cdata_end() const { return data() + count; }

	/// Gets an iterator to the first element of this array.
	LEAN_INLINE iterator begin() { return data(); }
	/// Gets an iterator to the first element of this array.
	LEAN_INLINE const_iterator begin() const { return data(); }
	/// Gets an iterator to the first element of this array.
	LEAN_INLINE const_iterator cbegin() const { return data(); }
	/// Gets an iterator one past the last element of this array.
	LEAN_INLINE iterator end() { return data_end(); }
	/// Gets an iterator one past the last element of this array.
	LEAN_INLINE const_iterator end() const { return data_end(); }
	/// Gets an iterator one past the last element of this array.
	LEAN_INLINE const_iterator cend() const { return data_end(); }

	/// Gets the number of elements in this array.
	LEAN_INLINE size_type size() const { return count; }
	/// Gets the number of elements in this array.
	LEAN_INLINE size_type max_size() const { return count; }
	/// True, iff size() > 0.
	LEAN_INLINE bool empty() const { return (count == 0); }

	/// Gets a reference to the element at the given position.
	LEAN_INLINE reference operator[](size_type pos) { return data()[pos]; }
	/// Gets a reference to the element at the given position.
	LEAN_INLINE const_reference operator[](size_type pos) const { return data()[pos]; }

	/// Gets a reference to the first element.
	LEAN_INLINE reference front() { return data()[0]; }
	/// Gets a reference to the first element.
	LEAN_INLINE const_reference front() const { return data()[0]; }
	/// Gets a reference to the last element.
	LEAN_INLINE reference back() { return data()[count - 1]; }
	/// Gets a reference to the last element.
	LEAN_INLINE const_reference back() const { return data()[count - 1]; }
};

} // namespace

using containers::array;

} // namespace

#endif