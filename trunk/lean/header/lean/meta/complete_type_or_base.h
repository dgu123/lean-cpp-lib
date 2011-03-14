/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_META_COMPLETE_TYPE_OR_BASE
#define LEAN_META_COMPLETE_TYPE_OR_BASE

#include "conditional_type.h"

namespace lean
{
namespace meta
{

/// Redefines FullType if complete (and derived from BaseType), BaseType otherwise.
template <class FullType, class BaseType>
struct complete_type_or_base
{
private:
	typedef char complete[1];
	typedef char incomplete[2];

	static complete& check_type(const BaseType*);
	static incomplete& check_type(const void*);

public:
	/// Specifies whether FullType is complete and was derived from BaseType.
	static const bool is_complete = (
		sizeof( check_type( static_cast<FullType*>(nullptr) ) )
		==
		sizeof(BaseType) );
	
	/// Full type if complete (and derived from BaseType), BaseType otherwise.
	typedef typename conditional_type<is_complete, FullType, BaseType>::type type;
};

} // namespace

using meta::complete_type_or_base;

} // namespace

#endif