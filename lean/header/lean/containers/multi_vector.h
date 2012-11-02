/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_CONTAINERS_MULTI_VECTOR
#define LEAN_CONTAINERS_MULTI_VECTOR

#include "../lean.h"
#include "../functional/variadic.h"
#include <vector>

namespace lean 
{
namespace containers
{
	
struct multi_vector_base
{
	LEAN_INLINE multi_vector_base() { }
	template <class T1>
	LEAN_INLINE multi_vector_base(T1 LEAN_FW_REF) { }
	template <class T1, class T2>
	LEAN_INLINE multi_vector_base(T1 LEAN_FW_REF, T2 LEAN_FW_REF) { }

	LEAN_INLINE void push_back() { }
	LEAN_INLINE void erase(size_t idx) { }
	LEAN_INLINE void clear() { }
	LEAN_INLINE void resize(size_t size) { }
	LEAN_INLINE void reserve(size_t size) { }
};

/// Default vector binder.
template <template <class E, class A> class Vector, template <class T> class Allocator>
struct vector_binder
{
	/// Constructs a vector type from the given element type.
	template <class Type>
	struct rebind
	{
		typedef Vector< Type, Allocator<Type> > type;
	};
};

/// Manages several parallel vectors.
template <class Type, int ID, class VectorBinder, class Base = multi_vector_base>
class multi_vector : private Base
{
	template <class OtherType, int OtherID, class OtherVectorBinder, class OtherBase>
	friend class multi_vector;

public:
	typedef typename VectorBinder::template rebind<Type>::type vector_type;
	typedef typename vector_type::allocator_type allocator_type;

private:
	vector_type v;

	template <class T>
	struct get_helper
	{
		typedef typename Base::get_helper<T>::type type;
		LEAN_INLINE static type& get(multi_vector &m) { return Base::get_helper<T>::get(m); }
		LEAN_INLINE static const type& get(const multi_vector &m) { return Base::get_helper<T>::get(m); }
	};
	template <>
	struct get_helper<Type>
	{
		typedef vector_type type;
		LEAN_INLINE static type& get(multi_vector &m) { return m.v; }
		LEAN_INLINE static const type& get(const multi_vector &m) { return m.v; }
	};

	template <int I>
	struct get_helper_int
	{
		typedef typename Base::get_helper_int<I>::type type;
		LEAN_INLINE static type& get(multi_vector &m) { return Base::get_helper_int<I>::get(m); }
		LEAN_INLINE static const type& get(const multi_vector &m) { return Base::get_helper_int<I>::get(m); }
	};
	template <>
	struct get_helper_int<ID>
	{
		typedef vector_type type;
		LEAN_INLINE static type& get(multi_vector &m) { return m.v; }
		LEAN_INLINE static const type& get(const multi_vector &m) { return m.v; }
	};

	LEAN_NOINLINE void pop_or_terminate()
	{
		try
		{
			v.pop_back();
		}
		catch (...)
		{
			LEAN_ASSERT_DEBUG(false);
			std::terminate();
		}
	}

	LEAN_NOINLINE void restore_or_terminate(size_t oldSize)
	{
		try
		{
			v.resize(oldSize);
		}
		catch (...)
		{
			LEAN_ASSERT_DEBUG(false);
			std::terminate();
		}
	}

public:
	LEAN_INLINE multi_vector() { }
	LEAN_INLINE multi_vector(const allocator_type &allocator)
		: Base(allocator),
		v(allocator) { }
	LEAN_INLINE multi_vector(size_t size, const allocator_type &allocator = allocator_type())
		: Base(size, allocator),
		v(size, allocator) { }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	LEAN_INLINE multi_vector(multi_vector &&right)
		: Base(std::move(right)),
		v(std::move(right.v)) { }

	multi_vector& operator =(multi_vector &&right)
	{
		this->Base::operator =(std::move(right));
		v = std::move(right.v);
		return *this;
	}
#endif

	template <class T>
	LEAN_INLINE typename get_helper<T>::type& get() { return get_helper<T>::get(*this); }
	template <class T>
	LEAN_INLINE const typename get_helper<T>::type& get() const { return get_helper<T>::get(*this); }

	template <int I>
	LEAN_INLINE typename get_helper_int<I>::type& get() { return get_helper_int<I>::get(*this); }
	template <int I>
	LEAN_INLINE const typename get_helper_int<I>::type& get() const { return get_helper_int<I>::get(*this); }

	void push_back()
	{
		v.push_back(Type());

		try
		{
			this->Base::push_back();
		}
		catch (...)
		{
			pop_or_terminate();
			throw;
		}
	}
	
	#define LEAN_PUSH_BACK_METHOD_TPARAMS \
		class T
	#define LEAN_PUSH_BACK_METHOD_DECL \
		void push_back
	#define LEAN_PUSH_BACK_METHOD_PARAMS \
		T LEAN_FW_REF val
	#define LEAN_PUSH_BACK_METHOD_BODY(call) \
		{ \
			v.push_back( LEAN_FORWARD(T, val) ); \
			try \
			{ \
				this->Base::push_back##call; \
			} \
			catch (...) \
			{ \
				pop_or_terminate(); \
				throw; \
			} \
		}
	LEAN_VARIADIC_TEMPLATE_TP(LEAN_FORWARD, LEAN_PUSH_BACK_METHOD_DECL, LEAN_PUSH_BACK_METHOD_TPARAMS, LEAN_PUSH_BACK_METHOD_PARAMS,
		LEAN_NOTHING, LEAN_PUSH_BACK_METHOD_BODY)

	void erase(size_t idx)
	{
		v.erase(v.begin() + idx);
		try
		{
			this->Base::erase(idx);
		}
		catch (...)
		{
			LEAN_ASSERT_DEBUG(false);
			std::terminate();
		}
	}
	
	void clear()
	{
		v.clear();
		try
		{
			this->Base::clear();
		}
		catch (...)
		{
			LEAN_ASSERT_DEBUG(false);
			std::terminate();
		}
	}

	void resize(size_t size)
	{
		size_t oldSize = v.size();
		v.resize(size);
		try
		{
			this->Base::resize(size);
		}
		catch (...)
		{
			restore_or_terminate(oldSize);
			throw;
		}
	}
	LEAN_INLINE void reserve(size_t size) { v.reserve(size); this->Base::reserve(size); }

	LEAN_INLINE size_t size() const { return v.size(); }

	LEAN_INLINE Type& operator [](size_t idx) { return v[idx]; }
	LEAN_INLINE const Type& operator [](size_t idx) const { return v[idx]; }
};

template < class VectorBinder = vector_binder<std::vector, std::allocator> >
struct multi_vector_t
{
	template <class Type1, int ID1,
		class Type2 = void, int ID2 = -1,
		class Type3 = void, int ID3 = -1,
		class Type4 = void, int ID4 = -1,
		class Type5 = void, int ID5 = -1,
		class Type6 = void, int ID6 = -1,
		class Type7 = void, int ID7 = -1,
		class Type8 = void, int ID8 = -1,
		class Type9 = void, int ID9 = -1>
	struct make
	{
		typedef multi_vector<Type1, ID1, VectorBinder, typename multi_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7, Type8, ID8, Type9, ID9>::type> type;
	};

	template <class Type1, int ID1>
	struct make<Type1, ID1>
	{
		typedef multi_vector<Type1, ID1, VectorBinder> type;
	};
	template <class Type1, int ID1, class Type2, int ID2>
	struct make<Type1, ID1, Type2, ID2>
	{
		typedef multi_vector<Type1, ID1, VectorBinder, typename multi_vector_t::template make<Type2, ID2>::type> type;
	};
	template <class Type1, int ID1, class Type2, int ID2, class Type3, int ID3>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3>
	{
		typedef multi_vector<Type1, ID1, VectorBinder, typename multi_vector_t::template make<Type2, ID2, Type3, ID3>::type> type;
	};
	template <class Type1, int ID1, class Type2, int ID2, class Type3, int ID3, class Type4, int ID4>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4>
	{
		typedef multi_vector<Type1, ID1, VectorBinder, typename multi_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4>::type> type;
	};
	template <class Type1, int ID1, class Type2, int ID2, class Type3, int ID3, class Type4, int ID4, class Type5, int ID5>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5>
	{
		typedef multi_vector<Type1, ID1, VectorBinder, typename multi_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5>::type> type;
	};
	template <class Type1, int ID1, class Type2, int ID2, class Type3, int ID3, class Type4, int ID4, class Type5, int ID5, class Type6, int ID6>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6>
	{
		typedef multi_vector<Type1, ID1, VectorBinder, typename multi_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6>::type> type;
	};
	template <class Type1, int ID1, class Type2, int ID2, class Type3, int ID3, class Type4, int ID4, class Type5, int ID5, class Type6, int ID6, class Type7, int ID7>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7>
	{
		typedef multi_vector<Type1, ID1, VectorBinder, typename multi_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7>::type> type;
	};
	template <class Type1, int ID1, class Type2, int ID2, class Type3, int ID3, class Type4, int ID4, class Type5, int ID5, class Type6, int ID6, class Type7, int ID7, class Type8, int ID8>
	struct make<Type1, ID1, Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7, Type8, ID8>
	{
		typedef multi_vector<Type1, ID1, VectorBinder, typename multi_vector_t::template make<Type2, ID2, Type3, ID3, Type4, ID4, Type5, ID5, Type6, ID6, Type7, ID7, Type8, ID8>::type> type;
	};
};

} // namespace

using containers::multi_vector;
using containers::multi_vector_t;

} // namespace

#endif