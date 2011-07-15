/*****************************************************/
/* lean Memory                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MEMORY_CHUNK_HEAP
#define LEAN_MEMORY_CHUNK_HEAP

#include "../lean.h"
#include "../tags/noncopyable.h"
#include "alignment.h"

namespace lean
{
namespace memory
{

/// Contiguous chunk allocator heap.
template <class Heap, size_t ChunkSize, size_t StaticChunkSize = ChunkSize>
class chunk_heap : public lean::noncopyable
{
public:
	/// Heap type.
	typedef Heap heap_type;
	/// Size type.
	typedef typename heap_type::size_type size_type;
	/// Chunk size.
	static const size_type chunk_size = ChunkSize;

private:
	// Optional first static chunk
	char m_firstChunk[StaticChunkSize];
	
	// Current chunk
	char *m_chunk;
	char *m_chunkOffset;
	char *m_chunkEnd;

	/// Chunk header
	struct chunk_header
	{
		char *prev_chunk;

		/// Constructor.
		chunk_header(char *prev_chunk)
			: prev_chunk(prev_chunk) { }
	};
	// Chunk alignment
	static const size_t chunk_alignment = alignof(chunk_header);

	/// Allocates the given amount of memory.
	template <size_t Alignment>
	void allocate_aligned(size_type size)
	{
		// Get next free memory location
		char *aligned = align<Alignment>(m_chunkOffset);

		// Allocate new chunk, if old chunk too small
		if (size > static_cast<size_type>(m_chunkEnd - aligned))
		{
			// Make sure new chunk is large enough for requested amount of memory + alignment
			size_type alignedSize = size + (Alignment - 1);

			size_type nextChunkSize = ChunkSize;
			if (nextChunkSize < alignedSize)
				nextChunkSize = alignedSize;

			nextChunkSize += sizeof(chunk_header);

			char *nextChunk = Heap::allocate<chunk_alignment>(nextChunkSize);
			new( static_cast<void*>(nextChunk) ) chunk_header(m_chunk);

			m_chunk = nextChunk;
			m_chunkOffset = nextChunk + sizeof(chunk_header);
			m_chunkEnd = nextChunk + nextChunkSize;

			// Get next free memory location
			aligned = align<Alignment>(m_chunkOffset);
		}

		// Memory now occupied
		m_chunkOffset = aligned + size;

		return aligned;
	}

public:
	/// Constructor.
	LEAN_INLINE chunk_heap()
		: m_chunk(m_firstChunk),
		m_chunkOffset(m_firstChunk),
		m_chunkEnd(m_firstChunk + StaticChunkSize) { }
	/// Destructor
	LEAN_INLINE ~chunk_heap()
	{
		clear();
	}

	/// Frees all chunks allocated by this allocator.
	void clear()
	{
		// Re-initialize first (exception-safe)
		m_chunkOffset = m_firstChunk;
		m_chunkEnd = m_firstChunk + StaticChunkSize;

		// Free as many chunks as possible
		while (m_chunk != m_firstChunk)
		{
			char *freeChunk = m_chunk;

			// Immediately store previous chunk in case clear is re-called after exception
			m_chunk = reinterpret_cast<chunk_header*>(m_chunk)->prev_chunk;

			Heap::free<chunk_alignment>(freeChunk);
		}
	}

	/// Allocates the given amount of memory.
	LEAN_INLINE void* allocate(size_type size) { return allocate<1>(size); }
	/// Frees the given block of memory.
	LEAN_INLINE void free(void *memory) { free<1>(memory); }

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