#include "stdafx.h"
#include <lean/containers/simple_vector.h>
#include <lean/time/highres_timer.h>
#include <vector>

struct bla
{
	int a;

	bla(int i) : a(i) { }
	~bla() { }
};

double perfTestSTL()
{
	lean::highres_timer timer;

	for (int x = 0; x < 1000; ++x)
	{
		std::vector<bla> a;
		a.reserve(10000);

		for (int i = 0; i < 10000; ++i)
			a.push_back(i);
	}

	return timer.milliseconds();
}

double perfTestLean()
{
	lean::highres_timer timer;

	for (int x = 0; x < 1000; ++x)
	{
		lean::simple_vector<bla> a;
		a.reserve(10000);

		for (int i = 0; i < 10000; ++i)
			a.push_back(i);
	}

	return timer.milliseconds();
}

BOOST_AUTO_TEST_SUITE( simple_vector )

BOOST_AUTO_TEST_CASE( perf )
{
	double lean = perfTestLean();
	double stl = perfTestSTL();
	BOOST_CHECK_EQUAL(stl / lean, 1.0);

	stl = perfTestSTL();
	lean = perfTestLean();
	BOOST_CHECK_EQUAL(stl / lean, 1.0);

	lean = perfTestLean();
	stl = perfTestSTL();
	BOOST_CHECK_EQUAL(stl / lean, 1.0);

	stl = perfTestSTL();
	lean = perfTestLean();
	BOOST_CHECK_EQUAL(stl / lean, 1.0);
}

BOOST_AUTO_TEST_SUITE_END()
