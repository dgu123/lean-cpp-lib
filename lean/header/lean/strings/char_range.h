/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_CHAR_RANGE
#define LEAN_STRINGS_CHAR_RANGE

#include "../types.h"
#include "../meta/strip.h"

namespace lean
{
namespace strings
{

/// Character range class that may be constructed from arbitrary string classes.
template < class Char, class Traits = char_traits< typename strip_const<Char>::type > >
class char_range
{
private:
	Char *m_begin;
	Char *m_end;

	/// Computes the length of the given C string, in characters.
	static LEAN_INLINE size_t length(const Char *begin)
	{
		LEAN_ASSERT(begin);
		return Traits::length(begin);
	}

public:
	/// Type of the characters referenced by this class.
	typedef Char value_type;
	
	/// Type of the size returned by this range.
	typedef size_t size_type;
	/// Type of the difference between the addresses of two elements in this range.
	typedef ptrdiff_t difference_type;

	/// Type of pointers to the elements contained by this range.
	typedef value_type* pointer;
	/// Type of constant pointers to the elements contained by this range.
	typedef const value_type* const_pointer;
	/// Type of references to the elements contained by this range.
	typedef value_type& reference;
	/// Type of constant references to the elements contained by this range.
	typedef const value_type& const_reference;

	/// Type of iterators to the elements contained by this range.
	typedef pointer iterator;
	/// Type of constant iterators to the elements contained by this range.
	typedef const_pointer const_iterator;

	/// Unsigned integer literal that indicates an invalid position.
	static const size_type npos = static_cast<size_type>(-1);

	/// Constructs an empty character range.
	LEAN_INLINE char_range()
		: m_begin(nullptr),
		m_end(nullptr) { }
	/// Constructs a character range from the given characters.
	LEAN_INLINE char_range(const_pointer begin, const_pointer end)
		: m_begin(begin),
		m_end(end) { }
	/// Constructs a character range from the given C string.
	LEAN_INLINE char_range(const_pointer begin)
		: m_begin(begin),
		m_end(begin + length(begin)) { }

	/// Gets whether this character range is currently empty.
	LEAN_INLINE bool empty() const { return (m_begin == m_end); }
	/// Gets the size of this character range, in characters.
	LEAN_INLINE size_type size() const { return m_end - m_begin; }
	/// Gets the length of this character range, in code points.
	LEAN_INLINE size_type count() const { return Traits::count(m_begin, m_end); }

	/// Gets an element by position, access violation on failure.
	LEAN_INLINE reference operator [](size_type pos) { return m_begin[pos]; };
	/// Gets an element by position, access violation on failure.
	LEAN_INLINE const_reference operator [](size_type pos) const { return m_begin[pos]; };

	/// Returns an iterator to the first element contained by this character range.
	LEAN_INLINE iterator begin(void) { return m_begin; };
	/// Returns a constant iterator to the first element contained by this character range.
	LEAN_INLINE const_iterator begin(void) const { return m_begin; };
	/// Returns an iterator beyond the last element contained by this character range.
	LEAN_INLINE iterator end(void) { return m_end; };
	/// Returns a constant iterator beyond the last element contained by this character range.
	LEAN_INLINE const_iterator end(void) const { return m_end; };
};

} // namespace

using strings::char_range;

} // namespace

#include "char_range.h"

#endif