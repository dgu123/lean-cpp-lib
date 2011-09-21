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
using meta::is_unsigned;
using meta::is_derived;

/// True if Type defines the given type, false otherwise.
#define LEAN_DEFINE_HAS_TYPE(TypeName)					\
	template <class Type>								\
	class has_type_##TypeName							\
	{													\
	private:											\
		typedef char yes[1];							\
		typedef char no[2];								\
														\
		template <class T>								\
		static yes& check(const T*,						\
			const typename T::TypeName* = nullptr);		\
		static no& check(...);							\
														\
	public:												\
		static const bool value =						\
			sizeof(check(static_cast<Type*>(nullptr)))	\
			==											\
			sizeof (yes);								\
	};

} // namespace

#endif