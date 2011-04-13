#include "../exceptions.h"
#include "../log.h"
#include <stdexcept>

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

// Throws a runtime_error exception.
LEAN_ALWAYS_LINK void lean::logging::throw_error(const char *source)
{
	// Store valid source, re-used in throw clause!
	source = impl::make_source_valid(source);
	log_stream(error_log()) << source << ": An error occured." << std::endl);
	throw std::runtime_error(source);
}

// Throws a runtime_error exception.
LEAN_ALWAYS_LINK void lean::logging::throw_error(const char *source, const char *reason)
{
	if (!reason)
		return throw_error(source);

	log_stream(error_log()) << impl::make_source_valid(source) << ": An error occured: " << reason << std::endl;
	throw std::runtime_error(reason);
}

// Throws an invalid_argument exception.
LEAN_ALWAYS_LINK void lean::logging::throw_invalid(const char *source)
{
	// Store valid source, re-used in throw clause!
	source = impl::make_source_valid(source);
	log_stream(error_log()) << source << ": Invalid argument." << std::endl;
	throw std::invalid_argument(source);
}

// Throws an invalid_argument exception.
LEAN_ALWAYS_LINK void lean::logging::throw_invalid(const char *source, const char *reason)
{
	if (!reason)
		return throw_invalid(source);

	log_stream(error_log()) << impl::make_source_valid(source) << ": Invalid argument: " << reason << std::endl;
	throw std::invalid_argument(reason);
}

// Throws a bad_alloc exception.
LEAN_ALWAYS_LINK void lean::logging::throw_bad_alloc(const char *source)
{
	// TODO: Log somewhere without allocating further memory? Depends on the amount of memory requested...
	log_stream(error_log()) << impl::make_source_valid(source) << ": Out of memory." << std::endl;
	throw std::bad_alloc();
}

// Throws a bad_alloc exception.
LEAN_ALWAYS_LINK void lean::logging::throw_bad_alloc(const char *source, size_t size)
{
	// TODO: Log somewhere without allocating further memory? Depends on the amount of memory requested...
	log_stream(error_log()) << impl::make_source_valid(source) << ": Out of memory while allocating " << size << " bytes." << std::endl;
	throw std::bad_alloc();
}
