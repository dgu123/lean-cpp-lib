/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONCURRENT_SHARABLE_SPINLOCK
#define LEAN_CONCURRENT_SHARABLE_SPINLOCK

#include "../lean.h"
#include "../tags/noncopyable.h"
#include "../platform/atomic.h"

namespace lean
{
namespace concurrent
{

/// Implements a sharable spin lock that is NOT reentrant.
template <class Counter = long>
class shareable_spin_lock : public noncopyable
{
private:
	Counter m_counter;

public:
	/// Constructs a shareable spin lock.
	shareable_spin_lock() : m_counter(0) {  }

	/// Tries to exclusively lock this spin lock, returning false if currently locked / shared by another user.
	LEAN_INLINE bool try_lock()
	{
		return atomic_test_and_set(m_counter, 0, reinterpret_cast<Counter>(-1));
	}

	/// Tries to atomically upgrade shared ownership of this spin lock to exclusive ownership,
	/// returning false if currently shared with another user.
	LEAN_INLINE bool try_upgrade_lock()
	{
		return atomic_test_and_set(m_counter, 1, reinterpret_cast<Counter>(-1));
	}

	/// Exclusively locks this spin lock, returning immediately on success, otherwise waiting for the lock to become available.
	LEAN_INLINE void lock()
	{
		while (!try_lock());
	}

	/// Atomically upgrades shared ownership of this spin lock to exclusive ownership,
	/// returning immediately on success, otherwise waiting for the lock to become available.
	LEAN_INLINE void upgrade_lock()
	{
		while (!try_upgrade_lock());
	}

	/// Atomically releases exclusive ownership and re-acquires shared ownership, permitting waiting threads to continue execution.
	LEAN_INLINE void downgrade_lock()
	{
		atomic_test_and_set(m_counter, reinterpret_cast<Counter>(-1), 1);
	}

	/// Unlocks this spin lock, permitting waiting threads to continue execution.
	LEAN_INLINE void unlock()
	{
		atomic_test_and_set(m_counter, reinterpret_cast<Counter>(-1), 0);
	}

	/// Tries to obtain shared ownership of this spin lock, returning false if currently locked exclusively by another user.
	LEAN_INLINE bool try_lock_shared()
	{
		for (;;)
		{
			Counter counter = static_cast<volatile Counter&>(m_counter);

			if (counter == reinterpret_cast<Counter>(-1))
				return false;
			else if (atomic_test_and_set(m_counter, counter, counter + 1))
				return true;
		}

		LEAN_ASSERT(0);
	}

	/// Obtains shared ownership of this spin lock, returning immediately on success, otherwise waiting for the lock to become available.
	LEAN_INLINE void lock_shared()
	{
		while (!try_lock_shared());
	}

	/// Releases shared ownership of this spin lock, permitting waiting threads to continue execution.
	LEAN_INLINE void unlock_shared()
	{
		for (;;)
		{
			Counter counter = static_cast<volatile Counter&>(m_counter);

			if (counter == reinterpret_cast<Counter>(-1) ||
				counter == 0 ||
				atomic_test_and_set(m_counter, counter, counter - 1))
				return;
		}

		LEAN_ASSERT(0);
	}
};

} // namespace

using concurrent::shareable_spin_lock;

} // namespace

#endif