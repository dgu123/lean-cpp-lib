/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_LOG_DEBUGGER
#define LEAN_LOGGING_LOG_DEBUGGER

#include "../lean.h"
#include "../strings/types.h"
#include "log_target.h"

namespace lean
{
namespace logging
{

/// Prints the given message to the debug output window.
LEAN_MAYBE_EXPORT void print_debugger(const char_ntri &message);

/// Log debugger class that prints any given input to the debug output window.
class log_debugger : public log_target
{
private:
	log_debugger() { }
	~log_debugger() { }

public:
	/// Gets the log target instance.
	LEAN_MAYBE_EXPORT static log_debugger& get();

	/// Prints the given message to the debug output window.
	void print(const char_ntri &message) { print_debugger(message); }
};

} // namespace

using logging::log_debugger;

} // namespace

#ifndef LEAN_LINKING
#include "source/log_debugger.cpp"
#endif

#endif