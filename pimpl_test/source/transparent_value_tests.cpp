#include "stdafx.h"

#define HANDLE_KNOWN
#include "pimpl_values.h"

BOOST_AUTO_TEST_SUITE( transparent_val )

static const int BOGUS_INT_HANDLE = 0xdeadbeef;
static const PrivateHandle BOGUS_HANDLE = reinterpret_cast<PrivateHandle>(BOGUS_INT_HANDLE);

BOOST_AUTO_TEST_CASE( construct )
{
	SomeHandle bogusHandle(BOGUS_HANDLE);
	BOOST_CHECK_EQUAL( bogusHandle.get(), BOGUS_HANDLE );
}

BOOST_AUTO_TEST_CASE( copy )
{
	SomeHandle bogusHandle(BOGUS_HANDLE);

	SomeHandle copyHandle(bogusHandle);
	BOOST_CHECK_EQUAL( copyHandle.get(), BOGUS_HANDLE );
}

BOOST_AUTO_TEST_CASE( assign )
{
	SomeHandle bogusHandle;

	bogusHandle = BOGUS_HANDLE;
	BOOST_CHECK_EQUAL( bogusHandle.get(), BOGUS_HANDLE );
}

BOOST_AUTO_TEST_CASE( assign_copy )
{
	SomeHandle bogusHandle(BOGUS_HANDLE);
	SomeHandle copyHandle;

	copyHandle = bogusHandle;
	BOOST_CHECK_EQUAL( copyHandle.get(), BOGUS_HANDLE );
}

BOOST_AUTO_TEST_CASE( get )
{
	SomeModule module(BOGUS_INT_HANDLE);

	PrivateHandle handle = module.getHandle().get();
	BOOST_CHECK_EQUAL( handle, BOGUS_HANDLE );

	PrivateInterface *pIfc = module.getInterface().get();
	BOOST_CHECK_EQUAL( pIfc->trueValue, true );
	BOOST_CHECK_EQUAL( pIfc->getTrue(), true );
}

BOOST_AUTO_TEST_CASE( pointer_access )
{
	SomeModule module(BOGUS_INT_HANDLE);

	BOOST_CHECK_EQUAL( module.getInterface()->trueValue, true );
	BOOST_CHECK_EQUAL( module.getInterface()->getTrue(), true );
}

BOOST_AUTO_TEST_CASE( dereference )
{
	SomeModule module(BOGUS_INT_HANDLE);

	PrivateInterface &ifc = *module.getInterface();
	BOOST_CHECK_EQUAL( ifc.trueValue, true );
	BOOST_CHECK_EQUAL( ifc.getTrue(), true );
}

BOOST_AUTO_TEST_CASE( cast )
{
	SomeModule module(BOGUS_INT_HANDLE);

	PrivateHandle handle = module.getHandle();
	BOOST_CHECK_EQUAL( handle, BOGUS_HANDLE );

	PrivateInterface *pIfc = module.getInterface();
	BOOST_CHECK_EQUAL( pIfc->trueValue, true );
	BOOST_CHECK_EQUAL( pIfc->getTrue(), true );
}

BOOST_AUTO_TEST_SUITE_END()
