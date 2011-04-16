#ifdef LEAN_BUILD_LIB
#include "lean/config/windows.h"
#endif

#include <windows.h>
#include "../raw_file.h"
#include "../../logging/log.h"
#include "../../logging/win_errors.h"

// Opens the given file according to the given flags. Throws a runtime_exception on error.
LEAN_MAYBE_INLINE lean::io::raw_file::raw_file(const utf8_ntri &name,
	uint4 access, open_mode mode, uint4 hints, uint4 share)
	: file(name, access, mode, hints, share)
{
}

// Closes this file.
LEAN_MAYBE_INLINE lean::io::raw_file::~raw_file()
{
}

// Reads the given number of bytes from the file, returning the number of bytes read. This method is thread-safe.
LEAN_MAYBE_INLINE size_t lean::io::raw_file::read(char *begin, size_t count) const
{
	DWORD read = 0;

	// Thread-safe: http://msdn.microsoft.com/en-us/library/ms810467
	if (!::ReadFile(handle(), begin, count, &read, nullptr))
		LEAN_LOG_ERROR("Error reading from file: " << get_last_win_error_msg() << " << " << name() << std::endl);
	
	return read;
}

// Writes the given number of bytes to the file, returning the number of bytes written. This method is thread-safe.
LEAN_MAYBE_INLINE size_t lean::io::raw_file::write(const char *begin, size_t count)
{
	DWORD written = 0;

	// Thread-safe: http://msdn.microsoft.com/en-us/library/ms810467
	if (!::WriteFile(handle(), begin, count, &written, nullptr))
		LEAN_LOG_ERROR("Error writing to file: " << get_last_win_error_msg() << " << " << name() << std::endl);
	
	return written;
}

// Prints the given range of characters to the file. This method is thread-safe.
LEAN_MAYBE_INLINE size_t lean::io::raw_file::print(const char_ntri &message)
{
	return write(message.c_str(), message.size());
}
