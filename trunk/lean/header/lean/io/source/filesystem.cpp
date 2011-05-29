#ifdef LEAN_BUILD_LIB
#include "../../depconfig.h"
#endif

#include <windows.h>
#include "../filesystem.h"
#include "../../logging/log.h"
#include "../../logging/win_errors.h"

#include "common.cpp"

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

	impl::handle_guard fileHandle( ::CreateFileW(file.c_str(),
		FILE_READ_ATTRIBUTES,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL) );

	if (fileHandle == INVALID_HANDLE_VALUE)
		LEAN_LOG_ERROR("Error opening file for revision: " << get_last_win_error_msg() << " << " << file.c_str());
	else
	{
		if (!::GetFileTime(fileHandle, nullptr, nullptr, reinterpret_cast<FILETIME*>(&revision)))
			LEAN_LOG_ERROR("Error reading file revision: " << get_last_win_error_msg() << " << " << file.c_str());

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

	impl::handle_guard fileHandle( ::CreateFileW(file.c_str(),
		FILE_READ_ATTRIBUTES,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL) );

	if (fileHandle == INVALID_HANDLE_VALUE)
		LEAN_LOG_ERROR("Error opening file for size: " << get_last_win_error_msg() << " << " << file.c_str());
	else if (!::GetFileSizeEx(fileHandle, reinterpret_cast<LARGE_INTEGER*>(&size)))
		LEAN_LOG_ERROR("Error reading file size: " << get_last_win_error_msg() << " << " << file.c_str());

	return size;
}
