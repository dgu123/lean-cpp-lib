/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_DEPENDENT_FALSE
#define LEAN_META_DEPENDENT_FALSE

namespace lean
{
namespace meta
{

/// Defines a false literal, ignoring any template arguments.
template <class T>
struct dependent_false
{
	/// Always false.
	static const bool value = false;
};

/// Defines a false literal, ignoring any template arguments.
template <int N>
struct int_dependent_false
{
	/// Always false.
	static const bool value = false;
};

} // namespace

using meta::dependent_false;

} // namespace

#endif