/*****************************************************/
/* lean Memory                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MEMORY_CHUNK_HEAP
#define LEAN_MEMORY_CHUNK_HEAP

#include "../lean.h"
#include "../tags/noncopyable.h"
#include "alignment.h"
#include "default_heap.h"

namespace lean
{
namespace memory
{

/// Contiguous chunk allocator heap.
template <size_t ChunkSize, class Heap = default_heap, size_t StaticChunkSize = ChunkSize, size_t DefaultAlignment = sizeof(void*)>
class chunk_heap : public lean::noncopyable
{
public:
	/// Heap type.
	typedef Heap heap_type;
	/// Size type.
	typedef typename heap_type::size_type size_type;
	/// Chunk size.
	static const size_type chunk_size = ChunkSize;
	/// Default alignment.
	static const size_type default_alignment = DefaultAlignment;

private:
	// Optional first static chunk
	char m_firstChunk[StaticChunkSize];
	
	// Current chunk
	char *m_chunk;
	char *m_chunkOffset;
	char *m_chunkEnd;

	// Next chunk size
	size_type m_nextChunkSize;

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
	char* allocate_aligned(size_type size)
	{
		// Get next free memory location
		char *aligned = align<Alignment>(m_chunkOffset);

		// Allocate new chunk, if old chunk too small
		if (size + static_cast<size_type>(aligned - m_chunkOffset) > static_cast<size_type>(m_chunkEnd - m_chunkOffset))
		{
			// Make sure new chunk is large enough for requested amount of memory + alignment
			size_type alignedSize = size + (Alignment - 1);

			size_type nextChunkSize = m_nextChunkSize;
			if (nextChunkSize < alignedSize)
				nextChunkSize = alignedSize;

			nextChunkSize += sizeof(chunk_header);

			char *nextChunk = static_cast<char*>( Heap::allocate<chunk_alignment>(nextChunkSize) );
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
	LEAN_INLINE chunk_heap(size_type chunkSize = ChunkSize)
		: m_chunk(m_firstChunk),
		m_chunkOffset(m_firstChunk),
		m_chunkEnd(m_firstChunk + StaticChunkSize),
		m_nextChunkSize(chunkSize) { }
	/// Destructor
	LEAN_INLINE ~chunk_heap()
	{
		clear();
	}

	/// Sets the next chunk size.
	LEAN_INLINE void nextChunkSize(size_type nextSize) { m_nextChunkSize = nextSize; }
	/// Gets the next chunk size.
	LEAN_INLINE size_type nextChunkSize() const { return m_nextChunkSize; }

	/// Gets the remaining capacity of the current chunk.
	LEAN_INLINE size_type capacity() const { return m_chunkEnd - m_chunkOffset; }

	/// Tweaks the next chunk size to exactly fit the given amount of objects about to be allocated.
	/// WARNING: Either call @code nextChunkSize()@endcode after you're done allocating or recall
	/// @code reserve()@endcode for sensible reallocation behavior in subsequent bulk allocations.
	LEAN_INLINE void reserve(size_type newCapacity)
	{
		if (newCapacity > capacity()) 
			nextChunkSize(newCapacity - capacity());
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