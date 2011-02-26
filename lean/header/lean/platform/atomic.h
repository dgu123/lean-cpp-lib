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
	template <class Type>
	__forceinline long atomic_decrement(long &value)
	{
		return _InterlockedDecrement(&value);
	}

	/// Atomically tests if the given value is equal to the given expected value, assigning the given new value on success.
	template <class Type>
	__forceinline void atomic_test_and_set(long &value, long expectedValue, long newValue)
	{
		_InterlockedCompareExchange(&value, newValue, expectedValue);
	}

	//// Short ////

	/// Atomically increments the given value, returning the results.
	__forceinline short atomic_increment(short &value)
	{
		return _InterlockedIncrement16(&value);
	}

	/// Atomically decrements the given value, returning the results.
	template <class Type>
	__forceinline short atomic_decrement(short &value)
	{
		return _InterlockedDecrement16(&value);
	}

	/// Atomically tests if the given value is equal to the given expected value, assigning the given new value on success.
	template <class Type>
	__forceinline void atomic_test_and_set(short &value, short expectedValue, short newValue)
	{
		_InterlockedCompareExchange16(&value, newValue, expectedValue);
	}

} // namespace
} // namespace

#endif

namespace lean
{

using platform::atomic_increment;
using platform::atomic_decrement;
using platform::atomic_test_and_set;

} // namespace

#endif