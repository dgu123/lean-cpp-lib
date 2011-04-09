#include "stdafx.h"
#include <lean/strings/strings.h>
#include <iostream>

BOOST_AUTO_TEST_SUITE( nullterminated )

BOOST_AUTO_TEST_CASE( casts )
{
	std::string test("hello world");
	lean::utf8_nti half_range(test);
	lean::utf8_ntri range(test);
	range = "hello cstring";
	half_range = "hello cstring";
	range = half_range;
	half_range = range;
	std::cout << half_range.c_str()<< std::endl;
	std::cout << static_cast<std::string>(range) << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
