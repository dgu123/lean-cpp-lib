/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_TYPES
#define LEAN_STRINGS_TYPES

#include "../lean.h"
#include "char_traits.h"
#include "utf_traits.h"
#include "nullterminated.h"
#include "nullterminated_range.h"
#include <string>

namespace lean
{
namespace strings
{

/// Nullterminated character half-range.
typedef nullterminated<char> char_nt;
/// Nullterminated character range.
typedef nullterminated_range<char> char_ntr;

/// Nullterminated wide-character half-range.
typedef nullterminated<wchar_t> wchar_nt;
/// Nullterminated wide-character range.
typedef nullterminated_range<wchar_t> wchar_ntr;


/// Nullterminated 1-byte-character half-range.
typedef nullterminated<char1> char1_nt;
/// Nullterminated 1-byte-character range.
typedef nullterminated_range<char1> char1_ntr;

/// Nullterminated 2-byte-character half-range.
typedef nullterminated<char2> char2_nt;
/// Nullterminated 2-byte-character range.
typedef nullterminated_range<char2> char2_ntr;

/// Nullterminated 4-byte-character half-range.
typedef nullterminated<char4> char4_nt;
/// Nullterminated 4-byte-character range.
typedef nullterminated_range<char4> char4_ntr;


/// Nullterminated utf8-character half-range.
typedef nullterminated< utf8_t, utf_traits<utf8_t> > utf8_nt;
/// Nullterminated utf8-character range.
typedef nullterminated_range< utf8_t, utf_traits<utf8_t> > utf8_ntr;


// STL compatibility
template <class Char, class StringTraits, class StringAllocator, class NullterminatedTraits>
struct nullterminated_compatible< std::basic_string<Char, StringTraits, StringAllocator>, Char, NullterminatedTraits >
{
	typedef std::basic_string<Char, StringTraits, StringAllocator> compatible_type;

	static LEAN_INLINE const Char* from(const compatible_type &from) { return from.c_str(); }
	static LEAN_INLINE const Char* from(const compatible_type &from, const Char *begin) { return begin + from.size(); }
	static LEAN_INLINE compatible_type to(const Char *begin) { return compatible_type(begin); }
	static LEAN_INLINE compatible_type to(const Char *begin, const Char *end) { return compatible_type(begin, end); }
};

} // namespace

using strings::char_nt;
using strings::char_ntr;
using strings::wchar_nt;
using strings::wchar_ntr;

using strings::char1_nt;
using strings::char1_ntr;
using strings::char2_nt;
using strings::char2_ntr;
using strings::char4_nt;
using strings::char4_ntr;

using strings::utf8_nt;
using strings::utf8_ntr;

} // namespace

#endif