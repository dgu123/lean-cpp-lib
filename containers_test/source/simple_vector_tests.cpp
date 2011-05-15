#include "stdafx.h"
#include <lean/containers/simple_vector.h>
#include <random>
#include <vector>

BOOST_AUTO_TEST_SUITE( simple_vector )

BOOST_AUTO_TEST_CASE( int_pod )
{
	const int count = 10000;

	typedef lean::simple_vector<int, lean::simple_vector_policies::pod> vec_type;
	vec_type vec;

	BOOST_CHECK(vec.empty());
	BOOST_CHECK_EQUAL(vec.size(), 0);
	BOOST_CHECK(vec.capacity() >= vec.size());

	// push_back
	for (int i = 0; i < count; ++i)
		if (i % 2)
			vec.push_back(i);
		else
			vec.push_back() = i;

	BOOST_CHECK(!vec.empty());
	BOOST_CHECK_EQUAL(vec.size(), count);
	BOOST_CHECK(vec.capacity() >= vec.size());

	BOOST_CHECK_EQUAL(vec.front(), 0);
	BOOST_CHECK_EQUAL(vec.back(), count - 1);

	BOOST_CHECK_EQUAL(*vec.begin(), 0);
	BOOST_CHECK_EQUAL(*(vec.end() - 1), count - 1);

	for (int i = 0; i < count; ++i)
	{
		BOOST_CHECK_EQUAL(vec[i], i);
		BOOST_CHECK_EQUAL(vec.at(i), i);
	}

	for (vec_type::iterator itElem = vec.begin(); itElem != vec.end(); ++itElem)
		BOOST_CHECK_EQUAL(*itElem, itElem - vec.begin());

	// pop_back
	vec.pop_back();
	vec.pop_back();
	vec.pop_back();

	BOOST_CHECK(!vec.empty());
	BOOST_CHECK_EQUAL(vec.size(), count - 3);
	BOOST_CHECK(vec.capacity() >= vec.size());

	BOOST_CHECK_EQUAL(vec.front(), 0);
	BOOST_CHECK_EQUAL(vec.back(), count - 4);

	BOOST_CHECK_EQUAL(*vec.begin(), 0);
	BOOST_CHECK_EQUAL(*(vec.end() - 1), count - 4);

	for (int i = 0; i < count - 3; ++i)
	{
		BOOST_CHECK_EQUAL(vec[i], i);
		BOOST_CHECK_EQUAL(vec.at(i), i);
	}

	// resize-

	// resize2+

	// clear
	vec.clear();

	BOOST_CHECK(vec.empty());
	BOOST_CHECK_EQUAL(vec.size(), 0);
	BOOST_CHECK(vec.capacity() >= vec.size());
}

BOOST_AUTO_TEST_SUITE_END()
