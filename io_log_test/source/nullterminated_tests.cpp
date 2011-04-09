#include "stdafx.h"
#include <lean/strings/strings.h>

BOOST_AUTO_TEST_SUITE( nullterminated )

BOOST_AUTO_TEST_CASE( casts )
{
	std::string test("hello world");
	lean::utf8_nt range(test);
	std::string test2 = range;
}

BOOST_AUTO_TEST_SUITE_END()
