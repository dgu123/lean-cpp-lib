/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_CONDITIONAL_TYPE
#define LEAN_META_CONDITIONAL_TYPE

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

/// Redefines TrueType if condition fulfilled, FalseType otherwise.
template <class TrueType, class FalseType>
struct conditional_type<true, TrueType, FalseType>
{
	/// TrueType if condition fulfilled, FalseType otherwise.
	typedef TrueType type;
};

} // namespace

using meta::conditional_type;

} // namespace

#endif