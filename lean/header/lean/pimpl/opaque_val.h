/*****************************************************/
/* lean PImpl                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PIMPL_OPAQUE_VAL
#define LEAN_PIMPL_OPAQUE_VAL

#include "../meta/conditional_type.h"
#include "../meta/complete_type_or_base.h"
#include "../meta/dereference.h"

namespace lean
{
namespace pimpl
{

/// Opaque value class that stores values of the type wrapped by
/// OpaqueTypeWrapperBase, providing access to these values via
/// the type wrapped by OpaqueTypeWrapper, provided OpaqueTypeWrapper
/// is both fully defined and derived from OpaqueTypeWrapperBase.
template <class OpaqueTypeWrapper, class OpaqueTypeWrapperBase>
class opaque_val
{
private:
	typename OpaqueTypeWrapperBase::type m_value;

	/// Asserts that this wrapper is not in an opaque state when values are assigned.
	template <class PassThroughType, bool IsOpaque>
	struct assignment_check
	{
#ifdef CPP0X_STATIC_ASSERT
		static_assert(!IsOpaque, "cannot assign value in opaque state");
#endif
		typedef PassThroughType type;
	};

#ifndef CPP0X_STATIC_ASSERT
	struct cannot_assign_value_in_opaque_state;

	template <class PassThroughType>
	struct assignment_check<PassThroughType, true>
	{
		typedef char error[sizeof(cannot_assign_value_in_opaque_state)];

		typedef PassThroughType type;
	};
#endif

public:
	/// Opaque type of the value concealed by this wrapper.
	typedef typename OpaqueTypeWrapperBase::type opaque_type;
	/// Actual type of the value concealed by this wrapper, if fully defined.
	typedef typename typename complete_type_or_base<OpaqueTypeWrapper, OpaqueTypeWrapperBase>::type::type actual_type;
	/// Dereferenced type of the value concealed by this wrapper.
	typedef typename maybe_dereference_once<actual_type>::value_type dereferenced_type;
	/// Dereferenced return type of the value concealed by this wrapper.
	typedef typename conditional_type<
		maybe_dereference_once<actual_type>::dereferenced,
		dereferenced_type&, dereferenced_type>::type dereferenced_return_type;
	/// True if this wrapper currently is in an opaque state, false otherwise.
	static const bool is_opaque = !complete_type_or_base<OpaqueTypeWrapper, OpaqueTypeWrapperBase>::is_complete;

	/// Constructs an opaque value object from the given (or default) value.
	opaque_val(const actual_type &value = actual_type())
		: m_value(value)
	{
		typedef assignment_check<void, is_opaque>::type assertion_type;
	}
#ifdef CPP0X_MOVE_SEMANTICS
	/// Constructs an opaque value object from the given value.
	opaque_val(opaque_val &&right) : m_value(std::move(right.m_value)) { }
#endif

	/// Replaces the stored value with the given new value.
	opaque_val& operator =(const actual_type &value)
	{
		typedef assignment_check<void, is_opaque>::type assertion_type;

		m_value = value;
		return *this;
	}
#ifdef CPP0X_MOVE_SEMANTICS
	/// Replaces the stored value with the given new value.
	opaque_val& operator =(opaque_val &&right)
	{
		m_value = std::move(right.m_value);
		return *this;
	}
#endif

	/// Gets the value concealed by this opaque wrapper.
	actual_type get(void) const { return static_cast<actual_type>(m_value); }
	
	/// Gets the value concealed by this opaque wrapper.
	dereferenced_return_type operator *() const { return maybe_dereference_once<actual_type>::dereference(get()); }
	/// Gets the value concealed by this opaque wrapper.
	actual_type operator ->() const { return get(); }

	/// Gets the value concealed by this opaque wrapper.
	operator actual_type() const { return get(); }
};

} // namespace

using pimpl::opaque_val;

} // namespace

/// Declares an opaque type of the given name, setting its internal opaque
/// value type to the given opaque type name.
#define DECLARE_OPAQUE_TYPE(NAME, OPAQUE_TYPE)	\
	struct NAME##_opaque_type_wrapper			\
	{											\
		typedef OPAQUE_TYPE type;				\
	};											\
												\
	struct NAME##_actual_type_wrapper;			\
												\
	typedef lean::opaque_val<					\
		NAME##_actual_type_wrapper,				\
		NAME##_opaque_type_wrapper> NAME;

/// Defines the previously declared opaque type of the given name, setting
/// its actual value type to the given actual type name and thus changing
/// its state from opaque to transparent.
#define DEFINE_OPAQUE_TYPE(NAME, ACTUAL_TYPE)								\
	struct NAME##_actual_type_wrapper : public NAME##_opaque_type_wrapper	\
	{																		\
		typedef ACTUAL_TYPE type;											\
	};

#endif