/*****************************************************/
/* lean Properties              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PROPERTIES_PROPERTY
#define LEAN_PROPERTIES_PROPERTY

#include <string>
#include <typeinfo>
#include "../smart/cloneable.h"
#include "../smart/cloneable_obj.h"

namespace lean
{
namespace properties
{

class property_setter : public cloneable
{
	virtual void operator ()(const void *values, size_t count) const = 0;
};

class property_getter : public cloneable
{
	virtual void operator ()(void *values, size_t count) const = 0;
};

/// Describes a property.
struct property_desc
{
	std::wstring name;						///< Property name.
	const std::type_info *type;				///< Element type.
	size_t count;							///< Number of elements.
	cloneable_obj<property_setter> setter;	///< Value setter.
	cloneable_obj<property_getter> getter;	///< Value getter.
};

template <class Class, class _SetTy, class _GetTy = _SetTy, class _SetValTy = _SetTy, class _GetValTy = _GetTy>
class CPropertyBinder4 : public PropertyBinderBase<_SetValTy, _GetValTy>, public CPropertyBinding
{
private:
	typedef PropertyBinderBase<_SetValTy, _GetValTy> base_type;
	typedef typename base_type::setter_type setter_type;
	typedef typename base_type::getter_type getter_type;

	_Ty *m_pThis;
	void (_Ty::*m_pSetter)(_SetTy, _SetTy, _SetTy, _SetTy);
	void (_Ty::*m_pGetter)(_GetTy&, _GetTy&, _GetTy&, _GetTy&) const;


template <class Class, class _SetTy, class _GetTy = _SetTy, class _SetValTy = _SetTy, class _GetValTy = _GetTy>
class CPropertyBinder4 : public PropertyBinderBase<_SetValTy, _GetValTy>, public CPropertyBinding
{
private:
	typedef PropertyBinderBase<_SetValTy, _GetValTy> base_type;
	typedef typename base_type::setter_type setter_type;
	typedef typename base_type::getter_type getter_type;

	_Ty *m_pThis;
	void (_Ty::*m_pSetter)(_SetTy, _SetTy, _SetTy, _SetTy);
	void (_Ty::*m_pGetter)(_GetTy&, _GetTy&, _GetTy&, _GetTy&) const;

public:
	/// Constructor. Binds a setter and a getter to a property.
	CPropertyBinder4(_Ty *pThis, void (_Ty::*pSetter)(_SetTy, _SetTy, _SetTy, _SetTy), void (_Ty::*pGetter)(_GetTy&, _GetTy&, _GetTy&, _GetTy&) const)
		: m_pThis(pThis), m_pSetter(pSetter), m_pGetter(pGetter) { };

	/// Sets property value
	void Set(const void *pValues)
	{
		if(m_pSetter)
		{
			const setter_type* pTypedValues = reinterpret_cast<const setter_type*>(pValues);
			(m_pThis->*m_pSetter)(pTypedValues[0], pTypedValues[1], pTypedValues[2], pTypedValues[3]);
		}
	}
	/// Gets property value
	void Get(void *pValues) const
	{
		if(m_pGetter)
		{
			getter_type* pTypedValues = reinterpret_cast<getter_type*>(pValues);
			(m_pThis->*m_pGetter)(pTypedValues[0], pTypedValues[1], pTypedValues[2], pTypedValues[3]);
		}
	}

	/// Gets whether this property is read-only.
	bool IsReadOnly() const { return !m_pSetter; };
};

/// Bind setter / getter to a new property.
template <class _Ty, class _SetTy, class _GetTy>
inline unsigned short make_property_setter(
	void (_Ty::*pSetter)(_SetTy),
	_GetTy (_Ty::*pGetter)(void) const)
{
	// Create property binding & bind
	typedef CPropertyBinder<_Ty, _SetTy, _GetTy> deduced_property_binder;
	return BindProperty( sName, desc, be_new( deduced_property_binder(static_cast<_Ty*>(this), pSetter, pGetter) ) );
}
/// Bind setter / getter to a new property.
template <class _Ty, class _SetTy, class _GetTy>
inline unsigned short CreateProperty(const ::beCore::String::xstring& sName, const PropertyDescription& desc,
	void (_Ty::*pSetter)(_SetTy*), void (_Ty::*pGetter)(_GetTy*) const)
{
	// Create property binding & bind
	typedef CPropertyBinderPtr<_Ty, _SetTy, _GetTy> deduced_property_binder;
	return BindProperty( sName, desc, be_new( deduced_property_binder(static_cast<_Ty*>(this), pSetter, pGetter) ) );
}
/// Bind setter / getter to a new property.
template <class _Ty, class _SetTy, class _GetTy>
inline unsigned short CreateProperty(const ::beCore::String::xstring& sName, const PropertyDescription& desc,
	void (_Ty::*pSetter)(_SetTy, _SetTy, _SetTy), void (_Ty::*pGetter)(_GetTy&, _GetTy&, _GetTy&) const)
{
	// Create property binding & bind
	typedef CPropertyBinder3<_Ty, _SetTy, _GetTy> deduced_property_binder;
	return BindProperty( sName, desc, be_new( deduced_property_binder(static_cast<_Ty*>(this), pSetter, pGetter) ) );
}
/// Bind setter / getter to a new property.
template <class _Ty, class _SetTy, class _GetTy>
inline unsigned short CreateProperty(const ::beCore::String::xstring& sName, const PropertyDescription& desc,
	void (_Ty::*pSetter)(_SetTy, _SetTy, _SetTy, _SetTy), void (_Ty::*pGetter)(_GetTy&, _GetTy&, _GetTy&, _GetTy&) const)
{
	// Create property binding & bind
	typedef CPropertyBinder4<_Ty, _SetTy, _GetTy> deduced_property_binder;
	return BindProperty( sName, desc, be_new( deduced_property_binder(static_cast<_Ty*>(this), pSetter, pGetter) ) );
}

} // namespace

using properties::property_accessor;

} // namespace

#endif