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

template <class Tag>
struct is_trivial_construction { static const bool value = false; };
template <>
struct is_trivial_construction<trivial_construction_t> { static const bool value = true; };

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
inline Element* copy_construct(Iterator source, Iterator sourceEnd, Element *dest, Allocator &allocator, nontrivial_construction_t = nontrivial_construction_t())
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

	return dest;
}
template <class Iterator, class Element, class Allocator>
LEAN_INLINE Element* copy_construct(Iterator source, Iterator sourceEnd, Element *dest, Allocator &allocator, trivial_construction_t)
{
	size_t count = sourceEnd - source;
	memcpy(dest, addressof(*source), count * sizeof(Element));
	return dest + count;
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
inline Element* move_construct(Iterator source, Iterator sourceEnd, Element *dest, Allocator &allocator, nontrivial_construction_t = nontrivial_construction_t())
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

	return dest;
}
template <class Iterator, class Element, class Allocator>
LEAN_INLINE Element* move_construct(Iterator source, Iterator sourceEnd, Element *dest, Allocator &allocator, trivial_construction_t)
{
	return copy_construct(source, sourceEnd, dest, allocator, trivial_construction_t());
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
inline Element* move(Iterator source, Iterator sourceEnd, Element *dest, nontrivial_construction_t = nontrivial_construction_t())
{
	for (; source != sourceEnd; ++dest, ++source)
		move(dest, *source);

	return dest;
}
template <class Iterator, class Element>
LEAN_INLINE Element* move(Iterator source, Iterator sourceEnd, Element *dest, trivial_construction_t)
{
	size_t count = sourceEnd - source;
	memmove(dest, addressof(*source), count * sizeof(Element));
	return dest + count;
}
/// Moves elements from the given source range to the given destination.
template <class Iterator, class Element>
inline Element* move_backwards(Iterator source, Iterator sourceEnd, Element *dest, nontrivial_construction_t = nontrivial_construction_t())
{
	Element *destEnd = dest + (sourceEnd - source);

	for (dest = destEnd; source != sourceEnd; )
		move(--dest, *--sourceEnd);

	return destEnd;
}
template <class Iterator, class Element>
LEAN_INLINE Element* move_backwards(Iterator source, Iterator sourceEnd, Element *dest, trivial_construction_t)
{
	size_t count = (sourceEnd - source);
	memmove(dest, addressof(*source), count * sizeof(Element));
	return dest + count;
}

/// Opens a gap of elements, returning the new end element.
template <class Element, class Allocator, class DestructTag>
LEAN_INLINE void open(Element *gap, Element *gapEnd, Element *&end, Allocator &allocator, trivial_construction_t moveTag, DestructTag destructTag)
{
	LEAN_ASSERT(gap <= gapEnd);
	LEAN_ASSERT(gap <= end);

	end = move_backwards(gap, end, gapEnd, moveTag);
}
/// Opens a gap of elements, returning the new end element via the given end reference.
template <class Element, class Allocator, class DestructTag>
inline void open(Element *gap, Element *gapEnd, Element *&end, Allocator &allocator, nontrivial_construction_t moveTag, DestructTag destructTag)
{
	LEAN_ASSERT(gap <= gapEnd);
	LEAN_ASSERT(gap <= end);

	size_t gapWidth = gapEnd - gap;
	size_t oldCount = end - gap;
	Element *oldEnd = end;
	Element *newEnd = end + gapWidth;
	
	size_t constructCount = min(gapWidth, oldCount);
	end = move_construct(oldEnd - constructCount, oldEnd, newEnd - constructCount, allocator, moveTag);
	
	move_backwards(gap, oldEnd - constructCount, gapEnd, moveTag);

	destruct(gap, gap + constructCount, allocator, destructTag);
}

/// Closes a gap of elements, returning the new end element via the given end reference.
template <class Element, class Allocator, class MoveTag, class DestructTag>
inline void close(Element *gap, Element *gapEnd, Element *&end, Allocator &allocator, MoveTag moveTag, DestructTag destructTag)
{
	LEAN_ASSERT(gap <= gapEnd);
	LEAN_ASSERT(gapEnd <= end);

	if (is_trivial_construction<MoveTag>::value)
		destruct(gap, gapEnd, allocator, destructTag);

	Element *oldEnd = end;
	end = move(gapEnd, oldEnd, gap, moveTag);
	
	if (!is_trivial_construction<MoveTag>::value)
		destruct(end, oldEnd, allocator, destructTag);
}

} // namespace

} // namespace

#endif