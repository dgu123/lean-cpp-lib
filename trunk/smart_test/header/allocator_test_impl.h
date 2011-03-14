#pragma once

#include <memory>

template<class Element>
class test_allocator
{
	template <class Other>
	friend class test_allocator;

private:
	typedef std::allocator<Element> allocator_type;
	allocator_type m_allocator;

	int *m_allocCount;
	int *m_constructCount;
	int *m_destructCount;
	int *m_deallocCount;

public:
	typedef typename allocator_type::value_type value_type;

	typedef typename allocator_type::pointer pointer;
	typedef typename allocator_type::reference reference;
	typedef typename allocator_type::const_pointer const_pointer;
	typedef typename allocator_type::const_reference const_reference;

	typedef typename allocator_type::size_type size_type;
	typedef typename allocator_type::difference_type difference_type;

	template<class Other>
	struct rebind
	{
		typedef test_allocator<Other> other;
	};
	
	test_allocator(int *allocCount, int *constructCount, int *destructCount, int *deallocCount)
		: m_allocCount(allocCount),
		m_constructCount(constructCount),
		m_destructCount(destructCount),
		m_deallocCount(deallocCount) { };
	template<class Other>
	test_allocator(const test_allocator<Other> &right)
		: m_allocator(right.m_allocator),
		m_allocCount(right.m_allocCount),
		m_constructCount(right.m_constructCount),
		m_destructCount(right.m_destructCount),
		m_deallocCount(right.m_deallocCount) { };
	template<class Other>
	test_allocator& operator=(const test_allocator<Other> &right)
	{
		m_allocator = right.m_allocator;
		m_allocCount = right.m_allocCount;
		m_constructCount = right.m_constructCount;
		m_destructCount = right.m_destructCount;
		m_deallocCount = right.m_deallocCount;
		return *this;
	}
	
	pointer allocate(size_type count)
	{
		*m_allocCount += count;
		return m_allocator.allocate(count);
	}
	pointer allocate(size_type count, const void *hint)
	{
		*m_allocCount += count;
		return m_allocator.allocate(count, hint);
	}
	void deallocate(pointer ptr, size_type count)
	{
		*m_deallocCount += count;
		m_allocator.deallocate(ptr, count);
	}

	void construct(pointer ptr, const Element& value)
	{
		*m_constructCount += count;
		m_allocator.construct(ptr, value);
	}
	void construct(pointer ptr, Element&& value)
	{
		++*m_constructCount;
		m_allocator.construct(ptr, ::std::move(value));
	}
	template<class Other>
	void construct(pointer ptr, Other&& value)
	{
		++*m_constructCount;
		m_allocator.construct(ptr, ::std::move(value));
	}
	void destroy(pointer ptr)
	{
		--*m_destructCount;
		m_allocator.destroy(ptr);
	}

	pointer address(reference value) const
	{
		return m_allocator.address(value);
	}
	const_pointer address(const_reference value) const
	{
		return m_allocator.address(value);
	}

	size_t max_size() const
	{
		return m_allocator.max_size();
	}

};

/*

#include <lean/meta/strip.h>

template<class Element>
class test_allocator
{
public:
	typedef typename lean::strip_const<Element>::type value_type;

	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type* const_pointer;
	typedef const value_type& const_reference;

	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template<class Other>
	struct rebind
	{
		typedef test_allocator<Other> other;
	};
	
	test_allocator() throw() { };
	test_allocator(const test_allocator&) throw() { };
	template<class Other>
	test_allocator(const test_allocator<Other>&) throw() { };
	template<class Other>
	test_allocator& operator=(const test_allocator<Other>&) { return *this; };
	
	pointer allocate(size_type count)
	{
		return (_Allocate(count, (pointer)0));
	}

	pointer allocate(size_type count, const void*)
	{
		return allocate(count);
	}

	void deallocate(pointer ptr, size_type)
	{
		::operator delete(ptr);
	}

	void construct(pointer _Ptr, const Element& _Val)
	{	// construct object at _Ptr with value _Val
		_Construct(_Ptr, _Val);
	}

	void construct(pointer _Ptr, Element&& _Val)
	{	// construct object at _Ptr with value _Val
		::new ((void _FARQ *)_Ptr) Element(_STD forward<Element>(_Val));
	}

	template<class _Other>
	void construct(pointer _Ptr, _Other&& _Val)
	{
		::new ((void _FARQ *)_Ptr) Element(_STD forward<_Other>(_Val));
	}

	void destroy(pointer _Ptr)
	{
		_Destroy(_Ptr);
	}

	pointer address(reference value) const
	{
		return reinterpret_cast<pointer>( &reinterpret_cast<char&>(value) );
	}

	const_pointer address(const_reference _Val) const
	{
		return reinterpret_cast<const_pointer>( &reinterpret_cast<const char&>(value) );
	}

	size_t max_size() const throw()
	{
		size_t count = static_cast<size_t>(-1) / sizeof(Element);
		return (0 < count) ? count : 1;
	}
};

*/