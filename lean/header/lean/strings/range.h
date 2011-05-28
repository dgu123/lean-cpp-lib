/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_RANGE
#define LEAN_STRINGS_RANGE

#include "../lean.h"

namespace lean
{
namespace strings
{

/// Iterator range.
template <class Iterator>
class range
{
private:
	Iterator m_begin;
	Iterator m_end;

public:
	/// Iterator type.
	typedef Iterator iterator;
	/// Iterator type.
	typedef Iterator const_iterator;

	/// Constructs an iterator range.
	LEAN_INLINE range(iterator begin, iterator end)
		: m_begin(begin), m_end(end) { }

	/// Gets the beginning of this range.
	LEAN_INLINE iterator& begin() { return m_begin; }
	/// Gets the beginning of this range.
	LEAN_INLINE iterator begin() const { return m_begin; }
	/// Gets the end of this range.
	LEAN_INLINE iterator& end() { return m_end; }
	/// Gets the beginning of this range.
	LEAN_INLINE iterator end() const { return m_end; }
};

/// Makes a range from the given pair of iterators.
template <class Iterator>
LEAN_INLINE range<Iterator> make_range(Iterator begin, Iterator end)
{
	return range<Iterator>(begin, end);
}

/// Constructs an object of the given type from the given range.
template <class Class, class Range>
LEAN_INLINE Class from_range(const Range &range)
{
	return Class(range.begin(), range.end());
}

} // namespace

using strings::range;
using strings::make_range;
using strings::from_range;

} // namespace

#endif