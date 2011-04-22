/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_STRIP
#define LEAN_META_STRIP

#include "../cpp0x.h"

namespace lean
{
namespace meta
{

/// Strips a reference from the given type.
template <class Type>
struct strip_reference
{
	/// Type without reference.
	typedef Type type;
	/// True, if any reference stripped.
	static const bool stripped = false;
};

#ifndef DOXYGEN_SKIP_THIS

template <class Type>
struct strip_reference<Type&>
{
	typedef Type type;
	static const bool stripped = true;
};

#ifndef LEAN0X_NO_RVALUE_REFERENCES
template <class Type>
struct strip_reference<Type&&>
{
	typedef Type type;
	static const bool stripped = true;
};
#endif

#endif

/// Strips a const modifier from the given type.
template <class Type>
struct strip_const
{
	/// Type without const modifier.
	typedef Type type;
	/// True, if any modifiers stripped.
	static const bool stripped = false;
};

#ifndef DOXYGEN_SKIP_THIS

template <class Type>
struct strip_const<const Type>
{
	typedef Type type;
	static const bool stripped = true;
};

#endif

/// Strips a volatile modifier from the given type.
template <class Type>
struct strip_volatile
{
	/// Type without volatile modifier.
	typedef Type type;
	/// True, if any modifiers stripped.
	static const bool stripped = false;
};

#ifndef DOXYGEN_SKIP_THIS

template <class Type>
struct strip_volatile<volatile Type>
{
	typedef Type type;
	static const bool stripped = true;
};

#endif

/// Strips a volatile modifier from the given type.
template <class Type>
struct strip_modifiers
{
	/// Type without volatile modifier.
	typedef typename strip_volatile<typename strip_const<Type>::type>::type type;
	/// True, if any modifiers stripped.
	static const bool stripped = strip_volatile<Type>::stripped || strip_const<Type>::stripped;
};

namespace impl
{

template <class Type>
struct do_strip_pointer
{
	typedef Type type;
	static const bool stripped = false;
};

template <class Type>
struct do_strip_pointer<Type*>
{
	typedef Type type;
	static const bool stripped = true;
};

}

/// Strips a pointer from the given type.
template <class Type>
struct strip_pointer
{
	/// Pointer type without modifiers.
	typedef typename strip_modifiers<Type>::type pointer;
	/// Type without pointer.
	typedef typename impl::do_strip_pointer<pointer>::type type;
	/// True, if any pointer stripped.
	static const bool stripped = impl::do_strip_pointer<pointer>::stripped;
};

} // namespace

using meta::strip_pointer;
using meta::strip_reference;
using meta::strip_const;
using meta::strip_volatile;
using meta::strip_modifiers;

} // namespace

#endif