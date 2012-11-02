/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
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
	typedef nullterminated_implicit<Char, Traits> base_type;

public:
	/// Type of the characters referenced by this range.
	typedef typename base_type::value_type value_type;
	
	/// Type of the size returned by this range.
	typedef typename base_type::size_type size_type;
	/// Type of the difference between the addresses of two elements in this range.
	typedef typename base_type::difference_type difference_type;

	/// Type of pointers to the elements contained by this range.
	typedef typename base_type::pointer pointer;
	/// Type of constant pointers to the elements contained by this range.
	typedef typename base_type::const_pointer const_pointer;
	/// Type of references to the elements contained by this range.
	typedef typename base_type::reference reference;
	/// Type of constant references to the elements contained by this range.
	typedef typename base_type::const_reference const_reference;

	/// Type of iterators to the elements contained by this range.
	typedef typename base_type::pointer iterator;
	/// Type of constant iterators to the elements contained by this range.
	typedef typename base_type::const_iterator const_iterator;

	/// Character traits used by this range.
	typedef typename base_type::traits_type traits_type;

private:
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
	const_pointer m_end;

public:
	/// Constructs a character range from the given half-range.
	LEAN_INLINE nullterminated_range_implicit(const nullterminated_implicit<Char, Traits> &right)
		: base_type(right),
		m_end(right.compute_end())
	{
		assert_null_terminated(m_end);
	}
	/// Constructs a character range from the given C string.
	LEAN_INLINE nullterminated_range_implicit(const_pointer begin)
		: base_type(begin),
		m_end(base_type::compute_end())
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
				base_type::compute_end() )
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
	Compatible to() const
	{
		typedef typename assert_nullterminated_compatible<
				Compatible,
				value_type, traits_type
			>::type assert_compatible;
		return nullterminated_compatible<Compatible, value_type, traits_type>::to(m_begin, m_end);
	}
};

/// Nullterminated character range class that may be constructed from arbitrary string classes.
template < class Char, class Traits = char_traits<typename strip_const<Char>::type> >
class nullterminated_range : public nullterminated_range_implicit<Char, Traits>
{
public:
	/// Corresponding implicit type.
	typedef nullterminated_range_implicit<Char, Traits> implicit_type;

	/// Constructs a character range from the given C string.
	explicit LEAN_INLINE nullterminated_range(typename implicit_type::const_pointer begin)
		: implicit_type(begin) { }
	/// Constructs a character range from the given C string range (*end must be null character).
	LEAN_INLINE nullterminated_range(typename implicit_type::const_pointer begin, typename implicit_type::const_pointer end)
		: implicit_type(begin, end) { }
	/// Constructs a character range from the given compatible object.
	template <class Compatible>
	explicit LEAN_INLINE nullterminated_range(const Compatible &from,
			typename enable_if<is_nullterminated_compatible<Compatible, value_type, traits_type>::value, const void*>::type = nullptr)
		: implicit_type(from) { }
	/// Constructs a character range from the given implicit range.
	explicit LEAN_INLINE nullterminated_range(const implicit_type &right)
		: implicit_type(right) { }

	/// Constructs a character range from the given half-range.
	explicit LEAN_INLINE nullterminated_range(const nullterminated_implicit<Char, Traits> &right)
		: implicit_type(right) { }
	/// Constructs a character range from the given half-range.
	LEAN_INLINE nullterminated_range(const nullterminated<Char, Traits> &right)
		: implicit_type(right) { }

	/// Constructs a half-range from this character range.
	LEAN_INLINE operator nullterminated<Char, Traits>() const
	{
		return nullterminated<Char, Traits>(*this);
	}
};

/// Makes an explicit nullterminated range from the given implicit range.
template <class Char, class Traits>
LEAN_INLINE nullterminated_range<Char, Traits> make_ntr(const nullterminated_range_implicit<Char, Traits> &range)
{
	return nullterminated_range<Char, Traits>(range);
}
/// Makes an explicit nullterminated range from the given implicit range.
template <class Char>
LEAN_INLINE nullterminated_range<Char> make_ntr(const Char *range)
{
	return nullterminated_range<Char>(range);
}
/// Makes an explicit nullterminated range from the given implicit range.
template <class Char, class Compatible>
LEAN_INLINE typename enable_if<
		is_nullterminated_compatible<Compatible, Char, typename nullterminated_range<Char>::traits_type>::value,
		nullterminated_range<Char>
	>::type	make_ntr(const Compatible &compatible)
{
	return nullterminated_range<Char>(compatible);
}

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator ==(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return (left.length() == right.length()) && nullterminated<Char, Traits>::traits_type::equal(left.c_str(), right.c_str());
}
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator ==(const nullterminated_range_implicit<Char, Traits>& left, const Compatible& right) { return left == make_ntr<Char, Traits>(right); }
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator ==(const Compatible& left, const nullterminated_range_implicit<Char, Traits>& right) { return make_ntr<Char, Traits>(left) == right; }

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator !=(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return !(left == right);
}
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator !=(const nullterminated_range_implicit<Char, Traits>& left, const Compatible& right) { return left != make_ntr<Char, Traits>(right); }
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator !=(const Compatible& left, const nullterminated_range_implicit<Char, Traits>& right) { return make_ntr<Char, Traits>(left) != right; }

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator <(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return nullterminated<Char, Traits>::traits_type::less(left.c_str(), right.c_str());
}
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator <(const nullterminated_range_implicit<Char, Traits>& left, const Compatible& right) { return left < make_ntr<Char, Traits>(right); }
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator <(const Compatible& left, const nullterminated_range_implicit<Char, Traits>& right) { return make_ntr<Char, Traits>(left) < right; }

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator >(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return (right < left);
}
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator >(const nullterminated_range_implicit<Char, Traits>& left, const Compatible& right) { return left > make_ntr<Char, Traits>(right); }
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator >(const Compatible& left, const nullterminated_range_implicit<Char, Traits>& right) { return make_ntr<Char, Traits>(left) > right; }

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator <=(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return !(right < left);
}
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator <=(const nullterminated_range_implicit<Char, Traits>& left, const Compatible& right) { return left <= make_ntr<Char, Traits>(right); }
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator <=(const Compatible& left, const nullterminated_range_implicit<Char, Traits>& right) { return make_ntr<Char, Traits>(left) <= right; }

/// Comparison operator.
template <class Char, class Traits>
LEAN_INLINE bool operator >=(const nullterminated_range_implicit<Char, Traits>& left, const nullterminated_range_implicit<Char, Traits>& right)
{
	return !(left < right);
}
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator >=(const nullterminated_range_implicit<Char, Traits>& left, const Compatible& right) { return left >= make_ntr<Char, Traits>(right); }
template <class Char, class Traits, class Compatible>
LEAN_INLINE typename enable_if<is_nullterminated_convertible<Compatible, Char, Traits>::value, bool>::type
	operator >=(const Compatible& left, const nullterminated_range_implicit<Char, Traits>& right) { return make_ntr<Char, Traits>(left) >= right; }

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