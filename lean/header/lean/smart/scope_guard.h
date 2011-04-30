/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_SCOPE_GUARD
#define LEAN_SMART_SCOPE_GUARD

#include "../lean.h"
#include "../tags/noncopyable.h"

namespace lean
{
namespace smart
{

/// Stores and calls a callable object on destruction.
template <class Callable>
class scope_annex : public noncopyable
{
private:
	Callable m_callable;

public:
	/// Stores the given callable, to be called on destruction.
	LEAN_INLINE explicit scope_annex(const Callable &callable)
		: m_callable(callable) { }
	/// Unlocks the lock object managed by this class.
	LEAN_INLINE ~scope_annex()
	{
		m_callable();
	}
};

/// Stores and calls a callable object on destruction, if not disarmed.
template <class Callable>
class scope_guard : public noncopyable
{
private:
	Callable m_callable;
	bool m_armed;

public:
	/// Stores the given callable, to be called on destruction, if not disarmed.
	LEAN_INLINE explicit scope_guard(const Callable &callable, bool arm = true)
		: m_callable(callable),
		m_armed(arm) { }
	/// Unlocks the lock object managed by this class, if not disarmed.
	LEAN_INLINE ~scope_guard()
	{
		if (m_armed)
			m_callable();
	}

	/// Sets whether the scope guard is currently armed.
	LEAN_INLINE void armed(bool arm) { m_armed = arm; }
	/// Gets whether the scope guard is currently armed.
	LEAN_INLINE bool armed() const { return m_armed; }

	/// Disarms this scope guard.
	LEAN_INLINE void disarm() { armed(false); }
	/// Re-arms this scope guard.
	LEAN_INLINE void arm() { armed(true); }
};

} // namespace

using smart::scope_annex;
using smart::scope_guard;

} // namespace

#endif