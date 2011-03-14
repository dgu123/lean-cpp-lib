#include "stdafx.h"
#include <lean/smart/ref_counter.h>

#include "allocator_test_impl.h"

BOOST_AUTO_TEST_SUITE( ref_counter )

BOOST_AUTO_TEST_CASE( basic_counting )
{
	lean::ref_counter<> refCounter1;
	BOOST_CHECK_EQUAL(refCounter1.valid(), true);
	BOOST_CHECK_EQUAL(refCounter1.count(), 1);
	BOOST_CHECK_EQUAL(refCounter1.weak_count(), 1);
	
	BOOST_CHECK_EQUAL((long) ++refCounter1, 2);

	BOOST_CHECK_EQUAL(refCounter1++, 2);
	BOOST_CHECK_EQUAL((long) refCounter1, 3);

	lean::ref_counter<> refCounter2 = refCounter1;
	BOOST_CHECK_EQUAL(refCounter1.weak_count(), 2);
	BOOST_CHECK_EQUAL(refCounter2.weak_count(), 2);

	BOOST_CHECK_EQUAL(refCounter2--, 3);
	BOOST_CHECK_EQUAL((long) refCounter2, 2);
	BOOST_CHECK_EQUAL((long) refCounter1, 2);

	BOOST_CHECK_EQUAL((long) --refCounter2, 1);
	BOOST_CHECK_EQUAL((long) --refCounter1, 0);

	BOOST_CHECK_EQUAL(refCounter2.valid(), false);
	BOOST_CHECK_EQUAL((bool) refCounter1, false);
}

BOOST_AUTO_TEST_CASE( allocator_destruction )
{
	int allocCount = 0, constructCount = 0, destructCount = 0, deallocCount = 0;
	test_allocator<long> alloc(&allocCount, &constructCount, &destructCount, &deallocCount);

	lean::ref_counter< long, test_allocator<long> > *refCounterG2;

	{
		lean::ref_counter< long, test_allocator<long> > refCounterG1(1, alloc);

		BOOST_CHECK_EQUAL(allocCount, 1);

		{
			lean::ref_counter< long, test_allocator<long> > refCounter1(alloc);
			BOOST_CHECK_EQUAL(allocCount, 2);

			lean::ref_counter< long, test_allocator<long> > refCounter2(alloc);
			BOOST_CHECK_EQUAL(allocCount, 3);

			lean::ref_counter< long, test_allocator<long> > refCounter3(alloc);
			BOOST_CHECK_EQUAL(allocCount, 4);

			refCounterG2 = new lean::ref_counter< long, test_allocator<long> >(refCounter2);
			
			BOOST_CHECK_EQUAL(allocCount, 4);
			BOOST_CHECK_EQUAL(deallocCount, 0);

			refCounterG1 = refCounter3;

			BOOST_CHECK_EQUAL(allocCount, 4);
			BOOST_CHECK_EQUAL(deallocCount, 1);
		}

		BOOST_CHECK_EQUAL(allocCount, 4);
		BOOST_CHECK_EQUAL(deallocCount, 2);
	}

	BOOST_CHECK_EQUAL(allocCount, 4);
	BOOST_CHECK_EQUAL(deallocCount, 3);

	delete refCounterG2;

	BOOST_CHECK_EQUAL(allocCount, 4);
	BOOST_CHECK_EQUAL(deallocCount, 4);
}

BOOST_AUTO_TEST_CASE( null_fixedref_assign )
{
	lean::ref_counter<> refCounter1(lean::ref_counter<>::null());
	lean::ref_counter<> refCounter2(2);
	lean::ref_counter<> refCounter3(3);
	lean::ref_counter<> refCounter4(4);

	BOOST_CHECK_EQUAL(refCounter2.count(), 2);
	BOOST_CHECK_EQUAL(refCounter3.count(), 3);
	BOOST_CHECK_EQUAL(refCounter4.count(), 4);
	
	refCounter3 = refCounter4;

	BOOST_CHECK_EQUAL(refCounter3.count(), 4);
	BOOST_CHECK_EQUAL(refCounter3.weak_count(), 2);
	BOOST_CHECK_EQUAL(refCounter4.count(), 4);
	BOOST_CHECK_EQUAL(refCounter4.weak_count(), 2);

	refCounter3 = refCounter1;
	refCounter1 = refCounter2;

	BOOST_CHECK_EQUAL(refCounter1.count(), 2);
	BOOST_CHECK_EQUAL(refCounter1.weak_count(), 2);
	BOOST_CHECK_EQUAL(refCounter2.count(), 2);
	BOOST_CHECK_EQUAL(refCounter2.weak_count(), 2);

	refCounter4 = refCounter1;

	BOOST_CHECK_EQUAL((long) --refCounter4, 1);
	BOOST_CHECK_EQUAL(refCounter4.count(), 1);
	BOOST_CHECK_EQUAL(refCounter4.weak_count(), 3);
	BOOST_CHECK_EQUAL(refCounter2.count(), 1);
	BOOST_CHECK_EQUAL(refCounter2.weak_count(), 3);
	BOOST_CHECK_EQUAL(refCounter1.count(), 1);
	BOOST_CHECK_EQUAL(refCounter1.weak_count(), 3);
	
}

BOOST_AUTO_TEST_CASE( move_assign )
{
	int allocCount = 0, constructCount = 0, destructCount = 0, deallocCount = 0;
	test_allocator<long> alloc(&allocCount, &constructCount, &destructCount, &deallocCount);

	lean::ref_counter< long, test_allocator<long> > *refCounterG2;

	{
		lean::ref_counter< long, test_allocator<long> > refCounterG1(1, alloc);

		BOOST_CHECK_EQUAL(allocCount, 1);

		{
			lean::ref_counter< long, test_allocator<long> > refCounter1(13, alloc);
			lean::ref_counter< long, test_allocator<long> > refCounter2(27, alloc);
			BOOST_CHECK_EQUAL(allocCount, 3);

			refCounterG2 = new lean::ref_counter< long, test_allocator<long> >(std::move(refCounter2));
			BOOST_CHECK_EQUAL(allocCount, 3);
			BOOST_CHECK_EQUAL(deallocCount, 0);

			refCounterG1 = std::move(refCounter1);
			BOOST_CHECK_EQUAL(allocCount, 3);
			BOOST_CHECK_EQUAL(deallocCount, 1);
		}

		BOOST_CHECK_EQUAL(allocCount, 3);
		BOOST_CHECK_EQUAL(deallocCount, 1);
		BOOST_CHECK_EQUAL(refCounterG1.count(), 13);
		BOOST_CHECK_EQUAL(refCounterG2->count(), 27);
	}

	BOOST_CHECK_EQUAL(allocCount, 3);
	BOOST_CHECK_EQUAL(deallocCount, 2);

	delete refCounterG2;

	BOOST_CHECK_EQUAL(allocCount, 3);
	BOOST_CHECK_EQUAL(deallocCount, 3);
}

BOOST_AUTO_TEST_CASE( increment_checked )
{
	lean::ref_counter<> refCounter;

	BOOST_CHECK_EQUAL(refCounter.increment_checked(), true);
	BOOST_CHECK_EQUAL(refCounter.increment(), 3);

	BOOST_CHECK_EQUAL(refCounter.decrement(), 2);
	BOOST_CHECK_EQUAL(refCounter.decrement(), 1);
	BOOST_CHECK_EQUAL(refCounter.decrement(), 0);

	BOOST_CHECK_EQUAL(refCounter.increment_checked(), false);
	BOOST_CHECK_EQUAL(refCounter.valid(), false);
	BOOST_CHECK_EQUAL(refCounter.count(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
