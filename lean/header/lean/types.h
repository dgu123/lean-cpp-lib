/*****************************************************/
/* lean built-in types          (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_TYPES
#define LEAN_TYPES

#include <cstddef>

namespace lean
{

/// Defines fixed-width and other standard types.
namespace types
{

/// Sign classes enumeration.
namespace sign_class
{
	/// Sign classes enumeration.
	enum t
	{
		no_sign,	///< Unsigned class.
		sign		///< Signed class.
	};
}

/// Provides an integer type of the given class and size.
template <sign_class::t Class, size_t Size>
struct int_type
{
	// Always checked, therefore use static_assert with care
	LEAN_STATIC_ASSERT_MSG_ALT(Size & ~Size, // = false, dependent
		"No integer type of the given size available.",
		No_integer_type_of_the_given_size_available);

	/// Integer type.
	typedef void type;
};

#ifndef DOXYGEN_SKIP_THIS

// Defaults that should work with most compilers
template<> struct int_type<sign_class::sign, sizeof(char)> { typedef char type; };
template<> struct int_type<sign_class::sign, sizeof(short)> { typedef short type; };
template<> struct int_type<sign_class::sign, sizeof(int)> { typedef int type; };
template<> struct int_type<sign_class::sign, sizeof(long long)> { typedef long long type; };

template<> struct int_type<sign_class::no_sign, sizeof(unsigned char)> { typedef unsigned char type; };
template<> struct int_type<sign_class::no_sign, sizeof(unsigned short)> { typedef unsigned short type; };
template<> struct int_type<sign_class::no_sign, sizeof(unsigned int)> { typedef unsigned int type; };
template<> struct int_type<sign_class::no_sign, sizeof(unsigned long long)> { typedef unsigned long long type; };

#endif

// Count bytes rather than bits (number of bits per char undefined)

/// 1 byte unsigned integer.
typedef int_type<sign_class::sign, 1>::type int1;
/// 2 byte unsigned integer.
typedef int_type<sign_class::sign, 2>::type int2;
/// 4 byte unsigned integer.
typedef int_type<sign_class::sign, 4>::type int4;
/// 8 byte unsigned integer.
typedef int_type<sign_class::sign, 8>::type int8;

/// 1 byte unsigned integer.
typedef int_type<sign_class::no_sign, 1>::type uint1;
/// 2 byte unsigned integer.
typedef int_type<sign_class::no_sign, 2>::type uint2;
/// 4 byte unsigned integer.
typedef int_type<sign_class::no_sign, 4>::type uint4;
/// 8 byte unsigned integer.
typedef int_type<sign_class::no_sign, 8>::type uint8;

/// Provides a floating-point type of the given size.
template <size_t Size>
struct float_type
{
	// Always checked, therefore use static_assert with care
	LEAN_STATIC_ASSERT_MSG_ALT(Size & ~Size, // = false, dependent
		"No floating-point type of the given size available.",
		No_floating_point_type_of_the_given_size_available);

	/// Floating-point type.
	typedef void type;
};

#ifndef DOXYGEN_SKIP_THIS

// Defaults that should work with most compilers
template<> struct float_type<sizeof(float)> { typedef float type; };
template<> struct float_type<sizeof(double)> { typedef double type; };

#endif

/// 4 byte float.
typedef float_type<4>::type float4;
/// 8 byte float.
typedef float_type<8>::type float8;

/// Std size type.
typedef std::size_t size_t;
/// Std pointer difference type.
typedef std::ptrdiff_t ptrdiff_t;
/// Std pointer address type.
typedef int_type<sign_class::no_sign, sizeof(void*)>::type uintptr_t;

/// Provides a character type of the given size.
template <size_t Size>
struct char_type
{
	/// Character type.
	typedef typename int_type<sign_class::sign, Size>::type type;
};

#ifndef DOXYGEN_SKIP_THIS

// Defaults that should work with most compilers
template<> struct char_type<sizeof(char)> { typedef char type; };
template<> struct char_type<sizeof(wchar_t)> { typedef wchar_t type; };

#endif

/// Character type.
typedef char_type<1>::type char1;
/// Character type.
typedef char_type<2>::type char2;
/// Character type.
typedef char_type<4>::type char4;

/// Character type.
typedef char1 utf8_t;
/// Character type.
typedef char2 utf16_t;
/// Character type.
typedef char4 utf32_t;

}

namespace sign_class = types::sign_class;
using types::int_type;
using types::float_type;
using types::char_type;

using types::int1;
using types::int2;
using types::int4;
using types::int8;

using types::uint1;
using types::uint2;
using types::uint4;
using types::uint8;

using types::float4;
using types::float8;

using types::size_t;
using types::ptrdiff_t;
using types::uintptr_t;

using types::char1;
using types::char2;
using types::char4;

using types::utf8_t;
using types::utf16_t;
using types::utf32_t;

}

#endif