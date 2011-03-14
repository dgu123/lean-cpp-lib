/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_DEREFERENCE
#define LEAN_META_DEREFERENCE

#include "strip.h"
#include "conditional_type.h"

namespace lean
{
namespace meta
{

namespace impl
{

template <class Value>
struct maybe_dereference_once
{
private:
	typedef typename strip_const<typename strip_reference<Value>::type>::type stripped_value_type;

public:
	static const bool dereferenced = false;
	
	typedef Value value_type;

	typedef stripped_value_type& parameter_type;
	typedef value_type& return_type;
	static return_type dereference(parameter_type value) { return value; }

	typedef const stripped_value_type& const_parameter_type;
	typedef const value_type& const_return_type;
	static const_return_type dereference(const_parameter_type value) { return value; }
};

template <class Value>
struct maybe_dereference_once<Value*>
{
private:
	struct disable_type { };
public:
	static const bool dereferenced = true;

	typedef Value value_type;
	
	typedef disable_type& parameter_type;
	typedef disable_type& return_type;
	static return_type dereference(parameter_type parameter) { return parameter; }

	typedef Value* const_parameter_type;
	typedef value_type& const_return_type;
	static const_return_type dereference(const_parameter_type pointer) { return *pointer; }
};

template <>
struct maybe_dereference_once<void*>
{
	static const bool dereferenced = false;

	typedef void* value_type;
	
	typedef value_type& parameter_type;
	typedef value_type& return_type;
	static return_type dereference(parameter_type value) { return value; }

	typedef const value_type& const_parameter_type;
	typedef const value_type& const_return_type;
	static const_return_type dereference(const_parameter_type value) { return value; }
};

template <>
struct maybe_dereference_once<const void*>
{
	static const bool dereferenced = false;

	typedef const void* value_type;
	
	typedef value_type& parameter_type;
	typedef value_type& return_type;
	static return_type dereference(parameter_type value) { return value; }

	typedef const value_type& const_parameter_type;
	typedef const value_type& const_return_type;
	static const_return_type dereference(const_parameter_type value) { return value; }
};

}

/// Dereferences a given value type once, if the value type is a pointer type.
template <class Type>
struct maybe_dereference_once
{
private:
	typedef typename strip_modifiers<typename strip_reference<Type>::type>::type stripped_type;
	
	static const bool dereferenced_internal = impl::maybe_dereference_once<stripped_type>::dereferenced;

	typedef typename conditional_type< dereferenced_internal,
		impl::maybe_dereference_once<stripped_type>,
		impl::maybe_dereference_once<Type> >::type internal_dereferencer;
public:
	/// True, if any dereferencing performed.
	static const bool dereferenced = dereferenced_internal;

	/// Value type after dereferencing.
	typedef typename internal_dereferencer::value_type value_type;

	/// Dereferences the given value parameter once, if the value is of a pointer type.
	static typename internal_dereferencer::return_type dereference(typename internal_dereferencer::parameter_type value)
	{
		return internal_dereferencer::dereference(value);
	}
	/// Dereferences the given value parameter once, if the value is of a pointer type.
	static typename internal_dereferencer::const_return_type dereference(typename internal_dereferencer::const_parameter_type value)
	{
		return internal_dereferencer::dereference(value);
	}
};

} // namespace

using meta::maybe_dereference_once;

} // namespace

#endif