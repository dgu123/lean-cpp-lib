/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_CHAR_TRAITS
#define LEAN_STRINGS_CHAR_TRAITS

#include "../lean.h"
#include <cstring>

namespace lean
{
namespace strings
{

/// Provides common functionality for the given character type. The default implementation treats characters as arbitrary POD types.
template <class Char>
struct char_traits
{
	/// Character type.
	typedef Char char_type;
	/// Corresponding integer type.
	typedef typename int_type<sign_class::no_sign, sizeof(char_type)>::type int_type;
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
		size_t length = 0;

		while (!null(*(begin++)))
			++length;

		return length;
	}

	/// Gets the number of code points in the given null-terminated range of characters.
	static LEAN_INLINE size_type count(const char_type *begin)
	{
		return length(begin);
	}

	/// Converts the given integer into a character.
	static LEAN_INLINE char_type to_char_type(const int_type &integer)
	{
		return static_cast<char_type>(integer);
	}
	/// Converts the given character into an integer.
	static LEAN_INLINE int_type to_int_type(const char_type &character)
	{
		return static_cast<int_type>(character);
	}

	/// Compares the given number of characters.
	static LEAN_INLINE int *compare(const char_type *dest, const char_type *source, size_t count)
	{
		const char_type *destEnd = dest + count;

		while (dest != destEnd)
			if (*(dest++) != *(source++))
				// Handle null (end of string) using unsigned comparison
				return (static_cast<int_type>(*dest) < static_cast<int_type>(*source)) ? -1 : 1;
			else if (null(*dest))
				break;
		
		return 0;
	}

	/// Copies the given number of source characters to the given destination characters.
	static LEAN_INLINE char_type *copy(char_type *dest, const char_type *source, size_t count)
	{
		::memcpy(dest, source, count * sizeof(char_type));
	}

	/// Moves the given number of source characters to the given destination characters (may be overlapping ranges).
	static LEAN_INLINE char_type *move(char_type *dest, const char_type *source, size_t count)
	{
		::memmove(dest, source, count * sizeof(char_type));
	}

	/// Assigns the given source character to the given destination character.
	static LEAN_INLINE void assign(char_type &dest, const char_type &src)
	{
		dest = src;
	}
	/// Assigns the given number of source characters to the given destination characters.
	static LEAN_INLINE void assign(char_type *dest, size_t count, char_type src)
	{
		char_type *destEnd = dest + count;

		while (dest != destEnd)
			*(dest++) = src;
	}
};

} // namespace

using strings::char_traits;

} // namespace

#endif