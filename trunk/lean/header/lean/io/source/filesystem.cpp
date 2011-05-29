#ifdef LEAN_BUILD_LIB
#include "../../depconfig.h"
#endif

#include <windows.h>
#include "../filesystem.h"
#include "../../logging/log.h"
#include "../../logging/win_errors.h"

namespace lean
{
namespace io
{
namespace impl
{
	/// Gets the file time null revision offset.
	inline uint8 get_null_revision_offset()
	{
		SYSTEMTIME refTime;
		refTime.wYear = 1901;
		refTime.wMonth = 1;
		refTime.wDay = 1;
		refTime.wHour = 0;
		refTime.wMinute = 0;
		refTime.wSecond = 0;
		refTime.wMilliseconds = 0;

		uint8 revision_offset;

		LEAN_ASSERT(sizeof(uint8) == sizeof(::FILETIME));
		SystemTimeToFileTime(&refTime, reinterpret_cast<FILETIME*>(&revision_offset));

		return revision_offset;
	}

} // namespace
} // namespace
} // namespace

// Checks whether the given file exists.
LEAN_MAYBE_LINK bool lean::io::file_exists(const utf16_nti& file)
{
	return (::GetFileAttributesW(file.c_str()) != INVALID_FILE_ATTRIBUTES);
}

// Gets the last modification time in microseconds since 1/1/1901. Returns 0 on error.
LEAN_MAYBE_LINK lean::uint8 lean::io::file_revision(const utf16_nti& file)
{
	uint8 revision = 0;

	LEAN_ASSERT(sizeof(uint8) == sizeof(::FILETIME));

	HANDLE hFile = ::CreateFileW(file.c_str(),
		FILE_READ_ATTRIBUTES,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		LEAN_LOG_ERROR("Error opening file for revision: " << get_last_win_error_msg() << " << " << file.c_str());
	else
	{
		if (!::GetFileTime(hFile, nullptr, nullptr, reinterpret_cast<FILETIME*>(&revision)))
			LEAN_LOG_ERROR("Error reading file revision: " << get_last_win_error_msg() << " << " << file.c_str());

		::CloseHandle(hFile);

		static const uint8 null_revision_offset = impl::get_null_revision_offset();
		
		if (revision != 0)
			revision = (revision - null_revision_offset) / 10U;
	}

	return revision;
}

// Gets the size of the given file, in bytes. Returns 0 on error.
LEAN_MAYBE_LINK lean::uint8 lean::io::file_size(const utf16_nti& file)
{
	uint8 size = 0;

	LEAN_ASSERT(sizeof(uint8) == sizeof(::LARGE_INTEGER));

	HANDLE hFile = ::CreateFileW(file.c_str(),
		FILE_READ_ATTRIBUTES,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		LEAN_LOG_ERROR("Error opening file for size: " << get_last_win_error_msg() << " << " << file.c_str());
	else
	{
		if (!::GetFileSizeEx(hFile, reinterpret_cast<LARGE_INTEGER*>(&size)))
			LEAN_LOG_ERROR("Error reading file size: " << get_last_win_error_msg() << " << " << file.c_str());

		::CloseHandle(hFile);
	}

	return size;
}
