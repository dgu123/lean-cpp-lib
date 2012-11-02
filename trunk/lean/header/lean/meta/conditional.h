/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_META_CONDITIONAL
#define LEAN_META_CONDITIONAL

namespace lean
{
namespace meta
{

/// Redefines TrueType if condition fulfilled, FalseType otherwise.
template <bool Condition, class TrueType, class FalseType>
struct conditional_type
{
	/// TrueType if condition fulfilled, FalseType otherwise.
	typedef FalseType type;
};

#ifndef DOXYGEN_SKIP_THIS

template <class TrueType, class FalseType>
struct conditional_type<true, TrueType, FalseType>
{
	typedef TrueType type;
};

#endif

/// Redefines Type1 if not void, else Type2 if not void, nothing otherwise.
template <class Type1, class Type2>
struct first_non_void
{
	/// Redefines Type1 or Type2 if not void, undefined otherwise.
	typedef Type1 type;
};

#ifndef DOXYGEN_SKIP_THIS

template <class Type2>
struct first_non_void<void, Type2>
{
	typedef Type2 type;
};

template <>
struct first_non_void<void, void>
{
};

#endif

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

} // namespace

using meta::conditional_type;
using meta::first_non_void;
using meta::complete_type_or_base;

} // namespace

#endif