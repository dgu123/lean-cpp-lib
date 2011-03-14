/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_MOVE_REALLOCATION_POLICY
#define LEAN_CONTAINERS_MOVE_REALLOCATION_POLICY

namespace lean
{
namespace containers
{

/// Allows for custom move semantics on element reallocation.
/** This class implements special element reallocation behavior,
  * calling a custom move method for every element to be moved
  * rather than copy constructing all existing elements on reallocation. */
template <class Container, int GrowthDenominator = 2>
class move_reservation_policy
{
public:
	/// Reserves memory for the specified number of elements,
	/// calling a custom move method for every element to be moved.
	/** Rather than copy constructing all existing elements into the
	  * new block of memory after reallocation, this method first performs
	  * default construction of all elements in the new range, issueing
	  * calls to a custom move method for each element in the old block
	  * of memory afterwards, passing the old elements as arguments to the
	  * move method of their corresponding new elements. */
	static inline void reserve(Container &container, typename Container::size_type newCapacity)
	{
		if (newCapacity > container.capacity())
		{
			Container newContainer;

			newContainer.reserve(newCapacity);
			newContainer.resize(container.size());

			for (typename Container::iterator itOld = container.begin(), itNew = newContainer.begin();
				 itOld != container.end(); ++itOld, ++itNew)
				itNew->move(*itOld);

			container.swap(newContainer);
		}
	}

	/// Reserves a new block of memory, if the old block of memory can
	/// no longer serve the given new element count.
	static inline void reallocate(Container &container, typename Container::size_type newCount)
	{
		typename Container::size_type capacity = container.capacity();

		if (newCount > capacity)
		{
			typename Container::size_type capacityDelta = capacity / GrowthDenominator;

			capacity = (container.max_size() - capacityDelta < capacity)
				? 0
				: capacity + capacityDelta;

			if (capacity < newCount)
				capacity = newCount;

			reserve(container, capacity);
		}
	}
};

} // namespace
} // namespace

#endif