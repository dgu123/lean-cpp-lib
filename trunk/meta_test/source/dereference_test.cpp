#include "stdafx.h"
#include <lean/meta/dereference.h>

BOOST_AUTO_TEST_SUITE( maybe_dereference_once )

BOOST_AUTO_TEST_CASE( value )
{
	int value;
	int &result = lean::maybe_dereference_once<int>::dereference(value);

	BOOST_CHECK_EQUAL( &result, &value );
}

BOOST_AUTO_TEST_CASE( reference_value )
{
	int value;
	int &result = lean::maybe_dereference_once<int&>::dereference(value);

	BOOST_CHECK_EQUAL( &result, &value);
}

BOOST_AUTO_TEST_CASE( const_value )
{
	const int value = 0;
	const int &result = lean::maybe_dereference_once<const int>::dereference(value);

	BOOST_CHECK_EQUAL( &result, &value );
}

BOOST_AUTO_TEST_CASE( const_reference_value )
{
	const int value = 0;
	const int &result = lean::maybe_dereference_once<const int&>::dereference(value);

	BOOST_CHECK_EQUAL( &result, &value );
}

BOOST_AUTO_TEST_CASE( volatile_value )
{
	volatile int value;
	volatile int &result = lean::maybe_dereference_once<volatile int>::dereference(value);

	BOOST_CHECK_EQUAL( const_cast<int*>(&result), const_cast<int*>(&value) );
}

BOOST_AUTO_TEST_CASE( volatile_reference_value )
{
	volatile int value;
	volatile int &result = lean::maybe_dereference_once<volatile int&>::dereference(value);

	BOOST_CHECK_EQUAL( const_cast<int*>(&result), const_cast<int*>(&value) );
}

BOOST_AUTO_TEST_CASE( const_volatile_value )
{
	const volatile int value = 0;
	const volatile int &result = lean::maybe_dereference_once<const volatile int>::dereference(value);

	BOOST_CHECK_EQUAL( const_cast<int*>(&result), const_cast<int*>(&value) );
}

BOOST_AUTO_TEST_CASE( const_volatile_reference_value )
{
	const volatile int value = 0;
	const volatile int &result = lean::maybe_dereference_once<const volatile int&>::dereference(value);

	BOOST_CHECK_EQUAL( const_cast<int*>(&result), const_cast<int*>(&value) );
}


BOOST_AUTO_TEST_CASE( pointer )
{
	int value;
	int *pointer = &value;
	int &result = lean::maybe_dereference_once<int*>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, &value );
}

BOOST_AUTO_TEST_CASE( pointer_reference )
{
	int value;
	int *pointer = &value;
	int &result = lean::maybe_dereference_once<int*&>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, &value);
}

BOOST_AUTO_TEST_CASE( pointer_const )
{
	int value;
	int *const pointer = &value;
	int &result = lean::maybe_dereference_once<int*const>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, &value);
}

BOOST_AUTO_TEST_CASE( pointer_const_reference )
{
	int value;
	int *const pointer = &value;
	int &result = lean::maybe_dereference_once<int*const&>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, &value);
}

BOOST_AUTO_TEST_CASE( pointer_volatile )
{
	int value;
	int *volatile pointer = &value;
	int &result = lean::maybe_dereference_once<int*volatile>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, &value);
}

BOOST_AUTO_TEST_CASE( pointer_volatile_reference )
{
	int value;
	int *volatile pointer = &value;
	int &result = lean::maybe_dereference_once<int*volatile&>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, &value);
}

BOOST_AUTO_TEST_CASE( pointer_const_volatile )
{
	int value;
	int *const volatile pointer = &value;
	int &result = lean::maybe_dereference_once<int*const volatile>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, pointer);
}

BOOST_AUTO_TEST_CASE( pointer_const_volatile_reference )
{
	int value;
	int *const volatile pointer = &value;
	int &result = lean::maybe_dereference_once<int*const volatile&>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, pointer);
}


BOOST_AUTO_TEST_CASE( const_pointer )
{
	int value;
	const int *pointer = &value;
	const int &result = lean::maybe_dereference_once<const int*>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, pointer );
}

BOOST_AUTO_TEST_CASE( const_pointer_reference )
{
	int value;
	const int *pointer = &value;
	const int &result = lean::maybe_dereference_once<const int*&>::dereference(pointer);

	BOOST_CHECK_EQUAL( &result, pointer );
}

BOOST_AUTO_TEST_CASE( volatile_pointer )
{
	volatile int value;
	volatile int *pointer = &value;
	volatile int &result = lean::maybe_dereference_once<volatile int*>::dereference(pointer);

	BOOST_CHECK_EQUAL( const_cast<int*>(&result), const_cast<int*>(pointer) );
}

BOOST_AUTO_TEST_CASE( volatile_pointer_reference )
{
	volatile int value;
	volatile int *pointer = &value;
	volatile int &result = lean::maybe_dereference_once<volatile int*&>::dereference(pointer);

	BOOST_CHECK_EQUAL( const_cast<int*>(&result), const_cast<int*>(pointer) );
}

BOOST_AUTO_TEST_CASE( const_volatile_pointer )
{
	const volatile int value = 0;
	const volatile int *pointer = &value;
	const volatile int &result = lean::maybe_dereference_once<const volatile int*>::dereference(pointer);

	BOOST_CHECK_EQUAL( const_cast<int*>(&result), const_cast<int*>(pointer) );
}

BOOST_AUTO_TEST_CASE( const_volatile_pointer_reference )
{
	const volatile int value = 0;
	const volatile int *pointer = &value;
	const volatile int &result = lean::maybe_dereference_once<const volatile int*&>::dereference(pointer);

	BOOST_CHECK_EQUAL( const_cast<int*>(&result), const_cast<int*>(pointer) );
}

BOOST_AUTO_TEST_SUITE_END()
