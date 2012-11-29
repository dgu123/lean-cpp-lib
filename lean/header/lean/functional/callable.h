/*****************************************************/
/* lean Functional              (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_FUNCTIONAL_CALLABLE
#define LEAN_FUNCTIONAL_CALLABLE

#include "../lean.h"
#include "../tags/noncopyable.h"

namespace lean
{
namespace functional
{

/// Stores the pointer to a function to be called on invokation of operator ().
template <class Signature>
class callable_fun
{
private:
	Signature *m_fun;

public:
	/// Stores the given function to be called by operator().
	LEAN_INLINE callable_fun(Signature *fun)
		: m_fun(fun)
	{
		LEAN_ASSERT(m_fun);
	}
	/// Calls the function stored by this callable object.
	LEAN_INLINE void operator ()()
	{
		(*m_fun)();
	}
};

/// Stores an object and a pointer to a method to be called on invokation of operator ().
template <class Class, class Signature>
class callable_objfun
{
private:
	Class *m_obj;
	Signature *m_fun;

public:
	/// Stores the given object and method to be called by operator().
	LEAN_INLINE callable_objfun(Class *obj, Signature *fun)
		: m_obj(obj),
		m_fun(fun)
	{
		LEAN_ASSERT(m_obj);
		LEAN_ASSERT(m_fun);
	}
	/// Calls the function stored by this callable object.
	LEAN_INLINE void operator ()()
	{
		(*m_fun)(*m_obj);
	}
};

/// Stores an object and a pointer to a method to be called on invokation of operator ().
template <class Class, class Signature>
class callable_memfun
{
private:
	Class *m_obj;
	Signature Class::*m_fun;

public:
	/// Stores the given object and method to be called by operator().
	LEAN_INLINE callable_memfun(Class *obj, Signature Class::*fun)
		: m_obj(obj),
		m_fun(fun)
	{
		LEAN_ASSERT(m_obj);
		LEAN_ASSERT(m_fun);
	}
	/// Calls the function stored by this callable object.
	LEAN_INLINE void operator ()()
	{
		(m_obj->*m_fun)();
	}
};

/// Constructs a callable object from the given function pointer.
template <class Signature>
LEAN_INLINE callable_fun<Signature> make_callable(Signature *fun)
{
	return callable_fun<Signature>(fun);
}

/// Constructs a callable object from the given object and method pointer.
template <class Class, class Signature>
LEAN_INLINE callable_objfun<Class, Signature> make_callable(Class *obj, Signature *fun)
{
	return callable_objfun<Class, Signature>(obj, fun);
}

/// Constructs a callable object from the given object and method pointer.
template <class Class, class Signature>
LEAN_INLINE callable_memfun<Class, Signature> make_callable(Class *obj, Signature Class::*fun)
{
	return callable_memfun<Class, Signature>(obj, fun);
}

/// Stores a pointer to a polymorphic method to be called on invokation of operator ().
class vcallable
{
public:
	/// Polymorphic function pointer type.
	typedef void (*fun_ptr)(vcallable&);
	
private:
	fun_ptr m_fun;

protected:
	/// Stores the given polymorphic method to be called by operator().
	LEAN_INLINE vcallable(fun_ptr fun) noexcept
		: m_fun(fun)
	{
		LEAN_ASSERT(m_fun);
	}
	
	LEAN_INLINE vcallable(const vcallable &right) noexcept
		: m_fun(right.m_fun) { }
	
	LEAN_INLINE vcallable& operator =(const vcallable &right) noexcept
	{
		m_fun = right.m_fun;
		return *this;
	}

public:
	/// Calls the function stored by this callable object.
	LEAN_INLINE void operator ()()
	{
		(*m_fun)(*this);
	}
};

/// Stores a pointer to a method to be called on invokation of operator ().
template <class Derived>
class vcallable_base : public vcallable
{
private:
	static void call(vcallable &self)
	{
		static_cast<Derived&>(self)();
	}

protected:
	/// Stores the given object and method to be called by operator().
	LEAN_INLINE vcallable_base() noexcept
		: vcallable(&call) { }

	LEAN_INLINE vcallable_base(const vcallable_base &right) noexcept
		: vcallable(right) { }
	
	LEAN_INLINE vcallable_base& operator =(const vcallable_base &right) noexcept
	{
		vcallable::operator =(right);
		return *this;
	}
};

} // namespace

using functional::callable_fun;
using functional::callable_memfun;

using functional::make_callable;

using functional::vcallable;
using functional::vcallable_base;

} // namespace

#endif