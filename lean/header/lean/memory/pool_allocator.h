/*****************************************************/
/* lean Memory                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MEMORY_POOL_ALLOCATOR
#define LEAN_MEMORY_POOL_ALLOCATOR

#include "../lean.h"
#include "../tags/noncopyable.h"
#include "chunk_heap.h"

namespace lean
{
namespace memory
{

/// Contiguous chunk allocator heap.
template <class Element, class Heap, size_t ChunkSize, size_t StaticChunkSize = ChunkSize, size_t Alignment = alignof(Element)>
class pool_allocator : public lean::noncopyable
{
public:
	/// Value type.
	typedef Element value_type;
	/// Heap type.
	typedef Heap heap_type;
	/// Size type.
	typedef typename heap_type::size_type size_type;
	/// Chunk size.
	static const size_type chunk_size = ChunkSize;
	/// Alignment.
	static const size_type alignment = Alignment;

private:
	typedef chunk_heap<Heap, ChunkSize * sizeof(Element), StaticChunkSize * sizeof(Element), Alignment> chunk_heap;
	chunk_heap m_heap;

	bi8s
public:
	/// Constructor.
	LEAN_INLINE pool_allocator() { }
	/// Destructor
	LEAN_INLINE ~pool_allocator()
	{
		clear();
	}

	/// Frees all chunks allocated by this allocator.
	void clear()
	{
		chunk_heap.clear();
	}

	// TODO: clear vs free

	/// Allocates the given amount of memory.
	LEAN_INLINE void* allocate(size_type size) { return allocate<default_alignment>(size); }
	/// Frees the given block of memory.
	LEAN_INLINE void free(void *memory) { free<default_alignment>(memory); }

	/// Allocates the given amount of memory respecting the given alignment.
	template <size_t Alignment>
	LEAN_INLINE void* allocate(size_type size)
	{
		return allocate_aligned<Alignment>(size);
	}
	/// Frees the given aligned block of memory.
	template <size_t Alignment>
	LEAN_INLINE void free(void *memory)
	{
		// Freeing of individual memory blocks unsupported
	}
};

} // namespace

using memory::chunk_heap;

} // namespace

#endif