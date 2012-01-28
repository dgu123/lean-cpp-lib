/*****************************************************/
/* lean XML                     (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_XML_UTILITY
#define LEAN_XML_UTILITY

#include "../lean.h"
#include "../strings/types.h"
#include "../rapidxml/rapidxml.hpp"
#include "../meta/strip.h"

namespace lean
{
namespace xml
{

namespace impl
{
	template <class Char, class Traits>
	LEAN_INLINE Char* allocate_string(rapidxml::xml_document<Char> &document, const nullterminated_range_implicit<Char, Traits> &string)
	{
		return document.allocate_string(string.c_str(), string.size() + 1);
	}
}
using impl::allocate_string;

/// Allocates an XML string from the given document.
template <class Char, class Range>
LEAN_INLINE Char* allocate_string(rapidxml::xml_document<Char> &document, const Range &string)
{
	// impl namespace required to avoid recursion
	return impl::allocate_string(document, make_ntr<Char>(string));
}

/// Allocates an XML attribute from the given document.
template <class Char, class Range1, class Rage2>
LEAN_INLINE rapidxml::xml_attribute<Char>* allocate_attribute(rapidxml::xml_document<Char> &document, const Range1 &name, const Rage2 &value)
{
	return document.allocate_attribute(
			allocate_string(document, name),
			allocate_string(document, value)
		);
}

/// Appends an XML attribute to the given node.
template <class Char, class Range1, class Rage2>
LEAN_INLINE void append_attribute(rapidxml::xml_document<Char> &document, rapidxml::xml_node<Char> &node, const Range1 &name, const Rage2 &value)
{
	node.append_attribute( allocate_attribute(document, name, value) );
}

namespace impl
{
	template <class Char, class Traits>
	LEAN_INLINE nullterminated_range<Char, Traits> get_attribute(const rapidxml::xml_node<Char> &node, const nullterminated_range_implicit<Char, Traits> &name)
	{
		rapidxml::xml_attribute<utf8_t> *att = node.first_attribute(name.c_str(), name.size());
		return (att)
			? nullterminated_range<Char, Traits>(att->value(), att->value() + att->value_size())
			: nullterminated_range<Char, Traits>("");
	}
}
using impl::get_attribute;

/// Gets the value of the given XML attribute or empty string if not available.
template <class Char, class Range>
LEAN_INLINE nullterminated_range<Char> get_attribute(const rapidxml::xml_node<Char> &node, const Range &name)
{
	// impl namespace required to avoid recursion
	return impl::get_attribute(node, make_ntr<Char>(name));
}

/// Gets the value of the given XML attribute or empty string if not available.
template <class Char, class Traits, class Range>
LEAN_INLINE nullterminated_range<Char, Traits> get_attribute(const rapidxml::xml_node<Char> &node, const Range &name)
{
	// impl namespace required to avoid recursion
	return impl::get_attribute(node, make_ntr<Char, Traits>(name));
}

namespace impl
{
	template <class Char, class Traits, class StringTraits, class StringAlloc>
	LEAN_INLINE void get_attribute(const rapidxml::xml_node<Char> &node,
		const nullterminated_range_implicit<Char, Traits> &name,
		std::basic_string<Char, StringTraits, StringAlloc> &value)
	{
		rapidxml::xml_attribute<utf8_t> *att = node.first_attribute(name.c_str(), name.size());

		if (att)
			value.assign(att->value(), att->value_size());
	}
}
using impl::get_attribute;

/// Assigns the value of the given XML attribute to the given variable, if available, otherwise leaves the variable untouched.
template <class Char, class Range, class StringTraits, class StringAlloc>
LEAN_INLINE void get_attribute(const rapidxml::xml_node<Char> &node, const Range &name,
	std::basic_string<Char, StringTraits, StringAlloc> &value)
{
	// impl namespace required to avoid recursion
	impl::get_attribute(node, make_ntr<Char>(name), value);
}

/// Allocates an XML node from the given document.
template <class Char>
LEAN_INLINE rapidxml::xml_node<Char>* allocate_node(rapidxml::xml_document<Char> &document)
{
	return document.allocate_node(rapidxml::node_element);
}

/// Allocates an XML node from the given document.
template <class Char, class Range>
LEAN_INLINE rapidxml::xml_node<Char>* allocate_node(rapidxml::xml_document<Char> &document, const Range &name)
{
	return document.allocate_node(
			rapidxml::node_element,
			allocate_string(document, name)
		);
}

/// Allocates an XML node from the given document.
template <class Char, class Range1, class Range2>
LEAN_INLINE rapidxml::xml_node<Char>* allocate_node(rapidxml::xml_document<Char> &document, const Range1 &name, const Range2 &value)
{
	return document.allocate_node(
			rapidxml::node_element,
			allocate_string(document, name),
			allocate_string(document, value)
		);
}

} // namespace

using xml::allocate_string;
using xml::allocate_attribute;
using xml::append_attribute;
using xml::get_attribute;
using xml::allocate_node;

} // namespace

#endif