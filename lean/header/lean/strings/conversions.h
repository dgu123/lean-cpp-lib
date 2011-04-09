/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_CONVERSIONS
#define LEAN_STRINGS_CONVERSIONS

#include "../lean.h"
#include "types.h"
#include <string>
#include <locale>
#include "../utf8.h"

namespace lean
{
namespace strings
{

/// Gets the current system locale.
static inline const std::locale& system_locale()
{
	static std::locale system("");
	return system;
}


/// Widens the given string using either the given locale or the current global locale.
inline std::wstring to_wchar(const char_ntri &narrow, const std::locale &locale = std::locale())
{
	std::wstring result(narrow.size(), 0);
	std::use_facet< std::ctype<wchar_t> >(locale).widen(narrow.begin(), narrow.end(), &result[0]);
	return result;
}

/// Narrows the given string using either the given locale or the current global locale.
inline std::string to_char(const wchar_ntri &wide, const std::locale &locale = std::locale(), char invalid = '?')
{
	std::string result(wide.size(), 0);
	std::use_facet< std::ctype<wchar_t> >(locale).narrow(wide.begin(), wide.end(), invalid, &result[0]);
	return result;
}


/// Widens the given string using either the given locale or the current global locale.
inline char2_string to_char2(const char1_ntri &narrow, const std::locale &locale = std::locale())
{
	char2_string result(narrow.size(), 0);
	std::use_facet< std::ctype<char2> >(locale).widen(narrow.begin(), narrow.end(), &result[0]);
	return result;
}

/// Narrows the given string using either the given locale or the current global locale.
inline char1_string to_char1(const char2_ntri &wide, const std::locale &locale = std::locale(), char invalid = '?')
{
	char1_string result(wide.size(), 0);
	std::use_facet< std::ctype<char2> >(locale).narrow(wide.begin(), wide.end(), invalid, &result[0]);
	return result;
}


/// Widens the given string using either the given locale or the current global locale.
inline char4_string to_char4(const char1_ntri &narrow, const std::locale &locale = std::locale())
{
	char4_string result(narrow.size(), 0);
	std::use_facet< std::ctype<char4> >(locale).widen(narrow.begin(), narrow.end(), &result[0]);
	return result;
}

/// Narrows the given string using either the given locale or the current global locale.
inline char1_string to_char1(const char4_ntri &wide, const std::locale &locale = std::locale(), char invalid = '?')
{
	char1_string result(wide.size(), 0);
	std::use_facet< std::ctype<char4> >(locale).narrow(wide.begin(), wide.end(), invalid, &result[0]);
	return result;
}


/// Converts the given string to utf-8.
inline utf8_string to_utf8(const utf32_ntri &wide)
{
	utf8_string result(wide.count(), 0);
	utf8::unchecked::utf32to8(wide.begin(), wide.end(), result.begin());
	return result;
}

/// Converts the given string to utf-8.
inline utf8_string to_utf8(const utf16_ntri &wide)
{
	utf8_string result(wide.count(), 0);
	utf8::unchecked::utf16to8(wide.begin(), wide.end(), result.begin());
	return result;
}

/// Converts the given string to utf-8 using either the given locale or the current global locale.
inline utf8_string to_utf8(const char_ntri &narrow, const std::locale &locale = std::locale())
{
	return to_utf8(to_wchar(narrow, locale));
}

} // namespace

} // namespace

#endif