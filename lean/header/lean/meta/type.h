/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_META_TYPE
#define LEAN_META_TYPE

namespace lean
{
namespace meta
{

/// Empty base class.
class empty_base
{
protected:
#ifdef LEAN0X_NO_DELETE_METHODS
	LEAN_INLINE empty_base() noexcept { }
	LEAN_INLINE empty_base(const empty_base&) noexcept { }
	LEAN_INLINE empty_base& operator =(const empty_base&) noexcept { return *this; }
#ifndef LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	LEAN_INLINE ~empty_base() noexcept { }
#endif
#else
	empty_base() noexcept = default;
	empty_base(const empty_base&) noexcept = default;
	empty_base& operator =(const empty_base&) noexcept = default;
	~empty_base() noexcept = default;
#endif
};

/// Redefines FullType if complete (and derived from BaseType), BaseType otherwise.
template <class FullType, class BaseType>
struct complete_type_or_base
{
private:
	typedef char complete[1];
	typedef char incomplete[2];

	static complete& check_type(const BaseType*);
	static incomplete& check_type(const void*);

public:
	/// Specifies whether FullType is complete and was derived from BaseType.
	static const bool is_complete = (
		sizeof( check_type( static_cast<FullType*>(nullptr) ) )
		==
		sizeof(complete) );
	
	/// Full type if complete (and derived from BaseType), BaseType otherwise.
	typedef typename conditional_type<is_complete, FullType, BaseType>::type type;
};

/// Absorbs the given values.
LEAN_MAYBE_EXPORT void absorb(...);
/// Cast functions to this type.
typedef void (*absorbfun)();

} // namespace
using meta::empty_base;
using meta::complete_type_or_base;

using meta::absorb;
using meta::absorbfun;

} // namespace

#ifdef LEAN_INCLUDE_LINKED
#include "source/support.cpp"
#endif

#endif