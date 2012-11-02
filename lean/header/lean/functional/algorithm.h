/*****************************************************/
/* lean Functional              (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
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

/// Compares the elements in the given ranges.
template <class Iterator1, class Iterator2>
inline bool equal(Iterator1 begin1, Iterator1 end1, Iterator2 begin2, Iterator2 end2)
{
	Iterator1 it1 = begin1;
	Iterator1 it2 = begin2;

	bool ended1 = (it1 == end1);
	bool ended2 = (it2 == end2);

	while (!ended1 && !ended2)
	{
		if (!(*it1 == *it2))
			return false;

		ended1 = (++it1 == end1);
		ended2 = (++it2 == end2);
	}

	return ended1 && ended2;
}

/// Compares the elements in the given ranges using the given predicate.
template <class Iterator1, class Iterator2, class Pred>
inline bool equal(Iterator1 begin1, Iterator1 end1, Iterator2 begin2, Iterator2 end2, Pred pred)
{
	Iterator1 it1 = begin1;
	Iterator1 it2 = begin2;

	bool ended1 = (it1 == end1);
	bool ended2 = (it2 == end2);

	while (!ended1 && !ended2)
	{
		if (!pred(*it1, *it2))
			return false;

		ended1 = (++it1 == end1);
		ended2 = (++it2 == end2);
	}

	return ended1 && ended2;
}

/// Compares the elements in the given ranges.
template <class Range1, class Range2>
inline bool equal(const Range1 &range1, const Range2 &range2)
{
	return equal(range1.begin(), range1.end(), range2.begin(), range2.end());
}

/// Compares the elements in the given ranges using the given predicate.
template <class Range1, class Range2, class Pred>
inline bool equal(const Range1 &range1, const Range2 &range2, Pred pred)
{
	return equal(range1.begin(), range1.end(), range2.begin(), range2.end(), pred);
}

/// Compares the elements in the given ranges.
template <class Range1, class Range2>
inline bool lexicographical_compare(const Range1 &range1, const Range2 &range2)
{
	return std::lexicographical_compare(range1.begin(), range1.end(), range2.begin(), range2.end());
}

/// Compares the elements in the given ranges using the given predicate.
template <class Range1, class Range2, class Pred>
inline bool lexicographical_compare(const Range1 &range1, const Range2 &range2, Pred pred)
{
	return std::lexicographical_compare(range1.begin(), range1.end(), range2.begin(), range2.end(), pred);
}

/// Inserts the element pointed at by <code>last</code> into the given sorted range <code>[first, last)</code>.
template <class Iterator>
inline Iterator insert_last(Iterator first, Iterator last)
{
	Iterator pos = std::upper_bound(first, last, *last);
	std::rotate(pos, last, next(last));
	return pos;
}

/// Inserts the element pointed at by <code>last</code> into the given sorted range <code>[first, last)</code>.
template <class Iterator, class Predicate>
inline Iterator insert_last(Iterator first, Iterator last, Predicate predicate)
{
	Iterator pos = std::upper_bound(first, last, *last, predicate);
	std::rotate(pos, last, next(last));
	return pos;
}

#ifndef LEAN0X_NO_RVALUE_REFERENCES

/// Pushes the given element onto the given vector.
template <class Vector, class Value>
inline typename Vector::iterator push_unique(Vector &vector, Value &&value)
{
	typename Vector::iterator pos = std::find( vector.begin(), vector.end(), value );

	if (pos == vector.end())
		pos = vector.insert( pos, std::forward<Value>(value) );

	return pos;
}

/// Pushes the given element into the given sorted vector.
template <class Vector, class Value>
inline typename Vector::iterator push_sorted(Vector &vector, Value &&value)
{
	typename Vector::iterator pos = std::upper_bound( vector.begin(), vector.end(), value );
	return vector.insert( pos, std::forward<Value>(value) );
}

/// Pushes the given element into the given sorted vector.
template <class Vector, class Value, class Predicate>
inline typename Vector::iterator push_sorted(Vector &vector, Value &&value, Predicate &&predicate)
{
	typename Vector::iterator pos = std::upper_bound( vector.begin(), vector.end(), value, std::forward<Predicate>(predicate) );
	return vector.insert( pos, std::forward<Value>(value) );
}

#else

/// Pushes the given element onto the given vector.
template <class Vector, class Value>
inline typename Vector::iterator push_unique(Vector &vector, const Value &value)
{
	typename Vector::iterator pos = std::find( vector.begin(), vector.end(), value );

	if (pos == vector.end())
		pos = vector.insert( pos, value );

	return pos;
}

/// Pushes the given element into the given sorted vector.
template <class Vector, class Value>
inline typename Vector::iterator push_sorted(Vector &vector, const Value &value)
{
	typename Vector::iterator pos = std::upper_bound( vector.begin(), vector.end(), value );
	return vector.insert( pos, value );
}

/// Pushes the given element into the given sorted vector.
template <class Vector, class Value, class Predicate>
inline typename Vector::iterator push_sorted(Vector &vector, const Value &value, Predicate predicate)
{
	typename Vector::iterator pos = std::upper_bound( vector.begin(), vector.end(), value, predicate );
	return vector.insert( pos, value );
}

#endif

/// Locates the position of the first occurence of the given element in the given sorted range.
template <class Iterator, class Value>
inline Iterator find_sorted(Iterator begin, Iterator end, const Value &value)
{
	Iterator element = std::lower_bound(begin, end, value);

	if (element != end && !(*element == value))
		element = end;

	return element;
}

/// Locates the position of the first occurence of the given element in the given sorted range.
template <class Iterator, class Value, class Ord, class Eq>
inline Iterator find_sorted(Iterator begin, Iterator end, const Value &value, Ord order, Eq equal)
{
	Iterator element = std::lower_bound(begin, end, value, order);

	if (element != end && !equal(*element, value))
		element = end;

	return element;
}

/// Removes the given element from the given vector.
template <class Vector, class Value>
inline bool remove(Vector &vector, const Value &value)
{
	bool bRemoved = false;

	typename Vector::iterator newEnd = std::remove(vector.begin(), vector.end(), value);
	bRemoved = (newEnd != vector.end());

	vector.erase(newEnd, vector.end());

	return bRemoved;
}

/// Removes the given element from the given vector.
template <class Vector, class Value>
inline bool remove_ordered(Vector &vector, const Value &value)
{
	bool bRemoved = false;

	typename Vector::const_iterator it = vector.begin();

	while (it != vector.end())
	{
		if (*it == value)
		{
			// Not quite optimal, assuming that elements mostly occur only once
			it = vector.erase(it);
			bRemoved = true;
		}
		else
			++it;
	}

	return bRemoved;
}

} // namespace

using functional::equal;
using functional::lexicographical_compare;
using functional::insert_last;
using functional::push_unique;
using functional::push_sorted;
using functional::find_sorted;
using functional::remove;
using functional::remove_ordered;

} // namespace

#endif