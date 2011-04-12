#include "../log.h"
#include <sstream>

// Destructor.
LEAN_ALWAYS_LINK lean::logging::log::log()
{
}

// Constructor.
LEAN_ALWAYS_LINK lean::logging::log::~log()
{
	for (stream_vector::iterator itStream = m_streams.begin();
		itStream != m_streams.end(); ++itStream)
		delete (*itStream);
}

// Prints the given message.
LEAN_ALWAYS_LINK void lean::logging::log::print(const char_ntri &message)
{
	// Print synchronized
	scoped_sl_lock lock(m_printLock);
	// TODO: pass on...
}

// Acquires a stream to write to.
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

// Prints the contents of the given stream and releases the stream for further re-use.
LEAN_ALWAYS_LINK void lean::logging::log::flushAndReleaseStream(output_stream &stream)
{
	string_stream& stringStream = static_cast<string_stream&>(stream);

	// Retrieve message & try to reset stream
	std::string message = stringStream.str();
	stringStream.str("");
	stream.clear();

	// Release stream if reset successfully
	{
		scoped_sl_lock lock(m_streamLock);
		m_freeStreams.push_back(&stream);
	}

	// Print stream message
	print(message);
}
