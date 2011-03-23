/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONCURRENT_LOCKSHARED
#define LEAN_CONCURRENT_LOCKSHARED

#include "../lean.h"
#include "../tags/noncopyable.h"

namespace lean
{
namespace concurrent
{

/// Adapts the shared locking functionality of a sharable lock to the standard lock interface.
template <class ShareableLock>
class lock_shared : public nonassignable
{
private:
	ShareableLock &m_lock;

public:
	/// Type of the underlying shareable lock.
	typedef ShareableLock lock_type;

	/// Constructs an shareable lock adapter from the given shareable lock.
	explicit LEAN_INLINE lock_shared(lock_type &lock)
		: m_lock(lock) {  }

	/// Calls try_lock_shared on the underlying shareable lock.
	LEAN_INLINE bool try_lock()
	{
		return m_lock.try_lock_shared();
	}

	/// Calls lock_shared on the underlying shareable lock.
	LEAN_INLINE void lock()
	{
		m_lock.lock_shared();
	}

	/// Calls unlock_shared on the underlying shareable lock.
	LEAN_INLINE void unlock()
	{
		m_lock.unlock_shared();
	}
};

/// Adapts the upgrading functionality of a sharable lock to the standard lock interface.
template <class ShareableLock>
class upgrade_lock : public nonassignable
{
private:
	ShareableLock &m_lock;

public:
	/// Type of the underlying shareable lock.
	typedef ShareableLock lock_type;

	/// Constructs an shareable lock adapter from the given shareable lock.
	explicit LEAN_INLINE upgrade_lock(lock_type &lock)
		: m_lock(lock) {  }

	/// Calls try_upgrade_lock on the underlying shareable lock.
	LEAN_INLINE bool try_lock()
	{
		return m_lock.try_upgrade_lock();
	}

	/// Calls upgrade_lock on the underlying shareable lock.
	LEAN_INLINE void lock()
	{
		m_lock.upgrade_lock();
	}

	/// Calls downgrade_lock on the underlying shareable lock.
	LEAN_INLINE void unlock()
	{
		m_lock.downgrade_lock();
	}
};

} // namespace

using concurrent::lock_shared;
using concurrent::upgrade_lock;

} // namespace

#endif