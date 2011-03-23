/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_CLONEABLE_OBJ
#define LEAN_SMART_CLONEABLE_OBJ

#include "../cpp0x.h"
#include "../meta/conditional_type.h"

namespace lean
{
namespace smart
{

/// Cloneable object class that stores an automatic instance of the given cloneable type.
template <class Cloneable, bool PointerSemantics = false>
class cloneable_obj
{
private:
	Cloneable *m_cloneable;

	/// Acquires the given cloneable.
	static Cloneable* acquire(const Cloneable &cloneable)
	{
		return static_cast<Cloneable*>( cloneable.clone() );
	}

	/// Acquires the given cloneable.
	static Cloneable* acquire(const Cloneable *cloneable)
	{
		return (cloneable)
			? acquire(*cloneable)
			: nullptr;
	}

	/// Releases the given cloneable.
	static void release(const Cloneable *cloneable)
	{
		if (cloneable)
			cloneable->destroy();
	}

protected:
	/// Constructs a cloneable object from the given cloneable pointer.
	cloneable_obj(Cloneable *cloneable)
		: m_cloneable(cloneable) { };

public:
	/// Type of the cloneable value stored by this cloneable object.
	typedef Cloneable value_type;
	/// Const value_type for value semantics, value_type for pointer semantics.
	typedef typename conditional_type<PointerSemantics, value_type, const value_type>::type maybe_const_value_type;

	/// Constructs a cloneable object by cloning the given cloneable value.
	cloneable_obj(const value_type &cloneable)
		: m_cloneable( acquire(cloneable) ) { };
	/// Constructs a cloneable object by cloning the given cloneable object.
	cloneable_obj(const cloneable_obj &right)
		: m_cloneable( acquire(right.m_cloneable) ) { };
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
		release(m_cloneable);
	}

	/// Gets a null cloneable object that may only be copied from and assigned to.
	static LEAN_INLINE cloneable_obj null()
	{
		LEAN_STATIC_ASSERT_MSG_ALT(PointerSemantics,
			"Null objects only available for pointer semantics.",
			Null_objects_only_available_for_pointer_semantics);

		// Warning: this object is effectively "broken"
		return cloneable_obj(nullptr);
	}

	/// Replaces the stored cloneable value with a clone of the given cloneable value.
	cloneable_obj& operator =(const value_type &cloneable)
	{
		CloneableType *prevCloneable = m_cloneable;
		m_cloneable = acquire(cloneable);
		release(prevCloneable);
		
		return *this;
	}
	/// Replaces the stored cloneable value with a clone of the given cloneable object.
	cloneable_obj& operator =(const cloneable_obj &right)
	{
		if (m_cloneable != right.m_cloneable)
		{
			CloneableType *prevCloneable = m_cloneable;
			m_cloneable = acquire(right.m_cloneable);
			release(prevCloneable);
		}
		
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Replaces the stored cloneable value with the value stored by the given cloneable object.
	cloneable_obj& operator =(cloneable_obj &&right)
	{
		if (m_cloneable != right.m_cloneable)
		{
			CloneableType *prevCloneable = m_cloneable;
			
			m_cloneable = std::move(right.m_cloneable);
			// Warning: this "breaks" the other object
			right.m_cloneable = nullptr;

			release(prevCloneable);
		}
		return *this;
	}
#endif

	/// Gets the value stored by this cloneable object.
	value_type& get(void) { LEAN_ASSERT(m_cloneable); return *m_cloneable; };
	/// Gets the value stored by this cloneable object.
	maybe_const_value_type& get(void) const { LEAN_ASSERT(m_cloneable); return *m_cloneable; };
	/// Gets the value stored by this cloneable object.
	value_type* getptr(void) { return m_cloneable; };
	/// Gets the value stored by this cloneable object.
	maybe_const_value_type* getptr(void) const { return m_cloneable; };

	/// Gets the value stored by this cloneable object.
	value_type& operator *() { return get(); };
	/// Gets the value stored by this cloneable object.
	maybe_const_value_type& operator *() const { return get(); };
	/// Gets the value stored by this cloneable object.
	value_type* operator ->() { LEAN_ASSERT(m_cloneable); return m_cloneable; };
	/// Gets the value stored by this cloneable object.
	maybe_const_value_type* operator ->() const { LEAN_ASSERT(m_cloneable); return m_cloneable; };

	/// Gets the value stored by this cloneable object.
	operator value_type&() { return get(); };
	/// Gets the value stored by this cloneable object.
	operator maybe_const_value_type&() const { return get(); };
};

} // namespace

using smart::cloneable_obj;

} // namespace

#endif