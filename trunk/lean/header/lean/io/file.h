/*****************************************************/
/* lean I/O                     (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_LOGGING_IO_FILE
#define LEAN_LOGGING_IO_FILE

#include "../lean.h"
#include "../strings/types.h"
#include "win_types.h"

namespace lean
{
namespace io
{

/// Log file class that prints any given input to a given file.
class file
{
private:
	windows_file_handle m_handle;

public:
	/// Access modes.
	enum access_modes
	{
		read = 0x1,		///< Read mode.
		write = 0x2		///< Write mode.
	};
	/// Open modes.
	enum open_mode
	{
		create,			///< Create file, if not existent (write access only).
		append,			///< Open file, if existent (enforced on read-only access).
		open,			///< Open file, if existent, otherwise create new file (write access only).
		overwrite		///< Overwrite file, if existent, otherwise create new file (write access only).
	};
	/// Optimization hints.
	enum hints
	{
		none = 0x0,			///< Make no assumptions on future file access.
		sequential = 0x1,	///< The file is about to be read strictly front-to-back.
		random = 0x2		///< The file is about to be accessed randomly.
	};

	/// Opens the given file according to the given flags. Throws a runtime_exception on error.
	LEAN_MAYBE_EXPORT explicit file(const utf8_ntri &name,
		uint4 access = read | write, uint4 share = read,
		open_mode mode = open, uint4 hints = none);
	/// Closes this file.
	LEAN_MAYBE_EXPORT ~file();

	/// Prints the given range of characters to the file. This method is thread-safe.
	LEAN_MAYBE_EXPORT void print(const char_ntri &message);

	/// Gets the last modification time in microseconds since 1/1/1901.
	LEAN_MAYBE_EXPORT uint8 revision() const;
	/// Gets the size of this file, in bytes.
	LEAN_MAYBE_EXPORT uint8 size() const;

	/// Gets the file handle of this file.
	LEAN_INLINE windows_file_handle handle() const { return m_handle; };
};

} // namespace

using io::file;

} // namespace

#ifdef LEAN_INLINING
#include "source/file.cpp"
#endif

#endif