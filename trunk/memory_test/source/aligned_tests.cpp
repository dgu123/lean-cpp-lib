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
	aligned_object a[16];
	BOOST_CHECK( reinterpret_cast<uintptr_t>(a) % 16 == 0 );
}

BOOST_AUTO_TEST_CASE( aligned_array )
{
	aligned_object *a = new aligned_object[16];
	BOOST_CHECK( reinterpret_cast<uintptr_t>(a) % 16 == 0 );
}

BOOST_AUTO_TEST_SUITE_END()
