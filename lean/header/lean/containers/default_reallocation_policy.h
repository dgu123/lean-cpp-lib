/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_DEFAULT_REALLOCATION_POLICY
#define LEAN_CONTAINERS_DEFAULT_REALLOCATION_POLICY

namespace lean
{
namespace containers
{

/// Leaves reallocation to the specified container type.
/** This class does not implement any special way of reallocating elements. */
template <class Container>
class default_reallocation_policy
{
public:
	/// Reserves memory for the specified number of elements using the given container's default reserve method.
	static LEAN_INLINE void reserve(Container &container, typename Container::size_type newCapacity)
	{
		container.reserve(newCapacity);
	}

	/// Does not reserve anything, entirely leaving element reallocation to the underlying container type.
	static LEAN_INLINE void pre_resize(Container &container, typename Container::size_type newCount)
	{
	}
};

} // namespace
} // namespace

#endif