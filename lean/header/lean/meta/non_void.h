/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_NON_VOID
#define LEAN_META_NON_VOID

namespace lean
{
namespace meta
{

/// Redefines Type1 if not void, Type2 otherwise.
template <class Type1, class Type2>
struct non_void
{
	/// Type1 if not void, Type2 otherwise.
	typedef Type1 type;
};

#ifndef DOXYGEN_SKIP_THIS

template <class Type2>
struct non_void<void, Type2>
{
	typedef Type2 type;
};

template <>
struct non_void<void, void>;

#endif

} // namespace

using meta::non_void;

} // namespace

#endif