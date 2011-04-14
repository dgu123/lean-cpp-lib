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
LEAN_MAYBE_EXPORT size_t lean::io::raw_file::read(char *begin, size_t count)
{
	DWORD read = 0;

	// Thread-safe: http://msdn.microsoft.com/en-us/library/ms810467
	if (!::ReadFile(handle(), begin, count, &read, NULL))
		LEAN_LOG_ERROR("Error reading from file: " << get_last_win_error_msg() << " << " << name() << std::endl);
	
	return read;
}

// Writes the given number of bytes to the file, returning the number of bytes written. This method is thread-safe.
LEAN_MAYBE_EXPORT size_t lean::io::raw_file::write(const char *begin, size_t count)
{
	DWORD written = 0;

	// Thread-safe: http://msdn.microsoft.com/en-us/library/ms810467
	if (!::WriteFile(handle(), begin, count, &written, NULL))
		LEAN_LOG_ERROR("Error writing to file: " << get_last_win_error_msg() << " << " << name() << std::endl);
	
	return written;
}

// Prints the given range of characters to the file. This method is thread-safe.
LEAN_MAYBE_INLINE size_t lean::io::raw_file::print(const char_ntri &message)
{
	return write(message.c_str(), message.size());
}

// Sets the current file cursor position. Throws a runtime_exception on error.
LEAN_MAYBE_INLINE void lean::io::raw_file::pos(uint8 newPos)
{
	LEAN_ASSERT(sizeof(uint8) == sizeof(::LARGE_INTEGER));

	if (!::SetFilePointerEx(handle(), reinterpret_cast<LARGE_INTEGER&>(newPos), NULL, FILE_BEGIN))
		LEAN_THROW_WIN_ERROR_CTX("SetFilePointerEx()", name().c_str());
}

// Gets the current file cursor position. Returns 0 on error.
LEAN_MAYBE_INLINE lean::uint8 lean::io::raw_file::pos() const
{
	const LARGE_INTEGER largeZero = { 0, 0 };
	uint8 pos = 0;

	LEAN_ASSERT(sizeof(uint8) == sizeof(::LARGE_INTEGER));

	::SetFilePointerEx(handle(), largeZero, reinterpret_cast<LARGE_INTEGER*>(&pos), FILE_CURRENT);
	return pos;
}

// Resizes the file, either extending or truncating it. Throws a runtime_exception on error.
LEAN_MAYBE_INLINE void lean::io::raw_file::resize(uint8 newSize)
{
	pos(newSize);

	if (!::SetEndOfFile(handle()))
		LEAN_THROW_WIN_ERROR_CTX("SetEndOfFile()", name().c_str());
}
