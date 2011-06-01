/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_HANDLE_GUARD
#define LEAN_SMART_HANDLE_GUARD

#include "../lean.h"
#include "../tags/noncopyable.h"

namespace lean
{
namespace smart
{

/// Default handle guard policy.
template <class Handle>
struct close_handle_policy
{
	/// Returns an invalid handle value.
	static LEAN_INLINE Handle invalid()
	{
		return INVALID_HANDLE_VALUE;
	}
	/// Releases the given handle by calling CloseHandle.
	static LEAN_INLINE void release(Handle handle)
	{
		if (handle != NULL && handle != INVALID_HANDLE_VALUE)
			::CloseHandle(handle);
	}
};

/// Handle guard that releases the stored handle on destruction.
template < class Handle, class ReleasePolicy = close_handle_policy<Handle> >
class handle_guard : public noncopyable
{
private:
	Handle m_handle;

public:
	/// Releases the given handle on destruction.
	LEAN_INLINE explicit handle_guard(Handle handle)
		: m_handle(handle) { }
	/// Releases the stored handle.
	LEAN_INLINE ~handle_guard()
	{
		ReleasePolicy::release(m_handle);
	}

	/// Detatches the stored handle.
	LEAN_INLINE Handle detatch()
	{
		Handle handle = m_handle;
		m_handle = ReleasePolicy::invalid();
		return handle;
	}

	/// Retrieves the stored handle.
	LEAN_INLINE Handle get() const { return m_handle; }
	/// Retrieves the stored handle.
	LEAN_INLINE operator Handle() const { return get(); }
};

} // namespace

using smart::handle_guard;

} // namespace

#endif