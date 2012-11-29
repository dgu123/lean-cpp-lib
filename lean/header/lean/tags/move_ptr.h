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
	Type *ptr;

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
	LEAN_INLINE explicit move_ptr(Type *ptr = nullptr)
		: ptr(ptr) { }

	/// Detaches the stored pointer.
	LEAN_INLINE Type* transfer()
	{
		Type *p = ptr;
		ptr = nullptr;
		return p;
	}

	/// Peeks at the stored pointer.
	LEAN_INLINE Type* peek() const
	{
		return ptr;
	}
};

/// Wraps the given reference to a pointer in a move pointer reference.
template <class Type>
LEAN_INLINE move_ptr<Type>& make_move_ptr(Type *&ptr) { return reinterpret_cast<move_ptr<Type>&>(ptr); }

/// Wraps the given pointer in a move pointer.
template <class Type>
LEAN_INLINE move_ptr<Type> ptr_move(Type *ptr) { return move_ptr<Type>(ptr); }
#ifndef LEAN0X_NO_NULLPTR
nullptr_t ptr_move(nullptr_t) { return nullptr; }
#else
int ptr_move(int) { return 0; }
#endif

} // namespace

using tags::move_ptr;

using tags::ptr_move;

} // namespace

#endif