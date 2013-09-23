#include "stdafx.h"
#include <lean/containers/simple_hash_map.h>
#include <unordered_map>
#include <lean/io/numeric.h>

template <bool PreAllocate>
struct int_int_test
{
	static const int element_count = 1000000 / DEBUG_DENOMINATOR;

	static double stl()
	{
		lean::highres_timer timer;

		{
			std::unordered_map<int, int> map;

			if (PreAllocate)
				map.rehash(element_count);

			for (int i = 0; i < element_count; ++i)
				map[rand() ^ i]++;
		}

		return timer.milliseconds();
	}

	static double lean()
	{
		lean::highres_timer timer;

		{
			typedef lean::simple_hash_map<int, int, lean::simple_hash_map_policies::pod> map_type;
			map_type map;

			if (PreAllocate)
				map.rehash(element_count);

			for (int i = 0; i < element_count; ++i)
				map[rand() ^ i]++;
		}

		return timer.milliseconds();
	}
};

template <bool PreAllocate>
struct string_string_test
{
	static const int element_count = 1000000 / DEBUG_DENOMINATOR;

	static double stl()
	{
		std::unordered_map<std::string, std::string> map;

		// Don't include re-hashing in timing
		if (PreAllocate)
			map.rehash(element_count);

		lean::highres_timer timer;

		{
			char buffer[lean::max_int_string_length<int>::value];

			for (int i = 0; i < element_count; ++i)
			{
				*lean::int_to_char(buffer, rand() ^ i) = 0;
				map[std::string(buffer)] = std::string(buffer);
			}
		}

		return timer.milliseconds();
	}

	static double lean()
	{
		lean::simple_hash_map<std::string, std::string> map;

		// Don't include re-hashing in timing, evidently takes much longer with open hashing
		if (PreAllocate)
			map.rehash(element_count);

		lean::highres_timer timer;
		
		{
			char buffer[lean::max_int_string_length<int>::value];

			for (int i = 0; i < element_count; ++i)
			{
				*lean::int_to_char(buffer, rand() ^ i) = 0;
				map[std::string(buffer)] = std::string(buffer);
			}
		}

		return timer.milliseconds();
	}
};

template <class Test>
void run_test(const char *name)
{
	static const int run_count = 10;

	double stlTime = 0.0;
	double leanTime = 0.0;

	for (int x = 0; x < run_count; ++x)
	{
		srand(12452);
		stlTime += Test::stl();
		srand(12452);
		leanTime += Test::lean();
	}

	print_results(name, "std", stlTime, "lean", leanTime);
}

LEAN_NOLTINLINE void hash_map_benchmark()
{
	run_test< int_int_test<false> >("int_int_hash_map");
	run_test< int_int_test<true> >("int_int_hash_map_preall");

	run_test< string_string_test<false> >("string_string_hash_map");
	run_test< string_string_test<true> >("string_string_hash_map_preall");
}
