#ifdef LEAN_BUILD_LIB
#include "lean/config/windows.h"
#endif

#include <windows.h>
#include "../log_file.h"
#include "../../strings/conversions.h"
#include "../../logging/log.h"
#include "../../logging/win_errors.h"

// Opens the given file for logging.
LEAN_ALWAYS_LINK lean::logging::log_file::log_file(const utf8_ntri &name)
	: m_handle(
		::CreateFileW(utf_to_utf16(name).c_str(),
			GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS,
			0, NULL) )
{
	if (m_handle == INVALID_HANDLE_VALUE)
		LEAN_LOG_ERROR("Error creating log file: " << get_last_win_error_msg() << " << " << name.c_str() << std::endl);
}

// Closes the log file.
LEAN_ALWAYS_LINK lean::logging::log_file::~log_file()
{
	if (m_handle != INVALID_HANDLE_VALUE)
		::CloseHandle(m_handle);
}

// Gets whether the file was opened successfully.
LEAN_ALWAYS_LINK bool lean::logging::log_file::valid() const
{
	return (m_handle != INVALID_HANDLE_VALUE);
}

// Prints the given message to the log file. This method is thread-safe.
LEAN_ALWAYS_LINK void lean::logging::log_file::print(const char_ntri &message)
{
	DWORD written;

	// Thread-safe: http://msdn.microsoft.com/en-us/library/ms810467
	if (m_handle != INVALID_HANDLE_VALUE)
		::WriteFile(m_handle, message.c_str(), message.size(), &written, NULL);
}
