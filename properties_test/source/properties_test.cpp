#include "stdafx.h"
#include <lean/properties/property_accessors.h>
#include <lean/properties/property_types.h>
#include <lean/properties/property_collection.h>
#include <lean/properties/property_serialization.h>
#include <lean/xml/xml_file.h>

#include <lean/time/highres_timer.h>

#include "property_driven_test_impl.h"

BOOST_AUTO_TEST_SUITE( property )

BOOST_AUTO_TEST_CASE( c_setter )
{
	test_property_driven object("test", 1);

	lean::properties::property_c_setter<test_property_driven, float, 1, void,
		&test_property_driven::setWeight> test;

	lean::properties::property_n_setter<test_property_driven, int, size_t, void,
		&test_property_driven::setInts, long long> test2;
	test2(object, typeid(char), nullptr, 0);

	LEAN_MAKE_PROPERTY_SETTER(&test_property_driven::setInts);
}

BOOST_AUTO_TEST_CASE( inplace )
{
	typedef lean::property_collection< test_property_driven, lean::named_property_desc<test_property_driven> > collection_type;
	
	static collection_type collection = collection_type::construct_inplace()

		<< collection_type::property_desc(L"name", lean::get_generic_property_type<test_property_driven, std::string, 1>(), 1)
			.set_setter(LEAN_MAKE_PROPERTY_SETTER(&test_property_driven::setName))
			.set_getter(LEAN_MAKE_PROPERTY_GETTER(&test_property_driven::getName))

		<< collection_type::property_desc(L"id", lean::get_int_property_type<test_property_driven, int, 1>(), 1)
			.set_getter(LEAN_MAKE_PROPERTY_GETTER(&test_property_driven::getID))

		<< collection_type::property_desc(L"weight", lean::get_float_property_type<test_property_driven, float, 1>(), 1)
			.set_setter(LEAN_MAKE_PROPERTY_SETTER(&test_property_driven::setWeight))
			.set_getter(LEAN_MAKE_PROPERTY_GETTER(&test_property_driven::getWeight))

		<< collection_type::property_desc(L"ints", lean::get_int_property_type<test_property_driven, int, 16>(), 16)
			.set_setter(LEAN_MAKE_PROPERTY_SETTER(&test_property_driven::setInts))
			.set_getter(LEAN_MAKE_PROPERTY_GETTER(&test_property_driven::getInts))

		<< collection_type::property_desc(L"floats", lean::get_float_property_type<test_property_driven, float, 16>(), 16)
			.set_setter(LEAN_MAKE_PROPERTY_SETTER(&test_property_driven::setFloats))
			.set_getter(LEAN_MAKE_PROPERTY_GETTER(&test_property_driven::getFloats));

	test_property_driven object("test", 1);
	object.setWeight(2.1521231561f);

	lean::xml_file<lean::utf8_t> file;
	rapidxml::xml_node<lean::utf8_t> *rootnode = file.document().allocate_node(rapidxml::node_element, "properties", nullptr);
	file.document().append_node(rootnode);
	
	lean::highres_timer write_timer;

	for (int i = 0; i < 10000; ++i)
		lean::properties_to_xml(object, collection, *rootnode);

	std::cout << "Properties_to_XML: " << write_timer.milliseconds() << std::endl;
	
	file.save("blabla.xml");

	test_property_driven object2("asfdas", 125);

	lean::highres_timer read_timer;

	lean::xml_file<lean::utf8_t> file2("blabla.xml");

	for (int i = 0; i < 10000; ++i)
		lean::properties_from_xml(object2, collection, *file2.document().first_node());

	std::cout << "Properties_from_XML: " << read_timer.milliseconds() << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
