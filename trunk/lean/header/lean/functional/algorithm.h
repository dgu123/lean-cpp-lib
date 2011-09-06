/*****************************************************/
/* lean Functional              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_FUNCTIONAL_ALGORITHM
#define LEAN_FUNCTIONAL_ALGORITHM

#include "../lean.h"
#include "../tags/noncopyable.h"
#include <functional>
#include <algorithm>

namespace lean
{
namespace functional
{

/// Inserts the element pointed at by <code>last</code> into the given sorted range <code>[first, last)</code>.
template <class Iterator, class Predicate>
inline Iterator insert_last(Iterator first, Iterator last)
{
	for  (Iterator current = first; current != last; ++current)
		if (*last < *current)
		{
			std::rotate(current, last, last + 1);
			return current;
		}

	return last;
}

/// Inserts the element pointed at by <code>last</code> into the given sorted range <code>[first, last)</code>.
template <class Iterator, class Predicate>
inline Iterator insert_last(Iterator first, Iterator last, Predicate predicate)
{
	for  (Iterator current = first; current != last; ++current)
		if (predicate(*last, *current))
		{
			std::rotate(current, last, last + 1);
			return current;
		}

	return last;
}

} // namespace

using functional::insert_last;

} // namespace

#endif