/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_CONSTRUCTION
#define LEAN_CONTAINERS_CONSTRUCTION

#include "../lean.h"
#include <memory>

namespace lean 
{
namespace containers
{

struct nontrivial_construction_t { };
struct trivial_construction_t { };

/// Destructs the elements in the given range.
template <class Element, class Allocator>
LEAN_INLINE void destruct(Element *destr, Allocator &allocator, nontrivial_construction_t = nontrivial_construction_t())
{
	allocator.destroy(destr);
}
template <class Element, class Allocator>
LEAN_INLINE void destruct(Element *destr, Allocator &allocator, trivial_construction_t) { }
/// Destructs the elements in the given range.
template <class Element, class Allocator>
inline void destruct(Element *destr, Element *destrEnd, Allocator &allocator, nontrivial_construction_t = nontrivial_construction_t())
{
	while (destr < destrEnd)
		destruct(--destrEnd, allocator);
}
template <class Element, class Allocator>
LEAN_INLINE void destruct(Element *destr, Element *destrEnd, Allocator &allocator, trivial_construction_t) { }

/// Default constructs an element at the given location.
template <class Element, class Allocator>
LEAN_INLINE void default_construct(Element *dest, Allocator &allocator)
{
	allocator.construct(dest, Element());
}
/// Default constructs elements in the given range.
template <class Element, class Allocator>
inline void default_construct(Element *dest, Element *destEnd, Allocator &allocator)
{
	Element *destr = dest;

	try
	{
		for (; dest < destEnd; ++dest)
			default_construct(dest, allocator);
	}
	catch (...)
	{
		destruct(destr, dest, allocator);
		throw;
	}
}

/// Copies the given source element to the given destination.
template <class Element, class Allocator>
LEAN_INLINE void copy_construct(Element *dest, const Element &source, Allocator &allocator, nontrivial_construction_t = nontrivial_construction_t())
{
	allocator.construct(dest, source);
}
template <class Element, class Allocator>
LEAN_INLINE void copy_construct(Element *dest, const Element &source, Allocator &allocator, trivial_construction_t)
{
	memcpy(dest, addressof(source), sizeof(Element));
}
/// Copies elements from the given source range to the given destination.
template <class Iterator, class Element, class Allocator>
inline void copy_construct(Iterator source, Iterator sourceEnd, Element *dest, Allocator &allocator, nontrivial_construction_t = nontrivial_construction_t())
{
	Element *destr = dest;

	try
	{
		for (; source != sourceEnd; ++dest, ++source)
			copy_construct(dest, *source, allocator);
	}
	catch (...)
	{
		destruct(destr, dest, allocator);
		throw;
	}
}
template <class Iterator, class Element, class Allocator>
LEAN_INLINE void copy_construct(Iterator source, Iterator sourceEnd, Element *dest, Allocator &allocator, trivial_construction_t)
{
	memcpy(dest, addressof(*source), (sourceEnd - source) * sizeof(Element));
}

/// Moves the given source element to the given destination.
template <class Element, class Allocator>
LEAN_INLINE void move_construct(Element *dest, Element &source, Allocator &allocator, nontrivial_construction_t = nontrivial_construction_t())
{
	allocator.construct(dest, LEAN_MOVE(source));
}
template <class Element, class Allocator>
LEAN_INLINE void move_construct(Element *dest, Element &source, Allocator &allocator, trivial_construction_t)
{
	copy_construct(dest, source, allocator, trivial_construction_t());
}
/// Moves elements from the given source range to the given destination.
template <class Iterator, class Element, class Allocator>
inline void move_construct(Iterator source, Iterator sourceEnd, Element *dest, Allocator &allocator, nontrivial_construction_t = nontrivial_construction_t())
{
	Element *destr = dest;

	try
	{
		for (; source != sourceEnd; ++dest, ++source)
			move_construct(dest, *source, allocator);
	}
	catch (...)
	{
		destruct(destr, dest, allocator);
		throw;
	}
}
template <class Iterator, class Element, class Allocator>
LEAN_INLINE void move_construct(Iterator source, Iterator sourceEnd, Element *dest, Allocator &allocator, trivial_construction_t)
{
	copy_construct(source, sourceEnd, dest, allocator, trivial_construction_t());
}

/// Moves the given source element to the given destination.
template <class Element>
LEAN_INLINE void move(Element *dest, Element &source, nontrivial_construction_t = nontrivial_construction_t())
{
	*dest = LEAN_MOVE(source);
}
template <class Element>
LEAN_INLINE void move(Element *dest, Element &source, trivial_construction_t)
{
	memcpy(dest, addressof(source), sizeof(Element));
}
/// Moves elements from the given source range to the given destination.
template <class Iterator, class Element>
inline void move(Iterator source, Iterator sourceEnd, Element *dest, nontrivial_construction_t = nontrivial_construction_t())
{
	for (; source != sourceEnd; ++dest, ++source)
		move(dest, *source);
}
template <class Iterator, class Element>
LEAN_INLINE void move(Iterator source, Iterator sourceEnd, Element *dest, trivial_construction_t)
{
	memcpy(dest, addressof(*source), (sourceEnd - source) * sizeof(Element));
}
/// Moves elements from the given source range to the given destination.
template <class Iterator, class Element>
inline void move_backwards(Iterator source, Iterator sourceEnd, Element *destEnd, nontrivial_construction_t = nontrivial_construction_t())
{
	while (source != sourceEnd)
		move(--destEnd, *--sourceEnd);
}
template <class Iterator, class Element>
LEAN_INLINE void move_backwards(Iterator source, Iterator sourceEnd, Element *destEnd, trivial_construction_t)
{
	size_t count = (sourceEnd - source);
	memmove(destEnd - count, addressof(*source), count * sizeof(Element));
}

} // namespace

} // namespace

#endif