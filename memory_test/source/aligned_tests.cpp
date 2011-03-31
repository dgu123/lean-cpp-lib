#include "stdafx.h"
#include <lean/memory/aligned.h>

#include <iostream>

struct aligned_object : public lean::aligned<16>
{
	float a[4];

	~aligned_object() { ::std::cout << "dtor"; };
};

BOOST_AUTO_TEST_SUITE( aligned )

BOOST_AUTO_TEST_CASE( stack_aligned_array )
{
	aligned_object a[7];
	BOOST_CHECK( reinterpret_cast<uintptr_t>(a) % 16 == 0 );
}

BOOST_AUTO_TEST_CASE( aligned_array )
{
	aligned_object *a = new aligned_object[4];
	aligned_object *b = new aligned_object[5];
	BOOST_CHECK( reinterpret_cast<uintptr_t>(a) % 16 == 0 );
	BOOST_CHECK( reinterpret_cast<uintptr_t>(b) % 16 == 0 );
	delete[] a;
	delete[] b;
}

BOOST_AUTO_TEST_SUITE_END()
