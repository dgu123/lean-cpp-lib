/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_NULLTERMINATED_RANGE
#define LEAN_STRINGS_NULLTERMINATED_RANGE

#include "../lean.h"
#include "../meta/strip.h"
#include "../meta/enable_if.h"
#include "char_traits.h"
#include "nullterminated.h"

namespace lean
{
namespace strings
{

/// Nullterminated character range class that may be constructed IMPLICITLY from arbitrary string classes.
/// May be used in parameter lists, not recommended elsewhere.
template < class Char, class Traits = char_traits<typename strip_const<Char>::type> >
class nullterminated_range_implicit : public nullterminated_implicit<Char, Traits>
{
private:
	typedef nullterminated_implicit<Char, Traits> base_type;
	
	/// Asserts null-termination.
	static LEAN_INLINE void assert_null_terminated(const_pointer end)
	{
		LEAN_ASSERT(end);
		LEAN_ASSERT(traits_type::null(*end));
	}

	/// Gets the valid of the given two pointers.
	static LEAN_INLINE const_pointer first_non_null(const_pointer a, const_pointer b)
	{
		return (a) ? a : b;
	}

protected:
	const Char *m_end;

public:
	/// Constructs a character range from the given half-range.
	LEAN_INLINE nullterminated_range_implicit(const nullterminated_implicit<Char, Traits> &right)
		: base_type(right),
		m_end(right.end())
	{
		assert_null_terminated(m_end);
	}
	/// Constructs a character range from the given C string.
	LEAN_INLINE nullterminated_range_implicit(const_pointer begin)
		: base_type(begin),
		m_end(base_type::end())
	{
		assert_null_terminated(m_end);
	}
	/// Constructs a character range from the given C string range (*end must be null character).
	LEAN_INLINE nullterminated_range_implicit(const_pointer begin, const_pointer end)
		: base_type(begin),
		m_end(end)
	{
		assert_null_terminated(m_end);
	}
	/// Constructs a character range from the given compatible object.
	template <class Compatible>
	LEAN_INLINE nullterminated_range_implicit(const Compatible &from,
		typename enable_if<is_nullterminated_compatible<Compatible, value_type, traits_type>::value, const void*>::type = nullptr)
		: base_type(from),
		m_end(
			first_non_null(
				nullterminated_compatible<Compatible, value_type, traits_type>::from(from, base_type::begin()),
				base_type::end() )
			)
	{
		assert_null_terminated(m_end);
	}

	/// Gets whether this character range is currently empty.
	LEAN_INLINE bool empty() const { return (m_begin == m_end); }
	/// Gets the length of this character range, in characters (same as size()). O(1).
	LEAN_INLINE size_type length() const { return m_end - m_begin; }
	/// Gets the length of this character range, in characters (same as length()). O(1).
	LEAN_INLINE size_type size() const { return length(); }
	/// Gets the length of this character range, in code points (might differ from length() and size()). O(n).
	LEAN_INLINE size_type count() const { return traits_type::count(m_begin, m_end); }
	
	/// Returns a constant iterator  the last element contained by this character range. O(1).
	LEAN_INLINE const_iterator end() const { return m_end; }

	/// Swaps the contents of this range with the contents of the given range.
	LEAN_INLINE void swap(nullterminated_range_implicit& right)
	{
		base_type::swap(right);
		const_pointer right_end = right.m_end;
		right.m_end = m_end;
		m_end = right_end;
	}

	/// Constructs a compatible object from this null-terminated character range.
	template <class Compatible>
	operator Compatible() const
	{
		typedef typename assert_nullterminated_compatible<Compatible, value_type, traits_type>::type assert_compatible;
		return nullterminated_compatible<Compatible, value_type, traits_type>::to(m_begin, m_end);
	}
};

/// Nullterminated character range class that may be constructed from arbitrary string classes.
template < class Char, class Traits = char_traits<typename strip_const<Char>::type> >
class nullterminated_range : public nullterminated_range_implicit<Char, Traits>
{
private:
	typedef nullterminated_range_implicit<Char, Traits> base_type;

public:
	/// Constructs a character range from the given half-range.
	LEAN_INLINE nullterminated_range(const nullterminated_implicit<Char, Traits> &right)
		: base_type(right) { }
	/// Constructs a character range from the given C string.
	explicit LEAN_INLINE nullterminated_range(const_pointer begin)
		: base_type(begin) { }
	/// Constructs a character range from the given C string range (*end must be null character).
	LEAN_INLINE nullterminated_range(const_pointer begin, const_pointer end)
		: base_type(begin, end) { }
	/// Constructs a character range from the given compatible object.
	template <class Compatible>
	explicit LEAN_INLINE nullterminated_range(const Compatible &from)
		: base_type(from) { }
};

/// Makes an explicit nullterminated range from the given implicit range.
template <class Char, class Traits>
LEAN_INLINE nullterminated_range<Char, Traits> make_ntr(const nullterminated_range_implicit<Char, Traits> &range)
{
	return nullterminated_range<Char, Traits>(range);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator ==(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return (left.length() == right.length()) && nullterminated<Char, Traits>::traits_type::equal(left.c_str(), right.c_str());
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator !=(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return !(left == right);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator <(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return left.length() < right.length() || nullterminated<Char, Traits>::traits_type::less(left.c_str(), right.c_str());
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator >(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return (right < left);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator <=(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return !(right < left);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator >=(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return !(left < right);
}

/// Swaps the elements of two null-terminated character ranges.
template <class Char, class Traits>
LEAN_INLINE void swap(nullterminated_range_implicit<Char, Traits>& left, nullterminated_range_implicit<Char, Traits>& right)
{
	left.swap(right);
}

} // namespace

using strings::nullterminated_range_implicit;
using strings::nullterminated_range;

using strings::make_ntr;

} // namespace

#endif