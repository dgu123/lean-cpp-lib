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

/// Checks if the given integer type is unsigned.
template <class Integer>
struct is_unsigned
{
	static const Integer min = Integer(0);
	static const Integer max = Integer(-1);

	/// True, if @code Integer@endcode is unsigned, false otherwise.
	static const bool value = (max > min);
};

/// True if Type is derived from Base, false otherwise.
template <class Type, class Base>
struct is_derived
{
private:
	typedef char yes[1];
	typedef char no[2];

	static yes& sfinae_check(Base*);
	static no& sfinae_check(void*);

public:
	/// Specifies whether Type is derived from Base.
	static const bool value = (
		sizeof( is_derived::sfinae_check( static_cast<Type*>(nullptr) ) )
		==
		sizeof(yes) );
};

/// True if Type is an empty class.
template <class Type>
struct is_empty
{
private:
	// MONITOR: Make use of empty base class optimization
	template <class T>
	struct check : private T { int i; };

public:
	/// Specifies whether Type is empty.
	static const bool value = (sizeof(check<Type>) == sizeof(int));
};

} // namespace

using meta::is_equal;
using meta::is_unsigned;
using meta::is_derived;
using meta::is_empty;

/// True if Type defines the given type, false otherwise.
#define LEAN_DEFINE_HAS_TYPE(TypeName)													\
	template <class Type>																\
	class has_type_##TypeName															\
	{																					\
	private:																			\
		typedef char yes[1];															\
		typedef char no[2];																\
																						\
		template <class T>																\
		static yes& sfinae_check(T*, typename T::TypeName* = nullptr);					\
		static no& sfinae_check(...);													\
																						\
	public:																				\
		static const bool value =														\
			sizeof( has_type_##TypeName::sfinae_check( static_cast<Type*>(nullptr) ) )	\
			==																			\
			sizeof(yes);																\
	};

} // namespace

#ifndef _MSC_VER
#include <type_traits>
#endif

namespace lean
{
namespace meta
{

#ifdef _MSC_VER

/// Checks if the given type is trivial.
template <class Type>
struct is_trivial
{
	/// True, if the given type is trivial.
	static const bool value = __has_trivial_constructor(Type) && __has_trivial_copy(Type) && __has_trivial_assign(Type) && __has_trivial_destructor(Type) || __is_pod(Type);
};

/// Checks if the given type is trivially copyable.
template <class Type>
struct is_trivially_copyable
{
	/// True, if the given type is trivial.
	static const bool value = __has_trivial_copy(Type) && __has_trivial_assign(Type) && __has_trivial_destructor(Type) || __is_pod(Type);
};

/// Checks if the given type is trivially destructible.
template <class Type>
struct is_trivially_destructible
{
	/// True, if the given type is trivial.
	static const bool value = __has_trivial_destructor(Type) || __is_pod(Type);
};

#else

using std::is_trivial;
using std::is_trivially_copyable;
using std::is_trivially_destructible;

#endif

} // namespace

using meta::is_trivial;
using meta::is_trivially_copyable;
using meta::is_trivially_destructible;

} // namespace

#endif