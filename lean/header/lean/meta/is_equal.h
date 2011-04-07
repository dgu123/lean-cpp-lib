/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_IS_EQUAL
#define LEAN_META_IS_EQUAL

namespace lean
{
namespace meta
{

/// True if types are equal, false otherwise.
template <class Type1, class Type2>
struct is_equal
{
	/// True if types are equal, false otherwise.
	static const bool value = false;
};

template <class Type>
struct is_equal<Type, Type>
{
	static const bool value = true;
};

} // namespace

using meta::is_equal;

} // namespace

#endif