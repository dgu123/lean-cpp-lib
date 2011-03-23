/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_SCOPED_LOCK
#define LEAN_SMART_SCOPED_LOCK

#include "../lean.h"
#include "../tags/noncopyable.h"

namespace lean
{
namespace smart
{

/// Default locking policy.
template <class Lockable>
struct default_locking_policy
{
	/// Calls try_lock on the given lock object.
	LEAN_INLINE void try_lock(Lockable &lock)
	{
		lock.try_lock();
	}
	/// Calls lock on the given lock object.
	LEAN_INLINE void lock(Lockable &lock)
	{
		lock.lock();
	}
	/// Calls unlock on the given lock object.
	LEAN_INLINE void unlock(Lockable &lock)
	{
		lock.unlock();
	}
};

/// Automatic lock management class that locks a given object on construction to be unlocked on destruction.
template < class Lockable, class Policy = default_locking_policy<Lockable> >
class scoped_lock : public noncopyable
{
private:
	Lockable &m_lock;

public:
	/// Type of the lock managed by this class.
	typedef Lockable lock_type;
	/// Type of the locking policy used by this class.
	typedef Policy policy_type;

	/// Locks the given object, to be unlocked on destruction.
	LEAN_INLINE explicit scoped_lock(lock_type &lock)
		: m_lock(lock)
	{
		policy_type::lock(m_lock);
	}
	/// Unlocks the lock object managed by this class.
	LEAN_INLINE ~scoped_lock()
	{
		policy_type::unlock(m_lock);
	}
	
	/// Gets the lock object managed by this class.
	LEAN_INLINE lock_type& get(void) { return m_lock; };
	/// Gets the lock object managed by this class.
	LEAN_INLINE const lock_type& get(void) const { return m_lock; };
};

} // namespace

using smart::default_locking_policy;
using smart::scoped_lock;

} // namespace

#endif