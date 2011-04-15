#include "stdafx.h"
#include <lean/io/raw_file.h>

BOOST_AUTO_TEST_SUITE( nullterminated )

BOOST_AUTO_TEST_CASE( overwrite )
{
	lean::raw_file file(MAKE_TEST_FILENAME("test1.txt"), lean::file::readwrite, lean::file::overwrite);

	BOOST_CHECK_EQUAL(file.name().c_str(), MAKE_TEST_FILENAME("test1.txt"));
	BOOST_CHECK_EQUAL(file.size(), 0);

	file.write("test", sizeof("test"));

	BOOST_CHECK_EQUAL(file.size(), sizeof("test"));

	file.resize(256);

	BOOST_CHECK_EQUAL(file.size(), 256);

	file.pos(0);

	BOOST_CHECK_EQUAL(file.pos(), 0);

	char buffer[sizeof("test")];
	file.read(buffer, sizeof("test"));

	BOOST_CHECK_EQUAL(buffer, "test");
}

BOOST_AUTO_TEST_CASE( sharing )
{
	lean::raw_file file(MAKE_TEST_FILENAME("test2.txt"));

	BOOST_CHECK_THROW(lean::raw_file(MAKE_TEST_FILENAME("test2.txt")), std::runtime_error);

	file.write("test", sizeof("test"));

	lean::raw_file readFile(MAKE_TEST_FILENAME("test2.txt"), lean::file::read);
	
	char buffer[sizeof("test")];
	readFile.read(buffer, sizeof("test"));

	BOOST_CHECK_EQUAL(buffer, "test");
}

BOOST_AUTO_TEST_SUITE_END()
