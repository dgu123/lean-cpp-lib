#include "stdafx.h"
#include <lean/io/endianness.h>

template <class T, class X>
inline T&& reinterpret_rval(X &&value) { return reinterpret_cast<T&&>(value); };
template <class T, class X> inline const T& reinterpret_rval(const X &value) { return reinterpret_cast<const T&>(value); };

BOOST_AUTO_TEST_SUITE( aligned )

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

BOOST_AUTO_TEST_SUITE_END()
