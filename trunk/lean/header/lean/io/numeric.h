/*****************************************************/
/* lean IO                      (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_IO_NUMERIC
#define LEAN_IO_NUMERIC

#include "../lean.h"
#include "../meta/strip.h"
#include <clocale>
#include <cstdio>
#include "../strings/types.h"

namespace lean
{
namespace io
{

/// Converts the given integer of the given type into an ascii character string, returning a pointer to the first character not written to.
/// Does not append a terminating null character.
template <class CharIter, class Integer>
inline CharIter int_to_char(CharIter buffer, Integer num)
{
	typedef typename lean::strip_modifiers<typename lean::strip_reference<Integer>::type>::type int_type;
	typedef typename lean::int_type<lean::sign_class::sign, sizeof(int_type)>::type sint_type;
	typedef typename lean::int_type<lean::sign_class::no_sign, sizeof(int_type)>::type uint_type;

	uint_type unum = static_cast<uint_type>(num);

	// Check, if signed
	if (static_cast<int_type>(-1) < static_cast<int_type>(0))
	{
		// Check sign
		if (num < static_cast<int_type>(0))
		{
			unum = static_cast<uint_type>(-static_cast<sint_type>(num));
			*(buffer++) = '-';
		}
	}

	CharIter first = buffer;

	// Convert backwards
	do
	{
		*(buffer++) = '0' + static_cast<char>(unum % 10U);
		unum = unum / 10U;
	}
	while (unum > 0);

	CharIter last = buffer;

	// Swap character order
	do
	{
		char tmp = *(--last);
		*last = *first;
		*(first++) = tmp;
	}
	while (first < last);

	// Return end
	return buffer;
}

/// Estimates the maximum string length for integers of the given type.
template <class Integer>
struct max_int_string_length
{
	/// Estimated maximum string length for integers of the given type.
	static const int value = (size_info<Integer>::bits + 2) / 3 + 3;
};

/// Converts the given integer of the given type into an ascii character string.
template <class Integer>
inline utf8_string int_to_string(Integer num)
{
	// Estimate decimal length
	char buffer[max_int_string_length<Integer>::value];
	// Assign to string
	return utf8_string(buffer, int_to_char(buffer, num));
}

/// Converts the given range of characters into an integer of the given type.
/// Does not require *end to be a terminating null character.
template <class CharIter, class Integer>
inline CharIter char_to_int(CharIter begin, CharIter end, Integer &num)
{
	typedef typename lean::strip_modifiers<Integer>::type int_type;
	typedef typename lean::int_type<lean::sign_class::sign, sizeof(int_type)>::type sint_type;
	typedef typename lean::int_type<lean::sign_class::no_sign, sizeof(int_type)>::type uint_type;

	bool flip = false;

	// Check, if signed
	if (static_cast<int_type>(-1) < static_cast<int_type>(0))
	{
		// Check sign
		if (*begin == '-')
		{
			flip = true;
			++begin;
		}
		else if(*begin == '+')
			++begin;
	}

	uint_type unum = 0U;

	CharIter first = begin;

	// Convert front-to-back (Horner)
	while (begin != end)
	{
		unsigned int digit = static_cast<unsigned int>(*begin - '0');

		// Stop on non-digit character
		if (digit > 9)
			break;

		unum = unum * 10U + static_cast<uint_type>(digit);
		++begin;
	}

	// Immediately return iterator to invalid character on error
	if (begin != first)
	{
		num = static_cast<int_type>(unum);

		// Check, if signed
		if (static_cast<int_type>(-1) < static_cast<int_type>(0))
		{
			// Flip number, if negative
			if (flip)
				num = static_cast<int_type>(-static_cast<sint_type>(unum));
		}
	}

	// Return end position
	return begin;
}

/// Converts the given range of characters into an integer of the given type.
template <class CharIter, class Integer>
inline bool string_to_int(const utf8_ntri &string, Integer &num)
{
	utf8_ntri::const_iterator end = string.end();
	return (char_to_int(string.begin(), end, num) == end);
}

/// Estimates the maximum string length for floating-point values of the given type.
template <class Float>
struct max_float_string_length
{
	/// Estimated maximum string length for floating-point values of the given type.
	static const int value = ((size_info<Float>::bits + 2) / 3) * 3 + 8;
};

/// Converts the given floating-point value of the given type into an ascii character string, returning a pointer to the first character not actively used.
/// Assumes the given iterator points to the beginning of a continuous range in memory. Overwrites *end with a terminating null character.
template <class CharIter, class Float>
inline CharIter float_to_char(CharIter buffer, Float num)
{
	static const int precision = static_cast<int>(
		(ieee_float_desc<Float>::mantissa_bits + 5) / 3 );

#ifdef _MSC_VER
	// Use MS extension
	static const _locale_t invariantLocale = _create_locale(LC_ALL, "C");
	return buffer + _sprintf_l(&(*buffer), "%.*g", invariantLocale, precision, num);
#else
	// TODO: Do what the standard library does?
	return buffer + sprintf(&(*buffer), "%.*g", precision, num);
#endif
}

/// Converts the given floating-point value of the given type into an ascii character string.
template <class Float>
inline utf8_string float_to_string(Float num)
{
	// Estimate decimal length
	char buffer[max_float_string_length<Float>::value];
	// Assign to string
	return utf8_string(buffer, float_to_char(buffer, num));
}

/// Converts the given range of characters into a floating-point value of the given type.
/// Assumes the given iterator points to the beginning of a continuous range in memory.
/// Expects *end to be either a terminating null or some other non-numeric character.
template <class CharIter, class Float>
inline CharIter char_to_float(CharIter begin, CharIter end, Float &num)
{
	double value;
	const char *pBegin = &(*begin);
	const char *pEnd;

#ifdef _MSC_VER
	// Use MS extension
	static const _locale_t invariantLocale = _create_locale(LC_ALL, "C");
	value = _strtod_l(pBegin, const_cast<char**>(&pEnd), invariantLocale);
#else
	// TODO: Do what the standard library does?
	value = strtod(pBegin, const_cast<char**>(&pEnd));
#endif

	CharIter stop = begin + (pEnd - pBegin);

	// First check for errors, the
	if (stop == end)
		num = static_cast<Float>(value);

	return stop;
}

/// Converts the given range of characters into a floating-point value of the given type.
template <class CharIter, class Float>
inline bool string_to_float(const utf8_ntri &string, Float &num)
{
	utf8_ntri::const_iterator end = string.end();
	return (char_to_float(string.begin(), end, num) == end);
}

} // namespace

using io::int_to_char;
using io::max_int_string_length;
using io::char_to_int;

using io::float_to_char;
using io::max_float_string_length;
using io::char_to_float;

} // namespace

#endif