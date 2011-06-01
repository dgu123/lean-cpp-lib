/*****************************************************/
/* lean I/O                     (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_IO_FILESYSTEM
#define LEAN_IO_FILESYSTEM

#include "../lean.h"
#include "../strings/types.h"
#include "../strings/conversions.h"
#include <algorithm>

#ifndef LEAN_FILESYSTEM_PATH_LENGTH_HINT
	/// Estimated length of filesystem paths (in characters).
	/// @ingroup AssortedSwitches
	#define LEAN_FILESYSTEM_PATH_LENGTH_HINT 256
#endif

namespace lean
{
namespace io
{

/// Checks whether the given file exists.
LEAN_MAYBE_EXPORT bool file_exists(const utf16_nti& file);
/// Checks whether the given file exists.
LEAN_INLINE bool file_exists(const utf8_ntri& file)
{
	return file_exists(utf_to_utf16(file));
}

/// Gets the size of the given file, in bytes.
LEAN_MAYBE_EXPORT uint8 file_size(const utf16_nti& file);
/// Gets the size of the given file, in bytes.
LEAN_INLINE uint8 file_size(const utf8_ntri& file)
{
	return file_size(utf_to_utf16(file));
}

/// Gets the last modification time in microseconds since 1/1/1970. Returns 0 on error.
LEAN_MAYBE_EXPORT uint8 file_revision(const utf16_nti& file);
/// Gets the last modification time in microseconds since 1/1/1970. Returns 0 on error.
LEAN_INLINE uint8 file_revision(const utf8_ntri& file)
{
	return file_revision(utf_to_utf16(file));
}

/// Gets the current directory. Will return the buffer size required to store the
/// current directory, if the given buffer is too small, the number of actual
/// characters written, otherwise (excluding the terminating null appended).
LEAN_MAYBE_EXPORT size_t current_directory(utf16_t *buffer, size_t bufferSize);

/// Gets the current directory.
template <class String>
String current_directory();

#ifndef DOXYGEN_SKIP_THIS

/// Gets the current directory.
template <>
LEAN_INLINE utf16_string current_directory()
{
	utf16_string result;
	
	result.resize(LEAN_FILESYSTEM_PATH_LENGTH_HINT);
	size_t actualLength = current_directory(&result[0], LEAN_FILESYSTEM_PATH_LENGTH_HINT);
	
	if (actualLength > LEAN_FILESYSTEM_PATH_LENGTH_HINT)
	{
		result.resize(actualLength);
		actualLength = current_directory(&result[0], actualLength);
	}

	result.erase(actualLength);
	return result;
}
/// Gets the current directory.
template <>
LEAN_INLINE utf8_string current_directory()
{
	return strings::utf16_to_utf8<utf8_string>(current_directory<utf16_string>());
}

#endif

/// Gets the current directory.
LEAN_INLINE utf8_string current_directory()
{
	return strings::utf16_to_utf8<utf8_string>(current_directory<utf16_string>());
}

/// Gets the initial directory.
LEAN_INLINE const utf8_string& initial_directory()
{
	static const utf8_string initialDir = current_directory<utf8_string>();
	return initialDir;
}

/// Special file system characters.
template <class Char>
struct filesystem_chars
{
	/// Path separator character.
	static const Char path_separator = '/';
	/// Alternative path separator character.
	static const Char alt_path_separator = '\\';
	/// Extension separator character.
	static const Char extension_separator = '.';
	/// Redirection character.
	static const Char redirection = '.';
};

/// Checks if the given character is a path separator.
template <class Char>
LEAN_INLINE bool is_path_separator(Char chr)
{
	return (chr == filesystem_chars<Char>::path_separator) ||
		(chr == filesystem_chars<Char>::alt_path_separator);
}

/// Checks if the given character is an extension separator.
template <class Char>
LEAN_INLINE bool is_extension_separator(Char chr)
{
	return (chr == filesystem_chars<Char>::extension_separator);
}

/// Checks if the given character is a redirection character.
template <class Char>
LEAN_INLINE bool is_redirection(Char chr)
{
	return (chr == filesystem_chars<Char>::redirection);
}

/// Assigns a path separator to the given character.
template <class Char>
LEAN_INLINE Char& assign_path_separator(Char &chr)
{
	return (chr = filesystem_chars<Char>::path_separator);
}

/// Assigns an extension separator to the given character.
template <class Char>
LEAN_INLINE Char& assign_extension_separator(Char &chr)
{
	return (chr = filesystem_chars<Char>::extension_separator);
}

/// Assigns a redirection character to the given character.
template <class Char>
LEAN_INLINE Char& assign_redirection(Char &chr)
{
	return (chr = filesystem_chars<Char>::redirection);
}

/// Gets the relative path euqivalent to the given absolute path when starting at the given base.
/// Does not resolve redirections, use CanonicalPath prior to calling this function to resolve these.
template <class String, class Range1, class Range2>
inline String relative_path(const Range1 &base, const Range2 &path)
{
	typename Range1::const_iterator baseMarker = base.begin(),
		baseCursor = base.begin();
	typename Range2::const_iterator pathMarker = path.begin(),
		pathCursor = path.begin();

	bool endOfBase = (baseCursor == base.end()),
		endOfPath = (pathCursor == path.end());
	bool divergent = false;

	// Skip identical parts
	while (!divergent && !endOfBase && !endOfPath)
	{
		// Move forward to the end of the next directory name
		while (!endOfBase && !is_path_separator(*baseCursor))
			endOfBase = (++baseCursor == base.end());
		while (!endOfPath && !is_path_separator(*pathCursor))
			endOfPath = (++pathCursor == path.end());

		// Compare directory names
		divergent = (baseCursor - baseMarker != pathCursor - pathMarker)
			|| !std::equal(baseMarker, baseCursor, pathMarker);

		// Skip path separator
		if (!endOfBase)
			endOfBase = (++baseCursor == base.end());
		if (!endOfPath)
			endOfPath = (++pathCursor == path.end());

		if (!divergent)
		{
			// Mark beginning of next directory name
			baseMarker = baseCursor;
			pathMarker = pathCursor;
		}
	}

	// ASSERT: paths equal up to their corresponding markers

	String result;
	result.reserve( static_cast<size_t>(base.end() - baseMarker)
		+ static_cast<size_t>(path.end() - pathMarker) );

	// Trailing slashes already skipped, don't lose corresponding directory
	int mismatchCount = (baseMarker != base.end()) ? 1 : 0;
	
	// Count mismatching base sub-directories
	for (baseCursor = baseMarker; baseCursor != base.end(); ++baseCursor)
		if (is_path_separator(*baseCursor))
			++mismatchCount;

	if (mismatchCount != 0)
	{
		result.resize(3U * mismatchCount - 1U);
		typename String::iterator insertCursor = result.begin();

		// Add redirection for each mismatching sub-directory
		for (int i = 0; i < mismatchCount; ++i)
		{
			if (i != 0)
				assign_path_separator(*(insertCursor++));
			assign_redirection(*(insertCursor++));
			assign_redirection(*(insertCursor++));
		}
	}

	// Append remaining sub-directories
	if (pathMarker != path.end())
	{
		bool needSeparation = !result.empty();
		
		size_t insertPos = result.size();
		result.resize(insertPos + static_cast<size_t>(path.end() - pathMarker) + needSeparation);
		typename String::iterator insertCursor = result.begin() + insertPos;
		
		if (needSeparation)
			assign_path_separator(*(insertCursor++));

		insertCursor = std::copy(pathMarker, path.end(), insertCursor);
	}

	return result;
}
/// Gets the relative path euqivalent to the given absolute path when starting at the given base.
/// Does not resolve redirections, use CanonicalPath prior to calling this function to resolve these.
template <class Char, class Range>
LEAN_INLINE std::basic_string<Char> relative_path(const Char *base, const Range &path)
{
	return relative_path< std::basic_string<Char> >(make_char_range(base), path);
}
/// Gets the relative path euqivalent to the given absolute path when starting at the given base.
/// Does not resolve redirections, use CanonicalPath prior to calling this function to resolve these.
template <class Char, class Range>
LEAN_INLINE std::basic_string<Char> relative_path(const Range &base, const Char *path)
{
	return relative_path< std::basic_string<Char> >(base, make_char_range(path));
}
/// Gets the relative path euqivalent to the given absolute path when starting at the given base.
/// Does not resolve redirections, use CanonicalPath prior to calling this function to resolve these.
template <class Char>
LEAN_INLINE std::basic_string<Char> relative_path(const Char *base, const Char *path)
{
	return relative_path< std::basic_string<Char> >(make_char_range(base), make_char_range(path));
}

/// Gets the absolute path euqivalent to the given relative path.
template <class String, class Range1, class Range2>
LEAN_INLINE String absolute_path(const Range1 &base, const Range2 &path)
{
	return canonical_path<String>( append_path<String>(base, path) );
}
/// Gets the absolute path euqivalent to the given relative path.
template <class Char, class Range>
LEAN_INLINE std::basic_string<Char> absolute_path(const Char *base, const Range &path)
{
	return canonical_path< std::basic_string<Char> >( append_path(base, path) );
}
/// Gets the absolute path euqivalent to the given relative path.
template <class Char, class Range>
LEAN_INLINE std::basic_string<Char> absolute_path(const Range &base, const Char *path)
{
	return canonical_path< std::basic_string<Char> >( append_path(base, path) );
}
/// Gets the absolute path euqivalent to the given relative path.
template <class Char>
LEAN_INLINE std::basic_string<Char> absolute_path(const Char *base, const Char *path)
{
	return canonical_path< std::basic_string<Char> >( append_path(base, path) );
}

/// Gets a canonical relative path euqivalent to the given relative path.
template <class String, class Range>
inline String canonical_path(const Range &path)
{
	String result;
	result.resize(path.size());

	typename Range::const_iterator srcCursor = path.end(),
		srcMarker = path.end();
	typename String::iterator destCursor = result.end();

	int skipCounter = 0;
	int redirCounter = 0;

	bool endOfPath = (srcCursor == path.begin());

	while (!endOfPath)
	{
		// Pre-decrement, end never dereferenceable
		endOfPath = (--srcCursor == path.begin());

		bool isPathSeparator = is_path_separator(*srcCursor);

		if (is_redirection(*srcCursor))
		{
			// Count redirection characters, as long as no other characters around
			if (redirCounter != -1)
				++redirCounter;
		}
		else if (!isPathSeparator)
			// Not a redirection, treat as actual directory name
			redirCounter = -1;

		// End of current directory
		if (isPathSeparator || endOfPath)
		{
			typename Range::const_iterator srcInsertionCursor = srcCursor;

			// Don't copy leading path separators
			if (!endOfPath) // == isPathSeparator, except that leading unix-style root slashes are kept
				++srcInsertionCursor;

			// Ignore empty directory names & local directory
			if (redirCounter != 0 && redirCounter != 1)
			{
				// Up one directory
				if(redirCounter == 2)
					// Skip this & also skip next actual directory name
					++skipCounter;
				// Actual directory name
				else if (skipCounter == 0)
					// Prepend directory
					destCursor = std::copy_backward(srcInsertionCursor, srcMarker, destCursor);
				else
					// Skip directory
					--skipCounter;
			}

			// Remember insertion cursor, any leading path separator copied as trailing path separator next time
			srcMarker = srcInsertionCursor;

			redirCounter = 0;
		}
		
	}

	// Prepend unresolved redirections
	while (skipCounter != 0)
	{
		assign_path_separator(*(--destCursor));
		assign_redirection(*(--destCursor));
		assign_redirection(*(--destCursor));
		--skipCounter;
	}

	// ASSERT: size of canonical path <= size of source path

	// Move canonical path to front
	if (destCursor != result.begin())
		result.assign(destCursor, result.end());

	return result;
}
/// Gets a canonical relative path euqivalent to the given relative path.
template <class String, class Char>
LEAN_INLINE String canonical_path(const Char *path)
{
	return canonical_path<String>(make_char_range(path));
}
/// Gets a canonical relative path euqivalent to the given relative path.
template <class Char>
LEAN_INLINE std::basic_string<Char> canonical_path(const Char *path)
{
	return canonical_path< std::basic_string<Char> >(path);
}

/// Appends the given file or directory to the given path.
template <class String, class Range1, class Range2>
inline String append_path(const Range1 &path, const Range2 &file)
{
	String result;

	result.resize(path.size() + 1 + file.size());
	typename String::iterator insertCursor = result.begin();

	insertCursor = std::copy(path.begin(), path.end(), insertCursor);

	if (!path.empty() && !file.empty() &&
		!is_path_separator(*(path.end() - 1)) && !is_path_separator(*file.begin()))
		assign_path_separator(*(insertCursor++));

	insertCursor = std::copy(file.begin(), file.end(), insertCursor);

	result.erase(insertCursor, result.end());
	return result;
}
/// Appends the given file or directory to the given path.
template <class String, class Char, class Range>
LEAN_INLINE String append_path(const Char *path, const Range &file)
{
	return append_path<String>(make_char_range(path), file);
}
/// Appends the given file or directory to the given path.
template <class String, class Char, class Range>
LEAN_INLINE String append_path(const Range &path, const Char *file)
{
	return append_path<String>(path, make_char_range(file));
}
/// Appends the given file or directory to the given path.
template <class String, class Char>
LEAN_INLINE String append_path(const Char *path, const Char *file)
{
	return append_path<String>(make_char_range(path), make_char_range(file));
}
/// Appends the given file or directory to the given path.
template <class Char, class Range>
LEAN_INLINE std::basic_string<Char> append_path(const Char *path, const Range &file)
{
	return append_path< std::basic_string<Char> >(path, file);
}
/// Appends the given file or directory to the given path.
template <class Char, class Range>
LEAN_INLINE std::basic_string<Char> append_path(const Range &path, const Char *file)
{
	return append_path< std::basic_string<Char> >(path, file);
}
/// Appends the given file or directory to the given path.
template <class Char>
LEAN_INLINE std::basic_string<Char> append_path(const Char *path, const Char *file)
{
	return append_path< std::basic_string<Char> >(path, file);
}

/// Gets the end of the parent directory, e.g. '..' from '../test.txt'.
template <class Iterator>
inline Iterator get_directory(Iterator fileBegin, Iterator fileEnd)
{
	Iterator dirEnd = fileEnd;

	while (dirEnd != fileBegin)
	{
		// Pre-decrement, end never dereferenceable
		--dirEnd;

		// Stop on first path separator, not included
		if (is_path_separator(*dirEnd))
			break;
	}

	return dirEnd;
}
/// Gets the parent directory, e.g. '..' from '../test.txt'.
template <class String, class Range>
LEAN_INLINE String get_directory(const Range &file)
{
	return String(
		file.begin(),
		get_directory(file.begin(), file.end()) );
}
/// Gets the parent directory, e.g. '..' from '../test.txt'.
template <class String, class Char>
LEAN_INLINE std::basic_string<Char> get_directory(const Char *file)
{
	return get_directory<String>(
		make_range(
			file,
			file + std::char_traits<Char>::length(file) ) );
}
/// Gets the parent directory, e.g. '..' from '../test.txt'.
template <class Char>
LEAN_INLINE std::basic_string<Char> get_directory(const Char *file)
{
	return get_directory< std::basic_string<Char> >(file);
}

/// Gets the beginning of the file name, e.g. 'test.txt' from '../test.txt'.
template <class Iterator>
inline Iterator get_filename(Iterator fileBegin, Iterator fileEnd)
{
	Iterator nameBegin = fileEnd;

	while (nameBegin != fileBegin)
	{
		// Pre-decrement, end never dereferenceable
		--nameBegin;

		// Stop on first path separator
		if (is_path_separator(*nameBegin))
		{
			// Separator not included
			++nameBegin;
			break;
		}
	}

	return nameBegin;
}
/// Gets the file name, e.g. 'test.txt' from '../test.txt'.
template <class String, class Range>
LEAN_INLINE String get_filename(const Range &file)
{
	return String(
		get_filename(file.begin(), file.end()),
		file.end() );
}
/// Gets the file name, e.g. 'test.txt' from '../test.txt'.
template <class Char>
LEAN_INLINE const Char* get_filename(const Char *file)
{
	return get_filename(
		file,
		file + std::char_traits<Char>::length(file) );
}

/// Gets the file stem, e.g. 'test' from '../test.txt'.
template <class Iterator>
inline range<Iterator> get_stem(Iterator fileBegin, Iterator fileEnd)
{
	Iterator nameBegin = fileEnd, stemEnd = fileEnd;

	while (nameBegin != fileBegin)
	{
		// Pre-decrement, end never dereferenceable
		--nameBegin;

		// Stop on first path separator
		if (is_path_separator(*nameBegin))
		{
			// Separator not included
			++nameBegin;
			break;
		}
		// Reset stem end on first extension separator
		else if (is_extension_separator(*nameBegin) && stemEnd == fileEnd)
			stemEnd = nameBegin;
	}

	return make_range(nameBegin, stemEnd);
}
/// Gets the file stem, e.g. 'test' from '../test.txt'.
template <class String, class Range>
LEAN_INLINE String get_stem(const Range &file)
{
	return from_range<String>(
		get_stem(file.begin(), file.end()) );
}
/// Gets the file stem, e.g. 'test' from '../test.txt'.
template <class String, class Char>
LEAN_INLINE String get_stem(const Char *file)
{
	return from_range<String>(
		get_stem(
			file,
			file + std::char_traits<Char>::length(file) ) );
}
/// Gets the file stem, e.g. 'test' from '../test.txt'.
template <class Char>
LEAN_INLINE std::basic_string<Char> get_stem(const Char *file)
{
	return get_stem< std::basic_string<Char> >(file);
}

/// Gets the beginning of the file extension, e.g. '.txt' from 'test.txt'.
template <class Iterator>
inline Iterator get_extension(Iterator fileBegin, Iterator fileEnd)
{
	Iterator extBegin = fileEnd;

	while (extBegin != fileBegin)
	{
		// Pre-decrement, end never dereferenceable
		--extBegin;

		// Stop on first extension or stop early on path separator
		if (is_extension_separator(*extBegin) || is_path_separator(*extBegin))
			break;
	}

	// Make sure return value is empty when no extension found
	if (extBegin != fileEnd && !is_extension_separator(*extBegin))
		extBegin = fileEnd;

	return extBegin;
}
/// Gets the file extension, e.g. '.txt' from 'test.txt'.
template <class String, class Range>
LEAN_INLINE String get_extension(const Range &file)
{
	return String(
		get_extension(file.begin(), file.end()),
		file.end() );
}
/// Gets the file extension, e.g. '.txt' from 'test.txt'.
template <class Char>
LEAN_INLINE const Char* get_extension(const Char *file)
{
	return get_extension(
		file,
		file + std::char_traits<Char>::length(file) );
}

} // namespace

using io::file_exists;
using io::file_revision;
using io::file_size;

using io::current_directory;
using io::initial_directory;

using io::relative_path;
using io::absolute_path;
using io::canonical_path;
using io::append_path;

using io::get_directory;
using io::get_filename;
using io::get_stem;
using io::get_extension;

} // namespace

#ifdef LEAN_INCLUDE_LINKED
#include "source/filesystem.cpp"
#endif

#endif