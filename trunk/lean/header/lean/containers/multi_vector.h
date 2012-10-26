/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

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
	LEAN_INLINE multi_vector_base(T1&&) { }
	template <class T1, class T2>
	LEAN_INLINE multi_vector_base(T1&&, T2&&) { }

	LEAN_INLINE void push_back() { }
	LEAN_INLINE void erase(size_t idx) { }
	LEAN_INLINE void clear() { }
	LEAN_INLINE void resize(size_t size) { }
	LEAN_INLINE void reserve(size_t size) { }
};

template < class Type, int ID, class Base = multi_vector_base, class Alloc = std::allocator<Type> >
class multi_vector : private Base
{
	template <class OtherType, int OtherID, class OtherBase, class OtherAlloc>
	friend class multi_vector;

public:
	typedef std::vector<Type, Alloc> vector_type;

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
	LEAN_INLINE multi_vector(const Alloc &allocator)
		: Base(allocator),
		v(allocator) { }
	LEAN_INLINE multi_vector(size_t size, const Alloc &allocator = Alloc())
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

} // namespace

using containers::multi_vector;

} // namespace

#endif