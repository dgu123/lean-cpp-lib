/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_UTF_TRAITS
#define LEAN_STRINGS_UTF_TRAITS

#include "../lean.h"
#include "../utf8.h"
#include "../meta/dependent_false.h"

namespace lean
{
namespace strings
{

// Provides common null-terminated character range functionality for the given utf character type.
template <class Char>
struct utf_traits
{
	LEAN_STATIC_ASSERT_MSG_ALT(dependent_false<Char>::value,
		"No UTF traits available for the given character type (only UTF-8 implemented, might use char_traits instead).",
		NO_UTF_traits_available_for_the_given_character_type__only_UTF8_implemented__might_use_char_traits_instead);
};

/// Provides common null-terminated character range functionality for the given utf character type.
template <>
struct utf_traits<utf8_t>
{
	/// Character type.
	typedef utf8_t char_type;
	/// Unsigned integer type of range equal to or greater than char_type's.
	typedef int_type<sign_class::no_sign, sizeof(char_type)>::type int_type;
	/// Size type.
	typedef size_t size_type;

	/// Checks if the given character is null.
	static LEAN_INLINE bool null(const char_type &src)
	{
		return (src == static_cast<char_type>(0));
	}

	/// Checks if the given null-terminated range of characters is empty.
	static LEAN_INLINE bool empty(const char_type *begin)
	{
		return null(*begin);
	}
	/// Gets the length of the given null-terminated range of characters.
	static size_type length(const char_type *begin)
	{
		return ::strlen(begin);
	}
	/// Gets the length of the given null-terminated range of characters.
	static size_type length(const char_type *begin, const char_type *end)
	{
		return end - begin;
	}
	/// Gets the number of code points in the given null-terminated range of characters.
	static LEAN_INLINE size_type count(const char_type *begin)
	{
		// MONITOR: Looping twice seems a bit suboptimal...
		return count(begin, begin + length(begin));
	}
	/// Gets the number of code points in the given null-terminated range of characters.
	static LEAN_INLINE size_type count(const char_type *begin, const char_type *end)
	{
		return utf8::unchecked::distance(begin, end);
	}

	/// Compares the characters in the given null-terminated ranges, returning true if equal.
	static bool equal(const char_type *begin1, const char_type *begin2)
	{
		return (::strcmp(begin1, begin2) == 0);
	}
	/// Compares the characters in the given null-terminated ranges, returning true if the first is less than the second.
	static bool less(const char_type *begin1, const char_type *begin2)
	{
		return (::strcmp(begin1, begin2) < 0);
	}
};

} // namespace

using strings::utf_traits;

} // namespace

#endif