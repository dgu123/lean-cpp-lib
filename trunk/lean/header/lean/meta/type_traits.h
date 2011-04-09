/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_TYPE_TRAITS
#define LEAN_META_TYPE_TRAITS

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

#ifndef DOXYGEN_SKIP_THIS

template <class Type>
struct is_equal<Type, Type>
{
	static const bool value = true;
};

#endif

/// True if Type is derived from Base, false otherwise.
template <class Type, class Base>
struct is_derived
{
private:
	typedef char yes[1];
	typedef char no[2];

	static yes& check_type(const Base*);
	static no& check_type(const void*);

public:
	/// Specifies whether Type is derived from Base.
	static const bool value = (
		sizeof( check_type( static_cast<Type*>(nullptr) ) )
		==
		sizeof(yes) );
};

} // namespace

using meta::is_equal;
using meta::is_derived;

} // namespace

#endif