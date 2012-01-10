/*****************************************************/
/* lean I/O                     (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_IO_FILESYSTEM
#define LEAN_IO_FILESYSTEM

#include "../lean.h"
#include "../strings/types.h"
#include "../strings/conversions.h"
#include "../strings/string_traits.h"
#include "../meta/strip.h"
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
	static const Char path_separator;
	/// Alternative path separator character.
	static const Char alt_path_separator;
	/// Extension separator character.
	static const Char extension_separator;
	/// Redirection character.
	static const Char redirection;
};

template <class Char>
const Char filesystem_chars<Char>::path_separator = '/';
template <class Char>
const Char filesystem_chars<Char>::alt_path_separator = '\\';
template <class Char>
const Char filesystem_chars<Char>::extension_separator = '.';
template <class Char>
const Char filesystem_chars<Char>::redirection = '.';

/// Checks if the given character is an alternative path separator.
template <class Char>
LEAN_INLINE bool is_alt_path_separator(Char chr)
{
	return (chr == filesystem_chars<Char>::alt_path_separator);
}

/// Checks if the given character is a path separator.
template <class Char>
LEAN_INLINE bool is_path_separator(Char chr)
{
	return (chr == filesystem_chars<Char>::path_separator) ||
		is_alt_path_separator(chr);
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

/// Replaces an alternative path separator by a canonical path separator in the given character.
template <class Char>
LEAN_INLINE Char& canonize_path_separator(Char &chr)
{
	if (is_alt_path_separator(chr))
		assign_path_separator(chr);
	return chr;
}

/// Gets the relative path euqivalent to the given absolute path when starting at the given base.
/// Does not resolve redirections, use CanonicalPath prior to calling this function to resolve these.
template <class String, class Range1, class Range2>
inline typename enable_if_range2<Range1, Range2, String>::type relative_path(const Range1 &base, const Range2 &path)
{
	typedef string_traits<String> string_traits;

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
	string_traits::reserve(
			result,
			static_cast<size_t>(base.end() - baseMarker)
			+ static_cast<size_t>(path.end() - pathMarker)
		);

	// Slash already skipped, don't lose corresponding directory
	int mismatchCount = (baseMarker != base.end()) ? 1 : 0;
	
	// Count mismatching base sub-directories
	for (baseCursor = baseMarker; baseCursor != base.end(); ++baseCursor)
		if (is_path_separator(*baseCursor))
			++mismatchCount;

	if (mismatchCount != 0)
	{
		string_traits::resize(result, 3U * mismatchCount - 1U);
		typename string_traits::iterator insertCursor = string_traits::begin(result);

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
		bool needSeparation = !string_traits::empty(result);
		
		size_t insertPos = string_traits::size(result);
		string_traits::resize(result, insertPos + static_cast<size_t>(path.end() - pathMarker) + needSeparation);
		typename string_traits::iterator insertCursor = string_traits::begin(result) + insertPos;
		
		if (needSeparation)
			assign_path_separator(*(insertCursor++));

		insertCursor = std::copy(pathMarker, path.end(), insertCursor);
	}

	return result;
}
/// Gets the relative path euqivalent to the given absolute path when starting at the given base.
/// Does not resolve redirections, use CanonicalPath prior to calling this function to resolve these.
template <class String, class Chars1, class Chars2>
LEAN_INLINE typename enable_if_not_range2<Chars1, Chars2, String>::type relative_path(const Chars1 &base, const Chars2 &path)
{
	return relative_path<String>(make_char_range(base), make_char_range(path));
}
/// Gets the relative path euqivalent to the given absolute path when starting at the given base.
/// Does not resolve redirections, use CanonicalPath prior to calling this function to resolve these.
template <class Chars1, class Chars2>
LEAN_INLINE std::basic_string<typename range_char_type2<Chars1, Chars2>::type> relative_path(const Chars1 &base, const Chars2 &path)
{
	return relative_path< std::basic_string<typename range_char_type2<Chars1, Chars2>::type> >(make_char_range(base), make_char_range(path));
}

/// Gets a canonical relative path euqivalent to the given relative path.
template <class String, class Range>
inline typename enable_if_range<Range, String>::type canonical_path(const Range &path)
{
	typedef string_traits<String> string_traits;

	String result;
	string_traits::resize(result, path.size());

	typename Range::const_iterator srcCursor = path.end(),
		srcMarker = path.end();
	typename string_traits::iterator destCursor = string_traits::end(result);

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
				{
					// Prepend directory
					typename string_traits::iterator newDestCursor = std::copy_backward(srcInsertionCursor, srcMarker, destCursor);

					// Replace trailing & leading alternative path separators
					canonize_path_separator(*(--destCursor));
					canonize_path_separator(*newDestCursor);

					destCursor = newDestCursor;
				}
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
	if (destCursor != string_traits::begin(result))
		string_traits::erase(result, string_traits::begin(result), destCursor);

	return result;
}
/// Gets a canonical relative path euqivalent to the given relative path.
template <class String, class Chars>
LEAN_INLINE typename enable_if_not_range<Chars, String>::type canonical_path(const Chars &path)
{
	return canonical_path<String>(make_char_range(path));
}
/// Gets a canonical relative path euqivalent to the given relative path.
template <class Chars>
LEAN_INLINE std::basic_string<typename range_char_type<Chars>::type> canonical_path(const Chars &path)
{
	return canonical_path< std::basic_string<typename range_char_type<Chars>::type> >(make_char_range(path));
}

/// Appends the given file or directory to the given path.
template <class String, class Range1, class Range2>
inline typename enable_if_range2<Range1, Range2, String>::type append_path(const Range1 &path, const Range2 &file)
{
	typedef string_traits<String> string_traits;

	String result;

	string_traits::resize(result, path.size() + 1 + file.size());
	typename string_traits::iterator insertCursor = string_traits::begin(result);

	insertCursor = std::copy(path.begin(), path.end(), insertCursor);

	if (!path.empty() && !file.empty() &&
		!is_path_separator(*(path.end() - 1)) && !is_path_separator(*file.begin()))
		assign_path_separator(*(insertCursor++));

	insertCursor = std::copy(file.begin(), file.end(), insertCursor);

	string_traits::erase(result, insertCursor, result.end());
	return result;
}
/// Appends the given file or directory to the given path.
template <class String, class Chars1, class Chars2>
LEAN_INLINE typename enable_if_not_range2<Chars1, Chars2, String>::type append_path(const Chars1 &path, const Chars2 &file)
{
	return append_path<String>(make_char_range(path), make_char_range(file));
}
/// Appends the given file or directory to the given path.
template <class Chars1, class Chars2>
LEAN_INLINE std::basic_string<typename range_char_type2<Chars1, Chars2>::type> append_path(const Chars1 &path, const Chars2 &file)
{
	return append_path< std::basic_string<typename range_char_type2<Chars1, Chars2>::type> >(make_char_range(path), make_char_range(file));
}

/// Gets the absolute path euqivalent to the given relative path.
template <class String, class Range1, class Range2>
LEAN_INLINE String absolute_path(const Range1 &base, const Range2 &path)
{
	return canonical_path<String>( append_path<String>(make_char_range(base), make_char_range(path)) );
}
/// Gets the absolute path euqivalent to the given relative path.
template <class Chars1, class Chars2>
LEAN_INLINE std::basic_string<typename range_char_type2<Chars1, Chars2>::type> absolute_path(const Chars1 &base, const Chars2 &path)
{
	typedef std::basic_string<typename range_char_type2<Chars1, Chars2>::type> string_type;
	return canonical_path<string_type>( append_path<string_type>(make_char_range(base), make_char_range(path)) );
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
	return string_traits<String>::construct(
			file.begin(),
			get_directory( file.begin(), file.end() )
		);
}
/// Gets the parent directory, e.g. '..' from '../test.txt'.
template <class String, class Char>
LEAN_INLINE String get_directory(const Char *file)
{
	return string_traits<String>::construct(
			file,
			get_directory( file, file + std::char_traits<Char>::length(file) )
		);
}
/// Gets the parent directory, e.g. '..' from '../test.txt'.
template <class Char>
LEAN_INLINE std::basic_string<Char> get_directory(const Char *file)
{
	return std::basic_string<Char>(
			file,
			get_directory( file, file + std::char_traits<Char>::length(file) )
		);
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
	return string_traits<String>::construct(
			get_filename( file.begin(), file.end() ),
			file.end()
		);
}
/// Gets the file name, e.g. 'test.txt' from '../test.txt'.
template <class Char>
LEAN_INLINE const Char* get_filename(const Char *file)
{
	return get_filename(
			file,
			file + std::char_traits<Char>::length(file)
		);
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
	return string_from_range<String>(
			get_stem( file.begin(), file.end() )
		);
}
/// Gets the file stem, e.g. 'test' from '../test.txt'.
template <class String, class Char>
LEAN_INLINE String get_stem(const Char *file)
{
	return string_from_range<String>(
			get_stem( file, file + std::char_traits<Char>::length(file) )
		);
}
/// Gets the file stem, e.g. 'test' from '../test.txt'.
template <class Char>
LEAN_INLINE std::basic_string<Char> get_stem(const Char *file)
{
	return string_from_range< std::basic_string<Char> >(
			get_stem( file, file + std::char_traits<Char>::length(file) )
		);
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
	return string_traits<String>::construct(
			get_extension( file.begin(), file.end() ),
			file.end()
		);
}
/// Gets the file extension, e.g. '.txt' from 'test.txt'.
template <class Char>
LEAN_INLINE const Char* get_extension(const Char *file)
{
	return get_extension( file, file + std::char_traits<Char>::length(file) );
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