#include "stdafx.h"
#include <lean/pimpl/pimpl_ptr.h>
#include "module_test_impl.h"

BOOST_AUTO_TEST_SUITE( pimpl_ptr )

template <class Implementation, class ImplementationBase>
struct test_extension
{
	typedef void (*function_type)(lean::pimpl_ptr<Implementation, ImplementationBase>&);
};

template <class Implementation, class ImplementationBase>
void assign_unbind_destruct_test(lean::pimpl_ptr<Implementation, ImplementationBase> &pimplPtr)
{
	int ctrCalls = 0, dtrCalls = 0;
	Implementation *impl = new Implementation(&ctrCalls, &dtrCalls);
	
	pimplPtr = impl;
	BOOST_CHECK( !pimplPtr.empty() );
	BOOST_CHECK_EQUAL( pimplPtr.getptr(), impl );
	BOOST_CHECK_EQUAL( ctrCalls, 1 );
	BOOST_CHECK_EQUAL( dtrCalls, 0 );

	pimplPtr = impl;
	BOOST_CHECK( !pimplPtr.empty() );
	BOOST_CHECK_EQUAL( pimplPtr.getptr(), impl );
	BOOST_CHECK_EQUAL( ctrCalls, 1 );
	BOOST_CHECK_EQUAL( dtrCalls, 0 );

	BOOST_CHECK_EQUAL( pimplPtr.unbind(), impl );
	BOOST_CHECK( pimplPtr.empty() );
	BOOST_CHECK( !pimplPtr.getptr() );
	BOOST_CHECK_EQUAL( ctrCalls, 1 );
	BOOST_CHECK_EQUAL( dtrCalls, 0 );

	pimplPtr = impl;
	pimplPtr = nullptr;
	BOOST_CHECK( pimplPtr.empty() );
	BOOST_CHECK( !pimplPtr.getptr() );
	BOOST_CHECK_EQUAL( ctrCalls, 1 );
	BOOST_CHECK_EQUAL( dtrCalls, 1 );
}

template <class Implementation, class ImplementationBase>
void empty_destruct_test(
	typename test_extension<Implementation, ImplementationBase>::function_type testExtension)
{
	lean::pimpl_ptr<Implementation, ImplementationBase> emptyPtr(nullptr);
	BOOST_CHECK( emptyPtr.empty() );
	BOOST_CHECK( !emptyPtr.getptr() );
	
	BOOST_CHECK( !emptyPtr.unbind() );
	BOOST_CHECK( !emptyPtr.getptr() );

	if (testExtension)
		testExtension(emptyPtr);
}

template <class Implementation, class ImplementationBase>
void init_destruct_test(
	typename test_extension<Implementation, ImplementationBase>::function_type testExtension)
{
	int ctrCalls = 0, dtrCalls = 0;
	Implementation *impl = new Implementation(&ctrCalls, &dtrCalls);

	{
		lean::pimpl_ptr<Implementation, ImplementationBase> initPtr(impl);
		BOOST_CHECK( !initPtr.empty() );
		BOOST_CHECK_EQUAL( initPtr.getptr(), impl );
		BOOST_CHECK_EQUAL( ctrCalls, 1 );
		BOOST_CHECK_EQUAL( dtrCalls, 0 );

		if (testExtension)
			testExtension(initPtr);
	}

	BOOST_CHECK_EQUAL( ctrCalls, 1 );
	BOOST_CHECK_EQUAL( dtrCalls, 1 );
}

BOOST_AUTO_TEST_CASE( empty_destruct )
{
	empty_destruct_test<UnsafeModuleTestImpl, UnsafeModuleTestImpl>(nullptr);
}

BOOST_AUTO_TEST_CASE( empty_assign_unbind_destruct )
{
	empty_destruct_test<UnsafeModuleTestImpl, UnsafeModuleTestImpl>(
		&assign_unbind_destruct_test<UnsafeModuleTestImpl, UnsafeModuleTestImpl>);
}

BOOST_AUTO_TEST_CASE( init_destruct )
{
	init_destruct_test<UnsafeModuleTestImpl, UnsafeModuleTestImpl>(nullptr);
}

BOOST_AUTO_TEST_CASE( init_assign_unbind_destruct )
{
	init_destruct_test<UnsafeModuleTestImpl, UnsafeModuleTestImpl>(
		&assign_unbind_destruct_test<UnsafeModuleTestImpl, UnsafeModuleTestImpl>);
}

BOOST_AUTO_TEST_SUITE_END()
