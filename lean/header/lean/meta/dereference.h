/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_DEREFERENCE
#define LEAN_META_DEREFERENCE

namespace lean
{
namespace meta
{

/// Dereferences a given value type once, if the value type is a pointer type.
template <class Value>
struct maybe_dereference_once
{
	/// Value type after dereferencing.
	typedef Value value_type;
	/// True, if any dereferencing performed.
	static const bool dereferenced = false;

	/// Dereferences the given value parameter once, if the value is of a pointer type.
	static value_type& dereference(Value& value) { return value; }
	/// Dereferences the given value parameter once, if the value is of a pointer type.
	static value_type dereference(const Value &value) { return value; }
};

template <class Value>
struct maybe_dereference_once<Value*>
{
	typedef Value value_type;
	static const bool dereferenced = true;

	static value_type& dereference(Value* value) { return *value; }
};

template <>
struct maybe_dereference_once<void*>
{
	typedef void* value_type;
	static const bool dereferenced = false;

	static void*& dereference(void*& value) { return value; }
	static void* dereference(void* value) { return value; }
};

template <>
struct maybe_dereference_once<const void*>
{
	typedef const void* value_type;
	static const bool dereferenced = false;

	static const void*& dereference(const void*& value) { return value; }
	static const void* dereference(const void* value) { return value; }
};

/// Dereferences a given value type recusively, if the value type is a pointer type.
template <class Value>
struct dereference_recursively
{
	/// Value type after dereferencing recusively.
	typedef Value value_type;
	/// Number of times dereferencing was performed.
	static const int dereferenced = 0;

	/// Dereferences the given value parameter recusively, if the value is of a pointer type.
	static value_type& dereference(Value& value) { return value; }
	/// Dereferences the given value parameter recusively, if the value is of a pointer type.
	static value_type dereference(const Value& value) { return value; }
};

template <class Value>
struct dereference_recursively<Value*>
{
	typedef typename dereference_recursively<Value>::value_type value_type;
	static const int dereferenced = dereference_recursively<Value>::dereferenced + 1;

	static value_type& dereference(Value* value) { return dereference_recursively<Value>::dereference(*value); }
};

template <>
struct dereference_recursively<void*>
{
	typedef void* value_type;
	static const int dereferenced = 0;

	static void*& dereference(void*& value) { return value; }
	static void* dereference(void* value) { return value; }
};

template <>
struct dereference_recursively<const void*>
{
	typedef const void* value_type;
	static const int dereferenced = 0;

	static const void*& dereference(const void*& value) { return value; }
	static const void* dereference(const void* value) { return value; }
};

} // namespace

using meta::maybe_dereference_once;
using meta::dereference_recursively;

} // namespace

#endif