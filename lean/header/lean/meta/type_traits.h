/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_META_TYPE_TRAITS
#define LEAN_META_TYPE_TRAITS

#include "strip.h"

namespace lean
{
namespace meta
{

#ifdef _MSC_VER

/// Checks if the given type is trivial.
template <class Type>
struct is_trivial
{
	/// True, if the given type is trivial.
	static const bool value = __has_trivial_constructor(Type) && __has_trivial_copy(Type) && __has_trivial_assign(Type) && __has_trivial_destructor(Type) || __is_pod(Type);
};

/// Checks if the given type is trivially copyable.
template <class Type>
struct is_trivially_copyable
{
	/// True, if the given type is trivial.
	static const bool value = __has_trivial_copy(Type) && __has_trivial_assign(Type) && __has_trivial_destructor(Type) || __is_pod(Type);
};

/// Checks if the given type is trivially destructible.
template <class Type>
struct is_trivially_destructible
{
	/// True, if the given type is trivial.
	static const bool value = __has_trivial_destructor(Type) || __is_pod(Type);
};

#else

using std::is_trivial;
using std::is_trivially_copyable;
using std::is_trivially_destructible;

#endif

} // namespace

using meta::is_trivial;
using meta::is_trivially_copyable;
using meta::is_trivially_destructible;

} // namespace

#endif