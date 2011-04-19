/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_CHARSTREAM
#define LEAN_STRINGS_CHARSTREAM

#include "../lean.h"
#include <iostream>

namespace lean
{
namespace strings
{

/// Character stream buffer class that is required to allow for allocation-free usage of STL streaming facilities.
template < class Elem, class Traits = std::char_traits<Elem> >
class basic_charbuf : public std::basic_streambuf<Elem, Traits>
{
public:
	/// Constructs a character stream buffer from the given character range.
	basic_charbuf(char_type *begin, char_type *end)
	{
		setp(begin, end);
		setg(begin, begin, end);
	}
};

namespace impl
{
	/// Helper class that allows for automatic stream buffer construction before stream base class construction.
	template < class Elem, class Traits = std::char_traits<Elem> >
	class charbuf_holder
	{
	protected:
		/// Stream buffer type.
		typedef basic_charbuf<Elem, Traits> stream_buffer;
		/// Stream buffer.
		stream_buffer m_buffer;

		/// Constructs a character stream buffer from the given character range.
		charbuf_holder(typename stream_buffer::char_type *begin, typename stream_buffer::char_type *end)
			: m_buffer(begin, end) { }
	};
}

/// Character stream class that allows for allocation-free usage of STL streaming facilities.
template < class Elem, class Traits = std::char_traits<Elem> >
class basic_charstream : private impl::charbuf_holder<Elem, Traits>, public std::basic_iostream<Elem, Traits>
{
public:
	/// Stream buffer type.
	typedef charbuf_holder<char_type, traits_type>::stream_buffer stream_buffer;

	/// Constructs a character stream from the given character range.
	basic_charstream(char_type *begin, char_type *end /*,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out*/ )
			: impl::charbuf_holder<char_type, traits_type>(begin, end),
			std::basic_iostream<char_type, traits_type>(&m_buffer) { }

	/// Returns the address of the stored stream buffer object.
	stream_buffer* rdbuf() const { return m_buffer; }

};

/// Character stream class.
typedef basic_charstream<char> charstream;
/// Wide-character stream class.
typedef basic_charstream<wchar_t> wcharstream;

} // namespace

using strings::basic_charbuf;

using strings::basic_charstream;
using strings::charstream;
using strings::wcharstream;

} // namespace

#endif