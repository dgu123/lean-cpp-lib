/*****************************************************/
/* lean Memory                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MEMORY_POOL_ALLOCATOR
#define LEAN_MEMORY_POOL_ALLOCATOR

#include "../lean.h"
#include "../tags/noncopyable.h"
#include "chunk_heap.h"
#include "default_heap.h"

namespace lean
{
namespace memory
{

/// Contiguous chunk allocator heap.
template <class Element, class Heap = default_heap, size_t ChunkSize, size_t StaticChunkSize = ChunkSize, size_t Alignment = alignof(Element)>
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
	typedef chunk_heap<ChunkSize * sizeof(Element), Heap, StaticChunkSize * sizeof(Element), Alignment> chunk_heap;
	chunk_heap m_heap;

	Element **m_pFirstFree;

public:
	/// Constructor.
	LEAN_INLINE pool_allocator()
		: m_pFirstFree(nullptr) { }
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
	LEAN_INLINE void* allocate()
	{
		return allocate<default_alignment>(size);
	}
	/// Frees the given block of memory.
	LEAN_INLINE void free(void *memory) { free<default_alignment>(memory); }
};

} // namespace

using memory::chunk_heap;

} // namespace

#endif