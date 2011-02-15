#include "stdafx.h"
#include "pimpl_modules.h"

BOOST_AUTO_TEST_SUITE( pimpl_modules )

template <class ModuleType>
void module_check()
{
	int ctrCalls = 0, dtrCalls = 0;
	ModuleType(ctrCalls, dtrCalls);

	BOOST_CHECK_EQUAL(ctrCalls, 1);
	BOOST_CHECK_EQUAL(dtrCalls, 1);
}

BOOST_AUTO_TEST_CASE( fixed_module )
{
	module_check<FixedModule>();
}

BOOST_AUTO_TEST_CASE( unsafe_pimpl_base_fixed_module )
{
    module_check<UnsafeFixedModule>();
}

BOOST_AUTO_TEST_CASE( safe_pimpl_base_fixed_module )
{
    module_check<SafeFixedModule>();
}

BOOST_AUTO_TEST_CASE( safe_pimpl_base_header_module )
{
    module_check<SafeHeaderModule>();
}

BOOST_AUTO_TEST_SUITE_END()
