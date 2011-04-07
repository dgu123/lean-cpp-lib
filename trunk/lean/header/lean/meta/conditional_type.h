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

#ifndef DOXYGEN_SKIP_THIS

template <class TrueType, class FalseType>
struct conditional_type<true, TrueType, FalseType>
{
	typedef TrueType type;
};

#endif

} // namespace

using meta::conditional_type;

} // namespace

#endif