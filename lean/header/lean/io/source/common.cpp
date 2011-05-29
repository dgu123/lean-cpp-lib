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

	/// Gets the file time null revision offset (midnight 1/1/1970).
	LEAN_NOINLINE uint8 get_null_revision_filetime_offset()
	{
		uint8 revisionOffset = 0;

		::SYSTEMTIME refTime;
		refTime.wYear = 1970;
		refTime.wMonth = 1;
		refTime.wDay = 1;
		refTime.wHour = 0;
		refTime.wMinute = 0;
		refTime.wSecond = 0;
		refTime.wMilliseconds = 0;

		LEAN_ASSERT(sizeof(uint8) == sizeof(::FILETIME));

		if (!::SystemTimeToFileTime(&refTime, reinterpret_cast<::FILETIME*>(&revisionOffset)))
			LEAN_THROW_WIN_ERROR_CTX("SystemTimeToFileTime()", "Null revision offset");

		return revisionOffset;
	}

	/// Converts the given windows file time to microseconds since midnight 1/1/1970.
	inline uint8 get_revision_from_filetime(uint8 fileTime)
	{
		static const uint8 nullOffset = impl::get_null_revision_filetime_offset();

		// File time in 100 ns intervals, 100 ns * 10 = 1 mys
		return (fileTime > nullOffset)
			? (fileTime - nullOffset) / 10U
			: 0U;
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
