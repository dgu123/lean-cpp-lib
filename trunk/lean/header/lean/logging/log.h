/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_LOG
#define LEAN_LOGGING_LOG

#include <iostream>
#include "../macros.h"
#include "../tags/noncopyable.h"
#include "../concurrent/spin_lock.h"
#include "../strings/types.h"
#include <iosfwd>
#include <vector>

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

	spin_lock<> m_printLock;

	/// Acquires a stream to write to.
	output_stream& acquireStream();
	/// Prints the contents of the given stream and releases the stream for further re-use.
	void flushAndReleaseStream(output_stream &stream);

public:
	/// Constructor.
	log();
	/// Destructor.
	~log();

	/// Prints the given message.
	void print(const char_ntri &message);
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

	/// Constructs a new TEMPORARY log stream from the log.
	log_stream(log &log)
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

	/// Gets the underlying output stream.
	output_stream& std()
	{
		return m_stream;
	}
};

} // namespace

using logging::log;
using logging::log_stream;

} // namespace

/// @addtogroup LoggingMacros Logging macros
/// @see lean::logging
/// @{

/// Logs the given message, prepending the caller's file and line.
#define LEAN_LOG(msg) ::std::clog << __FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) "): " << msg << ::std::endl

/// Logs the given error message, prepending the caller's file and line.
#define LEAN_LOG_ERROR(msg) ::std::cerr << __FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) "): " << msg << ::std::endl

/// @}

#ifndef LEAN_LINKING
#include "source/log.cpp"
#endif

#endif