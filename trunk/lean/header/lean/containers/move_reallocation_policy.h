/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_MOVE_REALLOCATION_POLICY
#define LEAN_CONTAINERS_MOVE_REALLOCATION_POLICY

#include "../lean.h"

namespace lean
{
namespace containers
{

/// Allows for custom move semantics on element reallocation.
/** This class implements special element reallocation behavior,
  * calling a custom move method for every element to be moved
  * rather than copy constructing all existing elements on reallocation. */
template <class Container, int GrowthDenominator = 2>
class move_reallocation_policy
{
private:
	typedef typename Container::iterator iterator;

public:
	/// Size type used by this policy class.
	typedef typename Container::size_type size_type;

	/// Reserves memory for the specified number of elements,
	/// calling a custom move method for every element to be moved.
	/** Rather than copy constructing all existing elements into the
	  * new block of memory after reallocation, this method first performs
	  * default construction of all elements in the new range, issueing
	  * calls to a custom move method for each element in the old block
	  * of memory afterwards, passing the old elements as arguments to the
	  * move method of their corresponding new elements. */
	static void reserve(Container &container, size_type newCapacity)
	{
		if (newCapacity > container.capacity())
		{
			Container newContainer;

			newContainer.reserve(newCapacity);
			newContainer.resize(container.size());

			for (iterator itOld = container.begin(), itNew = newContainer.begin();
				 itNew != newContainer.end(); ++itOld, ++itNew)
				itNew->move(*itOld);

			using std::swap;
			swap(container, newContainer);
		}

		LEAN_ASSERT(newCapacity <= container.capacity());
	}

	/// Reserves a new block of memory, if the old block of memory can
	/// no longer serve the given new element count.
	static void pre_resize(Container &container, size_type newCount)
	{
		size_t capacity = container.capacity();

		if (newCount > capacity)
		{
			size_type capacityDelta = capacity / GrowthDenominator;
			size_type maxSize = container.max_size();

			// Mind overflow
			capacity = (maxSize - capacityDelta < capacity)
				? maxSize
				: capacity + capacityDelta;

			if (capacity < newCount)
				capacity = newCount;

			reserve(container, capacity);
		}

		LEAN_ASSERT(newCount <= container.capacity());
	}
};

} // namespace

using containers::move_reallocation_policy;

} // namespace

#endif