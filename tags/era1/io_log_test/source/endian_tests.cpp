#include "stdafx.h"
#include <lean/io/endianness.h>

BOOST_AUTO_TEST_SUITE( endian )

BOOST_AUTO_TEST_CASE( hword )
{
	short a = lean::byteswap_big(static_cast<unsigned short>(0xdead));
	
	const unsigned char *ab = reinterpret_cast<const unsigned char*>(&a);
	BOOST_CHECK_EQUAL(ab[0], 0xde);
	BOOST_CHECK_EQUAL(ab[1], 0xad);

	short b = lean::byteswap_little(static_cast<unsigned short>(0xdead));
	
	const unsigned char *bb = reinterpret_cast<const unsigned char*>(&b);
	BOOST_CHECK_EQUAL(bb[1], 0xde);
	BOOST_CHECK_EQUAL(bb[0], 0xad);
}

BOOST_AUTO_TEST_CASE( word )
{
	int a = lean::byteswap_big(0xdeadbeefU);
	
	const unsigned char *ab = reinterpret_cast<const unsigned char*>(&a);
	BOOST_CHECK_EQUAL(ab[0], 0xde);
	BOOST_CHECK_EQUAL(ab[1], 0xad);
	BOOST_CHECK_EQUAL(ab[2], 0xbe);
	BOOST_CHECK_EQUAL(ab[3], 0xef);

	int b = lean::byteswap_little(0xdeadbeefU);
	
	const unsigned char *bb = reinterpret_cast<const unsigned char*>(&b);
	BOOST_CHECK_EQUAL(bb[3], 0xde);
	BOOST_CHECK_EQUAL(bb[2], 0xad);
	BOOST_CHECK_EQUAL(bb[1], 0xbe);
	BOOST_CHECK_EQUAL(bb[0], 0xef);
}

BOOST_AUTO_TEST_CASE( dword )
{
	long long a = lean::byteswap_big(0xdeadbeefdeadbeefULL);
	
	const unsigned char *ab = reinterpret_cast<const unsigned char*>(&a);
	BOOST_CHECK_EQUAL(ab[0], 0xde);
	BOOST_CHECK_EQUAL(ab[1], 0xad);
	BOOST_CHECK_EQUAL(ab[2], 0xbe);
	BOOST_CHECK_EQUAL(ab[3], 0xef);
	BOOST_CHECK_EQUAL(ab[4], 0xde);
	BOOST_CHECK_EQUAL(ab[5], 0xad);
	BOOST_CHECK_EQUAL(ab[6], 0xbe);
	BOOST_CHECK_EQUAL(ab[7], 0xef);

	long long b = lean::byteswap_little(0xdeadbeefdeadbeefULL);
	
	const unsigned char *bb = reinterpret_cast<const unsigned char*>(&b);
	BOOST_CHECK_EQUAL(bb[7], 0xde);
	BOOST_CHECK_EQUAL(bb[6], 0xad);
	BOOST_CHECK_EQUAL(bb[5], 0xbe);
	BOOST_CHECK_EQUAL(bb[4], 0xef);
	BOOST_CHECK_EQUAL(bb[3], 0xde);
	BOOST_CHECK_EQUAL(bb[2], 0xad);
	BOOST_CHECK_EQUAL(bb[1], 0xbe);
	BOOST_CHECK_EQUAL(bb[0], 0xef);
}

BOOST_AUTO_TEST_CASE( bulk )
{
	int a[] = { 0xdeadbeefU, 0xdeadbeefU, 0xdeadbeefU, 0xdeadbeefU, 0xdeadbeefU, 0xdeadbeefU };
	static const size_t count = sizeof(a) / sizeof(int);

	int b[count];
	lean::byteswap_big(a, a + count, b);

	for (int i = 0; i < count; ++i)
	{
		const unsigned char *bb = reinterpret_cast<const unsigned char*>(&b[i]);

		BOOST_CHECK_EQUAL(bb[0], 0xde);
		BOOST_CHECK_EQUAL(bb[1], 0xad);
		BOOST_CHECK_EQUAL(bb[2], 0xbe);
		BOOST_CHECK_EQUAL(bb[3], 0xef);
	}

	int c[count];
	lean::byteswap_little(a, a + count, c);

	for (int i = 0; i < count; ++i)
	{
		const unsigned char *cb = reinterpret_cast<const unsigned char*>(&c[i]);

		BOOST_CHECK_EQUAL(cb[3], 0xde);
		BOOST_CHECK_EQUAL(cb[2], 0xad);
		BOOST_CHECK_EQUAL(cb[1], 0xbe);
		BOOST_CHECK_EQUAL(cb[0], 0xef);
	}
}

BOOST_AUTO_TEST_SUITE_END()
