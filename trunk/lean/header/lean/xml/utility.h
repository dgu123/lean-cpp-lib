/*****************************************************/
/* lean XML                     (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_XML_UTILITY
#define LEAN_XML_UTILITY

#include "../lean.h"
#include "../strings/types.h"
#include "../rapidxml/rapidxml.hpp"

namespace lean
{
namespace xml
{

namespace impl
{
	/// Allocates an XML string from the given document.
	template <class Char, class Traits>
	LEAN_INLINE Char* allocate_string(rapidxml::xml_document<Char> &document, const nullterminated_range_implicit<Char, Traits> &string)
	{
		return document.allocate_string(string.c_str(), string.size() + 1);
	}
}
/// Allocates an XML string from the given document.
template <class Char>
LEAN_INLINE Char* allocate_string(rapidxml::xml_document<Char> &document, const nullterminated_range_implicit< Char, char_traits<Char> > &string)
{
	return impl::allocate_string(document, string);
}
using impl::allocate_string;

namespace impl
{
	/// Allocates an XML attribute from the given document.
	template <class Char, class Traits>
	LEAN_INLINE rapidxml::xml_attribute<Char>* allocate_attribute(rapidxml::xml_document<Char> &document,
		const nullterminated_range_implicit<Char, Traits> &name, const nullterminated_range_implicit<Char, Traits> &value)
	{
		return document.allocate_attribute(
			allocate_string(document, name),
			allocate_string(document, value) );
	}
}
/// Allocates an XML attribute from the given document.
template <class Char>
LEAN_INLINE rapidxml::xml_attribute<Char>* allocate_attribute(rapidxml::xml_document<Char> &document,
	const nullterminated_range_implicit< Char, char_traits<Char> > &name, const nullterminated_range_implicit< Char, char_traits<Char> > &value)
{
	return impl::allocate_attribute(document, name, value);
}
using impl::allocate_attribute;

namespace impl
{
	/// Appends an XML attribute to the given node.
	template <class Char, class Traits>
	LEAN_INLINE void append_attribute(rapidxml::xml_document<Char> &document, rapidxml::xml_node<Char> &node,
		const nullterminated_range_implicit<Char, Traits> &name, const nullterminated_range_implicit<Char, Traits> &value)
	{
		node.append_attribute( allocate_attribute(document, name, value) );
	}
}
/// Appends an XML attribute to the given node.
template <class Char>
LEAN_INLINE void append_attribute(rapidxml::xml_document<Char> &document, rapidxml::xml_node<Char> &node,
	const nullterminated_range_implicit< Char, char_traits<Char> > &name, const nullterminated_range_implicit< Char, char_traits<Char> > &value)
{
	impl::append_attribute(document, node, name, value);
}
using impl::append_attribute;

namespace impl
{
	/// Gets the value of the given XML attribute or empty string if not available.
	template <class Char, class Traits>
	LEAN_INLINE std::basic_string<Char> get_attribute(const rapidxml::xml_node<Char> &node, const nullterminated_range_implicit<Char, Traits> &name)
	{
		rapidxml::xml_attribute<utf8_t> *att = node.first_attribute(name.c_str(), name.size());
		return (att)
			? std::basic_string<Char>(att->value(), att->value_size())
			: std::basic_string<Char>();
	}
}
/// Gets the value of the given XML attribute or empty string if not available.
template <class Char>
LEAN_INLINE std::basic_string<Char> get_attribute(const rapidxml::xml_node<Char> &node, const nullterminated_range_implicit< Char, char_traits<Char> > &name)
{
	return impl::get_attribute(node, name);
}
using impl::get_attribute;

namespace impl
{
	/// Assigns the value of the given XML attribute to the given variable, if available, otherwise leaves the variable untouched.
	template <class Char, class Traits, class StringTraits, class StringAlloc>
	LEAN_INLINE void get_attribute(const rapidxml::xml_node<Char> &node, const nullterminated_range_implicit<Char, Traits> &name,
		std::basic_string<Char, StringTraits, StringAlloc> &value)
	{
		rapidxml::xml_attribute<utf8_t> *att = node.first_attribute(name.c_str(), name.size());

		if (att)
			value.assign(att->value(), att->value_size());
	}
}
/// Assigns the value of the given XML attribute to the given variable, if available, otherwise leaves the variable untouched.
template <class Char, class StringTraits, class StringAlloc>
LEAN_INLINE void get_attribute(const rapidxml::xml_node<Char> &node, const nullterminated_range_implicit< Char, char_traits<Char> > &name,
	std::basic_string<Char, StringTraits, StringAlloc> &value)
{
	impl::get_attribute(node, name, value);
}
using impl::get_attribute;

namespace impl
{
	/// Allocates an XML node from the given document.
	template <class Char, class Traits>
	LEAN_INLINE rapidxml::xml_node<Char>* allocate_node(rapidxml::xml_document<Char> &document,
		const nullterminated_range_implicit<Char, Traits> &name)
	{
		return document.allocate_node(
			rapidxml::node_element,
			allocate_string(document, name) );
	}
}
/// Allocates an XML node from the given document.
template <class Char>
LEAN_INLINE rapidxml::xml_node<Char>* allocate_node(rapidxml::xml_document<Char> &document,
	const nullterminated_range_implicit< Char, char_traits<Char> > &name)
{
	return impl::allocate_node(document, name);
}
using impl::allocate_node;

namespace impl
{
	/// Allocates an XML node from the given document.
	template <class Char, class Traits>
	LEAN_INLINE rapidxml::xml_node<Char>* allocate_node(rapidxml::xml_document<Char> &document,
		const nullterminated_range_implicit<Char, Traits> &name, const nullterminated_range_implicit<Char, Traits> &value)
	{
		return document.allocate_node(
			rapidxml::node_element,
			allocate_string(document, name),
			allocate_string(document, value) );
	}
}
/// Allocates an XML node from the given document.
template <class Char>
LEAN_INLINE rapidxml::xml_node<Char>* allocate_node(rapidxml::xml_document<Char> &document,
	const nullterminated_range_implicit< Char, char_traits<Char> > &name, const nullterminated_range_implicit< Char, char_traits<Char> > &value)
{
	return impl::allocate_node(document, name, value);
}
using impl::allocate_node;

} // namespace

using xml::allocate_string;
using xml::allocate_attribute;
using xml::append_attribute;
using xml::get_attribute;
using xml::allocate_node;

} // namespace

#endif