#include "stdafx.h"
#include <lean/meta/strip.h>

BOOST_AUTO_TEST_SUITE( strip_const )

BOOST_AUTO_TEST_CASE( strip_const )
{
	BOOST_CHECK( (std::tr1::is_same<int, lean::strip_const<int>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<int, lean::strip_const<const int>::type>::value) );
}

BOOST_AUTO_TEST_CASE( strip_pointer_const )
{
	BOOST_CHECK( (std::tr1::is_same<int*, lean::strip_const<int*>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<int*, lean::strip_const<int *const>::type>::value) );
}

BOOST_AUTO_TEST_CASE( strip_const_reference )
{
	BOOST_CHECK( (std::tr1::is_same<int&, lean::strip_const<int&>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<const int&, lean::strip_const<const int&>::type>::value) );
}

BOOST_AUTO_TEST_CASE( strip_const_pointer )
{
	BOOST_CHECK( (std::tr1::is_same<int*, lean::strip_const<int*>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<const int*, lean::strip_const<const int*>::type>::value) );
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( strip_volatile )

BOOST_AUTO_TEST_CASE( strip_volatile )
{
	BOOST_CHECK( (std::tr1::is_same<int, lean::strip_volatile<int>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<int, lean::strip_volatile<volatile int>::type>::value) );
}

BOOST_AUTO_TEST_CASE( strip_pointer_volatile )
{
	BOOST_CHECK( (std::tr1::is_same<int*, lean::strip_volatile<int*>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<int*, lean::strip_volatile<int *volatile>::type>::value) );
}

BOOST_AUTO_TEST_CASE( strip_volatile_reference )
{
	BOOST_CHECK( (std::tr1::is_same<int&, lean::strip_volatile<int&>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<volatile int&, lean::strip_volatile<volatile int&>::type>::value) );
}

BOOST_AUTO_TEST_CASE( strip_volatile_pointer )
{
	BOOST_CHECK( (std::tr1::is_same<int*, lean::strip_volatile<int*>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<volatile int*, lean::strip_volatile<volatile int*>::type>::value) );
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( strip_reference )

BOOST_AUTO_TEST_CASE( strip_reference )
{
	BOOST_CHECK( (std::tr1::is_same<int, lean::strip_reference<int>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<int, lean::strip_reference<int&>::type>::value) );
}

BOOST_AUTO_TEST_CASE( strip_const_reference )
{
	BOOST_CHECK( (std::tr1::is_same<const int, lean::strip_reference<const int>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<const int, lean::strip_reference<const int&>::type>::value) );
}

BOOST_AUTO_TEST_CASE( strip_pointer_reference )
{
	BOOST_CHECK( (std::tr1::is_same<int*, lean::strip_reference<int*>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<int*, lean::strip_reference<int*&>::type>::value) );
}

BOOST_AUTO_TEST_CASE( strip_const_pointer_reference )
{
	BOOST_CHECK( (std::tr1::is_same<int *const, lean::strip_reference<int *const>::type>::value) );
	BOOST_CHECK( (std::tr1::is_same<int *const, lean::strip_reference<int *const &>::type>::value) );
}

BOOST_AUTO_TEST_SUITE_END()
