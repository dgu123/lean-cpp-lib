/*****************************************************/
/* lean IO                      (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_IO_NUMERIC
#define LEAN_IO_NUMERIC

#include "../lean.h"
#include "../meta/strip.h"
#include <algorithm>

namespace lean
{
namespace io
{

/// Converts the given integer of the given type into an ascii character string, returning a pointer to the first character not written to.
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

namespace impl
{



}

/// Converts the given floating-point value of the given type into an ascii character string, returning a pointer to the first character not written to.
template <class CharIter, class Float>
static CharIter float_to_char(CharIter buffer, Float num)
{
	typedef typename strip_modifiers<typename strip_reference<Float>::type>::type float_type;
	typedef ieee_float<float_type> float_helper;
	typedef typename float_helper::int_type int_type;
	typedef typename float_helper::shift_type shift_type;
	
	int_type exponent = float_helper::exponent(num);
	int_type mantissa = float_helper::mantissa(num);
	
	// Check sign
	if (float_helper::sign(num))
		*(buffer++) = '-';

	// Handle special numbers
	if (float_helper::is_special(exponent))
	{
		const char *what = float_helper::is_infinity(mantissa) ? "inf" : "nan";
		buffer = std::copy(what, what + 3, buffer);
	}
	// Convert to decimal
	else
	{
		shift_type shift = float_helper::shift(exponent);
		int_type fixed = float_helper::fixed(mantissa, exponent);
		
		while (shift > 0)
		{
		}

/*		int_type exp10 = static_cast<int_type>((shift < 0) ? -shift : shift);
		exp10 = (exp10 << 12U) / 13607U; // exp10 /= log_2{10}

		if (shift > 0)
		{
			// ...
		}
*/
	}

	// Return end
	return buffer;
}

} // namespace

using io::int_to_char;
using io::char_to_int;

} // namespace

#endif