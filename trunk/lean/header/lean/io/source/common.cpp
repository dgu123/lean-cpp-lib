#ifdef LEAN_BUILD_LIB
#include "../../depconfig.h"
#endif

#include <windows.h>
#include "../../logging/win_errors.h"
#include "../../tags/noncopyable.h"

namespace lean
{
namespace io
{
namespace impl
{

	/// Gets the file time null revision offset (1901-1-1).
	LEAN_NOINLINE uint8 get_null_revision_offset()
	{
		uint8 revision_offset = 0;

		::SYSTEMTIME refTime;
		refTime.wYear = 1901;
		refTime.wMonth = 1;
		refTime.wDay = 1;
		refTime.wHour = 0;
		refTime.wMinute = 0;
		refTime.wSecond = 0;
		refTime.wMilliseconds = 0;

		LEAN_ASSERT(sizeof(uint8) == sizeof(::FILETIME));

		if (!::SystemTimeToFileTime(&refTime, reinterpret_cast<::FILETIME*>(&revision_offset)))
			LEAN_THROW_WIN_ERROR_CTX("SystemTimeToFileTime()", "Null revision offset");

		return revision_offset;
	}

	/// Handle wrapper.
	class handle_guard : public noncopyable
	{
	private:
		HANDLE m_handle;

	public:
		/// Closes the given handle on destruction.
		LEAN_INLINE explicit handle_guard(HANDLE handle)
			: m_handle(handle) { }
		/// Closes the stored handle.
		LEAN_INLINE ~handle_guard()
		{
			if (m_handle != INVALID_HANDLE_VALUE)
				::CloseHandle(m_handle);
		}

		/// Detatches the stored handle.
		LEAN_INLINE HANDLE detatch()
		{
			HANDLE handle = m_handle;
			m_handle = INVALID_HANDLE_VALUE;
			return handle;
		}

		/// Retrieves the stored handle.
		LEAN_INLINE HANDLE get() const { return m_handle; }
		/// Retrieves the stored handle.
		LEAN_INLINE operator HANDLE() const { return get(); }
	};

} // namespace
} // namespace
} // namespace
