#ifdef LEAN_BUILD_LIB
#include "../../depconfig.h"
#endif

#include "../log.h"
#include <sstream>
#include <algorithm>
#include "../log_debugger.h"

// Destructor.
LEAN_ALWAYS_LINK lean::logging::log::log(log_target *initialTarget)
{
	if (initialTarget)
		m_targets.push_back(initialTarget);
}

// Constructor.
LEAN_ALWAYS_LINK lean::logging::log::~log()
{
	for (stream_vector::iterator itStream = m_streams.begin();
		itStream != m_streams.end(); ++itStream)
		delete (*itStream);
}

// Adds the given target to this log. This method is thread-safe.
LEAN_ALWAYS_LINK void lean::logging::log::add_target(log_target *target)
{
	if (target != nullptr)
	{
		scoped_ssl_lock lock(m_targetLock);
		m_targets.push_back(target);
	}
}

// Removes the given target from this log. This method is thread-safe.
LEAN_ALWAYS_LINK void lean::logging::log::remove_target(log_target *target)
{
	if (target != nullptr)
	{
		scoped_ssl_lock lock(m_targetLock);
		m_targets.erase(
			std::remove(m_targets.begin(), m_targets.end(), target),
			m_targets.end() );
	}
}

// Prints the given message. This method is thread-safe.
LEAN_ALWAYS_LINK void lean::logging::log::print(const char_ntri &message)
{
	scoped_ssl_lock_shared lock(m_targetLock);

	for (target_vector::const_iterator itTarget = m_targets.begin();
		itTarget != m_targets.end(); ++itTarget)
		(*itTarget)->print(message);
}

// Acquires a stream to write to. This method is thread-safe.
LEAN_ALWAYS_LINK lean::logging::log::output_stream& lean::logging::log::acquireStream()
{
	scoped_sl_lock lock(m_streamLock);

	if (m_freeStreams.empty())
	{
		std::auto_ptr<string_stream> stream(new string_stream());

		// Make sure m_freeStreams.capacity() >= m_streams.size()
		// + Never add invalid element to m_streams
		m_freeStreams.reserve(m_streams.size() + 1);
		m_streams.push_back(stream.get());

		return *stream.release();
	}
	else
	{
		output_stream *stream = m_freeStreams.back();
		m_freeStreams.pop_back();
		return *stream;
	}
}

// Prints the contents of the given stream and releases the stream for further re-use. This method is thread-safe.
LEAN_ALWAYS_LINK void lean::logging::log::flushAndReleaseStream(output_stream &stream)
{
	string_stream& stringStream = static_cast<string_stream&>(stream);

	// Retrieve message & try to reset stream
	std::string message = stringStream.str();
	stringStream.str("");
	stream.clear();

	// Release stream (if reset successfully)
	{
		scoped_sl_lock lock(m_streamLock);
		m_freeStreams.push_back(&stream);
	}

	// Print stream message
	print(message);
}

// Gets the error log.
LEAN_ALWAYS_LINK lean::logging::log& lean::logging::error_log()
{
	static log errorLog(&log_debugger::get());
	return errorLog;
}

// Gets the info log.
LEAN_ALWAYS_LINK lean::logging::log& lean::logging::info_log()
{
	static log infoLog(&log_debugger::get());
	return infoLog;
}

namespace lean
{
namespace logging
{
namespace impl
{

	/// Makes the given source string valid in any case.
	inline const char* make_source_valid(const char *source)
	{
		return (source) ? source : "Unknown source";
	}

} // namespace
} // namespace
} // namespace

// Logs an error.
LEAN_ALWAYS_LINK void lean::logging::log_error(const char *source)
{
	log_stream(error_log()) << impl::make_source_valid(source) << ": An error occurred." << std::endl;
}

// Logs an error.
LEAN_ALWAYS_LINK void lean::logging::log_error(const char *source, const char *reason)
{
	if (!reason)
		return log_error(source);

	log_stream(error_log()) << impl::make_source_valid(source) << ": An error occurred: " << reason << std::endl;
}

// Logs an error.
LEAN_ALWAYS_LINK void lean::logging::log_error(const char *source, const char *reason, const char *context)
{
	if (!context)
		return log_error(source, reason);
	if (!reason)
		return log_error(source, context);

	log_stream(error_log()) << impl::make_source_valid(source) << ": An error occurred: " << reason << " (" << context << ")" << std::endl;
}

// Logs an error.
LEAN_ALWAYS_LINK void lean::logging::log_error_ex(const char *source, const char *reason, const char *origin)
{
	if (!origin)
		return log_error(source, reason);
	if (!reason)
		return log_error(source, origin);

	log_stream(error_log()) << impl::make_source_valid(source) << ": An error occurred: " << reason << " << " << origin << std::endl;
}

// Logs an error.
LEAN_ALWAYS_LINK void lean::logging::log_error_ex(const char *source, const char *reason, const char *origin, const char *context)
{
	if (!context)
		return log_error_ex(source, reason, origin);
	if (!origin)
		return log_error(source, reason, context);
	if (!reason)
		return log_error(source, origin, context);

	log_stream(error_log()) << impl::make_source_valid(source) << ": An error occurred: " << reason
		<< " << " << origin << " (" << context << ")" << std::endl;
}
