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

	/// Adds any reference stripped.
	template <class Other>
	struct undo
	{
		/// Type with reference, if any reference stripped.
		typedef Other type;
	};
};

#ifndef DOXYGEN_SKIP_THIS

template <class Type>
struct strip_reference<Type&>
{
	typedef Type type;
	static const bool stripped = true;
	template <class Other>
	struct undo { typedef Other& type; };
};

#ifndef LEAN0X_NO_RVALUE_REFERENCES
template <class Type>
struct strip_reference<Type&&>
{
	typedef Type type;
	static const bool stripped = true;
	template <class Other>
	struct undo { typedef Other&& type; };
};
#endif

#endif

/// Inherits the stripped type.
template <class Type>
struct inh_strip_reference : public strip_reference<Type>::type { };

/// Strips a const modifier from the given type.
template <class Type>
struct strip_const
{
	/// Type without const modifier.
	typedef Type type;
	/// True, if any modifiers stripped.
	static const bool stripped = false;

	/// Adds any modifiers stripped.
	template <class Other>
	struct undo
	{
		/// Type with modifier, if any modifier stripped.
		typedef Other type;
	};
};

#ifndef DOXYGEN_SKIP_THIS

template <class Type>
struct strip_const<const Type>
{
	typedef Type type;
	static const bool stripped = true;
	template <class Other>
	struct undo { typedef const Other type; };
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

	/// Adds any modifiers stripped.
	template <class Other>
	struct undo
	{
		/// Type with modifier, if any modifier stripped.
		typedef Other type;
	};
};

#ifndef DOXYGEN_SKIP_THIS

template <class Type>
struct strip_volatile<volatile Type>
{
	typedef Type type;
	static const bool stripped = true;
	template <class Other>
	struct undo { typedef volatile Other type; };
};

#endif

/// Strips cv-modifiers from the given type.
template <class Type>
struct strip_modifiers
{
	/// Type without cv-modifiers.
	typedef typename strip_volatile<typename strip_const<Type>::type>::type type;
	/// True, if any modifiers stripped.
	static const bool stripped = strip_volatile<Type>::stripped || strip_const<Type>::stripped;

	/// Adds any modifiers stripped.
	template <class Other>
	struct undo
	{
		/// Type with modifiers, if any modifiers stripped.
		typedef typename strip_const<Type>::template undo<typename strip_volatile<Type>::template undo<Other>::type>::type type;
	};
};

/// Strips cv-modifiers and references from the given type.
template <class Type>
struct strip_modref
{
	/// Value type.
	typedef typename strip_reference<Type>::type value_type;
	/// Type without cv-modifiers and references.
	typedef typename strip_modifiers<value_type>::type type;
	/// True, if any modifiers or references stripped.
	static const bool stripped = strip_reference<Type>::stripped || strip_modifiers<value_type>::stripped;

	/// Adds any modifiers and references stripped.
	template <class Other>
	struct undo
	{
		/// Type with modifiers and references, if any modifiers or references stripped.
		typedef typename strip_reference<Type>::template undo<typename strip_modifiers<value_type>::template undo<Other>::type>::type type;
	};
};

/// Inherits the stripped type.
template <class Type>
struct inh_strip_modref : public strip_modref<Type>::type { };

namespace impl
{

template <class Type>
struct do_strip_pointer
{
	typedef Type type;
	static const bool stripped = false;
	template <class Other>
	struct undo { typedef Other type; };
};

template <class Type>
struct do_strip_pointer<Type*>
{
	typedef Type type;
	static const bool stripped = true;
	template <class Other>
	struct undo { typedef Other* type; };
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

	/// Adds any pointer stripped.
	template <class Other>
	struct undo
	{
		/// Pointer type without modifiers.
		typedef typename impl::do_strip_pointer<pointer>::template undo<Other>::type pointer;
		/// Type with pointer, if any pointer stripped.
		typedef typename strip_modifiers<Type>::template undo<pointer>::type type;
	};
};

/// Inherits the stripped type.
template <class Type>
struct inh_strip_pointer : public strip_pointer<Type>::type { };

/// Redefines the given type.
template <class Type>
struct identity
{
	/// Type.
	typedef Type type;
};

} // namespace

using meta::strip_pointer;
using meta::strip_reference;
using meta::strip_const;
using meta::strip_volatile;
using meta::strip_modifiers;
using meta::strip_modref;
using meta::identity;

using meta::inh_strip_pointer;
using meta::inh_strip_reference;
using meta::inh_strip_modref;

} // namespace

#endif