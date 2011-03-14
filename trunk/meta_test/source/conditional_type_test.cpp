#include "stdafx.h"
#include <lean/meta/conditional_type.h>

BOOST_AUTO_TEST_SUITE( conditional_type )

BOOST_AUTO_TEST_CASE( true_type )
{
	int i;
	lean::conditional_type<true, int*, double*>::type pi = &i;
	BOOST_CHECK_EQUAL(pi, &i);
}

BOOST_AUTO_TEST_CASE( false_type )
{
	double j;
	lean::conditional_type<false, int*, double*>::type pj = &j;
	BOOST_CHECK_EQUAL(pj, &j);
}

BOOST_AUTO_TEST_SUITE_END()
