/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_CONTAINERS_PARALLEL_VECTOR
#define LEAN_CONTAINERS_PARALLEL_VECTOR

#include "../lean.h"
#include "../functional/variadic.h"
#include "construction.h"
#include "vector_policies.h"

namespace lean 
{
namespace containers
{

struct parallel_vector_base
{
	template <class Alloc>
	LEAN_INLINE void reallocate(size_t size, size_t capacity, Alloc) { }
	template <class Alloc>
	LEAN_INLINE void deallocate(size_t size, Alloc) { }
	LEAN_INLINE void operator ()(struct ignore&) const { }
	LEAN_INLINE void get(struct ignore&) const { }
	LEAN_INLINE void place_back() { }
	LEAN_INLINE void place_back_from(const parallel_vector_base&, size_t) { }
	LEAN_INLINE void erase(size_t idx) { }
	LEAN_INLINE void clear() { }
	LEAN_INLINE void resize(size_t size) { }
	LEAN_INLINE void reserve(size_t size) { }
	LEAN_INLINE void swap(parallel_vector_base&) { }
};

template <class Type, class Tag, class VectorBinder, class Base = parallel_vector_base>
class parallel_vector_array : private Base
{
	template <class OtherType, class OtherTag, class VectorBinder, class OtherBase>
	friend class parallel_vector_array;

public:
	typedef Type value_type;
	typedef value_type* pointer;
	typedef typename VectorBinder::template rebind<Type>::allocator_type allocator_type;
	typedef typename allocator_type::size_type size_type;

private:
	pointer p;
	
public:
	LEAN_INLINE parallel_vector_array() : p() { }
	LEAN_INLINE ~parallel_vector_array()
	{
		// IMPORTANT: Make sure array was deallocated
		LEAN_ASSERT(p == nullptr);
	}

#ifndef LEAN0X_NO_RVALUE_REFERENCES
	LEAN_INLINE parallel_vector_array(parallel_vector_array &&right)
		: Base(std::move(right)),
		p(right.p)
	{
		right.p = nullptr;
	}

	LEAN_INLINE parallel_vector_array& operator =(parallel_vector_array &&right)
	{
		this->Base::operator =(std::move(right));
		p = right.p;
		right.p = nullptr;
		return *this;
	}
#endif
		
	using Base::get;
	LEAN_INLINE value_type* get(Tag) { return p; }
	LEAN_INLINE const value_type* get(Tag) const { return p; }

	using Base::operator ();
	LEAN_INLINE value_type* operator ()(Tag) { return p; }
	LEAN_INLINE const value_type* operator ()(Tag) const { return p; }
	
	void reallocate(size_t capacity, allocator_type allocator, size_t size)
	{
		this->Base::reallocate(capacity, allocator, size);

		value_type *sourceP = p;
		p = allocator.allocate(capacity);
		
		if (sourceP)
		{
			try { move_construct(sourceP, sourceP + size, p, no_allocator); } LEAN_ASSERT_NOEXCEPT
			
			destruct(sourceP, sourceP + size, no_allocator);
			allocator.deallocate(sourceP);
		}
	}
	void deallocate(allocator_type allocator, size_t size)
	{
		this->Base::deallocate(allocator, size);

		if (p)
		{
			destruct(p, p + size, no_allocator);
			allocator.deallocate(p);
			p = nullptr;
		}
	}

	void place_back(size_t pos) noexcept
	{
		this->Base::place_back(pos);
		new(static_cast<void*>(p + pos)) value_type();
	}
	
	#define LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_TPARAMS \
		class T
	#define LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_DECL \
		void place_back
	#define LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_PARAMS \
		size_t pos, T LEAN_FW_REF val
	#define LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_BODY(call) \
		{ \
			this->Base::place_back##call; \
			new(static_cast<void*>(p + pos)) value_type( LEAN_FORWARD(T, val) ); \
		}
	#define LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_ARGS \
		pos
	LEAN_VARIADIC_TEMPLATE_TPA(LEAN_FORWARD, LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_DECL, LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_TPARAMS,
		LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_PARAMS, noexcept, LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_BODY, LEAN_PARALLEL_VEC_ARRAY_PUSH_BACK_METHOD_ARGS)

	LEAN_INLINE void place_back_from(size_t pos, const parallel_vector_array& src, size_t index) noexcept
	{
		this->Base::place_back_from(pos, src, index);
		new(static_cast<void*>(p + pos)) value_type(src.p[index]);
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	LEAN_INLINE void place_back_from(size_t pos, parallel_vector_array &&src, size_t index) noexcept
	{
		this->Base::place_back_from(pos, LEAN_MOVE(src), index);
		new(static_cast<void*>(p + pos)) value_type(LEAN_MOVE(src.p[index]));
	}
#endif

	void erase(size_t idx, size_t oldSize) noexcept
	{
		this->Base::erase(idx);
		p[idx].~value_type();
		move(p + idx + 1, p + oldSize, p, no_allocator);
	}
	
	void clear(size_t size) noexcept
	{
		this->Base::clear();
		destruct(p, p + size, no_allocator);
	}

	void resize(size_t newSize, size_t oldSize) noexcept
	{
		this->Base::resize(newSize, oldSize);

		if (oldSize < newSize)
			default_construct(p + oldSize, p + newSize, no_allocator);
		else if (oldSize > newSize)
			destruct(p + newSize, p + oldSize, no_allocator);
	}

	LEAN_INLINE value_type& operator [](size_t idx) { return p[idx]; }
	LEAN_INLINE const value_type& operator [](size_t idx) const { return p[idx]; }

	LEAN_INLINE value_type* current() { return p; }
	LEAN_INLINE const value_type* current() const { return p; }
	LEAN_INLINE Base& next() { return *this; }
	LEAN_INLINE const Base& next() const { return *this; }

	LEAN_INLINE void swap(parallel_vector_array &right)
	{
		this->Base::swap(right);
		using std::swap;
		swap(p, right.p);
	}
};


template <class Type, class Tag, class VectorBinder, class Base = parallel_vector_base>
class parallel_vector : private Base
{
public:
	typedef typename VectorBinder::template rebind<Type>::type vector_type;
	typedef typename vector_type::value_type value_type;
	typedef typename vector_type::allocator_type allocator_type;
	typedef typename allocator_type::size_type size_type;

private:
	vector_type v;

	LEAN_INLINE void grow_by(size_type count)
	{
		size_type oldCapacity = v.capacity();
		v.reserve_grow_by(count);
		size_type newCapacity = v.capacity();

		if (newCapacity > oldCapacity)
			this->Base::reallocate(newCapacity, v.get_allocator(), v.size());
	}

public:
	LEAN_INLINE parallel_vector() { }
	LEAN_INLINE parallel_vector(const allocator_type &allocator)
		: v(allocator) { }
	LEAN_INLINE ~parallel_vector()
	{
		this->Base::deallocate(v.get_allocator(), v.size());
	}

	LEAN_INLINE vector_type& get(Tag) { return v; }
	LEAN_INLINE const vector_type& get(Tag) const { return v; }

	LEAN_INLINE vector_type& operator ()(Tag) { return v; }
	LEAN_INLINE const vector_type& operator ()(Tag) const { return v; }

	void push_back()
	{
		grow_by(1);
		size_type at = v.size();
		v.push_back(value_type());
		// NOTE: noexcept
		this->Base::place_back(at);
	}
	
	#define LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_TPARAMS \
		class T
	#define LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_DECL \
		void push_back
	#define LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_PARAMS \
		T LEAN_FW_REF val
	#define LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_BODY(call) \
		{ \
			grow_by(1); \
			size_type at = v.size(); \
			v.push_back( LEAN_FORWARD(T, val) ); \
			/* NOTE: noexcept */ \
			this->Base::place_back##call; \
		}
	#define LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_ARGS(call) \
		at
	LEAN_VARIADIC_TEMPLATE_TPA(LEAN_FORWARD, LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_DECL, LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_TPARAMS,
		LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_PARAMS, LEAN_NOTHING, LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_BODY, LEAN_PARALLEL_VECTOR_PUSH_BACK_METHOD_ARGS)

	LEAN_INLINE void push_back_from(const parallel_vector& src, size_type index)
	{
		grow_by(1);
		size_type at = v.size();
		v.push_back(src.v[index]);
		// NOTE: noexcept
		this->Base::place_back_from(at, src, index);
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	LEAN_INLINE void push_back_from(parallel_vector &&src, size_type index)
	{
		grow_by(1);
		size_type at = v.size();
		v.push_back(LEAN_MOVE(src.v[index]));
		// NOTE: noexcept
		this->Base::place_back_from(at, LEAN_MOVE(src), index);
	}
#endif

	void erase(size_type idx)
	{
		size_type oldSize = v.size();
		v.erase(v.begin() + idx);
		// NOTE: noexcept
		this->Base::erase(idx, oldSize);
	}
	
	void clear()
	{
		size_type oldSize = v.size();
		v.clear();
		// NOTE: noexcept
		this->Base::clear(oldSize);
	}

	void resize(size_type size)
	{
		size_type oldCapacity = v.capacity();
		v.reserve_grow_to(size);
		size_type newCapacity = v.capacity();

		if (newCapacity > oldCapacity)
			this->Base::reallocate(newCapacity, v.get_allocator(), v.size());

		size_type oldSize = v.size();
		v.resize(size);
		// NOTE: noexcept
		this->Base::resize(size, oldSize);
	}
	LEAN_INLINE void reserve(size_type size)
	{
		size_type oldCapacity = v.capacity();
		v.reserve(size);
		size_type newCapacity = v.capacity();

		if (newCapacity > oldCapacity)
			this->Base::reallocate(newCapacity, v.get_allocator(), v.size());
	}

	LEAN_INLINE size_type size() const { return v.size(); }

	LEAN_INLINE value_type& operator [](size_type idx) { return v[idx]; }
	LEAN_INLINE const value_type& operator [](size_type idx) const { return v[idx]; }

	LEAN_INLINE vector_type& current() { return v; }
	LEAN_INLINE const vector_type& current() const { return v; }
	LEAN_INLINE Base& next() { return *this; }
	LEAN_INLINE const Base& next() const { return *this; }

	LEAN_INLINE void swap(parallel_vector &right)
	{
		using std::swap;
		swap(v, right.v);
	}
};

/// Manages several parallel vectors.

/// Swaps the given two multi_vectors.
template <class Type, class ID, class VectorBinder, class Base>
LEAN_INLINE void swap(parallel_vector<Type, ID, VectorBinder, Base> &left, parallel_vector<Type, ID, VectorBinder, Base> &right)
{
	left.swap(right);
}

/// Swizzles the given multi-vector using the given index array.
template <class Type, class ID, class VectorBinder, class Base, class Source, class Iterator>
LEAN_INLINE void append_swizzled(Source LEAN_FW_REF src, Iterator begin, Iterator end, parallel_vector<Type, ID, VectorBinder, Base> &dest)
{
	dest.reserve(dest.size() + (end - begin));
	while (begin < end)
		dest.push_back_from(LEAN_FORWARD(Source, src), *begin++);
}

template <class VectorBinder>
struct parallel_vector_t
{
	template <class Type1, class ID1,
		class Type2 = void, class ID2 = void,
		class Type3 = void, class ID3 = void,
		class Type4 = void, class ID4 = void,
		class Type5 = void, class ID5 = void,
		class Type6 = void, class ID6 = void,
		class Type7 = void, class ID7 = void,
		class Type8 = void, class ID8 = void,
		class Type9 = void, class ID9 = void>
	struct make
	{
		typedef parallel_vector<Type1, ID1, VectorBinder, typename parallel_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7, Type8, ID8, Type9, ID9>::type> type;
	};

	template <class Type1, class ID1>
	struct make<Type1, ID1>
	{
		typedef parallel_vector<Type1, ID1, VectorBinder> type;
	};
	template <class Type1, class ID1, class Type2, class ID2>
	struct make<Type1, ID1, Type2, ID2>
	{
		typedef parallel_vector<Type1, ID1, VectorBinder, typename parallel_vector_t::template make<Type2, ID2>::type> type;
	};
	template <class Type1, class ID1, class Type2, class ID2, class Type3, class ID3>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3>
	{
		typedef parallel_vector<Type1, ID1, VectorBinder, typename parallel_vector_t::template make<Type2, ID2, Type3, ID3>::type> type;
	};
	template <class Type1, class ID1, class Type2, class ID2, class Type3, class ID3, class Type4, class ID4>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4>
	{
		typedef parallel_vector<Type1, ID1, VectorBinder, typename parallel_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4>::type> type;
	};
	template <class Type1, class ID1, class Type2, class ID2, class Type3, class ID3, class Type4, class ID4, class Type5, class ID5>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5>
	{
		typedef parallel_vector<Type1, ID1, VectorBinder, typename parallel_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5>::type> type;
	};
	template <class Type1, class ID1, class Type2, class ID2, class Type3, class ID3, class Type4, class ID4, class Type5, class ID5, class Type6, class ID6>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6>
	{
		typedef parallel_vector<Type1, ID1, VectorBinder, typename parallel_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6>::type> type;
	};
	template <class Type1, class ID1, class Type2, class ID2, class Type3, class ID3, class Type4, class ID4, class Type5, class ID5, class Type6, class ID6, class Type7, class ID7>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7>
	{
		typedef parallel_vector<Type1, ID1, VectorBinder, typename parallel_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7>::type> type;
	};
	template <class Type1, class ID1, class Type2, class ID2, class Type3, class ID3, class Type4, class ID4, class Type5, class ID5, class Type6, class ID6, class Type7, class ID7, class Type8, class ID8>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7, Type8, ID8>
	{
		typedef parallel_vector<Type1, ID1, VectorBinder, typename parallel_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7, Type8, ID8>::type> type;
	};
};

} // namespace

using containers::parallel_vector;
using containers::parallel_vector_t;
using containers::append_swizzled;
using containers::swap;

} // namespace

#endif