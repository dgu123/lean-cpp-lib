#include "stdafx.h"
#include <lean/io/mapped_file.h>

BOOST_AUTO_TEST_SUITE( mapped_file )

BOOST_AUTO_TEST_CASE( write_read )
{
	static const int count = 16 * 1024 * 1024;

	{
		lean::mapped_file file(MAKE_TEST_FILENAME("test1.dat"), count * sizeof(int));
		BOOST_CHECK_EQUAL(file.size(), count * sizeof(int));

		int *values = static_cast<int*>(file.data());

		for (int i = 0; i < count; ++i)
			values[i] = i;
	}
	{
		lean::rmapped_file file(MAKE_TEST_FILENAME("test1.dat"));
		BOOST_CHECK_EQUAL(file.size(), count * sizeof(int));

		const int *values = static_cast<const int*>(file.data());

		int i = 0;

		for (; i < count; ++i)
			if (values[i] != i)
				break;
		BOOST_CHECK_EQUAL(i, count);
	}
}

BOOST_AUTO_TEST_CASE( map )
{
	static const int count = 1024 * 1024;

	{
		lean::mapped_file file(MAKE_TEST_FILENAME("test2.dat"), count * sizeof(int));
		BOOST_CHECK_EQUAL(file.size(), count * sizeof(int));

		int *values = static_cast<int*>(file.data());

		for (int i = 0; i < count; ++i)
			values[i] = i;
	}
	{
		lean::rmapped_file file(MAKE_TEST_FILENAME("test2.dat"), false);
		BOOST_CHECK_EQUAL(file.size(), count * sizeof(int));

		file.map(17351, 13513);
	}
}

BOOST_AUTO_TEST_SUITE_END()
