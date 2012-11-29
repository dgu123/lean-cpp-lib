/*****************************************************/
/* lean Tags                    (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_TAGS_MOVE_PTR
#define LEAN_TAGS_MOVE_PTR

#include "../lean.h"

namespace lean
{
namespace tags
{

/// Pointer wrapper class that signals transferral ownership.
template <class Type>
class move_ptr
{
	Type* *const ptr;

public:
#ifndef LEAN0X_NO_NULLPTR
	/// Constructs a nullptr.
	LEAN_INLINE move_ptr(nullptr_t)
		: ptr(nullptr) { }
#else
	/// Constructs a nullptr.
	LEAN_INLINE move_ptr(int)
		: ptr(nullptr) { }
#endif
	/// Wraps the given pointer whose ownership is to be transferred.
	LEAN_INLINE explicit move_ptr(Type *&ptr)
		: ptr(&ptr) { }

	/// Detaches the stored pointer.
	LEAN_INLINE Type* transfer()
	{
		if (ptr)
		{
			Type *p = *ptr;
			*ptr = nullptr;
			return p;
		}
		else
			return nullptr;
	}

	/// Peeks at the stored pointer.
	LEAN_INLINE Type* peek() const
	{
		return (ptr) ? *ptr : nullptr;
	}
};

} // namespace

using tags::move_ptr;

} // namespace

#endif