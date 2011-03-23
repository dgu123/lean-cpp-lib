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

} // namespace

using concurrent::lock_shared;

} // namespace

#endif