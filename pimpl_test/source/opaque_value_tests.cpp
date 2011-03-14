#include "stdafx.h"
#include "pimpl_values.h"

BOOST_AUTO_TEST_SUITE( opaque_val )

static const int BOGUS_INT_HANDLE = 0xdeadbeef;
static const void* const BOGUS_VOID_HANDLE = reinterpret_cast<const void*>(BOGUS_INT_HANDLE);

BOOST_AUTO_TEST_CASE( get )
{
	BOOST_CHECK_EQUAL( SomeModule(BOGUS_INT_HANDLE).getHandle().get(), BOGUS_VOID_HANDLE );
}

BOOST_AUTO_TEST_CASE( dereference )
{
	BOOST_CHECK_EQUAL( *SomeModule(BOGUS_INT_HANDLE).getHandle(), BOGUS_VOID_HANDLE );
}

BOOST_AUTO_TEST_CASE( cast )
{
	BOOST_CHECK_EQUAL( SomeModule(BOGUS_INT_HANDLE).getHandle(), BOGUS_VOID_HANDLE );
}

BOOST_AUTO_TEST_SUITE_END()
