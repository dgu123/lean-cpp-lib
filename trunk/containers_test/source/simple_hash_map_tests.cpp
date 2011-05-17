#include "stdafx.h"
#include <lean/containers/simple_hash_map.h>
#include <random>
#include <vector>

template <class Type>
struct some_source
{
	static const Type value;
};

template <class Type> const Type some_source<Type>::value = Type(
		(std::numeric_limits<Type>::min() != Type())
			? std::numeric_limits<Type>::min()
			: std::numeric_limits<Type>::max()
	);

template <class Type>
struct some_dest
{
	static const Type value;
};

template <class Type> const Type some_dest<Type>::value(some_source<Type>::value);

BOOST_AUTO_TEST_SUITE( simple_hash_map )

BOOST_AUTO_TEST_CASE( int_pod )
{
	int dest = some_dest<int>::value;
	int source = some_source<int>::value;

	const size_t count = 10000;

	int test = (std::numeric_limits<int>::has_infinity)
				? std::numeric_limits<int>::infinity()
				: (std::numeric_limits<int>::min() != int())
					? std::numeric_limits<int>::min()
					: std::numeric_limits<int>::max();
	int test2 = lean::containers::impl::default_numeric_keys<int>::invalid_key;
	int test3 = lean::containers::default_keys<int>::invalid_key;

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

	// clear
	map.clear();

	BOOST_CHECK(map.empty());
	BOOST_CHECK_EQUAL(map.size(), 0U);
	BOOST_CHECK(map.capacity() >= map.size());
}

BOOST_AUTO_TEST_SUITE_END()
