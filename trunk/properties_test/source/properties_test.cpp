#include "stdafx.h"
#include <lean/properties/property_accessors.h>
#include <lean/properties/property_collection.h>

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

		<< collection_type::property_desc(L"name", typeid(std::string), 1)
			.set_setter(LEAN_MAKE_PROPERTY_SETTER(&test_property_driven::setName))
			.set_getter(LEAN_MAKE_PROPERTY_GETTER(&test_property_driven::getName))

		<< collection_type::property_desc(L"id", typeid(int), 1)
			.set_getter(LEAN_MAKE_PROPERTY_GETTER(&test_property_driven::getID))

		<< collection_type::property_desc(L"weight", typeid(float), 1)
			.set_setter(LEAN_MAKE_PROPERTY_SETTER(&test_property_driven::setWeight))
			.set_getter(LEAN_MAKE_PROPERTY_GETTER(&test_property_driven::getWeight));
}

BOOST_AUTO_TEST_SUITE_END()
