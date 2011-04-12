#ifdef LEAN_BUILD_LIB
#include "lean/config/windows.h"
#endif

#include "../log_debugger.h"
#include <windows.h>

// Gets the log target instance.
LEAN_ALWAYS_LINK lean::logging::log_debugger& lean::logging::log_debugger::get()
{
	static log_debugger debugger;
	return debugger;
}

// Prints the given message to the debug output window.
LEAN_ALWAYS_LINK void lean::logging::print_debugger(const char_ntri &message)
{
	::OutputDebugStringA(message.c_str());
}
