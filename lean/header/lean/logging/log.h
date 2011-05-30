/*****************************************************/
/* lean Logging                 (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_LOG
#define LEAN_LOGGING_LOG

#include "../lean.h"
#include "../tags/noncopyable.h"
#include "../strings/types.h"
#include "../strings/conversions.h"
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

/// Logs an error.
LEAN_MAYBE_EXPORT void log_error(const char *source);
/// Logs an error.
LEAN_MAYBE_EXPORT void log_error(const char *source, const char *reason);
/// Logs an error.
LEAN_MAYBE_EXPORT void log_error(const char *source, const char *reason, const char *context);

/// Logs an error.
template <class String1>
inline void log_error(const String1 &source)
{
	log_error(utf_to_utf8(source).c_str());
}
/// Logs an error.
template <class String1, class String2>
inline void log_error(const String1 &source, const String2 &reason)
{
	log_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str());
}
/// Logs an error.
template <class String1, class String2, class String3>
inline void log_error(const String1 &source, const String2 &reason, const String3 &context)
{
	log_error(utf_to_utf8(source).c_str(), utf_to_utf8(reason).c_str(), utf_to_utf8(context).c_str());
}

} // namespace

using logging::log;
using logging::log_stream;

using logging::error_log;
using logging::info_log;

using logging::log_error;

} // namespace

// Drop-in replacements for missing utf8 streaming operators
template <class Traits, class StringTraits, class StringAlloc>
inline std::basic_ostream<lean::utf8_t, Traits>& operator <<(
	std::basic_ostream<lean::utf8_t, Traits>& stream,
	const std::basic_string<lean::utf16_t, StringTraits, StringAlloc> &string)
{
	return (stream << lean::utf_to_utf8(string));
}
template <class Traits, class StringTraits, class StringAlloc>
inline std::basic_ostream<lean::utf8_t, Traits>& operator <<(
	std::basic_ostream<lean::utf8_t, Traits>& stream,
	const std::basic_string<lean::utf32_t, StringTraits, StringAlloc> &string)
{
	return (stream << lean::utf_to_utf8(string));
}
template <class Traits>
inline std::basic_ostream<lean::utf8_t, Traits>& operator <<(std::basic_ostream<lean::utf8_t, Traits>& stream, const lean::utf16_t *str)
{
	return (stream << lean::utf_to_utf8(str));
}
template <class Traits>
inline std::basic_ostream<lean::utf8_t, Traits>& operator <<(std::basic_ostream<lean::utf8_t, Traits>& stream, const lean::utf32_t *str)
{
	return (stream << lean::utf_to_utf8(str));
}

// Drop-in replacements for missing utf16 streaming operators
template <class Traits, class StringTraits, class StringAlloc>
inline std::basic_ostream<lean::utf16_t, Traits>& operator <<(
	std::basic_ostream<lean::utf16_t, Traits>& stream,
	const std::basic_string<lean::utf32_t, StringTraits, StringAlloc> &string)
{
	return (stream << lean::utf_to_utf16(string));
}
template <class Traits>
inline std::basic_ostream<lean::utf16_t, Traits>& operator <<(std::basic_ostream<lean::utf16_t, Traits>& stream, const lean::utf32_t *str)
{
	return (stream << lean::utf_to_utf16(str));
}

// Drop-in replacements for missing utf32 streaming operators
template <class Traits, class StringTraits, class StringAlloc>
inline std::basic_ostream<lean::utf32_t, Traits>& operator <<(
	std::basic_ostream<lean::utf32_t, Traits>& stream,
	const std::basic_string<lean::utf16_t, StringTraits, StringAlloc> &string)
{
	return (stream << lean::utf_to_utf32(string));
}
template <class Traits>
inline std::basic_ostream<lean::utf32_t, Traits>& operator <<(std::basic_ostream<lean::utf32_t, Traits>& stream, const lean::utf16_t *str)
{
	return (stream << lean::utf_to_utf32(str));
}

/// @addtogroup LoggingMacros Logging macros
/// @see lean::logging
/// @{

/// Logs the given message, prepending the caller's file and line.
#define LEAN_LOG(msg) ::lean::log_stream(::lean::info_log()) << __FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) "): " << msg << ::std::endl

/// Logs the given error message, prepending the caller's file and line.
#define LEAN_LOG_ERROR(msg) ::lean::log_stream(::lean::error_log()) << __FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) "): " << msg << ::std::endl
/// Logs an error message, prepending the caller's file and line.
#define LEAN_LOG_ERROR_NIL() ::lean::logging::log_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")")
/// Logs the given error message, prepending the caller's file and line.
#define LEAN_LOG_ERROR_MSG(msg) ::lean::logging::log_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg)
/// Logs the given error message and context, prepending the caller's file and line.
#define LEAN_LOG_ERROR_CTX(msg, ctx) ::lean::logging::log_error(__FILE__ " (" LEAN_QUOTE_VALUE(__LINE__) ")", msg, ctx)

/// @}

#ifdef LEAN_INCLUDE_LINKED
#include "source/log.cpp"
#endif

#endif