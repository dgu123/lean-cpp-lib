/*****************************************************/
/* lean Memory                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MEMORY_HEAP_ALLOCATOR
#define LEAN_MEMORY_HEAP_ALLOCATOR

#include "../lean.h"
#include "../meta/strip.h"
#include "alignment.h"
#include "default_heap.h"

namespace lean
{
namespace memory
{

/// STL allocator heap adapter.
template<class Element, class Heap = default_heap, size_t Alignment = alignof(Element)>
class heap_allocator
{
public:
	/// Heap adapted by this heap allocator.
	typedef Heap heap_type;

	/// Value type.
	typedef typename strip_const<Element>::type value_type;

	/// Pointer type.
	typedef typename value_type* pointer;
	/// Reference type.
	typedef typename value_type& reference;
	/// Pointer type.
	typedef typename const value_type* const_pointer;
	/// Reference type.
	typedef typename const value_type& const_reference;

	/// Size type.
	typedef typename heap_type::size_type size_type;
	/// Pointer difference type.
	typedef ptrdiff_t difference_type;

	/// Allows for the creation of differently-typed equivalent allocators.
	template<class Other>
	struct rebind
	{
		/// Equivalent allocator allocating elements of type Other.
		typedef heap_allocator<Other, Heap, Alignment> other;
	};
	
	/// Default constructor.
	LEAN_INLINE heap_allocator() { }
	/// Copy constructor.
	template<class Other>
	LEAN_INLINE heap_allocator(const heap_allocator<Other, Heap, Alignment> &right) { }
	/// Assignment operator.
	template<class Other>
	LEAN_INLINE heap_allocator& operator=(const heap_allocator<Other, Heap, Alignment> &right) { return *this; }
	
	/// Allocates the given number of elements.
	LEAN_INLINE pointer allocate(size_type count)
	{
		return heap_type::allocate<Alignment>(count * sizeof(value_type));
	}
	/// Allocates the given amount of memory.
	LEAN_INLINE pointer allocate(size_type count, const void *)
	{
		return allocate(count);
	}
	/// Allocates the given amount of memory.
	LEAN_INLINE void deallocate(pointer ptr, size_type)
	{
		heap_type::free<Alignment>(ptr);
	}

	/// Constructs a new element from the given value at the given pointer.
	LEAN_INLINE void construct(pointer ptr, const value_type& value)
	{
		new(ptr) Element(value);
	}
	/// Constructs a new element from the given value at the given pointer.
	template<class Other>
	LEAN_INLINE void construct(pointer ptr, const Other& value)
	{
		new(ptr) Element(value);
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Constructs a new element from the given value at the given pointer.
	LEAN_INLINE void construct(pointer ptr, value_type&& value)
	{
		new(ptr) Element(std::forward<value_type>(value));
	}
	/// Constructs a new element from the given value at the given pointer.
	template<class Other>
	LEAN_INLINE void construct(pointer ptr, Other&& value)
	{
		new(ptr) Element(std::forward<Other>(value));
	}
#endif
	/// Destructs an element at the given pointer.
	LEAN_INLINE void destroy(pointer ptr)
	{
		ptr->~Element();
	}

	/// Gets the address of the given element.
	LEAN_INLINE pointer address(reference value) const
	{
		return reinterpret_cast<pointer>( &reinterpret_cast<char&>(value) );
	}
	/// Gets the address of the given element.
	LEAN_INLINE const_pointer address(const_reference value) const
	{
		return reinterpret_cast<const_pointer>( &reinterpret_cast<const char&>(value) );
	}

	/// Estimates the maximum number of elements that may be constructed.
	LEAN_INLINE size_t max_size() const
	{
		size_t count = static_cast<size_t>(-1) / sizeof(Element);
		return (0 < count) ? count : 1;
	}
};

#ifndef DOXYGEN_SKIP_THIS

/// STL allocator heap adapter.
template<class Heap, size_t Alignment = 1>
class heap_allocator<void, Heap, Alignment>
{
public:
	/// Heap adapted by this heap allocator.
	typedef Heap heap_type;

	/// Value type.
	typedef void value_type;

	/// Pointer type.
	typedef typename value_type* pointer;
	/// Pointer type.
	typedef typename const value_type* const_pointer;

	/// Size type.
	typedef typename heap_type::size_type size_type;
	/// Pointer difference type.
	typedef ptrdiff_t difference_type;

	/// Allows for the creation of differently-typed equivalent allocators.
	template<class Other>
	struct rebind
	{
		/// Equivalent allocator allocating elements of type Other.
		typedef heap_allocator<Other, Heap, Alignment> other;
	};
	
	/// Default constructor.
	LEAN_INLINE heap_allocator() { }
	/// Copy constructor.
	template<class Other>
	LEAN_INLINE heap_allocator(const heap_allocator<Other, Heap, Alignment> &right) { }
	/// Assignment operator.
	template<class Other>
	LEAN_INLINE heap_allocator& operator=(const heap_allocator<Other, Heap, Alignment> &right) { return *this; }
};

#endif

/// Checks the given two allocators for equivalence.
template<class Element, class Heap, size_t Alignment, class Other>
LEAN_INLINE bool operator ==(const heap_allocator<Element, Heap, Alignment>&, const allocator<Other, Heap, Alignment>&)
{
	return true;
}

/// Checks the given two allocators for inequivalence.
template<class Element, class Heap, size_t Alignment, class Other>
LEAN_INLINE bool operator !=(const heap_allocator<Element, Heap, Alignment>&, const allocator<Other, Heap, Alignment>&)
{
	return false;
}

} // namespace

using memory::heap_allocator;

} // namespace

#endif