/*****************************************************/
/* lean I/O                     (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_IO_RAW_FILE
#define LEAN_LOGGING_IO_RAW_FILE

#include "../lean.h"
#include "../strings/types.h"
#include "file.h"

namespace lean
{
namespace io
{

/// File class that allows for raw read/write operations on a given file.
class raw_file : public file
{
public:
	/// Opens the given file according to the given flags. Throws a runtime_exception on error.
	LEAN_MAYBE_EXPORT explicit raw_file(const utf8_ntri &name,
		uint4 access = read | write, open_mode mode = open,
		uint4 hints = none, uint4 share = read);
	/// Closes this file.
	LEAN_MAYBE_EXPORT ~raw_file();

	/// Prints the given range of characters to the file. This method is thread-safe.
	LEAN_MAYBE_EXPORT void print(const char_ntri &message);
};

} // namespace

using io::raw_file;

} // namespace

#ifdef LEAN_INLINING
#include "source/raw_file.cpp"
#endif

#endif