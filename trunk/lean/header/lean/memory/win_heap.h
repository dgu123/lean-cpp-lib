/*****************************************************/
/* lean Memory                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MEMORY_WIN_HEAP
#define LEAN_MEMORY_WIN_HEAP

#include "../lean.h"
#include "alignment.h"

#ifndef LEAN_ASSUME_WIN_ALIGNMENT
	// MONITOR: Windows heap aligns memory to 8 byte (16 on x64) boundaries by default
	// see http://stackoverflow.com/questions/2805896/what-alignment-does-heapalloc-use
	#ifdef LEAN_64_BIT
		/// Specifies alignment requirements that are assumed to always be met by the windows memory management functions
		#define LEAN_ASSUME_WIN_ALIGNMENT 16
	#else
		/// Specifies alignment requirements that are assumed to always be met by the windows memory management functions
		#define LEAN_ASSUME_WIN_ALIGNMENT 8
	#endif
#endif

namespace lean
{
namespace memory
{

/// Windows heap.
struct win_heap
{
	/// Size type.
	typedef size_t size_type;

	/// Allocates the given amount of memory.
	LEAN_MAYBE_EXPORT static void* allocate(size_type size);
	/// Frees the given block of memory.
	LEAN_MAYBE_EXPORT static void free(void *memory);

	/// Allocates the given amount of memory respecting the given alignment.
	template <size_t Alignment>
	static LEAN_INLINE void* allocate(size_type size)
	{
		if (Alignment <= LEAN_ASSUME_WIN_ALIGNMENT && check_alignment<Alignment>::valid)
			return allocate(size);
		else
			// MONITOR: Apparently, the windows heap handles offset pointers gracefully, no need to store anything
			return align<Alignment>( allocate(size + (Alignment - 1)) );
	}
	/// Frees the given aligned block of memory.
	template <size_t Alignment>
	static LEAN_INLINE void free(void *memory)
	{
		// MONITOR: Apparently, the windows heap handles offset pointers gracefully, no need to reconstruct anything
		free(memory);
	}
};

} // namespace

using memory::win_heap;

} // namespace

#ifndef LEAN_LINKING
#include "source/win_heap.cpp"
#endif

#endif