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

BOOST_AUTO_TEST_CASE( append )
{
	BOOST_CHECK_EQUAL( lean::append_path("abc/def", "ghi/test.txt"), "abc/def/ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::append_path("abc/def\\", "ghi/test.txt"), "abc/def\\ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::append_path("abc/def", "\\ghi/test.txt"), "abc/def\\ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::append_path("abc/def/", "ghi/test.txt"), "abc/def/ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::append_path("abc/def", "/ghi/test.txt"), "abc/def/ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::append_path("abc/def/", "/ghi/test.txt"), "abc/def//ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::append_path("", "ghi/test.txt"), "ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::append_path("abc/def", ""), "abc/def" );
	BOOST_CHECK_EQUAL( lean::append_path("", ""), "" );
}

BOOST_AUTO_TEST_CASE( canonical )
{
	BOOST_CHECK_EQUAL( lean::canonical_path("abc//def/../ghi/./test.txt"), "abc/ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path("abc//def/../ghi/../test.txt"), "abc/test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path("abc/../def/../ghi/../test.txt"), "test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path("abc/../../def/../ghi/../test.txt"), "../test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path("../abc/../../def/../ghi/../test.txt"), "../../test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path("./abc/../../def/../ghi/../test.txt"), "../test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path("test.txt"), "test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path("../test.txt"), "../test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path("./test.txt"), "test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path("/test.txt"), "/test.txt" );
	BOOST_CHECK_EQUAL( lean::canonical_path(""), "" );
}

BOOST_AUTO_TEST_CASE( absolute )
{
	BOOST_CHECK_EQUAL( lean::absolute_path("abc/def///", "../ghi/./test.txt"), "abc/ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::absolute_path("abc/def///", "../../ghi/../test.txt"), "test.txt" );
	BOOST_CHECK_EQUAL( lean::absolute_path("abc/def///", "../../../ghi/../test.txt"), "../test.txt" );
	BOOST_CHECK_EQUAL( lean::absolute_path("", ""), "" );
}

BOOST_AUTO_TEST_CASE( relative )
{
	BOOST_CHECK_EQUAL( lean::relative_path("abc/def", "abc/def/ghi/test.txt"), "ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::relative_path("abc/def/xyz", "abc/def/ghi/test.txt"), "../ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::relative_path("abc/def/xyz", "def/ghi/test.txt"), "../../../def/ghi/test.txt" );
	BOOST_CHECK_EQUAL( lean::relative_path("abc/def/xyz", "test.txt"), "../../../test.txt" );
	BOOST_CHECK_EQUAL( lean::relative_path("", "test.txt"), "test.txt" );
	BOOST_CHECK_EQUAL( lean::relative_path("test", ""), ".." );
	BOOST_CHECK_EQUAL( lean::relative_path("", ""), "" );
}

BOOST_AUTO_TEST_SUITE_END()
