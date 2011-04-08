/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_NULLTERMINATED
#define LEAN_STRINGS_NULLTERMINATED

#include "../lean.h"
#include "../meta/strip.h"
#include "char_traits.h"

namespace lean
{
namespace strings
{

/// Nullterminated character range class that may be constructed from arbitrary string classes.
template < class Char, class Traits = char_traits< typename strip_const<Char>::type > >
class nullterminated
{
private:
	const Char *m_begin;

public:
	/// Type of the characters referenced by this range.
	typedef typename strip_const<Char>::type value_type;
	
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

	/// Character traits used by this range.
	typedef Traits traits_type;
	
	/// Constructs a character range from the given C string.
	LEAN_INLINE char_range(const_pointer begin)
		: m_begin(begin)
	{
		LEAN_ASSERT(m_begin);
	}

	/// Gets whether this character range is currently empty.
	LEAN_INLINE bool empty() const { return char_traits::empty(m_begin); }
	/// Gets the length of this character range, in characters (same as size()).
	LEAN_INLINE size_type length() const { return char_traits::length(m_begin); }
	/// Gets the length of this character range, in characters (same as length()).
	LEAN_INLINE size_type size() const { return length(); }
	/// Gets the length of this character range, in code points (might differ from length() and size()).
	LEAN_INLINE size_type count() const { return char_traits::count(m_begin); }
	
	/// Gets an element by position, access violation on failure.
	LEAN_INLINE const_reference operator [](size_type pos) const { return m_begin[pos]; }
	
	/// Gets a pointer to this nullterminated range.
	LEAN_INLINE const_pointer c_str() const { return m_begin; }
	/// Gets a pointer to this nullterminated range.
	LEAN_INLINE const_pointer data() const { return c_str(); }

	/// Returns a constant iterator to the first element contained by this character range.
	LEAN_INLINE const_iterator begin(void) const { return m_begin; }
	/// Returns a constant iterator  the last element contained by this character range.
	LEAN_INLINE const_iterator end() const { return m_begin + length(); }

	/// Swaps the contents of this range with the contents of the given range.
	LEAN_INLINE void swap(nullterminated& right)
	{
		const_pointer right_begin = right.m_begin;
		right.m_begin = m_begin;
		m_begin = right_begin;
	}
};

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator ==(const nullterminated<Char, Traits>& left, const nullterminated<Char, Traits>& right)
{
	return (nullterminated<Char, Traits>::traits_type::compare(left.c_str(), right.c_str()) == 0);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator !=(const nullterminated<Char, Traits>& left, const nullterminated<Char, Traits>& right)
{
	return !(left == right);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator <(const nullterminated<Char, Traits>& left, const nullterminated<Char, Traits>& right)
{
	return (nullterminated<Char, Traits>::traits_type::compare(left.c_str(), right.c_str()) < 0);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator >(const nullterminated<Char, Traits>& left, const nullterminated<Char, Traits>& right)
{
	return (right < left);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator <=(const nullterminated<Char, Traits>& left, const nullterminated<Char, Traits>& right)
{
	return !(right < left);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator >=(const nullterminated<Char, Traits>& left, const nullterminated<Char, Traits>& right)
{
	return !(left < right);
}

/// Swaps the elements of two nullterminated character ranges.
template <class Char, class Traits>
LEAN_INLINE void swap(nullterminated<Char, Traits>& left, nullterminated<Char, Traits>& right)
{
	left.swap(right);
}

} // namespace

using strings::nullterminated;

} // namespace

#endif