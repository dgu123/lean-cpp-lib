#include "stdafx.h"
#include <lean/properties/property_accessors.h>

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

BOOST_AUTO_TEST_SUITE_END()
