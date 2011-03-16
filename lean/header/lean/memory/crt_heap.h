/*****************************************************/
/* lean Memory                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MEMORY_CRT_HEAP
#define LEAN_MEMORY_CRT_HEAP

#include "../lean.h"
#include "alignment.h"

namespace lean
{
namespace memory
{

/// Default CRT heap.
struct crt_heap
{
	/// Size type.
	typedef size_t size_type;

	/// Allocates the given amount of memory.
	static LEAN_INLINE void* allocate(size_type size) { return ::operator new(size); }
	/// Frees the given block of memory.
	static LEAN_INLINE void free(void *memory) { ::operator delete(memory); }

	/// Allocates the given amount of memory respecting the given alignment.
	template <size_t Alignment>
	static LEAN_INLINE void* allocate(size_type size)
	{
		LEAN_STATIC_ASSERT_MSG_ALT(Alignment > reinterpret_cast<unsigned char>(-1),
			"Alignment > max unsigned char unsupported.",
			Alignment_bigger_than_max_unsigned_char_unsupported);

		unsigned char *unaligned = reinterpret_cast<unsigned char*>( ::operator new(size + Alignment) );
		unsigned char *aligned = upper_align<Alignment>(unaligned);
		aligned[-1] = reinterpret_cast<unsigned char>(aligned - unaligned);
		return aligned;
	}
	/// Frees the given aligned block of memory.
	template <size_t Alignment>
	static LEAN_INLINE void free(void *memory)
	{
		if (memory)
			::operator delete(memory - reinterpret_cast<unsigned char*>(memory)[-1]);
	}

	template <>
	static LEAN_INLINE void* allocate<1>(size_type size) { return allocate(size); }
	template <>
	static LEAN_INLINE void free<1>(void *memory) { free(memory); }
};

} // namespace

using memory::crt_heap;

} // namespace

#endif