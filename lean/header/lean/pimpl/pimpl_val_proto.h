/*****************************************************/
/* lean PImpl                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PIMPL_PVAL_PROTOTYPE
#define LEAN_PIMPL_PVAL_PROTOTYPE

namespace lean
{
namespace pimpl
{

/// PImpl single value class.
template <class Implementation>
class pimpl_val
{
private:
	Implementation m_impl;

public:
	/// Type of the implementation value stored by this pimpl value.
	typedef Implementation implementation_type;

	/// Constructs a pimpl value from the default implementation value.
	pimpl_val() { }
	/// Constructs a pimpl value from the given implementation value.
	pimpl_val(const Implementation &impl) : m_impl(impl) { }
#ifdef CPP0X_MOVE_SEMANTICS
	/// Constructs a pimpl value from the given implementation value.
	pimpl_val(Implementation &&impl) : m_impl(std::move(impl)) { }
	/// Constructs a pimpl value from the given implementation value.
	pimpl_val(pimpl_val &&right) : m_impl(std::move(right.m_impl)) { }
#endif

	/// Replaces the stored implementation value with the given new implementation value.
	pimpl_val& operator =(const Implementation &impl)
	{
		m_impl = impl;
		return *this;
	}
#ifdef CPP0X_MOVE_SEMANTICS
	/// Replaces the stored implementation value with the given new implementation value.
	pimpl_val& operator =(Implementation &&impl)
	{
		m_impl = std::move(impl);
		return *this;
	}
	/// Replaces the stored implementation value with the given new implementation value.
	pimpl_val& operator =(pimpl_val &&right)
	{
		m_impl = std::move(right.m_impl);
		return *this;
	}
#endif

	/// Gets the implementation value managed by this pimpl value.
	Implementation& get(void) { return m_impl; }
	/// Gets the implementation value managed by this pimpl value.
	const Implementation& get(void) const { return m_impl; }
	
	/// Gets the implementation value managed by this pimpl value.
	Implementation& operator *() { return get(); }
	/// Gets the implementation value managed by this pimpl value.
	const Implementation& operator *() const { return get(); }
	/// Gets the implementation value managed by this pimpl value
	Implementation* operator ->() { return &get(); }
	/// Gets the implementation value managed by this pimpl value.
	const Implementation* operator ->() const { return &get(); }

	/// Gets the implementation value managed by this pimpl value.
	operator Implementation&() { return get(); }
	/// Gets the implementation value managed by this pimpl value.
	operator const Implementation&() const { return get(); }
};

} // namespace

using pimpl::pimpl_val;

} // namespace

//#define PIMPL_VALUE_TYPE(name, )

#endif