/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_LOG_TARGET
#define LEAN_LOGGING_LOG_TARGET

#include "../lean.h"
#include "../strings/types.h"
#include "../tags/noncopyable.h"

namespace lean
{
namespace logging
{

/// Log target interface.
class log_target : public noncopyable
{
public:
	/// Prints the given message.
	virtual void print(const char_ntri &message) = 0;
};

} // namespace

using logging::log_target;

} // namespace

#endif