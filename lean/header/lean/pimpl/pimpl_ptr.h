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

/// Smart pointer class allowing for secure storage of forward-declared pimpl classes.
template <class Implementation, class ImplementationBase = Implementation>
class pimpl_ptr : public noncopyable
{
private:
	ImplementationBase *m_impl;

	/// Deletes the implementation stored by this pimpl pointer.
	static void delete_impl(const ImplementationBase *impl) { delete_full_impl( impl, static_cast<const Implementation*>(nullptr) ); }
	/// Deletes the implementation stored by this pimpl pointer calling the implementation destructor.
	static void delete_full_impl(const ImplementationBase *impl, const ImplementationBase*) { delete static_cast<const Implementation*>(impl); }
	/// Deletes the implementation stored by this pimpl pointer calling the base destructor.
	static void delete_full_impl(const ImplementationBase *impl, const void*) { delete impl; }

public:
	/// Type of the implementation stored by this pimpl pointer.
	typedef Implementation implementation_type;
	/// Base type of the implementation stored by this pimpl pointer.
	typedef ImplementationBase base_type;

	/// Constructs a pimpl pointer from the given implementation.
	pimpl_ptr(Implementation *impl) : m_impl(impl) { }
	/// Destructs this pimpl pointer, deleting any implementation stored.
	~pimpl_ptr() { delete_impl(m_impl); }

	/// Deletes any implementation stored, storing the given new implementation in this pimpl pointer.
	pimpl_ptr& operator =(Implementation *impl)
	{
		if (impl != m_impl)
		{
			delete_impl(m_impl);
			m_impl = impl;
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
	Implementation& get(void) { return *getptr(); }
	/// Gets the implementation managed by this pimpl pointer.
	const Implementation& get(void) const { return *getptr(); }
	/// Gets the implementation managed by this pimpl pointer.
	Implementation* getptr(void) { return static_cast<Implementation*>(m_impl); }
	/// Gets the implementation managed by this pimpl pointer.
	const Implementation* getptr(void) const { return static_cast<const Implementation*>(m_impl); }

	/// Checks wehther this pimpl pointer is currently empty.
	bool empty(void) { return (m_impl == nullptr); }
	
	/// Gets the implementation managed by this pimpl pointer.
	Implementation& operator *() { return get(); }
	/// Gets the implementation managed by this pimpl pointer.
	const Implementation& operator *() const { return get(); }
	/// Gets the implementation managed by this pimpl pointer.
	Implementation* operator ->() { return &get(); }
	/// Gets the implementation managed by this pimpl pointer.
	const Implementation* operator ->() const { return &get(); }
};

} // namespace

using pimpl::pimpl_ptr;

} // namespace

#endif