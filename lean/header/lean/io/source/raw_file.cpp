#ifdef LEAN_BUILD_LIB
#include "lean/config/windows.h"
#endif

#include <windows.h>
#include "../raw_file.h"

// Opens the given file according to the given flags. Throws a runtime_exception on error.
LEAN_MAYBE_INLINE lean::io::raw_file::raw_file(const utf8_ntri &name,
	uint4 access, open_mode mode, uint4 hints, uint4 share)
	: file(access, mode, hints, share)
{
}

// Closes this file.
LEAN_MAYBE_INLINE lean::io::raw_file::~raw_file()
{
}

// Prints the given range of characters to the file. This method is thread-safe.
LEAN_MAYBE_INLINE void lean::io::file::print(const char_ntri &message)
{
	DWORD written;

	// Thread-safe: http://msdn.microsoft.com/en-us/library/ms810467
	::WriteFile(m_handle, message.c_str(), message.size(), &written, NULL);
}
