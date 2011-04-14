/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_LOG
#define LEAN_LOGGING_LOG

#include "../lean.h"
#include "../tags/noncopyable.h"
#include "../strings/types.h"
#include "../concurrent/spin_lock.h"
#include "../concurrent/shareable_spin_lock.h"
#include "log_target.h"
#include <vector>
#include <iosfwd>
#include <ostream>

namespace lean
{
namespace logging
{

class log_stream;

/// Log class.
class log : public noncopyable
{
friend class log_stream;

private:
	typedef std::basic_ostream<char> output_stream;
	typedef std::basic_ostringstream<char> string_stream;
	typedef std::vector<string_stream*> stream_vector;
	stream_vector m_streams;

	typedef std::vector<output_stream*> free_stream_vector;
	free_stream_vector m_freeStreams;
	spin_lock<> m_streamLock;

	typedef std::vector<log_target*> target_vector;
	target_vector m_targets;
	shareable_spin_lock<> m_targetLock;

	/// Acquires a stream to write to. This method is thread-safe.
	LEAN_MAYBE_EXPORT output_stream& acquireStream();
	/// Prints the contents of the given stream and releases the stream for further re-use. This method is thread-safe.
	LEAN_MAYBE_EXPORT void flushAndReleaseStream(output_stream &stream);

public:
	/// Constructor.
	LEAN_MAYBE_EXPORT log(log_target *initialTarget = nullptr);
	/// Destructor.
	LEAN_MAYBE_EXPORT ~log();

	/// Adds the given target to this log. This method is thread-safe.
	LEAN_MAYBE_EXPORT void add_target(log_target *target);
	/// Removes the given target from this log. This method is thread-safe.
	LEAN_MAYBE_EXPORT void remove_target(log_target *target);

	/// Prints the given message. This method is thread-safe.
	LEAN_MAYBE_EXPORT void print(const char_ntri &message);
};

/// Log stream class.
class log_stream : public noncopyable
{
private:
	log &m_log;
	log::output_stream &m_stream;

public:
	/// Output stream type.
	typedef log::output_stream output_stream;
	/// Basic output stream type.
	typedef std::basic_ostream<output_stream::char_type, output_stream::traits_type> basic_output_stream;
	/// Basic output ios type.
	typedef std::basic_ios<output_stream::char_type, output_stream::traits_type> basic_output_ios;

	/// Constructs a new TEMPORARY log stream from the log.
	explicit log_stream(log &log)
		: m_log(log),
		m_stream(log.acquireStream()) { }
	/// Prints the contents of this log stream and releases it for further re-use.
	~log_stream()
	{
		m_log.flushAndReleaseStream(m_stream);
	}

	/// Outputs the given value to this log stream.
	template <class Value>
	log_stream& operator <<(const Value &value)
	{
		m_stream << value;
		return *this;
	}
	// Passes the given manipulator to this log stream.
	log_stream& operator <<(std::ios_base& (*manip)(::std::ios_base&))
	{
		m_stream << manip;
		return *this;
	}
	// Passes the given manipulator to this log stream.
	log_stream& operator<<(basic_output_stream& (*manip)(basic_output_stream&))
	{
		m_stream << manip;
		return *this;
	}
	// Passes the given manipulator to this log stream.
	log_stream& operator<<(basic_output_ios& (*manip)(basic_output_ios&))
	{
		m_stream << manip;
		return *this;
	}

	/// Gets the underlying output stream.
	output_stream& std()
	{
		return m_stream;
	}
};

/// Gets the error log.
LEAN_MAYBE_EXPORT log& error_log();
/// Gets the info log.
LEAN_MAYBE_EXPORT log& info_log();

} // namespace

using logging::log;
using logging::log_stream;

using logging::error_log;
using logging::info_log;

} // namespace

/// @addtogroup LoggingMacros Logging macros
/// @see lean::logging
/// @{

/// Logs the given message, prepending the caller's file and line.
#define LEAN_LOG(msg) ::lean::log_stream(::lean::info_log()) << __FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) "): " << msg << ::std::endl

/// Logs the given error message, prepending the caller's file and line.
#define LEAN_LOG_ERROR(msg) ::lean::log_stream(::lean::error_log()) << __FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) "): " << msg << ::std::endl

/// @}

#ifndef LEAN_LINKING
#include "source/log.cpp"
#endif

#endif