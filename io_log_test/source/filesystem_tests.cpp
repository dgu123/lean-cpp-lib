#include "stdafx.h"
#include <lean/io/filesystem.h>

BOOST_AUTO_TEST_SUITE( filesystem )

BOOST_AUTO_TEST_CASE( directory )
{
	BOOST_CHECK_EQUAL( lean::get_directory("abc/./def/../test.txt"), "abc/./def/.." );
	BOOST_CHECK_EQUAL( lean::get_directory("abc/./def/../test."), "abc/./def/.." );
	BOOST_CHECK_EQUAL( lean::get_directory("abc/./def/../test"), "abc/./def/.." );
	BOOST_CHECK_EQUAL( lean::get_directory("/."), "" );
	BOOST_CHECK_EQUAL( lean::get_directory("/"), "" );
	BOOST_CHECK_EQUAL( lean::get_directory(""), "" );
}

BOOST_AUTO_TEST_CASE( filename )
{
	BOOST_CHECK_EQUAL( lean::get_filename("abc/./def/../test.txt"), "test.txt" );
	BOOST_CHECK_EQUAL( lean::get_filename("abc/./def/../test."), "test." );
	BOOST_CHECK_EQUAL( lean::get_filename("abc/./def/../test"), "test" );
	BOOST_CHECK_EQUAL( lean::get_filename("/."), "." );
	BOOST_CHECK_EQUAL( lean::get_filename("/"), "" );
	BOOST_CHECK_EQUAL( lean::get_filename(""), "" );
}

BOOST_AUTO_TEST_CASE( stem )
{
	BOOST_CHECK_EQUAL( lean::get_stem("abc/./def/../test.txt"), "test" );
	BOOST_CHECK_EQUAL( lean::get_stem("abc/./def/../test."), "test" );
	BOOST_CHECK_EQUAL( lean::get_stem("abc/./def/../test"), "test" );
	BOOST_CHECK_EQUAL( lean::get_stem("/."), "" );
	BOOST_CHECK_EQUAL( lean::get_stem("/"), "" );
	BOOST_CHECK_EQUAL( lean::get_stem(""), "" );
}

BOOST_AUTO_TEST_CASE( extension )
{
	BOOST_CHECK_EQUAL( lean::get_extension("abc/./def/../test.txt"), ".txt" );
	BOOST_CHECK_EQUAL( lean::get_extension("abc/./def/../test."), "." );
	BOOST_CHECK_EQUAL( lean::get_extension("abc/./def/../test"), "" );
	BOOST_CHECK_EQUAL( lean::get_extension("/."), "." );
	BOOST_CHECK_EQUAL( lean::get_extension("/"), "" );
	BOOST_CHECK_EQUAL( lean::get_extension(""), "" );
}

BOOST_AUTO_TEST_SUITE_END()
