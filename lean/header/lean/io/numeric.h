/*****************************************************/
/* lean IO                      (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_IO_NUMERIC
#define LEAN_IO_NUMERIC

#include "../lean.h"
#include "../meta/strip.h"
#include <clocale>
#include <cstdio>

namespace lean
{
namespace io
{

/// Converts the given integer of the given type into an ascii character string, returning a pointer to the first character not written to.
/// Does not append a terminating null character.
template <class CharIter, class Integer>
static CharIter int_to_char(CharIter buffer, Integer num)
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

/// Converts the given range of characters into an integer of the given type.
/// Does not require *end to be a terminating null character.
template <class CharIter, class Integer>
static CharIter char_to_int(CharIter begin, CharIter end, Integer &num)
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

	// Convert front-to-back (Horner)
	while (begin != end)
	{
		unsigned int digit = static_cast<unsigned int>(*begin - '0');

		// Return iterator to invalid character on error
		if (digit > 9)
			return begin;

		unum = unum * 10U + static_cast<uint_type>(digit);
		++begin;
	}

	num = static_cast<int_type>(unum);

	// Check, if signed
	if (static_cast<int_type>(-1) < static_cast<int_type>(0))
	{
		// Flip number, if negative
		if (flip)
			num = static_cast<int_type>(-static_cast<sint_type>(unum));
	}

	// Return end
	return begin;
}

/// Converts the given floating-point value of the given type into an ascii character string, returning a pointer to the first character not actively used.
/// Assumes the given iterator points to the beginning of a continuous range in memory. Overwrites *end with a terminating null character.
template <class CharIter, class Float>
static CharIter float_to_char(CharIter buffer, Float num)
{
	static const int precision = static_cast<int>(
		(ieee_float_desc<Float>::mantissa_bits + 2) / 3 );

#ifdef _MSC_VER
	// Use MS extension
	static const _locale_t invariantLocale = _create_locale(LC_ALL, "C");
	return buffer + _sprintf_l(&(*buffer), "%.*g", invariantLocale, precision, num);
#else
	// TODO: Do what the standard library does?
	return buffer + sprintf(&(*buffer), "%.*g", precision, num);
#endif
}

/// Converts the given range of characters into a floating-point value of the given type.
/// Assumes the given iterator points to the beginning of a continuous range in memory.
/// Expects *end to be either a terminating null or some other non-numeric character.
template <class CharIter, class Float>
static CharIter char_to_float(CharIter begin, CharIter end, Float &num)
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

} // namespace

using io::int_to_char;
using io::char_to_int;
using io::float_to_char;
using io::char_to_float;

} // namespace

#endif