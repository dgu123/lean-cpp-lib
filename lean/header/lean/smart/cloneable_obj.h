/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_CLONEABLE_OBJ
#define LEAN_SMART_CLONEABLE_OBJ

#include "../cpp0x.h"

namespace lean
{
namespace smart
{


/// Cloneable object class that stores an automatic instance of the given cloneable type.
template <class Cloneable>
class cloneable_obj
{
private:
	Cloneable *m_cloneable;

public:
	/// Type of the cloneable value stored by this cloneable object.
	typedef Cloneable value_type;

	/// Constructs a cloneable object by cloning the given cloneable value.
	cloneable_obj(const value_type &cloneable)
		: m_cloneable( static_cast<Cloneable*>(cloneable.clone()) ) { };
	/// Constructs a cloneable object by cloning the given cloneable object.
	cloneable_obj(const cloneable_obj &right)
		: m_cloneable( static_cast<Cloneable*>(right.m_cloneable->clone()) ) { };
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Constructs a cloneable object by cloning the given cloneable object.
	cloneable_obj(cloneable_obj &&right)
		: m_cloneable(std::move(right.m_cloneable))
	{
		// Warning: this "breaks" the other object
		right.m_cloneable = nullptr;
	}
#endif
	/// Destroys the cloneable object.
	~cloneable_obj() throw()
	{
		if (m_cloneable)
			m_cloneable->destroy();
	}

	/// Replaces the stored cloneable value with a clone of the given cloneable value.
	cloneable_obj& operator =(const value_type &cloneable)
	{
		CloneableType *prevCloneable = m_cloneable;
		
		m_cloneable = static_cast<Cloneable*>(cloneable.clone());
		
		if (prevCloneable)
			prevCloneable->destroy();
		
		return *this;
	}
	/// Replaces the stored cloneable value with a clone of the given cloneable object.
	cloneable_obj& operator =(const cloneable_obj &right)
	{
		if (&right != this)
			*this = *right.m_cloneable;
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Replaces the stored cloneable value with the value stored by the given cloneable object.
	cloneable_obj& operator =(cloneable_obj &&right)
	{
		if (&right != this)
		{
			m_cloneable = std::move(right.m_cloneable);

			// Warning: this "breaks" the other object
			right.m_cloneable = nullptr;
		}
		return *this;
	}
#endif

	/// Gets the value stored by this cloneable object.
	value_type& get(void) { return *m_cloneable; };
	/// Gets the value stored by this cloneable object.
	const value_type& get(void) const { return *m_cloneable; };
	/// Gets the value stored by this cloneable object.
	value_type* getptr(void) { return m_cloneable; };
	/// Gets the value stored by this cloneable object.
	const value_type* getptr(void) const { return m_cloneable; };

	/// Gets the value stored by this cloneable object.
	value_type& operator *() { return get(); };
	/// Gets the value stored by this cloneable object.
	const value_type& operator *() const { return get(); };
	/// Gets the value stored by this cloneable object.
	value_type* operator ->() { return getptr(); };
	/// Gets the value stored by this cloneable object.
	const value_type* operator ->() const { return getptr(); };

	/// Gets the value stored by this cloneable object.
	operator value_type&() { return get(); };
	/// Gets the value stored by this cloneable object.
	operator const value_type&() const { return get(); };
};

} // namespace

using smart::cloneable_obj;

} // namespace

#endif