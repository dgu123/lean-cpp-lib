/*****************************************************/
/* lean PImpl                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PIMPL_PIMPL_PTR
#define LEAN_PIMPL_PIMPL_PTR

#include "../tags/noncopyable.h"

namespace lean
{
namespace pimpl
{

/// Default PImpl pointer destruction policy.
struct pimpl_delete_policy
{
	/// Calls delete on the given implementation pointer.
	template <class Implementation>
	static void destroy(Implementation *impl)
	{
		if (sizeof(Implementation) > 0)
			delete impl;
	}
};

/// Smart pointer class allowing for secure storage of forward-declared pimpl classes.
template <class Implementation, class ImplementationBase = Implementation, class DestroyPolicy = pimpl_delete_policy>
class pimpl_ptr : public noncopyable
{
private:
	ImplementationBase *m_impl;

	/// Deletes the implementation stored by this pimpl pointer.
	static LEAN_INLINE void delete_impl(const ImplementationBase *impl)
	{
		// Check, if implementation class fully defined
		delete_full_impl( impl, static_cast<const Implementation*>(nullptr) );
	}
	/// Deletes the implementation stored by this pimpl pointer calling the implementation destructor.
	static LEAN_INLINE void delete_full_impl(const ImplementationBase *base, const ImplementationBase*)
	{
		// Overload resolution suggests that implementation class is fully defined
		DestroyPolicy::destroy( static_cast<const Implementation*>(base) );
	}
	/// Deletes the implementation stored by this pimpl pointer calling the base destructor.
	static LEAN_INLINE void delete_full_impl(const ImplementationBase *base, const void*)
	{
		// Overload resolution suggests that implementation definition is incomplete
		DestroyPolicy::destroy( base );
	}

public:
	/// Type of the implementation stored by this pimpl pointer.
	typedef Implementation implementation_type;
	/// Base type of the implementation stored by this pimpl pointer.
	typedef ImplementationBase base_type;

	/// Constructs a pimpl pointer from the given implementation.
	LEAN_INLINE pimpl_ptr(Implementation *impl)
		: m_impl(impl) { }
	/// Moves the implementation managed by the given pimpl pointer to this pimpl pointer.
	LEAN_INLINE pimpl_ptr(pimpl_ptr &&right)
		: m_impl(right.m_impl)
	{
		right.m_impl = nullptr;
	}
	/// Destructs this pimpl pointer, deleting any implementation stored.
	LEAN_INLINE ~pimpl_ptr() throw()
	{
		delete_impl(m_impl);
	}

	/// Deletes any implementation stored, storing the given new implementation in this pimpl pointer.
	pimpl_ptr& operator =(Implementation *impl)
	{
		if (impl != m_impl)
		{
			ImplementationBase *prevImpl = m_impl;
			m_impl = impl;
			delete_impl(prevImpl);
		}

		return *this;
	}
	/// Moves the implementation managed by the given pimpl pointer to this pimpl pointer.
	pimpl_ptr& operator =(pimpl_ptr &&right)
	{
		if (right.m_impl != m_impl)
		{
			ImplementationBase *prevImpl = m_impl;
			
			m_impl = right.m_impl;
			right.m_impl = nullptr;
			
			delete_impl(prevImpl);
		}
		return *this;
	}

	/// Retrieves the implementation stored, clearing this pimpl pointer to nullptr.
	Implementation* unbind()
	{
		Implementation *impl = getptr();
		m_impl = nullptr;
		return impl;
	}

	/// Gets the implementation managed by this pimpl pointer.
	LEAN_INLINE Implementation* getptr(void) { return static_cast<Implementation*>(m_impl); }
	/// Gets the implementation managed by this pimpl pointer.
	LEAN_INLINE const Implementation* getptr(void) const { return static_cast<const Implementation*>(m_impl); }
	/// Gets the implementation managed by this pimpl pointer.
	LEAN_INLINE Implementation& get(void) { return *getptr(); }
	/// Gets the implementation managed by this pimpl pointer.
	LEAN_INLINE const Implementation& get(void) const { return *getptr(); }

	/// Checks wehther this pimpl pointer is currently empty.
	LEAN_INLINE bool empty(void) { return (m_impl == nullptr); }
	
	/// Gets the implementation managed by this pimpl pointer.
	LEAN_INLINE Implementation& operator *() { return get(); }
	/// Gets the implementation managed by this pimpl pointer.
	LEAN_INLINE const Implementation& operator *() const { return get(); }
	/// Gets the implementation managed by this pimpl pointer.
	LEAN_INLINE Implementation* operator ->() { return getptr(); }
	/// Gets the implementation managed by this pimpl pointer.
	LEAN_INLINE const Implementation* operator ->() const { return getptr(); }
};

} // namespace

using pimpl::pimpl_ptr;

} // namespace

#endif