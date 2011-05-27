/*****************************************************/
/* lean I/O                     (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_IO_FILESYSTEM
#define LEAN_IO_FILESYSTEM

#include "../lean.h"
#include "../strings/types.h"
#include <utility>

namespace lean
{
namespace io
{

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
inline bool is_path_separator(Char chr)
{
	return (chr == filesystem_chars<Char>::path_separator) ||
		(chr == filesystem_chars<Char>::alt_path_separator);
}

/// Checks if the given character is an extension separator.
template <class Char>
inline bool is_extension_separator(Char chr)
{
	return (chr == filesystem_chars<Char>::extension_separator);
}

/// Checks if the given character is a redirection character.
template <class Char>
inline bool is_redirection(Char chr)
{
	return (chr == filesystem_chars<Char>::redirection);
}

/// Gets the relative path euqivalent to the given absolute path when starting at the given base.
/// Does not resolve redirections, use CanonicalPath prior to calling this function to resolve these.
template <class String>
inline String relative_path(const String &base, const String &path)
{
	typename String::const_iterator baseMarker = base.begin(), baseCursor = base.begin(),
		pathMarker = path.begin(), pathCursor = path.begin();

	bool endOfBase = (baseCursor == base.end()),
		endOfPath = (pathCursor == path.end());
	bool divergent = false;

	// Skip identical parts
	while (!divergent && !endOfBase && !endOfPath)
	{
		while (!endOfBase && !is_path_separator(*baseCursor))
			endOfBase = (++baseCursor == base.end());
		while (!endOfPath && !is_path_separator(*pathCursor))
			endOfPath = (++pathCursor == path.end());

		divergent = (baseCursor - baseMarker != pathCursor - pathMarker)
			|| !std::equal(baseMarker, baseCursor, pathMarker);

		if (!endOfBase)
			endOfBase = (++baseCursor == base.end());
		if (!endOfPath)
			endOfPath = (++pathCursor == path.end());

		baseMarker = baseCursor;
		pathMarker = pathCursor;
	}

	String result;
	result.reserve( static_cast<size_t>(base.end() - baseMarker) + static_cast<size_t>(path.end() - pathMarker) );

	while (baseCursor != base.end())
	{
		if (is_path_separator(*baseCursor))
		{
			if (!result.empty())
				result.append(PathSeparator);
			result.append(RedirectionCharacter);
			result.append(RedirectionCharacter);
		}

		++baseCursor;
	}

	if (pathCursor != path.end())
	{
		if (!result.empty())
			result.append(PathSeparator);

		result.append(pathCursor, path.end());
	}

	return result;
}

/// Gets the absolute path euqivalent to the given relative path.
template <class String>
inline String absolute_path(const String &base, const String &path)
{
	return CanonicalPath(AppendPath(base, path));
}

/// Gets a canonical relative path euqivalent to the given relative path.
template <class String>
inline String canonical_path(const String &path)
{
	String result(path);

	typename String::const_iterator srcEnd = path.end(),
		srcInsertEnd = path.end();
	typename String::iterator destBegin = result.end();

	int skipCounter = 0;
	int redirCounter = 0;

	bool endOfPath = (srcEnd == path.begin());

	while (!endOfPath)
	{
		// Pre-decrement, end never dereferenceable
		endOfPath = (--srcEnd == path.begin());

		bool isPathSeparator = is_path_separator(*srcEnd);

		// End of current directory
		if (isPathSeparator || endOfPath)
		{
			typename String::const_iterator srcInsertBegin = destEnd;

			// Don't copy leading path separators
			if (isPathSeparator)
				++srcInsertBegin;

			// Ignore empty directory names & local directory
			if (redirCounter != 0 && redirCounter != 1)
			{
				// Up one directory, skip next actual directory name
				if(redirCounter == 2)
					++skipCounter;
				// Actual directory name, prepend or skip
				else if (skipCounter == 0)
					destBegin = std::copy_backward(srcInsertBegin, srcInsertEnd, destBegin);
				else
					--skipCounter;
			}

			srcInsertEnd = srcInsertBegin;
			redirCounter = 0;
		}
		// Count redirection characters, as long as no other characters around
		else if (is_redirection(*srcEnd) && redirCounter != -1)
			++redirCounter;
		// Not a redirection, treat as actual directory name
		else
			redirCounter = -1;
	}

	// Prepend unresolved redirections
	while (skipCounter != 0)
	{
		*(--destBegin) = PathSeparator;
		*(--destBegin) = RedirectionCharacter;
		*(--destBegin) = RedirectionCharacter;
		--skipCounter;
	}

	// Move to front
	if (destBegin != result.begin())
		result.assign(destBegin, result.end());

	return result;
}

/// Appends the given file or directory to the given path.
template <class String>
inline String append_path(const String &path, const String &file)
{
	String result;

	result.reserve(path.size() + 1 + file.size());
	result.append(path);

	if (!path.empty())
	{
		typename String::const_iterator pathBack = --path.end();

		if (!is_path_separator(*pathBack))
			result.append(PathSeparator);
	}

	result.append(file);
	return result;
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
template <class String>
LEAN_INLINE String get_directory(const String &file)
{
	return get_directory<String>(file);
}
/// Gets the parent directory, e.g. '..' from '../test.txt'.
template <class Char>
LEAN_INLINE std::basic_string<Char> get_directory(const Char *file)
{
	return get_directory< std::basic_string<Char> >( make_range(file, file + std::char_traits<Char>::length(file)) );
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
template <class String>
LEAN_INLINE String get_filename(const String &file)
{
	return get_filename<String>(file);
}
/// Gets the file name, e.g. 'test.txt' from '../test.txt'.
template <class Char>
LEAN_INLINE const Char* get_filename(const Char *file)
{
	return get_filename(file, file + std::char_traits<Char>::length(file));
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
	range<typename Range::const_iterator> stemRange = get_stem(file.begin(), file.end());

	return String(stemRange.begin(), stemRange.end());
}
/// Gets the file stem, e.g. 'test' from '../test.txt'.
template <class String>
LEAN_INLINE String get_stem(const String &file)
{
	return get_stem<String>(file);
}
/// Gets the file stem, e.g. 'test' from '../test.txt'.
template <class Char>
LEAN_INLINE std::basic_string<Char> get_stem(const Char *file)
{
	return get_stem< std::basic_string<Char> >( make_range(file, file + std::char_traits<Char>::length(file)) );
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
template <class String>
LEAN_INLINE String get_extension(const String &file)
{
	return get_extension<String>(file);
}
/// Gets the file extension, e.g. '.txt' from 'test.txt'.
template <class Char>
LEAN_INLINE const Char* get_extension(const Char *file)
{
	return get_extension(file, file + std::char_traits<Char>::length(file));
}

} // namespace

using io::get_directory;
using io::get_filename;
using io::get_stem;
using io::get_extension;

} // namespace

#endif