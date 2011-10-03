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
template <class Iterator>
inline Iterator insert_last(Iterator first, Iterator last)
{
	for  (Iterator current = first; current != last; ++current)
		if (*last < *current)
		{
			std::rotate(current, last, next(last));
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
			std::rotate(current, last, next(last));
			return current;
		}

	return last;
}

#ifndef LEAN0X_NO_RVALUE_REFERENCES

/// Pushes the given element into the given sorted vector.
template <class Vector, class Value>
inline typename Vector::iterator push_sorted(Vector &vector, Value &&value)
{
	vector.push_back( std::forward<Value>(value) );
	return insert_last( vector.begin(), prev(vector.end()) );
}

/// Pushes the given element into the given sorted vector.
template <class Vector, class Value, class Predicate>
inline typename Vector::iterator push_sorted(Vector &vector, Value &&value, Predicate &&predicate)
{
	vector.push_back( std::forward<Value>(value) );
	return insert_last( vector.begin(), prev(vector.end()), std::forward<Predicate>(predicate) );
}

#else

/// Pushes the given element into the given sorted vector.
template <class Vector, class Value>
inline typename Vector::iterator push_sorted(Vector &vector, const Value &value)
{
	vector.push_back( value );
	return insert_last( vector.begin(), prev(vector.end()) );
}

/// Pushes the given element into the given sorted vector.
template <class Vector, class Value, class Predicate>
inline typename Vector::iterator push_sorted(Vector &vector, const Value &value, Predicate predicate)
{
	vector.push_back( value );
	return insert_last( vector.begin(), prev(vector.end()), predicate );
}

#endif

/// Removes the given element from the given vector.
template <class Vector, class Value>
inline void remove(Vector &vector, const Value &value)
{
	vector.erase(
			std::remove(vector.begin(), vector.end(), value),
			vector.end()
		);
}

/// Removes the given element from the given vector.
template <class Vector, class Value>
inline void remove_ordered(Vector &vector, const Value &value)
{
	typename Vector::const_iterator it = vector.begin();

	while (it != vector.end())
	{
		if (*it == value)
			it = vector.erase(it);
		else
			++it;
	}
}

} // namespace

using functional::insert_last;
using functional::push_sorted;
using functional::remove;
using functional::remove_ordered;

} // namespace

#endif