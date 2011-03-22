/*****************************************************/
/* lean Platform                (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PLATFORM_ATOMIC
#define LEAN_PLATFORM_ATOMIC

#include "../lean.h"

#ifdef _MSC_VER

#include <intrin.h>

namespace lean
{
namespace platform
{
	namespace impl
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

		//// Integers ////

		template <size_t size>
		struct atomic_type
		{
			LEAN_STATIC_ASSERT_MSG_ALT(false,
				"Atomic operations on integers of the given type unsupported."
				Atomic_operations_on_integers_of_the_given_type_unsupported);
		};

		template <> struct atomic_type<sizeof(short)> { typedef short type; };
		template <> struct atomic_type<sizeof(long)> { typedef long type; };

	} // namespace

	using platform::impl::atomic_increment;
	using platform::impl::atomic_decrement;
	using platform::impl::atomic_test_and_set;
	using platform::impl::atomic_set;

} // namespace
} // namespace

#else

#error Unknown compiler, intrinsics unavailable.

#endif

namespace lean
{
namespace platform
{
	//// Integers ////

	/// Atomically increments the given value, returning the results.
	template <class Integer>
	LEAN_INLINE Integer atomic_increment(Integer &value)
	{
		typedef typename impl::atomic_type<sizeof(Integer)>::type atomic_int;

		return reinterpret_cast<Integer>( impl::atomic_increment(
			reinterpret_cast<atomic_int&>(value) ) );
	}

	/// Atomically decrements the given value, returning the results.
	template <class Integer>
	LEAN_INLINE Integer atomic_decrement(Integer &value)
	{
		typedef typename impl::atomic_type<sizeof(Integer)>::type atomic_int;

		return reinterpret_cast<Integer>( impl::atomic_decrement(
			reinterpret_cast<atomic_int&>(value) ) );
	}

	/// Atomically tests if the given value is equal to the given expected value, assigning the given new value on success.
	template <class Integer>
	LEAN_INLINE bool atomic_test_and_set(Integer &value, Integer expectedValue, Integer newValue)
	{
		typedef typename impl::atomic_type<sizeof(Integer)>::type atomic_int;

		return impl::atomic_test_and_set(
			reinterpret_cast<atomic_int&>(value),
			reinterpret_cast<atomic_int>(expectedValue),
			reinterpret_cast<atomic_int>(newValue) );
	}

	/// Atomically sets the given value.
	template <class Integer>
	LEAN_INLINE void atomic_set(Integer &value, Integer newValue)
	{
		typedef typename impl::atomic_type<sizeof(Integer)>::type atomic_int;

		impl::atomic_set(
			reinterpret_cast<atomic_int&>(value),
			reinterpret_cast<atomic_int>(newValue) );
	}

} // namespace

using platform::atomic_increment;
using platform::atomic_decrement;
using platform::atomic_test_and_set;
using platform::atomic_set;

} // namespace

#endif