/*****************************************************/
/* lean Properties              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PROPERTIES_PROPERTY_SERIALIZATION
#define LEAN_PROPERTIES_PROPERTY_SERIALIZATION

#include "../lean.h"
#include "../strings/types.h"
#include "../strings/conversions.h"
#include "../rapidxml/rapidxml.hpp"
#include "property.h"
#include <sstream>

namespace lean
{
namespace properties
{

/// Constructs an XML node from the given property.
template <class Class, class Property>
inline rapidxml::xml_node<utf8_t>* property_to_xml(const Class &object, const Property &desc, rapidxml::xml_document<utf8_t> &document)
{
	const utf8_t *value = nullptr;

	// Check if readable
	if (desc.getter.valid())
	{
		static const size_t optBufferSize = 1024;
		
		size_t max_length = desc.type->max_length(desc.count);

		// Take optimized route, if available
		if (max_length != 0 && max_length < optBufferSize)
		{
			utf8_t buffer[optBufferSize];
			utf8_t *bufferEnd = desc.type->write(buffer, object, desc.getter, desc.count);

			// Required to be null-terminated
			*(bufferEnd++) = 0;

			size_t length = bufferEnd - buffer;
			LEAN_ASSERT(length < optBufferSize);
			
			value = document.allocate_string(buffer, length);
		}
		// Take generic route, otherwise
		else
		{
			std::basic_ostringstream<utf8_t> stream;
			desc.type->write(stream, object, desc.getter, desc.count);
			value = document.allocate_string(stream.str().c_str());
		}
	}

	rapidxml::xml_node<utf8_t> *node = document.allocate_node(rapidxml::node_element, "property", value);
	node->append_attribute(
		document.allocate_attribute("name",
			document.allocate_string(utf_to_utf8(desc.name).c_str()) ) );

	return node;
}

/// Writes the given property collection to the given XML node.
template <class Class, class Collection>
inline void properties_to_xml(const Class &object, const Collection &collection, rapidxml::xml_node<utf8_t> &node)
{
	rapidxml::xml_document<utf8_t> &document = *node.document();

	for (typename Collection::const_iterator itProperty = collection.begin();
		itProperty != collection.end(); ++itProperty)
		node.append_node( property_to_xml(object, *itProperty, document) );
}

/// Loads property values from the given XML node.
template <class Class, class Property>
inline void property_from_xml(Class &object, const Property &desc, const rapidxml::xml_node<utf8_t> &propertyNode)
{
	// Check if writeable
	if (desc.setter.valid())
		desc.type->read(propertyNode.value(), propertyNode.value() + propertyNode.value_size(),
			object, desc.setter, desc.count);
}

namespace impl
{
	/// Gets the next (circular) sibling property node until a certain node is reached.
	inline const rapidxml::xml_node<utf8_t>* next_property_node(const rapidxml::xml_node<utf8_t> *node, const rapidxml::xml_node<utf8_t> *endNode)
	{
		node = node->next_sibling(node->name());

		if (!node)
			node = endNode->parent()->first_node(endNode->name());

		return (node != endNode)
			? node
			: nullptr;
	}

	/// Locates a property node of the given name and reads from it.
	template <class Class, class Property>
	inline const rapidxml::xml_node<utf8_t>* find_and_load_property(Class &object, const Property &desc,
		const utf8_ntri &name, const rapidxml::xml_node<utf8_t> *startNode)
	{
		// Iterate once in a circle starting at the given start node
		for (const rapidxml::xml_node<utf8_t> *propertyNode = startNode;
			propertyNode; propertyNode = next_property_node(propertyNode, startNode))
		{
			const rapidxml::xml_attribute<utf8_t> *nameAttr = propertyNode->first_attribute("name");

			if (nameAttr && utf8_nt(nameAttr->value()) == name)
			{
				property_from_xml(object, desc, *propertyNode);
				startNode = propertyNode->next_sibling(propertyNode->name());
				return (startNode) ? startNode : propertyNode;
			}
		}

		return startNode;
	}

}

/// Loads property values from the given XML node.
template <class Class, class Collection>
inline void properties_from_xml(Class &object, const Collection &collection, const rapidxml::xml_node<utf8_t> &node)
{
	const rapidxml::xml_node<utf8_t> *startNode = node.first_node("property");

	if (startNode)
		for (typename Collection::const_iterator itProperty = collection.begin();
			 itProperty != collection.end(); ++itProperty)
			 // Optimized for sequential storage layout (as saved by properties_to_xml)
			startNode = impl::find_and_load_property(object, *itProperty, utf_to_utf8(itProperty->name), startNode);
}

} // namespace

using properties::property_to_xml;
using properties::properties_to_xml;

using properties::property_from_xml;
using properties::properties_from_xml;

} // namespace

#endif