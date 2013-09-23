#include "stdafx.h"
#include <lean/smart/resource_ptr.h>

#include "resource_test_impl.h"

BOOST_AUTO_TEST_SUITE( resource_ptr )

BOOST_AUTO_TEST_CASE( basic_destruct )
{
	int constrCount = 0, destrCount = 0;
	
	{
		test_resource *resource = new test_resource(&constrCount, &destrCount);
		lean::resource_ptr<test_resource> resourcePtr = lean::bind_resource(resource);

		BOOST_CHECK_EQUAL(constrCount, 1);
		BOOST_CHECK_EQUAL(destrCount, 0);

		BOOST_CHECK_EQUAL(&(*resourcePtr), resource);
		BOOST_CHECK_EQUAL(resourcePtr.get(), resource);
		BOOST_CHECK_EQUAL(resourcePtr, resource);
	}

	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 1);
}

BOOST_AUTO_TEST_CASE( assign_destruct )
{
	int constrCount = 0, destrCount = 0;

	{
		lean::resource_ptr<test_resource> resourcePtrG;
	
		{
			lean::resource_ptr<test_resource> resourcePtr =
				lean::bind_resource( new test_resource(&constrCount, &destrCount) );

			resourcePtrG = resourcePtr;

			BOOST_CHECK_EQUAL(constrCount, 1);
			BOOST_CHECK_EQUAL(destrCount, 0);
		}

		BOOST_CHECK_EQUAL(constrCount, 1);
		BOOST_CHECK_EQUAL(destrCount, 0);
	}
	
	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 1);
}

BOOST_AUTO_TEST_CASE( raw_assign_destruct )
{
	int constrCount = 0, destrCount = 0;

	{
		lean::resource_ptr<test_resource> resourcePtrG;
	
		{
			lean::resource_ptr<test_resource> resourcePtr =
				lean::bind_resource( new test_resource(&constrCount, &destrCount) );

			resourcePtrG = resourcePtr.get();

			BOOST_CHECK_EQUAL(constrCount, 1);
			BOOST_CHECK_EQUAL(destrCount, 0);
		}

		BOOST_CHECK_EQUAL(constrCount, 1);
		BOOST_CHECK_EQUAL(destrCount, 0);
	}
	
	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 1);
}

BOOST_AUTO_TEST_CASE( copy_destruct )
{
	int constrCount = 0, destrCount = 0;
	lean::resource_ptr<test_resource> *resourcePtrG;

	{
		lean::resource_ptr<test_resource> resourcePtr =
			lean::bind_resource( new test_resource(&constrCount, &destrCount) );

		resourcePtrG = new lean::resource_ptr<test_resource>(resourcePtr);

		BOOST_CHECK_EQUAL(constrCount, 1);
		BOOST_CHECK_EQUAL(destrCount, 0);
	}

	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 0);

	delete resourcePtrG;

	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 1);
}

BOOST_AUTO_TEST_CASE( raw_copy_destruct )
{
	int constrCount = 0, destrCount = 0;
	lean::resource_ptr<test_resource> *resourcePtrG;

	{
		lean::resource_ptr<test_resource> resourcePtr =
			lean::bind_resource( new test_resource(&constrCount, &destrCount) );

		resourcePtrG = new lean::resource_ptr<test_resource>(resourcePtr.get());

		BOOST_CHECK_EQUAL(constrCount, 1);
		BOOST_CHECK_EQUAL(destrCount, 0);
	}

	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 0);

	delete resourcePtrG;

	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 1);
}

BOOST_AUTO_TEST_CASE( move )
{
	int constrCount = 0, destrCount = 0;
	lean::resource_ptr<test_resource> *resourcePtrG;

	{
		lean::resource_ptr<test_resource> resourcePtr =
			lean::bind_resource( new test_resource(&constrCount, &destrCount) );

		resourcePtrG = new lean::resource_ptr<test_resource>(std::move(resourcePtr));

		BOOST_CHECK_EQUAL(constrCount, 1);
		BOOST_CHECK_EQUAL(destrCount, 0);
	}

	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 0);

	delete resourcePtrG;

	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 1);
}

BOOST_AUTO_TEST_CASE( move_assign )
{
	int constrCount = 0, destrCount = 0;

	{
		lean::resource_ptr<test_resource> resourcePtrG;

		{
			lean::resource_ptr<test_resource> resourcePtr =
				lean::bind_resource( new test_resource(&constrCount, &destrCount) );

			resourcePtrG = std::move(resourcePtr);

			BOOST_CHECK_EQUAL(constrCount, 1);
			BOOST_CHECK_EQUAL(destrCount, 0);
		}

		BOOST_CHECK_EQUAL(constrCount, 1);
		BOOST_CHECK_EQUAL(destrCount, 0);
	}

	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 1);
}

BOOST_AUTO_TEST_CASE( transfer )
{
	int constrCount = 0, destrCount = 0;

	{
		lean::resource_ptr<test_resource> resourcePtrG;

		{
			lean::resource_ptr<test_resource> resourcePtr =
				lean::bind_resource( new test_resource(&constrCount, &destrCount) );

			resourcePtrG = resourcePtr.transfer();

			BOOST_CHECK_EQUAL(constrCount, 1);
			BOOST_CHECK_EQUAL(destrCount, 0);
		}

		BOOST_CHECK_EQUAL(constrCount, 1);
		BOOST_CHECK_EQUAL(destrCount, 0);
	}

	BOOST_CHECK_EQUAL(constrCount, 1);
	BOOST_CHECK_EQUAL(destrCount, 1);
}

BOOST_AUTO_TEST_SUITE_END()
