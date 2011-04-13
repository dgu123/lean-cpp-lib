#ifdef LEAN_BUILD_LIB
#include "lean/config/windows.h"
#endif

#include <windows.h>
#include "../file.h"
#include "../../strings/conversions.h"
#include "../../logging/exceptions.h"

namespace lean
{
namespace io
{
namespace impl
{
	/// Gets the file time null revision offset.
	static inline uint8 get_null_revision_offset()
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

// Opens the given file according to the given flags. Throws a runtime_exception on error.
LEAN_MAYBE_INLINE lean::io::file::file(const utf8_ntri &name,
	uint4 access, uint4 share, open_mode mode, uint4 hints)
	: m_handle(
		::CreateFileW(utf_to_utf16(name).c_str(),
			((access & read) ? GENERIC_READ : 0) | ((access & write) ? GENERIC_WRITE : 0),
			((share & read) ? FILE_SHARE_READ : 0) | ((share & write) ? FILE_SHARE_WRITE : 0),
			NULL,
			((access & read) || (mode == append))
				? OPEN_EXISTING
				: (mode == create)
					? CREATE_NEW
					: (mode == overwrite)
						? CREATE_ALWAYS
						: OPEN_ALWAYS,
			((hints & sequential) ? FILE_FLAG_SEQUENTIAL_SCAN : 0) | ((hints & random) ? FILE_FLAG_RANDOM_ACCESS : 0),
			NULL) )
{
	if (m_handle == INVALID_HANDLE_VALUE)
		LEAN_THROW_ERROR_MSG(name.c_str());
}

// Closes this file.
LEAN_MAYBE_INLINE lean::io::file::~file()
{
	::CloseHandle(m_handle);
}

// Gets the last modification time in microseconds since 1/1/1901.
LEAN_MAYBE_INLINE lean::uint8 lean::io::file::revision() const
{
	static const uint8 null_revision_offset = impl::get_null_revision_offset();
	
	uint8 revision;

	LEAN_ASSERT(sizeof(uint8) == sizeof(::FILETIME));
	::GetFileTime(m_handle, NULL, NULL, reinterpret_cast<FILETIME*>(&revision));

	return (revision - null_revision_offset) / 10ULL;
}

// Gets the size of this file, in bytes.
LEAN_MAYBE_INLINE lean::uint8 lean::io::file::size() const
{
	DWORD sizeLow, sizeHigh;
	sizeLow = ::GetFileSize(m_handle, &sizeHigh);
	return static_cast<uint8>(sizeLow) | (static_cast<uint8>(sizeHigh) << size_info<DWORD>::bits);
}

// Prints the given range of characters to the file. This method is thread-safe.
LEAN_MAYBE_INLINE void lean::io::file::print(const char_ntri &message)
{
	DWORD written;

	// Thread-safe: http://msdn.microsoft.com/en-us/library/ms810467
	::WriteFile(m_handle, message.c_str(), message.size(), &written, NULL);
}
