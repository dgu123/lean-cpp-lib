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


//// UTF / UTF ////

/// Converts the given string from UTF-8 to UTF-16.
inline utf16_string utf_to_utf16(const utf8_ntri &wide)
{
	utf16_string result(wide.size(), 0);
	result.erase(
		utf8::unchecked::utf8to16(wide.begin(), wide.end(), result.begin()),
		result.end());
	return result;
}

/// Converts the given string from UTF-16 to UTF-8.
inline utf8_string utf_to_utf8(const utf16_ntri &wide)
{
	utf8_string result(2 * wide.size(), 0);
	result.erase(
		utf8::unchecked::utf16to8(wide.begin(), wide.end(), result.begin()),
		result.end());
	return result;
}


/// Converts the given string from UTF-8 to UTF-32.
inline utf32_string utf_to_utf32(const utf8_ntri &wide)
{
	utf32_string result(wide.size(), 0);
	result.erase(
		utf8::unchecked::utf8to32(wide.begin(), wide.end(), result.begin()),
		result.end());
	return result;
}

/// Converts the given string from UTF-32 to UTF-8.
inline utf8_string utf_to_utf8(const utf32_ntri &wide)
{
	utf8_string result(4 * wide.size(), 0);
	result.erase(
		utf8::unchecked::utf32to8(wide.begin(), wide.end(), result.begin()),
		result.end());
	return result;
}


//// Codepage / UTF-XX ////

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


/// Widens the given string to UTF-16 using either the given locale or the current global locale.
inline utf16_string to_utf16(const char_ntri &narrow, const std::locale &locale = std::locale())
{
	utf16_string result(narrow.size(), 0);
	std::use_facet< std::ctype<utf16_t> >(locale).widen(narrow.begin(), narrow.end(), &result[0]);
	return result;
}

/// Narrows the given to UTF-16 string using either the given locale or the current global locale.
inline std::string utf_to_char(const utf16_ntri &wide, const std::locale &locale = std::locale(), char invalid = '?')
{
	std::string result(wide.size(), 0);
	std::use_facet< std::ctype<utf16_t> >(locale).narrow(wide.begin(), wide.end(), invalid, &result[0]);
	return result;
}


/// Widens the given string to UTF-32 using either the given locale or the current global locale.
inline utf32_string to_utf32(const char_ntri &narrow, const std::locale &locale = std::locale())
{
	utf32_string result(narrow.size(), 0);
	std::use_facet< std::ctype<utf32_t> >(locale).widen(narrow.begin(), narrow.end(), &result[0]);
	return result;
}

/// Narrows the given UTF-32 string using either the given locale or the current global locale.
inline std::string utf_to_char(const utf32_ntri &wide, const std::locale &locale = std::locale(), char invalid = '?')
{
	std::string result(wide.size(), 0);
	std::use_facet< std::ctype<utf32_t> >(locale).narrow(wide.begin(), wide.end(), invalid, &result[0]);
	return result;
}


//// Codepage / UTF-8 ////

/// Widens the given string to UTF-8 using either the given locale or the current global locale.
inline utf8_string to_utf8(const char_ntri &narrow, const std::locale &locale = std::locale())
{
	return utf_to_utf8(to_utf16(narrow, locale));
}

/// Narrows the given UTF-8 string using either the given locale or the current global locale.
inline std::string utf_to_char(const utf8_ntri &utf8, const std::locale &locale = std::locale(), char invalid = '?')
{
	return utf_to_char(utf_to_utf16(utf8), locale, invalid);
}

} // namespace

using strings::to_wchar;
using strings::to_char;

using strings::to_utf8;
using strings::to_utf16;
using strings::to_utf32;

using strings::utf_to_utf8;
using strings::utf_to_utf16;
using strings::utf_to_utf32;

using strings::utf_to_char;

} // namespace

#endif