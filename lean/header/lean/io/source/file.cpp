#ifdef LEAN_BUILD_LIB
#include "lean/config/windows.h"
#endif

#include <windows.h>
#include "../file.h"
#include "../../strings/conversions.h"
#include "../../logging/exceptions.h"

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

// Prints the given range of characters to the file. This method is thread-safe.
LEAN_MAYBE_INLINE void lean::io::file::print(const char_ntri &message)
{
	DWORD written;

	// Thread-safe: http://msdn.microsoft.com/en-us/library/ms810467
	::WriteFile(m_handle, message.c_str(), message.size(), &written, NULL);
}
