/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_ENABLE_IF
#define LEAN_META_ENABLE_IF

namespace lean
{
namespace meta
{

/// Redefines the given type if true, empty otherwise.
template <bool Cond, class Type>
struct enable_if
{
	/// Redefines Type if condition true, undefined otherwise.
	typedef Type type;
};

#ifndef DOXYGEN_SKIP_THIS

template <class Type>
struct enable_if<false, Type> { };

#endif

} // namespace

using meta::enable_if;

} // namespace

#endif