/*****************************************************/
/* lean XML                     (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_XML_NUMERIC
#define LEAN_XML_NUMERIC

#include "../lean.h"
#include "../io/numeric.h"
#include "utility.h"

namespace lean
{
namespace xml
{

/// Appends an integer XML attribute to the given node.
template <class Char, class Integer, class Range>
inline void append_int_attribute(rapidxml::xml_document<Char> &document, rapidxml::xml_node<Char> &node, const Range &name, const Integer &value)
{
	Char numBuffer[max_int_string_length<Integer>::value + 1];
	
	Char *numEnd = int_to_char(numBuffer, value);
	*numEnd = 0;

	append_attribute( *node.document(), node, name, nullterminated_range<Char>(numBuffer, numEnd) );
}

/// Appends a floating-point XML attribute to the given node.
template <class Char, class Float, class Range>
inline void append_float_attribute(rapidxml::xml_document<Char> &document, rapidxml::xml_node<Char> &node, const Range &name, const Float &value)
{
	Char numBuffer[max_float_string_length<Float>::value + 1];
	
	Char *numEnd = float_to_char(numBuffer, value);
	*numEnd = 0;

	append_attribute( *node.document(), node, name, nullterminated_range<Char>(numBuffer, numEnd) );
}

/// Gets the integer value of the given XML attribute, returns the default value if not available.
template <class Char, class Integer, class Range>
inline Integer get_int_attribute(const rapidxml::xml_node<Char> &node, const Range &name, const Integer &defaultValue)
{
	Integer value(defaultValue);
	string_to_int( get_attribute(node, name), value );
	return value;
}

/// Gets the flooating-point value of the given XML attribute, returns the default value if not available.
template <class Char, class Float, class Range>
inline Float get_float_attribute(const rapidxml::xml_node<Char> &node, const Range &name, const Float &defaultValue)
{
	Float value(defaultValue);
	string_to_float( get_attribute(node, name), value );
	return value;
}

} // namespace

using xml::append_int_attribute;
using xml::append_float_attribute;
using xml::get_int_attribute;
using xml::get_float_attribute;

} // namespace

#endif