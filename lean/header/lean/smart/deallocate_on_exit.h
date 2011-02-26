/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_REF_COUNTER
#define LEAN_SMART_REF_COUNTER

#include <memory>

#include "../tags/noncopyable.h"

namespace lean
{
namespace smart
{

/// Deallocates a given value using a given allocator on destruction.
template < class Type, class Allocator = std::allocator<Type> >
class deallocate_on_exit : public noncopyable
{
public:
	// Allocator
	typedef typename Allocator::template rebind<Counter>::other _allocator_type;
	_allocator_type m_allocator;

	// Reference counts
	typedef typename _allocator_type::pointer_type _pointer_type;
	_pointer_type m_pointer;

public:
	/// Type of the allocator used by this class.
	typedef _allocator_type allocator_type;
	/// Type of the pointer stored by this class.
	typedef _pointer_type pointer_type;
	
	/// Constructor.
	explicit deallocate_on_exit(const pointer_type& pointer)
		: m_pointer(pointer) { }
	/// Constructor.
	deallocate_on_exit(const pointer_type& pointer, const allocator_type& allocator)
		: m_allocator(allocator),
		m_pointer(pointer) { }
	/// Destructor.
	~deallocate_on_exit()
	{
		m_allocator.deallocate(m_pointer, 1);
	}
};

} // namespace

using smart::deallocate_on_exit;

} // namespace

#endif