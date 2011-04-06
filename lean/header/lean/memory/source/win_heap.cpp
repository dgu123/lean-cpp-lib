#ifdef LEAN_BUILD_LIB
#include "lean/config/windows.h"
#endif

#include "../win_heap.h"
#include <windows.h>
#include <stdexcept>

namespace lean
{
namespace memory
{
namespace impl
{

/// Gets a handle to this process' heap.
inline HANDLE get_win_process_heap()
{
	// Use one-time initialization to save API call
	static const HANDLE hProcessHeap = ::GetProcessHeap();
	return hProcessHeap;
}

/// Throws a bad_alloc exception.
LEAN_NEVER_INLINE void win_bad_alloc()
{
	static const std::bad_alloc exception;
	throw exception;
}

} // namespace
} // namespace
} // namespace

// Allocates the given amount of memory.
LEAN_MAYBE_INLINE void* lean::memory::win_heap::allocate(size_type size)
{
	void *memory = ::HeapAlloc(impl::get_win_process_heap(), 0, size);

	if (!memory)
		impl::win_bad_alloc();

	// Optimization might skip subsequent pointer checking branches with pMemory != nullptr asserted
	LEAN_ASSERT(memory);
	return memory;
}

// Frees the given block of memory.
LEAN_MAYBE_INLINE void lean::memory::win_heap::free(void *memory)
{
	if (memory)
		::HeapFree(impl::get_win_process_heap(), 0, memory);
}
