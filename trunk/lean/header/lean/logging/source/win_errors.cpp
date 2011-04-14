#include "../win_errors.h"
#include "../exceptions.h"
#include "../../strings/utility.h"
#include <windows.h>
#include <stdexcept>

// Throws a runtime_error exception containing the last WinAPI error.
LEAN_ALWAYS_LINK void lean::logging::throw_last_win_error(const char *source)
{
	throw_error(source, get_last_win_error_msg().c_str());
}

// Throws a runtime_error exception containing the last WinAPI error.
LEAN_ALWAYS_LINK void lean::logging::throw_last_win_error(const char *source, const char *reason)
{
	if (!reason)
		return throw_last_win_error(source);

	std::string msg = get_last_win_error_msg();
	msg.append(" << ");
	msg.append(reason);

	throw_error(source, msg.c_str());
}

// Gets an error message describing the last WinAPI error that occurred. Returns the number of characters used.
LEAN_ALWAYS_LINK size_t lean::logging::get_last_win_error_msg(char *buffer, size_t maxCount)
{
	size_t count = ::FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, ::GetLastError(),
		0,
		buffer, maxCount,
		nullptr );

	if (count == 0)
		count = strmcpy(buffer, "Error unknown.", maxCount);
	else if (count == maxCount)
		buffer[--count] = 0;

	return count;
}
