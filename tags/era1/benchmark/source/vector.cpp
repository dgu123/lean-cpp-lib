#include "stdafx.h"
#include <lean/io/numeric.h>
#include <lean/containers/simple_vector.h>
#include <vector>

template <bool PreAllocate>
struct int_test
{
	static const int element_count = 1000000 / DEBUG_DENOMINATOR;

	static double stl()
	{
		lean::highres_timer timer;

		{
			std::vector<int> vec;

			if (PreAllocate)
				vec.reserve(element_count);

			for (int i = 0; i < element_count; ++i)
				vec.push_back(i);
		}

		return timer.milliseconds();
	}

	static double lean()
	{
		lean::highres_timer timer;

		{
			typedef lean::simple_vector<int, lean::simple_vector_policies::pod> vec_type;
			vec_type vec;

			if (PreAllocate)
				vec.reserve(element_count);

			for (int i = 0; i < element_count; ++i)
				vec.push_back(i);
		}

		return timer.milliseconds();
	}
};

template <bool PreAllocate>
struct string_test
{
	static const int element_count = 1000000 / DEBUG_DENOMINATOR;

	static double stl()
	{
		lean::highres_timer timer;

		{
			std::vector<std::string> vec;
		
			if (PreAllocate)
				vec.reserve(element_count);

			char buffer[lean::max_int_string_length<int>::value];
			*lean::int_to_char(buffer, rand()) = 0;

			for (int i = 0; i < element_count; ++i)
				vec.push_back(std::string(buffer));
		}

		return timer.milliseconds();
	}

	static double lean()
	{
		lean::highres_timer timer;

		{
			lean::simple_vector<std::string> vec;

			if (PreAllocate)
				vec.reserve(element_count);

			char buffer[lean::max_int_string_length<int>::value];
			*lean::int_to_char(buffer, rand()) = 0;

			for (int i = 0; i < element_count; ++i)
				vec.push_back(std::string(buffer));
		}

		return timer.milliseconds();
	}
};

template <class Test>
void run_test(const char *name)
{
	static const int run_count = 100;

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

LEAN_NOLTINLINE void vector_benchmark()
{
	run_test< int_test<false> >("int_vec");
	run_test< int_test<true> >("int_vec_preall");

	run_test< string_test<false> >("string_vec");
	run_test< string_test<true> >("string_vec_preall");
}
