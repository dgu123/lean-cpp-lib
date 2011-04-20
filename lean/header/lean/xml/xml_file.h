/*****************************************************/
/* lean XML                     (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_XML_XML_FILE
#define LEAN_XML_XML_FILE

#include "../lean.h"
#include "../strings/types.h"
#include "../meta/constexpr.h"
#include "../io/raw_file.h"
#include "../io/raw_file_inserter.h"
#include "../rapidxml/rapidxml.hpp"
#include "../rapidxml/rapidxml_print.hpp"

namespace lean
{
namespace xml
{

namespace impl
{
	/// Reads the whole contents of the given xml file into a pool-allocated block of memory.
	template <class Char>
	inline char* load_xml_source(const utf8_ntri &fileName, rapidxml::xml_document<Char> &document)
	{
		raw_file file(fileName, file::read);
		
		size_t fileSize = file.size();
		char *source = document.allocate_string(nullptr, fileSize);
		
		file.read(source, fileSize); // TODO: strict (exception on error)
		return source;
	}
}

/// Loads an xml document from the given file.
template <int ParseFlags, class Char>
LEAN_INLINE void load_xml_file(const utf8_ntri &fileName, rapidxml::xml_document<Char> &document)
{
	document.parse<ParseFlags>(
		impl::load_xml_source(fileName, document) );
}

/// Saves an xml document to the given file.
template <int PrintFlags, class Char>
LEAN_INLINE void save_xml_file(const utf8_ntri &fileName, const rapidxml::xml_node<Char> &document)
{
	raw_file file(fileName, file::write, file::overwrite);
	print(raw_file_inserter<>(file).iter(), document, PrintFlags);
}

/// This convenience class wraps up the most common xml file functionality.
template <class Char = char>
class xml_file
{
private:
	rapidxml::xml_document<Char> m_document;

public:
	/// Constructs an empty xml document.
	LEAN_INLINE xml_file() { }
	/// Loads an xml document from the given file.
	LEAN_INLINE explicit xml_file(const utf8_ntri &name)
	{
		load_xml_file<rapidxml::parse_trim_whitespace | rapidxml::parse_normalize_whitespace>(name, m_document);
	}
	/// Loads an xml document from the given file.
	template <int ParseFlags>
	LEAN_INLINE xml_file(const utf8_ntri &name, ce_int<ParseFlags>)
	{
		load_xml_file<ParseFlags>(name, m_document);
	}

	/// Saves this xml document to the given file.
	LEAN_INLINE void save(const utf8_ntri &name) const
	{
		save_xml_file<0>(name, m_document);
	}
	/// Saves this xml document to the given file.
	template <int ParseFlags>
	LEAN_INLINE void save(const utf8_ntri &name, ce_int<ParseFlags>) const
	{
		save_xml_file<PrintFlags>(name, m_document);
	}

	/// Gets the contained xml document.
	LEAN_INLINE rapidxml::xml_document<Char>& document() { return m_document; }
	/// Gets the contained xml document.
	LEAN_INLINE const rapidxml::xml_document<Char>& document() const { return m_document; }
};

} // namespace

using xml::xml_file;

} // namespace

#endif