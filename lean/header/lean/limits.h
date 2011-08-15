/*****************************************************/
/* lean built-in types          (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LIMITS
#define LEAN_LIMITS

#include <cstddef>
#include <limits>
#include <cfloat>
#include "cpp0x.h"

#ifdef DOXYGEN_READ_THIS
	/// Define this to enable distinct size_t specializations.
	/// @ingroup GlobalSwitches
	#define LEAN_BUILTIN_SIZE_T
	#undef LEAN_BUILTIN_SIZE_T
#endif

namespace lean
{

namespace types
{

namespace
{

/// Provides literal numeric limits information.
template <class Type>
struct numeric_limits
{
	// Always checked, therefore use static_assert with care
	LEAN_STATIC_ASSERT_MSG_ALT(!sizeof(Type), // = false, dependent
		"No numeric limits available for the given type.",
		No_numeric_limits_available_for_the_given_type);
	
	/// Specifies whether the type is an integer type.
	static const bool is_int;
	/// Specifies whether the type is an floating-point type.
	static const bool is_float;
	/// Specifies whether the type is unsigned.
	static const bool is_unsigned;
	/// Smallest value.
	static const Type min;
	/// Greatest value.
	static const Type max;
	/// Specifies whether infinity is available.
	static const bool has_infinity;
	/// Infinity.
	static const Type infinity;
};

namespace impl
{
	template <class Integer>
	struct int_limits_base
	{
		static const bool is_int = true;
		static const bool is_float = false;
		static const bool has_infinity = false;
		static const Integer infinity = static_cast<Integer>(0);
	};

	template <class SInteger>
	struct sint_limits_base : public int_limits_base<SInteger>
	{
		static const bool is_unsigned = false;
	};

	template <class UInteger>
	struct uint_limits_base : public int_limits_base<UInteger>
	{
		static const bool is_unsigned = true;
		static const UInteger min = static_cast<UInteger>(0);
	};

	struct float_limits_base
	{
		static const bool is_int = false;
		static const bool is_float = true;
		static const bool is_unsigned = false;
	};
}

template <>
struct numeric_limits<char> : public impl::sint_limits_base<char>
{
	static const char min = CHAR_MIN;
	static const char max = CHAR_MAX;
};
template <>
struct numeric_limits<unsigned char> : public impl::uint_limits_base<unsigned char>
{
	static const unsigned char max = UCHAR_MAX;
};

template <>
struct numeric_limits<short> : public impl::sint_limits_base<short>
{
	static const short min = SHRT_MIN;
	static const short max = SHRT_MAX;
};
template <>
struct numeric_limits<unsigned short> : public impl::uint_limits_base<unsigned short>
{
	static const unsigned short max = USHRT_MAX;
};

template <>
struct numeric_limits<int> : public impl::sint_limits_base<int>
{
	static const int min = INT_MIN;
	static const int max = INT_MAX;
};
template <>
struct numeric_limits<unsigned int> : public impl::uint_limits_base<unsigned int>
{
	static const unsigned int max = UINT_MAX;
};

template <>
struct numeric_limits<long> : public impl::sint_limits_base<long>
{
	static const long min = LONG_MIN;
	static const long max = LONG_MAX;
};
template <>
struct numeric_limits<unsigned long> : public impl::uint_limits_base<unsigned long>
{
	static const unsigned long max = ULONG_MAX;
};

template <>
struct numeric_limits<long long> : public impl::sint_limits_base<long long>
{
	static const long long min = LLONG_MIN;
	static const long long max = LLONG_MAX;
};
template <>
struct numeric_limits<unsigned long long> : public impl::uint_limits_base<unsigned long long>
{
	static const unsigned long long max = ULLONG_MAX;
};

#ifdef LEAN_BUILTIN_SIZE_T
template <>
struct numeric_limits<size_t> : public impl::uint_limits_base<size_t>
{
	static const size_t max = SIZE_MAX;
};
#endif

template <>
struct numeric_limits<float> : public impl::float_limits_base
{
	static const float min;
	static const float max;
	static const float has_infinity;
	static const float infinity;
};
const float numeric_limits<float>::min = FLT_MIN;
const float numeric_limits<float>::max = FLT_MAX;
const float numeric_limits<float>::has_infinity = std::numeric_limits<float>::has_infinity;
const float numeric_limits<float>::infinity = std::numeric_limits<float>::infinity();

template <>
struct numeric_limits<double> : public impl::float_limits_base
{
	static const double min;
	static const double max;
	static const double has_infinity;
	static const double infinity;
};
const double numeric_limits<double>::min = DBL_MIN;
const double numeric_limits<double>::max = DBL_MAX;
const double numeric_limits<double>::has_infinity = std::numeric_limits<double>::has_infinity;
const double numeric_limits<double>::infinity = std::numeric_limits<double>::infinity();

template <>
struct numeric_limits<long double> : public impl::float_limits_base
{
	static const long double min;
	static const long double max;
	static const long double has_infinity;
	static const long double infinity;
};
const long double numeric_limits<long double>::min = LDBL_MIN;
const long double numeric_limits<long double>::max = LDBL_MAX;
const long double numeric_limits<long double>::has_infinity = std::numeric_limits<long double>::has_infinity;
const long double numeric_limits<long double>::infinity = std::numeric_limits<long double>::infinity();

}

}

using types::numeric_limits;

}

#endif