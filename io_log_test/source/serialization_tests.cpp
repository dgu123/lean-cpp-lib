#include "stdafx.h"
#include <lean/time/highres_timer.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <locale>
#include <sstream>
#include <random>
#include <limits>

#include <lean/lean.h>
#include <lean/io/numeric.h>

#include <C:/SDKs/Boost/1_44/boost/lexical_cast.hpp>

template <class Integer>
void int_to_char_check_limits()
{
	char buf1[64];
	char buf2[64];

	*lean::int_to_char(buf1, std::numeric_limits<Integer>::min()) = 0;
	_itoa(std::numeric_limits<Integer>::min(), buf2, 10);
	BOOST_CHECK_EQUAL(buf1, buf2);

	*lean::int_to_char(buf1, Integer(0)) = 0;
	_itoa(Integer(0), buf2, 10);
	BOOST_CHECK_EQUAL(buf1, buf2);

	*lean::int_to_char(buf1, std::numeric_limits<Integer>::max()) = 0;
	_i64toa(std::numeric_limits<Integer>::max(), buf2, 10);
	BOOST_CHECK_EQUAL(buf1, buf2);
}

template <class Integer>
void int_to_char_check_random()
{
	char buf1[64];
	char buf2[64];

	for (int i = 0; i < 10000; ++i)
	{
		Integer num = static_cast<Integer>(rand());

		*lean::int_to_char(buf1, num) = 0;
		_itoa(num, buf2, 10);
		BOOST_CHECK_EQUAL(buf1, buf2);
	}
}

BOOST_AUTO_TEST_SUITE( serialization )

BOOST_AUTO_TEST_CASE( float_to_char )
{
	char buf1[64];

	*lean::float_to_char(buf1, std::numeric_limits<float>::quiet_NaN()) = 0;
	std::cout << "float_to_char: " << buf1 << std::endl;

	*lean::float_to_char(buf1, std::numeric_limits<float>::infinity()) = 0;
	std::cout << "float_to_char: " << buf1 << std::endl;

	*lean::float_to_char(buf1, -std::numeric_limits<float>::infinity()) = 0;
	std::cout << "float_to_char: " << buf1 << std::endl;

	*lean::float_to_char(buf1, 2.0f) = 0;
	std::cout << "float_to_char: " << buf1 << std::endl;
}

BOOST_AUTO_TEST_CASE( int_to_char )
{
	int_to_char_check_limits<char>();
	int_to_char_check_limits<unsigned char>();
	int_to_char_check_limits<short>();
	int_to_char_check_limits<unsigned short>();
	int_to_char_check_limits<int>();
	int_to_char_check_limits<unsigned int>();
	int_to_char_check_limits<long>();
	int_to_char_check_limits<unsigned long>();
	
	int_to_char_check_random<char>();
	int_to_char_check_random<unsigned char>();
	int_to_char_check_random<short>();
	int_to_char_check_random<unsigned short>();
	int_to_char_check_random<int>();
	int_to_char_check_random<unsigned int>();
	int_to_char_check_random<long>();
	int_to_char_check_random<unsigned long>();
}

BOOST_AUTO_TEST_CASE( itoa )
{
	std::locale locale;
	std::stringstream str;
	char buf[64];

	lean::highres_timer int_to_char_timer;

	for (int i = 0; i < 100000; ++i)
		lean::int_to_char(buf, i);

	std::cout << "int_to_char: " << int_to_char_timer.milliseconds() << std::endl;

	lean::highres_timer itoa_timer;

	for (int i = 0; i < 100000; ++i)
		_itoa(i, buf, 10);

	std::cout << "itoa: " << itoa_timer.milliseconds() << std::endl;

	lean::highres_timer sprintf_timer;

	for (int i = 0; i < 100000; ++i)
		sprintf(buf, "%d", i);

	std::cout << "sprintf: " << sprintf_timer.milliseconds() << std::endl;

	lean::highres_timer numpunct_timer;

	for (int i = 0; i < 100000; ++i)
		std::use_facet< std::num_put<char, char*> >(locale).put(buf, str, ' ', (long)i);

	std::cout << "num_put: " << numpunct_timer.milliseconds() << std::endl;

	lean::highres_timer lexcast_timer;

	for (int i = 0; i < 100000; ++i)
		boost::lexical_cast<std::string>(i);

	std::cout << "lexical_cast: " << lexcast_timer.milliseconds() << std::endl;
}

BOOST_AUTO_TEST_CASE( dtoa )
{
	std::locale locale;
	std::stringstream str;
	char buf[64];

	lean::highres_timer int_to_char_timer;

	for (int i = 0; i < 100000; ++i)
		lean::float_to_char(buf, (double) i);

	std::cout << "float_to_char: " << int_to_char_timer.milliseconds() << std::endl;

	lean::highres_timer itoa_timer;

	for (int i = 0; i < 100000; ++i)
		_gcvt(i, 6, buf);

	std::cout << "_gcvt: " << itoa_timer.milliseconds() << std::endl;

	lean::highres_timer sprintf_timer;

	for (int i = 0; i < 100000; ++i)
		sprintf(buf, "%f", (double) i);

	std::cout << "sprintf: " << sprintf_timer.milliseconds() << std::endl;

	lean::highres_timer numpunct_timer;

	for (int i = 0; i < 100000; ++i)
		std::use_facet< std::num_put<char, char*> >(locale).put(buf, str, ' ', (double)i);
	
	std::cout << "num_put: " << numpunct_timer.milliseconds() << std::endl;

	lean::highres_timer lexcast_timer;

	for (int i = 0; i < 100000; ++i)
		boost::lexical_cast<std::string>((double)i);

	std::cout << "lexical_cast: " << lexcast_timer.milliseconds() << std::endl;
}

BOOST_AUTO_TEST_CASE( atoi )
{
	std::locale locale;
	std::stringstream str;
	std::basic_ios<char>::iostate error;
	const char *buf = "1251754";
	int result;

	lean::highres_timer int_to_char_timer;

	for (int i = 0; i < 100000; ++i)
		lean::char_to_int(buf, buf + sizeof(buf), result);

	std::cout << "char_to_int: " << int_to_char_timer.milliseconds() << std::endl;

	lean::highres_timer atoi_timer;

	for (int i = 0; i < 100000; ++i)
		result = ::atoi(buf);

	std::cout << "atoi: " << atoi_timer.milliseconds() << std::endl;

	lean::highres_timer sscanf_timer;

	for (int i = 0; i < 100000; ++i)
		sscanf(buf, "%d", &result);

	std::cout << "sscanf: " << sscanf_timer.milliseconds() << std::endl;

	lean::highres_timer numpunct_timer;

	for (int i = 0; i < 100000; ++i)
		std::use_facet< std::num_get<char, const char*> >(locale).get(buf, buf + sizeof(buf), str, error, reinterpret_cast<long&>(result));

	std::cout << "num_get: " << numpunct_timer.milliseconds() << std::endl;

	lean::highres_timer lexcast_timer;

	for (int i = 0; i < 100000; ++i)
		boost::lexical_cast<int>(buf);

	std::cout << "lexical_cast: " << lexcast_timer.milliseconds() << std::endl;
}

BOOST_AUTO_TEST_CASE( atof )
{
	std::locale locale;
	std::stringstream str;
	std::basic_ios<char>::iostate error;
	const char *buf = "1251754.2531334";
	float result;

	lean::highres_timer atoi_timer;

	for (int i = 0; i < 100000; ++i)
		result = (float) ::atof(buf);

	std::cout << "atof: " << atoi_timer.milliseconds() << std::endl;

	lean::highres_timer int_to_char_timer;

	for (int i = 0; i < 100000; ++i)
		lean::char_to_float(buf, buf + sizeof(buf), result);

	std::cout << "char_to_float: " << int_to_char_timer.milliseconds() << std::endl;

	lean::highres_timer sscanf_timer;

	for (int i = 0; i < 100000; ++i)
		sscanf(buf, "%f", &result);

	std::cout << "sscanf: " << sscanf_timer.milliseconds() << std::endl;

	lean::highres_timer numpunct_timer;

	for (int i = 0; i < 100000; ++i)
		std::use_facet< std::num_get<char, const char*> >(locale).get(buf, buf + sizeof(buf), str, error, result);

	std::cout << "num_get: " << numpunct_timer.milliseconds() << std::endl;

	lean::highres_timer lexcast_timer;

	for (int i = 0; i < 100000; ++i)
		boost::lexical_cast<float>(buf);

	std::cout << "lexical_cast: " << lexcast_timer.milliseconds() << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
