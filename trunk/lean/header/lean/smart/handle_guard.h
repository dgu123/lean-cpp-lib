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
	/// Releases the given handle by calling @code CloseHandle()@endcode.
	static LEAN_INLINE void release(Handle handle)
	{
		if (handle != NULL && handle != INVALID_HANDLE_VALUE)
			::CloseHandle(handle);
	}
};

/// Window handle guard policy.
template <class Handle>
struct destroy_window_policy
{
	/// Returns an invalid handle value.
	static LEAN_INLINE Handle invalid()
	{
		return NULL;
	}
	/// Releases the given handle by calling @code DestroyWindow()@endcode.
	static LEAN_INLINE void release(Handle handle)
	{
		if (handle != NULL)
			::DestroyWindow(handle);
	}
};

/// Dll handle guard policy.
template <class Handle>
struct free_library_policy
{
	/// Returns an invalid handle value.
	static LEAN_INLINE Handle invalid()
	{
		return NULL;
	}
	/// Releases the given handle by calling @code FreeLibrary()@endcode.
	static LEAN_INLINE void release(Handle handle)
	{
		if (handle != NULL)
			::FreeLibrary(handle);
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

	/// Detaches the stored handle.
	LEAN_INLINE Handle detach()
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