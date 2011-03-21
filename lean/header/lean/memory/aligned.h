/*****************************************************/
/* lean Memory                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MEMORY_ALIGNED
#define LEAN_MEMORY_ALIGNED

#include "../lean.h"
#include "alignment.h"
#include "crt_heap.h"

namespace lean
{
namespace memory
{
	/// Aligns derived classes according to the given alignment template argument.
	template <size_t Alignment, class Heap = crt_heap>
	class aligned : public stack_aligned<Alignment>
	{
	protected:
		LEAN_INLINE aligned() { };
#ifndef LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
		LEAN_INLINE ~aligned() throw() { };
#endif

	public:
		/// Allocates an aligned block of memory of the given size.
		LEAN_INLINE void* operator new(size_t size)
		{
			return Heap::allocate<Alignment>(size);
		}
		/// Allocates an aligned block of memory of the given size.
		LEAN_INLINE void* operator new[](size_t size)
		{
			return Heap::allocate<Alignment>(size);
		}
		/// Frees an aligned block of memory of the given size.
		LEAN_INLINE void operator delete(void *memory)
		{
			return Heap::free<Alignment>(memory);
		}
		/// Allocates an aligned block of memory of the given size.
		LEAN_INLINE void operator delete[](void *memory)
		{
			return Heap::free<Alignment>(memory);
		}
	};

} // namespace

using memory::aligned;

} // namespace

#endif