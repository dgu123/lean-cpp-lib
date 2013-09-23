#include "stdafx.h"
#include <lean/containers/simple_hash_map.h>
#include <random>
#include <vector>

BOOST_AUTO_TEST_SUITE( simple_hash_map )

BOOST_AUTO_TEST_CASE( int_pod )
{
	const size_t count = 10000;

	typedef lean::simple_hash_map<int, int> map_type;
	map_type map;

	BOOST_CHECK(map.empty());
	BOOST_CHECK_EQUAL(map.size(), 0U);
	BOOST_CHECK(map.capacity() >= map.size());

	// insert
	for (int i = 0; i < count; ++i)
		if (i % 2)
			map.insert(map_type::value_type(i, 3 * i));
		else
			map.insert(i).second = 3 * i;

	BOOST_CHECK(!map.empty());
	BOOST_CHECK_EQUAL(map.size(), count);
	BOOST_CHECK(map.capacity() >= map.size());

	for (int i = 0; i < count; ++i)
	{
		map_type::iterator itElem = map.find(i);

		BOOST_CHECK(itElem != map.end());
		BOOST_CHECK_EQUAL(itElem->first, i);
		BOOST_CHECK_EQUAL(itElem->second, 3 * i);
	}

	// iteration
	for (map_type::iterator itElem = map.begin(); itElem != map.end(); ++itElem)
		BOOST_CHECK_EQUAL(3 * itElem->first, itElem->second);

	// erase
	for (int i = 0; i < count; i += 100)
		map.erase(i);

	for (int i = 0; i < count; ++i)
	{
		map_type::iterator itElem = map.find(i);

		if (i % 100)
		{
			BOOST_CHECK(itElem != map.end());
			BOOST_CHECK_EQUAL(itElem->first, i);
			BOOST_CHECK_EQUAL(itElem->second, 3 * i);
		}
		else
			BOOST_CHECK(itElem == map.end());
	}

	// clear
	map.clear();

	BOOST_CHECK(map.empty());
	BOOST_CHECK_EQUAL(map.size(), 0U);
	BOOST_CHECK(map.capacity() >= map.size());
}

BOOST_AUTO_TEST_SUITE_END()
