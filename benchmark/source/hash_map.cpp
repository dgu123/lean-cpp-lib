#include "stdafx.h"
#include <lean/containers/simple_hash_map.h>
#include <unordered_map>
#include <lean/io/numeric.h>

template <bool PreAllocate>
struct int_int_test
{
	static const int run_count = 10;
	static const int element_count = 100000;

	static double stl()
	{
		lean::highres_timer timer;

		for (int x = 0; x < run_count; ++x)
		{
			std::unordered_map<int, int> map;

			if (PreAllocate)
				map.rehash(element_count);

			for (int i = 0; i < element_count; ++i)
				map[i] = i;
		}

		return timer.milliseconds() / run_count;
	}

	static double lean()
	{
		lean::highres_timer timer;

		for (int x = 0; x < run_count; ++x)
		{
			lean::simple_hash_map<int, int> map;

			if (PreAllocate)
				map.rehash(element_count);

			for (int i = 0; i < element_count; ++i)
				map[i] = i;
		}

		return timer.milliseconds() / run_count;
	}
};

template <bool PreAllocate>
struct string_string_test
{
	static const int run_count = 10;
	static const int element_count = 1000;

	static double stl()
	{
		lean::highres_timer timer;

		for (int x = 0; x < run_count; ++x)
		{
			std::unordered_map<std::string, std::string> map;

			if (PreAllocate)
				map.rehash(element_count);

			char buffer[lean::max_int_string_length<int>::value];

			for (int i = 0; i < element_count; ++i)
			{
				*lean::int_to_char(buffer, i * 14237) = 0;
				map[buffer] = buffer;
			}
		}

		return timer.milliseconds() / run_count;
	}

	static double lean()
	{
		lean::highres_timer timer;

		for (int x = 0; x < run_count; ++x)
		{
			lean::simple_hash_map<std::string, std::string> map;

			if (PreAllocate)
				map.rehash(element_count);

			char buffer[lean::max_int_string_length<int>::value];

			for (int i = 0; i < element_count; ++i)
			{
				*lean::int_to_char(buffer, i * 14237) = 0;
				map[buffer] = buffer;
			}
		}

		return timer.milliseconds() / run_count;
	}
};

template <class Test>
void run_test(const char *name)
{
	double leanTime = Test::lean();
	double stlTime = Test::stl();

	leanTime += Test::lean();
	stlTime += Test::stl();

	print_results(name, "std", stlTime, "lean", leanTime);
}

LEAN_NOLTINLINE void hash_map_benchmark()
{
	run_test< int_int_test<false> >("int_int_hash_map");
	run_test< int_int_test<true> >("int_int_hash_map_preall");

	run_test< string_string_test<false> >("string_string_hash_map");
	run_test< string_string_test<true> >("string_string_hash_map_preall");
}
