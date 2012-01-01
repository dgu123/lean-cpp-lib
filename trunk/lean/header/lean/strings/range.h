/*****************************************************/
/* lean Strings                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_STRINGS_RANGE
#define LEAN_STRINGS_RANGE

#include "../lean.h"
#include "../meta/strip.h"
#include "../meta/type_traits.h"
#include "../meta/enable_if.h"

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

	/// Constructs an empty iterator range.
	LEAN_INLINE range()
		: m_begin(), m_end() { }
	/// Constructs an iterator range.
	LEAN_INLINE range(iterator begin, iterator end)
		: m_begin(begin), m_end(end) { }

	/// Assigns the given iterators to this range.
	LEAN_INLINE void assign(iterator begin, iterator end)
	{
		m_begin = begin;
		m_end = end;
	}

	/// Gets whether this range is empty.
	LEAN_INLINE bool empty() const { return (m_begin == m_end); }
	/// Gets the size of this range (only valid for random access iterators).
	LEAN_INLINE size_t size() const { return m_end - m_begin; }

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

/// Makes a range from the given null-terminated charcter string.
template <class Char>
LEAN_INLINE range<Char*> make_char_range(Char *nts)
{
	return range<Char*>(
		nts,
		nts + std::char_traits<typename strip_modifiers<Char>::type>::length(nts) );
}
/// Returns an unmodified reference to the given range. (generic convenience overload)
template <class Range>
LEAN_INLINE Range& make_char_range(Range &range) { return range; }


/// Constructs an object of the given type from the given range.
template <class Class, class Range>
LEAN_INLINE Class from_range(const Range &range)
{
	return Class(range.begin(), range.end());
}

template <class String>
struct string_traits;

/// Constructs an object of the given type from the given range.
template <class String, class Range>
LEAN_INLINE String string_from_range(const Range &range)
{
	return string_traits<String>::construct(range.begin(), range.end());
}

namespace impl
{
	LEAN_DEFINE_HAS_TYPE(iterator);
}

/// Checks if the given type is a range type.
template <class Type>
struct is_range
{
	/// True, if Type is a range type.
	static const bool value = impl::has_type_iterator<Type>::value;
};

/// Redefines the given type if Range is a range, empty otherwise.
template <class Range, class Type>
struct enable_if_range : public enable_if<is_range<Range>::value, Type> { };
/// Redefines the given type if Range is not a range, empty otherwise.
template <class Range, class Type>
struct enable_if_not_range : public enable_if<!is_range<Range>::value, Type> { };
/// Redefines the given type if both Range1 and Range2 are ranges, empty otherwise.
template <class Range1, class Range2, class Type>
struct enable_if_range2 : public enable_if<is_range<Range1>::value && is_range<Range2>::value, Type> { };
/// Redefines the given type if either Range1 or Range2 is not a range, empty otherwise.
template <class Range1, class Range2, class Type>
struct enable_if_not_range2 : public enable_if<!is_range<Range1>::value || !is_range<Range2>::value, Type> { };

/// Gets the character type for the given null-terminated array of character.
template <class Chars1>
struct range_char_type;
template <class Char>
struct range_char_type<Char*> { typedef typename strip_modifiers<Char>::type type; };

/// Gets the character type for the given null-terminated array of character.
template <class Chars1, class Chars2>
struct range_char_type2;
template <class Char, class Chars2>
struct range_char_type2<Char*, Chars2> { typedef typename strip_modifiers<Char>::type type; };
template <class Chars1, class Char>
struct range_char_type2<Chars1, Char*> { typedef typename strip_modifiers<Char>::type type; };
template <class Char1, class Char2>
struct range_char_type2<Char1*, Char2*> { typedef typename strip_modifiers<Char1>::type type; };

} // namespace

using strings::range;
using strings::make_range;
using strings::make_char_range;
using strings::from_range;
using strings::string_from_range;
using strings::is_range;
using strings::enable_if_range;
using strings::enable_if_not_range;
using strings::enable_if_range2;
using strings::enable_if_not_range2;
using strings::range_char_type;
using strings::range_char_type2;

} // namespace

#endif