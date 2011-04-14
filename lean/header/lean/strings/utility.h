/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_UTILITY
#define LEAN_STRINGS_UTILITY

#include "../lean.h"
#include <cstring>
#include <cwchar>

namespace lean
{
namespace strings
{
	/// Copies at maximum the given number of characters from source to dest string, always incorporating a terminating null.
	inline size_t strmcpy(char *dest, const char *source, size_t maxChars)
	{
		if (maxChars > 0)
		{
			size_t len = min(::strlen(source), maxChars - 1);
			::memcpy(dest, source, len);
			dest[len] = 0;
			return len;
		}
		else
			return 0;
	}

	/// Copies at maximum the given number of characters from source to dest string, always incorporating a terminating null.
	inline size_t wcsmcpy(wchar_t *dest, const wchar_t *source, size_t maxChars)
	{
		if (maxChars > 0)
		{
			size_t len = min(::wcslen(source), maxChars - 1);
			::memcpy(dest, source, len * sizeof(wchar_t));
			dest[len] = 0;
			return len;
		}
		else
			return 0;
	}

} // namespace

using strings::strmcpy;
using strings::wcsmcpy;

} // namespace

#endif