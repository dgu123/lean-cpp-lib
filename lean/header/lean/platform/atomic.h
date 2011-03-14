/*****************************************************/
/* lean Platform                (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PLATFORM_ATOMIC
#define LEAN_PLATFORM_ATOMIC

#ifdef _MSC_VER

#include <intrin.h>

namespace lean
{
namespace platform
{
	//// Long ////

	/// Atomically increments the given value, returning the results.
	__forceinline long atomic_increment(long &value)
	{
		return _InterlockedIncrement(&value);
	}

	/// Atomically decrements the given value, returning the results.
	__forceinline long atomic_decrement(long &value)
	{
		return _InterlockedDecrement(&value);
	}

	/// Atomically tests if the given value is equal to the given expected value, assigning the given new value on success.
	__forceinline bool atomic_test_and_set(long &value, long expectedValue, long newValue)
	{
		return (_InterlockedCompareExchange(&value, newValue, expectedValue) == expectedValue);
	}

	/// Atomically sets the given value.
	__forceinline void atomic_set(long &value, long newValue)
	{
		_InterlockedExchange(&value, newValue);
	}

	//// Short ////

	/// Atomically increments the given value, returning the results.
	__forceinline short atomic_increment(short &value)
	{
		return _InterlockedIncrement16(&value);
	}

	/// Atomically decrements the given value, returning the results.
	__forceinline short atomic_decrement(short &value)
	{
		return _InterlockedDecrement16(&value);
	}

	/// Atomically tests if the given value is equal to the given expected value, assigning the given new value on success.
	__forceinline bool atomic_test_and_set(short &value, short expectedValue, short newValue)
	{
		return (_InterlockedCompareExchange16(&value, newValue, expectedValue) == expectedValue);
	}

	/// Atomically sets the given value.
	__forceinline void atomic_set(short &value, short newValue)
	{
		_InterlockedExchange16(&value, newValue);
	}

} // namespace
} // namespace


#else

#error Unknown compiler, intrinsics unavailable.

#endif

namespace lean
{

using platform::atomic_increment;
using platform::atomic_decrement;
using platform::atomic_test_and_set;
using platform::atomic_set;

} // namespace

#endif